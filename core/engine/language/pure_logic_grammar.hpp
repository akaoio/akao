/**
 * akao:core:engine:language:pure-logic-grammar:v1
 * 
 * Pure Logic Grammar Implementation for .a Language
 * 
 * Specification: Complete formal grammar implementation for the Akao Pure Logic language
 *                with lexical analysis, parsing, and AST generation capabilities.
 * 
 * Scope: Lexical tokenization, syntactic parsing, semantic validation, and abstract
 *        syntax tree construction for .a language files.
 * 
 * Rationale: Provide comprehensive parsing infrastructure for Pure Logic language to enable
 *           compilation, interpretation, and analysis of .a programs.
 * 
 * Methodology: Recursive descent parser with precedence climbing for operators, 
 *             symbol table management, and comprehensive error reporting.
 * 
 * Integration: Symbol transformation system, type checker, logical reasoning engine,
 *             and code generation backends.
 */

#ifndef AKAO_CORE_ENGINE_LANGUAGE_PURE_LOGIC_GRAMMAR_HPP
#define AKAO_CORE_ENGINE_LANGUAGE_PURE_LOGIC_GRAMMAR_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <optional>
#include <variant>
#include <iostream>
#include <regex>

namespace akao {
namespace language {
namespace pure_logic {

// Forward declarations
class Token;
class Lexer;
class Parser;
class AST;

/**
 * Token types for lexical analysis
 */
enum class TokenType {
    // Literals
    INTEGER,
    REAL, 
    STRING,
    BOOLEAN,
    
    // Identifiers and keywords
    IDENTIFIER,
    QUALIFIED_IDENTIFIER,
    
    // Keywords
    LET,
    TYPE,
    PREDICATE,
    FACT,
    RULE,
    PROVE,
    QUERY,
    MATCH,
    IF,
    THEN,
    ELSE,
    WHERE,
    IMPORT,
    EXPORT,
    MODULE,
    FOREIGN,
    
    // Logical operators (Unicode)
    AND_SYMBOL,         // ∧
    OR_SYMBOL,          // ∨
    NOT_SYMBOL,         // ¬
    IMPLIES_SYMBOL,     // →
    IFF_SYMBOL,         // ↔
    XOR_SYMBOL,         // ⊕
    
    // Logical operators (ASCII)
    AND_KEYWORD,        // and
    OR_KEYWORD,         // or
    NOT_KEYWORD,        // not
    IMPLIES_KEYWORD,    // implies
    IFF_KEYWORD,        // iff
    XOR_KEYWORD,        // xor
    
    // Quantifiers (Unicode)
    FORALL_SYMBOL,      // ∀
    EXISTS_SYMBOL,      // ∃
    EXISTS_UNIQUE_SYMBOL, // ∃!
    
    // Quantifiers (ASCII)
    FORALL_KEYWORD,     // forall
    EXISTS_KEYWORD,     // exists
    EXISTS_UNIQUE_KEYWORD, // exists_unique
    
    // Set operators (Unicode)
    ELEMENT_OF,         // ∈
    NOT_ELEMENT_OF,     // ∉
    SUBSET,             // ⊆
    PROPER_SUBSET,      // ⊂
    UNION,              // ∪
    INTERSECTION,       // ∩
    DIFFERENCE,         // ∖
    
    // Set operators (ASCII)
    IN_KEYWORD,         // in
    NOT_IN_KEYWORD,     // not_in
    SUBSET_KEYWORD,     // subset
    PROPER_SUBSET_KEYWORD, // proper_subset
    UNION_KEYWORD,      // union
    INTERSECTION_KEYWORD, // intersection
    DIFFERENCE_KEYWORD, // difference
    
    // Comparison operators
    EQUAL,              // =
    NOT_EQUAL,          // ≠, !=
    LESS_THAN,          // <
    LESS_EQUAL,         // ≤, <=
    GREATER_THAN,       // >
    GREATER_EQUAL,      // ≥, >=
    APPROX_EQUAL,       // ≈, ~=
    
    // Arithmetic operators
    PLUS,               // +
    MINUS,              // -
    MULTIPLY,           // *
    DIVIDE,             // /
    MODULO,             // %
    POWER,              // ^, **
    
    // Punctuation
    LPAREN,             // (
    RPAREN,             // )
    LBRACKET,           // [
    RBRACKET,           // ]
    LBRACE,             // {
    RBRACE,             // }
    
