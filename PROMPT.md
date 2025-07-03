# Akao Framework One-Shot Implementation Prompt

## Session Objective

Generate the complete Akao framework implementation in C++ (C++17+) - a rule-based system for project structure enforcement, build management, automation, and documentation generation with cross-platform support. This single coding session must produce a fully functional framework that validates, builds, documents, and manages any software project while enforcing all philosophical principles through code.

## System Overview

Akao is a layered C++ framework implementing universal validation, rule-driven enforcement, and automation across any language, OS, or runtime. The system provides identical functionality through CLI, Web UI, TUI, and API interfaces while maintaining complete audit trails through a Global Rule Identifier (GID) system. All 13 major subsystems work together to enforce 19+ philosophical principles through testable rules and measurable compliance.

**Core Technology Stack:**
- **Language**: C++ (C++17 or later) with layered architecture
- **Cross-Platform**: Linux, macOS, Windows, Android, iOS, WebAssembly, embedded systems
- **Multi-Language Support**: C++, JavaScript, Rust, Python, Go language adapters
- **Interface Parity**: CLI = Web UI = TUI = API with identical functionality
- **Architecture Pattern**: Strict layered separation with interface adapters

## List of Components to Be Generated

### 1. Core Framework (`src/core/`)
**Responsibility**: Foundation services for configuration, file system operations, audit logging, and plugin management.
**Purpose**: Provide cross-platform foundation services that work identically across all supported platforms.
**Connections**: Used by all other components; provides configuration management, safe file operations, comprehensive audit trails, and plugin loading capabilities.

**Required Classes:**
- `src/core/config/ConfigManager.{hpp,cpp}` - `.akao/config.json` management with platform detection
- `src/core/filesystem/FileSystem.{hpp,cpp}` - Safe file operations with path validation and sandboxing
- `src/core/trace/TraceLogger.{hpp,cpp}` - GID-based audit logging and error tracking
- `src/core/plugin/PluginManager.{hpp,cpp}` - Plugin loading and management with security validation

### 2. Rule Engine (`src/rule/`)
**Responsibility**: Multi-format rule parsing, validation, and violation reporting with complete GID integration.
**Purpose**: Enforce structural and behavioral rules across all project types with traceable violations.
**Connections**: Core of validation system; used by CLI, build system, documentation generator, and all interfaces.

**Required Classes:**
- `src/rule/parser/RuleParser.{hpp,cpp}` - Multi-format parser (JSON/YAML/TOML) with GID validation
- `src/rule/validator/UniversalValidator.{hpp,cpp}` - Universal validation engine with GID tracking
- `src/rule/registry/RuleRegistry.{hpp,cpp}` - GID-based rule discovery and management
- `src/rule/reporter/ViolationReporter.{hpp,cpp}` - Violation reporting with GID references and suggestions

### 3. RuleSet Management (`src/rule/ruleset/`)
**Responsibility**: RuleSet lifecycle management, inheritance resolution, and organization.
**Purpose**: Organize rules into logical groupings with inheritance hierarchies for granular validation control.
**Connections**: Extends rule engine; used by CLI for targeted validation and audit operations.

**Required Classes:**
- `src/rule/ruleset/RuleSetManager.{hpp,cpp}` - RuleSet lifecycle and inheritance management
- `src/rule/ruleset/RuleSetResolver.{hpp,cpp}` - Inheritance, includes, and excludes resolution
- `src/rule/ruleset/RuleSetParser.{hpp,cpp}` - Multi-format RuleSet definition parser
- `src/rule/ruleset/RuleSetValidator.{hpp,cpp}` - RuleSet validation and consistency checking

### 4. Build System (`src/build/`)
**Responsibility**: Cross-platform build management with dev/prod profiles, hot reload, and reproducible builds.
**Purpose**: Provide fast development iteration and reliable production builds with integrity verification.
**Connections**: Integrates with rule engine for build validation; used by CLI and automation system.

**Required Classes:**
- `src/build/manager/BuildManager.{hpp,cpp}` - Build target management with platform profiles
- `src/build/watcher/FileWatcher.{hpp,cpp}` - File change monitoring for hot reload
- `src/build/hasher/BuildHasher.{hpp,cpp}` - Reproducible build verification and integrity checking
- `src/build/graph/DependencyGraph.{hpp,cpp}` - Build dependency resolution and optimization

