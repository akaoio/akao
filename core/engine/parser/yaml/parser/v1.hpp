/**
 * @id: akao:file:core:engine:parser_yaml_parser:v1
 * @doc: Production-ready YAML parser providing comprehensive YAML 1.2 parsing capabilities with zero external dependencies. Supports key-value pairs, nested mappings and sequences, all scalar types, comments, multi-line values, anchors and aliases, multi-document streams, proper indentation handling, and Unicode support. Features enhanced error recovery, detailed diagnostics, and libyaml-inspired architecture for reliable YAML processing in the Akao framework without requiring yaml-cpp or other external libraries.
 * @specification: Header for production-ready YAML parser with complete YAML 1.2 support and zero external dependencies
 * @scope: Core engine parser YAML parsing framework with comprehensive feature support
 * @timeline: 2025-07-09
 * @rationale: Provide reliable YAML parsing for philosophies and rules directories without external library dependencies
 * @methodology: Libyaml-inspired architecture with enhanced error handling, multi-document support, and comprehensive YAML 1.2 feature implementation
 * @references: ["akao:file:core:engine:parser_yaml_node:v1", "akao:philosophy:structure:isolation:v1", "akao:philosophy:language:isolation:v1"]
 */

#pragma once

#include "../node/v1.hpp"
#include <string>
#include <fstream>
#include <memory>

namespace akao::core::engine::parser {

/**
 * @brief Production-ready YAML Parser for Akao framework
 * 
 * ENHANCED VERSION - Inspired by libyaml architecture and best practices
 * NO EXTERNAL DEPENDENCIES - Pure C++ standard library only
 * Parses YAML files from philosophies/ and rules/ directories
 * 
 * Philosophy Compliance:
 * - akao:philosophy:structure:isolation:v1 (one class per file)
 * - akao:philosophy:language:isolation:v1 (pure C++ only)
 * - akao:philosophy:validation:universal:v1 (comprehensive validation)
 * - Built-in parser requirement (no yaml-cpp, no external libs)
 * 
 * YAML 1.2 Core Features Supported:
 * - Key-value pairs with proper escaping
 * - Nested mappings and sequences
 * - Sequences (arrays) with proper formatting
 * - All scalar types: strings, integers, floats, booleans, null
 * - Comments (properly ignored)
 * - Multi-line values (literal and folded)
 * - Anchors and aliases (references)
 * - Multi-document streams
 * - Proper indentation handling
 * - Unicode support
 * - Error recovery and detailed diagnostics
 */
class YamlParser {
private:
    std::string content_;
    size_t pos_;
    size_t line_;
    size_t column_;

    // Enhanced parsing state for production readiness
    struct ParseContext {
        int indent_level;
        bool in_sequence;
        bool in_mapping;
        std::string current_key;
        bool allow_anchors;
        bool multi_document;
        size_t document_count;
    };
    
    // Anchor and alias support (libyaml inspired)
    struct Anchor {
        std::string name;
        std::shared_ptr<YamlNode> value;
        size_t line;
        size_t column;
    };
    
    std::map<std::string, Anchor> anchors_;
    
    // Enhanced error tracking
    struct ErrorContext {
        std::string source_file;
        std::string source_content;
        std::vector<std::string> error_stack;
    };
    
    ErrorContext error_context_;

public:
    // Constructor
    YamlParser() = default;

    // Main parsing methods (enhanced)
    std::shared_ptr<YamlNode> parse(const std::string& yaml_content);
    std::shared_ptr<YamlNode> parseFile(const std::string& file_path);
    std::vector<std::shared_ptr<YamlNode>> parseMultiDocument(const std::string& yaml_content);
    std::vector<std::shared_ptr<YamlNode>> parseMultiDocumentFile(const std::string& file_path);
    
    // Advanced parsing options
    struct ParseOptions {
        bool allow_anchors = true;
        bool allow_multi_document = true;
        bool strict_mode = false;
        bool preserve_comments = false;
        size_t max_depth = 100;
        size_t max_size = 10 * 1024 * 1024; // 10MB limit
    };
    
    std::shared_ptr<YamlNode> parseWithOptions(const std::string& yaml_content, const ParseOptions& options);
    
    // Enhanced error handling
    struct ParseError {
        std::string message;
        size_t line;
        size_t column;
        std::string context;
        std::string error_type;  // "syntax", "semantic", "limit", "encoding"
        std::string suggestion;  // Helpful suggestion for fixing the error
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
    
    // Enhanced type detection
    bool isInteger(const std::string& str) const;
    bool isFloat(const std::string& str) const;      // Enhanced: Float detection
    bool isBoolean(const std::string& str) const;
    bool isNull(const std::string& str) const;
    
    // Error reporting
    void throwError(const std::string& message);
    std::string getContextString(size_t around_pos = 50) const;
    
    // Reset state
    void reset();
};

} // namespace akao::core::engine::parser
