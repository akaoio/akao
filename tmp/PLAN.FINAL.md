# Akao Framework Final Implementation Plan

> **Single-source specification for implementing the complete Akao framework in C++ with full automation, security, and consistency enforcement**

---

## 📋 Executive Summary

This document is the **canonical implementation specification** for the Akao framework - a rule-based C++ system for project structure enforcement, build management, automation, and documentation generation. This plan is designed for **single-session code generation** where GitHub Copilot can generate the complete working system without additional clarification.

**Implementation Requirements:**
- Pure C++ implementation (C++17 or later) 
- Static linking preferred, minimal runtime dependencies
- Cross-platform build support (Linux x86_64 + aarch64)
- Universal validation operational from day one
- Complete automation and CI/CD integration
- All philosophical principles enforced through code
- Zero ambiguity, zero placeholder content, zero contradictions

**Cross-Document Consistency Verified:**
- CLI commands match [README.md](./README.md) exactly
- All 8 components from [ARCHITECTURE.md](./ARCHITECTURE.md) implemented
- All principles from [PHILOSOPHY.md](./PHILOSOPHY.md) technically enforced
- All improvements from [DESIGN_DOCS_REFACTOR_SUMMARY.md](../DESIGN_DOCS_REFACTOR_SUMMARY.md) integrated

---

## 📁 Complete Project Structure

```
akao/
├── .akao/                    # Framework runtime configuration
│   ├── config.json          # Main configuration file
│   ├── pipeline.yaml        # CI/CD pipeline configuration
│   ├── profiles/             # Language-specific rule profiles
│   │   ├── cpp.json         # C++ rules and build settings
│   │   ├── js.json          # JavaScript rules and build settings
│   │   └── rust.json        # Rust rules and build settings
│   ├── rulesets/             # RuleSet definitions and inheritance
│   │   ├── core.yaml        # Core framework RuleSet
│   │   ├── cpp.yaml         # C++ language RuleSet
│   │   ├── security.yaml    # Security-focused RuleSet
│   │   ├── performance.yaml # Performance RuleSet
│   │   └── custom/          # Project-specific RuleSets
│   │       ├── api.yaml     # API development RuleSet
│   │       └── database.yaml # Database integration RuleSet
│   ├── features/             # Feature management
│   │   ├── installed.json   # List of installed features
│   │   ├── dependencies.json # Feature dependency graph
│   │   └── cache/           # Feature download cache
│   ├── registry/             # External registries
│   │   ├── official.json    # Official Akao registry
│   │   └── custom.json      # Custom registries
│   ├── trace.json           # Audit log and violation trace
│   ├── audit.json           # System audit and compliance metrics
│   ├── coverage.json        # Test coverage report
│   └── cache/               # Build and validation cache
│       ├── rules.cache      # Compiled rule cache
│       └── deps.cache       # Dependency resolution cache
├── .build/                   # Build outputs and artifacts
│   ├── dev/                 # Development builds (hot reload)
│   │   ├── akao             # Development binary
│   │   └── objects/         # Object files
│   ├── prod/                # Production builds (optimized)
│   │   ├── akao             # Production binary
│   │   └── hashes/          # File integrity hashes
│   ├── meta.json            # Build metadata and dependencies
│   └── hash.json            # Reproducible build verification
├── .github/                  # Auto-generated CI/CD workflows
│   ├── workflows/           # GitHub Actions workflows
│   │   ├── ci.yml           # Continuous integration
│   │   ├── cd.yml           # Continuous deployment
│   │   └── validate.yml     # Universal validation workflow
│   └── templates/           # Workflow templates
├── docs/                     # Auto-generated documentation
│   ├── README.md            # Generated project documentation
│   ├── RULES.md             # Generated rule documentation
│   ├── PRINCIPLES.md        # Generated principle documentation
│   ├── FEATURES.md          # Generated feature documentation
│   └── api/                 # Generated API documentation
│       ├── core.md          # Core framework API
│       ├── cli.md           # CLI interface API
│       ├── automation.md    # Automation system API
│       ├── project.md       # Project management API
│       └── feature.md       # Feature management API
├── rules/                    # Rule definitions and enforcement
│   ├── core.json            # Core framework rules
│   ├── structure.json       # Project structure rules
│   ├── naming.json          # Naming convention rules
│   ├── testing.json         # Testing requirement rules
│   ├── documentation.json   # Documentation rules
│   ├── security.json        # Security and sandboxing rules
│   └── performance.json     # Performance requirement rules
├── templates/               # Project and feature templates
│   ├── projects/            # Project initialization templates
│   │   ├── cpp/             # C++ project template
│   │   ├── web/             # Web project template
│   │   └── library/         # Library project template
│   └── features/            # Feature templates
│       ├── api/             # API feature template
│       ├── database/        # Database feature template
│       └── auth/            # Authentication feature template
├── src/                      # Source code (1 class per folder)
│   ├── main.cpp             # Application entry point
│   ├── core/                # Core framework components
│   │   ├── config/          # Configuration management
│   │   │   ├── config.cpp
│   │   │   └── config.hpp
│   │   ├── filesystem/      # File system operations
│   │   │   ├── filesystem.cpp
│   │   │   └── filesystem.hpp
│   │   ├── trace/           # Audit and trace logging
│   │   │   ├── trace.cpp
│   │   │   └── trace.hpp
│   │   └── plugin/          # Plugin system core
│   │       ├── plugin.cpp
│   │       └── plugin.hpp
│   ├── rule/                # Rule engine system
│   │   ├── parser/          # Rule file parser
│   │   │   ├── parser.cpp
│   │   │   └── parser.hpp
│   │   ├── validator/       # Universal validation engine
│   │   │   ├── validator.cpp
│   │   │   └── validator.hpp
│   │   ├── registry/        # Rule discovery and management
│   │   │   ├── registry.cpp
│   │   │   └── registry.hpp
│   │   ├── ruleset/         # RuleSet management and inheritance
│   │   │   ├── ruleset.cpp
│   │   │   └── ruleset.hpp
│   │   └── reporter/        # Violation reporting
│   │       ├── reporter.cpp
│   │       └── reporter.hpp
│   ├── graph/               # Graph generation system
│   │   ├── generator/       # Graph generation engine
│   │   │   ├── generator.cpp
│   │   │   └── generator.hpp
│   │   ├── exporter/        # Multi-format graph export
│   │   │   ├── exporter.cpp
│   │   │   └── exporter.hpp
│   │   └── analyzer/        # Graph analysis and metrics
│   │       ├── analyzer.cpp
│   │       └── analyzer.hpp
│   ├── build/               # Build engine system
│   │   ├── manager/         # Build target management
│   │   │   ├── manager.cpp
│   │   │   └── manager.hpp
│   │   ├── watcher/         # File change monitoring
│   │   │   ├── watcher.cpp
│   │   │   └── watcher.hpp
│   │   ├── hasher/          # Reproducible build hashing
│   │   │   ├── hasher.cpp
│   │   │   └── hasher.hpp
│   │   └── graph/           # Dependency graph resolution
│   │       ├── graph.cpp
│   │       └── graph.hpp
│   ├── docgen/              # Documentation generation system
│   │   ├── engine/          # Template-based generation engine
│   │   │   ├── engine.cpp
│   │   │   └── engine.hpp
│   │   ├── parser/          # Code parsing for documentation
│   │   │   ├── parser.cpp
│   │   │   └── parser.hpp
│   │   ├── mapper/          # Rule-to-documentation mapping
│   │   │   ├── mapper.cpp
│   │   │   └── mapper.hpp
│   │   └── updater/         # Auto-update coordination
│   │       ├── updater.cpp
│   │       └── updater.hpp
│   ├── cli/                 # Command line interface system
│   │   ├── parser/          # Command parsing and validation
│   │   │   ├── parser.cpp
│   │   │   └── parser.hpp
│   │   ├── controller/      # Unified command execution
│   │   │   ├── controller.cpp
│   │   │   └── controller.hpp
│   │   ├── interactive/     # Terminal UI interface
│   │   │   ├── interactive.cpp
│   │   │   └── interactive.hpp
│   │   └── web/             # Web UI backend
│   │       ├── web.cpp
│   │       └── web.hpp
│   ├── project/             # Project management system
│   │   ├── template/        # Template engine
│   │   │   ├── template.cpp
│   │   │   └── template.hpp
│   │   ├── initializer/     # Project initialization
│   │   │   ├── initializer.cpp
│   │   │   └── initializer.hpp
│   │   └── manager/         # Project lifecycle management
│   │       ├── manager.cpp
│   │       └── manager.hpp
│   ├── feature/             # Feature management system
│   │   ├── manager/         # Feature lifecycle management
│   │   │   ├── manager.cpp
│   │   │   └── manager.hpp
│   │   ├── registry/        # External registry client
│   │   │   ├── registry.cpp
│   │   │   └── registry.hpp
│   │   ├── resolver/        # Dependency resolution
│   │   │   ├── resolver.cpp
│   │   │   └── resolver.hpp
│   │   └── sandbox/         # Security sandboxing
│   │       ├── sandbox.cpp
│   │       └── sandbox.hpp
│   ├── automation/          # Automation and CI/CD system
│   │   ├── pipeline/        # Pipeline configuration generator
│   │   │   ├── pipeline.cpp
│   │   │   └── pipeline.hpp
│   │   ├── workflow/        # GitHub Actions workflow generator
│   │   │   ├── workflow.cpp
│   │   │   └── workflow.hpp
│   │   ├── generator/       # Template-based generation
│   │   │   ├── generator.cpp
│   │   │   └── generator.hpp
│   │   └── deployer/        # Deployment configuration
│   │       ├── deployer.cpp
│   │       └── deployer.hpp
│   └── metrics/             # Metrics and compliance system
│       ├── collector/       # Metrics collection
│       │   ├── collector.cpp
│       │   └── collector.hpp
│       ├── scorer/          # Compliance scoring
│       │   ├── scorer.cpp
│       │   └── scorer.hpp
│       └── reporter/        # Metrics reporting
│           ├── reporter.cpp
│           └── reporter.hpp
├── include/                  # Header files (mirrors src structure)
│   ├── akao/                # Main namespace headers
│   │   ├── core/            # Core component headers
│   │   ├── rule/            # Rule engine headers
│   │   ├── build/           # Build engine headers
│   │   ├── docgen/          # Documentation headers
│   │   ├── cli/             # CLI system headers
│   │   ├── project/         # Project management headers
│   │   ├── feature/         # Feature management headers
│   │   ├── automation/      # Automation system headers
│   │   └── metrics/         # Metrics system headers
│   └── akao.hpp             # Main framework header
├── tests/                    # Comprehensive test suite
│   ├── unit/                # Unit tests (per component)
│   │   ├── core/            # Core component tests
│   │   ├── rule/            # Rule engine tests
│   │   ├── build/           # Build engine tests
│   │   ├── docgen/          # Documentation tests
│   │   ├── cli/             # CLI system tests
│   │   ├── project/         # Project management tests
│   │   ├── feature/         # Feature management tests
│   │   ├── automation/      # Automation system tests
│   │   └── metrics/         # Metrics system tests
│   ├── integration/         # Integration tests
│   │   ├── full_workflow.cpp    # Complete workflow tests
│   │   ├── cli_integration.cpp  # CLI integration tests
│   │   ├── build_integration.cpp # Build system integration
│   │   ├── docgen_flow.cpp      # End-to-end documentation
│   │   ├── project_init.cpp     # Project initialization flow
│   │   ├── feature_mgmt.cpp     # Feature management flow
│   │   └── automation_flow.cpp  # CI/CD automation flow
│   ├── principles/          # Principle validation tests
│   │   ├── principle_validation.cpp  # Universal validation principles
│   │   ├── structure_rules.cpp      # Structure enforcement
│   │   ├── interface_parity.cpp     # CLI=Web=TUI consistency
│   │   ├── feature_security.cpp     # External feature sandboxing
│   │   └── trace_audit.cpp          # Traceability verification
│   └── fixtures/            # Test data and fixtures
│       ├── valid_projects/  # Valid project structures
│       └── invalid_projects/ # Invalid structures for testing
├── features/                 # Built-in project features
│   ├── api/                 # REST API feature
│   │   ├── feature.json     # Feature manifest
│   │   └── templates/       # Feature templates
│   ├── database/            # Database integration feature
│   │   ├── feature.json
│   │   └── templates/
│   └── auth/                # Authentication feature
│       ├── feature.json
│       └── templates/
├── plugins/                  # Optional extensions
│   ├── wasm/                # WebAssembly build target
│   │   ├── plugin.json      # Plugin manifest
│   │   └── src/             # Plugin source code
│   ├── js/                  # JavaScript bindings
│   │   ├── plugin.json
│   │   └── src/
│   └── android/             # Android NDK target
│       ├── plugin.json
│       └── src/
├── CMakeLists.txt           # CMake build configuration
├── .gitignore               # Git ignore patterns
└── LICENSE                  # MIT license file
```

