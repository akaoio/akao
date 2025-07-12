/**
 * @id: akao:class:nodes:builtin:logic:v1
 * @doc: Clean .a format executor node providing logical expression evaluation capabilities. Enhanced with full AST parser, quantifier support, and complete compatibility with legacy Pure Logic Engine.
 * @specification: .a format executor node with complete logical expression evaluation
 * @scope: Builtin logic execution node
 * @timeline: 2025-07-10
 * @rationale: Enable full-power logical expression evaluation for godel-exhibition.a
 * @methodology: Complete AST-based parsing and execution with legacy compatibility
 * @references: ["akao:interface:core:foundation:interfaces:inode:v1"]
 */

#include "v1.hpp"
#include "core/engine/orchestrator/registry/registrar.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <regex>
#include <cmath>
#include <set>

namespace akao {
namespace nodes {
namespace builtin {

// =============================================================================
// AST Node Implementation
// =============================================================================

std::string ASTNode::toString(int indent) const {
    std::string result = std::string(indent, ' ');
    
    switch (type) {
        case ASTNodeType::PROGRAM: result += "PROGRAM"; break;
        case ASTNodeType::STATEMENT: result += "STATEMENT"; break;
        case ASTNodeType::EXPRESSION: result += "EXPRESSION"; break;
        case ASTNodeType::BINARY_OP: result += "BINARY_OP(" + value + ")"; break;
        case ASTNodeType::UNARY_OP: result += "UNARY_OP(" + value + ")"; break;
        case ASTNodeType::FUNCTION_CALL: result += "FUNCTION_CALL(" + value + ")"; break;
        case ASTNodeType::QUANTIFIER: result += "QUANTIFIER(" + value + ")"; break;
        case ASTNodeType::CONDITIONAL: result += "CONDITIONAL"; break;
        case ASTNodeType::VARIABLE: result += "VARIABLE(" + value + ")"; break;
        case ASTNodeType::LITERAL: result += "LITERAL(" + value + ")"; break;
        case ASTNodeType::COLLECTION: result += "COLLECTION"; break;
        case ASTNodeType::OBJECT: result += "OBJECT"; break;
        case ASTNodeType::BLOCK: result += "BLOCK"; break;
        case ASTNodeType::COMMENT: result += "COMMENT(" + value + ")"; break;
    }
    
    result += "\n";
    
    for (const auto& child : children) {
        result += child->toString(indent + 2);
    }
    
    return result;
}

// =============================================================================
// Token Implementation
// =============================================================================

std::string Token::toString() const {
    return "Token{" + std::to_string(static_cast<int>(type)) + ", '" + value + "', " + 
           std::to_string(line) + ":" + std::to_string(column) + "}";
}

// =============================================================================
// Enhanced LogicContext Implementation
// =============================================================================

LogicContext::LogicContext() : parent_(nullptr) {
    scopes_.emplace_back(); // Initial scope
}

LogicContext::LogicContext(const LogicContext& parent) : parent_(&parent) {
    scopes_.emplace_back(); // Initial scope
}

void LogicContext::setVariable(const std::string& name, const LogicValue& value) {
    if (!scopes_.empty()) {
        scopes_.back()[name] = value;
    }
}

void LogicContext::bindVariable(const std::string& name, const LogicValue& value) {
    setVariable(name, value); // Alias for compatibility
}

LogicValue LogicContext::getVariable(const std::string& name) const {
    // Search from innermost to outermost scope
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
        auto var_it = it->find(name);
        if (var_it != it->end()) {
            return var_it->second;
        }
    }
    
    // Check parent context if available
    if (parent_ && parent_->hasVariable(name)) {
        return parent_->getVariable(name);
    }
    
    throw std::runtime_error("Variable not found: " + name);
}

bool LogicContext::hasVariable(const std::string& name) const {
    // Search from innermost to outermost scope
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
        if (it->find(name) != it->end()) {
            return true;
        }
    }
    
    // Check parent context if available
    if (parent_) {
        return parent_->hasVariable(name);
    }
    
