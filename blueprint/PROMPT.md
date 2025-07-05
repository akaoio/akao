# Akao Framework One-Shot Implementation Prompt

## Session Objective

Generate the complete Akao framework implementation in C++ (C++17+) - a rule-based system for project structure enforcement, build management, automation, and documentation generation with cross-platform support and multi-language capability. This single coding session must produce a fully functional framework that validates, builds, documents, and manages any software project while enforcing all philosophical principles through code with Global Rule Identifier (GID) traceability.

## System Overview

Akao is a comprehensive C++ framework implementing universal validation, rule-driven enforcement, and automation across any language, OS, or runtime. The system provides identical functionality through CLI, Web UI, TUI, and API interfaces while maintaining complete audit trails through a Global Rule Identifier (GID) system. All 13 major subsystems work together to enforce 22+ philosophical principles through testable rules and measurable compliance.

**Core Technology Stack:**
- **Language**: C++ (C++17 or later) with strict layered architecture
- **Cross-Platform**: Linux, macOS, Windows, Android, iOS, WebAssembly, embedded systems
- **Multi-Language Support**: C++, JavaScript, Rust, Python, Go language adapters
- **Interface Parity**: CLI = Web UI = TUI = API with verified identical functionality
- **Architecture Pattern**: Layered separation with interface adapters ensuring platform/language agnosticism

## Component Breakdown

### 1. Core Framework (`src/core/`)
**Responsibility**: Foundation services for configuration, file system operations, audit logging, and plugin management across all platforms.
**Purpose**: Provide cross-platform foundation services that work identically across all supported platforms.
**Connections**: Used by all other components; provides configuration management, safe file operations, comprehensive audit trails, and plugin loading capabilities.

**Required Classes:**
- `src/core/config/ConfigManager.{hpp,cpp}` - `.akao/config.yaml` management with platform detection
- `src/core/filesystem/FileSystem.{hpp,cpp}` - Safe file operations with path validation and cross-platform sandboxing
- `src/core/trace/TraceLogger.{hpp,cpp}` - GID-based audit logging and error tracking
- `src/core/plugin/PluginManager.{hpp,cpp}` - Plugin loading and management with security validation

### 2. Rule Engine (`src/rule/`)
**Responsibility**: Multi-format rule parsing, validation, and violation reporting with complete GID integration.
**Purpose**: Enforce structural and behavioral rules across all project types with traceable violations.
**Connections**: Core of validation system; used by CLI, build system, documentation generator, and all interfaces.

**Required Classes:**
- `src/rule/parser/RuleParser.{hpp,cpp}` - Multi-format parser (YAML/TOML) with GID validation
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
**Responsibility**: Cross-platform build management with dev/prod profiles and hot reload capability.
**Purpose**: Provide development and production builds with file monitoring and integrity verification.
**Connections**: Integrates with validation system, used by CLI and automation, generates artifacts for deployment.

**Required Classes:**
- `src/build/manager/BuildManager.{hpp,cpp}` - Build target management (dev/prod profiles)
- `src/build/watcher/FileWatcher.{hpp,cpp}` - File change monitoring for hot reload development
- `src/build/hasher/BuildHasher.{hpp,cpp}` - Reproducible build verification and integrity
- `src/build/graph/DependencyGraph.{hpp,cpp}` - Build dependency graph resolution

### 5. Documentation Generator (`src/doc/`)
**Responsibility**: Template-based documentation generation with auto-update coordination.
**Purpose**: Generate RULES.md, PRINCIPLES.md, FEATURES.md, and API documentation from code and rules.
**Connections**: Reads from rule engine, integrates with graph system, triggered by CLI commands.

**Required Classes:**
- `src/doc/engine/DocumentationEngine.{hpp,cpp}` - Template-based Markdown generation engine
- `src/doc/parser/CodeParser.{hpp,cpp}` - Code comment parsing and extraction
- `src/doc/mapper/RuleMapper.{hpp,cpp}` - Rule-to-documentation mapping system
- `src/doc/updater/AutoUpdater.{hpp,cpp}` - Automatic documentation update coordination

