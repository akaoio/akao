# Akao Framework

> **A unified, rule-based framework written in C++ for structuring, building, testing, and documenting any software project.**

---

## 🚀 Overview

Akao is not a library — it is a **philosophy, architecture, and enforcement system** encoded in code. It uses declarative rules, written in JSON, YAML, or TOML, to validate structure, enforce best practices, automate builds, and generate documentation. Each rule has a Global Rule Identifier (GID) in the format `akao:rule::<category>:<name>:v<version>` for precise tracking and management. Akao is written in **C++**, but is capable of managing polyglot repos (e.g., JS, Python, Rust) as long as each language follows structure and rule profiles.

Akao applies its own standards to itself, demonstrating that its validation system works universally across any project.

---

## 🧱 Core Capabilities

✅ Universal validation system  
✅ Rule-driven validation system  
✅ Build engine for dev/prod (hot reload, optimized)  
✅ Auto-generated documentation and principle tracing  
✅ Automated CI/CD pipeline generation  
✅ Audit logs, coverage, enforcement trace  
✅ Unified CLI + Web UI + TUI  
✅ Plugin system for targets (wasm, js, android, etc)  
✅ Pure C++ implementation  

---

## 🧠 Philosophy Highlights

* Rules are the law. They govern everything.
* All behavior is deterministic and reproducible.
* Documentation is generated, never hand-written.
* Every test, validation, and build must be traceable.
* Structure is enforced, not suggested.
* Every class = 1 folder, every folder = 1 rule scope.
* All classes must be tested and documented.

See full: [`PHILOSOPHY.md`](./PHILOSOPHY.md)

---

## 🧩 Use Cases

* C++ projects with strict structure and validation
* Mixed-language repositories with rules per language
* Validating large-scale enterprise codebases
* Framework development with rigorous standards
* Building rule-compliant plugins, mobile apps, APIs

---

## 📂 Structure

```bash
/                   # Root project
├── .akao/          # Runtime, profiles, trace, hash
│   ├── features/   # Installed features and dependencies
│   └── registry/   # External feature registries
├── .build/         # Dev & prod builds
├── docs/           # Auto-generated documentation
├── rules/          # JSON or DSL rule files
├── src/            # C++ classes (1 class per folder)
├── include/        # C++ headers (mirrors src)
├── tests/          # Principle & rule tests
├── features/       # Project-specific features
├── templates/      # Project and feature templates
├── plugins/        # Optional targets or language bindings
```

---

## 🧪 Commands

```bash
# Project Management
akao init                     # Initialize new Akao project with templates
akao init --template=cpp      # Initialize with specific language template
akao init --template=web      # Initialize web project template

# Feature Management
akao feature add <name>       # Add new feature to project
akao feature remove <name>    # Remove feature from project
akao feature list             # List all features in project
akao feature info <name>      # Show feature details and dependencies

# External Feature Management
akao registry add <url>       # Add external feature registry
akao registry list            # List available registries
akao install <feature>        # Install feature from registry
akao update <feature>         # Update installed feature
akao uninstall <feature>      # Remove installed feature

# Core Operations
akao validate                 # Enforce structure and rules
akao validate --gid=<gid>     # Validate specific rule by Global Rule Identifier
akao validate --category=<cat> # Validate all rules in a category
akao test                     # Run unit & principle tests
akao build --dev              # Start build in hot-reload mode
akao build --prod             # Production build with hashes
akao docgen                   # Generate RULES.md, README.md, etc
akao audit                    # Print audit of rules, coverage, status
akao audit --gid=<gid>        # Audit specific rule compliance

# Rule Management (GID-based)
akao rule list                # List all rules with their categories
akao rule list --format=gid   # List all rules showing their GIDs
akao rule info --gid=<gid>    # Show detailed rule information by GID
akao rule disable --gid=<gid> --reason="<reason>" # Disable rule (tracked)
akao rule enable --gid=<gid>  # Re-enable previously disabled rule

# RuleSet Management
akao ruleset list             # List all available RuleSets
akao ruleset info <name>      # Show RuleSet details and inheritance
akao ruleset validate <name>  # Validate RuleSet definition and dependencies
akao ruleset create <name>    # Create new RuleSet interactively

# RuleSet-based Operations
akao validate --ruleset=<name> # Validate using specific RuleSet
akao audit --ruleset=<name>   # Audit compliance for specific RuleSet
akao docgen --ruleset=<name>  # Generate docs for specific RuleSet rules

# Graph Generation
akao graph --type=rules --format=svg --output=rules.svg        # Rule dependency graph
akao graph --type=rulesets --format=dot --output=rulesets.dot  # RuleSet relationships
akao graph --type=project --format=json                        # Project structure (stdout)
akao graph --type=features --format=png --output=features.png  # Feature dependencies
akao graph --type=validation --format=svg                      # Validation flow graph
akao graph --type=audit --format=dot --ruleset=security        # Audit compliance graph

# Automation & CI/CD
akao pipeline generate        # Generate CI/CD pipeline configuration
akao pipeline validate       # Validate pipeline configuration
akao workflow create          # Create GitHub Actions workflows
akao workflow update          # Update existing workflows
akao deploy configure        # Configure deployment settings
```

---

## 🔌 Plugin Targets (Examples)

* wasm / web-assembly builds
* js bindings (for Node.js)
* android (via NDK)
* custom doc extractors or linters

Each plugin is declared in `plugins/*.json` with:

* inputs, outputs
* language scope
* enforcement flags
* optional trace hooks

---

## 📜 Licensing & Attribution

* Akao core is MIT-licensed
* The name "Akao" is trademarked by Son Nguyen
* You may fork, use, and remix the system, but cannot use the name "Akao" without permission

---

## 🧠 See Also

* [`ARCHITECTURE.md`](./ARCHITECTURE.md) – system layers and technical design
* [`PHILOSOPHY.md`](./PHILOSOPHY.md) – full principle set and philosophical doctrine

---

## 🏁 Status

This README is auto-generated and must always match actual structure and rule definitions.
Use `akao docgen` to regenerate.
