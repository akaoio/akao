# Akao Architecture

> **Technical architecture and system design for the Akao framework**

---

## 🏗️ System Overview

Akao is built as a modular C++ framework with **strict layered architecture** supporting any language, any OS, any runtime:

```
┌─────────────────────────────────────────────────────────────┐
│                    Interface Layer                          │
│  ┌─────────────┬─────────────┬─────────────┬─────────────┐  │
│  │     CLI     │   Web UI    │     TUI     │     API     │  │
│  │   Adapter   │   Adapter   │   Adapter   │   Adapter   │  │
│  └─────────────┴─────────────┴─────────────┴─────────────┘  │
├─────────────────────────────────────────────────────────────┤
│                  Language Binding Layer                     │
│  ┌─────────────┬─────────────┬─────────────┬─────────────┐  │
│  │    C++      │     JS      │    Rust     │   Python    │  │
│  │   Bindings  │  Bindings   │  Bindings   │  Bindings   │  │
│  └─────────────┴─────────────┴─────────────┴─────────────┘  │
├─────────────────────────────────────────────────────────────┤
│                   Platform Adapter Layer                    │
│  ┌─────────────┬─────────────┬─────────────┬─────────────┐  │
│  │   Linux     │   macOS     │  Windows    │  WebAssembly│  │
│  │   Android   │     iOS     │  Embedded   │   Server    │  │
│  └─────────────┴─────────────┴─────────────┴─────────────┘  │
├─────────────────────────────────────────────────────────────┤
│                    Core Business Logic                      │
│ ┌─────────────┬─────────────┬─────────────┬─────────────┐   │
│ │Rule Engine │Build System │Doc Generator│Graph System │   │
│ │RuleSet Mgmt│Project Mgmt │Feature Mgmt │Metrics Sys  │   │
│ │Automation  │   CLI Core  │             │             │   │
│ └─────────────┴─────────────┴─────────────┴─────────────┘   │
├─────────────────────────────────────────────────────────────┤
│                   Foundation Services                       │
│ ┌─────────────┬─────────────┬─────────────┬─────────────┐   │
│ │File System │Config Mgmt  │Trace/Audit  │Plugin Sys   │   │
│ │Cache Mgmt   │Error Handle │Logging      │Threading    │   │
│ └─────────────┴─────────────┴─────────────┴─────────────┘   │
└─────────────────────────────────────────────────────────────┘
```

**Architecture Principles:**
- **Core logic is completely agnostic** to language, interface, and OS
- **Language bindings** provide consistent APIs across C++, JS, Rust, Python, Go
- **Platform adapters** handle OS-specific implementations with unified interface
- **Interface layer** ensures CLI=Web=TUI=API parity through common execution layer

---

## 🧩 Core Components

The Akao framework consists of **9 major subsystems** implemented with layered architecture for cross-platform and multi-language support:

### 1. Rule Engine (`src/rule/`)
- **Parser**: Multi-format rule file parser (JSON/YAML/TOML) with GID validation
- **Validator**: Universal validation engine with GID tracking and cross-platform support
- **Registry**: GID-based rule discovery and management system (`akao:rule::<category>:<name>:v<version>`)
- **RuleSet Manager**: RuleSet lifecycle, inheritance, and resolution engine
- **Reporter**: Violation reporting with GID references and actionable suggestions

### 2. RuleSet Management (`src/rule/ruleset/`)
- **Manager**: RuleSet lifecycle and inheritance management
- **Resolver**: RuleSet inheritance, includes, and excludes resolution
- **Parser**: Multi-format RuleSet definition parser (YAML/JSON)
- **Validator**: RuleSet validation and consistency checking

### 3. Build System (`src/build/`)
- **Manager**: Cross-platform build target management (dev/prod profiles)
- **Watcher**: File change monitoring for hot reload development
- **Hasher**: Reproducible build verification and integrity checking
- **Graph**: Build dependency graph resolution and optimization