### 5. Documentation Generator (`src/docgen/`)
**Responsibility**: Template-based documentation generation with automatic updates and graph integration.
**Purpose**: Generate all documentation from code and rules to ensure accuracy and prevent staleness.
**Connections**: Integrates with rule engine, graph system; used by CLI and automation for doc generation.

**Required Classes:**
- `src/docgen/engine/DocumentationEngine.{hpp,cpp}` - Template-based generation with graph integration
- `src/docgen/parser/CodeParser.{hpp,cpp}` - Code comment parsing across languages
- `src/docgen/mapper/RuleMapper.{hpp,cpp}` - Rule-to-documentation mapping with GID references
- `src/docgen/updater/DocumentationUpdater.{hpp,cpp}` - Automatic update coordination

### 6. Graph Generation System (`src/graph/`)
**Responsibility**: Multi-type graph generation, analysis, and export for system visualization.
**Purpose**: Make all system relationships visually explorable and understandable.
**Connections**: Used by documentation generator, CLI, Web UI for visualization and analysis.

**Required Classes:**
- `src/graph/generator/GraphGenerator.{hpp,cpp}` - Core generation for all graph types
- `src/graph/exporter/GraphExporter.{hpp,cpp}` - Multi-format export (DOT, SVG, JSON, PNG)
- `src/graph/analyzer/GraphAnalyzer.{hpp,cpp}` - Graph analysis and metrics calculation
- `src/graph/renderer/GraphRenderer.{hpp,cpp}` - Visualization and layout algorithms

### 7. CLI & Interface System (`src/cli/` & `src/interfaces/`)
**Responsibility**: Unified command execution with interface parity across CLI, Web UI, TUI, and API.
**Purpose**: Provide identical functionality through all user interface modes with consistent error handling.
**Connections**: Primary user interface; orchestrates all other components through unified command execution.

**Required Classes:**
- `src/cli/parser/ArgumentParser.{hpp,cpp}` - Command parsing with GID parameter support
- `src/cli/controller/InterfaceController.{hpp,cpp}` - Unified command execution for all interfaces
- `src/interfaces/cli/CLIInterface.{hpp,cpp}` - Command-line interface implementation
- `src/interfaces/web/WebInterface.{hpp,cpp}` - Browser-based interface with REST API
- `src/interfaces/tui/TUIInterface.{hpp,cpp}` - Text User Interface with full functionality
- `src/interfaces/api/APIInterface.{hpp,cpp}` - REST API with comprehensive endpoints

### 8. Automation System (`src/automation/`)
**Responsibility**: CI/CD pipeline generation with multi-platform support and template management.
**Purpose**: Generate automated workflows for GitHub Actions, GitLab CI, Jenkins with platform-specific optimizations.
**Connections**: Integrates with build system and rule engine; used by CLI for automation setup.

**Required Classes:**
- `src/automation/pipeline/PipelineGenerator.{hpp,cpp}` - CI/CD pipeline generation
- `src/automation/template/TemplateEngine.{hpp,cpp}` - Platform-specific template engine
- `src/automation/configurator/PipelineConfigurator.{hpp,cpp}` - Configuration via `.akao/pipeline.yaml`
- `src/automation/executor/AutomationExecutor.{hpp,cpp}` - Local automation execution

### 9. Project Management (`src/project/`)
**Responsibility**: Project initialization, template management, and lifecycle operations.
**Purpose**: Create valid projects from templates with platform detection and rule compliance.
**Connections**: Uses rule engine for validation; integrates with feature management and templates.

**Required Classes:**
- `src/project/template/TemplateEngine.{hpp,cpp}` - Template system with variable substitution
- `src/project/initializer/ProjectInitializer.{hpp,cpp}` - Project initialization from templates
- `src/project/manager/ProjectManager.{hpp,cpp}` - Project lifecycle management
- `src/project/validator/ProjectValidator.{hpp,cpp}` - Project structure and compliance validation

### 10. Feature Management (`src/feature/`)
**Responsibility**: Feature lifecycle, external registry integration, and secure sandboxed execution.
**Purpose**: Manage local and external features with dependency resolution and security isolation.
**Connections**: Integrates with security system; used by project management and CLI for feature operations.