### 6. Graph Generation System (`src/graph/`)
**Responsibility**: Visual representation and analysis of system relationships and dependencies.
**Purpose**: Generate graphs for rules, rulesets, project structure, features, validation flows, and audit compliance.
**Connections**: Integrates with all systems for data extraction, used by CLI, Web UI, and documentation generator.

**Required Classes:**
- `src/graph/generator/GraphGenerator.{hpp,cpp}` - Core graph generation engine for all graph types
- `src/graph/exporter/GraphExporter.{hpp,cpp}` - Multi-format export (DOT, SVG, YAML, PNG)
- `src/graph/analyzer/GraphAnalyzer.{hpp,cpp}` - Graph analysis and metrics calculation
- `src/graph/renderer/GraphRenderer.{hpp,cpp}` - Graph visualization and layout algorithms

### 7. CLI & Interface System (`src/interfaces/`)
**Responsibility**: Unified interface system ensuring CLI = Web = TUI = API parity.
**Purpose**: Provide identical functionality across all user interfaces with common execution layer.
**Connections**: Front-end for all system operations, delegates to appropriate subsystems.

**Required Classes:**
- `src/interfaces/cli/CLIInterface.{hpp,cpp}` - Command-line interface implementation
- `src/interfaces/web/WebInterface.{hpp,cpp}` - Web UI with REST API backend
- `src/interfaces/tui/TUIInterface.{hpp,cpp}` - Text User Interface implementation
- `src/interfaces/api/APIInterface.{hpp,cpp}` - REST API for external integration
- `src/interfaces/core/InterfaceController.{hpp,cpp}` - Common command execution abstraction layer

### 8. Automation System (`src/automation/`)
**Responsibility**: CI/CD pipeline generation and management across platforms.
**Purpose**: Generate GitHub Actions, GitLab CI, Jenkins workflows with cross-platform support.
**Connections**: Reads project configuration, generates pipeline files, integrates with build system.

**Required Classes:**
- `src/automation/pipeline/PipelineGenerator.{hpp,cpp}` - CI/CD pipeline generator with multi-platform support
- `src/automation/template/TemplateEngine.{hpp,cpp}` - Pipeline template engine
- `src/automation/configurator/PipelineConfigurator.{hpp,cpp}` - Configuration via `.akao/pipeline.yaml`
- `src/automation/executor/LocalExecutor.{hpp,cpp}` - Local automation execution and testing

### 9. Project Management (`src/project/`)
**Responsibility**: Project template management, initialization, and lifecycle operations.
**Purpose**: Create and manage projects from validated templates with feature integration.
**Connections**: Uses template system, integrates with feature management, validates with rule engine.

**Required Classes:**
- `src/project/template/TemplateEngine.{hpp,cpp}` - Project template engine with variable substitution
- `src/project/initializer/ProjectInitializer.{hpp,cpp}` - Project initialization from templates
- `src/project/manager/ProjectManager.{hpp,cpp}` - Project lifecycle management
- `src/project/validator/ProjectValidator.{hpp,cpp}` - Project structure and compliance validation

### 10. Feature Management (`src/feature/`)
**Responsibility**: Feature lifecycle management with external registry support and security sandboxing.
**Purpose**: Install, manage, and secure external features with dependency resolution.
**Connections**: Integrates with security system, uses dependency resolver, manages external registries.

**Required Classes:**
- `src/feature/manager/FeatureManager.{hpp,cpp}` - Feature lifecycle management and orchestration
- `src/feature/registry/RegistryClient.{hpp,cpp}` - External registry client with caching
- `src/feature/resolver/DependencyResolver.{hpp,cpp}` - Dependency resolution with conflict handling
- `src/feature/installer/FeatureInstaller.{hpp,cpp}` - Secure feature installation with validation
- `src/feature/sandbox/SecuritySandbox.{hpp,cpp}` - Feature execution sandboxing and isolation

