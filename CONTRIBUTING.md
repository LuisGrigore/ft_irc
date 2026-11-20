# Contributing Guide

Practical guide for joining the project. The project is written in **C++98** and uses CMake, CTest, CppUTest, Valgrind and GitHub Actions (see `AgentGuide.md`).

All work is organized according to the [Implementation Plan](docs/implementation_plan.md), which defines features (FEAT-…), tasks (T-…), and acceptance criteria (AC-…) with traceability IDs.

## Getting started
j
Install dependencies and configure the environment:

```bash
./scripts/setup.sh
```

This installs CMake, Make, GCC/Clang, Git, Valgrind and GitHub CLI (gh), and activates the commit message template (`.gitmessage`) via `git config commit.template .gitmessage`.

## Work organization: Features, Tasks and acceptance criteria

All work is organized into **issues** with **traceability IDs** so they can be referenced from commits, branches and pull requests. The full traceability plan is documented in [docs/implementation_plan.md](docs/implementation_plan.md).

### Issue types

| Issue type | Example ID | Branch | Description |
|---|---|---|---|
| Feature | `FEAT-001` | `feature/…` | Top-level unit of work. Has acceptance criteria and can be broken down into tasks. |
| Task | `T-001` | `feature/…` (of its parent feature) | Smaller unit of work. It is a **sub-issue** of a parent feature. |
| Hotfix | `HOTFIX-001` | `hotfix/…` | One-off fix for a critical bug. |
| Devops | `DEVOPS-001` | `devops/…` | Infrastructure, CI, scripts, documentation or repository configuration. |
| Chore | `CHORE-001` | `chore/…` | Maintenance tasks (formatting, dependencies). |

### Traceability IDs

- Feature: `FEAT-NNN`
- Task: `T-NNN`
- Hotfix: `HOTFIX-NNN`
- Devops: `DEVOPS-NNN`
- Chore: `CHORE-NNN`

**Acceptance criteria** also carry their own traceability ID, e.g. `AC-001`, `AC-002`. A feature is considered complete only when all its acceptance criteria (and those of its tasks) are fulfilled and validated.

## Branch types

| Prefix | Purpose | Source | PR target |
|---|---|---|---|
| `feature/` | Implement a feature | `develop` | `develop` |
| `hotfix/` | One-off bug fix | `develop` | `develop` |
| `devops/` | Infrastructure, CI, documentation, repository | `develop` | `develop` |
| `chore/` | Maintenance tasks | `develop` | `develop` |
| `release/` | Synchronize `develop` into `main` | `develop` | `main` |

**Note:** There is no release pipeline. A `release` PR only synchronizes `main` with `develop`; after the merge, create a version tag on `main`.

## Create a branch

Always start from an up-to-date `develop`:

```bash
git checkout develop
git pull origin develop
git checkout -b feature/short-description    # or hotfix/…, devops/…, chore/…
```

When finished:

```bash
git add -A
git commit -m "type: short description (FEAT-001)"
git push origin feature/short-description
```

Then open the corresponding pull request (see next section).

## Script helper usage

### scripts/new-work.sh

Create a branch in an assisted way:

```bash
./scripts/new-work.sh [type] [issue-number] [descriptive-name]
```

All arguments are optional and can be given in any order. Supported types: `feature`, `hotfix`, `devops`, `chore`, `release`.

The script first asks whether the work corresponds to a GitHub issue:
- **If yes:** it asks for the issue number, fetches its title from GitHub, and uses it to build the branch name.
- **If no:** it interactively asks for the type, a descriptive name, and, if there are uncommitted changes, the commit message.

Examples:

```bash
# Fully interactive
./scripts/new-work.sh

# Type + issue number (name is derived from the issue title)
./scripts/new-work.sh feature 42

# Type + issue + custom name
./scripts/new-work.sh devops 5 update-docs
```

**Note:** This script does NOT create a Pull Request. It only creates the branch, commits changes, and pushes to origin. To open the PR, use `scripts/sync-develop.sh`.

### scripts/sync-develop.sh

Push changes from a feature/hotfix/devops/chore branch to `develop`. It updates the changelog, pushes the branch, and creates a Pull Request towards `develop`:

```bash
./scripts/sync-develop.sh [type] [descriptive-name]
```

After the PR is created, review it, wait for CI checks to pass, and merge manually on GitHub.

### scripts/sync-main.sh

Synchronize `develop` into `main` via a release PR. It updates the changelog before creating the PR:

```bash
./scripts/sync-main.sh [descriptive-name]
```

After merge, remember to create a version tag on `main` (e.g. `v1.0.0`).

### scripts/generate-changelog.sh

Automatically generate and update `CHANGELOG.md` from commit history:

```bash
./scripts/generate-changelog.sh [mode] [output-file]
```