**Required Classes:**
- `src/feature/manager/FeatureManager.{hpp,cpp}` - Feature lifecycle and orchestration
- `src/feature/registry/RegistryClient.{hpp,cpp}` - External registry integration with caching
- `src/feature/resolver/DependencyResolver.{hpp,cpp}` - Dependency resolution with conflict handling
- `src/feature/installer/FeatureInstaller.{hpp,cpp}` - Secure installation with validation
- `src/feature/sandbox/FeatureSandbox.{hpp,cpp}` - Security isolation and resource limits

### 11. Metrics System (`src/metrics/`)
**Responsibility**: Metrics collection, compliance scoring, and reporting with GID-based tracking.
**Purpose**: Provide measurable project health and compliance metrics for analysis and improvement.
**Connections**: Integrates with rule engine and audit system; used by CLI and Web UI for reporting.

**Required Classes:**
- `src/metrics/collector/MetricsCollector.{hpp,cpp}` - Metrics collection with GID tracking
- `src/metrics/scorer/ComplianceScorer.{hpp,cpp}` - Scoring algorithms per GID and RuleSet
- `src/metrics/reporter/MetricsReporter.{hpp,cpp}` - Reporting and visualization with GID breakdown
- `src/metrics/exporter/MetricsExporter.{hpp,cpp}` - External system export (Prometheus, Grafana)

### 12. Language Adapters (`src/language/`)
**Responsibility**: Language-specific adapters for multi-language repository support.
**Purpose**: Handle language-specific rules and build processes while maintaining language isolation.
**Connections**: Used by rule engine and build system for language-specific operations.

**Required Classes:**
- `src/language/cpp/CppAdapter.{hpp,cpp}` - C++ language support and rule binding
- `src/language/js/JavaScriptAdapter.{hpp,cpp}` - JavaScript/Node.js adapter
- `src/language/rust/RustAdapter.{hpp,cpp}` - Rust language adapter with cargo integration
- `src/language/python/PythonAdapter.{hpp,cpp}` - Python adapter with pip integration
- `src/language/go/GoAdapter.{hpp,cpp}` - Go adapter with modules integration
- `src/language/core/LanguageInterface.{hpp,cpp}` - Common adapter interface

### 13. Platform Adapters (`src/platform/`)
**Responsibility**: Platform-specific implementations for cross-platform support.
**Purpose**: Handle OS-specific operations while maintaining unified interface.
**Connections**: Used by core framework and all other components for platform-specific functionality.

**Required Classes:**
- `src/platform/linux/LinuxAdapter.{hpp,cpp}` - Linux-specific implementations
- `src/platform/macos/MacOSAdapter.{hpp,cpp}` - macOS-specific implementations
- `src/platform/windows/WindowsAdapter.{hpp,cpp}` - Windows-specific implementations
- `src/platform/android/AndroidAdapter.{hpp,cpp}` - Android NDK implementations
- `src/platform/ios/IOSAdapter.{hpp,cpp}` - iOS platform implementations
- `src/platform/wasm/WasmAdapter.{hpp,cpp}` - WebAssembly implementations
- `src/platform/embedded/EmbeddedAdapter.{hpp,cpp}` - Embedded systems implementations

## Technical Constraints

### Architecture Requirements
- **Must implement layered architecture** with strict separation: Interface Layer → Language Binding Layer → Platform Adapter Layer → Core Business Logic
- **Core business logic must be completely agnostic** to language, interface, and operating system
- **All interfaces (CLI/Web/TUI/API) must provide identical functionality** with automated parity testing
- **All components must follow one-class-per-folder principle** with corresponding header files in `include/` directory

### Global Rule Identifier (GID) System Implementation
- **All rules must have GID format**: `akao:rule::<category>:<n>:v<version>`
- **All rule files (JSON/YAML/TOML) must include GID** in their structure
- **All violations must include GID, file path, line number, and suggestions**
- **All audit and trace files must reference rules by GID**
- **CLI must support GID-based operations**: `--gid=<gid>` parameter for all rule commands

