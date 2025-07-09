# .akao/rules Directory

This directory contains user-configurable validation rules for the Akao framework. Rules can be written in YAML or .a format (Akao Pure Logic).

## Directory Structure

```
.akao/rules/
├── README.md              # This file
├── config.yaml            # Rules configuration
├── enabled/               # Enabled rules
│   ├── structure/         # Structure validation rules
│   ├── interface/         # Interface validation rules
│   ├── language/          # Language-specific rules
│   ├── build/             # Build system rules
│   └── custom/            # Custom project rules
├── disabled/              # Disabled rules
└── examples/              # Rule examples
```

## Rule Formats

### YAML Format
```yaml
# .akao/rules/enabled/structure/one-class-per-file.yaml
id: "structure:one_class_per_file"
name: "One Class Per File"
description: "Ensures each file contains exactly one class"
category: "structure"
severity: "error"
applies_to: ["*.cpp", "*.hpp"]

logic:
  forall:
    variable: "$file"
    domain:
      filesystem.get_cpp_files: ["."]
    condition:
      equals:
        - cpp.count_classes:
            - filesystem.read_file: ["$file"]
        - 1
```

### .a Format (Akao Pure Logic)
```a
# .akao/rules/enabled/structure/one-class-per-file.a
# id: structure:one_class_per_file
# name: One Class Per File
# description: Ensures each file contains exactly one class
# category: structure
# severity: error
# applies_to: *.cpp, *.hpp

∀$file in filesystem.get_cpp_files("."): 
    cpp.count_classes(filesystem.read_file($file)) == 1
```

## Rule Configuration

### config.yaml
```yaml
# Global rules configuration
rules:
  default_enabled: true
  parallel_execution: false
  auto_fix: false
  
# Category settings
categories:
  structure:
    enabled: true
    severity: error
  interface:
    enabled: true
    severity: warning
  language:
    enabled: true
    severity: error
  build:
    enabled: false
    severity: info
  custom:
    enabled: true
    severity: error

# File type mappings
file_types:
  cpp: ["*.cpp", "*.hpp", "*.cc", "*.hh"]
  yaml: ["*.yaml", "*.yml"]
  akao: ["*.a"]
  
# Exclusions
exclude_paths:
  - "build/"
  - "node_modules/"
  - ".git/"
  - "*.tmp"
```

## Usage

### Command Line
```bash
# Validate current directory with all enabled rules
akao validate

# Validate specific path
akao validate ./src

# Validate with specific rules
akao validate --rules structure:one_class_per_file,interface:command_parity

# Enable/disable rules
akao rules enable structure:one_class_per_file
akao rules disable interface:command_parity

# List available rules
akao rules list
akao rules list --category structure
```

### Programmatic Usage
```cpp
#include "core/engine/validator/unified/v1.hpp"

using namespace akao::core::engine::validator::unified;

// Create validation system
auto validator = ValidationSystemFactory::create(".");

// Validate with all enabled rules
auto result = validator->validate("./src");

// Validate with specific rules
auto result = validator->validateWithRules("./src", {
    "structure:one_class_per_file",
    "interface:command_parity"
});
```

## Built-in Functions

Rules have access to these built-in functions:

### Filesystem
- `filesystem.get_files(path)` - Get all files in directory
- `filesystem.get_cpp_files(path)` - Get C++ files
- `filesystem.read_file(path)` - Read file contents
- `filesystem.has_extension(file, ext)` - Check file extension

### C++ Analysis
- `cpp.count_classes(code)` - Count classes in code
- `cpp.extract_classes(code)` - Extract class names
- `cpp.count_functions(code)` - Count functions
- `cpp.has_main(code)` - Check for main function

### Collections
- `collection.count(items)` - Count items
- `collection.contains(items, item)` - Check if contains item
- `collection.filter(items, condition)` - Filter items

### String Operations
- `string.length(text)` - Get string length
- `string.contains(text, substring)` - Check if contains substring
- `string.matches(text, pattern)` - Check regex pattern

### Math Operations
- `math.add(a, b)` - Addition
- `math.subtract(a, b)` - Subtraction
- `math.multiply(a, b)` - Multiplication

## Rule Examples

See the `examples/` directory for complete rule examples covering:
- Structure validation
- Interface compliance
- Language-specific rules
- Build system validation
- Custom project rules

## Advanced Features

### Mathematical Validation
Rules support advanced mathematical concepts:
- Set theory operations
- Formal logic proofs
- Quantifier reasoning
- Constraint satisfaction

### Lazy Loading
Rules are loaded on-demand based on:
- Target file types
- Rule categories
- Validation context
- Performance requirements

### Parallel Execution
Rules can be executed in parallel when:
- No dependencies between rules
- Safe for concurrent execution
- Performance benefits available