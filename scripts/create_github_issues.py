#!/usr/bin/env python3
"""Create or update GitHub issues (features and their tasks as sub-issues) from the plan YAML.

The issue templates are NOT hardcoded: they are read from the repository's
issue-form files (default: .github/ISSUE_TEMPLATE/*.yml) and rendered the same way
GitHub renders a submitted form (title, labels, "### Label" sections, checkboxes).
`gh issue create` cannot fill YAML issue forms, so the script builds the body itself
and creates the issue through `gh api`.

Expected plan structure:

    features:
      - id: FEAT-001
        name: Feature title
        related_elements: [diagram.mmd]        # optional
        depends_on: [FEAT-00X]                  # optional
        acceptance_criteria:
          - {id: AC-001, description: "..."}
        tasks:
          - {id: T-001, description: "...", related_elements: [...]}   # last key optional

    Optional feature keys: `description` (or `detailed_description`), `complexity`
    (must match a dropdown option of the template: Low / Medium / High).
    Optional task key: `acceptance_criteria` (same shape as in features).

Template detection:
  * The feature template is the form whose `labels` contain "feature".
  * The task template is the form whose `labels` contain "task".
  * Override with --feature-template / --task-template.

Plan -> form field mapping:
  The only coupling with the templates is the field `id` values listed in
  FEATURE_FIELDS / TASK_FIELDS below. Everything else (title prefix, labels,
  section labels, checkbox texts, dropdown options, extra keys such as `assignees`
  or `type`) comes from the template files. Fields without a value in the plan
  (e.g. `complexity`) keep whatever a person typed into the existing issue, or are
  rendered as "_No response_" on creation, like GitHub does for empty fields.

Idempotency:
  Issues are matched by their title (`<template title prefix><ID>:`), so the ID is
  the identity of an issue. Re-running the script:
    * creates only the issues that do not exist yet;
    * updates the title/body of existing issues ONLY if the plan changed them
      (labels, assignees, and open/closed state are left alone; use --reopen to
      reopen closed issues);
    * links only the sub-issues that are not linked yet.
  Features are processed in dependency order, so every feature issue can mention
  the issue numbers (#N) of the features it depends on.

Usage:
    python3 create_github_issues.py --dry-run -v            # preview, no gh calls
    python3 create_github_issues.py                         # create/update everything
    python3 create_github_issues.py --only FEAT-001 FEAT-002
    python3 create_github_issues.py --repo OWNER/REPO

Requires: gh (authenticated, with sub-issues support) and PyYAML.
"""

import argparse
import glob
import itertools
import json
import os
import re
import shutil
import subprocess
import sys
import time

import yaml

PLACEHOLDER = re.compile(r"<[^>]*>")
NO_RESPONSE = "_No response_"
MISSING_TEXT = "(no description)"


# --------------------------------------------------------------------------- #
# Plan -> template field providers (keyed by the form field `id`)
# --------------------------------------------------------------------------- #
def render_elements(elements):
    return ", ".join("`%s`" % e if str(e).endswith(".mmd") else str(e) for e in elements)


def bullets(items):
    return "\n".join("- %s: %s" % (i["id"], (i.get("description") or "").strip()) for i in items)


def task_text(task):
    return (task.get("description") or "").strip() or MISSING_TEXT


def ref(ctx, ident):
    number = ctx["refs"].get(ident)
    return "%s (#%d)" % (ident, number) if number else ident


def feature_description(ctx):
    f = ctx["feature"]
    text = f.get("detailed_description") or f.get("description") or f["name"]
    parts = [text.strip()]
    elements = f.get("related_elements") or []
    if elements:
        parts.append("**Related elements:** %s" % render_elements(elements))
    return "\n\n".join(parts)


def feature_dependencies(ctx):
    """One dependency per line; the `#N` makes GitHub link (and mention) the issue."""
    return "\n".join("- %s" % ref(ctx, d) for d in ctx["feature"].get("depends_on") or [])


def feature_tasks(ctx):
    return "\n".join("- %s: %s" % (t["id"], task_text(t)) for t in ctx["feature"].get("tasks") or [])


def task_description(ctx):
    t = ctx["task"]
    parts = [task_text(t)]
    elements = t.get("related_elements") or []
    if elements:
        parts.append("**Related elements:** %s" % render_elements(elements))
    parts.append("**Parent feature:** %s" % ref(ctx, ctx["feature"]["id"]))
    return "\n\n".join(parts)