---

## 🔍 Global Rule Identifier (GID) System

### GID Format Specification

**Every rule in the Akao framework must have a globally unique identifier (GID)** that ensures consistent reference, traceability, and management across all system components.

**GID Structure:**
```
akao:rule::<category>:<name>:v<version>
```

**Format Components:**
- `akao:rule::` - Fixed namespace prefix for all Akao rules
- `<category>` - Rule category (cpp, structure, naming, testing, documentation, security, performance)
- `<name>` - Specific rule identifier using snake_case
- `v<version>` - Semantic version starting from v1

**Examples:**
```
akao:rule::cpp:naming:snake_case:v1
akao:rule::structure:one_class_per_folder:v1
akao:rule::testing:coverage_minimum:v2
akao:rule::documentation:auto_generation:v1
akao:rule::security:sandbox_external_features:v1
akao:rule::performance:startup_time_limit:v1
akao:rule::naming:file_extensions:v1
akao:rule::core:universal_validation:v1
```

### GID Integration Architecture

**Rule File Integration:**
All rule definition files (JSON, YAML, TOML) must include GID in their structure:

```json
// rules/naming.json
{
  "rules": [
    {
      "gid": "akao:rule::naming:snake_case:v1",
      "name": "Snake Case Variables",
      "description": "All variable names must use snake_case convention",
      "category": "naming",
      "severity": "error",
      "pattern": "^[a-z][a-z0-9_]*$",
      "applies_to": ["variables", "functions"],
      "examples": {
        "valid": ["user_name", "calculate_total"],
        "invalid": ["userName", "calculateTotal"]
      }
    }
  ]
}
```

```yaml
# rules/structure.yaml
rules:
  - gid: "akao:rule::structure:one_class_per_folder:v1"
    name: "One Class Per Folder"
    description: "Each folder must contain exactly one class implementation"
    category: "structure"
    severity: "error"
    enforcement:
      max_classes_per_folder: 1
      required_files: ["*.hpp", "*.cpp"]
    exceptions:
      - "templates/"
      - "tests/"
```

```toml
# rules/testing.toml
[[rules]]
gid = "akao:rule::testing:coverage_minimum:v2"
name = "Minimum Test Coverage"
description = "All source files must have minimum 80% test coverage"
category = "testing"
severity = "warning"
threshold = 0.80
applies_to = ["src/**/*.cpp"]
excludes = ["src/main.cpp"]
```

**Audit and Trace Integration:**
All audit and trace files reference rules by GID:

```json
// .akao/trace.json
{
  "timestamp": "2024-01-01T00:00:00Z",
  "operation": "validate",
  "violations": [
    {
      "gid": "akao:rule::naming:snake_case:v1",
      "file": "src/core/config/ConfigManager.cpp",
      "line": 42,
      "column": 8,
      "message": "Variable name 'configData' violates snake_case convention",
      "suggestion": "Rename to 'config_data'",
      "severity": "error"
    }
  ],
  "rules_applied": [
    "akao:rule::naming:snake_case:v1",
    "akao:rule::structure:one_class_per_folder:v1",
    "akao:rule::testing:coverage_minimum:v2"
  ]
}
```

```json
// .akao/audit.json
{
  "timestamp": "2024-01-01T00:00:00Z",
  "project_compliance": {
    "overall_score": 0.95,
    "rule_compliance": {
      "akao:rule::naming:snake_case:v1": {
        "score": 0.98,
        "violations": 2,
        "total_checks": 100
      },
      "akao:rule::structure:one_class_per_folder:v1": {
        "score": 1.0,
        "violations": 0,
        "total_checks": 45
      },
      "akao:rule::testing:coverage_minimum:v2": {
        "score": 0.90,
        "violations": 3,
        "total_checks": 30
      }
    }
  }
}
```

### CLI Integration with GID Support

**Enhanced CLI Commands:**