### 11. Metrics System (`src/metrics/`)
**Responsibility**: Metrics collection, compliance scoring, and reporting with GID-based tracking.
**Purpose**: Provide measurable compliance metrics and project health indicators.
**Connections**: Collects data from all systems, generates reports, integrates with audit system.

**Required Classes:**
- `src/metrics/collector/MetricsCollector.{hpp,cpp}` - Metrics collection system with GID-based tracking
- `src/metrics/scorer/ComplianceScorer.{hpp,cpp}` - Compliance scoring algorithms per GID
- `src/metrics/reporter/MetricsReporter.{hpp,cpp}` - Metrics reporting and visualization
- `src/metrics/exporter/MetricsExporter.{hpp,cpp}` - Metrics export for external systems

### 12. Language Adapters (`src/language/`)
**Responsibility**: Language-specific rule binding and multi-language repository support.
**Purpose**: Enable validation and management of polyglot repositories with language isolation.
**Connections**: Integrates with rule engine, provides language-specific validation logic.

**Required Classes:**
- `src/language/cpp/CppAdapter.{hpp,cpp}` - C++ language adapter and rule binding
- `src/language/js/JavaScriptAdapter.{hpp,cpp}` - JavaScript adapter with Node.js integration
- `src/language/rust/RustAdapter.{hpp,cpp}` - Rust adapter with Cargo integration
- `src/language/python/PythonAdapter.{hpp,cpp}` - Python adapter with pip integration
- `src/language/go/GoAdapter.{hpp,cpp}` - Go adapter with modules integration
- `src/language/core/LanguageAdapter.{hpp,cpp}` - Common language adapter interface

### 13. Platform Adapters (`src/platform/`)
**Responsibility**: Platform-specific implementations maintaining unified interface.
**Purpose**: Handle OS-specific operations while maintaining cross-platform compatibility.
**Connections**: Used by all systems requiring platform-specific functionality.

**Required Classes:**
- `src/platform/linux/LinuxPlatform.{hpp,cpp}` - Linux-specific implementations
- `src/platform/macos/MacOSPlatform.{hpp,cpp}` - macOS-specific implementations
- `src/platform/windows/WindowsPlatform.{hpp,cpp}` - Windows-specific implementations
- `src/platform/android/AndroidPlatform.{hpp,cpp}` - Android NDK implementations
- `src/platform/ios/IOSPlatform.{hpp,cpp}` - iOS platform implementations
- `src/platform/wasm/WebAssemblyPlatform.{hpp,cpp}` - WebAssembly implementations
- `src/platform/embedded/EmbeddedPlatform.{hpp,cpp}` - Embedded systems implementations
## Technical Constraints

### Architectural Constraints

**Layered Architecture Requirements:**
- Core business logic must be completely agnostic to language, interface, and OS
- Language binding layer provides consistent APIs across C++, JS, Rust, Python, Go
- Platform adapter layer handles OS-specific implementations with unified interface
- Interface layer ensures CLI=Web=TUI=API parity through common execution layer
- Foundation services (config, filesystem, trace, plugin) work across all platforms

**Cross-Platform Support Matrix:**
- **Desktop**: Linux (x86_64, ARM64), macOS (x86_64, ARM64), Windows (x86_64)
- **Mobile**: Android (NDK), iOS (SDK)
- **Web**: WebAssembly for browser execution
- **Embedded**: Minimal footprint for constrained environments
- **Cloud**: Docker containers, serverless functions

**Multi-Language Repository Validation:**
- C++, JavaScript, Rust, Python, Go language adapters
- Language boundary enforcement and isolation mechanisms
- Multi-language monorepo validation and rule application
- Language-specific rule profiles and configuration management

