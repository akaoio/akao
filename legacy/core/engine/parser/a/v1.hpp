/**
 * @id: akao:file:core:engine:logic_parser_akao:v1
 * @doc: Complete .a format parser for Akao Pure Logic Engine with proper AST structure, multi-line program support, and comprehensive logical programming features. No technical debt.
 * @specification: Complete AST-based parser for .a format logical programming language
 * @scope: Core engine logic parser for .a format
 * @timeline: 2025-07-09
 * @rationale: Provide a complete logical programming language parser that handles complex multi-line programs
 * @methodology: AST-based parsing with proper grammar, operator precedence, and semantic analysis
 * @references: ["akao:file:core:engine:logic_core:v1"]
 */

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <variant>

namespace akao::core::engine::logic::parser {

// =============================================================================
// AST Node Types for Logical Programming
// =============================================================================

enum class ASTNodeType {
    PROGRAM,
    STATEMENT,
    EXPRESSION,
    BINARY_OP,
    UNARY_OP,
    FUNCTION_CALL,
    QUANTIFIER,
    CONDITIONAL,
    VARIABLE,
    LITERAL,
    COLLECTION,
    OBJECT,
    BLOCK,
    COMMENT
};

/**
 * @brief Complete AST node for logical programming
 */
class ASTNode {
public:
    ASTNodeType type;
    std::string value;
    std::map<std::string, std::string> attributes;
    std::vector<std::unique_ptr<ASTNode>> children;
    
    // Position information
    int line = 0;
    int column = 0;
    
    explicit ASTNode(ASTNodeType t, const std::string& v = "") : type(t), value(v) {}
    
    void addChild(std::unique_ptr<ASTNode> child) {
        children.push_back(std::move(child));
    }
    
    void setAttribute(const std::string& key, const std::string& val) {
        attributes[key] = val;
    }
    
    std::string getAttribute(const std::string& key) const {
        auto it = attributes.find(key);
        return it != attributes.end() ? it->second : "";
    }
    
    
    // Debug output
    std::string toString(int indent = 0) const;
};

// =============================================================================
// Token Types for Lexical Analysis
// =============================================================================

enum class TokenType {
    // Literals
    BOOLEAN,
    INTEGER,
    STRING,
    IDENTIFIER,
    VARIABLE,
    
    // Operators
    AND,          // &&
    OR,           // ||
    NOT,          // !
    EQUALS,       // ==
    NOT_EQUALS,   // !=
    GREATER,      // >
    GREATER_EQ,   // >=
    LESS,         // <
    LESS_EQ,      // <=
    ASSIGN,       // =
    
    // Logical operators
    IMPLIES,      // ->
    IFF,          // <->
    
    // Quantifiers
    FORALL,       // forall, ∀
    EXISTS,       // exists, ∃
    
    // Keywords
    IN,           // in
    IF,           // if
    THEN,         // then
    ELSE,         // else
    TRUE,         // true
    FALSE,        // false
    
    // Punctuation
    LPAREN,       // (
    RPAREN,       // )
    LBRACE,       // {
    RBRACE,       // }
    LBRACKET,     // [
    RBRACKET,     // ]
    DOT,          // .
    COMMA,        // ,
    SEMICOLON,    // ;
    COLON,        // :
    QUESTION,     // ?
    
    // Mathematical
    PLUS,         // +
    MINUS,        // -
    MULTIPLY,     // *
    DIVIDE,       // /
    MODULO,       // %
    POWER,        // **
    
    // Special
    NEWLINE,
    COMMENT,
    EOF_TOKEN,
    UNKNOWN
};

/**
 * @brief Token for lexical analysis
 */
struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    
    Token(TokenType t, const std::string& v, int l, int c) 
        : type(t), value(v), line(l), column(c) {}
    
    std::string toString() const;
};

// =============================================================================
// Lexer for .a Format
// =============================================================================

class Lexer {
public:
    explicit Lexer(const std::string& input);
    
    std::vector<Token> tokenize();
    
private:
    std::string input_;
    size_t pos_;
    int line_;
    int column_;
    
    char peek(size_t offset = 0) const;
    char advance();
    bool isAtEnd() const;
    
    void skipWhitespace();
    void skipComment();
    