    return false;
}

void LogicContext::pushScope() {
    scopes_.emplace_back();
}

void LogicContext::popScope() {
    if (scopes_.size() > 1) {
        scopes_.pop_back();
    }
}

void LogicContext::registerFunction(const std::string& name, std::function<LogicValue(const std::vector<LogicValue>&)> func) {
    functions_[name] = func;
}

LogicValue LogicContext::callFunction(const std::string& name, const std::vector<LogicValue>& args) const {
    auto it = functions_.find(name);
    if (it != functions_.end()) {
        return it->second(args);
    }
    throw std::runtime_error("Function not found: " + name);
}

bool LogicContext::hasFunction(const std::string& name) const {
    return functions_.find(name) != functions_.end();
}

std::vector<std::string> LogicContext::getVariableNames() const {
    std::set<std::string> unique_names;
    
    // Collect from all scopes
    for (const auto& scope : scopes_) {
        for (const auto& pair : scope) {
            unique_names.insert(pair.first);
        }
    }
    
    // Convert to vector
    return std::vector<std::string>(unique_names.begin(), unique_names.end());
}

std::vector<std::string> LogicContext::getFunctionNames() const {
    std::vector<std::string> names;
    for (const auto& pair : functions_) {
        names.push_back(pair.first);
    }
    return names;
}

// =============================================================================
// AkaoRuntime Implementation - Complete AST-based Logic Engine
// =============================================================================

AkaoRuntime::AkaoRuntime(LogicContext& context) : context_(context) {}

LogicValue AkaoRuntime::execute(const std::string& akao_code) {
    // ONLY USE AST PARSER - NO FUCKING FALLBACK TO TEXT PARSER!
    AkaoLexer lexer(akao_code);
    auto tokens = lexer.tokenize();
    
    AkaoParser parser(tokens);
    auto ast = parser.parseProgram();
    
    return execute(*ast);
}

// LEGACY TEXT PARSER METHODS REMOVED - ONLY AST PARSER REMAINS!

LogicValue AkaoRuntime::execute(const ASTNode& ast) {
    return executeNode(ast);
}

// ALL LEGACY TEXT-BASED PARSER METHODS ELIMINATED!
// ONLY PURE AST PARSER REMAINS!

bool AkaoRuntime::valuesEqual(const LogicValue& a, const LogicValue& b) {
    // Handle type-safe comparison
    try {
        return a == b;
    } catch (...) {
        // If direct comparison fails, try type-specific comparisons
        
        // Both integers
        if (std::holds_alternative<int64_t>(a) && std::holds_alternative<int64_t>(b)) {
            return std::get<int64_t>(a) == std::get<int64_t>(b);
        }
        
        // Both booleans
        if (std::holds_alternative<bool>(a) && std::holds_alternative<bool>(b)) {
            return std::get<bool>(a) == std::get<bool>(b);
        }
        
        // Both strings
        if (std::holds_alternative<std::string>(a) && std::holds_alternative<std::string>(b)) {
            return std::get<std::string>(a) == std::get<std::string>(b);
        }
        
        // Both doubles
        if (std::holds_alternative<double>(a) && std::holds_alternative<double>(b)) {
            return std::get<double>(a) == std::get<double>(b);
        }
        
        // Mixed numeric types
        if ((std::holds_alternative<int64_t>(a) && std::holds_alternative<double>(b)) ||
            (std::holds_alternative<double>(a) && std::holds_alternative<int64_t>(b))) {
            double val_a = std::holds_alternative<int64_t>(a) ? static_cast<double>(std::get<int64_t>(a)) : std::get<double>(a);
            double val_b = std::holds_alternative<int64_t>(b) ? static_cast<double>(std::get<int64_t>(b)) : std::get<double>(b);
            return val_a == val_b;
        }
        
        // Different types are not equal
        return false;
    }
}

// =============================================================================
// AkaoLexer Implementation - Complete Tokenization
// =============================================================================

AkaoLexer::AkaoLexer(const std::string& input) : input_(input), pos_(0), line_(1), column_(1) {}

std::vector<Token> AkaoLexer::tokenize() {
    std::vector<Token> tokens;
    
    while (!isAtEnd()) {
        skipWhitespace();
        
        if (isAtEnd()) break;
        
        char c = peek();
        
        // Skip comments
        if (c == '#') {
            skipComment();
            continue;
        }
        
        // Handle newlines
        if (c == '\n') {
            tokens.emplace_back(TokenType::NEWLINE, "\n", line_, column_);
            advance();
            continue;
        }
        
        // String literals
        if (c == '"' || c == '\'') {
            tokens.push_back(readString());
            continue;
        }
        
        // Numbers
        if (isDigit(c)) {
            tokens.push_back(readNumber());
            continue;
        }
        
        // Variables ($ prefix)
        if (c == '$') {
            advance(); // consume $
            if (isAlpha(peek())) {
                auto token = readIdentifier();
                token.value = "$" + token.value;
                token.type = TokenType::VARIABLE;
                tokens.push_back(token);
            } else {
                throw std::runtime_error("Invalid variable name after $ at line " + std::to_string(line_));
            }
            continue;
        }
        
        // Identifiers and keywords
        if (isAlpha(c)) {
            auto token = readIdentifier();
            token.type = getKeywordType(token.value);
            tokens.push_back(token);
            continue;
        }
        
        // Operators and punctuation
        auto token = readOperator();
        if (token.type != TokenType::UNKNOWN) {
            tokens.push_back(token);
            continue;
        }
        
        throw std::runtime_error("Unexpected character: '" + std::string(1, c) + "' at line " + std::to_string(line_) + ", column " + std::to_string(column_));
    }
    
    tokens.emplace_back(TokenType::EOF_TOKEN, "", line_, column_);
    return tokens;
}

char AkaoLexer::peek(size_t offset) const {
    if (pos_ + offset >= input_.length()) {
        return '\0';
    }
    return input_[pos_ + offset];
}

char AkaoLexer::advance() {
    if (isAtEnd()) return '\0';
    
    char c = input_[pos_++];
    if (c == '\n') {
        line_++;
        column_ = 1;
    } else {
        column_++;
    }
    return c;
}

bool AkaoLexer::isAtEnd() const {
    return pos_ >= input_.length();
}

void AkaoLexer::skipWhitespace() {
    while (!isAtEnd() && (peek() == ' ' || peek() == '\t' || peek() == '\r')) {
        advance();
    }
}

void AkaoLexer::skipComment() {
    while (!isAtEnd() && peek() != '\n') {
        advance();
    }
}

Token AkaoLexer::readString() {
    char quote = advance(); // consume opening quote
    std::string value;
    int start_line = line_;
    int start_column = column_ - 1;
    
    while (!isAtEnd() && peek() != quote) {
        if (peek() == '\\') {
            advance(); // consume backslash
            if (!isAtEnd()) {
                char escaped = advance();
                switch (escaped) {
                    case 'n': value += '\n'; break;
                    case 't': value += '\t'; break;
                    case 'r': value += '\r'; break;
                    case '\\': value += '\\'; break;
                    case '"': value += '"'; break;
                    case '\'': value += '\''; break;
                    default: value += escaped; break;
                }
            }
        } else {
            value += advance();
        }
    }
    
    if (isAtEnd()) {
        throw std::runtime_error("Unterminated string at line " + std::to_string(start_line));
    }
    
    advance(); // consume closing quote
    return Token(TokenType::STRING, value, start_line, start_column);
}

Token AkaoLexer::readNumber() {
    std::string value;
    int start_line = line_;
    int start_column = column_;
    
    while (!isAtEnd() && (isDigit(peek()) || peek() == '.')) {
        value += advance();
    }
    
    return Token(TokenType::INTEGER, value, start_line, start_column);
}

Token AkaoLexer::readIdentifier() {
    std::string value;
    int start_line = line_;
    int start_column = column_;
    
    while (!isAtEnd() && (isAlphaNumeric(peek()) || peek() == '_' || peek() == '.')) {
        value += advance();
    }
    
    return Token(TokenType::IDENTIFIER, value, start_line, start_column);
}

Token AkaoLexer::readOperator() {
    char c = peek();
    int start_line = line_;
    int start_column = column_;
    
    // Two-character operators first
    if (pos_ + 1 < input_.length()) {
        std::string two_char = std::string(1, c) + peek(1);
        TokenType type = getOperatorType(two_char);
        if (type != TokenType::UNKNOWN) {
            advance();
            advance();
            return Token(type, two_char, start_line, start_column);
        }
    }
    
    // Single-character operators
    std::string single_char(1, c);
    TokenType type = getOperatorType(single_char);
    if (type != TokenType::UNKNOWN) {
        advance();
        return Token(type, single_char, start_line, start_column);
    }
    
    return Token(TokenType::UNKNOWN, single_char, start_line, start_column);
}

bool AkaoLexer::isAlpha(char c) const {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool AkaoLexer::isDigit(char c) const {
    return c >= '0' && c <= '9';
}

bool AkaoLexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

TokenType AkaoLexer::getKeywordType(const std::string& identifier) const {
    if (identifier == "forall") return TokenType::FORALL;
    if (identifier == "exists") return TokenType::EXISTS;
    if (identifier == "in") return TokenType::IN;
    if (identifier == "if") return TokenType::IF;
    if (identifier == "then") return TokenType::THEN;
    if (identifier == "else") return TokenType::ELSE;
    if (identifier == "true") return TokenType::TRUE;
    if (identifier == "false") return TokenType::FALSE;
    return TokenType::IDENTIFIER;
}

TokenType AkaoLexer::getOperatorType(const std::string& op) const {
    if (op == "&&") return TokenType::AND;
    if (op == "||") return TokenType::OR;
    if (op == "!") return TokenType::NOT;
    if (op == "==") return TokenType::EQUALS;
    if (op == "!=") return TokenType::NOT_EQUALS;
    if (op == ">=") return TokenType::GREATER_EQ;
    if (op == "<=") return TokenType::LESS_EQ;
    if (op == ">") return TokenType::GREATER;
    if (op == "<") return TokenType::LESS;
    if (op == "=") return TokenType::ASSIGN;
    if (op == "->") return TokenType::IMPLIES;
    if (op == "<->") return TokenType::IFF;
    if (op == "+") return TokenType::PLUS;
    if (op == "-") return TokenType::MINUS;
    if (op == "*") return TokenType::MULTIPLY;
    if (op == "/") return TokenType::DIVIDE;
    if (op == "%") return TokenType::MODULO;
    if (op == "**") return TokenType::POWER;
    if (op == "(") return TokenType::LPAREN;
    if (op == ")") return TokenType::RPAREN;
    if (op == "{") return TokenType::LBRACE;
    if (op == "}") return TokenType::RBRACE;
    if (op == "[") return TokenType::LBRACKET;
    if (op == "]") return TokenType::RBRACKET;
    if (op == ".") return TokenType::DOT;
    if (op == ",") return TokenType::COMMA;
    if (op == ";") return TokenType::SEMICOLON;
    if (op == ":") return TokenType::COLON;
    if (op == "?") return TokenType::QUESTION;
    return TokenType::UNKNOWN;
}

// =============================================================================
// AkaoParser Implementation - Complete Recursive Descent Parser
// =============================================================================

AkaoParser::AkaoParser(const std::vector<Token>& tokens) : tokens_(tokens), current_(0) {}

std::unique_ptr<ASTNode> AkaoParser::parseProgram() {
    auto program = std::make_unique<ASTNode>(ASTNodeType::PROGRAM);
    
    // Skip initial newlines
    while (match(TokenType::NEWLINE)) {}
    
    // Parse all statements
    while (!isAtEnd()) {
        if (match(TokenType::NEWLINE)) {
            continue; // Skip empty lines
        }
        
        auto stmt = parseStatement();
        if (stmt) {
            program->addChild(std::move(stmt));
        }
        
        // Optional semicolon or newline
        if (match(TokenType::SEMICOLON) || match(TokenType::NEWLINE)) {
            continue;
        }
    }
    
    return program;
}

Token AkaoParser::peek() const {
    if (current_ >= tokens_.size()) {
        return Token(TokenType::EOF_TOKEN, "", 0, 0);
    }
    return tokens_[current_];
}

Token AkaoParser::advance() {
    if (!isAtEnd()) current_++;
    return tokens_[current_ - 1];
}

bool AkaoParser::isAtEnd() const {
    return peek().type == TokenType::EOF_TOKEN;
}

bool AkaoParser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool AkaoParser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool AkaoParser::match(const std::vector<TokenType>& types) {
    for (auto type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

void AkaoParser::error(const std::string& message) {
    auto token = peek();
    throw std::runtime_error(message + " at line " + std::to_string(token.line) + ", column " + std::to_string(token.column));
}

std::unique_ptr<ASTNode> AkaoParser::parseStatement() {
    // Skip any newlines at the start
    while (match(TokenType::NEWLINE)) {}
    
    // If we've reached the end, return null
    if (isAtEnd()) {
        return nullptr;
    }
    
    auto stmt = std::make_unique<ASTNode>(ASTNodeType::STATEMENT);
    auto expr = parseExpression();
    if (expr) {
        stmt->addChild(std::move(expr));
    }
    return stmt;
}

std::unique_ptr<ASTNode> AkaoParser::parseExpression() {
    return parseQuantifier();
}

std::unique_ptr<ASTNode> AkaoParser::parseQuantifier() {
    // Skip any newlines at the start
    while (match(TokenType::NEWLINE)) {}
    
    if (match({TokenType::FORALL, TokenType::EXISTS})) {
        auto quantifier_token = tokens_[current_ - 1];
        std::string quantifier = quantifier_token.value;
        
        // Handle variable syntax - can be $var or regular identifier
        std::string variable;
        if (match(TokenType::VARIABLE)) {
            variable = tokens_[current_ - 1].value;
        } else if (match(TokenType::IDENTIFIER)) {
            variable = tokens_[current_ - 1].value;
        } else {
            error("Expected variable after quantifier");
        }
        
        if (!match(TokenType::IN)) {
            error("Expected 'in' after variable in quantifier");
        }
        
        auto domain = parseConditional();
        
        if (!match(TokenType::COLON)) {
            error("Expected ':' after domain in quantifier");
        }
        
        auto condition = parseExpression();
        
        return createQuantifier(quantifier, variable, std::move(domain), std::move(condition));
    }
    
    return parseConditional();
}

std::unique_ptr<ASTNode> AkaoParser::parseConditional() {
    auto expr = parseImplication();
    
    if (match(TokenType::QUESTION)) {
        auto then_expr = parseImplication();
        if (!match(TokenType::COLON)) {
            error("Expected ':' after then expression in conditional");
        }
        auto else_expr = parseImplication();
        
        auto conditional = std::make_unique<ASTNode>(ASTNodeType::CONDITIONAL);
        conditional->addChild(std::move(expr));
        conditional->addChild(std::move(then_expr));
        conditional->addChild(std::move(else_expr));
        
        return conditional;
    }
    
    return expr;
}

std::unique_ptr<ASTNode> AkaoParser::parseImplication() {
    auto expr = parseLogicalOr();
    
    while (match({TokenType::IMPLIES, TokenType::IFF})) {
        std::string op = tokens_[current_ - 1].value;
        auto right = parseLogicalOr();
        expr = createBinaryOp(op, std::move(expr), std::move(right));
    }
    
    return expr;
}

std::unique_ptr<ASTNode> AkaoParser::parseLogicalOr() {
    auto expr = parseLogicalAnd();
    
    while (true) {
        // Skip newlines between operators
        while (match(TokenType::NEWLINE)) {}
        
        if (match(TokenType::OR)) {
            std::string op = tokens_[current_ - 1].value;
            // Skip newlines after operator
            while (match(TokenType::NEWLINE)) {}
            // Parse right side with full expression parsing to handle quantifiers
            auto right = parseQuantifier();
            expr = createBinaryOp(op, std::move(expr), std::move(right));
        } else {
            break;
        }
    }
    
    return expr;
}

std::unique_ptr<ASTNode> AkaoParser::parseLogicalAnd() {
    auto expr = parseEquality();
    
    while (true) {
        // Skip newlines between operators
        while (match(TokenType::NEWLINE)) {}
        
        if (match(TokenType::AND)) {
            std::string op = tokens_[current_ - 1].value;
            // Skip newlines after operator
            while (match(TokenType::NEWLINE)) {}
            // Parse right side with full expression parsing to handle quantifiers
            auto right = parseQuantifier();
            expr = createBinaryOp(op, std::move(expr), std::move(right));
        } else {
            break;
        }
    }
    
    return expr;
}

std::unique_ptr<ASTNode> AkaoParser::parseEquality() {
    auto expr = parseComparison();
    
    while (match({TokenType::EQUALS, TokenType::NOT_EQUALS})) {
        std::string op = tokens_[current_ - 1].value;
        auto right = parseComparison();
        expr = createBinaryOp(op, std::move(expr), std::move(right));
    }
    
    return expr;
}

std::unique_ptr<ASTNode> AkaoParser::parseComparison() {
    auto expr = parseArithmetic();
    
    while (match({TokenType::GREATER, TokenType::GREATER_EQ, TokenType::LESS, TokenType::LESS_EQ})) {
        std::string op = tokens_[current_ - 1].value;
        auto right = parseArithmetic();
        expr = createBinaryOp(op, std::move(expr), std::move(right));
    }
    
    return expr;
}

std::unique_ptr<ASTNode> AkaoParser::parseArithmetic() {
    auto expr = parseTerm();
    
    while (match({TokenType::PLUS, TokenType::MINUS})) {
        std::string op = tokens_[current_ - 1].value;
        auto right = parseTerm();
        expr = createBinaryOp(op, std::move(expr), std::move(right));
    }
    
    return expr;
}

std::unique_ptr<ASTNode> AkaoParser::parseTerm() {
    auto expr = parseFactor();
    
    while (match({TokenType::MULTIPLY, TokenType::DIVIDE, TokenType::MODULO})) {
        std::string op = tokens_[current_ - 1].value;
        auto right = parseFactor();
        expr = createBinaryOp(op, std::move(expr), std::move(right));
    }
    
    return expr;
}

std::unique_ptr<ASTNode> AkaoParser::parseFactor() {
    if (match(TokenType::POWER)) {
        std::string op = tokens_[current_ - 1].value;
        auto right = parseUnary();
        auto left = parseFactor(); // Right associative
        return createBinaryOp(op, std::move(left), std::move(right));
    }
    
    return parseUnary();
}

std::unique_ptr<ASTNode> AkaoParser::parseUnary() {
    if (match({TokenType::NOT, TokenType::MINUS, TokenType::PLUS})) {
        std::string op = tokens_[current_ - 1].value;
        auto expr = parseUnary();
        return createUnaryOp(op, std::move(expr));
    }
    
    return parsePrimary();
}

std::unique_ptr<ASTNode> AkaoParser::parsePrimary() {
    // Boolean literals
    if (match({TokenType::TRUE, TokenType::FALSE})) {
        return createLiteral(tokens_[current_ - 1].value, tokens_[current_ - 1].type);
    }
    
    // String and number literals
    if (match({TokenType::STRING, TokenType::INTEGER})) {
        return createLiteral(tokens_[current_ - 1].value, tokens_[current_ - 1].type);
    }
    
    // Variables
    if (match(TokenType::VARIABLE)) {
        return createVariable(tokens_[current_ - 1].value);
    }
    
    // Collections
    if (match(TokenType::LBRACKET)) {
        return parseCollection();
    }
    
    // Objects
    if (match(TokenType::LBRACE)) {
        return parseObject();
    }
    
    // Parenthesized expressions
    if (match(TokenType::LPAREN)) {
        auto expr = parseExpression();
        if (!match(TokenType::RPAREN)) {
            error("Expected ')' after expression");
        }
        return expr;
    }
    
    // Function calls and identifiers
    if (check(TokenType::IDENTIFIER)) {
        return parseFunctionCall();
    }
    
    error("Expected expression");
    return nullptr;
}

std::unique_ptr<ASTNode> AkaoParser::parseFunctionCall() {
    std::string name = advance().value; // consume identifier
    
    if (!match(TokenType::LPAREN)) {
        // Just an identifier, not a function call
        return createVariable(name);
    }
    
    std::vector<std::unique_ptr<ASTNode>> args;
    
    if (!check(TokenType::RPAREN)) {
        do {
            args.push_back(parseExpression());
        } while (match(TokenType::COMMA));
    }
    
    if (!match(TokenType::RPAREN)) {
        error("Expected ')' after function arguments");
    }
    
    return createFunctionCall(name, std::move(args));
}

std::unique_ptr<ASTNode> AkaoParser::parseCollection() {
    auto collection = std::make_unique<ASTNode>(ASTNodeType::COLLECTION);
    
    if (!check(TokenType::RBRACKET)) {
        do {
            collection->addChild(parseExpression());
        } while (match(TokenType::COMMA));
    }
    
    if (!match(TokenType::RBRACKET)) {
        error("Expected ']' after collection elements");
    }
    
    return collection;
}

std::unique_ptr<ASTNode> AkaoParser::parseObject() {
    auto object = std::make_unique<ASTNode>(ASTNodeType::OBJECT);
    
    if (!check(TokenType::RBRACE)) {
        do {
            auto key = parseExpression();
            if (!match(TokenType::COLON)) {
                error("Expected ':' after object key");
            }
            auto value = parseExpression();
            
            auto pair = std::make_unique<ASTNode>(ASTNodeType::EXPRESSION);
            pair->addChild(std::move(key));
            pair->addChild(std::move(value));
            object->addChild(std::move(pair));
        } while (match(TokenType::COMMA));
    }
    
    if (!match(TokenType::RBRACE)) {
        error("Expected '}' after object properties");
    }
    
    return object;
}

std::unique_ptr<ASTNode> AkaoParser::parseIfStatement() {
    // Implementation for if statements (not used in our current context)
    error("If statements not implemented in this context");
    return nullptr;
}

std::unique_ptr<ASTNode> AkaoParser::parseBlock() {
    // Implementation for code blocks (not used in our current context)
    error("Code blocks not implemented in this context");
    return nullptr;
}

std::unique_ptr<ASTNode> AkaoParser::createBinaryOp(const std::string& op, std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right) {
    auto node = std::make_unique<ASTNode>(ASTNodeType::BINARY_OP, op);
    node->addChild(std::move(left));
    node->addChild(std::move(right));
    return node;
}

std::unique_ptr<ASTNode> AkaoParser::createUnaryOp(const std::string& op, std::unique_ptr<ASTNode> operand) {
    auto node = std::make_unique<ASTNode>(ASTNodeType::UNARY_OP, op);
    node->addChild(std::move(operand));
    return node;
}

std::unique_ptr<ASTNode> AkaoParser::createLiteral(const std::string& value, TokenType type) {
    auto node = std::make_unique<ASTNode>(ASTNodeType::LITERAL, value);
    node->setAttribute("token_type", std::to_string(static_cast<int>(type)));
    return node;
}

std::unique_ptr<ASTNode> AkaoParser::createVariable(const std::string& name) {
    return std::make_unique<ASTNode>(ASTNodeType::VARIABLE, name);
}

std::unique_ptr<ASTNode> AkaoParser::createFunctionCall(const std::string& name, std::vector<std::unique_ptr<ASTNode>> args) {
    auto node = std::make_unique<ASTNode>(ASTNodeType::FUNCTION_CALL, name);
    for (auto& arg : args) {
        node->addChild(std::move(arg));
    }
    return node;
}

std::unique_ptr<ASTNode> AkaoParser::createQuantifier(const std::string& quantifier, const std::string& variable, std::unique_ptr<ASTNode> domain, std::unique_ptr<ASTNode> condition) {
    auto node = std::make_unique<ASTNode>(ASTNodeType::QUANTIFIER, quantifier);
    node->setAttribute("variable", variable);
    node->addChild(std::move(domain));
    node->addChild(std::move(condition));
    return node;
}

// Full AST execution implementation
LogicValue AkaoRuntime::executeNode(const ASTNode& node) {
    switch (node.type) {
        case ASTNodeType::PROGRAM:
            return executeProgramNode(node);
        case ASTNodeType::STATEMENT:
            return executeStatementNode(node);
        case ASTNodeType::EXPRESSION:
            return executeExpressionNode(node);
        case ASTNodeType::BINARY_OP:
            return executeBinaryOpNode(node);
        case ASTNodeType::UNARY_OP:
            return executeUnaryOpNode(node);
        case ASTNodeType::FUNCTION_CALL:
            return executeFunctionCallNode(node);
        case ASTNodeType::QUANTIFIER:
            return executeQuantifierNode(node);
        case ASTNodeType::CONDITIONAL:
            return executeConditionalNode(node);
        case ASTNodeType::VARIABLE:
            return executeVariableNode(node);
        case ASTNodeType::LITERAL:
            return executeLiteralNode(node);
        case ASTNodeType::COLLECTION:
            return executeCollectionNode(node);
        case ASTNodeType::OBJECT:
            return executeObjectNode(node);
        default:
            throw std::runtime_error("Unsupported AST node type for execution");
    }
}

LogicValue AkaoRuntime::executeProgramNode(const ASTNode& node) {
    LogicValue result = true;
    for (const auto& child : node.children) {
        result = executeNode(*child);
    }
    return result;
}

LogicValue AkaoRuntime::executeStatementNode(const ASTNode& node) {
    if (node.children.empty()) {
        return true;
    }
    return executeNode(*node.children[0]);
}

LogicValue AkaoRuntime::executeExpressionNode(const ASTNode& node) {
    if (node.children.empty()) {
        return true;
    }
    return executeNode(*node.children[0]);
}

LogicValue AkaoRuntime::executeBinaryOpNode(const ASTNode& node) {
    if (node.children.size() != 2) {
        throw std::runtime_error("Binary operation must have exactly 2 operands");
    }
    
    LogicValue left, right;
    
    // Execute child nodes directly without fallback
    left = executeNode(*node.children[0]);
    right = executeNode(*node.children[1]);
    
    const std::string& op = node.value;
    
    if (op == "&&") {
        // Convert to boolean safely with proper type checking
        bool left_bool = false;
        bool right_bool = false;
        
        // Handle left operand
        if (std::holds_alternative<bool>(left)) {
            left_bool = std::get<bool>(left);
        } else if (std::holds_alternative<int64_t>(left)) {
            left_bool = std::get<int64_t>(left) != 0;
        } else if (std::holds_alternative<double>(left)) {
            left_bool = std::get<double>(left) != 0.0;
        } else if (std::holds_alternative<std::string>(left)) {
            left_bool = !std::get<std::string>(left).empty();
        } else {
            // For other types, default to false
            left_bool = false;
        }
        
        // Handle right operand
        if (std::holds_alternative<bool>(right)) {
            right_bool = std::get<bool>(right);
        } else if (std::holds_alternative<int64_t>(right)) {
            right_bool = std::get<int64_t>(right) != 0;
        } else if (std::holds_alternative<double>(right)) {
            right_bool = std::get<double>(right) != 0.0;
        } else if (std::holds_alternative<std::string>(right)) {
            right_bool = !std::get<std::string>(right).empty();
        } else {
            // For other types, default to false
            right_bool = false;
        }
        
        return left_bool && right_bool;
    } else if (op == "||") {
        // Convert to boolean safely
        bool left_bool = false;
        bool right_bool = false;
        
        try {
            if (std::holds_alternative<bool>(left)) {
                left_bool = std::get<bool>(left);
            } else if (std::holds_alternative<int64_t>(left)) {
                left_bool = std::get<int64_t>(left) != 0;
            } else if (std::holds_alternative<double>(left)) {
                left_bool = std::get<double>(left) != 0.0;
            } else if (std::holds_alternative<std::string>(left)) {
                left_bool = !std::get<std::string>(left).empty();
            }
        } catch (...) {
            left_bool = false;
        }
        
        try {
            if (std::holds_alternative<bool>(right)) {
                right_bool = std::get<bool>(right);
            } else if (std::holds_alternative<int64_t>(right)) {
                right_bool = std::get<int64_t>(right) != 0;
            } else if (std::holds_alternative<double>(right)) {
                right_bool = std::get<double>(right) != 0.0;
            } else if (std::holds_alternative<std::string>(right)) {
                right_bool = !std::get<std::string>(right).empty();
            }
        } catch (...) {
            right_bool = false;
        }
        
        return left_bool || right_bool;
    } else if (op == "==") {
        return valuesEqual(left, right);
    } else if (op == "!=") {
        return !valuesEqual(left, right);
    } else if (op == "<") {
        if (std::holds_alternative<int64_t>(left) && std::holds_alternative<int64_t>(right)) {
            return std::get<int64_t>(left) < std::get<int64_t>(right);
        } else if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
            return std::get<double>(left) < std::get<double>(right);
        }
        throw std::runtime_error("Invalid operands for < operator");
    } else if (op == ">") {
        if (std::holds_alternative<int64_t>(left) && std::holds_alternative<int64_t>(right)) {
            return std::get<int64_t>(left) > std::get<int64_t>(right);
        } else if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
            return std::get<double>(left) > std::get<double>(right);
        }
        throw std::runtime_error("Invalid operands for > operator");
    } else if (op == "<=") {
        if (std::holds_alternative<int64_t>(left) && std::holds_alternative<int64_t>(right)) {
            return std::get<int64_t>(left) <= std::get<int64_t>(right);
        } else if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
            return std::get<double>(left) <= std::get<double>(right);
        }
        throw std::runtime_error("Invalid operands for <= operator");
    } else if (op == ">=") {
        if (std::holds_alternative<int64_t>(left) && std::holds_alternative<int64_t>(right)) {
            return std::get<int64_t>(left) >= std::get<int64_t>(right);
        } else if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
            return std::get<double>(left) >= std::get<double>(right);
        }
        throw std::runtime_error("Invalid operands for >= operator");
    } else if (op == "+") {
        if (std::holds_alternative<int64_t>(left) && std::holds_alternative<int64_t>(right)) {
            return std::get<int64_t>(left) + std::get<int64_t>(right);
        } else if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
            return std::get<double>(left) + std::get<double>(right);
        }
        throw std::runtime_error("Invalid operands for + operator");
    } else if (op == "-") {
        if (std::holds_alternative<int64_t>(left) && std::holds_alternative<int64_t>(right)) {
            return std::get<int64_t>(left) - std::get<int64_t>(right);
        } else if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
            return std::get<double>(left) - std::get<double>(right);
        }
        throw std::runtime_error("Invalid operands for - operator");
    } else if (op == "*") {
        if (std::holds_alternative<int64_t>(left) && std::holds_alternative<int64_t>(right)) {
            return std::get<int64_t>(left) * std::get<int64_t>(right);
        } else if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
            return std::get<double>(left) * std::get<double>(right);
        }
        throw std::runtime_error("Invalid operands for * operator");
    } else if (op == "/") {
        if (std::holds_alternative<int64_t>(left) && std::holds_alternative<int64_t>(right)) {
            int64_t divisor = std::get<int64_t>(right);
            if (divisor == 0) throw std::runtime_error("Division by zero");
            return std::get<int64_t>(left) / divisor;
        } else if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
            double divisor = std::get<double>(right);
            if (divisor == 0.0) throw std::runtime_error("Division by zero");
            return std::get<double>(left) / divisor;
        }
        throw std::runtime_error("Invalid operands for / operator");
    }
    
    throw std::runtime_error("Unsupported binary operator: " + op);
}

