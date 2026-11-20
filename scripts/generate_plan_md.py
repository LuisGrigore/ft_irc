#!/usr/bin/env python3
"""Generate implementation-plan Markdown from its YAML source.

Usage:
    python3 generate_plan_md.py [-i implementation_plan_normalized.yaml]
                                [-o IMPLEMENTATION_PLAN.md]
    python3 generate_plan_md.py -o -  # write to stdout

Requires PyYAML (pip install pyyaml).

The current YAML format contains a top-level `document` mapping with `title`
and `intro`, plus a `features` list. Feature descriptions and dependency
rationales are not required. Optional document sections (sources, ID scheme,
constraints, and TDD methodology) are rendered when present.
"""

import argparse
import sys

import yaml

TDD_STEPS = ("Red", "Green", "Refactor")


# --------------------------------------------------------------------------- #
# Helpers
# --------------------------------------------------------------------------- #
def code(text):
    return "`%s`" % text


def cell(text):
    """Escape a value for use inside a Markdown table cell."""
    return str(text).replace("|", "\\|").replace("\n", " ")


def render_elements(elements):
    """`.mmd` references become inline code; other values stay plain text."""
    return ", ".join(
        code(e) if str(e).endswith(".mmd") else str(e)
        for e in (elements or [])
    ) or "None"


def span(ids):
    if not ids:
        return "none"
    return ids[0] if len(ids) == 1 else "%s to %s" % (ids[0], ids[-1])


def depends_text(feature):
    """Use an explicit label if supplied; otherwise derive it from depends_on."""
    label = feature.get("depends_on_label")
    if label:
        return label
    deps = feature.get("depends_on") or []
    return ", ".join(deps) if deps else "None"


def table(headers, rows):
    """Render a Markdown table with the compact style used by the plan."""
    out = ["| " + " | ".join(headers) + " |"]
    out.append("|" + "|".join("-" * (len(h) + 2) for h in headers) + "|")
    for row in rows:
        out.append("| " + " | ".join(cell(c) for c in row) + " |")
    return out


def id_table(headers, rows):
    """Render the ID-prefix table, if the YAML provides one."""
    width = max([len(headers[0])] + [len(str(r[0])) for r in rows])
    out = ["| %s | %s |" % (headers[0].ljust(width), headers[1])]
    out.append("|%s|%s|" % ("-" * (width + 2), "-" * (len(headers[1]) + 2)))
    for row in rows:
        out.append("| %s | %s |" % (str(row[0]).ljust(width), cell(row[1])))
    return out


# --------------------------------------------------------------------------- #
# Validation
# --------------------------------------------------------------------------- #
def validate(data):
    errors = []

    if not isinstance(data, dict):
        return ["YAML root must be a mapping"]

    if not isinstance(data.get("document"), dict):
        errors.append("missing or invalid top-level 'document' mapping")
    else:
        if not data["document"].get("title"):
            errors.append("document.title is required")
        if not data["document"].get("intro"):
            errors.append("document.intro is required")

    features = data.get("features")
    if not isinstance(features, list):
        errors.append("missing or invalid top-level 'features' list")
        return errors

    feature_ids = []
    for index, feature in enumerate(features):
        if not isinstance(feature, dict):
            errors.append("features[%d] must be a mapping" % index)
            continue
        if not feature.get("id"):
            errors.append("features[%d] is missing id" % index)
        else:
            feature_ids.append(feature["id"])

    known_features = set(feature_ids)

    # Duplicate IDs are deliberately not fatal: the source plan may contain
    # repeated AC identifiers, and this renderer must preserve/render them.
    for feature in features:
        if not isinstance(feature, dict):
            continue

        feature_id = feature.get("id", "<unknown feature>")
        for dep in feature.get("depends_on") or []:
            if dep not in known_features:
                errors.append("%s depends on unknown feature %s" % (feature_id, dep))

        for task in feature.get("tasks") or []:
            if not isinstance(task, dict):
                errors.append("%s contains a task that is not a mapping" % feature_id)
                continue
            if not task.get("id"):
                errors.append("%s contains a task missing id" % feature_id)
            # TDD step and related_elements are optional in the new format.
            elif task.get("tdd_step") is not None and task["tdd_step"] not in TDD_STEPS:
                errors.append(
                    "%s has invalid tdd_step %r"
                    % (task["id"], task.get("tdd_step"))
                )

        for criterion in feature.get("acceptance_criteria") or []:
            if not isinstance(criterion, dict):
                errors.append(
                    "%s contains an acceptance criterion that is not a mapping"
                    % feature_id
                )
            elif not criterion.get("id"):
                errors.append("%s contains an acceptance criterion missing id" % feature_id)

    return errors