```cpp
namespace akao::cli {
class ValidateCommand : public Command {
public:
    CommandResult execute(const CommandRequest& request) override {
        auto project_path = request.parameters.at("path");
        
        // Support GID-specific validation
        if (request.parameters.contains("gid")) {
            auto gid = request.parameters.at("gid");
            return validateSpecificRule(project_path, gid);
        }
        
        // Support category-specific validation
        if (request.parameters.contains("category")) {
            auto category = request.parameters.at("category");
            return validateRuleCategory(project_path, category);
        }
        
        // Default: validate all rules
        return validateAllRules(project_path);
    }
    
private:
    CommandResult validateSpecificRule(const std::string& path, const std::string& gid) {
        auto rule = rule::Registry::getRuleByGID(gid);
        if (!rule) {
            return CommandResult{false, "", {}, {"Rule not found: " + gid}};
        }
        
        auto violations = rule->validate(path);
        return CommandResult{
            violations.empty(),
            formatViolations(violations, gid),
            {{"gid", gid}, {"violations", std::to_string(violations.size())}},
            {}
        };
    }
};
}
```

**CLI Usage Examples:**

```bash
# Validate specific rule by GID
akao validate --gid="akao:rule::naming:snake_case:v1"

# Validate all rules in a category
akao validate --category="naming"

# Audit specific rule compliance
akao audit --gid="akao:rule::testing:coverage_minimum:v2"

# List all rules with their GIDs
akao rule list --format=gid

# Show rule details by GID
akao rule info --gid="akao:rule::structure:one_class_per_folder:v1"

# Disable specific rule by GID (tracked in audit)
akao rule disable --gid="akao:rule::performance:startup_time_limit:v1" --reason="Development mode"
```

### Rule Registry with GID Management

```cpp
namespace akao::rule {
class Registry {
public:
    static std::shared_ptr<Rule> getRuleByGID(const std::string& gid) {
        return rule_cache_[gid];
    }
    
    static std::vector<std::shared_ptr<Rule>> getRulesByCategory(const std::string& category) {
        std::vector<std::shared_ptr<Rule>> rules;
        for (const auto& [gid, rule] : rule_cache_) {
            if (rule->getCategory() == category) {
                rules.push_back(rule);
            }
        }
        return rules;
    }
    
    static std::vector<std::string> getAllGIDs() {
        std::vector<std::string> gids;
        for (const auto& [gid, rule] : rule_cache_) {
            gids.push_back(gid);
        }
        return gids;
    }
    
    static bool registerRule(std::shared_ptr<Rule> rule) {
        auto gid = rule->getGID();
        
        // Validate GID format
        if (!isValidGID(gid)) {
            return false;
        }
        
        // Check for duplicates
        if (rule_cache_.contains(gid)) {
            return false;
        }
        
        rule_cache_[gid] = rule;
        return true;
    }
    
private:
    static std::map<std::string, std::shared_ptr<Rule>> rule_cache_;
    
    static bool isValidGID(const std::string& gid) {
        // Validate format: akao:rule::<category>:<name>:v<version>
        std::regex pattern("^akao:rule::[a-z_]+:[a-z_]+:v\\d+$");
        return std::regex_match(gid, pattern);
    }
};

class Rule {
public:
    virtual ~Rule() = default;
    virtual std::string getGID() const = 0;
    virtual std::string getCategory() const = 0;
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::vector<Violation> validate(const std::string& project_path) = 0;
};
}
```

### Multi-Format Rule Support

**JSON Format:**
```json
{
  "gid": "akao:rule::cpp:naming:snake_case:v1",
  "metadata": {
    "name": "Snake Case Variables",
    "description": "All variable names must use snake_case convention",
    "category": "cpp",
    "severity": "error",
    "version": "v1"
  },
  "validation": {
    "pattern": "^[a-z][a-z0-9_]*$",
    "applies_to": ["variables", "functions"],
    "excludes": ["macros", "constants"]
  }
}
```

**YAML Format:**
```yaml
gid: "akao:rule::structure:one_class_per_folder:v1"
metadata:
  name: "One Class Per Folder"
  description: "Each folder must contain exactly one class implementation"
  category: "structure"
  severity: "error"
  version: "v1"
validation:
  max_classes_per_folder: 1
  required_files: ["*.hpp", "*.cpp"]
  exceptions: ["templates/", "tests/"]
```

**TOML Format:**
```toml
gid = "akao:rule::testing:coverage_minimum:v2"

[metadata]
name = "Minimum Test Coverage"
description = "All source files must have minimum 80% test coverage"
category = "testing"
severity = "warning"
version = "v2"

[validation]
threshold = 0.80
applies_to = ["src/**/*.cpp"]
excludes = ["src/main.cpp"]
```

### GID-Based Traceability Implementation

```cpp
namespace akao::trace {
class Logger {
public:
    static void logRuleViolation(const std::string& gid, const Violation& violation) {
        auto entry = TraceEntry{
            .timestamp = getCurrentTimestamp(),
            .type = "rule_violation",
            .gid = gid,
            .file = violation.file_path,
            .line = violation.line_number,
            .message = violation.message,
            .severity = violation.severity
        };
        
        appendToTrace(entry);
        updateAuditMetrics(gid, false);
    }
    
    static void logRuleCompliance(const std::string& gid, const std::string& file_path) {
        auto entry = TraceEntry{
            .timestamp = getCurrentTimestamp(),
            .type = "rule_compliance",
            .gid = gid,
            .file = file_path,
            .message = "Rule compliance verified",
            .severity = "info"
        };
        
        appendToTrace(entry);
        updateAuditMetrics(gid, true);
    }
    
private:
    static void updateAuditMetrics(const std::string& gid, bool compliant) {
        auto audit_data = loadAuditData();
        if (!audit_data.rule_compliance.contains(gid)) {
            audit_data.rule_compliance[gid] = ComplianceMetrics{};
        }
        
        auto& metrics = audit_data.rule_compliance[gid];
        metrics.total_checks++;
        if (!compliant) {
            metrics.violations++;
        }
        metrics.score = 1.0 - (static_cast<double>(metrics.violations) / metrics.total_checks);
        
        saveAuditData(audit_data);
    }
};

struct ComplianceMetrics {
    double score = 1.0;
    int violations = 0;
    int total_checks = 0;
};
}
```

---

## 📦 RuleSet System

### RuleSet Architecture

**RuleSets enable logical grouping of rules using GID collections** for organizational efficiency, inheritance hierarchies, and granular validation control. RuleSets are defined in `.akao/rulesets/` directory and support nested inheritance.

**RuleSet Structure:**
```
.akao/rulesets/
├── core.yaml               # Core framework RuleSet
├── cpp.yaml               # C++ language RuleSet  
├── security.yaml          # Security-focused RuleSet
├── performance.yaml       # Performance RuleSet
└── custom/                # Project-specific RuleSets
    ├── api.yaml           # API development RuleSet
    └── database.yaml      # Database integration RuleSet
```

### RuleSet Definition Format

```yaml
# .akao/rulesets/cpp.yaml
name: "cpp"
version: "v1"
description: "C++ development standards and best practices"
parent: "core"  # Inherits from core RuleSet

rules:
  - "akao:rule:cpp:naming:snake_case:v1"
  - "akao:rule:cpp:structure:one_class_per_folder:v1" 
  - "akao:rule:cpp:testing:coverage_threshold:v2"
  - "akao:rule:cpp:documentation:header_comments:v1"

includes:
  - "security"  # Include all rules from security RuleSet
  
excludes:
  - "akao:rule:core:build:incremental:v1"  # Override parent rule

metadata:
  author: "Akao Team"
  tags: ["cpp", "language", "development"]
  last_updated: "2024-01-15T10:30:00Z"
```

```json
// .akao/rulesets/security.json (JSON format)
{
  "name": "security",
  "version": "v1", 
  "description": "Security and sandboxing enforcement",
  "rules": [
    "akao:rule:security:sandboxing:external_features:v1",
    "akao:rule:security:validation:input_sanitization:v1",
    "akao:rule:security:dependencies:vulnerability_scan:v1"
  ],
  "metadata": {
    "author": "Security Team",
    "tags": ["security", "compliance"],
    "severity": "critical"
  }
}
```

### RuleSet Inheritance and Resolution

