#pragma once

#include "../yaml_node/v1.hpp"
#include <string>
#include <fstream>
#include <memory>

namespace akao::core::engine::parser {

/**
 * @brief Pure C++ YAML Parser for Akao framework
 * 
 * NO EXTERNAL DEPENDENCIES - Pure C++ standard library only
 * Parses YAML files from philosophies/ and rules/ directories
 * 
 * Philosophy Compliance:
 * - akao:philosophy::structure:isolation:v1 (one class per file)
 * - akao:philosophy::language:isolation:v1 (pure C++ only)
 * - Built-in parser requirement (no yaml-cpp, no external libs)
 * 
 * Supported YAML Features:
 * - Key-value pairs
 * - Nested mappings
 * - Sequences (arrays)
 * - Strings, integers, booleans
 * - Comments (ignored)
 * - Multi-line values
 */
class YamlParser {
private:
    std::string content_;
    size_t pos_;
    size_t line_;
    size_t column_;

    // Parsing state
    struct ParseContext {
        int indent_level;
        bool in_sequence;
        bool in_mapping;
        std::string current_key;
    };

public:
    // Constructor
    YamlParser() = default;

    // Main parsing methods
    std::shared_ptr<YamlNode> parse(const std::string& yaml_content);
    std::shared_ptr<YamlNode> parseFile(const std::string& file_path);

    // Error handling
    struct ParseError {
        std::string message;
        size_t line;
        size_t column;
        std::string context;
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
    
    // Mapping parsing helpers (for improved separation of concerns)
    bool shouldSkipLine(const std::string& line) const;
    bool handleDedentLine(const std::string& line, int base_indent);
    std::shared_ptr<YamlNode> parseNestedValue(int line_indent);
    enum class ValueType { SEQUENCE, MAPPING, STRING_VALUE };
    ValueType determineValueType(const std::string& next_line) const;
    
    // Type detection
    bool isInteger(const std::string& str) const;
    bool isBoolean(const std::string& str) const;
    bool isNull(const std::string& str) const;
    
    // Error reporting
    void throwError(const std::string& message);
    std::string getContextString(size_t around_pos = 50) const;
    
    // Reset state
    void reset();
};

} // namespace akao::core::engine::parser