### Global Rule Identifier (GID) System

**GID Format Requirements:**
- Format: `akao:rule::<category>:<n>:v<version>`
- Examples: `akao:rule::cpp:naming:snake_case:v1`, `akao:rule::structure:one_class_per_folder:v1`
- Global uniqueness across all registries and rule sets
- Version management preserves GID stability across rule updates

**GID Integration Requirements:**
- All rule files (YAML/TOML) must include GID in structure
- CLI operations support `--gid=<gid>` parameter for specific rule targeting
- Audit trails reference all rules by GID in trace.yaml and audit.yaml
- Violation reports include GID, file path, line number, and actionable suggestions
- RuleSet definitions reference rules by GID for organizational grouping

### RuleSet System Requirements

**RuleSet Structure:**
- Location: `.akao/rulesets/` directory with YAML or YAML format
- Inheritance: Parent RuleSets, includes, excludes resolution
- Built-in RuleSets: core.yaml, cpp.yaml, security.yaml, performance.yaml
- CLI integration: `ruleset list/info/validate/create` commands

**RuleSet Operations:**
- RuleSet-based validation: `validate --ruleset=<name>`
- RuleSet-based auditing: `audit --ruleset=<name>`
- RuleSet inheritance resolution with conflict handling

### Interface Parity Requirements

**CLI = Web = TUI = API Enforcement:**
- All commands must produce identical results across interfaces
- Error messages and status codes consistent across interfaces
- Response formats standardized (YAML/XML/YAML)
- Automated testing validates full parity across all interfaces

**Command Mapping Requirements:**
- 1:1 mapping between CLI commands and API endpoints
- Web UI provides browser interface with identical functionality
- TUI provides full-screen terminal interface with complete feature set
- API provides comprehensive REST endpoints for external integration

### Graph Generation Requirements

**Graph Types:**
- Rule dependency graphs showing GID relationships
- RuleSet relationship graphs with inheritance visualization
- Project structure graphs for architecture visualization
- Feature dependency graphs with conflict detection
- Validation flow graphs for debugging and optimization
- Audit compliance graphs for metrics and trends

**Export Formats:**
- DOT format for Graphviz compatibility
- SVG format for web and documentation embedding
- YAML format for programmatic access
- PNG format for presentations and reports

### Philosophy Enforcement Requirements

**All 22+ Philosophical Principles Must Be Technically Enforced:**

1. **Structure is enforced** - Rule engine validates project structure with GID tracking
2. **Universal validation** - Validation works on any project including Akao across all platforms
3. **No rules without tests** - Every rule has corresponding test case with GID validation
4. **Every rule is traceable** - Violations include GID, file path, line number, and suggestions
5. **Documentation is code** - All docs generated from rules and tests with GID references
6. **One class per folder** - Project structure enforces this principle with validation
7. **One language per scope** - Multi-language support with clear boundary enforcement
8. **CLI = Web = TUI = API** - Identical functionality verified through automated testing
9. **Every action is measurable** - Complete audit trail and metrics with GID breakdown
10. **Rules can be toggled, not skipped** - Disabled rules tracked, audited, and reported by GID
11. **Auto-update documentation** - Docs regenerate on code/rule changes with graph integration
12. **Everything builds dev + prod** - Development and production builds for all platforms
13. **No external implicit behavior** - All dependencies explicitly declared and managed
14. **Only one truth** - Single source of truth maintained across all components
15. **Observable, explainable, deterministic** - Full transparency in all operations with traceability
16. **Features are composable** - Features work together without conflicts across languages
17. **Templates are validated** - All templates pass the same rules on all platforms
18. **External features are sandboxed** - Security isolation with platform-specific enforcement
19. **Dependency resolution is explicit** - Clear dependency management with conflict resolution
20. **Every rule is part of at least one RuleSet** - Organizational grouping with inheritance
21. **Code must be graph-explorable** - Visual representation of all relationships
22. **All logic must be explainable visually** - Graph generation for debugging and comprehension