    // Delimiters
    COMMA,              // ,
    SEMICOLON,          // ;
    COLON,              // :
    DOT,                // .
    DOUBLE_COLON,       // ::
    PIPE,               // |
    ARROW,              // →, ->
    THICK_ARROW,        // ⇒, =>
    
    // Assignment and binding
    ASSIGN,             // :=
    BINDING,            // =
    
    // Special symbols
    LAMBDA,             // λ, \
    TURNSTILE,          // ⊢
    DOUBLE_TURNSTILE,   // ⊨
    TOP,                // ⊤
    BOTTOM,             // ⊥
    INFINITY,           // ∞
    
    // Comments and whitespace
    COMMENT,
    WHITESPACE,
    NEWLINE,
    
    // Special tokens
    EOF_TOKEN,
    ERROR_TOKEN
};

/**
 * Source position information
 */
struct SourcePosition {
    std::string filename;
    size_t line;
    size_t column;
    size_t offset;
    
    SourcePosition(const std::string& file = "", size_t l = 1, size_t c = 1, size_t o = 0)
        : filename(file), line(l), column(c), offset(o) {}
};

/**
 * Lexical token
 */
class Token {
public:
    TokenType type;
    std::string value;
    SourcePosition position;
    
    Token(TokenType t, const std::string& v, const SourcePosition& pos)
        : type(t), value(v), position(pos) {}
    
    bool is_logical_operator() const;
    bool is_quantifier() const;
    bool is_set_operator() const;
    bool is_comparison_operator() const;
    bool is_keyword() const;
    
    std::string to_string() const;
};

/**
 * Abstract Syntax Tree node types
 */
enum class ASTNodeType {
    // Literals
    INTEGER_LITERAL,
    REAL_LITERAL,
    STRING_LITERAL,
    BOOLEAN_LITERAL,
    
    // Identifiers
    IDENTIFIER_NODE,
    QUALIFIED_IDENTIFIER_NODE,
    
    // Expressions
    BINARY_EXPRESSION,
    UNARY_EXPRESSION,
    APPLICATION_EXPRESSION,
    LAMBDA_EXPRESSION,
    CONDITIONAL_EXPRESSION,
    MATCH_EXPRESSION,
    
    // Logical expressions
    LOGICAL_AND,
    LOGICAL_OR,
    LOGICAL_NOT,
    LOGICAL_IMPLIES,
    LOGICAL_IFF,
    LOGICAL_XOR,
    
    // Quantified expressions
    UNIVERSAL_QUANTIFICATION,
    EXISTENTIAL_QUANTIFICATION,
    UNIQUE_EXISTENCE,
    
    // Statements
    LET_BINDING,
    TYPE_DEFINITION,
    PREDICATE_DEFINITION,
    FACT_DECLARATION,
    RULE_DECLARATION,
    PROOF_BLOCK,
    
    // Patterns
    IDENTIFIER_PATTERN,
    TUPLE_PATTERN,
    LIST_PATTERN,
    RECORD_PATTERN,
    
    // Types
    PRIMITIVE_TYPE,
    FUNCTION_TYPE,
    PRODUCT_TYPE,
    SUM_TYPE,
    LIST_TYPE,
    SET_TYPE,
    MAP_TYPE,
    
    // Program structure
    MODULE_DECLARATION,
    IMPORT_DECLARATION,
    EXPORT_DECLARATION,
    PROGRAM
};

/**
 * AST Node base class
 */
class ASTNode {
public:
    ASTNodeType node_type;
    SourcePosition position;
    
    ASTNode(ASTNodeType type, const SourcePosition& pos)
        : node_type(type), position(pos) {}
    
    virtual ~ASTNode() = default;
    virtual std::string to_string() const = 0;
    virtual void accept_visitor(class ASTVisitor& visitor) = 0;
};

/**
 * Expression AST nodes
 */
class Expression : public ASTNode {
public:
    Expression(ASTNodeType type, const SourcePosition& pos)
        : ASTNode(type, pos) {}
};

class LiteralExpression : public Expression {
public:
    std::variant<int64_t, double, std::string, bool> value;
    
    LiteralExpression(ASTNodeType type, const SourcePosition& pos, auto val)
        : Expression(type, pos), value(val) {}
    
    std::string to_string() const override;
    void accept_visitor(ASTVisitor& visitor) override;
};

class IdentifierExpression : public Expression {
public:
    std::string name;
    std::vector<std::string> namespace_parts;
    
    IdentifierExpression(const SourcePosition& pos, const std::string& id)
        : Expression(ASTNodeType::IDENTIFIER_NODE, pos), name(id) {}
    