### 4. Documentation Generator (`src/docgen/`)
- **Engine**: Template-based Markdown generation with graph integration
- **Parser**: Code comment parsing and extraction across languages
- **Mapper**: Rule-to-documentation mapping with GID references
- **Updater**: Automatic documentation update coordination

### 5. Graph Generation System (`src/graph/`)
- **Generator**: Core graph generation engine for all graph types (rules, rulesets, project, features, validation, audit)
- **Exporter**: Multi-format export (DOT, SVG, JSON, PNG) with consistent quality
- **Analyzer**: Graph analysis and metrics calculation
- **Renderer**: Graph visualization and layout algorithms

### 6. CLI & Interface System (`src/cli/` & `src/interfaces/`)
- **CLI Interface**: Command-line interface with GID support and cross-platform compatibility
- **Web UI Interface**: Browser-based interface with identical functionality to CLI
- **TUI Interface**: Text User Interface with full command parity
- **API Interface**: REST API with comprehensive endpoint coverage
- **Core Abstraction**: Common command execution layer ensuring interface parity

### 7. Automation System (`src/automation/`)
- **Pipeline Generator**: CI/CD pipeline generator with multi-platform support (GitHub Actions, GitLab CI, Jenkins)
- **Template Engine**: Pipeline template engine with platform-specific optimizations
- **Configurator**: Pipeline configuration via `.akao/pipeline.yaml` with cross-platform settings
- **Executor**: Local automation execution and testing framework

### 8. Project Management (`src/project/`)
- **Template Engine**: Project template system with variable substitution and multi-language support
- **Initializer**: Project initialization from templates with platform detection
- **Manager**: Project lifecycle management across platforms and languages
- **Validator**: Project structure and compliance validation

### 9. Feature Management (`src/feature/`)
- **Manager**: Feature lifecycle management and orchestration
- **Registry**: External registry client with caching and security validation
- **Resolver**: Dependency resolution with conflict handling
- **Installer**: Secure feature installation with platform-specific sandboxing
- **Sandbox**: Feature execution security isolation and resource limits

### 10. Core Framework (`src/core/`)
- **Config Manager**: Cross-platform `.akao/` configuration handling with platform detection
- **File System**: Safe file operations with platform-specific path handling and sandboxing
- **Trace System**: GID-based audit logging and error tracking with comprehensive metrics
- **Plugin API**: Extension point for custom rules with platform-specific loading

### 11. Metrics System (`src/metrics/`)
- **Collector**: Metrics collection system with GID-based tracking
- **Scorer**: Compliance scoring algorithms per GID and RuleSet
- **Reporter**: Metrics reporting and visualization with GID breakdown
- **Exporter**: Metrics export for external systems (Prometheus, Grafana)

### 12. Language Adapters (`src/language/`)
- **C++ Adapter**: Native C++ language support and rule binding
- **JavaScript Adapter**: JavaScript/Node.js language adapter with V8 integration
- **Rust Adapter**: Rust language adapter with cargo integration
- **Python Adapter**: Python language adapter with pip integration
- **Go Adapter**: Go language adapter with go modules integration
- **Core Interface**: Common language adapter interface and utilities

### 13. Platform Adapters (`src/platform/`)
- **Linux**: Linux-specific implementations (x86_64, ARM64)
- **macOS**: macOS-specific implementations (Intel, Apple Silicon)
- **Windows**: Windows-specific implementations (MSVC, MinGW)
- **Android**: Android NDK platform implementations
- **iOS**: iOS platform implementations
- **WebAssembly**: WebAssembly platform implementations
- **Embedded**: Embedded systems platform implementations

### 6. Project Manager (`src/project/`)
- **Template Engine**: Project initialization from templates
- **Feature Manager**: Add/remove/update project features
- **Registry Client**: External feature registry integration
- **Dependency Resolver**: Feature dependency management

### 7. Feature System (`src/feature/`)
- **Feature Loader**: Load and validate features
- **Package Manager**: Install/update external features
- **Version Manager**: Feature versioning and compatibility
- **Conflict Resolver**: Handle feature conflicts and dependencies

