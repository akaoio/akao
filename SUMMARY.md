# Akao Framework Technical Summary

> **Developer Quick Reference**: Complete technical specification for implementing the Akao framework

---

## 📋 System Overview

**Core Technology**: C++ (C++17+) with layered architecture for cross-platform, multi-language support  
**Primary Purpose**: Rule-based project structure enforcement, build management, automation, and documentation generation  
**Architecture Pattern**: Layered separation with interface adapters (CLI=Web=TUI=API parity)  

**Major Subsystems (9 core + supporting)**:
- Core Framework, Rule Engine, RuleSet Management, Build System, Documentation Generator
- Graph Generation System, CLI Interface, Automation System, Project Management, Metrics System

---

## 🗂️ Directory Structure

### Core Implementation (`src/`)
```
src/
├── core/                    # Foundation services
│   ├── config/             # Configuration management
│   ├── filesystem/         # Cross-platform file operations
│   ├── trace/              # Audit logging and trace system
│   └── plugin/             # Plugin loading and management
├── rule/                   # Rule processing engine
│   ├── parser/             # Multi-format rule parser (JSON/YAML/TOML)
│   ├── validator/          # Universal validation with GID tracking
│   ├── registry/           # GID-based rule discovery
│   ├── reporter/           # Violation reporting with GID references
│   └── ruleset/            # RuleSet management subsystem
│       ├── manager/        # RuleSet lifecycle management
│       ├── resolver/       # Inheritance and dependency resolution
│       ├── parser/         # RuleSet definition parser
│       └── validator/      # RuleSet validation
├── build/                  # Build system components
│   ├── manager/            # Build target management (dev/prod)
│   ├── watcher/            # File change monitoring
│   ├── hasher/             # Build verification and integrity
│   └── graph/              # Build dependency resolution
├── docgen/                 # Documentation generation
│   ├── engine/             # Template-based generation
│   ├── parser/             # Code comment extraction
│   ├── mapper/             # Rule-to-doc mapping
│   └── updater/            # Auto-update coordination
├── graph/                  # Graph generation system
│   ├── generator/          # Core graph generation
│   ├── exporter/           # Multi-format export (DOT/SVG/JSON/PNG)
│   ├── analyzer/           # Graph analysis and metrics
│   └── renderer/           # Visualization and layout
├── cli/                    # Command processing
│   ├── parser/             # Command parsing with GID support
│   ├── controller/         # Unified command execution
│   ├── formatter/          # Output formatting
│   └── validator/          # Command validation
├── interfaces/             # Interface adapters (parity enforcement)
│   ├── cli/                # Command-line interface
│   ├── web/                # Web UI with REST API
│   ├── tui/                # Text User Interface
│   ├── api/                # REST API for external integration
│   └── core/               # Common execution abstraction
├── automation/             # CI/CD automation
│   ├── pipeline/           # Pipeline generator
│   ├── template/           # Template engine (GitHub Actions/GitLab/Jenkins)
│   ├── configurator/       # Configuration via .akao/pipeline.yaml
│   └── executor/           # Local automation execution
├── project/                # Project management
│   ├── template/           # Project template engine
│   ├── initializer/        # Project initialization
│   ├── manager/            # Project lifecycle
│   └── validator/          # Project compliance validation
├── feature/                # Feature management
│   ├── manager/            # Feature lifecycle orchestration
│   ├── registry/           # External registry client
│   ├── resolver/           # Dependency resolution
│   ├── installer/          # Secure installation
│   └── sandbox/            # Security isolation
├── metrics/                # Metrics and compliance
│   ├── collector/          # Metrics collection with GID tracking
│   ├── scorer/             # Compliance scoring per GID
│   ├── reporter/           # Metrics reporting and visualization
│   └── exporter/           # External system export
└── language/               # Language adapters
    ├── cpp/                # C++ adapter and rule binding
    ├── js/                 # JavaScript adapter
    ├── rust/               # Rust adapter
    ├── python/             # Python adapter
    ├── go/                 # Go adapter
    └── core/               # Common adapter interface
```

### Project Runtime (`.akao/`)
```
.akao/
├── config.json            # Main configuration
├── pipeline.yaml          # CI/CD pipeline configuration
├── profiles/               # Language-specific rule profiles
│   ├── cpp.json           # C++ rules and build settings
│   ├── js.json            # JavaScript rules
│   └── rust.json          # Rust rules
├── rulesets/               # RuleSet definitions and inheritance
│   ├── core.yaml          # Core framework RuleSet
│   ├── cpp.yaml           # C++ language RuleSet
│   ├── security.yaml      # Security-focused RuleSet
│   └── custom/            # Project-specific RuleSets
├── features/               # Installed features and dependencies
├── registry/               # External feature registries
├── trace.json             # Operation audit trail with GID references
└── audit.json             # Compliance metrics and violation tracking
```

---

