#include "v1.hpp"
#include <sstream>
#include <stdexcept>

namespace akao::core::engine::parser {

// Constructors
YamlNode::YamlNode() : type_(Type::UNDEFINED), value_(std::string{}) {}

YamlNode::YamlNode(const std::string& value) : type_(Type::STRING), value_(value) {}

YamlNode::YamlNode(int value) : type_(Type::INTEGER), value_(value) {}

YamlNode::YamlNode(bool value) : type_(Type::BOOLEAN), value_(value) {}

YamlNode::YamlNode(const std::vector<std::shared_ptr<YamlNode>>& sequence) 
    : type_(Type::SEQUENCE), value_(sequence) {}

YamlNode::YamlNode(const std::map<std::string, std::shared_ptr<YamlNode>>& mapping) 
    : type_(Type::MAPPING), value_(mapping) {}

// Type checking
YamlNode::Type YamlNode::getType() const {
    return type_;
}

bool YamlNode::isString() const {
    return type_ == Type::STRING;
}

bool YamlNode::isInteger() const {
    return type_ == Type::INTEGER;
}

bool YamlNode::isBoolean() const {
    return type_ == Type::BOOLEAN;
}

bool YamlNode::isSequence() const {
    return type_ == Type::SEQUENCE;
}

bool YamlNode::isMapping() const {
    return type_ == Type::MAPPING;
}

bool YamlNode::isDefined() const {
    return type_ != Type::UNDEFINED;
}

// Value accessors
std::string YamlNode::asString() const {
    if (type_ != Type::STRING) {
        throw std::runtime_error("YamlNode is not a string");
    }
    return std::get<std::string>(value_);
}

int YamlNode::asInteger() const {
    if (type_ != Type::INTEGER) {
        throw std::runtime_error("YamlNode is not an integer");
    }
    return std::get<int>(value_);
}

bool YamlNode::asBoolean() const {
    if (type_ != Type::BOOLEAN) {
        throw std::runtime_error("YamlNode is not a boolean");
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

// String representation
std::string YamlNode::toString() const {
    switch (type_) {
        case Type::STRING:
            return asString();
        case Type::INTEGER:
            return std::to_string(asInteger());
        case Type::BOOLEAN:
            return asBoolean() ? "true" : "false";
        case Type::SEQUENCE:
            return "[sequence with " + std::to_string(size()) + " elements]";
        case Type::MAPPING:
            return "{mapping with " + std::to_string(size()) + " keys}";
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

// Factory methods
std::shared_ptr<YamlNode> YamlNode::createString(const std::string& value) {
    return std::make_shared<YamlNode>(value);
}

std::shared_ptr<YamlNode> YamlNode::createInteger(int value) {
    return std::make_shared<YamlNode>(value);
}

std::shared_ptr<YamlNode> YamlNode::createBoolean(bool value) {
    return std::make_shared<YamlNode>(value);
}

std::shared_ptr<YamlNode> YamlNode::createSequence() {
    return std::make_shared<YamlNode>(std::vector<std::shared_ptr<YamlNode>>{});
}

std::shared_ptr<YamlNode> YamlNode::createMapping() {
    return std::make_shared<YamlNode>(std::map<std::string, std::shared_ptr<YamlNode>>{});
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
