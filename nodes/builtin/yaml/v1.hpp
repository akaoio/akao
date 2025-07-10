/**
 * @id: akao:class:nodes:builtin:yaml:v1
 * @doc: Production-ready YAML parser node providing comprehensive YAML 1.2 parsing and generation capabilities with zero external dependencies. Supports all YAML features including key-value pairs, nested mappings and sequences, all scalar types, comments, multi-line values, anchors and aliases, multi-document streams, proper indentation handling, and Unicode support. Integrates legacy YAML parser architecture into the node-based workflow system for reliable YAML processing.
 * @specification: YAML parser node with complete YAML 1.2 support using legacy architecture
 * @scope: Builtin YAML parsing and generation node
 * @timeline: 2025-07-10
 * @rationale: Enable powerful YAML processing in node-based workflows without external dependencies
 * @methodology: Adapt legacy YAML parser architecture to node interface with enhanced error handling
 * @references: ["akao:interface:core:foundation:interfaces:inode:v1"]
 */

#pragma once

#include "core/foundation/interfaces/inode/v1.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <variant>
#include <stdexcept>

namespace akao {
namespace nodes {
namespace builtin {

/**
 * YAML Node representation from legacy architecture
 */
class YamlNode {
public:
    enum class Type {
        UNDEFINED,
        STRING,
        INTEGER,
        FLOAT,
        BOOLEAN,
        NULL_VALUE,
        SEQUENCE,
        MAPPING,
        ANCHOR,
        ALIAS
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
    
    struct SourceLocation {
        size_t line = 0;
        size_t column = 0;
        std::string file_path;
    };

private:
    Type type_;
    Value value_;
    SourceLocation source_location_;
    std::string anchor_name_;
    
public:
    // Constructors
    YamlNode();
    explicit YamlNode(const std::string& value);
    explicit YamlNode(int value);
    explicit YamlNode(double value);
    explicit YamlNode(bool value);
    explicit YamlNode(std::nullptr_t);
    explicit YamlNode(const std::vector<std::shared_ptr<YamlNode>>& sequence);
    explicit YamlNode(const std::map<std::string, std::shared_ptr<YamlNode>>& mapping);
    
    // Copy and move constructors
    YamlNode(const YamlNode& other);
    YamlNode(YamlNode&& other) noexcept;
    YamlNode& operator=(const YamlNode& other);
    YamlNode& operator=(YamlNode&& other) noexcept;

    // Type checking
    Type getType() const;
    bool isString() const;
    bool isInteger() const;
    bool isFloat() const;
    bool isBoolean() const;
    bool isNull() const;
    bool isSequence() const;
    bool isMapping() const;
    bool isAnchor() const;
    bool isAlias() const;
    bool isDefined() const;
    bool isScalar() const;

    // Value accessors
    std::string asString() const;
    std::string asStringOrDefault(const std::string& default_value = "") const;
    int asInteger() const;
    int asIntegerOrDefault(int default_value = 0) const;
    double asFloat() const;
    double asFloatOrDefault(double default_value = 0.0) const;
    bool asBoolean() const;
    bool asBooleanOrDefault(bool default_value = false) const;
    const std::vector<std::shared_ptr<YamlNode>>& asSequence() const;
    const std::map<std::string, std::shared_ptr<YamlNode>>& asMapping() const;

    // Convenience accessors
    std::shared_ptr<YamlNode> operator[](const std::string& key) const;
    std::shared_ptr<YamlNode> operator[](size_t index) const;

    // Utility methods
    bool hasKey(const std::string& key) const;
    size_t size() const;
    std::vector<std::string> getKeys() const;
    std::string toString() const;
    std::string toYaml(int indent = 0) const;

    // Factory methods
    static std::shared_ptr<YamlNode> createString(const std::string& value);
    static std::shared_ptr<YamlNode> createInteger(int value);
    static std::shared_ptr<YamlNode> createFloat(double value);
    static std::shared_ptr<YamlNode> createBoolean(bool value);
    static std::shared_ptr<YamlNode> createNull();
    static std::shared_ptr<YamlNode> createSequence();
    static std::shared_ptr<YamlNode> createMapping();
    static std::shared_ptr<YamlNode> createAnchor(const std::string& name, std::shared_ptr<YamlNode> value);
    static std::shared_ptr<YamlNode> createAlias(const std::string& name);

    // Operations
    void addToSequence(std::shared_ptr<YamlNode> node);
    void setMapping(const std::string& key, std::shared_ptr<YamlNode> node);
    
    // Convert to NodeValue
    foundation::types::NodeValue toNodeValue() const;
    static std::shared_ptr<YamlNode> fromNodeValue(const foundation::types::NodeValue& value);
};

/**
 * YAML Parser from legacy architecture
 */
class YamlParser {
private:
    std::string content_;
    size_t pos_;
    size_t line_;
    size_t column_;

    struct ParseContext {
        int indent_level;
        bool in_sequence;
        bool in_mapping;
        std::string current_key;
        bool allow_anchors;
        bool multi_document;
        size_t document_count;
    };
    
