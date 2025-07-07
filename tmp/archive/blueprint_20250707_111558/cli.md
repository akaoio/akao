# 🚀 AKAO CLI SPECIFICATION

> **The Language of Languages - Framework of Frameworks**  
> Universal command-line interface for infinite validation

## 🎯 **CLI PHILOSOPHY COMPLIANCE**

**Interface Parity Law**: `CLI command = API endpoint = Web UI action`  
**Universal Validation**: `akao validate` works on ANY project  
**Traceability Law**: Every violation includes ID + file + line + suggestion  

---

## 📋 **MASTER COMMAND TABLE**

| Command | Subcommand | Options | Description | API Equivalent | Philosophy |
|---------|------------|---------|-------------|----------------|------------|
| `akao` | `--version` | | Show Akao version | `GET /version` | Universal |
| `akao` | `--help` | | Show help information | `GET /help` | Universal |
| `akao` | `init` | `[project-type]` | Initialize new Akao project | `POST /init` | Universal Genesis |
| `akao` | `validate` | `[target] [--rules]` | Universal validation | `POST /validate` | Universal Validation |
| `akao` | `check` | `[category] [--fix]` | Check specific rules | `POST /check` | Rule Governance |
| `akao` | `trace` | `[violation-id]` | Trace violation details | `GET /trace/{id}` | Traceability |
| `akao` | `generate` | `[type] [--template]` | Generate universes | `POST /generate` | Universal Genesis |
| `akao` | `build` | `[mode] [--target]` | Build with adapters | `POST /build` | Build Duality |
| `akao` | `test` | `[scope] [--coverage]` | Run compliance tests | `POST /test` | Testing Coverage |
| `akao` | `docs` | `[--generate] [--serve]` | Documentation commands | `GET /docs` | Documentation Generation |
| `akao` | `metrics` | `[--export] [--analyze]` | Metrics operations | `GET /metrics` | Observability |
| `akao` | `security` | `[--scan] [--enforce]` | Security operations | `POST /security` | Explicit Security |
| `akao` | `rules` | `[--list] [--add] [--verify]` | Rule management | `GET/POST /rules` | Rule Governance |
| `akao` | `config` | `[--set] [--get] [--reset]` | Configuration | `GET/POST /config` | Universal |

---

## 🌟 **CORE COMMANDS SPECIFICATION**

### 1. **`akao validate`** - Universal Validation Engine

```bash
# Universal validation (works on ANY project)
akao validate                           # Validate current directory
akao validate /path/to/project          # Validate specific project  
akao validate --rules structure         # Validate specific rule category
akao validate --rules all               # Validate all rules
akao validate --philosophy isolation    # Validate specific philosophy
akao validate --fix                     # Auto-fix violations where possible
akao validate --trace                   # Include detailed trace information
akao validate --format json             # Output in JSON format
akao validate --format yaml             # Output in YAML format
akao validate --format table            # Output in table format (default)
akao validate --export metrics.yaml     # Export metrics to file
```

**Output Format**:
```
✅ AKAO VALIDATION REPORT
📊 Project: /home/user/my-project
🕒 Timestamp: 2025-07-05T18:30:00Z
🏛️ Rules: 32 checked, 31 passed, 1 failed

❌ VIOLATIONS:
akao:rule::structure:class_separation:v1:violation:src/main.cpp:42
  📁 File: src/main.cpp:42
  📋 Rule: Structure Class Separation (v1)
  🎯 Issue: Multiple classes in single file
  💡 Suggestion: Move SecondClass to separate file
  📚 Philosophy: akao:philosophy::structure:isolation:v1
  🔍 Trace: validate_structure → check_class_separation → file_scanner
```

### 2. **`akao init`** - Universal Project Genesis

```bash
# Initialize different project types
akao init                               # Interactive project setup
akao init cpp-project                   # C++ project with Akao compliance
akao init rust-project                  # Rust project with Akao compliance
akao init framework                     # New framework with Akao rules
akao init language-adapter              # Language adapter template
akao init --template minimal            # Minimal Akao project
akao init --template full               # Full-featured Akao project
akao init --rules custom.yaml           # Custom rule set
akao init --philosophy strict           # Strict philosophy compliance
```

### 3. **`akao generate`** - Universe Generation

```bash
# Generate infinite possibilities
akao generate project --lang cpp        # Generate C++ project
akao generate framework --name MyFW     # Generate new framework
akao generate language-support --lang rust  # Generate Rust adapter
akao generate build-system --type cmake # Generate CMake integration
akao generate documentation             # Generate docs from rules
akao generate tests --coverage full     # Generate comprehensive tests
akao generate --template ./my-template  # Use custom template
akao generate --universe ./target-dir   # Generate into specific universe
```