    std::string to_string() const override;
    void accept_visitor(ASTVisitor& visitor) override;
};

class BinaryExpression : public Expression {
public:
    std::unique_ptr<Expression> left;
    TokenType operator_type;
    std::unique_ptr<Expression> right;
    
    BinaryExpression(const SourcePosition& pos, 
                    std::unique_ptr<Expression> l,
                    TokenType op,
                    std::unique_ptr<Expression> r)
        : Expression(ASTNodeType::BINARY_EXPRESSION, pos)
        , left(std::move(l))
        , operator_type(op)
        , right(std::move(r)) {}
    
    std::string to_string() const override;
    void accept_visitor(ASTVisitor& visitor) override;
};

class UnaryExpression : public Expression {
public:
    TokenType operator_type;
    std::unique_ptr<Expression> operand;
    
    UnaryExpression(const SourcePosition& pos,
                   TokenType op,
                   std::unique_ptr<Expression> expr)
        : Expression(ASTNodeType::UNARY_EXPRESSION, pos)
        , operator_type(op)
        , operand(std::move(expr)) {}
    
    std::string to_string() const override;
    void accept_visitor(ASTVisitor& visitor) override;
};

class QuantifiedExpression : public Expression {
public:
    TokenType quantifier_type;
    std::vector<std::string> variables;
    std::unique_ptr<Expression> domain;  // Optional domain restriction
    std::unique_ptr<Expression> body;
    
    QuantifiedExpression(const SourcePosition& pos,
                        TokenType quant,
                        const std::vector<std::string>& vars,
                        std::unique_ptr<Expression> body_expr)
        : Expression(ASTNodeType::UNIVERSAL_QUANTIFICATION, pos)
        , quantifier_type(quant)
        , variables(vars)
        , body(std::move(body_expr)) {}
    
    std::string to_string() const override;
    void accept_visitor(ASTVisitor& visitor) override;
};

/**
 * Statement AST nodes
 */
class Statement : public ASTNode {
public:
    Statement(ASTNodeType type, const SourcePosition& pos)
        : ASTNode(type, pos) {}
};

class LetBinding : public Statement {
public:
    std::string identifier;
    std::unique_ptr<Expression> type_annotation;
    std::unique_ptr<Expression> value;
    
    LetBinding(const SourcePosition& pos,
              const std::string& id,
              std::unique_ptr<Expression> val)
        : Statement(ASTNodeType::LET_BINDING, pos)
        , identifier(id)
        , value(std::move(val)) {}
    
    std::string to_string() const override;
    void accept_visitor(ASTVisitor& visitor) override;
};

class PredicateDefinition : public Statement {
public:
    std::string name;
    std::vector<std::pair<std::string, std::unique_ptr<Expression>>> parameters;
    std::unique_ptr<Expression> return_type;
    std::unique_ptr<Expression> body;
    
    PredicateDefinition(const SourcePosition& pos, const std::string& pred_name)
        : Statement(ASTNodeType::PREDICATE_DEFINITION, pos), name(pred_name) {}
    
    std::string to_string() const override;
    void accept_visitor(ASTVisitor& visitor) override;
};

/**
 * Lexical analyzer
 */
class Lexer {
public:
    explicit Lexer(const std::string& source, const std::string& filename = "");
    
    Token next_token();
    Token peek_token();
    bool has_more_tokens() const;
    
    SourcePosition get_position() const;
    std::vector<Token> tokenize_all();

private:
    std::string source_;
    std::string filename_;
    size_t position_;
    size_t line_;
    size_t column_;
    
    std::map<std::string, TokenType> keywords_;
    std::map<std::string, TokenType> symbols_;
    
    void initialize_keywords();
    void initialize_symbols();
    
    char current_char() const;
    char peek_char(size_t offset = 1) const;
    void advance();
    void skip_whitespace();
    void skip_comment();
    
    Token read_number();
    Token read_string();
    Token read_identifier();
    Token read_symbol();
    
    bool is_digit(char c) const;
    bool is_alpha(char c) const;
    bool is_alnum(char c) const;
    bool is_whitespace(char c) const;
};

/**
 * Operator precedence information
 */
struct OperatorInfo {
    int precedence;
    enum Associativity { LEFT, RIGHT, NONE } associativity;
    bool is_prefix;
    bool is_postfix;
};

/**
 * Recursive descent parser
 */
class Parser {
public:
    explicit Parser(std::vector<Token> tokens);
    
    std::unique_ptr<ASTNode> parse_program();
    std::unique_ptr<Expression> parse_expression();
    std::unique_ptr<Statement> parse_statement();
    