```cpp
namespace akao::ruleset {
class RuleSetResolver {
public:
    static std::vector<std::string> resolveRuleSet(const std::string& name) {
        auto ruleset = loadRuleSet(name);
        std::vector<std::string> resolved_gids;
        
        // Resolve parent RuleSets first (depth-first)
        if (ruleset.parent.has_value()) {
            auto parent_gids = resolveRuleSet(ruleset.parent.value());
            resolved_gids.insert(resolved_gids.end(), parent_gids.begin(), parent_gids.end());
        }
        
        // Add included RuleSets
        for (const auto& include : ruleset.includes) {
            auto included_gids = resolveRuleSet(include);
            resolved_gids.insert(resolved_gids.end(), included_gids.begin(), included_gids.end());
        }
        
        // Add direct rules
        resolved_gids.insert(resolved_gids.end(), ruleset.rules.begin(), ruleset.rules.end());
        
        // Remove excluded rules
        for (const auto& exclude_gid : ruleset.excludes) {
            resolved_gids.erase(
                std::remove(resolved_gids.begin(), resolved_gids.end(), exclude_gid),
                resolved_gids.end()
            );
        }
        
        // Remove duplicates while preserving order
        return removeDuplicates(resolved_gids);
    }
    
    static std::vector<std::string> listAvailableRuleSets() {
        return filesystem::listFiles(".akao/rulesets/", {".yaml", ".yml", ".json"});
    }
    
    static RuleSetInfo getRuleSetInfo(const std::string& name) {
        auto ruleset = loadRuleSet(name);
        auto resolved_gids = resolveRuleSet(name);
        
        return RuleSetInfo{
            .name = ruleset.name,
            .version = ruleset.version,
            .description = ruleset.description,
            .total_rules = resolved_gids.size(),
            .direct_rules = ruleset.rules.size(),
            .inherited_rules = resolved_gids.size() - ruleset.rules.size(),
            .parent = ruleset.parent,
            .includes = ruleset.includes,
            .excludes = ruleset.excludes
        };
    }
};
}
```

### CLI Integration for RuleSets

```bash
# RuleSet Management
akao ruleset list                    # List all available RuleSets
akao ruleset info <name>             # Show RuleSet details and inheritance
akao ruleset validate <name>         # Validate RuleSet definition and dependencies
akao ruleset create <name>           # Create new RuleSet interactively

# RuleSet-based Operations  
akao validate --ruleset=cpp          # Validate using C++ RuleSet
akao validate --ruleset=security     # Validate using security RuleSet
akao audit --ruleset=performance     # Audit performance compliance
akao docgen --ruleset=core           # Generate docs for core RuleSet rules
```

---

## 📊 Graph Generation System

### Graph Architecture

**The graph system provides visual representation and analysis capabilities** for rule dependencies, RuleSet relationships, project structure, feature dependencies, and validation flows. All graphs are generated from live system state and support multiple output formats.

**Graph Types:**
- **Rule Dependency Graph**: Shows GID relationships and inheritance
- **RuleSet Relationship Graph**: Displays RuleSet inheritance and includes/excludes
- **Project Structure Graph**: Visualizes folder structure and class relationships  
- **Feature Dependency Graph**: Maps feature dependencies and compatibility
- **Validation Flow Graph**: Shows validation execution paths and rule evaluation order
- **Audit Compliance Graph**: Displays compliance metrics and violation patterns

### Graph Generation Implementation

```cpp
namespace akao::graph {
class GraphGenerator {
public:
    enum class GraphType {
        RULES,
        RULESETS, 
        PROJECT_STRUCTURE,
        FEATURES,
        VALIDATION_FLOW,
        AUDIT_COMPLIANCE
    };
    
    enum class OutputFormat {
        DOT,        // Graphviz DOT format
        SVG,        // Scalable Vector Graphics
        JSON,       // Structured graph data
        PNG         // Raster image (requires graphviz)
    };
    
    static GraphData generateGraph(GraphType type, const GraphConfig& config = {}) {
        switch (type) {
            case GraphType::RULES:
                return generateRuleDependencyGraph(config);
            case GraphType::RULESETS:
                return generateRuleSetGraph(config);
            case GraphType::PROJECT_STRUCTURE:
                return generateProjectStructureGraph(config);
            case GraphType::FEATURES:
                return generateFeatureDependencyGraph(config);
            case GraphType::VALIDATION_FLOW:
                return generateValidationFlowGraph(config);
            case GraphType::AUDIT_COMPLIANCE:
                return generateAuditComplianceGraph(config);
        }
    }
    
    static void exportGraph(const GraphData& graph, OutputFormat format, const std::string& output_path) {
        switch (format) {
            case OutputFormat::DOT:
                exportToDot(graph, output_path);
                break;
            case OutputFormat::SVG:
                exportToSvg(graph, output_path);
                break;
            case OutputFormat::JSON:
                exportToJson(graph, output_path);
                break;
            case OutputFormat::PNG:
                exportToPng(graph, output_path);
                break;
        }
    }

private:
    static GraphData generateRuleDependencyGraph(const GraphConfig& config) {
        GraphData graph;
        graph.type = "rule_dependency";
        
        auto rules = rule::Registry::getAllRules();
        for (const auto& rule : rules) {
            auto gid = rule->getGID();
            graph.nodes.emplace_back(Node{
                .id = gid,
                .label = rule->getName(),
                .type = "rule",
                .metadata = {
                    {"category", rule->getCategory()},
                    {"version", rule->getVersion()},
                    {"severity", rule->getSeverity()}
                }
            });
            
            // Add dependency edges
            for (const auto& dep_gid : rule->getDependencies()) {
                graph.edges.emplace_back(Edge{
                    .from = dep_gid,
                    .to = gid,
                    .type = "depends_on",
                    .weight = 1.0
                });
            }
        }
        
        return graph;
    }
    
    static GraphData generateRuleSetGraph(const GraphConfig& config) {
        GraphData graph;
        graph.type = "ruleset_relationship";
        
        auto rulesets = ruleset::RuleSetResolver::listAvailableRuleSets();
        for (const auto& name : rulesets) {
            auto info = ruleset::RuleSetResolver::getRuleSetInfo(name);
            graph.nodes.emplace_back(Node{
                .id = name,
                .label = info.description,
                .type = "ruleset",
                .metadata = {
                    {"version", info.version},
                    {"total_rules", std::to_string(info.total_rules)},
                    {"direct_rules", std::to_string(info.direct_rules)}
                }
            });
            
            // Add inheritance edges
            if (info.parent.has_value()) {
                graph.edges.emplace_back(Edge{
                    .from = info.parent.value(),
                    .to = name,
                    .type = "inherits",
                    .weight = 1.0
                });
            }
            
            // Add include edges
            for (const auto& include : info.includes) {
                graph.edges.emplace_back(Edge{
                    .from = include,
                    .to = name,
                    .type = "includes",
                    .weight = 0.5
                });
            }
        }
        
        return graph;
    }
};

struct Node {
    std::string id;
    std::string label;
    std::string type;
    std::map<std::string, std::string> metadata;
};

struct Edge {
    std::string from;
    std::string to;
    std::string type;
    double weight;
};

struct GraphData {
    std::string type;
    std::vector<Node> nodes;
    std::vector<Edge> edges;
    std::map<std::string, std::string> metadata;
};
}
```

### CLI Integration for Graph Generation

```bash
# Graph Generation Commands
akao graph --type=rules --format=svg --output=rules.svg        # Rule dependency graph
akao graph --type=rulesets --format=dot --output=rulesets.dot  # RuleSet relationships
akao graph --type=project --format=json                        # Project structure (stdout)
akao graph --type=features --format=png --output=features.png  # Feature dependencies
akao graph --type=validation --format=svg                      # Validation flow graph
akao graph --type=audit --format=dot --ruleset=security        # Audit compliance graph

# Graph Integration with Other Commands
akao docgen --include-graphs                                   # Embed graphs in documentation
akao audit --graph-output=compliance.svg                       # Generate compliance graph
akao validate --graph-output=validation_flow.dot               # Show validation execution graph
```

### Graph Integration Points

**Documentation Integration**: Generated graphs automatically embedded in `akao docgen` output:
```markdown
# Rules Overview
![Rule Dependency Graph](graphs/rules.svg)

# RuleSet Inheritance
![RuleSet Graph](graphs/rulesets.svg)
```

**Web UI Integration**: Graph viewer mode with interactive navigation:
```cpp
namespace akao::web {
class GraphController {
public:
    void serveGraphViewer(const HttpRequest& req, HttpResponse& res) {
        auto type = req.getParam("type");
        auto format = req.getParam("format", "json");
        
        auto graph = graph::GraphGenerator::generateGraph(parseGraphType(type));
        res.setContentType("application/json");
        res.setBody(graph::GraphGenerator::exportToJson(graph));
    }
};
}
```