def task_acceptance_criteria(ctx):
    """Tasks carry no ACs in the plan (they hang from the feature).

    Use the task's own `acceptance_criteria` if the YAML ever provides them;
    otherwise point to the acceptance criteria of the parent feature.
    """
    t, f = ctx["task"], ctx["feature"]
    if t.get("acceptance_criteria"):
        return bullets(t["acceptance_criteria"])
    ids = ", ".join(ac["id"] for ac in f.get("acceptance_criteria") or [])
    return "- Contributes to the acceptance criteria of %s%s." % (
        ref(ctx, f["id"]), " (%s)" % ids if ids else "")


FEATURE_FIELDS = {
    "feat-id": lambda c: c["feature"]["id"],
    "description": feature_description,
    "dependencies": feature_dependencies,
    "acceptance-criteria": lambda c: bullets(c["feature"].get("acceptance_criteria") or []),
    "tasks": feature_tasks,
    "complexity": lambda c: c["feature"].get("complexity"),
}

TASK_FIELDS = {
    "task-id": lambda c: c["task"]["id"],
    "parent-feature": lambda c: c["feature"]["id"],
    "description": task_description,
    "acceptance-criteria": task_acceptance_criteria,
}


# --------------------------------------------------------------------------- #
# Plan validation and ordering
# --------------------------------------------------------------------------- #
def validate_plan(plan):
    errors, warnings = [], []
    features = plan.get("features") if isinstance(plan, dict) else None
    if not isinstance(features, list) or not features:
        return ["the plan has no `features` list"], warnings

    feature_ids, task_ids = set(), set()
    for f in features:
        fid = f.get("id")
        if not fid:
            errors.append("feature without `id`: %r" % f.get("name"))
            continue
        if fid in feature_ids:
            errors.append("duplicate feature id %s" % fid)
        feature_ids.add(fid)
        if not f.get("name"):
            errors.append("%s has no `name`" % fid)
        for ac in f.get("acceptance_criteria") or []:
            if not ac.get("id") or not ac.get("description"):
                errors.append("%s has an acceptance criterion without `id` or `description`" % fid)
        for t in f.get("tasks") or []:
            tid = t.get("id")
            if not tid:
                errors.append("%s has a task without `id`" % fid)
                continue
            if tid in task_ids or tid in feature_ids:
                errors.append("duplicate id %s" % tid)
            task_ids.add(tid)
            if not (t.get("description") or "").strip():
                warnings.append("task %s (%s) has no `description`; it will be titled %r"
                                % (tid, fid, MISSING_TEXT))

    for f in features:
        for dep in f.get("depends_on") or []:
            if dep not in feature_ids:
                errors.append("%s depends on unknown feature %s" % (f.get("id"), dep))
    return errors, warnings


def order_features(features):
    """Topological order (dependencies first), stable with respect to file order."""
    by_id = {f["id"]: f for f in features}
    ordered, state = [], {}

    def visit(fid, path):
        if state.get(fid) == "done":
            return
        if state.get(fid) == "visiting":
            raise ValueError("dependency cycle: %s" % " -> ".join(path + [fid]))
        state[fid] = "visiting"
        for dep in by_id[fid].get("depends_on") or []:
            visit(dep, path + [fid])
        state[fid] = "done"
        ordered.append(by_id[fid])

    for f in features:
        visit(f["id"], [])
    return ordered


# --------------------------------------------------------------------------- #
# Issue-form handling
# --------------------------------------------------------------------------- #
def load_form(path):
    with open(path, encoding="utf-8") as fh:
        form = yaml.safe_load(fh)
    if not isinstance(form, dict) or "body" not in form:
        raise ValueError("%s is not an issue form (no `body`)" % path)
    return form


def discover_templates(directory):
    found = {}
    for path in sorted(glob.glob(os.path.join(directory, "*.y*ml"))):
        try:
            form = load_form(path)
        except (ValueError, yaml.YAMLError):
            continue  # config.yml or invalid form
        labels = {str(l).lower() for l in (form.get("labels") or [])}
        for kind in ("feature", "task"):
            if kind in labels and kind not in found:
                found[kind] = (path, form)
    return found


