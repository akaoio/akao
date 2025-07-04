# Akao Framework

> **A unified, rule-based framework designed for 100% AI-driven automation of software project lifecycle management.**

## Table of Contents {#akao:doc::readme:table_of_contents:v1}

1. [Overview](#akao:doc::readme:overview:v1)
2. [Core Capabilities](#akao:doc::readme:core_capabilities:v1)
3. [Philosophy Highlights](#akao:doc::readme:philosophy_highlights:v1)
4. [Use Cases](#akao:doc::readme:use_cases:v1)
5. [Structure](#akao:doc::readme:structure:v1)
6. [Commands](#akao:doc::readme:commands:v1)
7. [Interface Parity](#akao:doc::readme:interface_parity:v1)
8. [Cross-Platform Support](#akao:doc::readme:cross_platform_support:v1)
9. [Status](#akao:doc::readme:status:v1)

---

## Overview {#akao:doc::readme:overview:v1}

Akao is not a library — it is a **deterministic, AI-executable philosophy, architecture, and enforcement system** encoded in C++. It uses declarative rules, written in JSON, YAML, or TOML, to validate structure, enforce best practices, automate builds, and generate documentation. Each rule has a Global Rule Identifier (GID) in the format `akao:rule::<category>:<name>:v<version>` for precise tracking and management.

Akao applies its own standards to itself, demonstrating universal validation capabilities across any project while enabling complete automation from [philosophical foundations](#akao:doc::philosophy:rule_centric_system_core:v1) to production deployment.

> **Philosophical Foundation**: This approach implements the [AI-Native Execution](#akao:doc::philosophy:ai_native_execution:v1) and [Universal Validation](#akao:doc::philosophy:determinism_reproducibility:v1) principles — "Any project can be validated using `akao validate`, including Akao itself."

> **Technical Architecture**: The C++ implementation with layered architecture is detailed in [ARCHITECTURE.md](#akao:doc::architecture:system_overview:v1). The complete technical specification is in [PLAN.md](#akao:doc::plan:executive_summary:v1).

---

## Core Capabilities {#akao:doc::readme:core_capabilities:v1}

✅ **Universal validation system** - Validates any project structure with rule-based enforcement  
✅ **AI-native automation** - Complete automation workflow executable by AI agents  
✅ **Rule-driven validation** - GID-based rule system with complete traceability  
✅ **Build engine for dev/prod** - Hot reload development and optimized production builds  
✅ **Auto-generated documentation** - [Single source documentation](#akao:doc::philosophy:single_source_docgen:v1) with principle tracing  
✅ **Automated CI/CD pipeline generation** - Deterministic pipeline generation from rules  
✅ **Audit logs, coverage, enforcement trace** - Complete [observability and auditability](#akao:doc::philosophy:observability_auditability:v1)  
✅ **Unified CLI + Web UI + TUI** - [Interface parity](#akao:doc::philosophy:interface_parity_consistency:v1) across all interaction modes  
✅ **Plugin system** - [Modular, composable](#akao:doc::philosophy:modular_composable_extensible:v1) targets (wasm, js, android, etc)  
✅ **Pure C++ implementation** - [Language-agnostic enforcement](#akao:doc::philosophy:language_agnostic_enforcement:v1) with multi-language support  

> **Philosophy Implementation**: These capabilities directly implement core philosophical principles:
> - [Determinism & Reproducibility](#akao:doc::philosophy:determinism_reproducibility:v1) → Universal validation system
> - [Interface Parity & Consistency](#akao:doc::philosophy:interface_parity_consistency:v1) → Unified interface system
> - [Single Source Documentation](#akao:doc::philosophy:single_source_docgen:v1) → Auto-generated documentation
> - [Global Traceability](#akao:doc::philosophy:global_traceability_orphan_prevention:v1) → Complete audit trail

---

## Philosophy Highlights {#akao:doc::readme:philosophy_highlights:v1}

* [Rules are the law](#akao:doc::philosophy:rule_centric_system_core:v1). They govern everything.
* [All behavior is deterministic](#akao:doc::philosophy:determinism_reproducibility:v1) and reproducible.
* [Documentation is generated](#akao:doc::philosophy:single_source_docgen:v1), never hand-written.
* [Every test, validation, and build must be traceable](#akao:doc::philosophy:global_traceability_orphan_prevention:v1).
* [Structure is enforced](#akao:doc::philosophy:structural_enforcement:v1), not suggested.
* [Every class = 1 folder](#akao:doc::philosophy:structural_enforcement:v1), every folder = 1 rule scope.
* [All classes must be tested](#akao:doc::philosophy:rule_integrity_lifecycle:v1) and documented.
* [AI agents can regenerate the entire system](#akao:doc::philosophy:ai_native_execution:v1) without human intervention.

> **Complete Philosophy**: See full philosophical foundation in [PHILOSOPHY.md](#akao:doc::philosophy:core_principles:v1) with detailed explanations of all 19 principles.

> **Technical Enforcement**: How these principles are technically enforced is detailed in [ARCHITECTURE.md](#akao:doc::architecture:core_components:v1) and implementation specifications in [PLAN.md](#akao:doc::plan:implementation_requirements:v1).

---

## Use Cases {#akao:doc::readme:use_cases:v1}

### AI-Driven Development {#akao:doc::readme:ai_driven_development:v1}

- **Automated project generation** from philosophical requirements
- **Continuous compliance monitoring** with rule-based validation
- **Self-healing systems** that detect and correct violations
- **Complete documentation regeneration** from living code

### Enterprise Compliance {#akao:doc::readme:enterprise_compliance:v1}

- **SBOM generation** with complete dependency tracking
- **Audit trail maintenance** for all system changes
- **[Privacy and legal compliance](#akao:doc::philosophy:privacy_legal_compliance:v1)** by architectural design
- **Security enforcement** through [security-by-architecture](#akao:doc::philosophy:security_by_architecture:v1)

### Cross-Platform Development {#akao:doc::readme:cross_platform_development:v1}

- **Polyglot repository management** with [language-agnostic enforcement](#akao:doc::philosophy:language_agnostic_enforcement:v1)
- **Consistent build and validation** across all target platforms
- **Plugin-based extensibility** for new platforms and languages
- **[Resource-aware scalability](#akao:doc::philosophy:resource_aware_scalability:v1)** from local to distributed

---

## Structure {#akao:doc::readme:structure:v1}

```
project/
├── .akao/                    # Akao metadata and cache
│   ├── config.toml          # Project configuration
│   ├── audit.json           # Audit trail and compliance metrics
│   └── cache/               # Build and validation cache
├── rules/                   # Rule definitions (JSON/YAML/TOML)
│   ├── structure.toml       # Structural enforcement rules
│   ├── testing.toml         # Test coverage and quality rules
│   └── documentation.toml   # Documentation generation rules
├── rulesets/               # RuleSet definitions for rule grouping
│   ├── default.toml        # Default project ruleset
│   ├── enterprise.toml     # Enterprise compliance ruleset
│   └── ai-development.toml # AI-driven development ruleset
├── src/                    # Source code (language-specific structure)
├── tests/                  # Test code (mirrors src/ structure)
├── docs/                   # Generated documentation
│   ├── README.md           # Auto-generated project overview
│   ├── RULES.md            # Auto-generated rule documentation
│   └── ARCHITECTURE.md     # Auto-generated architecture docs
└── .gitignore              # Generated based on language and rules
```

> **Structural Enforcement**: This structure implements [Structural Enforcement](#akao:doc::philosophy:structural_enforcement:v1) principles with [rule-based validation](#akao:doc::philosophy:rule_centric_system_core:v1).

> **Architecture Details**: Complete directory structure and data flow are detailed in [ARCHITECTURE.md](#akao:doc::architecture:directory_structure:v1).

---

## Commands {#akao:doc::readme:commands:v1}

### Core Operations {#akao:doc::readme:core_operations:v1}

```bash
# Project lifecycle
akao init <project>           # Initialize new project with AI-guided setup
akao validate                 # Validate project against all applicable rules
akao build --dev             # Development build with hot reload
akao build --prod            # Production build with optimization
akao test                     # Run all tests with coverage tracking

# Documentation and automation
akao docgen                   # Generate README.md, RULES.md, etc from rules and code
akao audit                    # Print audit of rules, coverage, status
akao audit --gid=<gid>        # Audit specific rule compliance
```

### Rule Management (GID-based) {#akao:doc::readme:rule_management:v1}

```bash
# Rule discovery and management
akao rule list                # List all rules with their categories
akao rule list --format=gid   # List all rules showing their GIDs
akao rule info --gid=<gid>    # Show detailed rule information by GID
akao rule disable --gid=<gid> --reason="<reason>" # Disable rule (tracked)
akao rule enable --gid=<gid>  # Re-enable previously disabled rule
```

### RuleSet Management {#akao:doc::readme:ruleset_management:v1}

```bash
# RuleSet operations
akao ruleset list             # List all available RuleSets
akao ruleset info <name>      # Show RuleSet details and inheritance
akao ruleset validate <name>  # Validate RuleSet definition and dependencies
akao ruleset create <name>    # Create new RuleSet interactively
```

### AI Integration {#akao:doc::readme:ai_integration:v1}

```bash
# AI-driven operations
akao generate project <requirements> # Generate complete project from requirements
akao regenerate docs         # Regenerate all documentation with full traceability
akao autofix                  # Automatically fix detected rule violations
akao suggest improvements     # AI-generated improvement suggestions
```

> **Command Philosophy**: All commands implement [Interface Parity](#akao:doc::philosophy:interface_parity_consistency:v1) and are available identically through CLI, Web UI, TUI, and API interfaces.

> **Implementation Details**: Complete command specifications and API mappings are in [PLAN.md](#akao:doc::plan:cli_integration:v1).

---

## Interface Parity (CLI = Web = TUI = API) {#akao:doc::readme:interface_parity:v1}

> **Philosophy Implementation**: This implements [Interface Parity & Consistency](#akao:doc::philosophy:interface_parity_consistency:v1) - "CLI, Web, API, and MCP must expose the same capabilities and logic."

### CLI Interface {#akao:doc::readme:cli_interface:v1}
```bash
akao validate --ruleset=default --output=json
```

### Web API Interface {#akao:doc::readme:web_api_interface:v1}
```http
POST /api/v1/validate
{
  "ruleset": "default",
  "output": "json"
}
```

### TUI Interface {#akao:doc::readme:tui_interface:v1}
```
┌─ Akao Validation ─────────────────────┐
│ RuleSet: [default        ▼]          │
│ Output:  [json          ▼]           │
│ [Validate]                           │
└──────────────────────────────────────┘
```

### MCP (Model Context Protocol) Interface {#akao:doc::readme:mcp_interface:v1}
```json
{
  "method": "akao.validate",
  "params": {
    "ruleset": "default",
    "output": "json"
  }
}
```

All interfaces return identical results and support identical capabilities with consistent error handling and response formats.

---

## Cross-Platform Support {#akao:doc::readme:cross_platform_support:v1}

> **Philosophy Implementation**: This implements [Language-Agnostic Enforcement](#akao:doc::philosophy:language_agnostic_enforcement:v1) and [Resource-Aware Scalability](#akao:doc::philosophy:resource_aware_scalability:v1).

### Supported Platforms {#akao:doc::readme:supported_platforms:v1}

**Operating Systems:**
- Linux (x86_64, ARM64)
- macOS (Intel, Apple Silicon)
- Windows (x86_64, ARM64)
- WebAssembly (browser, server)

**Language Ecosystems:**
- C++ (native implementation)
- JavaScript/TypeScript (Node.js, Deno, Bun)
- Python (CPython, PyPy)
- Rust (native integration)
- Go (native integration)

**Deployment Targets:**
- Local development
- CI/CD pipelines
- Container environments (Docker, Podman)
- Cloud platforms (AWS, Azure, GCP)
- Edge computing
- Mobile (Android, iOS via bindings)

### Architecture Scalability {#akao:doc::readme:architecture_scalability:v1}

- **Local**: Single developer workflows
- **Team**: Shared rule enforcement and audit
- **Enterprise**: Distributed validation and compliance
- **Global**: Multi-region deployment with consistency

---

## Status {#akao:doc::readme:status:v1}

> **Philosophy Foundation**: This README implements the [Single Source Documentation](#akao:doc::philosophy:single_source_docgen:v1) and [AI-Native Execution](#akao:doc::philosophy:ai_native_execution:v1) principles from [PHILOSOPHY.md](#akao:doc::philosophy:core_principles:v1).
> 
> **Architecture Validation**: All features described here are architecturally specified in [ARCHITECTURE.md](#akao:doc::architecture:core_components:v1) and technically implementable according to [PLAN.md](#akao:doc::plan:implementation_requirements:v1).
> 
> **Cross-Reference Validation**: All commands, capabilities, and technical details are cross-referenced for complete auditability and [Global Traceability](#akao:doc::philosophy:global_traceability_orphan_prevention:v1).
> 
> **Generation Command**: This README is auto-generated and must always match actual structure and rule definitions. AI agents can regenerate using documented workflows in [PROMPT.md](#akao:doc::prompt:regeneration_workflow:v1).

### Cross-Document Consistency Checklist {#akao:doc::readme:consistency_checklist:v1}

- ✅ All CLI commands match [PLAN.md](#akao:doc::plan:cli_integration:v1) specifications
- ✅ All philosophical principles reference [PHILOSOPHY.md](#akao:doc::philosophy:core_principles:v1) IDs
- ✅ All architectural components link to [ARCHITECTURE.md](#akao:doc::architecture:core_components:v1) specifications  
- ✅ All automation workflows reference [PROMPT.md](#akao:doc::prompt:automation_workflow:v1) procedures
- ✅ All cross-references resolve to valid document IDs
- ✅ No orphaned sections or broken traceability chains

### Validation Commands {#akao:doc::readme:validation_commands:v1}

```bash
# Validate this documentation set
akao validate --ruleset=documentation

# Verify all cross-references resolve
akao audit --type=traceability

# Check for orphaned content
akao audit --type=orphan-detection

# Regenerate with full audit trail
akao docgen --audit-trail --verify-links
```

> **Validation**: All cross-references can be verified using `akao validate --ruleset=documentation` as specified in [Rule Integrity Across Lifecycle](#akao:doc::philosophy:rule_integrity_lifecycle:v1).

This README serves as the primary entry point for understanding the Akao framework while maintaining complete traceability to philosophical foundations and technical implementations.