### Implementation Enforcement
- **Each principle must be implemented as a testable rule** with corresponding GID
- **Each principle must have test coverage** in `tests/principles/`
- **Each principle must be validated** by the universal validation system
- **Principle violations must cause build failures** and be tracked in audit logs


## Expected Output

### Complete Directory Structure (555+ Files)

```
akao/
├── .akao/                      # Framework runtime configuration
│   ├── config.yaml            # Main configuration file
│   ├── pipeline.yaml          # CI/CD pipeline configuration
│   ├── profiles/               # Language-specific rule profiles
│   │   ├── cpp.yaml           # C++ rules and build settings
│   │   ├── js.yaml            # JavaScript rules and build settings
│   │   ├── rust.yaml          # Rust rules and build settings
│   │   ├── python.yaml        # Python rules and build settings
│   │   └── go.yaml            # Go rules and build settings
│   ├── rulesets/               # RuleSet definitions and inheritance
│   │   ├── core.yaml          # Core framework RuleSet
│   │   ├── cpp.yaml           # C++ language RuleSet
│   │   ├── security.yaml      # Security-focused RuleSet
│   │   ├── performance.yaml   # Performance RuleSet
│   │   └── custom/            # Project-specific RuleSets
│   │       ├── api.yaml       # API development RuleSet
│   │       └── database.yaml  # Database integration RuleSet
│   ├── features/               # Feature management
│   │   ├── installed.yaml     # List of installed features
│   │   ├── dependencies.yaml  # Feature dependency graph
│   │   └── cache/             # Feature download cache
│   ├── registry/               # External registries
│   │   ├── official.yaml      # Official Akao registry
│   │   └── custom.yaml        # Custom registries
│   ├── trace.yaml             # Audit log and violation trace
│   ├── audit.yaml             # System audit and compliance metrics
│   └── cache/                 # Build and validation cache
│       ├── rules.cache        # Compiled rule cache
│       └── deps.cache         # Dependency resolution cache
├── .build/                     # Build outputs and artifacts
│   ├── dev/                   # Development builds (hot reload)
│   │   ├── akao               # Development binary
│   │   └── objects/           # Object files
│   ├── prod/                  # Production builds (optimized)
│   │   ├── akao               # Production binary
│   │   └── hashes/            # File integrity hashes
│   ├── meta.yaml              # Build metadata and dependencies
│   └── hash.yaml              # Reproducible build verification
├── .github/                    # Auto-generated CI/CD workflows
│   ├── workflows/             # GitHub Actions workflows
│   │   ├── ci.yml             # Continuous integration
│   │   ├── cd.yml             # Continuous deployment
│   │   └── validate.yml       # Universal validation workflow
│   └── templates/             # Workflow templates
├── docs/                       # Auto-generated documentation
│   ├── README.md              # Generated project documentation
│   ├── RULES.md               # Generated rule documentation
│   ├── PRINCIPLES.md          # Generated principle documentation
│   ├── FEATURES.md            # Generated feature documentation
│   └── api/                   # Generated API documentation
│       ├── core.md            # Core framework API
│       ├── cli.md             # CLI interface API
│       ├── automation.md      # Automation system API
│       ├── project.md         # Project management API
│       └── feature.md         # Feature management API
├── rules/                      # Rule definitions and enforcement
│   ├── core.yaml              # Core framework rules
│   ├── structure.yaml         # Project structure rules
│   ├── naming.yaml            # Naming convention rules
│   ├── testing.yaml           # Testing requirement rules
│   ├── documentation.yaml     # Documentation rules
│   ├── security.yaml          # Security and sandboxing rules
│   └── performance.yaml       # Performance requirement rules
├── templates/                  # Project and feature templates
│   ├── projects/              # Project initialization templates
│   │   ├── cpp/               # C++ project template
│   │   ├── web/               # Web project template
│   │   └── library/           # Library project template
│   └── features/              # Feature templates
│       ├── api/               # API feature template
│       ├── database/          # Database feature template
│       └── auth/              # Authentication feature template
├── src/                        # Source code (1 class per folder)
│   ├── main.cpp               # Application entry point
│   ├── core/                  # Core framework components
│   ├── rule/                  # Rule engine system
│   ├── build/                 # Build engine system
│   ├── doc/                # Documentation generation system
│   ├── graph/                 # Graph generation system
│   ├── interfaces/            # Interface system (CLI/Web/TUI/API)
│   ├── automation/            # Automation and CI/CD system
│   ├── project/               # Project management system
│   ├── feature/               # Feature management system
│   ├── metrics/               # Metrics and compliance system
│   ├── language/              # Language adapters
│   └── platform/              # Platform adapters
├── include/                    # Header files (mirrors src structure)
│   ├── akao/                  # Main namespace headers
│   └── akao.hpp               # Main framework header
├── tests/                      # Comprehensive test suite
│   ├── unit/                  # Unit tests (per component)
│   ├── integration/           # Integration tests
│   └── principles/            # Principle validation tests
├── features/                   # Built-in project features
│   ├── api/                   # REST API feature
│   ├── database/              # Database integration feature
│   └── auth/                  # Authentication feature
├── plugins/                    # Optional extensions
│   ├── wasm/                  # WebAssembly build target
│   ├── js/                    # JavaScript bindings
│   └── android/               # Android NDK target
├── CMakeLists.txt             # CMake build configuration
├── .gitignore                 # Git ignore patterns
└── LICENSE                    # MIT license file
```