Modes: `release` (from last tag to HEAD), `sync` (from develop to main), `all` (full history).

### scripts/commit.sh

Interactive commit helper that builds a commit message following the project template:

```bash
<type>: <short description> [(<TRACEABILITY_ID>)] [Closes #<ISSUE_NUMBER>]
```

```bash
./scripts/commit.sh        # ask for each field and create the commit
./scripts/commit.sh -n     # dry run: print the message instead of committing
./scripts/commit.sh -h     # show help
```

The template enforces:
- `<type>`: `feature`, `hotfix`, `devops`, `chore` or `release`
- `<short description>`: imperative mood, lowercase, no period at the end
- `<TRACEABILITY_ID>`: `FEAT-NNN`, `T-NNN`, `HOTFIX-NNN`, `DEVOPS-NNN` or `AC-NNN` (optional)
- `<ISSUE_NUMBER>`: GitHub issue number closed by this commit (optional)

Examples:

```
feature: implement Client getters and setters (FEAT-001) Closes #18
devops: add commit message template (DEVOPS-159) Closes #187
```

### scripts/generate_plan_md.py

Generate the implementation plan Markdown from its YAML source:

```bash
python3 scripts/generate_plan_md.py
```

### scripts/create_github_issues.py

Create or update GitHub issues (features and their tasks as sub-issues) from the plan YAML:

```bash
python3 scripts/create_github_issues.py           # create/update everything
python3 scripts/create_github_issues.py --dry-run # preview, no gh calls
python3 scripts/create_github_issues.py --only FEAT-001 FEAT-002
```

Requires `gh` (authenticated) and PyYAML.

## Pull requests

Each PR must indicate its type in the pull request template:

- **`feature`**: implements a new feature.
- **`hotfix`**: one-off bug fix.
- **`devops`**: infrastructure, CI, scripts, documentation or configuration.
- **`chore`**: maintenance tasks.
- **`release`**: synchronizes `develop` into `main`.

### PRs of type feature / hotfix / devops / chore

1. Source branch is `feature/…`, `hotfix/…`, `devops/…` or `chore/…`.
2. Target branch is `develop`.
3. Indicate the traceability ID of the issue closed by this PR (e.g. `Closes #123 — FEAT-001`).
4. List fulfilled acceptance criteria (`AC-001`, `AC-002`, …).
5. After review and CI passing, merge into `develop`.
6. **Delete the branch** after merge.
7. **Close the corresponding issue** (feature/task/hotfix) when all its acceptance criteria are completed.

### PRs of type release

1. Source branch is `release/…`.
2. Target branch is `main`.
3. No release pipeline: the PR only synchronizes `main` with `develop`.
4. After merge on `main`, create a version tag (e.g. `v1.0.0`).

## Compile the project

```bash
make build
```

The default build is in `build/` in `Debug` mode.

## Add unit tests

1. Create a file in `tests/unit/` (e.g. `tests/unit/test_foo.cpp`).
2. Use CppUTest (`#include "CppUTest/TestHarness.h"`, macros `TEST_GROUP` and `TEST`).
3. Add the file to the `unit_tests` executable in `tests/CMakeLists.txt`.
4. Run `make test-unit` or `ctest --test-dir build -L unit --output-on-failure`.

Requirements: no external dependencies, deterministic, no filesystem or network operations unless essential, one behavior per test.

## Add integration tests

1. Create a file in `tests/integration/`.
2. Register the executable in `tests/CMakeLists.txt` and add a test with `add_test(...)` and the `integration` label.
3. Run `make test-integration` or `ctest --test-dir build -L integration --output-on-failure`.

Requirements: independent, cleans up temporary resources, does not depend on execution order, does not modify production data.

## Run all checks

```bash
make build
make test
make valgrind
```

For sanitizers (AddressSanitizer + UndefinedBehaviorSanitizer):

```bash
make sanitize
```

## Basic code conventions

- Strict C++98: do not use `unique_ptr`, `shared_ptr`, lambdas, `auto`, `nullptr` or C++11 or later features.
- Clear names, small functions and simple responsibilities.
- RAII and explicit ownership; avoid unnecessary dynamic allocation and raw owning pointers.
- Compiler warnings active: `-Wall -Wextra -Werror -Wpedantic -std=c++98`.
- Consistent formatting (.clang-format).
- Use traceability IDs (`FEAT-…`, `T-…`, `HOTFIX-…`, `DEVOPS-…`, `AC-…`) in commit messages and PR descriptions.

## Pull request checklist

- Describe the change and why it is necessary.
- Indicate the PR type (`feature`, `hotfix`, `devops`, `chore` or `release`).
- Add or update tests covering the new behavior.
- List fulfilled acceptance criteria.
- Verify `make build`, `make test` and `make valgrind` locally.
- Make sure CI passes before requesting review.