**Audit Integration**: Compliance graphs show rule violations and trends:
```cpp
auto compliance_graph = graph::GraphGenerator::generateGraph(
    graph::GraphGenerator::GraphType::AUDIT_COMPLIANCE,
    {{"ruleset", "security"}, {"timerange", "7d"}}
);
```

---

## 🎯 Implementation Phases

### Phase 1: Core Framework Foundation
**Establish core infrastructure and universal validation**:

1. **Core Framework Components**
   - `src/core/config/` - Configuration management with `.akao/config.json` support
   - `src/core/filesystem/` - Safe file operations with path validation
   - `src/core/trace/` - Comprehensive audit logging and trace system
   - `src/core/plugin/` - Plugin loading and management foundation

2. **Rule Engine Foundation**
   - `src/rule/parser/` - Multi-format rule file parser (JSON/YAML/TOML) with GID validation
   - `src/rule/validator/` - Universal validation engine with GID tracking
   - `src/rule/registry/` - GID-based rule discovery and management
   - `src/rule/reporter/` - Violation reporting with GID references and stack traces

3. **CLI System Foundation**
   - `src/cli/parser/` - Command parsing with GID parameter support
   - `src/cli/controller/` - Unified command execution controller
   - Basic command structure for all operations with GID integration

4. **GID System Foundation**
   - GID format validation: `akao:rule::<category>:<name>:v<version>`
   - Rule file GID integration for JSON, YAML, and TOML formats
   - Trace and audit system GID referencing
   - CLI GID-based rule operations (validate, audit, info)

**Success Criteria Phase 1:**
- [ ] Core configuration system loads `.akao/config.json`
- [ ] Universal validation engine can validate basic project structures
- [ ] CLI can parse and execute basic commands
- [ ] Trace system logs all operations with full audit trail and GID references
- [ ] Plugin system can load and validate plugins
- [ ] GID format validation works for all rule categories
- [ ] Rule registry supports GID-based lookup and management

### Phase 2: Universal Validation & Principle Testing
**Implement comprehensive rule system and principle validation**:

1. **Enhanced Rule System**
   - Complete multi-format rule parser (JSON/YAML/TOML) with GID support
   - Rule caching and optimization with GID-based indexing
   - Comprehensive violation reporting with GID references and suggestions
   - Rule dependency resolution using GID relationships

2. **RuleSet System Implementation**
   - `src/rule/ruleset/` - RuleSet management and inheritance engine
   - Multi-format RuleSet definition support (YAML/JSON) in `.akao/rulesets/`
   - RuleSet inheritance, includes, and excludes resolution
   - CLI commands: `ruleset list`, `ruleset info`, `ruleset validate`, `ruleset create`
   - RuleSet-based validation: `validate --ruleset=<name>`, `audit --ruleset=<name>`

3. **Graph Generation System**
   - `src/graph/generator/` - Core graph generation engine for all graph types
   - `src/graph/exporter/` - Multi-format export (DOT, SVG, JSON, PNG)
   - `src/graph/analyzer/` - Graph analysis and metrics calculation
   - Graph types: rules, rulesets, project structure, features, validation flow, audit compliance
   - CLI commands: `graph --type=<type> --format=<format> --output=<path>`

4. **Principle Testing Framework**
   - `tests/principles/principle_validation.cpp` - Core principle tests with GID validation
   - Universal validation that works on any project including Akao
   - Complete integration with all philosophical principles and GID traceability

3. **Metrics Foundation**
   - `src/metrics/collector/` - Metrics collection system with GID-based tracking
   - `src/metrics/scorer/` - Compliance scoring algorithms per GID
   - `src/metrics/reporter/` - Metrics reporting and visualization with GID breakdown

4. **GID System Enhancement**
   - Complete GID-based CLI command support
   - Audit and trace system GID integration
   - Multi-format rule file GID validation
   - GID-based rule versioning and management

**Success Criteria Phase 2:**
- [ ] All 19 core principles from PHILOSOPHY.md are testable and enforced with GIDs
- [ ] Universal validation works on Akao itself with zero violations
- [ ] Compliance scoring provides measurable project health metrics per GID
- [ ] All rule violations include GID, file path, line number, and suggestions
- [ ] Rule caching improves validation performance by >90%
- [ ] GID-based CLI operations (validate, audit, info, disable) fully functional
- [ ] Multi-format rule files (JSON/YAML/TOML) support GID integration
- [ ] Trace and audit files correctly reference all rules by GID
- [ ] RuleSet system supports inheritance, includes, and excludes resolution
- [ ] RuleSet-based validation and audit operations fully functional
- [ ] All graph types (rules, rulesets, project, features, validation, audit) generate correctly
- [ ] Graph export works for all formats (DOT, SVG, JSON, PNG)
- [ ] Graph integration with docgen, audit, and Web UI operational

### Phase 3: Project Management & Templates
**Enable project initialization and feature management**:

1. **Project Template System**
   - `src/project/template/` - Template engine with variable substitution
   - `src/project/initializer/` - Project initialization from templates
   - `src/project/manager/` - Project lifecycle management
   - Built-in templates for C++, web, and library projects

2. **Basic Feature System**
   - `src/feature/manager/` - Feature lifecycle management
   - Feature manifest parsing and validation
   - Local feature installation and management

**Success Criteria Phase 3:**
- [ ] `akao init` creates valid projects from templates
- [ ] `akao feature add/remove/list` commands functional
- [ ] All generated projects pass universal validation
- [ ] Template validation ensures generated projects are rule-compliant
- [ ] Project templates support customization and variable substitution

### Phase 4: Build System & Documentation
**Implement development and production builds with documentation generation**:

1. **Build Engine**
   - `src/build/manager/` - Build target management (dev/prod profiles)
   - `src/build/watcher/` - File change monitoring for hot reload
   - `src/build/hasher/` - Reproducible build verification
   - `src/build/graph/` - Dependency graph resolution

2. **Documentation Generation**
   - `src/docgen/engine/` - Template-based documentation generation
   - `src/docgen/parser/` - Code parsing for automatic documentation
   - `src/docgen/mapper/` - Rule-to-documentation mapping
   - `src/docgen/updater/` - Auto-update coordination

**Success Criteria Phase 4:**
- [ ] `akao build --dev` enables hot reload development
- [ ] `akao build --prod` produces reproducible artifacts with hash verification
- [ ] `akao docgen` generates RULES.md, PRINCIPLES.md, FEATURES.md
- [ ] Documentation auto-updates when rules or code change
- [ ] Build caching reduces rebuild times by >80%

### Phase 5: External Feature Management
**Implement external registry and secure feature installation**:

1. **External Registry System**
   - `src/feature/registry/` - External registry client with caching
   - `src/feature/resolver/` - Dependency resolution with conflict handling
   - Registry protocol implementation for feature discovery

2. **Feature Security System**
   - `src/feature/sandbox/` - Security sandboxing for external features
   - Feature signature verification and validation
   - Permission system for feature capabilities

**Success Criteria Phase 5:**
- [ ] `akao registry add/list` manages external registries
- [ ] `akao install/update/uninstall` manages external features
- [ ] All external features run in secure sandboxes
- [ ] Feature dependency resolution handles conflicts gracefully
- [ ] Security validation prevents malicious feature execution

### Phase 6: Automation & CI/CD
**Implement complete automation system**:

1. **Automation System**
   - `src/automation/pipeline/` - Pipeline configuration generator
   - `src/automation/workflow/` - GitHub Actions workflow generator
   - `src/automation/generator/` - Template-based generation
   - `src/automation/deployer/` - Deployment configuration

2. **CI/CD Integration**
   - `.akao/pipeline.yaml` configuration format specification
   - `.github/workflows/` auto-generation with full integration
   - Integration with all core operations (validate, test, build, deploy)

**Success Criteria Phase 6:**
- [ ] `akao pipeline generate` creates complete CI/CD configuration
- [ ] `akao workflow create/update` manages GitHub Actions workflows
- [ ] Generated pipelines include validate, test, build, deploy stages
- [ ] Automation integrates seamlessly with all core Akao operations
- [ ] Pipeline configuration validates before generation

### Phase 7: Interface Consistency (CLI=Web=TUI)
**Implement unified interface system**:

1. **Interface Consistency Framework**
   - Enhanced `src/cli/controller/` - Unified command execution
   - `src/cli/interactive/` - Terminal UI interface implementation
   - `src/cli/web/` - Web UI backend with REST API

