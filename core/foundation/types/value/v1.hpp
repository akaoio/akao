/**
 * @id: akao:file:core:foundation:types:value:v1
 * @doc: Universal data exchange system for node-based workflows. Provides a type-safe container that can hold any data type commonly used in validation and analysis workflows, including strings, numbers, boolean values, binary data, and complex nested structures. Enables seamless data flow between different types of nodes regardless of their implementation language.
 * @specification: Universal data exchange value type for node workflows
 * @scope: Core foundation data types subsystem
 * @timeline: 2025-07-10
 * @rationale: Enable type-safe inter-node communication in workflow system
 * @methodology: C++ variant-based container with metadata and binary support
 * @references: ["akao:class:core:foundation:types:nodevalue:v1"]
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <memory>
#include <cstdint>

namespace akao {
namespace foundation {
namespace types {

/**
 * @id: akao:class:core:foundation:types:nodevalue:v1
 * @doc: Universal container for data exchange between workflow nodes. Supports all common data types including strings, numbers, booleans, arrays, objects, and binary data. Includes metadata support for workflow context and type information. Designed for high-performance data flow with minimal overhead.
 * @specification: Universal data container with variant storage and metadata
 * @scope: Core foundation data types
 * @timeline: 2025-07-10
 * @rationale: Type-safe data exchange between heterogeneous workflow nodes
 * @methodology: std::variant with metadata map and binary data support
 * @references: ["akao:class:core:foundation:types:executionresult:v1"]
 */
class NodeValue {
public:
    enum class ValueType {
        STRING,
        INTEGER,
        DOUBLE,
        BOOLEAN,
        ARRAY,
        OBJECT,
        BINARY,
        NULL_VALUE
    };

    using Array = std::vector<NodeValue>;
    using Object = std::map<std::string, NodeValue>;
    using BinaryData = std::vector<uint8_t>;

private:
    using ValueVariant = std::variant<
        std::string,
        int64_t,
        double,
        bool,
        Array,
        Object,
        BinaryData,
        std::nullptr_t
    >;

    ValueVariant value_;
    std::map<std::string, NodeValue> metadata_;

public:
    // Constructors
    NodeValue();
    NodeValue(const std::string& value);
    NodeValue(const char* value);
    NodeValue(int64_t value);
    NodeValue(double value);
    NodeValue(bool value);
    NodeValue(const Array& value);
    NodeValue(const Object& value);
    NodeValue(const BinaryData& value);

    // Type checking
    ValueType getType() const;
    bool isString() const;
    bool isInteger() const;
    bool isDouble() const;
    bool isBoolean() const;
    bool isArray() const;
    bool isObject() const;
    bool isBinary() const;
    bool isNull() const;

    // Value extraction
    const std::string& asString() const;
    int64_t asInteger() const;
    double asDouble() const;
    bool asBoolean() const;
    const Array& asArray() const;
    const Object& asObject() const;
    const BinaryData& asBinary() const;

    // Mutable access
    std::string& asString();
    Array& asArray();
    Object& asObject();
    BinaryData& asBinary();

    // Type conversion
    std::string toString() const;
    int64_t toInteger() const;
    double toDouble() const;
    bool toBoolean() const;

    // Metadata operations
    void setMetadata(const std::string& key, const NodeValue& value);
    NodeValue getMetadata(const std::string& key) const;
    bool hasMetadata(const std::string& key) const;
    void clearMetadata();
    const std::map<std::string, NodeValue>& getAllMetadata() const;

    // Array operations
    void push(const NodeValue& value);
    NodeValue& operator[](size_t index);
    const NodeValue& operator[](size_t index) const;
    size_t size() const;

    // Object operations
    NodeValue& operator[](const std::string& key);
    const NodeValue& operator[](const std::string& key) const;
    bool hasKey(const std::string& key) const;

    // Operators
    bool operator==(const NodeValue& other) const;
    bool operator!=(const NodeValue& other) const;
};

} // namespace types
} // namespace foundation
} // namespace akao