def check_form(kind, path, form, providers):
    """Warn about mismatches between template fields and available providers."""
    ids = set()
    for field in form.get("body", []):
        if field.get("type") == "markdown":
            continue
        fid = field.get("id")
        ids.add(fid)
        required = bool((field.get("validations") or {}).get("required"))
        if field.get("type") != "checkboxes" and fid not in providers and required:
            sys.stderr.write("warning: %s template (%s): required field '%s' has no value source; "
                             "it will be '%s'\n" % (kind, path, fid, NO_RESPONSE))
    for fid in providers:
        if fid not in ids:
            sys.stderr.write("warning: %s template (%s) has no field '%s'; its value is ignored\n"
                             % (kind, path, fid))


def render_title(form, ident, text):
    template = form.get("title") or ""
    title_text = "%s: %s" % (ident, text)
    if PLACEHOLDER.search(template):
        return PLACEHOLDER.sub(lambda _m: title_text, template, count=1)
    return (template + " " + title_text).strip()


def title_pattern(form):
    head = (form.get("title") or "").split("<", 1)[0]
    return re.compile("^" + re.escape(head) + r"([A-Za-z]+-\d+):")


def parse_sections(body):
    """Parse a rendered form body into {label: content}."""
    parts = re.split(r"(?m)^### (.+?)\s*$", (body or "").replace("\r\n", "\n"))
    return {label.strip(): content.strip() for label, content in zip(parts[1::2], parts[2::2])}


def render_body(form, values, previous=None, check_boxes=True):
    """Render a form the way GitHub does when it is submitted.

    `previous` holds the sections of the existing issue body (if any). A field with no
    value in the plan keeps what is already in the issue (e.g. a dropdown filled by hand).
    """
    previous = previous or {}
    sections = []
    for field in form.get("body", []):
        ftype = field.get("type")
        if ftype == "markdown":
            continue
        attrs = field.get("attributes") or {}
        label = attrs.get("label") or field.get("id") or ""
        if ftype == "checkboxes":
            content = "\n".join("- [%s] %s" % ("x" if check_boxes else " ", o.get("label", ""))
                                for o in attrs.get("options") or [])
        else:
            value = values.get(field.get("id"))
            if value in (None, ""):
                kept = previous.get(label)
                content = kept if kept else NO_RESPONSE
            elif ftype == "dropdown":
                options = [str(o) for o in attrs.get("options") or []]
                if options and str(value) not in options:
                    sys.stderr.write("warning: value %r is not an option of dropdown '%s'\n"
                                     % (value, field.get("id")))
                content = str(value)
            elif ftype == "textarea" and attrs.get("render"):
                content = "```%s\n%s\n```" % (attrs["render"], value)
            else:
                content = str(value)
        sections.append("### %s\n\n%s" % (label, content))
    return "\n\n".join(sections)


def build_values(providers, ctx):
    return {fid: fn(ctx) for fid, fn in providers.items()}


def norm(text):
    return (text or "").replace("\r\n", "\n").strip()


# --------------------------------------------------------------------------- #
# gh wrapper
# --------------------------------------------------------------------------- #
class GhError(RuntimeError):
    pass


def gh(*args, input_data=None, retries=3):
    for attempt in range(retries):
        proc = subprocess.run(["gh"] + list(args), input=input_data,
                              capture_output=True, text=True)
        if proc.returncode == 0:
            return proc.stdout
        err = (proc.stderr or "") + (proc.stdout or "")
        if "rate limit" in err.lower() and attempt < retries - 1:
            wait = 60 * (attempt + 1)
            sys.stderr.write("rate limited; sleeping %ds\n" % wait)
            time.sleep(wait)
            continue
        raise GhError("gh %s failed: %s" % (" ".join(args[:3]), err.strip()))
    raise GhError("unreachable")


def resolve_repo(explicit):
    if explicit:
        return explicit
    return gh("repo", "view", "--json", "nameWithOwner", "-q", ".nameWithOwner").strip()


def fetch_existing_issues(repo):
    """All issues (not PRs), oldest first, one compact JSON object per line."""
    jq = (".[] | select(.pull_request | not)"
          " | {id, number, state, title, body, html_url} | tojson")
    out = gh("api", "--paginate",
             "repos/%s/issues?state=all&per_page=100&sort=created&direction=asc" % repo,
             "--jq", jq)
    return [json.loads(line) for line in out.splitlines() if line.strip()]


def update_issue(repo, number, payload):
    gh("api", "-X", "PATCH", "repos/%s/issues/%d" % (repo, number), "--input", "-",
       input_data=json.dumps(payload))


