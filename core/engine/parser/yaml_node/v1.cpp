#include "v1.hpp"
#include <sstream>
#include <stdexcept>

namespace akao::core::engine::parser {

// Enhanced constructors
YamlNode::YamlNode() : type_(Type::UNDEFINED), value_(std::string{}) {}

YamlNode::YamlNode(const std::string& value) : type_(Type::STRING), value_(value) {}

YamlNode::YamlNode(int value) : type_(Type::INTEGER), value_(value) {}

YamlNode::YamlNode(double value) : type_(Type::FLOAT), value_(value) {}

YamlNode::YamlNode(bool value) : type_(Type::BOOLEAN), value_(value) {}

YamlNode::YamlNode(std::nullptr_t) : type_(Type::NULL_VALUE), value_(nullptr) {}

YamlNode::YamlNode(const std::vector<std::shared_ptr<YamlNode>>& sequence) 
    : type_(Type::SEQUENCE), value_(sequence) {}

YamlNode::YamlNode(const std::map<std::string, std::shared_ptr<YamlNode>>& mapping) 
    : type_(Type::MAPPING), value_(mapping) {}

// Copy and move constructors
YamlNode::YamlNode(const YamlNode& other) 
    : type_(other.type_), value_(other.value_), source_location_(other.source_location_), 
      anchor_name_(other.anchor_name_) {}

YamlNode::YamlNode(YamlNode&& other) noexcept 
    : type_(other.type_), value_(std::move(other.value_)), 
      source_location_(std::move(other.source_location_)), anchor_name_(std::move(other.anchor_name_)) {
    other.type_ = Type::UNDEFINED;
}

YamlNode& YamlNode::operator=(const YamlNode& other) {
    if (this != &other) {
        type_ = other.type_;
        value_ = other.value_;
        source_location_ = other.source_location_;
        anchor_name_ = other.anchor_name_;
    }
    return *this;
}

YamlNode& YamlNode::operator=(YamlNode&& other) noexcept {
    if (this != &other) {
        type_ = other.type_;
        value_ = std::move(other.value_);
        source_location_ = std::move(other.source_location_);
        anchor_name_ = std::move(other.anchor_name_);
        other.type_ = Type::UNDEFINED;
    }
    return *this;
}

// Enhanced type checking
YamlNode::Type YamlNode::getType() const {
    return type_;
}

bool YamlNode::isString() const {
    return type_ == Type::STRING;
}

bool YamlNode::isInteger() const {
    return type_ == Type::INTEGER;
}

bool YamlNode::isFloat() const {
    return type_ == Type::FLOAT;
}

bool YamlNode::isBoolean() const {
    return type_ == Type::BOOLEAN;
}

bool YamlNode::isNull() const {
    return type_ == Type::NULL_VALUE;
}

bool YamlNode::isSequence() const {
    return type_ == Type::SEQUENCE;
}

bool YamlNode::isMapping() const {
    return type_ == Type::MAPPING;
}

bool YamlNode::isAnchor() const {
    return type_ == Type::ANCHOR;
}

bool YamlNode::isAlias() const {
    return type_ == Type::ALIAS;
}

bool YamlNode::isDefined() const {
    return type_ != Type::UNDEFINED;
}

bool YamlNode::isScalar() const {
    return type_ == Type::STRING || type_ == Type::INTEGER || 
           type_ == Type::FLOAT || type_ == Type::BOOLEAN || type_ == Type::NULL_VALUE;
}

// Enhanced value accessors with safety
std::string YamlNode::asString() const {
    if (type_ != Type::STRING) {
        throw std::runtime_error("YamlNode is not a string");
    }
    return std::get<std::string>(value_);
}

std::string YamlNode::asStringOrDefault(const std::string& default_value) const {
    if (type_ != Type::STRING) {
        return default_value;
    }
    return std::get<std::string>(value_);
}

int YamlNode::asInteger() const {
    if (type_ != Type::INTEGER) {
        throw std::runtime_error("YamlNode is not an integer");
    }
    return std::get<int>(value_);
}

int YamlNode::asIntegerOrDefault(int default_value) const {
    if (type_ != Type::INTEGER) {
        return default_value;
    }
    return std::get<int>(value_);
}

double YamlNode::asFloat() const {
    if (type_ != Type::FLOAT) {
        throw std::runtime_error("YamlNode is not a float");
    }
    return std::get<double>(value_);
}

double YamlNode::asFloatOrDefault(double default_value) const {
    if (type_ != Type::FLOAT) {
        return default_value;
    }
    return std::get<double>(value_);
}

bool YamlNode::asBoolean() const {
    if (type_ != Type::BOOLEAN) {
        throw std::runtime_error("YamlNode is not a boolean");
    }
    return std::get<bool>(value_);
}

bool YamlNode::asBooleanOrDefault(bool default_value) const {
    if (type_ != Type::BOOLEAN) {
        return default_value;
    }
    return std::get<bool>(value_);
}

const std::vector<std::shared_ptr<YamlNode>>& YamlNode::asSequence() const {
    if (type_ != Type::SEQUENCE) {
        throw std::runtime_error("YamlNode is not a sequence");
    }
    return std::get<std::vector<std::shared_ptr<YamlNode>>>(value_);
}

const std::map<std::string, std::shared_ptr<YamlNode>>& YamlNode::asMapping() const {
    if (type_ != Type::MAPPING) {
        throw std::runtime_error("YamlNode is not a mapping");
    }
    return std::get<std::map<std::string, std::shared_ptr<YamlNode>>>(value_);
}

// Convenience accessors
std::shared_ptr<YamlNode> YamlNode::operator[](const std::string& key) const {
    if (type_ != Type::MAPPING) {
        return nullptr;
    }
    const auto& mapping = asMapping();
    auto it = mapping.find(key);
    return (it != mapping.end()) ? it->second : nullptr;
}

std::shared_ptr<YamlNode> YamlNode::operator[](size_t index) const {
    if (type_ != Type::SEQUENCE) {
        return nullptr;
    }
    const auto& sequence = asSequence();
    return (index < sequence.size()) ? sequence[index] : nullptr;
}

// Utility methods
bool YamlNode::hasKey(const std::string& key) const {
    if (type_ != Type::MAPPING) {
        return false;
    }
    const auto& mapping = asMapping();
    return mapping.find(key) != mapping.end();
}

size_t YamlNode::size() const {
    switch (type_) {
        case Type::SEQUENCE:
            return asSequence().size();
        case Type::MAPPING:
            return asMapping().size();
        case Type::STRING:
            return asString().length();
        default:
            return 0;
    }
}

std::vector<std::string> YamlNode::getKeys() const {
    std::vector<std::string> keys;
    if (type_ == Type::MAPPING) {
        const auto& mapping = asMapping();
        for (const auto& pair : mapping) {
            keys.push_back(pair.first);
        }
    }
    return keys;
}

// Enhanced string representation
std::string YamlNode::toString() const {
    switch (type_) {
        case Type::STRING:
            return asString();
        case Type::INTEGER:
            return std::to_string(asInteger());
        case Type::FLOAT:
            return std::to_string(asFloat());
        case Type::BOOLEAN:
            return asBoolean() ? "true" : "false";
        case Type::NULL_VALUE:
            return "null";
        case Type::SEQUENCE:
            return "[sequence with " + std::to_string(size()) + " elements]";
        case Type::MAPPING:
            return "{mapping with " + std::to_string(size()) + " keys}";
        case Type::ANCHOR:
            return "&" + anchor_name_;
        case Type::ALIAS:
            return "*" + std::get<std::string>(value_);
        default:
            return "undefined";
    }
}

std::string YamlNode::toYaml(int indent) const {
    std::string indentStr(indent, ' ');
    std::stringstream ss;

    switch (type_) {
        case Type::STRING:
            ss << asString();
            break;
        case Type::INTEGER:
            ss << asInteger();
            break;
        case Type::BOOLEAN:
            ss << (asBoolean() ? "true" : "false");
            break;
        case Type::SEQUENCE: {
            const auto& seq = asSequence();
            for (size_t i = 0; i < seq.size(); ++i) {
                if (i > 0) ss << "\n";
                ss << indentStr << "- " << seq[i]->toYaml(indent + 2);
            }
            break;
        }
        case Type::MAPPING: {
            const auto& mapping = asMapping();
            bool first = true;
            for (const auto& pair : mapping) {
                if (!first) ss << "\n";
                ss << indentStr << pair.first << ": " << pair.second->toYaml(indent + 2);
                first = false;
            }
            break;
        }
        default:
            ss << "null";
            break;
    }

    return ss.str();
}

// Enhanced factory methods
std::shared_ptr<YamlNode> YamlNode::createString(const std::string& value) {
    return std::make_shared<YamlNode>(value);
}

std::shared_ptr<YamlNode> YamlNode::createInteger(int value) {
    return std::make_shared<YamlNode>(value);
}

std::shared_ptr<YamlNode> YamlNode::createFloat(double value) {
    return std::make_shared<YamlNode>(value);
}

std::shared_ptr<YamlNode> YamlNode::createBoolean(bool value) {
    return std::make_shared<YamlNode>(value);
}

std::shared_ptr<YamlNode> YamlNode::createNull() {
    return std::make_shared<YamlNode>(nullptr);
}

std::shared_ptr<YamlNode> YamlNode::createSequence() {
    return std::make_shared<YamlNode>(std::vector<std::shared_ptr<YamlNode>>{});
}

std::shared_ptr<YamlNode> YamlNode::createMapping() {
    return std::make_shared<YamlNode>(std::map<std::string, std::shared_ptr<YamlNode>>{});
}

std::shared_ptr<YamlNode> YamlNode::createAnchor(const std::string& name, std::shared_ptr<YamlNode> value) {
    auto anchor = std::make_shared<YamlNode>();
    anchor->type_ = Type::ANCHOR;
    anchor->anchor_name_ = name;
    anchor->value_ = name; // Store the anchor name in the value
    return anchor;
}

std::shared_ptr<YamlNode> YamlNode::createAlias(const std::string& name) {
    auto alias = std::make_shared<YamlNode>();
    alias->type_ = Type::ALIAS;
    alias->value_ = name; // Store the alias name in the value
    return alias;
}

// Sequence operations
void YamlNode::addToSequence(std::shared_ptr<YamlNode> node) {
    if (type_ != Type::SEQUENCE) {
        throw std::runtime_error("Cannot add to non-sequence node");
    }
    auto& sequence = std::get<std::vector<std::shared_ptr<YamlNode>>>(value_);
    sequence.push_back(node);
}

// Mapping operations
void YamlNode::setMapping(const std::string& key, std::shared_ptr<YamlNode> node) {
    if (type_ != Type::MAPPING) {
        throw std::runtime_error("Cannot set mapping on non-mapping node");
    }
    auto& mapping = std::get<std::map<std::string, std::shared_ptr<YamlNode>>>(value_);
    mapping[key] = node;
}

} // namespace akao::core::engine::parser
