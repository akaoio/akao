/**
 * @id: akao:file:core:foundation:types:value:v1
 * @doc: Implementation of universal data exchange system for node workflows. Provides comprehensive type conversion, metadata management, and high-performance data operations. Supports all workflow data types with robust error handling and type safety.
 * @specification: NodeValue implementation with variant storage and operations
 * @scope: Core foundation data types implementation
 * @timeline: 2025-07-10
 * @rationale: Concrete implementation of universal data container for workflows
 * @methodology: Template specialization with robust type checking and conversions
 * @references: ["akao:class:core:foundation:types:nodevalue:v1"]
 */

#include "v1.hpp"
#include <stdexcept>
#include <sstream>

namespace akao {
namespace foundation {
namespace types {

// Constructors
NodeValue::NodeValue() : value_(nullptr) {}

NodeValue::NodeValue(const std::string& value) : value_(value) {}

NodeValue::NodeValue(const char* value) : value_(std::string(value)) {}

NodeValue::NodeValue(int64_t value) : value_(value) {}

NodeValue::NodeValue(double value) : value_(value) {}

NodeValue::NodeValue(bool value) : value_(value) {}

NodeValue::NodeValue(const Array& value) : value_(value) {}

NodeValue::NodeValue(const Object& value) : value_(value) {}

NodeValue::NodeValue(const BinaryData& value) : value_(value) {}

// Type checking
NodeValue::ValueType NodeValue::getType() const {
    return static_cast<ValueType>(value_.index());
}

bool NodeValue::isString() const { return std::holds_alternative<std::string>(value_); }
bool NodeValue::isInteger() const { return std::holds_alternative<int64_t>(value_); }
bool NodeValue::isDouble() const { return std::holds_alternative<double>(value_); }
bool NodeValue::isBoolean() const { return std::holds_alternative<bool>(value_); }
bool NodeValue::isArray() const { return std::holds_alternative<Array>(value_); }
bool NodeValue::isObject() const { return std::holds_alternative<Object>(value_); }
bool NodeValue::isBinary() const { return std::holds_alternative<BinaryData>(value_); }
bool NodeValue::isNull() const { return std::holds_alternative<std::nullptr_t>(value_); }

// Value extraction
const std::string& NodeValue::asString() const {
    if (!isString()) {
        throw std::runtime_error("NodeValue is not a string");
    }
    return std::get<std::string>(value_);
}

int64_t NodeValue::asInteger() const {
    if (!isInteger()) {
        throw std::runtime_error("NodeValue is not an integer");
    }
    return std::get<int64_t>(value_);
}

double NodeValue::asDouble() const {
    if (!isDouble()) {
        throw std::runtime_error("NodeValue is not a double");
    }
    return std::get<double>(value_);
}

bool NodeValue::asBoolean() const {
    if (!isBoolean()) {
        throw std::runtime_error("NodeValue is not a boolean");
    }
    return std::get<bool>(value_);
}

const NodeValue::Array& NodeValue::asArray() const {
    if (!isArray()) {
        throw std::runtime_error("NodeValue is not an array");
    }
    return std::get<Array>(value_);
}

const NodeValue::Object& NodeValue::asObject() const {
    if (!isObject()) {
        throw std::runtime_error("NodeValue is not an object");
    }
    return std::get<Object>(value_);
}

const NodeValue::BinaryData& NodeValue::asBinary() const {
    if (!isBinary()) {
        throw std::runtime_error("NodeValue is not binary data");
    }
    return std::get<BinaryData>(value_);
}

// Mutable access
std::string& NodeValue::asString() {
    if (!isString()) {
        throw std::runtime_error("NodeValue is not a string");
    }
    return std::get<std::string>(value_);
}

NodeValue::Array& NodeValue::asArray() {
    if (!isArray()) {
        throw std::runtime_error("NodeValue is not an array");
    }
    return std::get<Array>(value_);
}

NodeValue::Object& NodeValue::asObject() {
    if (!isObject()) {
        throw std::runtime_error("NodeValue is not an object");
    }
    return std::get<Object>(value_);
}

NodeValue::BinaryData& NodeValue::asBinary() {
    if (!isBinary()) {
        throw std::runtime_error("NodeValue is not binary data");
    }
    return std::get<BinaryData>(value_);
}

// Type conversion
std::string NodeValue::toString() const {
    switch (getType()) {
        case ValueType::STRING:
            return asString();
        case ValueType::INTEGER:
            return std::to_string(asInteger());
        case ValueType::DOUBLE:
            return std::to_string(asDouble());
        case ValueType::BOOLEAN:
            return asBoolean() ? "true" : "false";
        case ValueType::NULL_VALUE:
            return "null";
        default:
            throw std::runtime_error("Cannot convert complex type to string");
    }
}

int64_t NodeValue::toInteger() const {
    switch (getType()) {
        case ValueType::INTEGER:
            return asInteger();
        case ValueType::DOUBLE:
            return static_cast<int64_t>(asDouble());
        case ValueType::BOOLEAN:
            return asBoolean() ? 1 : 0;
        case ValueType::STRING: {
            try {
                return std::stoll(asString());
            } catch (const std::exception&) {
                throw std::runtime_error("Cannot convert string to integer");
            }
        }
        default:
            throw std::runtime_error("Cannot convert type to integer");
    }
}

double NodeValue::toDouble() const {
    switch (getType()) {
        case ValueType::DOUBLE:
            return asDouble();
        case ValueType::INTEGER:
            return static_cast<double>(asInteger());
        case ValueType::BOOLEAN:
            return asBoolean() ? 1.0 : 0.0;
        case ValueType::STRING: {
            try {
                return std::stod(asString());
            } catch (const std::exception&) {
                throw std::runtime_error("Cannot convert string to double");
            }
        }
        default:
            throw std::runtime_error("Cannot convert type to double");
    }
}

bool NodeValue::toBoolean() const {
    switch (getType()) {
        case ValueType::BOOLEAN:
            return asBoolean();
        case ValueType::INTEGER:
            return asInteger() != 0;
        case ValueType::DOUBLE:
            return asDouble() != 0.0;
        case ValueType::STRING:
            return !asString().empty();
        case ValueType::ARRAY:
            return !asArray().empty();
        case ValueType::OBJECT:
            return !asObject().empty();
        case ValueType::NULL_VALUE:
            return false;
        default:
            return true;
    }
}

// Metadata operations
void NodeValue::setMetadata(const std::string& key, const NodeValue& value) {
    metadata_[key] = value;
}

NodeValue NodeValue::getMetadata(const std::string& key) const {
    auto it = metadata_.find(key);
    return (it != metadata_.end()) ? it->second : NodeValue();
}

bool NodeValue::hasMetadata(const std::string& key) const {
    return metadata_.find(key) != metadata_.end();
}

void NodeValue::clearMetadata() {
    metadata_.clear();
}

const std::map<std::string, NodeValue>& NodeValue::getAllMetadata() const {
    return metadata_;
}

// Array operations
void NodeValue::push(const NodeValue& value) {
    if (!isArray()) {
        throw std::runtime_error("NodeValue is not an array");
    }
    asArray().push_back(value);
}

NodeValue& NodeValue::operator[](size_t index) {
    if (!isArray()) {
        throw std::runtime_error("NodeValue is not an array");
    }
    return asArray()[index];
}

const NodeValue& NodeValue::operator[](size_t index) const {
    if (!isArray()) {
        throw std::runtime_error("NodeValue is not an array");
    }
    return asArray()[index];
}

size_t NodeValue::size() const {
    if (isArray()) {
        return asArray().size();
    } else if (isObject()) {
        return asObject().size();
    } else if (isString()) {
        return asString().size();
    }
    throw std::runtime_error("NodeValue does not support size operation");
}

// Object operations
NodeValue& NodeValue::operator[](const std::string& key) {
    if (!isObject()) {
        throw std::runtime_error("NodeValue is not an object");
    }
    return asObject()[key];
}

const NodeValue& NodeValue::operator[](const std::string& key) const {
    if (!isObject()) {
        throw std::runtime_error("NodeValue is not an object");
    }
    return asObject().at(key);
}

bool NodeValue::hasKey(const std::string& key) const {
    if (!isObject()) {
        return false;
    }
    return asObject().find(key) != asObject().end();
}

// Operators
bool NodeValue::operator==(const NodeValue& other) const {
    if (getType() != other.getType()) {
        return false;
    }
    return value_ == other.value_;
}

bool NodeValue::operator!=(const NodeValue& other) const {
    return !(*this == other);
}

} // namespace types
} // namespace foundation
} // namespace akao