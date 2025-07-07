#pragma once

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <memory>

namespace akao::core::engine::parser {

/**
 * @brief Enhanced YAML Node representation for Akao framework
 * 
 * Production-ready YAML node system inspired by libyaml architecture
 * Pure C++ implementation without external dependencies
 * Supports nested structures, arrays, and all YAML 1.2 scalar types
 * 
 * Philosophy Compliance:
 * - akao:philosophy::structure:isolation:v1 (one class per file)
 * - akao:philosophy::language:isolation:v1 (pure C++ only)
 * - akao:philosophy::validation:universal:v1 (comprehensive validation)
 * 
 * Enhanced Features:
 * - Float/double support
 * - Null value handling
 * - Anchor and alias support
 * - Source location tracking
 * - Memory-efficient operations
 * - Deep copy and comparison
 */
class YamlNode {
public:
    enum class Type {
        UNDEFINED,
        STRING,
        INTEGER,
        FLOAT,      // Enhanced: Added float support
        BOOLEAN,
        NULL_VALUE, // Enhanced: Explicit null type
        SEQUENCE,
        MAPPING,
        ANCHOR,     // Enhanced: Anchor reference
        ALIAS       // Enhanced: Alias reference
    };

    using Value = std::variant<
        std::string,                                             // For STRING, ANCHOR, ALIAS names
        int,                                                     // For INTEGER
        double,                                                  // For FLOAT  
        bool,                                                    // For BOOLEAN
        std::nullptr_t,                                          // For NULL_VALUE
        std::vector<std::shared_ptr<YamlNode>>,                 // For SEQUENCE
        std::map<std::string, std::shared_ptr<YamlNode>>        // For MAPPING
    >;
    
    // Enhanced: Source location tracking
    struct SourceLocation {
        size_t line = 0;
        size_t column = 0;
        std::string file_path;
    };

private:
    Type type_;
    Value value_;
    SourceLocation source_location_;  // Enhanced: Track source location
    std::string anchor_name_;         // Enhanced: Anchor name if applicable
    
public:
    // Enhanced constructors
    YamlNode();
    explicit YamlNode(const std::string& value);
    explicit YamlNode(int value);
    explicit YamlNode(double value);              // Enhanced: Float constructor
    explicit YamlNode(bool value);
    explicit YamlNode(std::nullptr_t);            // Enhanced: Null constructor
    explicit YamlNode(const std::vector<std::shared_ptr<YamlNode>>& sequence);
    explicit YamlNode(const std::map<std::string, std::shared_ptr<YamlNode>>& mapping);
    
    // Enhanced: Copy and move constructors
    YamlNode(const YamlNode& other);
    YamlNode(YamlNode&& other) noexcept;
    YamlNode& operator=(const YamlNode& other);
    YamlNode& operator=(YamlNode&& other) noexcept;

    // Enhanced type checking
    Type getType() const;
    bool isString() const;
    bool isInteger() const;
    bool isFloat() const;           // Enhanced: Float type checking
    bool isBoolean() const;
    bool isNull() const;            // Enhanced: Null type checking
    bool isSequence() const;
    bool isMapping() const;
    bool isAnchor() const;          // Enhanced: Anchor type checking
    bool isAlias() const;           // Enhanced: Alias type checking
    bool isDefined() const;
    bool isScalar() const;          // Enhanced: Check if scalar type

    // Enhanced value accessors with safety
    std::string asString() const;
    std::string asStringOrDefault(const std::string& default_value = "") const;
    int asInteger() const;
    int asIntegerOrDefault(int default_value = 0) const;
    double asFloat() const;         // Enhanced: Float accessor
    double asFloatOrDefault(double default_value = 0.0) const;
    bool asBoolean() const;
    bool asBooleanOrDefault(bool default_value = false) const;
    const std::vector<std::shared_ptr<YamlNode>>& asSequence() const;
    const std::map<std::string, std::shared_ptr<YamlNode>>& asMapping() const;

    // Convenience accessors for nested access
    std::shared_ptr<YamlNode> operator[](const std::string& key) const;
    std::shared_ptr<YamlNode> operator[](size_t index) const;

    // Utility methods
    bool hasKey(const std::string& key) const;
    size_t size() const;
    std::vector<std::string> getKeys() const;

    // String representation
    std::string toString() const;
    std::string toYaml(int indent = 0) const;

    // Enhanced factory methods
    static std::shared_ptr<YamlNode> createString(const std::string& value);
    static std::shared_ptr<YamlNode> createInteger(int value);
    static std::shared_ptr<YamlNode> createFloat(double value);        // Enhanced: Float factory
    static std::shared_ptr<YamlNode> createBoolean(bool value);
    static std::shared_ptr<YamlNode> createNull();                     // Enhanced: Null factory
    static std::shared_ptr<YamlNode> createSequence();
    static std::shared_ptr<YamlNode> createMapping();
    
    // Enhanced: Anchor and alias factories
    static std::shared_ptr<YamlNode> createAnchor(const std::string& name, std::shared_ptr<YamlNode> value);
    static std::shared_ptr<YamlNode> createAlias(const std::string& name);

    // Sequence operations
    void addToSequence(std::shared_ptr<YamlNode> node);
    
    // Mapping operations
    void setMapping(const std::string& key, std::shared_ptr<YamlNode> node);
};

} // namespace akao::core::engine::parser
