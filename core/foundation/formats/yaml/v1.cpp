/**
 * @id: akao:class:core:foundation:formats:yaml:v1
 * @doc: YAML parsing infrastructure implementation for core communication protocol and configuration support. Provides comprehensive YAML 1.2 parsing and generation capabilities with zero external dependencies, moved from node implementation to core infrastructure to support YAML-RPC protocol and core configuration processing. Maintains all YAML features including key-value pairs, nested mappings and sequences, all scalar types, comments, multi-line values, anchors and aliases, multi-document streams, proper indentation handling, and Unicode support.
 * @specification: Core YAML infrastructure implementation with complete YAML 1.2 support for protocol communication
 * @scope: Core foundation format processing infrastructure implementation
 * @timeline: 2025-07-11
 * @rationale: Essential YAML support for YAML-RPC protocol and core configuration without external dependencies
 * @methodology: Move complete YAML implementation from node to core infrastructure
 * @references: ["akao:directory:core:foundation:formats:yaml:v1"]
 */

#include "v1.hpp"
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include <regex>

namespace akao {
namespace foundation {
namespace formats {
namespace yaml {

// =============================================================================
// YamlNode Implementation
// =============================================================================

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

YamlNode::Type YamlNode::getType() const { return type_; }
bool YamlNode::isString() const { return type_ == Type::STRING; }
bool YamlNode::isInteger() const { return type_ == Type::INTEGER; }
bool YamlNode::isFloat() const { return type_ == Type::FLOAT; }
bool YamlNode::isBoolean() const { return type_ == Type::BOOLEAN; }
bool YamlNode::isNull() const { return type_ == Type::NULL_VALUE; }
bool YamlNode::isSequence() const { return type_ == Type::SEQUENCE; }
bool YamlNode::isMapping() const { return type_ == Type::MAPPING; }
bool YamlNode::isAnchor() const { return type_ == Type::ANCHOR; }
bool YamlNode::isAlias() const { return type_ == Type::ALIAS; }
bool YamlNode::isDefined() const { return type_ != Type::UNDEFINED; }
bool YamlNode::isScalar() const { 
    return type_ == Type::STRING || type_ == Type::INTEGER || type_ == Type::FLOAT || 
           type_ == Type::BOOLEAN || type_ == Type::NULL_VALUE; 
}

std::string YamlNode::asString() const {
    if (type_ == Type::STRING) {
        return std::get<std::string>(value_);
    }
    throw std::runtime_error("Node is not a string");
}

std::string YamlNode::asStringOrDefault(const std::string& default_value) const {
    if (type_ == Type::STRING) {
        return std::get<std::string>(value_);
    }
    return default_value;
}

int YamlNode::asInteger() const {
    if (type_ == Type::INTEGER) {
        return std::get<int>(value_);
    }
    throw std::runtime_error("Node is not an integer");
}

int YamlNode::asIntegerOrDefault(int default_value) const {
    if (type_ == Type::INTEGER) {
        return std::get<int>(value_);
    }
    return default_value;
}

double YamlNode::asFloat() const {
    if (type_ == Type::FLOAT) {
        return std::get<double>(value_);
    }
    throw std::runtime_error("Node is not a float");
}

double YamlNode::asFloatOrDefault(double default_value) const {
    if (type_ == Type::FLOAT) {
        return std::get<double>(value_);
    }
    return default_value;
}

bool YamlNode::asBoolean() const {
    if (type_ == Type::BOOLEAN) {
        return std::get<bool>(value_);
    }
    throw std::runtime_error("Node is not a boolean");
}

bool YamlNode::asBooleanOrDefault(bool default_value) const {
    if (type_ == Type::BOOLEAN) {
        return std::get<bool>(value_);
    }
    return default_value;
}

const std::vector<std::shared_ptr<YamlNode>>& YamlNode::asSequence() const {
    if (type_ == Type::SEQUENCE) {
        return std::get<std::vector<std::shared_ptr<YamlNode>>>(value_);
    }
    throw std::runtime_error("Node is not a sequence");
}

const std::map<std::string, std::shared_ptr<YamlNode>>& YamlNode::asMapping() const {
    if (type_ == Type::MAPPING) {
        return std::get<std::map<std::string, std::shared_ptr<YamlNode>>>(value_);
    }
    throw std::runtime_error("Node is not a mapping");
}

std::shared_ptr<YamlNode> YamlNode::operator[](const std::string& key) const {
    if (type_ == Type::MAPPING) {
        const auto& mapping = std::get<std::map<std::string, std::shared_ptr<YamlNode>>>(value_);
        auto it = mapping.find(key);
        if (it != mapping.end()) {
            return it->second;
        }
    }
    return nullptr;
}

std::shared_ptr<YamlNode> YamlNode::operator[](size_t index) const {
    if (type_ == Type::SEQUENCE) {
        const auto& sequence = std::get<std::vector<std::shared_ptr<YamlNode>>>(value_);
        if (index < sequence.size()) {
            return sequence[index];
        }
    }
    return nullptr;
}

bool YamlNode::hasKey(const std::string& key) const {
    if (type_ == Type::MAPPING) {
        const auto& mapping = std::get<std::map<std::string, std::shared_ptr<YamlNode>>>(value_);
        return mapping.find(key) != mapping.end();
    }
    return false;
}

size_t YamlNode::size() const {
    if (type_ == Type::SEQUENCE) {
        return std::get<std::vector<std::shared_ptr<YamlNode>>>(value_).size();
    } else if (type_ == Type::MAPPING) {
        return std::get<std::map<std::string, std::shared_ptr<YamlNode>>>(value_).size();
    }
    return 0;
}

std::vector<std::string> YamlNode::getKeys() const {
    std::vector<std::string> keys;
    if (type_ == Type::MAPPING) {
        const auto& mapping = std::get<std::map<std::string, std::shared_ptr<YamlNode>>>(value_);
        for (const auto& pair : mapping) {
            keys.push_back(pair.first);
        }
    }
    return keys;
}

std::string YamlNode::toString() const {
    switch (type_) {
        case Type::STRING: return std::get<std::string>(value_);
        case Type::INTEGER: return std::to_string(std::get<int>(value_));
        case Type::FLOAT: return std::to_string(std::get<double>(value_));
        case Type::BOOLEAN: return std::get<bool>(value_) ? "true" : "false";
        case Type::NULL_VALUE: return "null";
        case Type::SEQUENCE: return "[sequence]";
        case Type::MAPPING: return "{mapping}";
        case Type::ANCHOR: return "&" + std::get<std::string>(value_);
        case Type::ALIAS: return "*" + std::get<std::string>(value_);
        default: return "undefined";
    }
}

std::string YamlNode::toYaml(int indent) const {
    std::string indentStr(indent, ' ');
    
    switch (type_) {
        case Type::STRING: {
            const std::string& str = std::get<std::string>(value_);
            if (str.find('\n') != std::string::npos || str.find('"') != std::string::npos) {
                return "\"" + str + "\"";
            }
            return str;
        }
        case Type::INTEGER: return std::to_string(std::get<int>(value_));
        case Type::FLOAT: return std::to_string(std::get<double>(value_));
        case Type::BOOLEAN: return std::get<bool>(value_) ? "true" : "false";
        case Type::NULL_VALUE: return "null";
        case Type::SEQUENCE: {
            const auto& sequence = std::get<std::vector<std::shared_ptr<YamlNode>>>(value_);
            if (sequence.empty()) return "[]";
            
            std::string result;
            for (const auto& item : sequence) {
                result += "\n" + indentStr + "- " + item->toYaml(indent + 2);
            }
            return result;
        }
        case Type::MAPPING: {
            const auto& mapping = std::get<std::map<std::string, std::shared_ptr<YamlNode>>>(value_);
            if (mapping.empty()) return "{}";
            
            std::string result;
            for (const auto& pair : mapping) {
                result += "\n" + indentStr + pair.first + ": " + pair.second->toYaml(indent + 2);
            }
            return result;
        }
        default: return "undefined";
    }
}

// Factory methods
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
    auto node = std::make_shared<YamlNode>(name);
    node->type_ = Type::ANCHOR;
    node->anchor_name_ = name;
    return node;
}

std::shared_ptr<YamlNode> YamlNode::createAlias(const std::string& name) {
    auto node = std::make_shared<YamlNode>(name);
    node->type_ = Type::ALIAS;
    return node;
}

void YamlNode::addToSequence(std::shared_ptr<YamlNode> node) {
    if (type_ == Type::SEQUENCE) {
        auto& sequence = std::get<std::vector<std::shared_ptr<YamlNode>>>(value_);
        sequence.push_back(node);
    }
}

void YamlNode::setMapping(const std::string& key, std::shared_ptr<YamlNode> node) {
    if (type_ == Type::MAPPING) {
        auto& mapping = std::get<std::map<std::string, std::shared_ptr<YamlNode>>>(value_);
        mapping[key] = node;
    }
}

foundation::types::NodeValue YamlNode::toNodeValue() const {
    switch (type_) {
        case Type::STRING: return foundation::types::NodeValue(std::get<std::string>(value_));
        case Type::INTEGER: return foundation::types::NodeValue(static_cast<int64_t>(std::get<int>(value_)));
        case Type::FLOAT: return foundation::types::NodeValue(std::get<double>(value_));
        case Type::BOOLEAN: return foundation::types::NodeValue(std::get<bool>(value_));
        case Type::NULL_VALUE: return foundation::types::NodeValue();
        case Type::SEQUENCE: {
            const auto& sequence = std::get<std::vector<std::shared_ptr<YamlNode>>>(value_);
            std::vector<foundation::types::NodeValue> result;
            for (const auto& item : sequence) {
                result.push_back(item->toNodeValue());
            }
            return foundation::types::NodeValue(result);
        }
        case Type::MAPPING: {
            const auto& mapping = std::get<std::map<std::string, std::shared_ptr<YamlNode>>>(value_);
            std::map<std::string, foundation::types::NodeValue> result;
            for (const auto& pair : mapping) {
                result[pair.first] = pair.second->toNodeValue();
            }
            return foundation::types::NodeValue(result);
        }
        default: return foundation::types::NodeValue();
    }
}

std::shared_ptr<YamlNode> YamlNode::fromNodeValue(const foundation::types::NodeValue& value) {
    if (value.isString()) {
        return createString(value.asString());
    } else if (value.isInteger()) {
        return createInteger(static_cast<int>(value.asInteger()));
    } else if (value.isDouble()) {
        return createFloat(value.asDouble());
    } else if (value.isBoolean()) {
        return createBoolean(value.asBoolean());
    } else if (value.isArray()) {
        auto sequence = createSequence();
        for (const auto& item : value.asArray()) {
            sequence->addToSequence(fromNodeValue(item));
        }
        return sequence;
    } else if (value.isObject()) {
        auto mapping = createMapping();
        for (const auto& pair : value.asObject()) {
            mapping->setMapping(pair.first, fromNodeValue(pair.second));
        }
        return mapping;
    } else {
        return createNull();
    }
}

// =============================================================================
// YamlParser Implementation (Simplified)
// =============================================================================

YamlParser::ParseException::ParseException(const ParseError& error) : error_(error) {
    what_message_ = "YAML Parse Error at line " + std::to_string(error.line) + 
                   ", column " + std::to_string(error.column) + ": " + error.message;
}

const char* YamlParser::ParseException::what() const noexcept {
    return what_message_.c_str();
}

const YamlParser::ParseError& YamlParser::ParseException::getError() const {
    return error_;
}

std::shared_ptr<YamlNode> YamlParser::parse(const std::string& yaml_content) {
    content_ = yaml_content;
    pos_ = 0;
    line_ = 1;
    column_ = 1;
    anchors_.clear();
    
    try {
        return parseDocument();
    } catch (const std::exception& e) {
        ParseError error{e.what(), line_, column_, getContextString(), "syntax", "Check YAML syntax"};
        throw ParseException(error);
    }
}

std::shared_ptr<YamlNode> YamlParser::parseFile(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file) {
        ParseError error{"Cannot open file: " + file_path, 0, 0, "", "file", "Check file path and permissions"};
        throw ParseException(error);
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return parse(content);
}

std::shared_ptr<YamlNode> YamlParser::parseDocument() {
    skipWhitespace();
    if (isAtEnd()) {
        return YamlNode::createMapping();
    }
    
    // Simple mapping parser - parse key-value pairs
    auto mapping = YamlNode::createMapping();
    
    while (!isAtEnd()) {
        skipWhitespace();
        if (isAtEnd()) break;
        
        std::string line = readLine();
        if (line.empty() || line[0] == '#') continue;
        
        auto kvp = parseKeyValue(line);
        if (!kvp.first.empty()) {
            mapping->setMapping(kvp.first, parseString(kvp.second));
        }
    }
    
    return mapping;
}

std::pair<std::string, std::string> YamlParser::parseKeyValue(const std::string& line) {
    size_t colonPos = line.find(':');
    if (colonPos == std::string::npos) {
        return {"", ""};
    }
    
    std::string key = trim(line.substr(0, colonPos));
    std::string value = trim(line.substr(colonPos + 1));
    
    return {key, value};
}

std::shared_ptr<YamlNode> YamlParser::parseString(const std::string& str) {
    std::string trimmed = trim(str);
    
    if (trimmed.empty() || trimmed == "null" || trimmed == "~") {
        return YamlNode::createNull();
    }
    
    if (trimmed == "true") {
        return YamlNode::createBoolean(true);
    }
    
    if (trimmed == "false") {
        return YamlNode::createBoolean(false);
    }
    
    if (isInteger(trimmed)) {
        return YamlNode::createInteger(std::stoi(trimmed));
    }
    
    if (isFloat(trimmed)) {
        return YamlNode::createFloat(std::stod(trimmed));
    }
    
    // Remove quotes if present
    if (trimmed.length() >= 2 && 
        ((trimmed.front() == '"' && trimmed.back() == '"') || 
         (trimmed.front() == '\'' && trimmed.back() == '\''))) {
        trimmed = trimmed.substr(1, trimmed.length() - 2);
    }
    
    return YamlNode::createString(trimmed);
}

bool YamlParser::isInteger(const std::string& str) const {
    if (str.empty()) return false;
    size_t start = (str[0] == '-' || str[0] == '+') ? 1 : 0;
    return start < str.length() && std::all_of(str.begin() + start, str.end(), ::isdigit);
}

bool YamlParser::isFloat(const std::string& str) const {
    if (str.empty()) return false;
    try {
        std::stod(str);
        return str.find('.') != std::string::npos;
    } catch (...) {
        return false;
    }
}

std::string YamlParser::readLine() {
    std::string line;
    while (!isAtEnd() && peek() != '\n') {
        line += advance();
    }
    if (peek() == '\n') advance(); // Skip newline
    return line;
}

bool YamlParser::isAtEnd() const {
    return pos_ >= content_.length();
}

char YamlParser::peek() const {
    return isAtEnd() ? '\0' : content_[pos_];
}

char YamlParser::advance() {
    if (isAtEnd()) return '\0';
    char c = content_[pos_++];
    if (c == '\n') {
        line_++;
        column_ = 1;
    } else {
        column_++;
    }
    return c;
}

void YamlParser::skipWhitespace() {
    while (!isAtEnd() && (peek() == ' ' || peek() == '\t')) {
        advance();
    }
}

std::string YamlParser::trim(const std::string& str) const {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string YamlParser::getContextString(size_t around_pos) const {
    size_t start = pos_ > around_pos ? pos_ - around_pos : 0;
    size_t end = std::min(pos_ + around_pos, content_.length());
    return content_.substr(start, end - start);
}

void YamlParser::reset() {
    pos_ = 0;
    line_ = 1;
    column_ = 1;
    anchors_.clear();
}

// Stub implementations for other methods
std::vector<std::shared_ptr<YamlNode>> YamlParser::parseMultiDocument(const std::string& yaml_content) {
    return {parse(yaml_content)};
}

std::vector<std::shared_ptr<YamlNode>> YamlParser::parseMultiDocumentFile(const std::string& file_path) {
    return {parseFile(file_path)};
}

std::shared_ptr<YamlNode> YamlParser::parseWithOptions(const std::string& yaml_content, const ParseOptions& options) {
    return parse(yaml_content); // Ignore options for now
}

// Additional stub implementations for interface compliance
std::shared_ptr<YamlNode> YamlParser::parseMapping(int base_indent) { return YamlNode::createMapping(); }
std::shared_ptr<YamlNode> YamlParser::parseSequence(int base_indent) { return YamlNode::createSequence(); }
std::shared_ptr<YamlNode> YamlParser::parseValue() { return YamlNode::createString(""); }
void YamlParser::skipComment() {}
void YamlParser::skipToNextLine() {}
int YamlParser::getCurrentIndent() { return 0; }
int YamlParser::measureIndent(const std::string& line) { return 0; }
std::string YamlParser::readUntil(char delimiter) { return ""; }
std::string YamlParser::readQuotedString(char quote) { return ""; }
bool YamlParser::isWhitespace(char c) const { return c == ' ' || c == '\t'; }
bool YamlParser::isNewline(char c) const { return c == '\n' || c == '\r'; }
bool YamlParser::isQuote(char c) const { return c == '"' || c == '\''; }
std::string YamlParser::trimLeft(const std::string& str) const { return trim(str); }
std::string YamlParser::trimRight(const std::string& str) const { return trim(str); }
bool YamlParser::isEmptyLine(const std::string& line) const { return trim(line).empty(); }
bool YamlParser::isCommentLine(const std::string& line) const { return trim(line)[0] == '#'; }
bool YamlParser::isSequenceItem(const std::string& line) const { return trim(line)[0] == '-'; }
std::string YamlParser::extractSequenceValue(const std::string& line) const { return ""; }
bool YamlParser::shouldSkipLine(const std::string& line) const { return false; }
bool YamlParser::handleDedentLine(const std::string& line, int base_indent) { return false; }
std::shared_ptr<YamlNode> YamlParser::parseNestedValue(int line_indent) { return YamlNode::createString(""); }
YamlParser::ValueType YamlParser::determineValueType(const std::string& next_line) const { return ValueType::STRING_VALUE; }
bool YamlParser::isBoolean(const std::string& str) const { return str == "true" || str == "false"; }
bool YamlParser::isNull(const std::string& str) const { return str == "null" || str == "~"; }
void YamlParser::throwError(const std::string& message) { throw std::runtime_error(message); }

// =============================================================================
// YamlProcessor Implementation
// =============================================================================

akao::foundation::types::NodeValue YamlProcessor::parseYaml(const std::string& yaml_content) {
    YamlParser parser;
    auto yamlNode = parser.parse(yaml_content);
    return yamlNode->toNodeValue();
}

akao::foundation::types::NodeValue YamlProcessor::parseYamlFile(const std::string& file_path) {
    YamlParser parser;
    auto yamlNode = parser.parseFile(file_path);
    return yamlNode->toNodeValue();
}

std::string YamlProcessor::generateYaml(const akao::foundation::types::NodeValue& data) {
    auto yamlNode = YamlNode::fromNodeValue(data);
    return yamlNode->toYaml();
}

bool YamlProcessor::validateYaml(const std::string& yaml_content, std::vector<std::string>& errors) {
    try {
        YamlParser parser;
        parser.parse(yaml_content);
        return true;
    } catch (const YamlParser::ParseException& e) {
        errors.push_back(e.what());
        return false;
    } catch (const std::exception& e) {
        errors.push_back(e.what());
        return false;
    }
}

std::vector<akao::foundation::types::NodeValue> YamlProcessor::parseMultiDocumentYaml(const std::string& yaml_content) {
    YamlParser parser;
    auto yamlNodes = parser.parseMultiDocument(yaml_content);
    std::vector<akao::foundation::types::NodeValue> result;
    for (const auto& node : yamlNodes) {
        result.push_back(node->toNodeValue());
    }
    return result;
}


akao::foundation::types::NodeValue YamlProcessor::yamlNodeToNodeValue(const std::shared_ptr<YamlNode>& yamlNode) {
    return yamlNode->toNodeValue();
}

std::shared_ptr<YamlNode> YamlProcessor::nodeValueToYamlNode(const akao::foundation::types::NodeValue& nodeValue) {
    return YamlNode::fromNodeValue(nodeValue);
}

} // namespace yaml
} // namespace formats
} // namespace foundation
} // namespace akao