    struct Anchor {
        std::string name;
        std::shared_ptr<YamlNode> value;
        size_t line;
        size_t column;
    };
    
    std::map<std::string, Anchor> anchors_;

public:
    YamlParser() = default;

    // Main parsing methods
    std::shared_ptr<YamlNode> parse(const std::string& yaml_content);
    std::shared_ptr<YamlNode> parseFile(const std::string& file_path);
    std::vector<std::shared_ptr<YamlNode>> parseMultiDocument(const std::string& yaml_content);
    std::vector<std::shared_ptr<YamlNode>> parseMultiDocumentFile(const std::string& file_path);
    
    struct ParseOptions {
        bool allow_anchors = true;
        bool allow_multi_document = true;
        bool strict_mode = false;
        bool preserve_comments = false;
        size_t max_depth = 100;
        size_t max_size = 10 * 1024 * 1024; // 10MB limit
    };
    
    std::shared_ptr<YamlNode> parseWithOptions(const std::string& yaml_content, const ParseOptions& options);
    
    struct ParseError {
        std::string message;
        size_t line;
        size_t column;
        std::string context;
        std::string error_type;
        std::string suggestion;
    };

    class ParseException : public std::exception {
    private:
        ParseError error_;
        std::string what_message_;

    public:
        explicit ParseException(const ParseError& error);
        const char* what() const noexcept override;
        const ParseError& getError() const;
    };

private:
    // Core parsing methods
    std::shared_ptr<YamlNode> parseDocument();
    std::shared_ptr<YamlNode> parseMapping(int base_indent);
    std::shared_ptr<YamlNode> parseSequence(int base_indent);
    std::shared_ptr<YamlNode> parseValue();
    std::shared_ptr<YamlNode> parseString(const std::string& str);

    // Utility methods
    void skipWhitespace();
    void skipComment();
    void skipToNextLine();
    int getCurrentIndent();
    int measureIndent(const std::string& line);
    std::string readLine();
    std::string readUntil(char delimiter);
    std::string readQuotedString(char quote);
    
    // Character checking
    bool isAtEnd() const;
    bool isWhitespace(char c) const;
    bool isNewline(char c) const;
    bool isQuote(char c) const;
    char peek() const;
    char advance();
    
    // Line processing
    std::string trimLeft(const std::string& str) const;
    std::string trimRight(const std::string& str) const;
    std::string trim(const std::string& str) const;
    bool isEmptyLine(const std::string& line) const;
    bool isCommentLine(const std::string& line) const;
    
    // Key-value parsing
    std::pair<std::string, std::string> parseKeyValue(const std::string& line);
    bool isSequenceItem(const std::string& line) const;
    std::string extractSequenceValue(const std::string& line) const;
    
    // Mapping parsing helpers
    bool shouldSkipLine(const std::string& line) const;
    bool handleDedentLine(const std::string& line, int base_indent);
    std::shared_ptr<YamlNode> parseNestedValue(int line_indent);
    enum class ValueType { SEQUENCE, MAPPING, STRING_VALUE };
    ValueType determineValueType(const std::string& next_line) const;
    
    // Type detection
    bool isInteger(const std::string& str) const;
    bool isFloat(const std::string& str) const;
    bool isBoolean(const std::string& str) const;
    bool isNull(const std::string& str) const;
    
    // Error reporting
    void throwError(const std::string& message);
    std::string getContextString(size_t around_pos = 50) const;
    void reset();
};

/**
 * YAML processor node for parsing and generating YAML
 */
class YamlProcessorNode : public foundation::interfaces::INode {
private:
    std::string nodeId_;
    std::string nodeType_;
    std::string version_;
    std::string description_;

    // Conversion utilities
    foundation::types::NodeValue yamlNodeToNodeValue(const std::shared_ptr<YamlNode>& yamlNode) const;
    std::shared_ptr<YamlNode> nodeValueToYamlNode(const foundation::types::NodeValue& nodeValue) const;

public:
    YamlProcessorNode();
    ~YamlProcessorNode() = default;
    
    // INode interface implementation
    std::string getNodeId() const override;
    std::string getNodeType() const override;
    std::string getVersion() const override;
    std::string getDescription() const override;
    
    foundation::interfaces::ValidationResult validate(const foundation::interfaces::NodeParameters& params) const override;
    foundation::types::ExecutionResult execute(const foundation::interfaces::NodeContext& context,
                                             const foundation::interfaces::NodeParameters& params) override;
    foundation::types::NodeValue getParameterSchema() const override;
    
    // Static utility methods for direct YAML processing
    static foundation::types::NodeValue parseYaml(const std::string& yaml_content);
    static foundation::types::NodeValue parseYamlFile(const std::string& file_path);
    static std::string generateYaml(const foundation::types::NodeValue& data);
    static bool validateYaml(const std::string& yaml_content, std::vector<std::string>& errors);
    static std::vector<foundation::types::NodeValue> parseMultiDocumentYaml(const std::string& yaml_content);
};

} // namespace builtin
} // namespace nodes
} // namespace akao