### 8. Automation System (`src/automation/`)
- **Pipeline Generator**: CI/CD pipeline configuration generator
- **Workflow Manager**: GitHub Actions workflow management
- **Template Engine**: Automated generation from templates
- **Deployment Configurator**: Automated deployment setup

### 9. Graph Generation System (`src/graph/`)
- **Graph Generator**: Multi-type graph generation (rules, rulesets, project, features, validation, audit)
- **Format Exporter**: Multi-format export (DOT, SVG, JSON, PNG)
- **Graph Analyzer**: Graph analysis, metrics, and relationship discovery
- **Integration Engine**: Graph integration with docgen, audit, and Web UI

---

## 📁 Directory Structure

```
akao/
├── .akao/                    # Framework configuration
│   ├── config.json          # Main configuration
│   ├── pipeline.yaml        # CI/CD pipeline configuration
│   ├── profiles/             # Language-specific profiles
│   ├── features/             # Installed features metadata
│   │   ├── installed.json   # List of installed features
│   │   ├── dependencies.json # Feature dependency graph
│   │   └── cache/           # Feature download cache
│   ├── registry/             # External registries
│   │   ├── official.json    # Official Akao registry
│   │   └── custom.json      # Custom registries
│   ├── trace.json           # GID-based audit and error trace with rule compliance
│   ├── audit.json           # System audit and compliance metrics per GID
│   └── cache/               # Build and validation cache
├── .github/                  # Auto-generated CI/CD workflows
│   ├── workflows/           # GitHub Actions workflows
│   │   ├── ci.yml           # Continuous integration
│   │   ├── cd.yml           # Continuous deployment
│   │   ├── validate.yml     # Validation workflow
│   │   ├── test.yml         # Testing workflow
│   │   └── docs.yml         # Documentation generation
│   └── dependabot.yml       # Automated dependency updates
├── .build/                   # Build outputs
│   ├── dev/                 # Development builds
│   ├── prod/                # Production builds
│   └── meta.json            # Build metadata and hashes
├── docs/                     # Auto-generated documentation
│   ├── README.md            # Generated project overview
│   ├── RULES.md             # Generated rule documentation
│   ├── PRINCIPLES.md        # Generated principle documentation
│   ├── FEATURES.md          # Generated feature documentation
│   └── api/                 # API documentation
├── rules/                    # Rule definitions
│   ├── core/                # Core Akao rules
│   ├── cpp/                 # C++ specific rules
│   ├── js/                  # JavaScript specific rules
│   └── custom/              # Project-specific rules
├── features/                 # Project features
│   ├── core/                # Core project features
│   ├── external/            # Installed external features
│   └── custom/              # Custom project features
├── templates/                # Project and feature templates
│   ├── projects/            # Project initialization templates
│   │   ├── cpp/             # C++ project template
│   │   ├── web/             # Web project template
│   │   └── library/         # Library project template
│   └── features/            # Feature templates
│       ├── api/             # API feature template
│       ├── database/        # Database feature template
│       └── auth/            # Authentication feature template
├── src/                      # Source code (1 class per folder)
│   ├── rule/                # Rule engine implementation
│   ├── build/               # Build system implementation
│   ├── docgen/              # Documentation generator
│   ├── cli/                 # Command-line interface
│   ├── core/                # Core framework
│   ├── automation/          # CI/CD pipeline generation
│   ├── project/             # Project management
│   └── feature/             # Feature management
├── include/                  # C++ headers (mirrors src structure)
├── tests/                    # Test suite
│   ├── unit/                # Unit tests per class
│   ├── integration/         # Integration tests
│   └── principles/          # Principle validation tests
└── plugins/                  # Optional extensions
    ├── wasm/                # WebAssembly build target
    ├── js/                  # JavaScript bindings
    └── android/             # Android NDK target
```

---

## 🔄 Data Flow

### Validation Flow
```
1. Load rules from rules/ with GID validation
2. Parse project structure
3. Apply rules to each scope with GID tracking
4. Generate violation reports with GID references
5. Update trace.json and audit.json with GID-based results
```

