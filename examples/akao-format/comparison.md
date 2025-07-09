# Akao Pure Logic (.a) Format vs YAML Comparison

This document shows the dramatic improvement in readability and conciseness that the .a format provides over YAML.

## Basic Operations

### .a Format
```a
true && false
x > 5
!condition
```

### YAML Equivalent
```yaml
and:
  - true
  - false
---
greater:
  - x
  - 5
---
not: condition
```

## Function Calls

### .a Format
```a
filesystem.get_files("/path")
collection.count(items)
```

### YAML Equivalent
```yaml
filesystem.get_files: ["/path"]
---
collection.count: [items]
```

## Complex Expressions

### .a Format
```a
(cpp.count_classes(code) == 1) && (string.length(filename) > 0)
```

### YAML Equivalent
```yaml
and:
  - equals:
      - cpp.count_classes: [code]
      - 1
  - greater:
      - string.length: [filename]
      - 0
```

## Quantifiers

### .a Format
```a
∀file in filesystem.get_cpp_files("."): cpp.count_classes(filesystem.read_file(file)) == 1
```

### YAML Equivalent
```yaml
forall:
  variable: file
  domain:
    filesystem.get_cpp_files: ["."]
  condition:
    equals:
      - cpp.count_classes:
          - filesystem.read_file: [file]
      - 1
```

## Conditionals

### .a Format
```a
count > 0 ? "files found" : "no files"
```

### YAML Equivalent
```yaml
if:
  greater:
    - count
    - 0
then: "files found"
else: "no files"
```

## Benefits of .a Format

1. **Conciseness**: 70-80% reduction in syntax overhead
2. **Readability**: Mathematical notation familiar to developers
3. **Maintainability**: Easier to write and modify
4. **Error Prevention**: Less nesting reduces indentation errors
5. **Familiarity**: Syntax similar to popular programming languages

## Use Cases

- **Rules**: Quick validation rules in `.akao/rules/`
- **Conditions**: Inline logic in configuration
- **Testing**: Concise test expressions
- **Documentation**: Readable examples in docs