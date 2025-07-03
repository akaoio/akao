# Akao Framework

> **A unified, rule-based framework written in C++ for structuring, building, testing, and documenting any software project.**

## Table of Contents

1. [Overview](#overview)
2. [Core Capabilities](#core-capabilities)
3. [Philosophy Highlights](#philosophy-highlights)
4. [Use Cases](#use-cases)
5. [Structure](#structure)
6. [Commands](#commands)
7. [Plugin Targets (Examples)](#plugin-targets-examples)
8. [Licensing & Attribution](#licensing-attribution)
9. [Interface Parity (CLI = Web = TUI = API)](#interface-parity-cli-web-tui-api)
10. [Cross-Platform Support](#cross-platform-support)
11. [Status](#status)

---

## Overview

Akao is not a library — it is a **philosophy, architecture, and enforcement system** encoded in code. It uses declarative rules, written in JSON, YAML, or TOML, to validate structure, enforce best practices, automate builds, and generate documentation. Each rule has a Global Rule Identifier (GID) in the format `akao:rule::<category>:<name>:v<version>` for precise tracking and management. Akao is written in **C++**, but is capable of managing polyglot repos (e.g., JS, Python, Rust) as long as each language follows structure and rule profiles.

Akao applies its own standards to itself, demonstrating that its validation system works universally across any project.

> **Philosophical Foundation**: This approach implements the "Universal validation" principle from [PHILOSOPHY.md](./PHILOSOPHY.md#core-beliefs) - "Any project can be validated using `akao validate`, including Akao itself."

> **Technical Architecture**: The C++ implementation with layered architecture is detailed in [ARCHITECTURE.md](./ARCHITECTURE.md#system-overview). The complete technical specification is in [PLAN.md](./PLAN.md#executive-summary).

> **Quick Reference**: For detailed technical specifications and implementation details, see [SUMMARY.md](./SUMMARY.md#system-overview).

---

## Core Capabilities

✅ Universal validation system  
✅ Rule-driven validation system  
✅ Build engine for dev/prod (hot reload, optimized)  
✅ Auto-generated documentation and principle tracing  
✅ Automated CI/CD pipeline generation  
✅ Audit logs, coverage, enforcement trace  
✅ Unified CLI + Web UI + TUI  
✅ Plugin system for targets (wasm, js, android, etc)  
✅ Pure C++ implementation  

> **Philosophy Implementation**: These capabilities directly implement core philosophical principles:
> - "Universal validation" → Universal validation system ([PHILOSOPHY.md](./PHILOSOPHY.md#core-beliefs))
> - "CLI = Web = TUI" → Unified interface system ([PHILOSOPHY.md](./PHILOSOPHY.md#core-beliefs))
> - "Documentation is code" → Auto-generated documentation ([PHILOSOPHY.md](./PHILOSOPHY.md#core-beliefs))
> - "Every action is measurable" → Audit logs and tracing ([PHILOSOPHY.md](./PHILOSOPHY.md#core-beliefs))

> **Technical Details**: The architecture implementing these capabilities is detailed in [ARCHITECTURE.md](./ARCHITECTURE.md#core-components) and technical specifications in [SUMMARY.md](./SUMMARY.md#system-overview).

---

## Philosophy Highlights

* Rules are the law. They govern everything.
* All behavior is deterministic and reproducible.
* Documentation is generated, never hand-written.
* Every test, validation, and build must be traceable.
* Structure is enforced, not suggested.
* Every class = 1 folder, every folder = 1 rule scope.
* All classes must be tested and documented.

> **Complete Philosophy**: See full philosophical foundation in [PHILOSOPHY.md](./PHILOSOPHY.md#core-beliefs) with detailed explanations of each principle.

> **Technical Enforcement**: How these principles are technically enforced is detailed in [ARCHITECTURE.md](./ARCHITECTURE.md#design-principles) and implementation specifications in [PLAN.md](./PLAN.md#implementation-requirements-class-structure).

See full: [`PHILOSOPHY.md`](./PHILOSOPHY.md#core-beliefs)

---

## Use Cases

> **Philosophy Foundation**: These use cases demonstrate the practical application of principles from [PHILOSOPHY.md](./PHILOSOPHY.md#core-beliefs), particularly "Universal validation" and "Structure is enforced".
> 
> **Technical Implementation**: The architecture enabling these use cases is detailed in [ARCHITECTURE.md](./ARCHITECTURE.md#system-overview) and complete implementation roadmap in [PLAN.md](./PLAN.md#executive-summary).
> 
> **Technical Reference**: Implementation requirements for these use cases are documented in [SUMMARY.md](./SUMMARY.md#implementation-requirements).

* C++ projects with strict structure and validation
* Mixed-language repositories with rules per language
* Validating large-scale enterprise codebases
* Framework development with rigorous standards
* Building rule-compliant plugins, mobile apps, APIs

---

## Structure

> **Philosophy Foundation**: This structure implements the "One class per folder", "Only one truth", and "No external implicit behavior" principles from [PHILOSOPHY.md](./PHILOSOPHY.md#core-beliefs).
> 
> **Architecture Implementation**: This structure is architected according to the layered design in [ARCHITECTURE.md](./ARCHITECTURE.md#system-overview) and detailed in [ARCHITECTURE.md](./ARCHITECTURE.md#directory-structure).
> 
> **Implementation Details**: Complete directory specifications and rationale are provided in [PLAN.md](./PLAN.md#complete-project-structure).
> 
> **Technical Reference**: Quick reference to this structure is available in [SUMMARY.md](./SUMMARY.md#directory-structure).

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

## Commands

> **Philosophy Foundation**: These commands implement the "CLI = Web = TUI", "Every rule is traceable", and "Every action is measurable" principles from [PHILOSOPHY.md](./PHILOSOPHY.md#core-beliefs).
> 
> **Architecture Implementation**: Command execution is handled through the CLI & Interface System detailed in [ARCHITECTURE.md](./ARCHITECTURE.md#core-components) with interface parity enforcement.
> 
> **Implementation Details**: Complete command specifications and GID integration are provided in [PLAN.md](./PLAN.md#global-rule-identifier-gid-system) and [PLAN.md](./PLAN.md#ruleset-system).
> 
> **Technical Reference**: Command taxonomy and patterns are summarized in [SUMMARY.md](./SUMMARY.md#cli-command-taxonomy).

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

## Plugin Targets (Examples)

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

## Licensing & Attribution

* Akao core is MIT-licensed
* The name "Akao" is trademarked by Son Nguyen
* You may fork, use, and remix the system, but cannot use the name "Akao" without permission

---

## Interface Parity (CLI = Web = TUI = API)

Akao provides **identical functionality** across all user interfaces:

### Command Line Interface (CLI)
```bash
akao validate --gid="akao:rule::cpp:naming:snake_case:v1"
akao build --dev --watch
akao graph --type=rules --format=svg
```

### Web User Interface
- Browser-based interface at `http://localhost:8080`
- All CLI commands available through web forms
- Real-time validation and build status
- Interactive graph visualization

### Text User Interface (TUI) 
```bash
akao --tui  # Launch interactive terminal interface
```
- Full-screen terminal interface with menus and forms
- Keyboard navigation for all operations
- Live dashboard with metrics and status

### REST API
```bash
POST /api/validate HTTP/1.1
Content-Type: application/json
{
  "gid": "akao:rule::cpp:naming:snake_case:v1",
  "project_path": "/path/to/project"
}

GET /api/graph?type=rules&format=svg
GET /api/rulesets
POST /api/build {"profile": "dev", "watch": true}
```

**Interface Parity Guarantee:**
- All commands produce identical results across interfaces
- Error messages and status codes are consistent
- Response formats are standardized (JSON/XML/YAML)
- Automated tests verify parity across all interfaces

> **Philosophy Reference**: This guarantee implements the "CLI = Web = TUI" principle from [PHILOSOPHY.md](./PHILOSOPHY.md#core-beliefs) - "All tooling must behave identically across user interface modes."

> **Technical Implementation**: The interface parity architecture is detailed in [ARCHITECTURE.md](./ARCHITECTURE.md#core-components) under "CLI & Interface System" and implementation specifications in [PLAN.md](./PLAN.md#interface-parity-enforcement).

> **Technical Details**: Complete interface specifications and command mappings are in [SUMMARY.md](./SUMMARY.md#cli-command-taxonomy).

---

## Cross-Platform Support

Akao runs on **any language, any OS, any runtime**:

### Supported Platforms
- **Desktop**: Linux (x86_64, ARM64), macOS (Intel, Apple Silicon), Windows
- **Mobile**: Android (NDK), iOS (Framework)
- **Web**: WebAssembly (WASM) for browser execution
- **Embedded**: Minimal footprint for constrained environments
- **Cloud**: Docker containers, serverless functions

### Language Support
- **Native**: C++ (primary implementation)
- **Bindings**: JavaScript/Node.js, Rust, Python, Go
- **Multi-language repositories**: Mixed-language monorepo validation
- **Language isolation**: Clear boundaries between language scopes

### Runtime Environments
- **Native binaries**: Direct OS execution
- **Containers**: Docker, Podman, OCI-compliant runtimes
- **Serverless**: AWS Lambda, Google Cloud Functions, Azure Functions
- **CI/CD**: GitHub Actions, GitLab CI, Jenkins, any platform

---

## Status

> **Philosophy Foundation**: This README implements the "Documentation is code" and "Auto-update all documentation" principles from [PHILOSOPHY.md](./PHILOSOPHY.md#core-beliefs).
> 
> **Architecture Validation**: All features described here are architecturally specified in [ARCHITECTURE.md](./ARCHITECTURE.md#core-components) and technically implementable according to [PLAN.md](./PLAN.md#implementation-requirements-class-structure).
> 
> **Cross-Reference Validation**: All commands, capabilities, and technical details are cross-referenced in [SUMMARY.md](./SUMMARY.md#reference-documents) for complete auditability.
> 
> **Generation Command**: This README is auto-generated and must always match actual structure and rule definitions. Use `akao docgen` to regenerate.

**Cross-Document Consistency Checklist:**
- ✅ All CLI commands match [PLAN.md](./PLAN.md#implementation-requirements-class-structure) specifications
- ✅ All capabilities are architecturally supported by [ARCHITECTURE.md](./ARCHITECTURE.md#core-components) 
- ✅ All principles reference their philosophical foundation in [PHILOSOPHY.md](./PHILOSOPHY.md#core-beliefs)
- ✅ All technical details are summarized in [SUMMARY.md](./SUMMARY.md#system-overview)
- ✅ All cross-references are verified and point to valid section anchors

> **Cross-Document Consistency**: This document is synchronized with:
> - [PHILOSOPHY.md](./PHILOSOPHY.md#core-beliefs) for principle alignment  
> - [ARCHITECTURE.md](./ARCHITECTURE.md#core-components) for technical accuracy
> - [PLAN.md](./PLAN.md#executive-summary) for implementation details
> - [SUMMARY.md](./SUMMARY.md#system-overview) for technical reference
> 
> **Validation**: All cross-references can be verified using `akao validate --ruleset=documentation` as specified in [PHILOSOPHY.md](./PHILOSOPHY.md#rules-as-doctrine).