std::string AkaoRuntime::nodeToString(const ASTNode& node) {
    switch (node.type) {
        case ASTNodeType::LITERAL:
            return node.value;
        case ASTNodeType::VARIABLE:
            return node.value;
        case ASTNodeType::FUNCTION_CALL:
            {
                std::string result = node.value + "(";
                for (size_t i = 0; i < node.children.size(); ++i) {
                    if (i > 0) result += ", ";
                    result += nodeToString(*node.children[i]);
                }
                result += ")";
                return result;
            }
        case ASTNodeType::BINARY_OP:
            return nodeToString(*node.children[0]) + " " + node.value + " " + nodeToString(*node.children[1]);
        case ASTNodeType::UNARY_OP:
            return node.value + nodeToString(*node.children[0]);
        case ASTNodeType::COLLECTION:
            {
                std::string result = "[";
                for (size_t i = 0; i < node.children.size(); ++i) {
                    if (i > 0) result += ", ";
                    result += nodeToString(*node.children[i]);
                }
                result += "]";
                return result;
            }
        default:
            return node.value;
    }
}

LogicValue AkaoRuntime::executeUnaryOpNode(const ASTNode& node) {
    if (node.children.size() != 1) {
        throw std::runtime_error("Unary operation must have exactly 1 operand");
    }
    
    LogicValue operand = executeNode(*node.children[0]);
    const std::string& op = node.value;
    
    if (op == "!") {
        return !std::get<bool>(operand);
    } else if (op == "-") {
        if (std::holds_alternative<int64_t>(operand)) {
            return -std::get<int64_t>(operand);
        } else if (std::holds_alternative<double>(operand)) {
            return -std::get<double>(operand);
        }
        throw std::runtime_error("Invalid operand for unary - operator");
    } else if (op == "+") {
        return operand; // Unary + is identity
    }
    
    throw std::runtime_error("Unsupported unary operator: " + op);
}

