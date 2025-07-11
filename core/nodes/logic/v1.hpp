/**
 * @id: akao:class:nodes:builtin:logic:v1
 * @doc: Clean .a format executor node providing logical expression evaluation capabilities. Inspired by legacy Pure Logic Engine architecture but implemented as standalone component with simplified syntax support, built-in functions, and zero external dependencies for powerful validation workflows.
 * @specification: .a format executor node with logical expression evaluation
 * @scope: Builtin logic execution node
 * @timeline: 2025-07-10
 * @rationale: Enable sophisticated logical expression evaluation for validation rules
 * @methodology: Clean implementation learning from legacy Pure Logic Engine patterns
 * @references: ["akao:interface:core:foundation:interfaces:inode:v1"]
 */

#pragma once

#include "core/foundation/interfaces/inode/v1.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <variant>
#include <functional>
#include <stdexcept>

namespace akao {
namespace nodes {
namespace builtin {

/**
 * Value type for logic execution
 */
using LogicValue = std::variant<
    bool,
    int64_t,
    double,
    std::string,
    std::vector<foundation::types::NodeValue>
>;

// =============================================================================
// AST Node Types for Full .a Format Support
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
 * Complete AST node for .a format parsing
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
 * Token for lexical analysis
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
// Complete .a Format Lexer
// =============================================================================

class AkaoLexer {
public:
    explicit AkaoLexer(const std::string& input);
    
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
// Complete .a Format Parser
// =============================================================================

class AkaoParser {
public:
    explicit AkaoParser(const std::vector<Token>& tokens);
    
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

/**
 * Enhanced execution context with proper variable scoping
 */
class LogicContext {
private:
    std::vector<std::map<std::string, LogicValue>> scopes_;
    std::map<std::string, std::function<LogicValue(const std::vector<LogicValue>&)>> functions_;
    const LogicContext* parent_;
    
public:
    LogicContext();
    LogicContext(const LogicContext& parent);
    ~LogicContext() = default;
    
    // Variable operations with scoping
    void setVariable(const std::string& name, const LogicValue& value);
    void bindVariable(const std::string& name, const LogicValue& value); // Alias for compatibility
    LogicValue getVariable(const std::string& name) const;
    bool hasVariable(const std::string& name) const;
    
    // Scope management
    void pushScope();
    void popScope();
    
    // Function operations
    void registerFunction(const std::string& name, std::function<LogicValue(const std::vector<LogicValue>&)> func);
    LogicValue callFunction(const std::string& name, const std::vector<LogicValue>& args) const;
    bool hasFunction(const std::string& name) const;
    
    // Utility
    std::vector<std::string> getVariableNames() const;
    std::vector<std::string> getFunctionNames() const;
};

/**
 * Exception for forall violations with specific failing values
 */
class ForallViolationException : public std::runtime_error {
public:
    std::vector<LogicValue> failing_values;
    std::string quantifier_variable;
    
    ForallViolationException(const std::string& message, 
                           const std::vector<LogicValue>& failures,
                           const std::string& var_name)
        : std::runtime_error(message), failing_values(failures), quantifier_variable(var_name) {}
};

/**
 * Complete AST-based runtime for .a format execution
 */
class AkaoRuntime {
private:
    LogicContext& context_;
    
    // Core AST execution
    LogicValue executeNode(const ASTNode& node);
    LogicValue executeProgram(const ASTNode& program);
    LogicValue executeStatement(const ASTNode& statement);
    LogicValue executeExpression(const ASTNode& expression);
    LogicValue executeBinaryOp(const ASTNode& binary_op);
    LogicValue executeUnaryOp(const ASTNode& unary_op);
    LogicValue executeFunctionCall(const ASTNode& func_call);
    LogicValue executeQuantifier(const ASTNode& quantifier);
    LogicValue executeConditional(const ASTNode& conditional);
    LogicValue executeVariable(const ASTNode& variable);
    LogicValue executeLiteral(const ASTNode& literal);
    LogicValue executeCollection(const ASTNode& collection);
    
    // Error handling
    void throwLogicError(const std::string& message, const ASTNode& context);
    
    // Helper methods
    bool valuesEqual(const LogicValue& a, const LogicValue& b);
    std::string nodeToString(const ASTNode& node);
    LogicValue nodeValueToLogicValue(const foundation::types::NodeValue& value) const;
    
    // AST execution methods
    LogicValue executeProgramNode(const ASTNode& node);
    LogicValue executeStatementNode(const ASTNode& node);
    LogicValue executeExpressionNode(const ASTNode& node);
    LogicValue executeBinaryOpNode(const ASTNode& node);
    LogicValue executeUnaryOpNode(const ASTNode& node);
    LogicValue executeFunctionCallNode(const ASTNode& node);
    LogicValue executeQuantifierNode(const ASTNode& node);
    LogicValue executeConditionalNode(const ASTNode& node);
    LogicValue executeVariableNode(const ASTNode& node);
    LogicValue executeLiteralNode(const ASTNode& node);
    LogicValue executeCollectionNode(const ASTNode& node);
    LogicValue executeObjectNode(const ASTNode& node);
    
