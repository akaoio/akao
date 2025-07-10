# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build System

This project uses **Make** as the sole build system for architectural simplicity:

```bash
make all            # Build main executable
make debug          # Build in debug mode
make release        # Build in release mode (default)
make tests          # Build test suite
make run-tests      # Build and run all tests
```

## Common Development Commands

### Building
```bash
make all                    # Build main executable
make tests                  # Build test suite
```

### Testing
```bash
make run-tests              # Run all tests
make self-validate          # Run self-validation
make validate-project       # Validate current project
```

### Validation
```bash
./build/bin/akao validate .     # Validate current project
make self-validate              # Self-validation via Make
make validate-project           # Validate project via Make
```

### Linting and Type Checking
No specific linting commands are configured. The project uses C++17 standards with compiler warnings enabled.

## Architecture Overview

### Core Structure
- **Pure Logic Engine**: Domain-agnostic runtime interpreter for logical expressions (`core/engine/logic/runtime/`)
- **Unified Validator**: System for validating code against rules and philosophies (`core/engine/validator/`)
- **YAML Parser**: Internal YAML parsing implementation (`core/engine/parser/yaml/`)
- **Akao Pure Logic Parser**: Parser for .a format files (`core/engine/logic/components/parser/akao/`)
- **File System Operations**: File scanning, watching, and enforcement (`core/filesystem/`)
- **Tracing System**: Logging, reporting, and violation tracking (`core/trace/`)

**Note**: Rule management functionality has been consolidated into the Unified Validator for architectural simplicity.

### Key Design Principles
1. **Zero External Dependencies**: Uses only C++ standard library
2. **Philosophy-Driven Architecture**: All code must align with philosophical foundations
3. **Universal Validation**: Can validate any codebase regardless of language
4. **Self-Validating**: Framework validates itself against its own rules
5. **Pure Logic Engine**: Core engine has no hardcoded domain knowledge
6. **Rule-Driven Validation**: Uses .akao/rules/ directory structure with enabled/disabled organization
7. **Strict Folder Hierarchy**: No mixing of files and folders, parent-child relationships enforced

### Main Components

#### Pure Logic Engine (`core/engine/logic/runtime/`)
- Domain-agnostic runtime interpreter
- YAML syntax support for logical expressions
- Built-in function registry (47+ functions)
- Variable scoping and execution context management
- Support for quantifiers (forall, exists) and logical operators

#### Unified Validator (`core/engine/validator/`)
- Systematic governance enforcement
- Rule-driven validation using .akao/rules/ directory
- Automated compliance checking
- Validation issue reporting and remediation
- Multiple validation modes (universal, compliance, metadata, namespace)

#### YAML Parser (`core/engine/parser/yaml/`)
- Internal YAML parsing implementation
- Node processing and validation
- No external YAML library dependencies

#### Akao Pure Logic Parser (`core/engine/logic/components/parser/akao/`)
- Parser for .a format files (Akao Pure Logic)
- Lexical analysis and tokenization
- Support for mathematical notation and operators
- Quantifier parsing and function calls

### Testing Architecture
- **Unit Tests**: Individual component testing (`tests/unit/`)
- **Integration Tests**: System-level testing (`tests/integration/`)
- **CLI Tests**: Command-line interface testing (`tests/cli/`)
- **Compliance Tests**: Self-validation testing (`tests/compliance/`)
- **Performance Tests**: Benchmarking and performance validation (`tests/performance/`)

### File Organization
- One class per file (strictly enforced)
- Version-based naming (v1.cpp, v1.hpp)
- Comprehensive metadata headers in all files
- Hierarchical directory structure reflecting logical organization
- Strict folder content rules: containers with subdirectories OR implementation folders with version files
- No mixing of files and folders at same level (except root config files)
- Parent-child components must maintain hierarchical relationships

## Development Workflow

### Test and Example File Organization (MANDATORY)

**File Placement Rules:**
- **ALL** development test files MUST go in `artifacts/` directory
- **NO** development test files allowed in `examples/` directory  
- `examples/` reserved ONLY for exhibition-quality demonstration files

