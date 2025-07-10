/**
 * @id: akao:file:core:node:data:nodevalue:v1
 * @doc: Universal data exchange system for node-based workflow automation providing type-safe value containers, binary data handling, and seamless conversion with existing Akao Logic Value system. Enables standardized data flow between workflow nodes while maintaining compatibility with .a format expressions.
 * @specification: Type-safe data exchange containers for node-based workflow system
 * @scope: Core node infrastructure data exchange subsystem
 * @timeline: 2025-07-10
 * @rationale: Provide standardized data containers for node communication while preserving .a format logic integration
 * @methodology: Variant-based type system with explicit type checking and conversion utilities
 * @references: ["akao:class:core:node:data:nodevalue:v1", "akao:class:core:node:data:nodeitem:v1", "akao:class:core:engine:logic:value:v1"]
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <optional>
#include <memory>
#include <stdexcept>

// Forward declaration for compatibility with existing logic system
namespace akao::logic {
    class Value;
}

namespace akao::node {

/**
 * @id: akao:class:core:node:data:valuetype:v1
 * @doc: Type enumeration for NodeValue containers defining all supported data types for node communication including primitives, collections, objects, and binary data with null value support.
 * @specification: Type enumeration for node data exchange values
 * @scope: Core node data type system
 * @timeline: 2025-07-10
 * @rationale: Provide explicit type checking for node value containers
 * @methodology: Enumeration-based type classification with explicit null handling
 * @references: ["akao:class:core:node:data:nodevalue:v1"]
 */
enum class ValueType {
    STRING,
    INTEGER,
    FLOAT,
    BOOLEAN,
    OBJECT,
    ARRAY,
    BINARY,
    NULL_VALUE
};

/**
 * @id: akao:class:core:node:data:binarydata:v1
 * @doc: Binary data container for handling raw bytes, file contents, images, and other binary attachments in node workflows with MIME type support and efficient memory management for large data transfers.
 * @specification: Binary data container with MIME type and efficient storage
 * @scope: Core node binary data handling
 * @timeline: 2025-07-10
 * @rationale: Enable nodes to process files, images, and binary data efficiently
 * @methodology: Shared pointer for efficient memory management with metadata
 * @references: ["akao:class:core:node:data:nodeitem:v1"]
 */
class BinaryData {
public:
    BinaryData() = default;
    BinaryData(const std::vector<uint8_t>& data, const std::string& mime_type = "application/octet-stream");
    BinaryData(std::vector<uint8_t>&& data, const std::string& mime_type = "application/octet-stream");
    
    const std::vector<uint8_t>& getData() const { return data_; }
    const std::string& getMimeType() const { return mime_type_; }
    size_t getSize() const { return data_.size(); }
    bool isEmpty() const { return data_.empty(); }
    
    void setMimeType(const std::string& mime_type) { mime_type_ = mime_type; }

private:
    std::vector<uint8_t> data_;
    std::string mime_type_;
};

/**
 * @id: akao:class:core:node:data:executionerror:v1
 * @doc: Error information container for node execution failures providing detailed error messages, error codes, stack traces, and context information to enable proper error handling and debugging in workflow execution.
 * @specification: Node execution error container with detailed diagnostic information
 * @scope: Core node error handling system
 * @timeline: 2025-07-10
 * @rationale: Provide comprehensive error information for workflow debugging and recovery
 * @methodology: Structured error data with severity levels and context preservation
 * @references: ["akao:class:core:node:data:nodeitem:v1", "akao:class:core:node:execution:executionresult:v1"]
 */
class ExecutionError {
public:
    enum class Severity {
        INFO,
        WARNING,
        ERROR,
        FATAL
    };
    
    ExecutionError() = default;
    ExecutionError(const std::string& message, Severity severity = Severity::ERROR);
    ExecutionError(const std::string& message, const std::string& code, Severity severity = Severity::ERROR);
    
    const std::string& getMessage() const { return message_; }
    const std::string& getCode() const { return code_; }
    Severity getSeverity() const { return severity_; }
    const std::string& getContext() const { return context_; }
    
    void setContext(const std::string& context) { context_ = context; }
    std::string toString() const;

private:
    std::string message_;
    std::string code_;
    Severity severity_ = Severity::ERROR;
    std::string context_;
};

/**
 * @id: akao:class:core:node:data:nodevalue:v1
 * @doc: Universal value container for node data exchange supporting all primitive types, collections, objects, and binary data with type-safe access methods, conversion utilities, and compatibility with existing Akao Logic Value system for seamless .a format integration.
 * @specification: Type-safe universal value container for node communication
 * @scope: Core node data exchange system
 * @timeline: 2025-07-10
 * @rationale: Standardize data exchange between nodes while maintaining .a format compatibility
 * @methodology: Variant-based storage with explicit type checking and conversion methods
 * @references: ["akao:class:core:engine:logic:value:v1", "akao:class:core:node:data:nodeitem:v1"]
 */
class NodeValue {
public:
    using Object = std::map<std::string, NodeValue>;
    using Array = std::vector<NodeValue>;
    