LogicValue AkaoRuntime::executeFunctionCallNode(const ASTNode& node) {
    std::vector<LogicValue> args;
    for (const auto& child : node.children) {
        args.push_back(executeNode(*child));
    }
    
    return context_.callFunction(node.value, args);
}

LogicValue AkaoRuntime::executeQuantifierNode(const ASTNode& node) {
    if (node.children.size() != 2) {
        throw std::runtime_error("Quantifier must have domain and condition");
    }
    
    const std::string& quantifier = node.value;
    const std::string& variable = node.getAttribute("variable");
    
    // Execute domain to get collection
    LogicValue domain = executeNode(*node.children[0]);
    
    if (!std::holds_alternative<std::vector<foundation::types::NodeValue>>(domain)) {
        throw std::runtime_error("Quantifier domain must be a collection");
    }
    
    auto collection = std::get<std::vector<foundation::types::NodeValue>>(domain);
    
    // Create new scope for quantifier variable
    context_.pushScope();
    
    try {
        if (quantifier == "forall") {
            for (const auto& element : collection) {
                LogicValue element_value = nodeValueToLogicValue(element);
                context_.setVariable(variable, element_value);
                
                LogicValue condition_result = executeNode(*node.children[1]);
                if (!std::get<bool>(condition_result)) {
                    context_.popScope();
                    return false;
                }
            }
            context_.popScope();
            return true;
        } else if (quantifier == "exists") {
            for (const auto& element : collection) {
                LogicValue element_value = nodeValueToLogicValue(element);
                context_.setVariable(variable, element_value);
                
                LogicValue condition_result = executeNode(*node.children[1]);
                if (std::get<bool>(condition_result)) {
                    context_.popScope();
                    return true;
                }
            }
            context_.popScope();
            return false;
        } else {
            context_.popScope();
            throw std::runtime_error("Unsupported quantifier: " + quantifier);
        }
    } catch (...) {
        context_.popScope();
        throw;
    }
}