**Directory Purposes:**
- **`artifacts/`**: Development artifacts, test files, debugging scripts, temporary files
- **`examples/`**: Polished, exhibition-quality demonstrations for public display

**Correct Placement:**
```bash
✅ Development Tests:
artifacts/godel-development/test-undecidability.a
artifacts/parser-fixes/test-multiline.a
artifacts/feature-branch/unit-test-simple.a

✅ Exhibition Files:
examples/godel-exhibition.a

❌ Wrong:
examples/test-*.a           # NO tests in examples/
examples/debug-*.a          # NO debug files in examples/
```

### Standard Workflow

1. **Code Changes**: Follow the one-class-per-file rule
2. **Testing**: Place development tests in `artifacts/`, not `examples/`
3. **Build**: Use `make all` (Make is the only build system)
4. **Test**: Run `make run-tests` to ensure all tests pass
5. **Validate**: Run `make self-validate` to check compliance
6. **Metadata**: Ensure all files have proper metadata headers

## CLI Usage

The main executable provides these commands:
- `validate <path>`: Universal validation of any project
- `run <file.a>`: Execute Pure Logic .a files
- `parse <file>`: Parse and analyze files
- `test`: Run system tests
- `build`: Build the project

## Rule System

### Rule Organization
- **Rule Directory**: `.akao/rules/`
- **Active Rules**: `.akao/rules/enabled/`
- **Inactive Rules**: `.akao/rules/disabled/`
- **Rule Format**: `.a` files (Akao Pure Logic format)

### Rule Management
```bash
# Enable a rule
mv .akao/rules/disabled/category/rule.a .akao/rules/enabled/category/rule.a

# Disable a rule
mv .akao/rules/enabled/category/rule.a .akao/rules/disabled/category/rule.a
```

### Akao Pure Logic Format (.a)
- Concise syntax for logical expressions
- Supports quantifiers: `forall`, `exists`
- Mathematical operators: `&&`, `||`, `!`, `==`, `!=`, `>`, `<`, `>=`, `<=`
- Function calls with parentheses
- Example:
  ```
  forall $file in filesystem.get_cpp_files("."): 
      cpp.count_classes(filesystem.read_file($file)) == 1
  ```

### Built-in Functions
- **Filesystem**: `filesystem.get_files()`, `filesystem.read_file()`, `filesystem.get_cpp_files()`
- **C++ Analysis**: `cpp.count_classes()`, `cpp.uses_modern_cpp()`, `cpp.has_proper_headers()`
- **String Operations**: `string.normalize()`, `string.length()`, `string.concat()`
- **Collections**: `collection.count()`, `collection.contains()`
- **Math**: `math.add()`, `math.multiply()`, `math.power()`
- **Interface**: `interface.get_cli_commands()`, `interface.get_programmatic_commands()`
- **Rules**: `rules.get_enabled_rules()`, `rules.execute_rule()`
- **Philosophy**: `philosophy.self_proves()`

## Special Notes

- The project implements a "Universal Validation Framework" that can validate any codebase
- All code follows strict philosophical principles defined in the project
- The Pure Logic Engine is designed to be completely domain-agnostic
- Self-validation is a core requirement - the framework must validate itself
- **IMPORTANT**: Follow CONTRIBUTING.md for complete guidelines on folder structure, naming conventions, and metadata requirements

## Folder Structure Rules

### Container Folders
- Contains ONLY subdirectories + optional `_.yaml`
- NO implementation files allowed

### Implementation Folders  
- Contains ONLY version files (`v1.cpp`, `v1.hpp`)
- NO `_.yaml` file allowed
- NO subdirectories allowed

### Prohibited Patterns
- ❌ Files and folders at same level (except root config files)
- ❌ Parent-child components as siblings
- ❌ Mixed content in folders
- ❌ Underscores in file/folder names
- ❌ Incorrect English grammar in names

## Metadata Requirements

All components must include:
- **@id**: akao namespace identifier
- **@doc**: User-facing documentation (20-200 words)
- **@specification**: Technical description (10-100 words)
- **@scope**: Architectural location
- **@timeline**: YYYY-MM-DD format
- **@rationale**: Why it exists
- **@methodology**: How it works
- **@references**: Related akao IDs