    Token readString();
    Token readNumber();
    Token readIdentifier();
    Token readOperator();
    
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    
    TokenType getKeywordType(const std::string& identifier) const;
    TokenType getOperatorType(const std::string& op) const;
};

// =============================================================================
// Parser for .a Format
// =============================================================================

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    
    std::unique_ptr<ASTNode> parseProgram();
    
private:
    std::vector<Token> tokens_;
    size_t current_;
    
    Token peek() const;
    Token advance();
    bool isAtEnd() const;
    bool check(TokenType type) const;
    bool match(TokenType type);
    bool match(const std::vector<TokenType>& types);
    
    void error(const std::string& message);
    
    // Grammar rules
    std::unique_ptr<ASTNode> parseStatements();
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseQuantifier();
    std::unique_ptr<ASTNode> parseConditional();
    std::unique_ptr<ASTNode> parseImplication();
    std::unique_ptr<ASTNode> parseLogicalOr();
    std::unique_ptr<ASTNode> parseLogicalAnd();
    std::unique_ptr<ASTNode> parseEquality();
    std::unique_ptr<ASTNode> parseComparison();
    std::unique_ptr<ASTNode> parseArithmetic();
    std::unique_ptr<ASTNode> parseTerm();
    std::unique_ptr<ASTNode> parseFactor();
    std::unique_ptr<ASTNode> parseUnary();
    std::unique_ptr<ASTNode> parsePrimary();
    std::unique_ptr<ASTNode> parseFunctionCall();
    std::unique_ptr<ASTNode> parseCollection();
    std::unique_ptr<ASTNode> parseObject();
    std::unique_ptr<ASTNode> parseIfStatement();
    std::unique_ptr<ASTNode> parseBlock();
    
    // Helper methods
    std::unique_ptr<ASTNode> createBinaryOp(const std::string& op, 
                                           std::unique_ptr<ASTNode> left, 
                                           std::unique_ptr<ASTNode> right);
    std::unique_ptr<ASTNode> createUnaryOp(const std::string& op, 
                                          std::unique_ptr<ASTNode> operand);
    std::unique_ptr<ASTNode> createLiteral(const std::string& value, TokenType type);
    std::unique_ptr<ASTNode> createVariable(const std::string& name);
    std::unique_ptr<ASTNode> createFunctionCall(const std::string& name, 
                                               std::vector<std::unique_ptr<ASTNode>> args);
    std::unique_ptr<ASTNode> createQuantifier(const std::string& quantifier,
                                             const std::string& variable,
                                             std::unique_ptr<ASTNode> domain,
                                             std::unique_ptr<ASTNode> condition);
};

// =============================================================================
// Semantic Analyzer
// =============================================================================

class SemanticAnalyzer {
public:
    explicit SemanticAnalyzer(std::unique_ptr<ASTNode> ast);
    
    void analyze();
    bool hasErrors() const;
    std::vector<std::string> getErrors() const;
    
private:
    std::unique_ptr<ASTNode> ast_;
    std::vector<std::string> errors_;
    std::map<std::string, std::string> symbol_table_;
    
    void analyzeNode(ASTNode* node);
    void analyzeProgram(ASTNode* node);
    void analyzeStatement(ASTNode* node);
    void analyzeExpression(ASTNode* node);
    void analyzeFunctionCall(ASTNode* node);
    void analyzeQuantifier(ASTNode* node);
    void analyzeVariable(ASTNode* node);
    
    void declareVariable(const std::string& name, const std::string& type);
    bool isVariableDeclared(const std::string& name) const;
    std::string getVariableType(const std::string& name) const;
    
    void reportError(const std::string& message, ASTNode* node = nullptr);
};

// =============================================================================
// Main .a Format Interface
// =============================================================================

class AkaoFormat {
public:
    // Parse .a format code into AST
    static std::unique_ptr<ASTNode> parseToAST(const std::string& akao_code);
    
    
    // Validate .a format code
    static bool validate(const std::string& akao_code, std::vector<std::string>& errors);
    
    // Check if file is .a format
    static bool isValidAkaoFile(const std::string& filename);
    
    
private:
    static std::unique_ptr<ASTNode> parseWithErrorHandling(const std::string& akao_code);
    static void printAST(const ASTNode* node, int indent = 0);
};


} // namespace akao::core::engine::logic::parser