    // LEGACY TEXT-BASED PARSER METHODS REMOVED - ONLY AST PARSER REMAINS
    
public:
    AkaoRuntime(LogicContext& context);
    
    // Main execution methods
    LogicValue execute(const std::string& akao_code);
    LogicValue execute(const ASTNode& ast);
    // LEGACY TEXT-BASED PARSER PUBLIC METHODS REMOVED - ONLY AST PARSER REMAINS
};

/**
 * Clean .a format executor node
 */
class AkaoLogicExecutorNode : public foundation::interfaces::INode {
    // Conversion utilities
    LogicValue nodeValueToLogicValue(const foundation::types::NodeValue& value) const;
    foundation::types::NodeValue logicValueToNodeValue(const LogicValue& value) const;
    void populateContextFromParameters(LogicContext& context, const foundation::interfaces::NodeParameters& params) const;

private:
    std::string nodeId_;
    std::string nodeType_;
    std::string version_;
    std::string description_;

public:
    // Built-in function library
    void initializeBuiltinFunctions(LogicContext& context) const;
    
    // Built-in functions (made public for testing)
    LogicValue func_string_length(const std::vector<LogicValue>& args) const;
    LogicValue func_string_concat(const std::vector<LogicValue>& args) const;
    LogicValue func_string_contains(const std::vector<LogicValue>& args) const;
    LogicValue func_string_starts_with(const std::vector<LogicValue>& args) const;
    LogicValue func_math_add(const std::vector<LogicValue>& args) const;
    LogicValue func_math_subtract(const std::vector<LogicValue>& args) const;
    LogicValue func_math_multiply(const std::vector<LogicValue>& args) const;
    LogicValue func_math_divide(const std::vector<LogicValue>& args) const;
    LogicValue func_math_power(const std::vector<LogicValue>& args) const;
    LogicValue func_math_modulo(const std::vector<LogicValue>& args) const;
    LogicValue func_collection_count(const std::vector<LogicValue>& args) const;
    LogicValue func_collection_contains(const std::vector<LogicValue>& args) const;
    LogicValue func_filesystem_exists(const std::vector<LogicValue>& args) const;
    LogicValue func_filesystem_is_file(const std::vector<LogicValue>& args) const;
    LogicValue func_filesystem_is_directory(const std::vector<LogicValue>& args) const;
    LogicValue func_filesystem_get_files(const std::vector<LogicValue>& args) const;
    LogicValue func_filesystem_get_cpp_files(const std::vector<LogicValue>& args) const;
    LogicValue func_filesystem_read_file(const std::vector<LogicValue>& args) const;
    LogicValue func_cpp_count_classes(const std::vector<LogicValue>& args) const;
    LogicValue func_peano_successor(const std::vector<LogicValue>& args) const;
    LogicValue func_peano_is_zero(const std::vector<LogicValue>& args) const;
    LogicValue func_peano_predecessor(const std::vector<LogicValue>& args) const;
    LogicValue func_metalogic_encode_formula(const std::vector<LogicValue>& args) const;
    LogicValue func_metalogic_self_reference(const std::vector<LogicValue>& args) const;
    
public:
    AkaoLogicExecutorNode();
    ~AkaoLogicExecutorNode() = default;
    
    // INode interface implementation
    std::string getNodeId() const override;
    std::string getNodeType() const override;
    std::string getVersion() const override;
    std::string getDescription() const override;
    
    foundation::interfaces::ValidationResult validate(const foundation::interfaces::NodeParameters& params) const override;
    foundation::types::ExecutionResult execute(const foundation::interfaces::NodeContext& context,
                                             const foundation::interfaces::NodeParameters& params) override;
    foundation::types::NodeValue getParameterSchema() const override;
    
    // Static utility methods for direct .a format execution
    static bool executeExpression(const std::string& expression);
    static bool executeExpressionWithContext(const std::string& expression, const std::map<std::string, foundation::types::NodeValue>& variables);
    static foundation::types::NodeValue evaluateExpression(const std::string& expression);
    static std::vector<std::string> validateExpression(const std::string& expression);
    static bool isValidExpression(const std::string& expression);
    
    // Full .a format parsing and execution
    static std::unique_ptr<ASTNode> parseAkaoCode(const std::string& akao_code);
    static LogicValue executeAkaoCode(const std::string& akao_code);
    static bool validateAkaoCode(const std::string& akao_code, std::vector<std::string>& errors);
};

} // namespace builtin
} // namespace nodes
} // namespace akao