## 🖥️ CLI Command Taxonomy

### Project Management
- `akao init [--template=<type>]` - Initialize project with templates
- `akao feature add/remove/list/info <name>` - Local feature management
- `akao registry add/list <url>` - External feature registry management
- `akao install/update/uninstall <feature>` - External feature operations

### Core Operations
- `akao validate [--gid=<gid>] [--category=<cat>] [--ruleset=<name>]` - Structure/rule enforcement
- `akao test` - Unit and principle tests
- `akao build --dev/--prod` - Hot reload or production builds
- `akao docgen [--ruleset=<name>]` - Generate documentation
- `akao audit [--gid=<gid>] [--ruleset=<name>]` - Compliance audit and coverage

### Rule Management (GID-based)
- `akao rule list [--format=gid]` - List rules with categories/GIDs
- `akao rule info --gid=<gid>` - Detailed rule information
- `akao rule disable/enable --gid=<gid> [--reason="<reason>"]` - Rule state management

### RuleSet Management
- `akao ruleset list/info/validate/create <name>` - RuleSet operations
- `akao validate/audit --ruleset=<name>` - RuleSet-based operations

### Graph Generation
- `akao graph --type=<type> --format=<format> [--output=<path>] [--ruleset=<name>]`
  - **Types**: rules, rulesets, project, features, validation, audit
  - **Formats**: dot, svg, json, png

### Automation & CI/CD
- `akao pipeline generate/validate` - Pipeline configuration
- `akao workflow create/update` - GitHub Actions workflows
- `akao deploy configure` - Deployment settings

---

## 🔍 Global Rule Identifier (GID) System

### Format Specification
```
akao:rule::<category>:<name>:v<version>
```

**Examples**:
- `akao:rule::cpp:naming:snake_case:v1`
- `akao:rule::structure:one_class_per_folder:v1` 
- `akao:rule::testing:coverage_minimum:v2`

### Integration Points
- **Rule Files**: All JSON/YAML/TOML rule definitions include GID
- **CLI Operations**: `--gid=<gid>` parameter for specific rule targeting
- **Audit Trails**: All trace.json and audit.json entries reference rules by GID
- **Violation Reports**: Every violation includes GID, file path, line, suggestion
- **RuleSet Definitions**: RuleSets reference rules by GID for grouping

### GID-based CLI Usage
```bash
akao validate --gid="akao:rule::cpp:naming:snake_case:v1"
akao rule info --gid="akao:rule::structure:one_class_per_folder:v1"
akao rule disable --gid="akao:rule::testing:coverage_minimum:v2" --reason="Legacy code"
akao audit --gid="akao:rule::cpp:naming:snake_case:v1"
```

---

## 📦 RuleSet System

### Structure and Location
- **Location**: `.akao/rulesets/` directory
- **Formats**: YAML or JSON
- **Inheritance**: Parent RuleSets, includes, excludes resolution
- **Built-in RuleSets**: core.yaml, cpp.yaml, security.yaml, performance.yaml

### Example RuleSet Definition
```yaml
# .akao/rulesets/cpp.yaml
name: "C++ Development RuleSet"
description: "Standard C++ development rules"
version: "1.0"
parent: "core"
includes:
  - "akao:rule::cpp:naming:snake_case:v1"
  - "akao:rule::structure:one_class_per_folder:v1"
  - "akao:rule::testing:coverage_minimum:v2"
excludes:
  - "akao:rule::general:line_length:v1"
metadata:
  tags: ["cpp", "development"]
  documentation: "Standard rules for C++ development"
```

### RuleSet Operations
```bash
akao ruleset list                    # List all available RuleSets
akao ruleset info cpp                # Show cpp RuleSet details
akao validate --ruleset=security     # Validate using security RuleSet
akao audit --ruleset=cpp             # Audit compliance for C++ RuleSet
```

---

## 📄 File Formats and Structure

### Rule Files (Multi-format support)
```json
// rules/naming.json
{
  "rules": [
    {
      "gid": "akao:rule::cpp:naming:snake_case:v1",
      "name": "Snake Case Naming",
      "description": "Variables and functions must use snake_case",
      "category": "naming",
      "severity": "error",
      "applies_to": ["variables", "functions"],
      "examples": {
        "valid": ["user_name", "calculate_total"],
        "invalid": ["userName", "calculateTotal"]
      }
    }
  ]
}
```

### Audit Output Files
```json
// .akao/trace.json - Operation audit trail
{
  "timestamp": "2024-01-01T00:00:00Z",
  "operation": "validate",
  "violations": [
    {
      "gid": "akao:rule::naming:snake_case:v1",
      "file": "src/core/config/ConfigManager.cpp",
      "line": 42,
      "message": "Variable violates snake_case convention",
      "suggestion": "Rename to 'config_data'"
    }
  ]
}

// .akao/audit.json - Compliance metrics
{
  "project_compliance": {
    "overall_score": 0.95,
    "rule_compliance": {
      "akao:rule::naming:snake_case:v1": {
        "score": 0.98,
        "violations": 2,
        "total_checks": 100
      }
    }
  }
}
```

