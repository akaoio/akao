# Akao Architecture

> **Technical architecture and system design for the Akao framework**

---

## 🏗️ System Overview

Akao is built as a modular C++ framework with clear architectural layers:

```
┌─────────────────────────────────────────┐
│             CLI Interface               │
├─────────────────────────────────────────┤
│          Command Dispatcher             │
├─────────────────────────────────────────┤
│   Rule Engine  │  Build Engine  │  Doc  │
│   Validator    │  Dev/Prod      │  Gen  │
├─────────────────────────────────────────┤
│         Core Framework Layer            │
├─────────────────────────────────────────┤
│    File System │ Config │ Trace/Audit   │
└─────────────────────────────────────────┘
```

---

## 🧩 Core Components

### 1. Rule Engine (`src/rule/`)
- **Parser**: JSON/DSL rule file parser
- **Validator**: Rule execution and validation logic
- **Registry**: Rule discovery and management
- **Reporter**: Violation reporting and suggestions

### 2. Build Engine (`src/build/`)
- **Target Manager**: Dev/prod build profiles
- **Dependency Graph**: Build order resolution
- **File Watcher**: Hot reload for dev builds
- **Hash Validator**: Reproducible build verification

### 3. Documentation Generator (`src/docgen/`)
- **Template Engine**: Markdown generation from rules
- **Code Parser**: Extract docs from source comments
- **Rule Mapper**: Link principles to enforcement
- **Auto Updater**: Keep docs in sync with code

### 4. CLI System (`src/cli/`)
- **Command Parser**: Argument parsing and validation
- **Interactive Mode**: TUI interface
- **Web Server**: Web UI backend
- **Plugin Loader**: External tool integration

### 5. Core Framework (`src/core/`)
- **Config Manager**: `.akao/` configuration handling
- **File System**: Safe file operations with sandboxing
- **Trace System**: Audit logging and error tracking
- **Plugin API**: Extension point for custom rules

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

### 7. Automation System (`src/automation/`)
- **Pipeline Generator**: CI/CD pipeline configuration generator
- **Workflow Manager**: GitHub Actions workflow management
- **Template Engine**: Automated generation from templates
- **Deployment Configurator**: Automated deployment setup

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
│   ├── trace.json           # Audit and error trace
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
1. Load rules from rules/
2. Parse project structure
3. Apply rules to each scope
4. Generate violation reports
5. Update trace.json with results
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