### RuleSet System Requirements
- **RuleSet definitions in `.akao/rulesets/`** with YAML/JSON format support
- **RuleSet inheritance with parent, includes, excludes resolution**
- **CLI commands**: `ruleset list/info/validate/create` functionality
- **RuleSet-based operations**: `validate --ruleset=<n>` and `audit --ruleset=<n>`

### Cross-Platform Support Requirements
- **Must build and run on**: Linux (x86_64, ARM64), macOS (Intel, Apple Silicon), Windows
- **Must support mobile platforms**: Android (NDK), iOS (Framework)
- **Must support web**: WebAssembly for browser execution
- **Must support embedded**: Minimal footprint for constrained environments
- **Must support containers**: Docker builds for multiple base images

### Multi-Language Repository Support
- **Language adapters for**: C++, JavaScript, Rust, Python, Go
- **Language isolation enforcement** with clear boundaries
- **Multi-language validation** with language-specific rules
- **Language-specific build integration** while maintaining unified interface

### Build System Requirements
- **CMake-based build system** with cross-platform configuration
- **Dev and prod build profiles** with hot reload for development
- **Reproducible builds** with hash verification and integrity checking
- **Static linking preferred** for core, dynamic loading for language adapters

### Testing Requirements
- **Comprehensive test suite** with >95% code coverage
- **Unit tests per class** in `tests/unit/`
- **Integration tests** in `tests/integration/`
- **Principle tests** in `tests/principles/` that validate all 19+ philosophical principles
- **Interface parity tests** in `tests/parity/` that verify CLI=Web=TUI=API consistency

## Design Principles

### Core Philosophical Principles (Must Be Enforced Through Code)

1. **Structure is enforced** - Every file, class, and folder must follow rule-driven structure
2. **Universal validation** - Any project can be validated using `akao validate`, including Akao itself
3. **No rules without tests** - Each rule must have measurable test coverage
4. **Every rule is traceable** - Violations include GID, stack trace, file, line, and suggestion
5. **Documentation is code** - All documentation generated from actual rules and tests
6. **One class per folder** - Every folder contains exactly one class and its tests
7. **One language per scope** - Multi-language repositories must isolate languages clearly
8. **CLI = Web = TUI = API** - All interfaces provide identical functionality
9. **Every action is measurable** - Complete audit trail and metrics collection
10. **Rules can be toggled, not skipped** - Disabled rules are tracked and audited
11. **Auto-update all documentation** - Documentation regenerates on code/rule changes
12. **Everything must build dev + prod** - Both development and production builds required
13. **No external implicit behavior** - All dependencies must be declared explicitly
14. **Only one truth** - Single source of truth maintained across all components
15. **Observable, explainable, deterministic** - Full transparency in all operations
16. **Features are composable** - Features work together without conflicts
17. **Templates are validated** - All templates pass the same rules as generated projects
18. **External features are sandboxed** - Features from external sources run in controlled environments
19. **Dependency resolution is explicit** - All dependencies declared and resolved transparently
20. **Every rule is part of at least one RuleSet** - Rules organized into logical groupings
21. **Code must be graph-explorable** - All relationships visualizable through generated graphs
22. **All logic must be explainable visually** - Every operation representable in visual form

### Implementation Enforcement
- **Each principle must be implemented as a testable rule** with corresponding GID
- **Each principle must have test coverage** in `tests/principles/`
- **Each principle must be validated** by the universal validation system
- **Principle violations must cause build failures** and be tracked in audit logs

## Expected Output