### Configuration Files
```json
// .akao/config.json
{
  "project": {
    "name": "example-project",
    "version": "1.0.0",
    "language": "cpp"
  },
  "validation": {
    "default_ruleset": "cpp",
    "strict_mode": true
  },
  "build": {
    "dev_watch": true,
    "prod_optimization": "aggressive"
  }
}
```

---

## 🌐 Platform and Language Support

### Supported Operating Systems
- **Desktop**: Linux (x86_64, ARM64), macOS (x86_64, ARM64), Windows (x86_64)
- **Mobile**: Android (NDK), iOS (SDK)
- **Embedded**: WebAssembly, Embedded Linux ARM
- **Cloud**: Docker Alpine/Ubuntu containers

### Language Adapters
- **Primary**: C++ (core implementation)
- **Supported**: JavaScript, Rust, Python, Go
- **Architecture**: Language-agnostic core with language-specific adapters

### CI/CD Matrix
```yaml
# Auto-generated GitHub Actions matrix
platforms: [ubuntu-latest, ubuntu-arm64, macos-latest, macos-arm64, windows-latest, android-ndk, ios-sdk, wasm32]
validation: ["akao validate", "akao test", "akao build --dev", "akao build --prod"]
```

---

## ⚙️ Implementation Requirements

### Architectural Constraints
- **One Class Per Folder**: Every `src/` folder contains exactly one class (`.cpp` + `.hpp`)
- **Corresponding Tests**: Every class has unit test in `tests/unit/<path>/`
- **Interface Parity**: CLI = Web = TUI = API (identical functionality)
- **Layered Architecture**: Core logic agnostic to language/interface/OS
- **Universal Validation**: Works on any project including Akao itself

### Code Organization Rules
```cpp
// Structure example
src/core/config/
├── config.cpp      # Implementation
└── config.hpp      # Header

tests/unit/core/config/
└── config_test.cpp # Unit tests
```

### Build Requirements
- **Dev Build**: Hot reload, fast compilation, debugging symbols
- **Prod Build**: Optimization, reproducible hashes, static linking preferred
- **Cross-platform**: All builds must succeed on CI matrix platforms

---

## 📊 Key System Outputs

### Documentation Generation
- **Auto-generated**: README.md, RULES.md, API documentation
- **Rule Documentation**: Generated from rule definitions with GID references
- **Graph Integration**: Embedded graphs in documentation

### Audit and Compliance
- **Trace Files**: Complete operation audit trail (`.akao/trace.json`)
- **Audit Reports**: Compliance metrics per GID (`.akao/audit.json`)
- **Compliance Scoring**: Measurable project health metrics
- **Violation Reports**: Detailed file/line violations with suggestions

### Graph Outputs
- **Rule Dependencies**: Rule relationship graphs (DOT, SVG, JSON, PNG)
- **RuleSet Inheritance**: RuleSet relationship visualization
- **Project Structure**: Visual project architecture
- **Validation Flow**: Validation process graphs
- **Audit Compliance**: Compliance status visualization

### Build Artifacts
- **Development**: Hot-reload binaries with debugging support
- **Production**: Optimized, reproducible binaries with integrity hashes
- **Cross-platform**: Native binaries for all supported platforms

---

## 🔧 Development Phase Order

1. **Phase 1**: Core Framework Foundation (config, filesystem, trace, plugin, basic CLI, GID system)
2. **Phase 2**: Universal Validation & Principle Testing (rule engine, RuleSet system, graph generation, metrics)
3. **Phase 3**: Project Management & Templates (project initialization, feature management)
4. **Phase 4**: Build System & Hot Reload (dev/prod builds, file watching, dependency graphs)
5. **Phase 5**: Documentation Generator (auto-generation, template engine, graph integration)
6. **Phase 6**: Interface Parity (Web UI, TUI, API with CLI functionality matching)
7. **Phase 7**: Advanced Feature Management (external registries, security sandboxing)
8. **Phase 8**: Automation & CI/CD (pipeline generation, GitHub Actions, deployment)
9. **Phase 9**: Integration & Polish (performance optimization, comprehensive testing, final validation)

---

## 📚 Reference Documents

- **[PLAN.md](./PLAN.md)**: Complete implementation specification (2600+ lines)
- **[ARCHITECTURE.md](./ARCHITECTURE.md)**: System components and technical design  
- **[PHILOSOPHY.md](./PHILOSOPHY.md)**: Enforced principles and rules doctrine
- **[README.md](./README.md)**: User interface and CLI command reference

---

*This summary provides the complete technical scope for implementing Akao. For detailed implementation specifications, refer to PLAN.md.*