### Cross-Platform Build Configuration

Complete CMakeLists.txt supporting all target platforms:

```cmake
cmake_minimum_required(VERSION 3.20)
project(akao VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform detection and configuration
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(AKAO_PLATFORM "linux")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(AKAO_PLATFORM "macos")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(AKAO_PLATFORM "windows")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Android")
    set(AKAO_PLATFORM "android")
elseif(CMAKE_SYSTEM_NAME STREQUAL "iOS")
    set(AKAO_PLATFORM "ios")
elseif(EMSCRIPTEN)
    set(AKAO_PLATFORM "wasm")
else()
    set(AKAO_PLATFORM "embedded")
endif()

# Cross-platform configuration options
option(AKAO_BUILD_TESTS "Build comprehensive test suite" ON)
option(AKAO_BUILD_PLUGINS "Build language and platform plugins" ON)
option(AKAO_BUILD_WEB_UI "Build Web UI interface" ON)
option(AKAO_BUILD_TUI "Build Text User Interface" ON)
option(AKAO_STATIC_LINKING "Use static linking for dependencies" ON)

# All core libraries and executables with cross-platform support
# [Complete CMake configuration for all platforms and features]
```

### Interface Parity Implementation

All interfaces must provide identical functionality through common execution layer:

```cpp
namespace akao::interfaces {
class InterfaceController {
public:
    CommandResult executeCommand(const CommandRequest& request);
private:
    CommandRegistry command_registry_;
};

struct CommandRequest {
    std::string command_name;
    std::map<std::string, std::string> parameters;
    std::string interface_type;  // "cli", "web", "tui", "api"
};

struct CommandResult {
    bool success;
    std::string output;
    std::map<std::string, std::string> metadata;
    std::vector<std::string> errors;
};
}
```

This prompt specification enables complete one-shot implementation of the Akao framework with full cross-platform support, multi-language capability, comprehensive testing, and strict philosophical principle enforcement through measurable, traceable rules with Global Rule Identifier (GID) integration.
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
akao doc [--ruleset=<n>]                 # Generate documentation
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
# Formats: dot, svg, yaml, png
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
- `.akao/config.yaml` - Main framework configuration with platform detection
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