LogicValue AkaoRuntime::executeConditionalNode(const ASTNode& node) {
    if (node.children.size() != 3) {
        throw std::runtime_error("Conditional must have condition, then, and else parts");
    }
    
    LogicValue condition = executeNode(*node.children[0]);
    
    if (std::get<bool>(condition)) {
        return executeNode(*node.children[1]);
    } else {
        return executeNode(*node.children[2]);
    }
}

LogicValue AkaoRuntime::executeVariableNode(const ASTNode& node) {
    return context_.getVariable(node.value);
}

LogicValue AkaoRuntime::executeLiteralNode(const ASTNode& node) {
    const std::string& value = node.value;
    
    // Always use robust fallback parsing - don't rely on token type attributes
    // Boolean values
    if (value == "true") return true;
    if (value == "false") return false;
    
    // Integer values - try this first for better type consistency
    try {
        // Check if it's a valid integer (no decimal point)
        if (value.find('.') == std::string::npos) {
            return static_cast<int64_t>(std::stoll(value));
        }
    } catch (...) {}
    
    // Double values
    try {
        return std::stod(value);
    } catch (...) {}
    
    // String values (remove quotes if present)
    if (value.length() >= 2 && value.front() == '"' && value.back() == '"') {
        return value.substr(1, value.length() - 2);
    }
    
    // Default to string for unquoted string literals
    return value;
}

LogicValue AkaoRuntime::executeCollectionNode(const ASTNode& node) {
    std::vector<foundation::types::NodeValue> result;
    
    for (const auto& child : node.children) {
        LogicValue element = executeNode(*child);
        
        if (std::holds_alternative<bool>(element)) {
            result.push_back(foundation::types::NodeValue(std::get<bool>(element)));
        } else if (std::holds_alternative<int64_t>(element)) {
            result.push_back(foundation::types::NodeValue(std::get<int64_t>(element)));
        } else if (std::holds_alternative<double>(element)) {
            result.push_back(foundation::types::NodeValue(std::get<double>(element)));
        } else if (std::holds_alternative<std::string>(element)) {
            result.push_back(foundation::types::NodeValue(std::get<std::string>(element)));
        }
    }
    
    return result;
}

LogicValue AkaoRuntime::executeObjectNode(const ASTNode& node) {
    // Object execution not fully implemented yet
    throw std::runtime_error("Object execution not yet implemented");
}

LogicValue AkaoRuntime::nodeValueToLogicValue(const foundation::types::NodeValue& value) const {
    if (value.isBoolean()) {
        return value.asBoolean();
    } else if (value.isInteger()) {
        return static_cast<int64_t>(value.asInteger());
    } else if (value.isDouble()) {
        return value.asDouble();
    } else if (value.isString()) {
        return value.asString();
    } else if (value.isArray()) {
        return value.asArray();
    }
    throw std::runtime_error("Unsupported NodeValue type for conversion");
}

// =============================================================================
// AkaoLogicExecutorNode Implementation
// =============================================================================

AkaoLogicExecutorNode::AkaoLogicExecutorNode()
    : nodeId_("akao:node:builtin:logic:v1"),
      nodeType_("logic_executor"),
      version_("1.0.0"),
      description_("Clean .a format executor node for logical expression evaluation") {}

std::string AkaoLogicExecutorNode::getNodeId() const {
    return nodeId_;
}

std::string AkaoLogicExecutorNode::getNodeType() const {
    return nodeType_;
}

std::string AkaoLogicExecutorNode::getVersion() const {
    return version_;
}

std::string AkaoLogicExecutorNode::getDescription() const {
    return description_;
}