### 4. **`akao check`** - Rule Compliance Checking

```bash
# Check specific rule categories
akao check structure                    # Check structure rules only
akao check interface                    # Check interface consistency
akao check security                     # Check security rules
akao check testing                      # Check test coverage rules
akao check --category automation        # Check automation rules
akao check --rule class_separation      # Check specific rule
akao check --philosophy isolation       # Check philosophy compliance
akao check --fix                        # Auto-fix violations
akao check --dry-run                    # Show what would be fixed
```

### 5. **`akao trace`** - Violation Traceability

```bash
# Trace violation details
akao trace akao:rule::structure:class_separation:v1:violation:src/main.cpp:42
akao trace --rule structure             # Trace all structure violations
akao trace --file src/main.cpp          # Trace all violations in file
akao trace --philosophy isolation       # Trace philosophy violations
akao trace --export trace-report.yaml   # Export trace to file
akao trace --graph                      # Show violation dependency graph
```

---

## 🏗️ **BUILD & DEPLOYMENT COMMANDS**

### 6. **`akao build`** - Build Duality Support

```bash
# Build with environment duality
akao build                              # Build with default mode
akao build --mode development           # Development mode build
akao build --mode production            # Production mode build
akao build --target all                 # Build all targets
akao build --adapter cmake              # Use CMake adapter
akao build --adapter make               # Use Make adapter
akao build --watch                      # Watch mode for development
akao build --validate                   # Validate before build
```

### 7. **`akao test`** - Testing Coverage

```bash
# Comprehensive testing
akao test                               # Run all tests
akao test unit                          # Run unit tests only
akao test integration                   # Run integration tests
akao test compliance                    # Run compliance tests
akao test --coverage                    # Show coverage report
akao test --rule structure              # Test specific rule category
akao test --philosophy isolation        # Test philosophy compliance
akao test --generate-missing            # Generate missing tests
```

---

## 📚 **DOCUMENTATION & METRICS**

### 8. **`akao docs`** - Documentation Generation

```bash
# Auto-generated documentation
akao docs generate                      # Generate all documentation
akao docs serve                         # Serve docs locally
akao docs --rules                       # Generate rules documentation
akao docs --philosophies                # Generate philosophy docs
akao docs --api                         # Generate API documentation
akao docs --examples                    # Generate usage examples
akao docs --export ./docs               # Export to directory
```

### 9. **`akao metrics`** - Observability

```bash
# Metrics and observability
akao metrics collect                    # Collect current metrics
akao metrics analyze                    # Analyze collected metrics
akao metrics export --format prometheus # Export to Prometheus
akao metrics export --format json       # Export to JSON
akao metrics dashboard                  # Launch metrics dashboard
akao metrics --compliance               # Compliance metrics only
akao metrics --performance              # Performance metrics only
```

---

## 🛡️ **SECURITY & CONFIGURATION**

### 10. **`akao security`** - Security Operations

```bash
# Security validation and enforcement
akao security scan                      # Scan for security violations
akao security enforce                   # Enforce security rules
akao security validate                  # Validate security compliance
akao security --behavior explicit       # Check explicit behavior definition
akao security --audit                   # Security audit report
```

### 11. **`akao rules`** - Rule Management

```bash
# Rule management operations
akao rules list                         # List all available rules
akao rules add ./custom-rule.yaml       # Add custom rule
akao rules verify                       # Verify rule consistency
akao rules --category structure         # Rules by category
akao rules --philosophy isolation       # Rules by philosophy
akao rules export --format yaml         # Export rules definition
```

### 12. **`akao config`** - Configuration Management

```bash
# Configuration operations
akao config get                         # Get current configuration
akao config set rule.strict=true        # Set configuration value
akao config reset                       # Reset to defaults
akao config export                      # Export configuration
akao config import ./config.yaml        # Import configuration
akao config validate                    # Validate configuration
```

---

## 🔄 **CLI OUTPUT FORMATS**

### **Default Table Format**
```
✅ AKAO VALIDATION RESULTS
┌─────────────────────────┬──────────┬──────────┬───────────┐
│ Category                │ Rules    │ Passed   │ Failed    │
├─────────────────────────┼──────────┼──────────┼───────────┤
│ Structure               │ 4        │ 3        │ 1         │
│ Interface               │ 3        │ 3        │ 0         │
│ Language                │ 2        │ 2        │ 0         │
│ Security                │ 2        │ 2        │ 0         │
├─────────────────────────┼──────────┼──────────┼───────────┤
│ TOTAL                   │ 32       │ 31       │ 1         │
└─────────────────────────┴──────────┴──────────┴───────────┘
```