# --------------------------------------------------------------------------- #
# Rendering
# --------------------------------------------------------------------------- #
def render(data):
    doc = data["document"]
    features = data["features"]

    all_tasks = [
        (feature, task)
        for feature in features
        for task in (feature.get("tasks") or [])
    ]
    all_acs = sorted(
        (
            (feature, criterion)
            for feature in features
            for criterion in (feature.get("acceptance_criteria") or [])
        ),
        key=lambda pair: pair[1]["id"],
    )

    out = []

    # Header ----------------------------------------------------------------
    out += ["# " + str(doc["title"]), "", str(doc["intro"]), ""]

    for source in doc.get("sources") or []:
        path, _, rest = str(source).partition(" ")
        out.append("- %s%s" % (code(path), (" " + rest) if rest else ""))

    if doc.get("sources"):
        out.append("")

    if doc.get("id_scheme_intro"):
        out += [str(doc["id_scheme_intro"]), ""]

    prefixes = doc.get("id_prefixes") or []
    if prefixes:
        out += id_table(
            ["Prefix", "Meaning"],
            [(item["prefix"], item["meaning"]) for item in prefixes],
        )
        out += [""]

    out += ["---", ""]

    # 1. Features table -----------------------------------------------------
    out += ["## 1. Features Table", ""]
    out += table(
        ["ID", "Name", "Related Elements"],
        [
            (
                feature["id"],
                feature["name"],
                render_elements(feature.get("related_elements")),
            )
            for feature in features
        ],
    )
    out += ["", "---", ""]

    # 2. Tasks table --------------------------------------------------------
    out += ["## 2. Tasks Table", ""]
    out += table(
        ["ID", "Description", "TDD Step", "Feature ID", "Related Elements"],
        [
            (
                task["id"],
                task.get("description", ""),
                task.get("tdd_step") or "—",
                feature["id"],
                render_elements(task.get("related_elements")),
            )
            for feature, task in all_tasks
        ],
    )
    out += ["", "---", ""]

    # 3. Acceptance criteria table -----------------------------------------
    out += ["## 3. Acceptance Criteria Table", ""]
    out += table(
        ["ID", "Description", "Associated Element"],
        [
            (
                criterion["id"],
                criterion.get("description", ""),
                feature["id"],
            )
            for feature, criterion in all_acs
        ],
    )
    out += ["", "---", ""]

    # Optional constraints section -----------------------------------------
    constraints = data.get("constraints")
    if constraints:
        out += ["## " + constraints.get("title", "Constraints"), ""]
        for constraint in constraints.get("items") or []:
            if "items" in constraint:
                body = ", ".join(code(item) for item in constraint["items"])
            else:
                body = constraint.get("text", "")
            out.append(
                "- **%s**%s%s"
                % (
                    constraint.get("label", ""),
                    constraint.get("separator", ": "),
                    body,
                )
            )
        out += ["", "---", ""]

    # Optional TDD methodology section -------------------------------------
    tdd = data.get("tdd_methodology")
    if tdd:
        out += [
            "## " + tdd.get("title", "TDD Methodology"),
            "",
            tdd.get("intro", ""),
            "",
        ]
        for index, step in enumerate(tdd.get("steps") or [], 1):
            out.append(
                "%d. **%s** — %s"
                % (index, step.get("name", ""), step.get("description", ""))
            )
        if tdd.get("ordering_note"):
            out += ["", tdd["ordering_note"]]
        out += ["", "---", ""]

    # 4. Detailed breakdown -------------------------------------------------
    out += ["## 4. Detailed Breakdown by Feature", ""]
    for feature in features:
        task_ids = [
            task["id"] for task in (feature.get("tasks") or [])
            if task.get("id")
        ]
        criterion_ids = sorted(
            criterion["id"]
            for criterion in (feature.get("acceptance_criteria") or [])
            if criterion.get("id")
        )

        out += [
            "### %s — %s" % (feature["id"], feature["name"]),
            "",
        ]

        # The normalized format has no detailed_description. Do not fabricate
        # one; show the feature's related elements and traceability instead.
        out.append(
            "**Related Elements:** %s  "
            % render_elements(feature.get("related_elements"))
        )
        out.append("**Tasks:** %s  " % span(task_ids))
        out.append("**Acceptance Criteria:** %s  " % span(criterion_ids))
        out.append("**Depends on:** %s" % depends_text(feature))
        out.append("")

    out += ["---", ""]

    # Dependency summary ----------------------------------------------------
    out += ["## Dependency Summary", ""]
    rows = []
    for feature in features:
        rows.append((feature["id"], depends_text(feature)))
    out += table(["Feature", "Depends On"], rows)

    return "\n".join(out) + "\n"


# --------------------------------------------------------------------------- #
# CLI
# --------------------------------------------------------------------------- #
def main():
    parser = argparse.ArgumentParser(
        description="Generate the implementation-plan Markdown from YAML."
    )
    parser.add_argument(
        "-i",
        "--input",
        default="implementation_plan_normalized.yaml",
        help="YAML source (default: implementation_plan_normalized.yaml)",
    )
    parser.add_argument(
        "-o",
        "--output",
        default="IMPLEMENTATION_PLAN.md",
        help="Markdown output path, or '-' for stdout (default: IMPLEMENTATION_PLAN.md)",
    )
    args = parser.parse_args()

    try:
        with open(args.input, encoding="utf-8") as handle:
            data = yaml.safe_load(handle)
    except OSError as exc:
        sys.stderr.write("error: cannot read %s: %s\n" % (args.input, exc))
        return 1
    except yaml.YAMLError as exc:
        sys.stderr.write("error: invalid YAML: %s\n" % exc)
        return 1

    errors = validate(data)
    if errors:
        for error in errors:
            sys.stderr.write("error: %s\n" % error)
        return 1

    markdown = render(data)
    if args.output == "-":
        sys.stdout.write(markdown)
    else:
        try:
            with open(args.output, "w", encoding="utf-8") as handle:
                handle.write(markdown)
        except OSError as exc:
            sys.stderr.write("error: cannot write %s: %s\n" % (args.output, exc))
            return 1
        sys.stderr.write("wrote %s\n" % args.output)

    return 0


if __name__ == "__main__":
    sys.exit(main())