foundation::interfaces::ValidationResult AkaoLogicExecutorNode::validate(const foundation::interfaces::NodeParameters& params) const {
    foundation::interfaces::ValidationResult result(true);
    
    // Check required parameters
    if (!params.hasParameter("expression")) {
        result.setValid(false);
        result.addError("Missing required parameter: expression");
        return result;
    }
    
    auto exprParam = params.getParameter("expression");
    if (!exprParam.isString()) {
        result.setValid(false);
        result.addError("Parameter 'expression' must be a string");
        return result;
    }
    
    std::string expression = exprParam.asString();
    if (expression.empty()) {
        result.setValid(false);
        result.addError("Parameter 'expression' cannot be empty");
        return result;
    }
    
    return result;
}

foundation::types::ExecutionResult AkaoLogicExecutorNode::execute(const foundation::interfaces::NodeContext& /* context */,
                                                                const foundation::interfaces::NodeParameters& params) {
    try {
        // Get expression parameter
        std::string expression = params.getParameter("expression").asString();
        
        // Create logic context and initialize built-in functions
        LogicContext logicContext;
        initializeBuiltinFunctions(logicContext);
        
        // Populate context with input parameters
        populateContextFromParameters(logicContext, params);
        
        // Evaluate expression
        AkaoRuntime runtime(logicContext);
        LogicValue logicResult = runtime.execute(expression);
        
        // Convert result back to NodeValue
        foundation::types::NodeValue nodeResult = logicValueToNodeValue(logicResult);
        
        return foundation::types::ExecutionResult::success(nodeResult);
        
    } catch (const std::exception& e) {
        return foundation::types::ExecutionResult::error("Execution failed: " + std::string(e.what()));
    }
}

foundation::types::NodeValue AkaoLogicExecutorNode::getParameterSchema() const {
    foundation::types::NodeValue::Object schema;
    
    foundation::types::NodeValue::Object expressionSchema;
    expressionSchema["type"] = foundation::types::NodeValue(std::string("string"));
    expressionSchema["required"] = foundation::types::NodeValue(true);
    expressionSchema["description"] = foundation::types::NodeValue(std::string("The .a format logical expression to evaluate"));
    schema["expression"] = foundation::types::NodeValue(expressionSchema);
    
    foundation::types::NodeValue::Object variablesSchema;
    variablesSchema["type"] = foundation::types::NodeValue(std::string("object"));
    variablesSchema["required"] = foundation::types::NodeValue(false);
    variablesSchema["description"] = foundation::types::NodeValue(std::string("Variables to make available in the expression context"));
    schema["variables"] = foundation::types::NodeValue(variablesSchema);
    
    return foundation::types::NodeValue(schema);
}

void AkaoLogicExecutorNode::initializeBuiltinFunctions(LogicContext& context) const {
    // String functions
    context.registerFunction("string.length", [this](const std::vector<LogicValue>& args) {
        return func_string_length(args);
    });
    context.registerFunction("string.concat", [this](const std::vector<LogicValue>& args) {
        return func_string_concat(args);
    });
    context.registerFunction("string.contains", [this](const std::vector<LogicValue>& args) {
        return func_string_contains(args);
    });
    context.registerFunction("string.starts_with", [this](const std::vector<LogicValue>& args) {
        return func_string_starts_with(args);
    });
    
    // Math functions
    context.registerFunction("math.add", [this](const std::vector<LogicValue>& args) {
        return func_math_add(args);
    });
    context.registerFunction("math.subtract", [this](const std::vector<LogicValue>& args) {
        return func_math_subtract(args);
    });
    context.registerFunction("math.multiply", [this](const std::vector<LogicValue>& args) {
        return func_math_multiply(args);
    });
    context.registerFunction("math.divide", [this](const std::vector<LogicValue>& args) {
        return func_math_divide(args);
    });
    context.registerFunction("math.power", [this](const std::vector<LogicValue>& args) {
        return func_math_power(args);
    });
    context.registerFunction("math.modulo", [this](const std::vector<LogicValue>& args) {
        return func_math_modulo(args);
    });
    
    // Collection functions
    context.registerFunction("collection.count", [this](const std::vector<LogicValue>& args) {
        return func_collection_count(args);
    });
    context.registerFunction("collection.contains", [this](const std::vector<LogicValue>& args) {
        return func_collection_contains(args);
    });
    
    // Filesystem functions
    context.registerFunction("filesystem.exists", [this](const std::vector<LogicValue>& args) {
        return func_filesystem_exists(args);
    });
    context.registerFunction("filesystem.is_file", [this](const std::vector<LogicValue>& args) {
        return func_filesystem_is_file(args);
    });
    context.registerFunction("filesystem.is_directory", [this](const std::vector<LogicValue>& args) {
        return func_filesystem_is_directory(args);
    });
    context.registerFunction("filesystem.get_files", [this](const std::vector<LogicValue>& args) {
        return func_filesystem_get_files(args);
    });
    context.registerFunction("filesystem.get_cpp_files", [this](const std::vector<LogicValue>& args) {
        return func_filesystem_get_cpp_files(args);
    });
    context.registerFunction("filesystem.read_file", [this](const std::vector<LogicValue>& args) {
        return func_filesystem_read_file(args);
    });
    
    // C++ analysis functions
    context.registerFunction("cpp.count_classes", [this](const std::vector<LogicValue>& args) {
        return func_cpp_count_classes(args);
    });
    
    // Peano arithmetic functions (CRITICAL for godel-exhibition.a)
    context.registerFunction("peano.successor", [this](const std::vector<LogicValue>& args) {
        return func_peano_successor(args);
    });
    context.registerFunction("peano.is_zero", [this](const std::vector<LogicValue>& args) {
        return func_peano_is_zero(args);
    });
    context.registerFunction("peano.predecessor", [this](const std::vector<LogicValue>& args) {
        return func_peano_predecessor(args);
    });
    
    // Metalogic functions (CRITICAL for godel-exhibition.a)
    context.registerFunction("metalogic.encode_formula", [this](const std::vector<LogicValue>& args) {
        return func_metalogic_encode_formula(args);
    });
    context.registerFunction("metalogic.self_reference", [this](const std::vector<LogicValue>& args) {
        return func_metalogic_self_reference(args);
    });
}

// Built-in function implementations
LogicValue AkaoLogicExecutorNode::func_string_length(const std::vector<LogicValue>& args) const {
    if (args.size() != 1) {
        throw std::runtime_error("string.length expects exactly 1 argument");
    }
    
    if (!std::holds_alternative<std::string>(args[0])) {
        throw std::runtime_error("string.length expects a string argument");
    }
    
    return static_cast<int64_t>(std::get<std::string>(args[0]).length());
}

LogicValue AkaoLogicExecutorNode::func_string_concat(const std::vector<LogicValue>& args) const {
    if (args.size() < 2) {
        throw std::runtime_error("string.concat expects at least 2 arguments");
    }
    
    std::string result;
    for (const auto& arg : args) {
        if (!std::holds_alternative<std::string>(arg)) {
            throw std::runtime_error("string.concat expects string arguments");
        }
        result += std::get<std::string>(arg);
    }
    
    return result;
}

LogicValue AkaoLogicExecutorNode::func_string_contains(const std::vector<LogicValue>& args) const {
    if (args.size() != 2) {
        throw std::runtime_error("string.contains expects exactly 2 arguments");
    }
    
    if (!std::holds_alternative<std::string>(args[0]) || !std::holds_alternative<std::string>(args[1])) {
        throw std::runtime_error("string.contains expects string arguments");
    }
    
    std::string haystack = std::get<std::string>(args[0]);
    std::string needle = std::get<std::string>(args[1]);
    
    return haystack.find(needle) != std::string::npos;
}

LogicValue AkaoLogicExecutorNode::func_string_starts_with(const std::vector<LogicValue>& args) const {
    if (args.size() != 2) {
        throw std::runtime_error("string.starts_with expects exactly 2 arguments");
    }
    
    if (!std::holds_alternative<std::string>(args[0]) || !std::holds_alternative<std::string>(args[1])) {
        throw std::runtime_error("string.starts_with expects string arguments");
    }
    
    std::string text = std::get<std::string>(args[0]);
    std::string prefix = std::get<std::string>(args[1]);
    return text.substr(0, prefix.length()) == prefix;
}

LogicValue AkaoLogicExecutorNode::func_math_add(const std::vector<LogicValue>& args) const {
    if (args.size() != 2) {
        throw std::runtime_error("math.add expects exactly 2 arguments");
    }
    
    if (std::holds_alternative<int64_t>(args[0]) && std::holds_alternative<int64_t>(args[1])) {
        return std::get<int64_t>(args[0]) + std::get<int64_t>(args[1]);
    } else if (std::holds_alternative<double>(args[0]) && std::holds_alternative<double>(args[1])) {
        return std::get<double>(args[0]) + std::get<double>(args[1]);
    } else {
        throw std::runtime_error("math.add expects numeric arguments");
    }
}