2. **Interface Validation**
   - `tests/principles/interface_parity.cpp` - Validate identical functionality
   - Command abstraction with `CommandRequest`/`CommandResponse` pattern
   - Comprehensive interface testing framework

**Success Criteria Phase 7:**
- [ ] All commands work identically across CLI, Web, and TUI interfaces
- [ ] Interface parity validation passes for all operations
- [ ] Web API provides REST endpoints for all CLI commands
- [ ] TUI provides interactive mode for all operations
- [ ] Interface switching requires zero relearning of workflows

### Phase 8: Plugin System & Extensibility
**Complete plugin architecture and example implementations**:

1. **Enhanced Plugin System**
   - Complete plugin loading, validation, and management
   - Plugin security sandboxing and permission system
   - Plugin testing framework with automated validation

2. **Example Plugin Implementations**
   - `plugins/wasm/` - WebAssembly build target with full integration
   - `plugins/js/` - JavaScript bindings for Node.js integration
   - `plugins/android/` - Android NDK target for mobile development

**Success Criteria Phase 8:**
- [ ] Plugin loading and management fully functional
- [ ] Plugin security sandboxing operational and tested
- [ ] Example plugins (wasm, js, android) working with integration tests
- [ ] Plugin testing framework validates all plugins automatically
- [ ] Plugin system passes comprehensive security audit

### Phase 9: Final Integration & Optimization
**Complete system integration and performance optimization**:

1. **Performance Optimization**
   - Caching systems optimization across all components
   - Build performance improvements and memory optimization
   - Startup time optimization (target: <100ms cold start)

2. **Complete Testing & Validation**
   - All integration tests passing with >95% coverage
   - Performance benchmarks met across all operations
   - Security audits complete for all components

**Success Criteria Phase 9:**
- [ ] All performance requirements met (startup <100ms, validation <1s for medium projects)
- [ ] Complete test coverage >95% across all components
- [ ] All integration tests passing including end-to-end workflows
- [ ] Security audit passes for framework and all plugins
- [ ] Documentation complete, accurate, and auto-updated

---

## 🔧 Critical Implementation Details

### Universal Validation Implementation

**Bootstrap Validation**: Akao applies universal validation to any project, including itself as a standard use case:

```cpp
// Universal validation that works on any project
namespace akao::rule {
class Validator {
public:
    static ValidationResult validateProject(const std::string& project_path) {
        auto config = core::Config::load(project_path);
        auto rules = rule::Registry::loadRules(config.getRulesPath());
        auto violations = std::vector<Violation>();
        
        for (const auto& rule : rules) {
            auto result = rule.validate(project_path);
            if (!result.isValid()) {
                violations.insert(violations.end(), 
                                result.getViolations().begin(), 
                                result.getViolations().end());
            }
        }
        
        return ValidationResult{violations.empty(), violations};
    }
    
    static bool selfValidate() {
        // Akao validates itself using the same universal system
        return validateProject(".").isValid();
    }
};

struct ValidationResult {
    bool isValid;
    std::vector<Violation> violations;
};

struct Violation {
    std::string rule_name;
    std::string file_path;
    int line_number;
    std::string message;
    std::string suggestion;
    std::string stack_trace;
};
}
```

### CLI Command Specification

**Complete Command Interface** (matching README.md exactly):

```cpp
namespace akao::cli {
class CommandRegistry {
public:
    void registerAllCommands() {
        // Project Management
        registerCommand("init", &ProjectInitCommand::execute);
        registerCommand("init --template=cpp", &ProjectInitCommand::executeWithTemplate);
        registerCommand("init --template=web", &ProjectInitCommand::executeWithTemplate);
        
        // Feature Management
        registerCommand("feature add", &FeatureCommand::add);
        registerCommand("feature remove", &FeatureCommand::remove);
        registerCommand("feature list", &FeatureCommand::list);
        registerCommand("feature info", &FeatureCommand::info);
        
        // External Feature Management
        registerCommand("registry add", &RegistryCommand::add);
        registerCommand("registry list", &RegistryCommand::list);
        registerCommand("install", &FeatureInstallCommand::install);
        registerCommand("update", &FeatureInstallCommand::update);
        registerCommand("uninstall", &FeatureInstallCommand::uninstall);
        
        // Core Operations
        registerCommand("validate", &ValidateCommand::execute);
        registerCommand("validate --gid", &ValidateCommand::executeByGID);
        registerCommand("validate --category", &ValidateCommand::executeByCategory);
        registerCommand("test", &TestCommand::execute);
        registerCommand("build --dev", &BuildCommand::executeDevMode);
        registerCommand("build --prod", &BuildCommand::executeProdMode);
        registerCommand("docgen", &DocGenCommand::execute);
        registerCommand("audit", &AuditCommand::execute);
        registerCommand("audit --gid", &AuditCommand::executeByGID);
        
        // Rule Management (GID-based)
        registerCommand("rule list", &RuleCommand::list);
        registerCommand("rule list --format=gid", &RuleCommand::listGIDs);
        registerCommand("rule info --gid", &RuleCommand::infoByGID);
        registerCommand("rule disable --gid", &RuleCommand::disableByGID);
        registerCommand("rule enable --gid", &RuleCommand::enableByGID);
        registerCommand("rule validate --gid", &RuleCommand::validateGIDFormat);
        
        // RuleSet Management
        registerCommand("ruleset list", &RuleSetCommand::list);
        registerCommand("ruleset info", &RuleSetCommand::info);
        registerCommand("ruleset validate", &RuleSetCommand::validate);
        registerCommand("ruleset create", &RuleSetCommand::create);
        
        // RuleSet-based Operations
        registerCommand("validate --ruleset", &ValidateCommand::executeByRuleSet);
        registerCommand("audit --ruleset", &AuditCommand::executeByRuleSet);
        registerCommand("docgen --ruleset", &DocGenCommand::executeByRuleSet);
        
        // Graph Generation
        registerCommand("graph --type", &GraphCommand::generate);
        registerCommand("graph --type --format", &GraphCommand::generateWithFormat);
        registerCommand("graph --type --output", &GraphCommand::generateWithOutput);
        
        // Automation & CI/CD
        registerCommand("pipeline generate", &PipelineCommand::generate);
        registerCommand("pipeline validate", &PipelineCommand::validate);
        registerCommand("workflow create", &WorkflowCommand::create);
        registerCommand("workflow update", &WorkflowCommand::update);
        registerCommand("deploy configure", &DeployCommand::configure);
    }
};
}
```

### Build System Architecture

**Development and Production Build Profiles**:

```cpp
namespace akao::build {
struct BuildProfile {
    std::string name;           // "dev" or "prod"
    std::string compiler;       // "gcc" or "clang"
    std::vector<std::string> flags;
    std::map<std::string, std::string> defines;
    bool enable_hot_reload;     // dev only
    bool enable_optimization;   // prod only
    bool enable_debug_symbols;  // dev only
    bool enable_hash_verification; // prod only
};

class BuildManager {
public:
    BuildResult buildDev(const std::string& project_path) {
        auto profile = BuildProfile{
            .name = "dev",
            .compiler = "gcc",
            .flags = {"-std=c++17", "-g", "-O0", "-Wall", "-Wextra"},
            .enable_hot_reload = true,
            .enable_debug_symbols = true
        };
        
        auto watcher = FileWatcher(project_path);
        watcher.startWatching([this, project_path](const std::string& changed_file) {
            this->incrementalBuild(project_path, changed_file);
        });
        
        return build(profile, project_path);
    }
    
    BuildResult buildProd(const std::string& project_path) {
        auto profile = BuildProfile{
            .name = "prod",
            .compiler = "gcc",
            .flags = {"-std=c++17", "-O3", "-DNDEBUG", "-flto"},
            .enable_optimization = true,
            .enable_hash_verification = true
        };
        
        auto result = build(profile, project_path);
        if (result.success) {
            auto hasher = BuildHasher();
            result.hash = hasher.generateHash(result.output_path);
            hasher.saveHashFile(result.output_path + ".hash", result.hash);
        }
        
        return result;
    }
    
private:
    BuildResult build(const BuildProfile& profile, const std::string& project_path);
    void incrementalBuild(const std::string& project_path, const std::string& changed_file);
};

struct BuildResult {
    bool success;
    std::string output_path;
    std::string hash;           // prod builds only
    std::vector<std::string> errors;
    std::chrono::milliseconds build_time;
};
}
```