def ensure_labels(repo, labels):
    out = gh("api", "--paginate", "repos/%s/labels?per_page=100" % repo, "--jq", ".[].name")
    existing = {l.strip().lower() for l in out.splitlines()}
    for label in sorted(set(labels)):
        if label.lower() not in existing:
            gh("api", "-X", "POST", "repos/%s/labels" % repo, "-f", "name=%s" % label)
            print("created label '%s'" % label)


def create_issue(repo, form, title, body):
    payload = {"title": title, "body": body}
    for key in ("labels", "assignees", "type"):
        if form.get(key):
            payload[key] = form[key]
    out = gh("api", "-X", "POST", "repos/%s/issues" % repo, "--input", "-",
             input_data=json.dumps(payload))
    return json.loads(out)


def list_sub_issue_ids(repo, number):
    out = gh("api", "--paginate", "repos/%s/issues/%d/sub_issues?per_page=100" % (repo, number),
             "--jq", ".[].id")
    return {int(x) for x in out.split()}


def link_sub_issue(repo, parent_number, child_db_id):
    gh("api", "-X", "POST", "repos/%s/issues/%d/sub_issues" % (repo, parent_number),
       "--input", "-", input_data=json.dumps({"sub_issue_id": child_db_id}))


# --------------------------------------------------------------------------- #
# Main flow
# --------------------------------------------------------------------------- #
def main():
    ap = argparse.ArgumentParser(description="Create or update GitHub issues from the plan YAML.")
    ap.add_argument("-i", "--input", default="implementation_plan.yml")
    ap.add_argument("--templates-dir", default=".github/ISSUE_TEMPLATE")
    ap.add_argument("--feature-template", help="path to the feature issue form")
    ap.add_argument("--task-template", help="path to the task issue form")
    ap.add_argument("--repo", help="OWNER/REPO (default: current repository)")
    ap.add_argument("--only", nargs="+", metavar="FEAT-ID", help="only process these features")
    ap.add_argument("--delay", type=float, default=1.0, help="seconds between API writes")
    ap.add_argument("--reopen", action="store_true", help="reopen existing issues that are closed")
    ap.add_argument("--dry-run", action="store_true", help="do not call gh; print what would be done")
    ap.add_argument("-v", "--verbose", action="store_true", help="print rendered bodies in dry-run")
    args = ap.parse_args()

    with open(args.input, encoding="utf-8") as fh:
        plan = yaml.safe_load(fh)

    # Plan ----------------------------------------------------------------------
    errors, warnings = validate_plan(plan)
    for w in warnings:
        sys.stderr.write("warning: %s\n" % w)
    if errors:
        for e in errors:
            sys.stderr.write("error: %s\n" % e)
        return 1
    try:
        ordered = order_features(plan["features"])
    except ValueError as exc:
        sys.stderr.write("error: %s\n" % exc)
        return 1

    features = ordered
    if args.only:
        unknown = set(args.only) - {f["id"] for f in ordered}
        if unknown:
            sys.stderr.write("error: unknown feature(s): %s\n" % ", ".join(sorted(unknown)))
            return 1
        features = [f for f in ordered if f["id"] in args.only]

    # Templates -----------------------------------------------------------------
    found = discover_templates(args.templates_dir)
    templates = {}
    for kind, explicit in (("feature", args.feature_template), ("task", args.task_template)):
        if explicit:
            templates[kind] = (explicit, load_form(explicit))
        elif kind in found:
            templates[kind] = found[kind]
        else:
            sys.stderr.write("error: no %s template found in %s (use --%s-template)\n"
                             % (kind, args.templates_dir, kind))
            return 1
    f_path, f_form = templates["feature"]
    t_path, t_form = templates["task"]
    check_form("feature", f_path, f_form, FEATURE_FIELDS)
    check_form("task", t_path, t_form, TASK_FIELDS)

    # Repository state ----------------------------------------------------------
    existing = {"feature": {}, "task": {}}
    refs = {}      # plan ID -> issue number
    if args.dry_run:
        repo = args.repo or "OWNER/REPO"
    else:
        if not shutil.which("gh"):
            sys.stderr.write("error: gh CLI not found in PATH\n")
            return 1
        repo = resolve_repo(args.repo)
        pats = {"feature": title_pattern(f_form), "task": title_pattern(t_form)}
        for issue in fetch_existing_issues(repo):
            for kind, pat in pats.items():
                m = pat.match(issue["title"])
                if not m:
                    continue
                ident = m.group(1)
                if ident in existing[kind]:
                    sys.stderr.write("warning: duplicate issues for %s: #%d and #%d; using #%d\n"
                                     % (ident, existing[kind][ident]["number"], issue["number"],
                                        existing[kind][ident]["number"]))
                else:
                    existing[kind][ident] = issue  # oldest wins (list is ascending)
        # Known numbers up front, so dependencies resolve even with --only.
        for kind in existing:
            for ident, issue in existing[kind].items():
                refs[ident] = issue["number"]
        ensure_labels(repo, list(f_form.get("labels") or []) + list(t_form.get("labels") or []))
    counter = itertools.count(1)  # only consumed in dry-run mode
    print("repository: %s%s" % (repo, " (dry run)" if args.dry_run else ""), flush=True)

    stats = {"created": 0, "updated": 0, "unchanged": 0, "linked": 0}

    def get_or_create(kind, form, ident, text, ctx, providers):
        title = render_title(form, ident, text)
        values = build_values(providers, ctx)

        if ident in existing[kind]:
            issue = dict(existing[kind][ident])
            issue["created"] = False
            number = issue["number"]
            body = render_body(form, values, previous=parse_sections(issue.get("body")))
            payload = {}
            if norm(issue.get("title")) != norm(title):
                payload["title"] = title
            if norm(issue.get("body")) != norm(body):
                payload["body"] = body
            if args.reopen and issue.get("state") != "open":
                payload["state"] = "open"
            if payload:
                update_issue(repo, number, payload)
                stats["updated"] += 1
                print("[%s] updated (%s): #%d %s" % (ident, ", ".join(sorted(payload)), number, title),
                      flush=True)
                time.sleep(args.delay)
            else:
                stats["unchanged"] += 1
                print("[%s] up to date: #%d %s" % (ident, number, title), flush=True)
            return issue

        body = render_body(form, values)
        if args.dry_run:
            issue = {"id": None, "number": next(counter), "html_url": "(dry run)", "created": True}
            print("[%s] would create #%d: %s  labels=%s" % (ident, issue["number"], title,
                                                              form.get("labels") or []))
            if args.verbose:
                print("-" * 70 + "\n" + body + "\n" + "-" * 70)
        else:
            issue = create_issue(repo, form, title, body)
            issue["created"] = True
            existing[kind][ident] = issue
            print("[%s] created #%d: %s" % (ident, issue["number"], issue["html_url"]), flush=True)
            time.sleep(args.delay)
        stats["created"] += 1
        return issue

    # Features (dependencies first), then their tasks as sub-issues -------------
    for feature in features:
        fctx = {"feature": feature, "refs": refs}
        f_issue = get_or_create("feature", f_form, feature["id"], feature["name"], fctx, FEATURE_FIELDS)
        refs[feature["id"]] = f_issue["number"]

        linked = set()
        if not args.dry_run and not f_issue["created"]:
            linked = list_sub_issue_ids(repo, f_issue["number"])

        for task in feature.get("tasks") or []:
            tctx = {"feature": feature, "task": task, "refs": refs}
            t_issue = get_or_create("task", t_form, task["id"], task_text(task), tctx, TASK_FIELDS)
            refs[task["id"]] = t_issue["number"]
            if args.dry_run:
                print("    -> would link #%d as sub-issue of #%d" % (t_issue["number"], f_issue["number"]), flush=True)
                stats["linked"] += 1
            elif t_issue["id"] in linked:
                continue
            else:
                try:
                    link_sub_issue(repo, f_issue["number"], t_issue["id"])
                    stats["linked"] += 1
                    print("    -> #%d is now a sub-issue of #%d" % (t_issue["number"], f_issue["number"]), flush=True)
                    time.sleep(args.delay)
                except GhError as exc:
                    sys.stderr.write("warning: could not link #%d to #%d: %s\n"
                                     % (t_issue["number"], f_issue["number"], exc))

    print("\ndone: %(created)d created, %(updated)d updated, %(unchanged)d unchanged, "
          "%(linked)d sub-issue links" % stats, flush=True)
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except GhError as exc:
        sys.stderr.write("error: %s\n" % exc)
        sys.exit(1)