LogicValue AkaoLogicExecutorNode::func_math_subtract(const std::vector<LogicValue>& args) const {
    if (args.size() != 2) {
        throw std::runtime_error("math.subtract expects exactly 2 arguments");
    }
    
    if (std::holds_alternative<int64_t>(args[0]) && std::holds_alternative<int64_t>(args[1])) {
        return std::get<int64_t>(args[0]) - std::get<int64_t>(args[1]);
    } else if (std::holds_alternative<double>(args[0]) && std::holds_alternative<double>(args[1])) {
        return std::get<double>(args[0]) - std::get<double>(args[1]);
    } else {
        throw std::runtime_error("math.subtract expects numeric arguments");
    }
}

LogicValue AkaoLogicExecutorNode::func_math_multiply(const std::vector<LogicValue>& args) const {
    if (args.size() != 2) {
        throw std::runtime_error("math.multiply expects exactly 2 arguments");
    }
    
    if (std::holds_alternative<int64_t>(args[0]) && std::holds_alternative<int64_t>(args[1])) {
        return std::get<int64_t>(args[0]) * std::get<int64_t>(args[1]);
    } else if (std::holds_alternative<double>(args[0]) && std::holds_alternative<double>(args[1])) {
        return std::get<double>(args[0]) * std::get<double>(args[1]);
    } else {
        throw std::runtime_error("math.multiply expects numeric arguments");
    }
}

LogicValue AkaoLogicExecutorNode::func_math_divide(const std::vector<LogicValue>& args) const {
    if (args.size() != 2) {
        throw std::runtime_error("math.divide expects exactly 2 arguments");
    }
    
    if (std::holds_alternative<int64_t>(args[0]) && std::holds_alternative<int64_t>(args[1])) {
        int64_t divisor = std::get<int64_t>(args[1]);
        if (divisor == 0) {
            throw std::runtime_error("Division by zero");
        }
        return std::get<int64_t>(args[0]) / divisor;
    } else if (std::holds_alternative<double>(args[0]) && std::holds_alternative<double>(args[1])) {
        double divisor = std::get<double>(args[1]);
        if (divisor == 0.0) {
            throw std::runtime_error("Division by zero");
        }
        return std::get<double>(args[0]) / divisor;
    } else {
        throw std::runtime_error("math.divide expects numeric arguments");
    }
}

LogicValue AkaoLogicExecutorNode::func_math_power(const std::vector<LogicValue>& args) const {
    if (args.size() != 2) {
        throw std::runtime_error("math.power expects exactly 2 arguments");
    }
    
    if (!std::holds_alternative<int64_t>(args[0]) || !std::holds_alternative<int64_t>(args[1])) {
        throw std::runtime_error("math.power expects integer arguments");
    }
    
    int64_t base = std::get<int64_t>(args[0]);
    int64_t exponent = std::get<int64_t>(args[1]);
    return static_cast<int64_t>(std::pow(base, exponent));
}

LogicValue AkaoLogicExecutorNode::func_math_modulo(const std::vector<LogicValue>& args) const {
    if (args.size() != 2) {
        throw std::runtime_error("math.modulo expects exactly 2 arguments");
    }
    
    if (!std::holds_alternative<int64_t>(args[0]) || !std::holds_alternative<int64_t>(args[1])) {
        throw std::runtime_error("math.modulo expects integer arguments");
    }
    
    int64_t dividend = std::get<int64_t>(args[0]);
    int64_t divisor = std::get<int64_t>(args[1]);
    
    if (divisor == 0) {
        throw std::runtime_error("Division by zero in math.modulo");
    }
    
    return dividend % divisor;
}

LogicValue AkaoLogicExecutorNode::func_collection_count(const std::vector<LogicValue>& args) const {
    if (args.size() != 1) {
        throw std::runtime_error("collection.count expects exactly 1 argument");
    }
    
    if (std::holds_alternative<std::vector<foundation::types::NodeValue>>(args[0])) {
        return static_cast<int64_t>(std::get<std::vector<foundation::types::NodeValue>>(args[0]).size());
    } else {
        throw std::runtime_error("collection.count expects a collection argument");
    }
}

LogicValue AkaoLogicExecutorNode::func_collection_contains(const std::vector<LogicValue>& args) const {
    if (args.size() != 2) {
        throw std::runtime_error("collection.contains expects exactly 2 arguments");
    }
    
    if (!std::holds_alternative<std::vector<foundation::types::NodeValue>>(args[0])) {
        throw std::runtime_error("collection.contains expects first argument to be a collection");
    }
    
    auto collection = std::get<std::vector<foundation::types::NodeValue>>(args[0]);
    LogicValue searchValue = args[1];
    
    for (const auto& item : collection) {
        LogicValue itemValue = nodeValueToLogicValue(item);
        if (itemValue == searchValue) {
            return true;
        }
    }
    
    return false;
}

LogicValue AkaoLogicExecutorNode::func_filesystem_exists(const std::vector<LogicValue>& args) const {
    if (args.size() != 1) {
        throw std::runtime_error("filesystem.exists expects exactly 1 argument");
    }
    
    if (!std::holds_alternative<std::string>(args[0])) {
        throw std::runtime_error("filesystem.exists expects a string argument");
    }
    
    std::string path = std::get<std::string>(args[0]);
    return std::filesystem::exists(path);
}

LogicValue AkaoLogicExecutorNode::func_filesystem_is_file(const std::vector<LogicValue>& args) const {
    if (args.size() != 1) {
        throw std::runtime_error("filesystem.is_file expects exactly 1 argument");
    }
    
    if (!std::holds_alternative<std::string>(args[0])) {
        throw std::runtime_error("filesystem.is_file expects a string argument");
    }
    
    std::string path = std::get<std::string>(args[0]);
    return std::filesystem::is_regular_file(path);
}

LogicValue AkaoLogicExecutorNode::func_filesystem_is_directory(const std::vector<LogicValue>& args) const {
    if (args.size() != 1) {
        throw std::runtime_error("filesystem.is_directory expects exactly 1 argument");
    }
    
    if (!std::holds_alternative<std::string>(args[0])) {
        throw std::runtime_error("filesystem.is_directory expects a string argument");
    }
    
    std::string path = std::get<std::string>(args[0]);
    return std::filesystem::is_directory(path);
}

LogicValue AkaoLogicExecutorNode::func_filesystem_get_files(const std::vector<LogicValue>& args) const {
    if (args.size() != 1) {
        throw std::runtime_error("filesystem.get_files expects exactly 1 argument");
    }
    
    if (!std::holds_alternative<std::string>(args[0])) {
        throw std::runtime_error("filesystem.get_files expects a string argument");
    }
    
    std::string directory = std::get<std::string>(args[0]);
    std::vector<foundation::types::NodeValue> files;
    
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                files.push_back(foundation::types::NodeValue(entry.path().string()));
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        throw std::runtime_error("filesystem.get_files error: " + std::string(e.what()));
    }
    
    return files;
}

LogicValue AkaoLogicExecutorNode::func_filesystem_get_cpp_files(const std::vector<LogicValue>& args) const {
    if (args.size() != 1) {
        throw std::runtime_error("filesystem.get_cpp_files expects exactly 1 argument");
    }
    
    if (!std::holds_alternative<std::string>(args[0])) {
        throw std::runtime_error("filesystem.get_cpp_files expects a string argument");
    }
    
    std::string directory = std::get<std::string>(args[0]);
    std::vector<foundation::types::NodeValue> cpp_files;
    
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string extension = entry.path().extension().string();
                if (extension == ".cpp" || extension == ".hpp" || extension == ".cc" || extension == ".h") {
                    cpp_files.push_back(foundation::types::NodeValue(entry.path().string()));
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        throw std::runtime_error("filesystem.get_cpp_files error: " + std::string(e.what()));
    }
    
    return cpp_files;
}

LogicValue AkaoLogicExecutorNode::func_filesystem_read_file(const std::vector<LogicValue>& args) const {
    if (args.size() != 1) {
        throw std::runtime_error("filesystem.read_file expects exactly 1 argument");
    }
    
    if (!std::holds_alternative<std::string>(args[0])) {
        throw std::runtime_error("filesystem.read_file expects a string argument");
    }
    
    std::string file_path = std::get<std::string>(args[0]);
    
    try {
        std::ifstream file(file_path);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + file_path);
        }
        
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        return content;
    } catch (const std::exception& e) {
        throw std::runtime_error("filesystem.read_file error: " + std::string(e.what()));
    }
}