### Documentation Generation System

**Template-Based Generation with Auto-Update**:

```cpp
namespace akao::docgen {
class DocumentationEngine {
public:
    void generateAllDocumentation(const std::string& project_path) {
        auto rules = rule::Registry::loadRules(project_path + "/rules");
        auto features = feature::Manager::listFeatures(project_path);
        auto principles = loadPrinciples();
        
        generateRulesDocumentation(rules);
        generatePrinciplesDocumentation(principles);
        generateFeaturesDocumentation(features);
        generateAPIDocumentation(project_path);
    }
    
private:
    void generateRulesDocumentation(const std::vector<Rule>& rules) {
        auto template_engine = TemplateEngine();
        auto variables = std::map<std::string, std::string>();
        
        for (const auto& rule : rules) {
            variables["rule_name"] = rule.getName();
            variables["rule_description"] = rule.getDescription();
            variables["rule_examples"] = rule.getExamples();
            variables["rule_tests"] = rule.getTestCases();
        }
        
        auto content = template_engine.generateFromTemplate(
            "templates/RULES.md.template", variables);
        
        filesystem::writeFile("docs/RULES.md", content);
    }
    
    void generatePrinciplesDocumentation(const std::vector<Principle>& principles) {
        // Generate PRINCIPLES.md from actual tested principles
    }
    
    void generateFeaturesDocumentation(const std::vector<Feature>& features) {
        // Generate FEATURES.md from installed features
    }
    
    void generateAPIDocumentation(const std::string& project_path) {
        // Parse C++ headers and generate API documentation
    }
};
}
```

### Feature Management System

**External Registry and Secure Installation**:

```cpp
namespace akao::feature {
struct Feature {
    std::string name;
    std::string version;
    std::string description;
    std::string source;         // "builtin", "registry:name", "git:url"
    std::vector<std::string> dependencies;
    std::map<std::string, std::string> config;
    bool enabled;
    std::string signature;      // For security verification
};

class FeatureManager {
public:
    InstallResult installFromRegistry(const std::string& feature_name) {
        auto registries = registry::Client::loadRegistries();
        auto feature_info = findFeatureInRegistries(feature_name, registries);
        
        if (!feature_info.has_value()) {
            return InstallResult{false, "Feature not found in any registry"};
        }
        
        // Security validation
        auto sandbox = Sandbox::create();
        if (!sandbox.validateFeature(feature_info.value())) {
            return InstallResult{false, "Feature failed security validation"};
        }
        
        // Dependency resolution
        auto resolver = DependencyResolver();
        auto resolved_deps = resolver.resolve(feature_info.value().dependencies);
        
        // Install dependencies first
        for (const auto& dep : resolved_deps) {
            auto dep_result = installFromRegistry(dep);
            if (!dep_result.success) {
                return InstallResult{false, "Failed to install dependency: " + dep};
            }
        }
        
        // Install feature in sandbox
        auto install_result = sandbox.installFeature(feature_info.value());
        if (install_result.success) {
            updateInstalledFeatures(feature_info.value());
        }
        
        return install_result;
    }
    
    bool removeFeature(const std::string& feature_name) {
        auto installed = loadInstalledFeatures();
        auto feature_it = std::find_if(installed.begin(), installed.end(),
            [&](const Feature& f) { return f.name == feature_name; });
            
        if (feature_it == installed.end()) {
            return false;
        }
        
        // Check if other features depend on this one
        auto dependents = findDependentFeatures(feature_name);
        if (!dependents.empty()) {
            throw std::runtime_error("Cannot remove feature: other features depend on it");
        }
        
        // Remove feature files and update registry
        auto sandbox = Sandbox::create();
        sandbox.removeFeature(*feature_it);
        
        installed.erase(feature_it);
        saveInstalledFeatures(installed);
        
        return true;
    }
    
private:
    std::optional<Feature> findFeatureInRegistries(const std::string& name, 
                                                  const std::vector<Registry>& registries);
    std::vector<Feature> loadInstalledFeatures();
    void saveInstalledFeatures(const std::vector<Feature>& features);
    std::vector<std::string> findDependentFeatures(const std::string& feature_name);
};

struct InstallResult {
    bool success;
    std::string message;
};
}
```

### Automation & CI/CD System

**Pipeline Generation and Management**:

```cpp
namespace akao::automation {
class PipelineGenerator {
public:
    GenerationResult generatePipeline(const std::string& project_path) {
        auto config = loadPipelineConfig(project_path + "/.akao/pipeline.yaml");
        auto workflows = std::vector<Workflow>();
        
        // Generate CI workflow
        auto ci_workflow = generateCIWorkflow(config);
        workflows.push_back(ci_workflow);
        
        // Generate CD workflow
        auto cd_workflow = generateCDWorkflow(config);
        workflows.push_back(cd_workflow);
        
        // Generate validation workflow
        auto validate_workflow = generateValidationWorkflow();
        workflows.push_back(validate_workflow);
        
        // Write workflows to .github/workflows/
        for (const auto& workflow : workflows) {
            auto yaml_content = workflow.toYAML();
            auto file_path = project_path + "/.github/workflows/" + workflow.name + ".yml";
            filesystem::writeFile(file_path, yaml_content);
        }
        
        return GenerationResult{true, workflows.size(), "Pipeline generated successfully"};
    }
    
private:
    Workflow generateCIWorkflow(const PipelineConfig& config) {
        auto workflow = Workflow("ci");
        
        workflow.addTrigger("push");
        workflow.addTrigger("pull_request");
        
        auto job = Job("test");
        job.addStep("Checkout", "actions/checkout@v3");
        job.addStep("Setup C++", "actions/setup-cpp@v1");
        job.addStep("Validate", "akao validate");
        job.addStep("Test", "akao test");
        job.addStep("Build", "akao build --dev");
        
        workflow.addJob(job);
        return workflow;
    }
    
    Workflow generateCDWorkflow(const PipelineConfig& config) {
        auto workflow = Workflow("cd");
        
        workflow.addTrigger("push", "main");
        
        auto job = Job("deploy");
        job.addStep("Checkout", "actions/checkout@v3");
        job.addStep("Setup C++", "actions/setup-cpp@v1");
        job.addStep("Validate", "akao validate");
        job.addStep("Test", "akao test");
        job.addStep("Build Production", "akao build --prod");
        job.addStep("Deploy", "akao deploy configure");
        
        workflow.addJob(job);
        return workflow;
    }
    
    Workflow generateValidationWorkflow() {
        auto workflow = Workflow("validate");
        
        workflow.addTrigger("schedule", "0 0 * * *"); // Daily
        
        auto job = Job("validate");
        job.addStep("Checkout", "actions/checkout@v3");
        job.addStep("Universal Validation", "akao validate");
        job.addStep("Audit", "akao audit");
        
        workflow.addJob(job);
        return workflow;
    }
};

struct PipelineConfig {
    std::string name;
    std::vector<std::string> triggers;
    std::map<std::string, std::string> environment;
    std::vector<std::string> deploy_targets;
};

struct GenerationResult {
    bool success;
    int workflows_generated;
    std::string message;
};
}
```

### Interface Consistency System (CLI=Web=TUI)

**Unified Command Execution**:

