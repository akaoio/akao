# Akao

A universal validation framework for any codebase.

## What it does

Akao validates code against rules. It works with any programming language by parsing rule files in `.akao/rules/` directories.

## Quick start

```bash
# Build
make all

# Validate a project
./akao validate /path/to/project

# Show available rules
./akao rules list

# Execute Pure Logic files
./akao run example.a
```

## How it works

- **Rules**: Define validation logic in `.a` format (Akao Pure Logic)
- **Engine**: Zero-dependency C++17 core validates any codebase
- **Architecture**: One class per file, strict folder hierarchy

## Core components

- **Pure Logic Engine** (`core/engine/logic/`): Domain-agnostic runtime interpreter
- **Unified Validator** (`core/engine/validator/`): Rule-driven validation system
- **YAML Parser** (`core/engine/parser/yaml/`): Internal YAML implementation
- **File Operations** (`core/filesystem/`): Scanning, watching, enforcement

## Build

```bash
make all           # Build main executable
make run-tests     # Build and run all tests
make self-validate # Self-validation check
```

## Testing

The framework includes comprehensive test coverage:
- Unit tests for core components
- Integration tests for system behavior  
- CLI command validation
- Self-compliance verification

## Rules

Rules live in `.akao/rules/enabled/` (active) and `.akao/rules/disabled/` (inactive).

Example rule:
```
forall $file in filesystem.get_cpp_files("."): 
    cpp.count_classes(filesystem.read_file($file)) == 1
```

## License

MIT License - see LICENSE file.