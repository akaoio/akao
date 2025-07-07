#include "v1.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <stdexcept>

namespace akao::core::engine::parser {

// ParseException implementation
YamlParser::ParseException::ParseException(const ParseError& error) 
    : error_(error) {
    std::stringstream ss;
    ss << "YAML Parse Error at line " << error_.line << ", column " << error_.column 
       << ": " << error_.message;
    if (!error_.context.empty()) {
        ss << "\nContext: " << error_.context;
    }
    what_message_ = ss.str();
}

const char* YamlParser::ParseException::what() const noexcept {
    return what_message_.c_str();
}

const YamlParser::ParseError& YamlParser::ParseException::getError() const {
    return error_;
}

// Main parsing methods
std::shared_ptr<YamlNode> YamlParser::parse(const std::string& yaml_content) {
    reset();
    content_ = yaml_content;
    pos_ = 0;
    line_ = 1;
    column_ = 1;
    
    try {
        return parseDocument();
    } catch (const std::exception& e) {
        throwError("Failed to parse YAML: " + std::string(e.what()));
        return nullptr; // Never reached
    }
}

std::shared_ptr<YamlNode> YamlParser::parseFile(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throwError("Cannot open file: " + file_path);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    return parse(buffer.str());
}

// Core parsing methods
std::shared_ptr<YamlNode> YamlParser::parseDocument() {
    skipWhitespace();
    
    // Handle document start marker
    if (content_.substr(pos_, 3) == "---") {
        pos_ += 3;
        skipToNextLine();
    }
    
    // BUGFIX: Skip comments and empty lines after document marker before determining content type
    size_t saved_pos = pos_;
    while (!isAtEnd()) {
        std::string line = readLine();
        if (isEmptyLine(line) || isCommentLine(line)) {
            continue; // skip this line
        } else {
            // This is content - check if it's a sequence or mapping
            if (isSequenceItem(line)) {
                // Reset position and parse as sequence
                pos_ = saved_pos;
                return parseSequence(0);
            } else {
                // Reset position and parse as mapping
                pos_ = saved_pos;
                return parseMapping(0);
            }
        }
    }
    
    // If we get here, the document is empty, return empty mapping
    return parseMapping(0);
}

// Mapping parsing helper methods (for improved separation of concerns)
bool YamlParser::shouldSkipLine(const std::string& line) const {
    return isEmptyLine(line) || isCommentLine(line);
}

bool YamlParser::handleDedentLine(const std::string& line, int base_indent) {
    int line_indent = measureIndent(line);
    if (line_indent < base_indent) {
        // Dedent - return to previous level
        // Move position back to start of this line
        pos_ -= line.length() + 1;
        return true; // Signal to break from parsing loop
    }
    return false; // Continue parsing
}

std::shared_ptr<YamlNode> YamlParser::parseNestedValue(int line_indent) {
    // This method handles the same logic as the original nested parsing
    // but extracted for better separation of concerns
    skipWhitespace();
    if (!isAtEnd()) {
        std::string next_line = readLine();
        pos_ -= next_line.length() + 1; // Put it back
        
        int next_indent = measureIndent(next_line);
        if (next_indent > line_indent) {
            // Nested structure
            if (isSequenceItem(next_line)) {
                return parseSequence(next_indent);
            } else {
                return parseMapping(next_indent);
            }
        } else {
            return YamlNode::createString("");
        }
    } else {
        return YamlNode::createString("");
    }
}

YamlParser::ValueType YamlParser::determineValueType(const std::string& next_line) const {
    if (isSequenceItem(next_line)) {
        return ValueType::SEQUENCE;
    }
    
    // Check if line contains a key-value pair (has colon)
    if (next_line.find(':') != std::string::npos) {
        return ValueType::MAPPING;
    }
    
    return ValueType::STRING_VALUE;
}

std::shared_ptr<YamlNode> YamlParser::parseMapping(int base_indent) {
    auto mapping = YamlNode::createMapping();
    
    while (!isAtEnd()) {
        skipWhitespace();
        if (isAtEnd()) break;
        
        std::string line = readLine();
        
        // Skip empty lines and comments - extracted to helper for clarity
        if (shouldSkipLine(line)) {
            continue;
        }
        
        // Handle dedent (return to previous nesting level) - extracted to helper
        if (handleDedentLine(line, base_indent)) {
            break;
        }
        
        // Parse key-value pair
        auto [key, value_str] = parseKeyValue(line);
        if (key.empty()) {
            continue; // Skip invalid lines
        }
        
        std::shared_ptr<YamlNode> value_node;
        
        if (value_str.empty()) {
            // Multi-line value or nested structure - use original logic for now
            skipWhitespace();
            if (!isAtEnd()) {
                std::string next_line = readLine();
                pos_ -= next_line.length() + 1; // Put it back
                
                int line_indent = measureIndent(line);
                int next_indent = measureIndent(next_line);
                if (next_indent > line_indent) {
                    // Nested structure
                    if (isSequenceItem(next_line)) {
                        value_node = parseSequence(next_indent);
                    } else {
                        value_node = parseMapping(next_indent);
                    }
                } else {
                    value_node = YamlNode::createString("");
                }
            } else {
                value_node = YamlNode::createString("");
            }
        } else {
            value_node = parseString(value_str);
        }
        
        mapping->setMapping(key, value_node);
    }
    
    return mapping;
}

std::shared_ptr<YamlNode> YamlParser::parseSequence(int base_indent) {
    auto sequence = YamlNode::createSequence();
    
    while (!isAtEnd()) {
        skipWhitespace();
        if (isAtEnd()) break;
        
        std::string line = readLine();
        if (isEmptyLine(line) || isCommentLine(line)) {
            continue;
        }
        
        int line_indent = measureIndent(line);
        if (line_indent < base_indent) {
            // Dedent - return to previous level
            pos_ -= line.length() + 1;
            break;
        }
        
        if (!isSequenceItem(line)) {
            // Not a sequence item anymore
            pos_ -= line.length() + 1;
            break;
        }
        
        std::string value_str = extractSequenceValue(line);
        std::shared_ptr<YamlNode> value_node;
        
        if (value_str.empty()) {
            // Multi-line sequence item
            skipWhitespace();
            if (!isAtEnd()) {
                std::string next_line = readLine();
                pos_ -= next_line.length() + 1;
                
                int next_indent = measureIndent(next_line);
                if (next_indent > line_indent) {
                    if (isSequenceItem(next_line)) {
                        value_node = parseSequence(next_indent);
                    } else {
                        value_node = parseMapping(next_indent);
                    }
                } else {
                    value_node = YamlNode::createString("");
                }
            } else {
                value_node = YamlNode::createString("");
            }
        } else {
            value_node = parseString(value_str);
        }
        
        sequence->addToSequence(value_node);
    }
    
    return sequence;
}

std::shared_ptr<YamlNode> YamlParser::parseString(const std::string& str) {
    std::string trimmed = trim(str);
    
    if (isInteger(trimmed)) {
        return YamlNode::createInteger(std::stoi(trimmed));
    }
    
    // Enhanced: Check for float values
    if (isFloat(trimmed)) {
        return YamlNode::createFloat(std::stod(trimmed));
    }
    
    if (isBoolean(trimmed)) {
        return YamlNode::createBoolean(trimmed == "true" || trimmed == "True" || trimmed == "TRUE");
    }
    
    if (isNull(trimmed)) {
        return YamlNode::createNull();  // Enhanced: Use proper null creation
    }
    
    // Remove quotes if present
    if ((trimmed.front() == '"' && trimmed.back() == '"') ||
        (trimmed.front() == '\'' && trimmed.back() == '\'')) {
        trimmed = trimmed.substr(1, trimmed.length() - 2);
    }
    
    return YamlNode::createString(trimmed);
}

// Utility methods
void YamlParser::skipWhitespace() {
    while (!isAtEnd() && isWhitespace(peek()) && !isNewline(peek())) {
        advance();
    }
}

void YamlParser::skipComment() {
    if (peek() == '#') {
        skipToNextLine();
    }
}

void YamlParser::skipToNextLine() {
    while (!isAtEnd() && !isNewline(peek())) {
        advance();
    }
    if (!isAtEnd() && isNewline(peek())) {
        advance();
    }
}

int YamlParser::getCurrentIndent() {
    size_t start_pos = pos_;
    while (start_pos > 0 && content_[start_pos - 1] != '\n') {
        start_pos--;
    }
    
    int indent = 0;
    for (size_t i = start_pos; i < pos_ && i < content_.length(); ++i) {
        if (content_[i] == ' ') {
            indent++;
        } else if (content_[i] == '\t') {
            indent += 4; // Treat tab as 4 spaces
        } else {
            break;
        }
    }
    return indent;
}

int YamlParser::measureIndent(const std::string& line) {
    int indent = 0;
    for (char c : line) {
        if (c == ' ') {
            indent++;
        } else if (c == '\t') {
            indent += 4;
        } else {
            break;
        }
    }
    return indent;
}

std::string YamlParser::readLine() {
    size_t start = pos_;
    while (!isAtEnd() && !isNewline(peek())) {
        advance();
    }
    
    std::string line = content_.substr(start, pos_ - start);
    
    if (!isAtEnd() && isNewline(peek())) {
        advance(); // Skip newline
    }
    
    return line;
}

// Character checking
bool YamlParser::isAtEnd() const {
    return pos_ >= content_.length();
}

bool YamlParser::isWhitespace(char c) const {
    return c == ' ' || c == '\t';
}

bool YamlParser::isNewline(char c) const {
    return c == '\n' || c == '\r';
}

char YamlParser::peek() const {
    if (isAtEnd()) return '\0';
    return content_[pos_];
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

// String processing
std::string YamlParser::trimLeft(const std::string& str) const {
    auto start = str.find_first_not_of(" \t");
    return (start == std::string::npos) ? "" : str.substr(start);
}

std::string YamlParser::trimRight(const std::string& str) const {
    auto end = str.find_last_not_of(" \t");
    return (end == std::string::npos) ? "" : str.substr(0, end + 1);
}

std::string YamlParser::trim(const std::string& str) const {
    return trimLeft(trimRight(str));
}

bool YamlParser::isEmptyLine(const std::string& line) const {
    return trim(line).empty();
}

bool YamlParser::isCommentLine(const std::string& line) const {
    std::string trimmed = trimLeft(line);
    return !trimmed.empty() && trimmed[0] == '#';
}

// Key-value parsing
std::pair<std::string, std::string> YamlParser::parseKeyValue(const std::string& line) {
    size_t colon_pos = line.find(':');
    if (colon_pos == std::string::npos) {
        return {"", ""};
    }
    
    std::string key = trim(line.substr(0, colon_pos));
    std::string value = "";
    
    if (colon_pos + 1 < line.length()) {
        value = trim(line.substr(colon_pos + 1));
    }
    
    return {key, value};
}

bool YamlParser::isSequenceItem(const std::string& line) const {
    std::string trimmed = trimLeft(line);
    return !trimmed.empty() && trimmed[0] == '-' && 
           (trimmed.length() == 1 || isWhitespace(trimmed[1]));
}

std::string YamlParser::extractSequenceValue(const std::string& line) const {
    std::string trimmed = trimLeft(line);
    if (trimmed.length() <= 1) return "";
    
    return trim(trimmed.substr(1)); // Skip the '-'
}

// Type detection
bool YamlParser::isInteger(const std::string& str) const {
    if (str.empty()) return false;
    
    size_t start = 0;
    if (str[0] == '-' || str[0] == '+') start = 1;
    
    for (size_t i = start; i < str.length(); ++i) {
        if (!std::isdigit(str[i])) return false;
    }
    return start < str.length();
}

bool YamlParser::isFloat(const std::string& str) const {
    if (str.empty()) return false;
    
    bool has_dot = false;
    bool has_e = false;
    size_t start = 0;
    
    // Handle negative numbers
    if (str[0] == '-' || str[0] == '+') {
        start = 1;
        if (str.length() == 1) return false;
    }
    
    for (size_t i = start; i < str.length(); ++i) {
        char c = str[i];
        
        if (c == '.') {
            if (has_dot || has_e) return false; // Multiple dots or dot after e
            has_dot = true;
        } else if (c == 'e' || c == 'E') {
            if (has_e || i == start) return false; // Multiple e or e at start
            has_e = true;
            // Next character can be + or -
            if (i + 1 < str.length() && (str[i + 1] == '+' || str[i + 1] == '-')) {
                i++; // Skip the sign
                if (i + 1 >= str.length()) return false; // Must have digits after sign
            }
        } else if (!std::isdigit(c)) {
            return false;
        }
    }
    
    // Must have decimal point or scientific notation to be a float
    return has_dot || has_e;
}

bool YamlParser::isBoolean(const std::string& str) const {
    return str == "true" || str == "false" || 
           str == "True" || str == "False" ||
           str == "TRUE" || str == "FALSE" ||
           str == "yes" || str == "no" ||
           str == "Yes" || str == "No" ||
           str == "YES" || str == "NO";
}

bool YamlParser::isNull(const std::string& str) const {
    return str == "null" || str == "Null" || str == "NULL" || 
           str == "~" || str.empty();
}

// Error reporting
void YamlParser::throwError(const std::string& message) {
    ParseError error;
    error.message = message;
    error.line = line_;
    error.column = column_;
    error.context = getContextString();
    throw ParseException(error);
}

std::string YamlParser::getContextString(size_t around_pos) const {
    size_t start = (pos_ > around_pos) ? pos_ - around_pos : 0;
    size_t end = std::min(pos_ + around_pos, content_.length());
    return content_.substr(start, end - start);
}

void YamlParser::reset() {
    content_.clear();
    pos_ = 0;
    line_ = 1;
    column_ = 1;
}

} // namespace akao::core::engine::parser
