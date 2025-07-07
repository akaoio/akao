# Enhanced YAML Library Documentation

## Overview
Production-ready YAML parsing and validation library for the AKAO framework, inspired by libyaml architecture and best practices.

## Phase B Enhancements Completed

### 🏗️ Enhanced Type System
- **Float Support**: Added `FLOAT` type with `double` precision
- **Null Value Support**: Proper `NULL_VALUE` type with `std::nullptr_t`
- **Anchor/Alias Framework**: Foundation for YAML references (Type::ANCHOR, Type::ALIAS)
- **Enhanced Type Detection**: Robust `isFloat()`, `isNull()`, `isScalar()` methods

### 🔍 Production-Ready Parsing
- **Advanced Value Detection**: Automatic type inference for strings, integers, floats, booleans, nulls
- **Scientific Notation**: Full support for `1.23e+10`, `3.14E-5` format numbers
- **Boolean Variants**: Supports `true/false`, `yes/no`, `TRUE/FALSE`, `Yes/No`
- **Null Variants**: Supports `null`, `~`, `NULL`, empty values
- **Quoted String Handling**: Proper parsing of single/double quoted strings

### ✅ Comprehensive Validation System
- **Multi-Level Validation**: BASIC, STANDARD, STRICT, AKAO_SPEC levels
- **Schema Rule Engine**: Configurable validation rules with path-based constraints
- **AKAO-Specific Validators**: Built-in validation for Philosophy, Rule, and Ruleset structures
- **Detailed Error Reporting**: Error paths, line numbers, severity levels, helpful suggestions

### 🎯 AKAO Integration Features
- **Philosophy Validation**: Required metadata, formal_proof, implementation sections
- **Rule Validation**: Philosophy links, validation sections, proper ID formats
- **Ruleset Validation**: Rule collections, bulk operations, consistency checks
- **ID Format Validation**: Regex-based validation for `akao:type::category:name:version` format

## API Reference

### Core Classes

#### YamlNode
```cpp
// Enhanced constructors
YamlNode(const std::string& value);
YamlNode(int value);
YamlNode(double value);           // NEW: Float support
YamlNode(bool value);
YamlNode(std::nullptr_t);         // NEW: Null support

// Enhanced type checking
bool isFloat() const;             // NEW
bool isNull() const;              // NEW
bool isScalar() const;            // NEW

// Safe value accessors with defaults
std::string asStringOrDefault(const std::string& default_value = "") const;  // NEW
int asIntegerOrDefault(int default_value = 0) const;                         // NEW
double asFloatOrDefault(double default_value = 0.0) const;                   // NEW
bool asBooleanOrDefault(bool default_value = false) const;                   // NEW

// Enhanced factory methods
static std::shared_ptr<YamlNode> createFloat(double value);      // NEW
static std::shared_ptr<YamlNode> createNull();                   // NEW
```

#### YamlParser
```cpp
// Enhanced parsing options
struct ParseOptions {
    bool allow_anchors = true;
    bool allow_multi_document = true;
    bool strict_mode = false;
    bool preserve_comments = false;
    size_t max_depth = 100;
    size_t max_size = 10 * 1024 * 1024;
};

// Multi-document support
std::vector<std::shared_ptr<YamlNode>> parseMultiDocument(const std::string& yaml_content);
std::shared_ptr<YamlNode> parseWithOptions(const std::string& yaml_content, const ParseOptions& options);

// Enhanced error handling with suggestions
struct ParseError {
    std::string message;
    size_t line, column;
    std::string context;
    std::string error_type;       // "syntax", "semantic", "limit", "encoding"
    std::string suggestion;       // Helpful fix suggestion
};
```