### Complete Directory Structure
```
akao/
├── .akao/                    # Framework runtime configuration
│   ├── config.json          # Main configuration file
│   ├── pipeline.yaml        # CI/CD pipeline configuration
│   ├── profiles/             # Language-specific profiles
│   │   ├── cpp.json          # C++ rules and build settings
│   │   ├── js.json           # JavaScript rules
│   │   └── rust.json         # Rust rules
│   ├── rulesets/             # RuleSet definitions
│   │   ├── core.yaml         # Core framework RuleSet
│   │   ├── cpp.yaml          # C++ language RuleSet
│   │   ├── security.yaml     # Security-focused RuleSet
│   │   └── custom/           # Project-specific RuleSets
│   ├── features/             # Installed features metadata
│   │   ├── installed.json    # List of installed features
│   │   ├── dependencies.json # Feature dependency graph
│   │   └── cache/            # Feature download cache
│   ├── registry/             # External registries
│   │   ├── official.json     # Official Akao registry
│   │   └── custom.json       # Custom registries
│   ├── trace.json            # GID-based audit and error trace
│   ├── audit.json            # System audit and compliance metrics
│   └── cache/                # Build and validation cache
├── .build/                   # Build outputs
│   ├── dev/                  # Development builds (hot reload)
│   │   ├── akao              # Development binary
│   │   └── plugins/          # Development plugins
│   ├── prod/                 # Production builds (optimized)
│   │   ├── akao              # Production binary
│   │   └── hashes/           # File integrity hashes
│   ├── meta.json             # Build metadata and dependencies
│   └── hash.json             # Reproducible build verification
├── .github/                  # Auto-generated CI/CD workflows
│   ├── workflows/            # GitHub Actions workflows
│   │   ├── ci.yml            # Continuous integration
│   │   ├── cd.yml            # Continuous deployment
│   │   └── validate.yml      # Universal validation workflow
│   └── templates/            # Workflow templates
├── docs/                     # Auto-generated documentation
│   ├── README.md             # Generated project documentation
│   ├── RULES.md              # Generated rule documentation
│   ├── PRINCIPLES.md         # Generated principle documentation
│   ├── FEATURES.md           # Generated feature documentation
│   └── api/                  # Generated API documentation
│       ├── core.md           # Core framework API
│       ├── cli.md            # CLI interface API
│       ├── automation.md     # Automation system API
│       ├── project.md        # Project management API
│       └── feature.md        # Feature management API
├── rules/                    # Rule definitions and enforcement
│   ├── core.json             # Core framework rules
│   ├── structure.json        # Project structure rules
│   ├── naming.json           # Naming convention rules
│   ├── testing.json          # Testing requirement rules
│   ├── documentation.json    # Documentation rules
│   ├── security.json         # Security and sandboxing rules
│   └── performance.json      # Performance requirement rules
├── templates/                # Project and feature templates
│   ├── projects/             # Project initialization templates
│   │   ├── cpp/              # C++ project template
│   │   ├── web/              # Web application template
│   │   └── library/          # Library project template
│   ├── features/             # Feature templates
│   │   ├── api/              # API feature template
│   │   ├── database/         # Database feature template
│   │   └── auth/             # Authentication feature template
│   └── docs/                 # Documentation templates
│       ├── RULES.md.template # Rule documentation template
│       └── README.md.template# Project README template
├── src/                      # Source code (1 class per folder)
│   ├── core/                 # Foundation services
│   │   ├── config/           # Configuration management
│   │   ├── filesystem/       # Cross-platform file operations
│   │   ├── trace/            # Audit logging and trace system
│   │   └── plugin/           # Plugin loading and management
│   ├── rule/                 # Rule processing engine
│   │   ├── parser/           # Multi-format rule parser
│   │   ├── validator/        # Universal validation engine
│   │   ├── registry/         # GID-based rule discovery
│   │   ├── reporter/         # Violation reporting
│   │   └── ruleset/          # RuleSet management
│   │       ├── manager/      # RuleSet lifecycle management
│   │       ├── resolver/     # Inheritance resolution
│   │       ├── parser/       # RuleSet definition parser
│   │       └── validator/    # RuleSet validation
│   ├── build/                # Build system implementation
│   │   ├── manager/          # Build target management
│   │   ├── watcher/          # File change monitoring
│   │   ├── hasher/           # Build integrity verification
│   │   └── graph/            # Dependency graph resolution
│   ├── docgen/               # Documentation generator
│   │   ├── engine/           # Template-based generation
│   │   ├── parser/           # Code comment parsing
│   │   ├── mapper/           # Rule-to-documentation mapping
│   │   └── updater/          # Automatic update coordination
│   ├── graph/                # Graph generation system
│   │   ├── generator/        # Core graph generation
│   │   ├── exporter/         # Multi-format export
│   │   ├── analyzer/         # Graph analysis
│   │   └── renderer/         # Visualization algorithms
│   ├── cli/                  # Command-line interface
│   │   ├── parser/           # Argument parsing
│   │   └── controller/       # Command execution
│   ├── interfaces/           # Interface implementations
│   │   ├── cli/              # CLI interface
│   │   ├── web/              # Web UI interface
│   │   ├── tui/              # Terminal UI interface
│   │   └── api/              # REST API interface
│   ├── automation/           # CI/CD pipeline generation
│   │   ├── pipeline/         # Pipeline generator
│   │   ├── template/         # Template engine
│   │   ├── configurator/     # Pipeline configuration
│   │   └── executor/         # Local automation execution
│   ├── project/              # Project management
│   │   ├── template/         # Template engine
│   │   ├── initializer/      # Project initialization
│   │   ├── manager/          # Project lifecycle
│   │   └── validator/        # Project validation
│   ├── feature/              # Feature management
│   │   ├── manager/          # Feature lifecycle
│   │   ├── registry/         # External registry client
│   │   ├── resolver/         # Dependency resolution
│   │   ├── installer/        # Secure installation
│   │   └── sandbox/          # Security isolation
│   ├── metrics/              # Metrics and compliance
│   │   ├── collector/        # Metrics collection
│   │   ├── scorer/           # Compliance scoring
│   │   ├── reporter/         # Metrics reporting
│   │   └── exporter/         # External export
│   ├── language/             # Language adapters
│   │   ├── cpp/              # C++ adapter
│   │   ├── js/               # JavaScript adapter
│   │   ├── rust/             # Rust adapter
│   │   ├── python/           # Python adapter
│   │   ├── go/               # Go adapter
│   │   └── core/             # Common interface
│   ├── platform/             # Platform adapters
│   │   ├── linux/            # Linux implementations
│   │   ├── macos/            # macOS implementations
│   │   ├── windows/          # Windows implementations
│   │   ├── android/          # Android NDK
│   │   ├── ios/              # iOS implementations
│   │   ├── wasm/             # WebAssembly
│   │   └── embedded/         # Embedded systems
│   └── main.cpp              # Main application entry point
├── include/                  # C++ headers (mirrors src structure)
│   ├── akao/                 # Main namespace headers
│   │   ├── core/             # Core framework headers
│   │   ├── rule/             # Rule engine headers
│   │   ├── build/            # Build system headers
│   │   ├── docgen/           # Documentation headers
│   │   ├── graph/            # Graph system headers
│   │   ├── cli/              # CLI headers
│   │   ├── interfaces/       # Interface headers
│   │   ├── automation/       # Automation headers
│   │   ├── project/          # Project management headers
│   │   ├── feature/          # Feature management headers
│   │   ├── metrics/          # Metrics headers
│   │   ├── language/         # Language adapter headers
│   │   └── platform/         # Platform adapter headers
│   └── akao.hpp              # Main include header
├── tests/                    # Test suite
│   ├── unit/                 # Unit tests per class
│   │   ├── core/             # Core framework tests
│   │   ├── rule/             # Rule engine tests
│   │   ├── build/            # Build system tests
│   │   ├── docgen/           # Documentation tests
│   │   ├── graph/            # Graph system tests
│   │   ├── cli/              # CLI tests
│   │   ├── interfaces/       # Interface tests
│   │   ├── automation/       # Automation tests
│   │   ├── project/          # Project management tests
│   │   ├── feature/          # Feature management tests
│   │   ├── metrics/          # Metrics tests
│   │   ├── language/         # Language adapter tests
│   │   └── platform/         # Platform adapter tests
│   ├── integration/          # Integration tests
│   │   ├── end_to_end/       # Complete workflow tests
│   │   ├── cross_platform/   # Platform compatibility tests
│   │   ├── multi_language/   # Language support tests
│   │   └── interface_parity/ # Interface consistency tests
│   ├── principles/           # Principle validation tests
│   │   ├── structure_enforcement.cpp     # Test structure enforcement
│   │   ├── universal_validation.cpp      # Test universal validation
│   │   ├── rule_traceability.cpp         # Test GID traceability
│   │   ├── documentation_generation.cpp  # Test doc generation
│   │   ├── interface_parity.cpp          # Test CLI=Web=TUI=API
│   │   └── principle_validation.cpp      # Master principle test
│   └── parity/               # Interface parity tests
│       ├── cli_web_parity.cpp           # CLI vs Web UI tests
│       ├── tui_api_parity.cpp           # TUI vs API tests
│       └── full_parity_test.cpp         # Complete parity validation
├── features/                 # Project-specific features
│   ├── manifest.json         # Feature manifest
│   └── examples/             # Example features
├── plugins/                  # Platform and language plugins
│   ├── wasm/                 # WebAssembly plugin
│   ├── js/                   # JavaScript plugin
│   ├── rust/                 # Rust plugin
│   ├── python/               # Python plugin
│   ├── go/                   # Go plugin
│   ├── android/              # Android plugin
│   └── ios/                  # iOS plugin
├── CMakeLists.txt            # Cross-platform build configuration
├── .gitignore                # Git ignore patterns
└── LICENSE                   # MIT license file
```

