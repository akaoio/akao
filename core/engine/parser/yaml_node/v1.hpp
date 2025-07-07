#pragma once

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <memory>

namespace akao::core::engine::parser {

/**
 * @brief YAML Node representation for Akao framework
 * 
 * Pure C++ implementation without external dependencies
 * Supports nested structures, arrays, and basic YAML types
 * 
 * Philosophy Compliance:
 * - akao:philosophy::structure:isolation:v1 (one class per file)
 * - akao:philosophy::language:isolation:v1 (pure C++ only)
 */
class YamlNode {
public:
    enum class Type {
        UNDEFINED,
        STRING,
        INTEGER,
        BOOLEAN,
        SEQUENCE,
        MAPPING
    };

    using Value = std::variant<
        std::string,
        int,
        bool,
        std::vector<std::shared_ptr<YamlNode>>,
        std::map<std::string, std::shared_ptr<YamlNode>>
    >;

private:
    Type type_;
    Value value_;

public:
    // Constructors
    YamlNode();
    explicit YamlNode(const std::string& value);
    explicit YamlNode(int value);
    explicit YamlNode(bool value);
    explicit YamlNode(const std::vector<std::shared_ptr<YamlNode>>& sequence);
    explicit YamlNode(const std::map<std::string, std::shared_ptr<YamlNode>>& mapping);

    // Type checking
    Type getType() const;
    bool isString() const;
    bool isInteger() const;
    bool isBoolean() const;
    bool isSequence() const;
    bool isMapping() const;
    bool isDefined() const;

    // Value accessors
    std::string asString() const;
    int asInteger() const;
    bool asBoolean() const;
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

    // Factory methods
    static std::shared_ptr<YamlNode> createString(const std::string& value);
    static std::shared_ptr<YamlNode> createInteger(int value);
    static std::shared_ptr<YamlNode> createBoolean(bool value);
    static std::shared_ptr<YamlNode> createSequence();
    static std::shared_ptr<YamlNode> createMapping();

    // Sequence operations
    void addToSequence(std::shared_ptr<YamlNode> node);
    
    // Mapping operations
    void setMapping(const std::string& key, std::shared_ptr<YamlNode> node);
};

} // namespace akao::core::engine::parser
