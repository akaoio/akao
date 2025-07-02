# Akao Philosophy

> Akao is not only a framework. It is a set of codified doctrines — enforced by rule, measured by trace, validated by code.

---

## 🔍 Core Beliefs

| Principle                                  | Description                                                                     |
| ------------------------------------------ | ------------------------------------------------------------------------------- |
| **Structure is enforced**                  | Every file, class, and folder must follow rule-driven structure.                |
| **Self-validation**                        | Akao must validate itself with no exceptions.                                   |
| **No rules without tests**                 | Each rule must have measurable test coverage.                                   |
| **Every rule is traceable**                | Violations include stack trace, file, line, and suggestion.                     |
| **Documentation is code**                  | All docs (README, RULES, PRINCIPLES) must be generated from actual rules/tests. |
| **One class per folder**                   | Every folder must contain exactly one class and its tests.                      |
| **One language per scope**                 | Multi-language repos must isolate languages clearly.                            |
| **CLI = Web = TUI**                        | All tooling must behave identically across UI modes.                            |
| **Every action is measurable**             | Coverage, logs, hash, audit state are enforced per commit.                      |
| **Rules can be toggled, not skipped**      | All disabled rules are tracked.                                                 |
| **Auto-update all doc**                    | Whenever rules change, documentation must change.                               |
| **Everything must build dev + prod**       | Dev (live reload) and prod (deterministic) builds are mandatory.                |
| **No external implicit behavior**          | Every dependency, path, and output must be declared.                            |
| **Only one truth**                         | All configuration must converge to a single source of truth.                    |
| **Observable, explainable, deterministic** | Nothing is magic; everything is reasoned, traceable, and intentional.           |

---

## 📐 Inspirations

While Akao is original in vision, its principles are strengthened by prior systems:

* Bazel (hermetic, deterministic build)
* Cargo (manifest-driven modules)
* Clean Architecture (layered logic separation)
* ESLint/Clippy (rule enforcement with testable logic)
* Git + GitHub Actions (traceable actions)
* Unix Philosophy (everything composable and testable)

But unlike them, Akao combines enforcement, testing, auditing, and documentation into a single **language-agnostic system** — with **C++ as its first-class core**.

---

## 🧠 Rules as Doctrine

All principles must:

1. Be implemented as a JSON rule.
2. Be backed by test file.
3. Be referenced in the CLI.
4. Be used to generate documentation.
5. Cause a CI failure if broken.

> Akao is the union of structure and behavior, measured in commits.

---

## ✅ Verification

Every principle here is:

* Part of `PRINCIPLES.md` (generated)
* Validated by a test in `tests/`
* Mapped to a rule in `rules/`
* Linked to violations in `.akao/trace.json`

Akao refuses to function in contradiction to its principles.

---

## 🧩 Usage

Whether a solo developer or an enterprise team:

* You may disable any rule — but not hide it.
* You may extend principles — but must test and document them.
* You must not rename the Akao core or claim its principles under another name.

Akao is strict not for ego, but for **precision, traceability, and reproducibility**.

---

## 🧬 Summary

* Rules = principles = code.
* Tests = measurement = enforcement.
* Docs = trace = explanation.

If any part is missing, Akao is broken.

> "Akao is not negotiable. Akao is enforced truth."