### CLI Command Interface (Must Be Fully Implemented)

#### Project Management Commands
```bash
akao init [--template=<type>]               # Initialize project with templates
akao feature add/remove/list/info <name>    # Local feature management
akao registry add/list <url>                # External feature registry management
akao install/update/uninstall <feature>     # External feature operations
```

#### Core Operations Commands
```bash
akao validate [--gid=<gid>] [--category=<cat>] [--ruleset=<n>]  # Structure/rule enforcement
akao test                                    # Unit and principle tests
akao build --dev/--prod                     # Hot reload or production builds
akao docgen [--ruleset=<n>]                 # Generate documentation
akao audit [--gid=<gid>] [--ruleset=<n>]    # Compliance audit and coverage
```

#### Rule Management Commands (GID-based)
```bash
akao rule list [--format=gid]               # List rules with categories/GIDs
akao rule info --gid=<gid>                  # Detailed rule information
akao rule disable/enable --gid=<gid> [--reason="<reason>"]  # Rule state management
```

#### RuleSet Management Commands
```bash
akao ruleset list/info/validate/create <name>  # RuleSet operations
akao validate/audit --ruleset=<name>           # RuleSet-based operations
```

#### Graph Generation Commands
```bash
akao graph --type=<type> --format=<format> [--output=<path>] [--ruleset=<n>]
# Types: rules, rulesets, project, features, validation, audit
# Formats: dot, svg, json, png
```