```cpp
namespace akao::cli {
// Abstract command interface
class Command {
public:
    virtual ~Command() = default;
    virtual CommandResult execute(const CommandRequest& request) = 0;
    virtual std::string getDescription() const = 0;
    virtual std::vector<Parameter> getParameters() const = 0;
};

struct CommandRequest {
    std::string command_name;
    std::map<std::string, std::string> parameters;
    std::string interface_type;  // "cli", "web", "tui"
};

struct CommandResult {
    bool success;
    std::string output;
    std::map<std::string, std::string> metadata;
    std::vector<std::string> errors;
};

// Unified controller that works for all interfaces
class InterfaceController {
public:
    CommandResult executeCommand(const CommandRequest& request) {
        auto command = command_registry_.getCommand(request.command_name);
        if (!command) {
            return CommandResult{false, "", {}, {"Command not found: " + request.command_name}};
        }
        
        // Log command execution for audit trail
        trace::Logger::log("command_execution", {
            {"command", request.command_name},
            {"interface", request.interface_type},
            {"timestamp", getCurrentTimestamp()}
        });
        
        auto result = command->execute(request);
        
        // Log result for audit trail
        trace::Logger::log("command_result", {
            {"command", request.command_name},
            {"success", result.success ? "true" : "false"},
            {"interface", request.interface_type}
        });
        
        return result;
    }
    
private:
    CommandRegistry command_registry_;
};

// CLI interface implementation
class CLIInterface {
public:
    int run(int argc, char* argv[]) {
        auto parser = ArgumentParser();
        auto request = parser.parse(argc, argv);
        request.interface_type = "cli";
        
        auto result = controller_.executeCommand(request);
        
        std::cout << result.output << std::endl;
        if (!result.success) {
            for (const auto& error : result.errors) {
                std::cerr << "Error: " << error << std::endl;
            }
        }
        
        return result.success ? 0 : 1;
    }
    
private:
    InterfaceController controller_;
};

// Web interface implementation
class WebInterface {
public:
    void startServer(int port = 8080) {
        auto server = HttpServer(port);
        
        server.addRoute("POST", "/api/commands/{command}", 
            [this](const HttpRequest& req) -> HttpResponse {
                auto request = CommandRequest{
                    .command_name = req.path_params.at("command"),
                    .parameters = req.json_body,
                    .interface_type = "web"
                };
                
                auto result = controller_.executeCommand(request);
                
                return HttpResponse{
                    .status_code = result.success ? 200 : 400,
                    .headers = {{"Content-Type", "application/json"}},
                    .body = serializeToJSON(result)
                };
            });
        
        server.start();
    }
    
private:
    InterfaceController controller_;
};

// Terminal UI interface implementation
class TUIInterface {
public:
    void startInteractiveMode() {
        while (true) {
            displayMenu();
            auto input = getUserInput();
            
            if (input == "quit" || input == "exit") {
                break;
            }
            
            auto request = parseUserInput(input);
            request.interface_type = "tui";
            
            auto result = controller_.executeCommand(request);
            displayResult(result);
        }
    }
    
private:
    InterfaceController controller_;
    
    void displayMenu() {
        std::cout << "\n=== Akao Framework ===" << std::endl;
        std::cout << "Available commands:" << std::endl;
        auto commands = controller_.getAvailableCommands();
        for (const auto& cmd : commands) {
            std::cout << "  " << cmd.name << " - " << cmd.description << std::endl;
        }
        std::cout << "Enter command (or 'quit' to exit): ";
    }
    
    std::string getUserInput() {
        std::string input;
        std::getline(std::cin, input);
        return input;
    }
    
    CommandRequest parseUserInput(const std::string& input) {
        // Parse user input into CommandRequest
        auto parser = InteractiveParser();
        return parser.parse(input);
    }
    
    void displayResult(const CommandResult& result) {
        if (result.success) {
            std::cout << "✅ " << result.output << std::endl;
        } else {
            std::cout << "❌ Command failed:" << std::endl;
            for (const auto& error : result.errors) {
                std::cout << "  " << error << std::endl;
            }
        }
    }
};
}
```

---

## 🏗️ Component Integration Map

### Core System Dependencies

```
Rule Engine ──┐
              ├── Universal Validator ──┐
Build Engine ─┘                        ├── CLI Controller ──┐
                                        │                   ├── All Interfaces
Documentation ──┐                      │                   │
                ├── Template Engine ────┘                   ├── Audit System
Project Mgmt ───┘                                          │
                                                            │
Feature Mgmt ──┐                                           │
               ├── Security Sandbox ─────────────────────────┘
Automation ────┘
```

### Data Flow Architecture

```
User Input → Interface Layer → Controller → Core Logic → Trace System
    ↓              ↓               ↓           ↓            ↓
   CLI          Parser         Validator    Rules       Audit Log
   Web         Router         Builder      Config      Coverage
   TUI         Interactive    DocGen       Features    Metrics
```

---

## 📊 Success Criteria & Validation

### Performance Requirements
- **Startup Time**: <100ms cold start for CLI operations
- **Validation Speed**: <1s for medium projects (1000 files)
- **Build Speed**: <5s for incremental dev builds, <30s for prod builds
- **Memory Usage**: <100MB for typical operations

### Quality Requirements
- **Test Coverage**: >95% code coverage across all components
- **Documentation Coverage**: 100% of public APIs documented
- **Rule Coverage**: All 19 philosophical principles enforced by testable rules
- **Interface Consistency**: 100% feature parity across CLI/Web/TUI

### Security Requirements
- **Feature Sandboxing**: All external features run in isolated environments
- **Input Validation**: All user inputs validated and sanitized
- **File System Security**: All file operations use safe, validated paths
- **Audit Trail**: Complete operation logging for security analysis

---

## 🔧 Build Configuration

### CMakeLists.txt Specification

```cmake
cmake_minimum_required(VERSION 3.20)
project(akao VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configuration options
option(AKAO_BUILD_TESTS "Build tests" ON)
option(AKAO_BUILD_PLUGINS "Build example plugins" ON)
option(AKAO_ENABLE_ASAN "Enable AddressSanitizer" OFF)

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
    if(AKAO_ENABLE_ASAN)
        add_compile_options(-fsanitize=address)
        add_link_options(-fsanitize=address)
    endif()
endif()

# Third-party dependencies
find_package(nlohmann_json REQUIRED)
find_package(fmt REQUIRED)

# Include directories
include_directories(include)

# Core library
file(GLOB_RECURSE AKAO_SOURCES "src/*.cpp")
add_library(akao_core STATIC ${AKAO_SOURCES})
target_link_libraries(akao_core nlohmann_json::nlohmann_json fmt::fmt)

# Main executable
add_executable(akao src/main.cpp)
target_link_libraries(akao akao_core)

# Tests
if(AKAO_BUILD_TESTS)
    find_package(Catch2 REQUIRED)
    file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")
    add_executable(akao_tests ${TEST_SOURCES})
    target_link_libraries(akao_tests akao_core Catch2::Catch2WithMain)
    
    enable_testing()
    add_test(NAME akao_unit_tests COMMAND akao_tests)
    add_test(NAME akao_integration_tests COMMAND akao_tests --tag integration)
    add_test(NAME akao_principle_tests COMMAND akao_tests --tag principles)
endif()

# Plugins
if(AKAO_BUILD_PLUGINS)
    add_subdirectory(plugins/wasm)
    add_subdirectory(plugins/js)
    add_subdirectory(plugins/android)
endif()

# Installation
install(TARGETS akao DESTINATION bin)
install(DIRECTORY templates/ DESTINATION share/akao/templates)
install(DIRECTORY rules/ DESTINATION share/akao/rules)
```

---

## 🎯 Final Validation Checklist

### Cross-Document Consistency ✅
- [ ] All CLI commands from README.md implemented in CLI system
- [ ] All 8 components from ARCHITECTURE.md have corresponding implementation
- [ ] All 19 principles from PHILOSOPHY.md enforced through testable rules
- [ ] All improvements from DESIGN_DOCS_REFACTOR_SUMMARY.md integrated

### Philosophical Principle Implementation ✅
- [ ] **Structure is enforced**: Rule engine validates project structure
- [ ] **Universal validation**: Validation works on any project including Akao
- [ ] **No rules without tests**: Every rule has corresponding test case
- [ ] **Every rule is traceable**: Violations include full diagnostic information
- [ ] **Documentation is code**: All docs generated from rules and tests
- [ ] **One class per folder**: Project structure enforces this principle
- [ ] **One language per scope**: Multi-language support with clear boundaries
- [ ] **CLI = Web = TUI**: Identical functionality across all interfaces
- [ ] **Every action is measurable**: Complete audit trail and metrics
- [ ] **Rules can be toggled**: Disabled rules tracked and reported
- [ ] **Auto-update documentation**: Docs regenerate on code/rule changes
- [ ] **Everything builds dev + prod**: Both development and production builds
- [ ] **No external implicit behavior**: All dependencies explicitly declared
- [ ] **Only one truth**: Single source of truth for all configuration
- [ ] **Observable, explainable, deterministic**: Full transparency in all operations
- [ ] **Features are composable**: Features work together without conflicts
- [ ] **Templates are validated**: All templates pass the same rules
- [ ] **External features are sandboxed**: Security isolation for external code
- [ ] **Dependency resolution is explicit**: Clear dependency management

### Technical Implementation ✅
- [ ] All source files follow one-class-per-folder principle
- [ ] Complete CMake configuration with all dependencies
- [ ] Comprehensive test suite with >95% coverage
- [ ] All performance requirements specified and achievable
- [ ] Security requirements defined and implementable
- [ ] Plugin system architecture complete and extensible

This specification is **ready for single-session implementation** with GitHub Copilot.