### **JSON Format** (`--format json`)
```json
{
  "akao_validation": {
    "version": "1.0.0",
    "timestamp": "2025-07-05T18:30:00Z",
    "project": "/home/user/my-project",
    "summary": {
      "total_rules": 32,
      "passed": 31,
      "failed": 1,
      "compliance_rate": 96.875
    },
    "violations": [
      {
        "id": "akao:rule::structure:class_separation:v1:violation:src/main.cpp:42",
        "rule": "akao:rule::structure:class_separation:v1",
        "philosophy": "akao:philosophy::structure:isolation:v1",
        "file": "src/main.cpp",
        "line": 42,
        "message": "Multiple classes in single file",
        "suggestion": "Move SecondClass to separate file",
        "severity": "MEDIUM",
        "auto_fix_available": false
      }
    ]
  }
}
```

### **YAML Format** (`--format yaml`)
```yaml
akao_validation:
  version: 1.0.0
  timestamp: '2025-07-05T18:30:00Z'
  project: /home/user/my-project
  summary:
    total_rules: 32
    passed: 31
    failed: 1
    compliance_rate: 96.875
  violations:
  - id: akao:rule::structure:class_separation:v1:violation:src/main.cpp:42
    rule: akao:rule::structure:class_separation:v1
    philosophy: akao:philosophy::structure:isolation:v1
    file: src/main.cpp
    line: 42
    message: Multiple classes in single file
    suggestion: Move SecondClass to separate file
    severity: MEDIUM
    auto_fix_available: false
```

---

## 🌐 **INTERFACE PARITY GUARANTEE**

| CLI Command | API Endpoint | Web UI Action | Response Format |
|-------------|--------------|---------------|-----------------|
| `akao validate` | `POST /api/v1/validate` | "Validate" Button | JSON/YAML/Table |
| `akao check structure` | `POST /api/v1/check/structure` | "Check Structure" | JSON/YAML/Table |
| `akao trace {id}` | `GET /api/v1/trace/{id}` | Click Violation ID | JSON/YAML/Table |
| `akao generate project` | `POST /api/v1/generate/project` | "Generate Project" | JSON/YAML/Table |
| `akao metrics collect` | `POST /api/v1/metrics/collect` | "Collect Metrics" | JSON/YAML/Table |

---

## 🚀 **UNIVERSAL EXAMPLES**

### **Validate Any Project**
```bash
# Works on ANY project type
akao validate /path/to/cpp-project       # C++ project
akao validate /path/to/rust-project      # Rust project  
akao validate /path/to/python-project    # Python project
akao validate /path/to/javascript-app    # JavaScript app
akao validate /path/to/akao-itself       # Self-validation
```

### **Generate Any Universe**
```bash
# Generate infinite possibilities
akao generate project --lang any         # Any language
akao generate framework --type universal # Universal framework
akao generate language-adapter --lang new # New language support
akao generate platform --target any      # Any platform
```

### **Cross-Language Validation**
```bash
# Validate projects with multiple languages
akao validate --scope cpp-core           # C++ core only
akao validate --scope rust-bindings      # Rust bindings only  
akao validate --scope python-wrappers    # Python wrappers only
akao validate --scope all-languages      # All language scopes
```

---

## 🏁 **CLI IMPLEMENTATION REQUIREMENTS**

### **Technical Requirements**
1. ✅ **Pure C++ implementation** (no external dependencies)
2. ✅ **Built-in YAML parser** for configuration
3. ✅ **Interface parity** with API and Web UI
4. ✅ **Universal validation** capability
5. ✅ **Formal logic integration** (Datalog/Prolog)
6. ✅ **Complete traceability** with violation IDs
7. ✅ **Self-validation** capability
8. ✅ **Universe generation** support

### **Philosophy Compliance**
- ✅ **Interface Consistency**: CLI = API = Web
- ✅ **Universal Validation**: Works on any project
- ✅ **Traceability**: Every violation traceable
- ✅ **Rule Governance**: CLI validates itself
- ✅ **Structure Enforcement**: One class per command
- ✅ **Documentation Generation**: Auto-generated help
- ✅ **Testing Coverage**: CLI fully tested
- ✅ **Security Explicit**: Security operations explicit

**The Akao CLI: Where infinite validation meets universal compliance!** 🌟