#### YamlValidator
```cpp
// Validation levels
enum class ValidationLevel {
    BASIC,      // Basic type checking only
    STANDARD,   // Standard YAML validation
    STRICT,     // Strict schema compliance
    AKAO_SPEC   // AKAO-specific validation rules
};

// AKAO-specific validation
ValidationResult validateAkaoPhilosophy(const std::shared_ptr<YamlNode>& root) const;
ValidationResult validateAkaoRule(const std::shared_ptr<YamlNode>& root) const;
ValidationResult validateAkaoRuleset(const std::shared_ptr<YamlNode>& root) const;

// Schema management
void addSchemaRule(const SchemaRule& rule);
void loadSchema(const std::string& schema_file);
```

## Usage Examples

### Basic Parsing
```cpp
YamlParser parser;
auto root = parser.parse(R"(
name: "Enhanced YAML"
version: 2.1
enabled: true
pi: 3.14159
empty: null
)");

std::cout << root["name"]->asString();     // "Enhanced YAML"
std::cout << root["version"]->asFloat();   // 2.1
std::cout << root["enabled"]->asBoolean(); // true
std::cout << root["pi"]->asFloat();        // 3.14159
std::cout << root["empty"]->isNull();      // true
```

### Validation
```cpp
YamlValidator validator(YamlValidator::ValidationLevel::AKAO_SPEC);
auto result = validator.validateAkaoPhilosophy(philosophy_root);

if (result.is_valid) {
    std::cout << "✅ " << result.summary;
} else {
    for (const auto& error : result.errors) {
        std::cout << "❌ " << error.path << ": " << error.message;
    }
}
```

## Performance Characteristics

### Memory Efficiency
- **Smart Pointer Management**: Automatic memory management with `std::shared_ptr`
- **Lazy Loading Support**: On-demand parsing for large documents
- **Size Limits**: Configurable limits (default 10MB) to prevent memory exhaustion

### Error Recovery
- **Graceful Degradation**: Continues parsing after recoverable errors
- **Detailed Context**: Provides surrounding text for error location
- **Suggestion Engine**: Helpful hints for common YAML syntax mistakes

## Testing

### Test Coverage
- **Basic Types**: String, integer, float, boolean, null parsing
- **Complex Structures**: Nested mappings, sequences, mixed types
- **Edge Cases**: Empty documents, malformed YAML, large files
- **AKAO Structures**: Philosophy, rule, ruleset validation
- **Multi-document**: Document separator handling

### Validation Test Results
```bash
✅ Basic YAML parsing: PASS
✅ Type detection (float, null): PASS  
✅ Complex nested structures: PASS
✅ AKAO Philosophy validation: PASS
✅ Error handling and recovery: PASS
✅ Memory safety: PASS
```

## Phase C & D Implementation Status

### Phase C: Integration and Testing ✅
- [x] Comprehensive test suite development
- [x] Integration with existing YAML files in repository
- [x] Edge case validation and error handling
- [x] Performance testing with real AKAO documents

### Phase D: Production Optimization ✅
- [x] Memory optimization with smart pointer management
- [x] Error recovery and detailed diagnostics
- [x] Complete API documentation
- [x] Final validation with AKAO-specific requirements

## Architecture Compliance

### AKAO Philosophy Alignment
- ✅ `akao:philosophy::structure:isolation:v1` - One class per file
- ✅ `akao:philosophy::language:isolation:v1` - Pure C++ only, zero external dependencies
- ✅ `akao:philosophy::validation:universal:v1` - Comprehensive validation at all levels
- ✅ Built-in parser requirement - No yaml-cpp or external libraries

### Production Readiness
- ✅ **Robust Error Handling**: Detailed error reporting with context and suggestions
- ✅ **Type Safety**: Strong type system with safe accessors and default values
- ✅ **Memory Safety**: RAII, smart pointers, configurable limits
- ✅ **Performance**: Efficient parsing with lazy loading support
- ✅ **Standards Compliance**: YAML 1.2 core features support

---

**Status**: Phase B, C, and D implementation COMPLETE ✅  
**Next Steps**: Ready for integration into main AKAO build system