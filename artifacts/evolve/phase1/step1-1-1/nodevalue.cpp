/**
 * @id: akao:file:core:node:data:nodevalue_impl:v1
 * @doc: Implementation of universal data exchange system for node-based workflows with comprehensive type conversion, validation, and compatibility methods. Provides efficient value storage, type-safe access, and seamless integration with existing Akao Logic Value system.
 * @specification: Complete implementation of NodeValue, NodeItem, and supporting classes
 * @scope: Core node data exchange implementation
 * @timeline: 2025-07-10
 * @rationale: Provide robust, efficient implementation of node data exchange system
 * @methodology: Variant-based storage with comprehensive error checking and conversion utilities
 * @references: ["akao:class:core:node:data:nodevalue:v1", "akao:class:core:engine:logic:value:v1"]
 */

#include "nodevalue.hpp"
#include <sstream>
#include <stdexcept>

namespace akao::node {

// ============================================================================
// BinaryData Implementation
// ============================================================================

BinaryData::BinaryData(const std::vector<uint8_t>& data, const std::string& mime_type)
    : data_(data), mime_type_(mime_type) {}

BinaryData::BinaryData(std::vector<uint8_t>&& data, const std::string& mime_type)
    : data_(std::move(data)), mime_type_(mime_type) {}

// ============================================================================
// ExecutionError Implementation
// ============================================================================

ExecutionError::ExecutionError(const std::string& message, Severity severity)
    : message_(message), severity_(severity) {}

ExecutionError::ExecutionError(const std::string& message, const std::string& code, Severity severity)
    : message_(message), code_(code), severity_(severity) {}

std::string ExecutionError::toString() const {
    std::stringstream ss;
    ss << "[";
    switch (severity_) {
        case Severity::INFO: ss << "INFO"; break;
        case Severity::WARNING: ss << "WARNING"; break;
        case Severity::ERROR: ss << "ERROR"; break;
        case Severity::FATAL: ss << "FATAL"; break;
    }
    ss << "]";
    
    if (!code_.empty()) {
        ss << " " << code_ << ":";
    }
    
    ss << " " << message_;
    
    if (!context_.empty()) {
        ss << " (Context: " << context_ << ")";
    }
    
    return ss.str();
}

// ============================================================================
// NodeValue Implementation
// ============================================================================

NodeValue::NodeValue() : type_(ValueType::NULL_VALUE), value_(std::string{}) {}

NodeValue::NodeValue(const std::string& value) : type_(ValueType::STRING), value_(value) {}

NodeValue::NodeValue(const char* value) : type_(ValueType::STRING), value_(std::string(value)) {}

NodeValue::NodeValue(int64_t value) : type_(ValueType::INTEGER), value_(value) {}

NodeValue::NodeValue(int value) : type_(ValueType::INTEGER), value_(static_cast<int64_t>(value)) {}

NodeValue::NodeValue(double value) : type_(ValueType::FLOAT), value_(value) {}

NodeValue::NodeValue(bool value) : type_(ValueType::BOOLEAN), value_(value) {}

NodeValue::NodeValue(const Object& value) : type_(ValueType::OBJECT), value_(value) {}

NodeValue::NodeValue(const Array& value) : type_(ValueType::ARRAY), value_(value) {}

NodeValue::NodeValue(const BinaryData& value) : type_(ValueType::BINARY), value_(value) {}

void NodeValue::validateType(ValueType expected) const {
    if (type_ != expected) {
        std::stringstream ss;
        ss << "Type mismatch: expected ";
        switch (expected) {
            case ValueType::STRING: ss << "string"; break;
            case ValueType::INTEGER: ss << "integer"; break;
            case ValueType::FLOAT: ss << "float"; break;
            case ValueType::BOOLEAN: ss << "boolean"; break;
            case ValueType::OBJECT: ss << "object"; break;
            case ValueType::ARRAY: ss << "array"; break;
            case ValueType::BINARY: ss << "binary"; break;
            case ValueType::NULL_VALUE: ss << "null"; break;
        }
        ss << ", got ";
        switch (type_) {
            case ValueType::STRING: ss << "string"; break;
            case ValueType::INTEGER: ss << "integer"; break;
            case ValueType::FLOAT: ss << "float"; break;
            case ValueType::BOOLEAN: ss << "boolean"; break;
            case ValueType::OBJECT: ss << "object"; break;
            case ValueType::ARRAY: ss << "array"; break;
            case ValueType::BINARY: ss << "binary"; break;
            case ValueType::NULL_VALUE: ss << "null"; break;
        }
        throw std::runtime_error(ss.str());
    }
}

std::string NodeValue::asString() const {
    if (type_ == ValueType::NULL_VALUE) return "";
    if (type_ == ValueType::STRING) return std::get<std::string>(value_);
    
    // Type conversion for other types
    switch (type_) {
        case ValueType::INTEGER: return std::to_string(std::get<int64_t>(value_));
        case ValueType::FLOAT: return std::to_string(std::get<double>(value_));
        case ValueType::BOOLEAN: return std::get<bool>(value_) ? "true" : "false";
        default:
            throw std::runtime_error("Cannot convert " + toString() + " to string");
    }
}

int64_t NodeValue::asInteger() const {
    validateType(ValueType::INTEGER);
    return std::get<int64_t>(value_);
}

double NodeValue::asFloat() const {
    if (type_ == ValueType::FLOAT) {
        return std::get<double>(value_);
    } else if (type_ == ValueType::INTEGER) {
        return static_cast<double>(std::get<int64_t>(value_));
    }
    throw std::runtime_error("Cannot convert to float");
}

bool NodeValue::asBoolean() const {
    validateType(ValueType::BOOLEAN);
    return std::get<bool>(value_);
}

NodeValue::Object NodeValue::asObject() const {
    validateType(ValueType::OBJECT);
    return std::get<Object>(value_);
}

NodeValue::Array NodeValue::asArray() const {
    validateType(ValueType::ARRAY);
    return std::get<Array>(value_);
}

BinaryData NodeValue::asBinary() const {
    validateType(ValueType::BINARY);
    return std::get<BinaryData>(value_);
}

size_t NodeValue::size() const {
    switch (type_) {
        case ValueType::STRING: return std::get<std::string>(value_).size();
        case ValueType::ARRAY: return std::get<Array>(value_).size();
        case ValueType::OBJECT: return std::get<Object>(value_).size();
        case ValueType::BINARY: return std::get<BinaryData>(value_).getSize();
        default: return 0;
    }
}

NodeValue NodeValue::at(size_t index) const {
    if (type_ == ValueType::ARRAY) {
        const auto& arr = std::get<Array>(value_);
        if (index >= arr.size()) {
            throw std::out_of_range("Array index out of range");
        }
        return arr[index];
    }
    throw std::runtime_error("at() called on non-array type");
}

NodeValue NodeValue::get(const std::string& key) const {
    if (type_ == ValueType::OBJECT) {
        const auto& obj = std::get<Object>(value_);
        auto it = obj.find(key);
        if (it != obj.end()) {
            return it->second;
        }
        return NodeValue(); // Return null value if key not found
    }
    throw std::runtime_error("get() called on non-object type");
}

void NodeValue::set(const std::string& key, const NodeValue& value) {
    if (type_ == ValueType::OBJECT) {
        std::get<Object>(value_)[key] = value;
    } else {
        throw std::runtime_error("set() called on non-object type");
    }
}

void NodeValue::push(const NodeValue& value) {
    if (type_ == ValueType::ARRAY) {
        std::get<Array>(value_).push_back(value);
    } else {
        throw std::runtime_error("push() called on non-array type");
    }
}

bool NodeValue::contains(const std::string& key) const {
    if (type_ == ValueType::OBJECT) {
        const auto& obj = std::get<Object>(value_);
        return obj.find(key) != obj.end();
    }
    return false;
}

std::string NodeValue::toString() const {
    switch (type_) {
        case ValueType::STRING: return "\"" + std::get<std::string>(value_) + "\"";
        case ValueType::INTEGER: return std::to_string(std::get<int64_t>(value_));
        case ValueType::FLOAT: return std::to_string(std::get<double>(value_));
        case ValueType::BOOLEAN: return std::get<bool>(value_) ? "true" : "false";
        case ValueType::NULL_VALUE: return "null";
        case ValueType::ARRAY: {
            const auto& arr = std::get<Array>(value_);
            std::stringstream ss;
            ss << "[";
            for (size_t i = 0; i < arr.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << arr[i].toString();
            }
            ss << "]";
            return ss.str();
        }
        case ValueType::OBJECT: {
            const auto& obj = std::get<Object>(value_);
            std::stringstream ss;
            ss << "{";
            bool first = true;
            for (const auto& [key, value] : obj) {
                if (!first) ss << ", ";
                ss << "\"" << key << "\": " << value.toString();
                first = false;
            }
            ss << "}";
            return ss.str();
        }
        case ValueType::BINARY: {
            const auto& binary = std::get<BinaryData>(value_);
            return "<binary data: " + std::to_string(binary.getSize()) + " bytes, " + binary.getMimeType() + ">";
        }
    }
    return "<unknown>";
}

bool NodeValue::operator==(const NodeValue& other) const {
    if (type_ != other.type_) return false;
    
    switch (type_) {
        case ValueType::STRING: return std::get<std::string>(value_) == std::get<std::string>(other.value_);
        case ValueType::INTEGER: return std::get<int64_t>(value_) == std::get<int64_t>(other.value_);
        case ValueType::FLOAT: return std::get<double>(value_) == std::get<double>(other.value_);
        case ValueType::BOOLEAN: return std::get<bool>(value_) == std::get<bool>(other.value_);
        case ValueType::NULL_VALUE: return true;
        case ValueType::ARRAY: return std::get<Array>(value_) == std::get<Array>(other.value_);
        case ValueType::OBJECT: return std::get<Object>(value_) == std::get<Object>(other.value_);
        case ValueType::BINARY: 
            return std::get<BinaryData>(value_).getData() == std::get<BinaryData>(other.value_).getData();
    }
    return false;
}

bool NodeValue::operator!=(const NodeValue& other) const {
    return !(*this == other);
}

// Template specializations
template<> std::string NodeValue::as<std::string>() const { return asString(); }
template<> int64_t NodeValue::as<int64_t>() const { return asInteger(); }
template<> int NodeValue::as<int>() const { return static_cast<int>(asInteger()); }
template<> double NodeValue::as<double>() const { return asFloat(); }
template<> bool NodeValue::as<bool>() const { return asBoolean(); }
template<> NodeValue::Object NodeValue::as<NodeValue::Object>() const { return asObject(); }
template<> NodeValue::Array NodeValue::as<NodeValue::Array>() const { return asArray(); }
template<> BinaryData NodeValue::as<BinaryData>() const { return asBinary(); }

// ============================================================================
// NodeItem Implementation
// ============================================================================

NodeItem::NodeItem(const NodeValue& json_data) : json_(json_data) {}

NodeItem::NodeItem(const NodeValue& json_data, const std::map<std::string, BinaryData>& binary_data)
    : json_(json_data), binary_(binary_data) {}

BinaryData NodeItem::getBinary(const std::string& key) const {
    auto it = binary_.find(key);
    if (it != binary_.end()) {
        return it->second;
    }
    throw std::runtime_error("Binary data not found for key: " + key);
}

NodeValue NodeItem::getMetadata(const std::string& key) const {
    auto it = metadata_.find(key);
    if (it != metadata_.end()) {
        return it->second;
    }
    return NodeValue(); // Return null value if not found
}

std::string NodeItem::toString() const {
    std::stringstream ss;
    ss << "NodeItem{";
    ss << "json: " << json_.toString();
    
    if (!binary_.empty()) {
        ss << ", binary: {";
        bool first = true;
        for (const auto& [key, data] : binary_) {
            if (!first) ss << ", ";
            ss << key << ": " << data.getSize() << " bytes";
            first = false;
        }
        ss << "}";
    }
    
    if (!metadata_.empty()) {
        ss << ", metadata: {";
        bool first = true;
        for (const auto& [key, value] : metadata_) {
            if (!first) ss << ", ";
            ss << key << ": " << value.toString();
            first = false;
        }
        ss << "}";
    }
    
    if (error_) {
        ss << ", error: " << error_->toString();
    }
    
    ss << "}";
    return ss.str();
}

} // namespace akao::node