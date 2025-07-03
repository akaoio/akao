# Akao Philosophy

> Akao is not only a framework. It is a set of codified doctrines — enforced by rule, measured by trace, validated by code.

---

## 🔍 Core Beliefs

### **Structure is enforced**
Every file, class, and folder must follow rule-driven structure. This is not merely a suggestion or convention—it is the fundamental law that governs all code existence within Akao. Structure becomes the skeleton upon which all other principles rest. Without enforced structure, chaos emerges, maintainability dies, and the codebase becomes a digital wasteland. Akao treats structural violations as compilation errors, making disorder impossible rather than inconvenient.

### **Universal validation**
Any project can be validated using `akao validate`, including Akao itself. Validation is not special or recursive—it's a standard operation that applies consistent rules to any codebase. The framework validates structure, enforces principles, and reports violations using the same systematic approach whether applied to Akao or any other project. This ensures validation logic is robust, well-tested, and universally applicable.

### **No rules without tests**
Each rule must have measurable test coverage. Rules without tests are merely suggestions disguised as law. Every principle declared must be backed by executable proof that demonstrates both compliance and violation scenarios. This creates a feedback loop where rules are continuously validated against reality, ensuring they remain practical and enforceable rather than theoretical and aspirational.

### **Every rule is traceable**
Violations include Global Rule Identifier (GID), stack trace, file, line, and suggestion. When a rule is broken, the system must provide not just identification but education. Each violation becomes a teaching moment, complete with forensic evidence of exactly what went wrong and how to fix it. The GID system ensures that every rule has a globally unique identifier (format: `akao:rule::<category>:<name>:v<version>`) enabling precise tracking across audit logs, trace files, and compliance metrics. This transforms error messages from cryptic failures into guided learning experiences with full traceability.

### **Documentation is code**
All documentation (README, RULES, PRINCIPLES) must be generated from actual rules and tests. Hand-written documentation lies by omission and becomes stale by neglect. Generated documentation cannot lie because it is extracted from the living, breathing codebase itself. This ensures that what is documented is what is implemented, and what is implemented is what is tested.

### **One class per folder**
Every folder must contain exactly one class and its tests. This principle enforces cognitive simplicity and prevents the common antipattern of "god folders" that become dumping grounds for related but distinct concerns. Each folder becomes a bounded context with clear responsibility, making navigation intuitive and refactoring safe.

### **One language per scope**
Multi-language repositories must isolate languages clearly. While polyglot systems are inevitable in modern development, they must be organized with surgical precision. Each language exists within its own realm, with clear boundaries and explicit interfaces. This prevents the chaos of mixed-language files and ensures that language-specific tooling can operate effectively.

### **CLI = Web = TUI**
All tooling must behave identically across user interface modes. The interface is merely the skin; the underlying logic must be invariant. A command that works in CLI must provide identical functionality through web API and terminal UI. This ensures that users can switch interfaces without relearning workflows or encountering inconsistent behavior.

### **Every action is measurable**
Coverage, logs, hash, audit state are enforced per commit using Global Rule Identifiers (GIDs) for precise tracking. If an action cannot be measured, it effectively didn't happen. Every operation must leave traces that can be analyzed, audited, and verified through the GID system. Each rule compliance check, violation, and remediation is tracked by its unique GID, creating a complete paper trail of system behavior. This enables forensic analysis of failures, empirical optimization of performance, and detailed compliance reporting per rule category.

### **Rules can be toggled, not skipped**
All disabled rules are tracked by their Global Rule Identifier (GID) with reason and audit trail. When a rule is disabled, the system must record why, when, by whom, and the specific GID being disabled. This prevents rules from being quietly ignored and ensures that temporary exceptions are eventually addressed. Disabled rules remain visible in all reporting with their full GID path, creating pressure to either fix the violation or formally acknowledge the exception through proper governance.

### **Auto-update all documentation**
Whenever rules change, documentation must change automatically. Manual documentation updates are promises that will be broken. When code evolves, documentation must evolve in lockstep, without human intervention. This ensures that documentation reflects current reality rather than historical intentions.

### **Everything must build dev + prod**
Development (live reload) and production (deterministic) builds are mandatory. Every codebase must support both rapid iteration and reliable deployment. Development builds prioritize speed and debugging capability, while production builds prioritize optimization and reproducibility. Both must succeed for the system to be considered complete.

### **No external implicit behavior**
Every dependency, path, and output must be declared explicitly. Hidden dependencies are time bombs waiting to detonate in different environments. All external interactions must be made visible and manageable. This enables reliable builds, predictable deployments, and comprehensive security analysis.

### **Only one truth**
All configuration must converge to a single source of truth. Duplicate configuration is a breeding ground for inconsistency and confusion. Every configurable aspect of the system must have exactly one authoritative source, with all other references being derived or validated against that source.

### **Observable, explainable, deterministic**
Nothing is magic; everything is reasoned, traceable, and intentional. Every behavior must be explicable through logical chains of cause and effect. When something happens, the system must be able to explain why it happened and how to control it. This eliminates "works on my machine" problems and enables reliable debugging.

### **Features are composable**
Features must work together without conflicts or hidden dependencies. Each feature is a building block that can be combined with others to create more complex functionality. Features cannot make assumptions about the presence or absence of other features, and they must declare all their dependencies explicitly.

### **Templates are validated**
All project templates must pass the same rules as generated projects. Templates are not exempt from quality standards—they are held to even higher standards because they become the foundation for new projects. A template that generates non-compliant code is worse than no template at all.

### **External features are sandboxed**
Features from external sources run in controlled environments. Trust is not transitive—just because you trust Akao doesn't mean you should trust every feature that claims to work with Akao. External features operate within security boundaries that prevent them from compromising the host system or other features.

### **Dependency resolution is explicit**
All feature dependencies must be declared and resolved transparently. The system must be able to explain exactly why each dependency was included and how conflicts were resolved. This enables reproducible builds and prevents the emergence of phantom dependencies that work in development but fail in production.

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