    // Constructors for all supported types
    NodeValue();
    NodeValue(const std::string& value);
    NodeValue(const char* value);
    NodeValue(int64_t value);
    NodeValue(int value);
    NodeValue(double value);
    NodeValue(bool value);
    NodeValue(const Object& value);
    NodeValue(const Array& value);
    NodeValue(const BinaryData& value);
    
    // Type checking methods
    ValueType getType() const { return type_; }
    bool isString() const { return type_ == ValueType::STRING; }
    bool isInteger() const { return type_ == ValueType::INTEGER; }
    bool isFloat() const { return type_ == ValueType::FLOAT; }
    bool isNumber() const { return type_ == ValueType::INTEGER || type_ == ValueType::FLOAT; }
    bool isBoolean() const { return type_ == ValueType::BOOLEAN; }
    bool isObject() const { return type_ == ValueType::OBJECT; }
    bool isArray() const { return type_ == ValueType::ARRAY; }
    bool isBinary() const { return type_ == ValueType::BINARY; }
    bool isNull() const { return type_ == ValueType::NULL_VALUE; }
    
    // Type-safe value access with template specialization
    template<typename T>
    T as() const;
    
    // Convenience accessors
    std::string asString() const;
    int64_t asInteger() const;
    double asFloat() const;
    bool asBoolean() const;
    Object asObject() const;
    Array asArray() const;
    BinaryData asBinary() const;
    
    // Collection operations
    size_t size() const;
    NodeValue at(size_t index) const;
    NodeValue get(const std::string& key) const;
    void set(const std::string& key, const NodeValue& value);
    void push(const NodeValue& value);
    bool contains(const std::string& key) const;
    
    // String representation
    std::string toString() const;
    
    // Comparison operators
    bool operator==(const NodeValue& other) const;
    bool operator!=(const NodeValue& other) const;
    
    // Conversion utilities for compatibility with existing logic system
    static NodeValue fromLogicValue(const akao::logic::Value& value);
    akao::logic::Value toLogicValue() const;

private:
    ValueType type_;
    std::variant<std::string, int64_t, double, bool, Object, Array, BinaryData> value_;
    
    void validateType(ValueType expected) const;
};

/**
 * @id: akao:class:core:node:data:nodeitem:v1
 * @doc: Data item container for node workflow processing containing main JSON data payload, binary attachments, metadata, and error information. Represents a single unit of data flowing between nodes with comprehensive error handling and rich metadata support for debugging and tracing.
 * @specification: Workflow data item with payload, attachments, and metadata
 * @scope: Core node workflow data flow system
 * @timeline: 2025-07-10
 * @rationale: Provide comprehensive data containers for workflow item processing with error handling
 * @methodology: Structured data item with JSON payload, binary attachments, and metadata
 * @references: ["akao:class:core:node:data:nodevalue:v1", "akao:class:core:node:execution:executionresult:v1"]
 */
class NodeItem {
public:
    NodeItem() = default;
    explicit NodeItem(const NodeValue& json_data);
    NodeItem(const NodeValue& json_data, const std::map<std::string, BinaryData>& binary_data);
    
    // Main data payload access
    const NodeValue& getJson() const { return json_; }
    NodeValue& getJson() { return json_; }
    void setJson(const NodeValue& value) { json_ = value; }
    
    // Binary attachments
    const std::map<std::string, BinaryData>& getBinary() const { return binary_; }
    void setBinary(const std::string& key, const BinaryData& data) { binary_[key] = data; }
    bool hasBinary(const std::string& key) const { return binary_.find(key) != binary_.end(); }
    BinaryData getBinary(const std::string& key) const;
    
    // Metadata access
    const std::map<std::string, NodeValue>& getMetadata() const { return metadata_; }
    void setMetadata(const std::string& key, const NodeValue& value) { metadata_[key] = value; }
    NodeValue getMetadata(const std::string& key) const;
    bool hasMetadata(const std::string& key) const { return metadata_.find(key) != metadata_.end(); }
    
    // Error handling
    const std::optional<ExecutionError>& getError() const { return error_; }
    void setError(const ExecutionError& error) { error_ = error; }
    void clearError() { error_.reset(); }
    bool hasError() const { return error_.has_value(); }
    
    // Utility methods
    bool isEmpty() const { return json_.isNull() && binary_.empty() && metadata_.empty(); }
    std::string toString() const;

private:
    NodeValue json_;
    std::map<std::string, BinaryData> binary_;
    std::map<std::string, NodeValue> metadata_;
    std::optional<ExecutionError> error_;
};

// Template specializations for NodeValue::as<T>()
template<> std::string NodeValue::as<std::string>() const;
template<> int64_t NodeValue::as<int64_t>() const;
template<> int NodeValue::as<int>() const;
template<> double NodeValue::as<double>() const;
template<> bool NodeValue::as<bool>() const;
template<> NodeValue::Object NodeValue::as<NodeValue::Object>() const;
template<> NodeValue::Array NodeValue::as<NodeValue::Array>() const;
template<> BinaryData NodeValue::as<BinaryData>() const;

} // namespace akao::node