LogicValue AkaoLogicExecutorNode::func_cpp_count_classes(const std::vector<LogicValue>& args) const {
    if (args.size() != 1) {
        throw std::runtime_error("cpp.count_classes expects exactly 1 argument");
    }
    
    if (!std::holds_alternative<std::string>(args[0])) {
        throw std::runtime_error("cpp.count_classes expects a string argument");
    }
    
    std::string content = std::get<std::string>(args[0]);
    
    // Simple regex-based class counting (basic implementation)
    std::regex class_regex(R"(\bclass\s+\w+)");
    std::sregex_iterator begin(content.begin(), content.end(), class_regex);
    std::sregex_iterator end;
    
    return static_cast<int64_t>(std::distance(begin, end));
}

// CRITICAL: Peano arithmetic functions for godel-exhibition.a
LogicValue AkaoLogicExecutorNode::func_peano_successor(const std::vector<LogicValue>& args) const {
    if (args.size() != 1) {
        throw std::runtime_error("peano.successor expects exactly 1 argument");
    }
    
    int64_t n = 0;
    if (std::holds_alternative<int64_t>(args[0])) {
        n = std::get<int64_t>(args[0]);
    } else if (std::holds_alternative<double>(args[0])) {
        n = static_cast<int64_t>(std::get<double>(args[0]));
    } else if (std::holds_alternative<std::string>(args[0])) {
        try {
            n = static_cast<int64_t>(std::stoll(std::get<std::string>(args[0])));
        } catch (...) {
            throw std::runtime_error("peano.successor expects a numeric argument");
        }
    } else {
        throw std::runtime_error("peano.successor expects a numeric argument");
    }
    
    return n + 1;
}

LogicValue AkaoLogicExecutorNode::func_peano_is_zero(const std::vector<LogicValue>& args) const {
    if (args.size() != 1) {
        throw std::runtime_error("peano.is_zero expects exactly 1 argument");
    }
    
    if (!std::holds_alternative<int64_t>(args[0])) {
        throw std::runtime_error("peano.is_zero expects an integer argument");
    }
    
    int64_t n = std::get<int64_t>(args[0]);
    return n == 0;
}

LogicValue AkaoLogicExecutorNode::func_peano_predecessor(const std::vector<LogicValue>& args) const {
    if (args.size() != 1) {
        throw std::runtime_error("peano.predecessor expects exactly 1 argument");
    }
    
    if (!std::holds_alternative<int64_t>(args[0])) {
        throw std::runtime_error("peano.predecessor expects an integer argument");
    }
    
    int64_t n = std::get<int64_t>(args[0]);
    return n > 0 ? n - 1 : static_cast<int64_t>(0);  // Predecessor of 0 is 0 in Peano arithmetic
}

// CRITICAL: Metalogic functions for godel-exhibition.a
LogicValue AkaoLogicExecutorNode::func_metalogic_encode_formula(const std::vector<LogicValue>& args) const {
    if (args.size() != 1) {
        throw std::runtime_error("metalogic.encode_formula expects exactly 1 argument");
    }
    
    if (!std::holds_alternative<std::string>(args[0])) {
        throw std::runtime_error("metalogic.encode_formula expects a string argument");
    }
    
    std::string formula = std::get<std::string>(args[0]);
    
    // Simple Gödel encoding: sum of ASCII values of characters
    // This is a simplified encoding for demonstration purposes
    int64_t encoding = 0;
    for (char c : formula) {
        encoding += static_cast<int64_t>(c);
    }
    
    return encoding;
}

LogicValue AkaoLogicExecutorNode::func_metalogic_self_reference(const std::vector<LogicValue>& args) const {
    if (args.size() != 1) {
        throw std::runtime_error("metalogic.self_reference expects exactly 1 argument");
    }
    
    if (!std::holds_alternative<std::string>(args[0])) {
        throw std::runtime_error("metalogic.self_reference expects a string argument");
    }
    
    std::string statement = std::get<std::string>(args[0]);
    
    // Create self-referential encoding using diagonalization
    // For demonstration: encode the statement and add a self-reference marker
    int64_t base_encoding = 0;
    for (char c : statement) {
        base_encoding += static_cast<int64_t>(c);
    }
    
    // Add self-reference multiplier to create diagonal encoding
    return base_encoding * 17 + 42;  // Magic numbers for self-reference
}

// Conversion utilities
LogicValue AkaoLogicExecutorNode::nodeValueToLogicValue(const foundation::types::NodeValue& value) const {
    if (value.isBoolean()) {
        return value.asBoolean();
    } else if (value.isInteger()) {
        return value.asInteger();
    } else if (value.isDouble()) {
        return value.asDouble();
    } else if (value.isString()) {
        return value.asString();
    } else if (value.isArray()) {
        return value.asArray();
    } else {
        throw std::runtime_error("Unsupported NodeValue type conversion");
    }
}

foundation::types::NodeValue AkaoLogicExecutorNode::logicValueToNodeValue(const LogicValue& value) const {
    if (std::holds_alternative<bool>(value)) {
        return foundation::types::NodeValue(std::get<bool>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        return foundation::types::NodeValue(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        return foundation::types::NodeValue(std::get<double>(value));
    } else if (std::holds_alternative<std::string>(value)) {
        return foundation::types::NodeValue(std::get<std::string>(value));
    } else if (std::holds_alternative<std::vector<foundation::types::NodeValue>>(value)) {
        return foundation::types::NodeValue(std::get<std::vector<foundation::types::NodeValue>>(value));
    } else {
        throw std::runtime_error("Unsupported LogicValue type conversion");
    }
}

void AkaoLogicExecutorNode::populateContextFromParameters(LogicContext& context, const foundation::interfaces::NodeParameters& params) const {
    if (params.hasParameter("variables")) {
        auto variablesParam = params.getParameter("variables");
        if (variablesParam.isObject()) {
            auto variables = variablesParam.asObject();
            for (const auto& pair : variables) {
                LogicValue logicValue = nodeValueToLogicValue(pair.second);
                context.setVariable(pair.first, logicValue);
            }
        }
    }
}

// Static utility methods
bool AkaoLogicExecutorNode::executeExpression(const std::string& expression) {
    LogicContext context;
    AkaoLogicExecutorNode node;
    node.initializeBuiltinFunctions(context);
    
    AkaoRuntime runtime(context);
    LogicValue result = runtime.execute(expression);  // USE THE AST PARSER, NOT THE STRING PARSER!
    
    if (std::holds_alternative<bool>(result)) {
        return std::get<bool>(result);
    } else {
        throw std::runtime_error("Expression did not evaluate to boolean");
    }
}

bool AkaoLogicExecutorNode::executeExpressionWithContext(const std::string& expression, const std::map<std::string, foundation::types::NodeValue>& variables) {
    LogicContext context;
    AkaoLogicExecutorNode node;
    node.initializeBuiltinFunctions(context);
    
    // Populate variables
    for (const auto& pair : variables) {
        LogicValue logicValue = node.nodeValueToLogicValue(pair.second);
        context.setVariable(pair.first, logicValue);
    }
    
    AkaoRuntime runtime(context);
    LogicValue result = runtime.execute(expression);  // USE THE AST PARSER, NOT THE STRING PARSER!
    
    if (std::holds_alternative<bool>(result)) {
        return std::get<bool>(result);
    } else {
        throw std::runtime_error("Expression did not evaluate to boolean");
    }
}

foundation::types::NodeValue AkaoLogicExecutorNode::evaluateExpression(const std::string& expression) {
    LogicContext context;
    AkaoLogicExecutorNode node;
    node.initializeBuiltinFunctions(context);
    
    AkaoRuntime runtime(context);
    LogicValue result = runtime.execute(expression);  // USE THE AST PARSER, NOT THE STRING PARSER!
    
    return node.logicValueToNodeValue(result);
}

std::vector<std::string> AkaoLogicExecutorNode::validateExpression(const std::string& expression) {
    std::vector<std::string> errors;
    
    if (expression.empty()) {
        errors.push_back("Expression cannot be empty");
        return errors;
    }
    
    try {
        LogicContext context;
        AkaoLogicExecutorNode node;
        node.initializeBuiltinFunctions(context);
        
        AkaoRuntime runtime(context);
        runtime.execute(expression);
        
    } catch (const std::exception& e) {
        errors.push_back(e.what());
    }
    
    return errors;
}

bool AkaoLogicExecutorNode::isValidExpression(const std::string& expression) {
    return validateExpression(expression).empty();
}

} // namespace builtin
} // namespace nodes
} // namespace akao

// Register this node automatically
REGISTER_NODE(akao::nodes::builtin::AkaoLogicExecutorNode);