#### Automation & CI/CD Commands
```bash
akao pipeline generate/validate             # Pipeline configuration
akao workflow create/update                 # GitHub Actions workflows
akao deploy configure                       # Deployment settings
```

### Build Configuration Files

#### CMakeLists.txt (Cross-Platform)
Must include complete CMake configuration supporting:
- All target platforms (Linux, macOS, Windows, Android, iOS, WebAssembly, embedded)
- Cross-platform dependency management
- Platform-specific optimizations
- Complete test suite integration
- Plugin system support
- Static and dynamic linking options
- Sanitizer support for debugging
- Cross-compilation support

#### Configuration Files
- `.akao/config.json` - Main framework configuration with platform detection
- `.akao/pipeline.yaml` - CI/CD pipeline configuration with platform matrix
- All rule files in `rules/` directory with complete GID integration
- All RuleSet definitions in `.akao/rulesets/` with inheritance support
- Project templates in `templates/` directory with validation

### Integration Requirements

#### Universal Validation Implementation
- Must validate any project structure including Akao itself
- Must work identically across all platforms
- Must produce consistent results with GID-based violation reporting
- Must integrate with all rule categories and RuleSet systems

#### Interface Parity Implementation
- All four interfaces (CLI, Web UI, TUI, API) must provide identical functionality
- Automated tests must verify complete parity
- Consistent error handling and response formatting across all interfaces
- Common command execution layer shared by all interfaces

#### Cross-Platform Build Support
- Native compilation for all target platforms
- Cross-compilation support where applicable
- Platform-specific optimizations while maintaining code compatibility
- Automated CI/CD pipeline generation for all platforms

#### Security and Sandboxing
- Complete feature sandboxing for external features
- Input validation and sanitization for all user inputs
- Safe file system operations with path validation
- Resource limits and access controls for sandboxed features

This prompt provides complete specifications for implementing the entire Akao framework in a single coding session. All components, interfaces, and requirements are specified in sufficient detail to enable comprehensive implementation without additional clarification or ambiguity.