### Build Flow
```
1. Read build profile (dev/prod)
2. Resolve dependency graph
3. Execute build steps
4. Generate output artifacts
5. Update build metadata with hashes
```

### Documentation Flow
```
1. Parse rule definitions
2. Extract code comments
3. Generate markdown templates
4. Cross-reference principles
5. Write documentation files
```

### Project Initialization Flow
```
1. Select project template
2. Generate base structure
3. Apply language-specific rules
4. Initialize configuration
5. Validate initial structure
```

### Feature Management Flow
```
1. Discover available features
2. Resolve dependencies
3. Download and validate
4. Integrate into project
5. Update configuration and docs
```

### Automation/CI/CD Flow
```
1. Analyze project structure and configuration
2. Generate pipeline configuration (.akao/pipeline.yaml)
3. Create GitHub Actions workflows (.github/workflows/)
4. Configure automated validation, testing, and builds
5. Set up deployment and release automation
```

---

## 🧪 Testing Strategy

### Unit Tests
- Each class in `src/` has corresponding test in `tests/unit/`
- Mock external dependencies
- Test isolated functionality

### Integration Tests
- Test component interactions
- Validate CLI command flows
- Test rule engine with real projects

### Principle Tests
- Validate that Akao follows its own rules
- Universal validation on any codebase
- Continuous principle compliance

---

## 🔌 Plugin Architecture

Plugins extend Akao functionality through:

```cpp
class AkaoPlugin {
public:
    virtual std::string name() const = 0;
    virtual bool validate(const Project& project) = 0;
    virtual BuildResult build(const BuildConfig& config) = 0;
    virtual void configure(const PluginConfig& config) = 0;
};
```

Plugin registration:
```json
{
  "name": "wasm-builder",
  "version": "1.0.0",
  "entry_point": "libwasm_builder.so",
  "supported_languages": ["cpp", "rust"],
  "build_targets": ["wasm32"]
}
```

---

## �️ Security & Sandboxing

- All file operations go through controlled API
- Build processes run in isolated environments
- Plugin execution is sandboxed
- Network access restricted during builds

---

## 📊 Performance Considerations

- Rule validation uses cached results when possible
- Build dependency graph optimized for parallel execution
- File watching uses efficient OS-specific mechanisms
- Documentation generation is incremental

---

## 🔧 Configuration

Main configuration in `.akao/config.json`:
```json
{
  "version": "1.0.0",
  "project": {
    "name": "my-project",
    "type": "cpp",
    "template": "library",
    "features": ["core", "testing", "docs"]
  },
  "language_profiles": ["cpp", "js"],
  "enabled_rules": ["*"],
  "disabled_rules": [],
  "build_targets": ["binary", "wasm"],
  "doc_output": "docs/",
  "trace_level": "info",
  "registries": [
    {
      "name": "official",
      "url": "https://registry.akao.dev",
      "enabled": true
    }
  ]
}
```

Feature configuration in `.akao/features/installed.json`:
```json
{
  "version": "1.0.0",
  "features": [
    {
      "name": "core",
      "version": "1.0.0",
      "source": "builtin",
      "dependencies": [],
      "enabled": true
    },
    {
      "name": "database",
      "version": "2.1.0",
      "source": "registry:official",
      "repository": "https://github.com/akao-features/database",
      "dependencies": ["core", "networking"],
      "enabled": true,
      "config": {
        "driver": "postgresql",
        "migrations": true
      }
    }
  ]
}
```

Language profiles in `.akao/profiles/`:
- `cpp.json` - C++ specific rules and build settings
- `js.json` - JavaScript specific rules and build settings
- `rust.json` - Rust specific rules and build settings

---

## � Design Principles

1. **Modularity**: Each component can be tested and replaced independently
2. **Extensibility**: Plugin system allows custom functionality
3. **Performance**: Designed for large codebases with efficient caching
4. **Reliability**: Comprehensive error handling and recovery
5. **Transparency**: All operations are logged and traceable