    bool has_errors() const { return !errors_.empty(); }
    const std::vector<std::string>& get_errors() const { return errors_; }

private:
    std::vector<Token> tokens_;
    size_t current_;
    std::vector<std::string> errors_;
    std::map<TokenType, OperatorInfo> operator_info_;
    
    void initialize_operator_precedence();
    
    Token current_token() const;
    Token peek_token(size_t offset = 1) const;
    bool match(TokenType type);
    bool check(TokenType type) const;
    void advance();
    void synchronize();
    
    void error(const std::string& message);
    void error_at_current(const std::string& message);
    void error_at_previous(const std::string& message);
    
    // Expression parsing
    std::unique_ptr<Expression> parse_logical_expression();
    std::unique_ptr<Expression> parse_quantified_expression();
    std::unique_ptr<Expression> parse_binary_expression(int min_precedence = 0);
    std::unique_ptr<Expression> parse_unary_expression();
    std::unique_ptr<Expression> parse_primary_expression();
    std::unique_ptr<Expression> parse_application();
    std::unique_ptr<Expression> parse_conditional();
    std::unique_ptr<Expression> parse_match_expression();
    
    // Statement parsing
    std::unique_ptr<Statement> parse_let_binding();
    std::unique_ptr<Statement> parse_type_definition();
    std::unique_ptr<Statement> parse_predicate_definition();
    std::unique_ptr<Statement> parse_fact_declaration();
    std::unique_ptr<Statement> parse_rule_declaration();
    std::unique_ptr<Statement> parse_proof_block();
    
    // Utility parsing
    std::vector<std::string> parse_identifier_list();
    std::unique_ptr<Expression> parse_type_expression();
    std::vector<std::pair<std::string, std::unique_ptr<Expression>>> parse_parameter_list();
};

/**
 * AST Visitor pattern for traversal
 */
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    virtual void visit_literal(LiteralExpression& node) = 0;
    virtual void visit_identifier(IdentifierExpression& node) = 0;
    virtual void visit_binary_expression(BinaryExpression& node) = 0;
    virtual void visit_unary_expression(UnaryExpression& node) = 0;
    virtual void visit_quantified_expression(QuantifiedExpression& node) = 0;
    virtual void visit_let_binding(LetBinding& node) = 0;
    virtual void visit_predicate_definition(PredicateDefinition& node) = 0;
};

/**
 * Pretty printer for AST
 */
class ASTPrettyPrinter : public ASTVisitor {
public:
    std::string print(ASTNode& node);
    
    void visit_literal(LiteralExpression& node) override;
    void visit_identifier(IdentifierExpression& node) override;
    void visit_binary_expression(BinaryExpression& node) override;
    void visit_unary_expression(UnaryExpression& node) override;
    void visit_quantified_expression(QuantifiedExpression& node) override;
    void visit_let_binding(LetBinding& node) override;
    void visit_predicate_definition(PredicateDefinition& node) override;

private:
    std::ostringstream output_;
    int indent_level_;
    
    void indent();
    void print_indent();
};

/**
 * Symbol table for semantic analysis
 */
class SymbolTable {
public:
    struct Symbol {
        std::string name;
        std::string type;
        SourcePosition declaration_position;
        bool is_predicate;
        bool is_type;
        std::map<std::string, std::string> attributes;
    };
    
    void enter_scope();
    void exit_scope();
    
    bool declare_symbol(const std::string& name, const Symbol& symbol);
    Symbol* lookup_symbol(const std::string& name);
    std::vector<Symbol*> lookup_predicates(const std::string& name);
    
    bool is_in_current_scope(const std::string& name) const;

private:
    std::vector<std::map<std::string, Symbol>> scopes_;
    
    std::map<std::string, Symbol>& current_scope();
    const std::map<std::string, Symbol>& current_scope() const;
};

/**
 * Grammar factory for parser creation
 */
class PureLogicGrammar {
public:
    static std::unique_ptr<Lexer> create_lexer(const std::string& source, 
                                              const std::string& filename = "");
    static std::unique_ptr<Parser> create_parser(std::vector<Token> tokens);
    
    static std::unique_ptr<ASTNode> parse_string(const std::string& source,
                                                 const std::string& filename = "");
    static std::unique_ptr<ASTNode> parse_file(const std::string& filename);
    
    static bool validate_grammar();
    static std::string get_grammar_version();
};

} // namespace pure_logic
} // namespace language
} // namespace akao

#endif // AKAO_CORE_ENGINE_LANGUAGE_PURE_LOGIC_GRAMMAR_HPP
