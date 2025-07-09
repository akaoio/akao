/**
 * @id: akao:file:core:engine:logic_pure_impl:v1
 * @doc: Implementation of Pure Logic Engine - A truly functional logical programming language runtime with .a format as native builtin format, comprehensive logical programming features, and zero dependencies on YAML.
 * @specification: Complete functional logical programming language with native .a format AST execution
 * @scope: Core engine pure logic implementation
 * @timeline: 2025-07-09
 * @rationale: Provide a complete logical programming language that handles .a format natively without external dependencies
 * @methodology: AST-based execution, functional programming principles, and comprehensive logical operations
 * @references: ["akao:file:core:engine:logic_pure:v1"]
 */

#include "v1.hpp"
#include "../components/builtin/v1.hpp"
#include "../components/meta/v1.hpp"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <stack>
#include <regex>
#include <fstream>
#include <cmath>

namespace akao::logic {

// =============================================================================
// Value Implementation
// =============================================================================

Value::Value() : type_(Type::NULL_VALUE), data_(false) {}
Value::Value(bool value) : type_(Type::BOOLEAN), data_(value) {}
Value::Value(int value) : type_(Type::INTEGER), data_(value) {}
Value::Value(double value) : type_(Type::FLOAT), data_(value) {}
Value::Value(const char* value) : type_(Type::STRING), data_(std::string(value)) {}
Value::Value(const std::string& value) : type_(Type::STRING), data_(value) {}
Value::Value(const std::vector<Value>& collection) : type_(Type::COLLECTION), data_(collection) {}
Value::Value(const std::map<std::string, Value>& object) : type_(Type::OBJECT), data_(object) {}

Value::Type Value::getType() const { return type_; }
bool Value::isBoolean() const { return type_ == Type::BOOLEAN; }
bool Value::isBool() const { return isBoolean(); }
bool Value::isInteger() const { return type_ == Type::INTEGER; }
bool Value::isFloat() const { return type_ == Type::FLOAT; }
bool Value::isNumber() const { return isInteger() || isFloat(); }
bool Value::isString() const { return type_ == Type::STRING; }
bool Value::isCollection() const { return type_ == Type::COLLECTION; }
bool Value::isList() const { return isCollection(); }
bool Value::isObject() const { return type_ == Type::OBJECT; }
bool Value::isNull() const { return type_ == Type::NULL_VALUE; }

bool Value::asBoolean() const {
    if (type_ == Type::BOOLEAN) return std::get<bool>(data_);
    if (type_ == Type::INTEGER) return std::get<int>(data_) != 0;
    if (type_ == Type::FLOAT) return std::get<double>(data_) != 0.0;
    if (type_ == Type::STRING) return !std::get<std::string>(data_).empty();
    if (type_ == Type::COLLECTION) return !std::get<std::vector<Value>>(data_).empty();
    return false;
}

bool Value::asBool() const { return asBoolean(); }

int Value::asInteger() const {
    if (type_ == Type::INTEGER) return std::get<int>(data_);
    if (type_ == Type::FLOAT) return static_cast<int>(std::get<double>(data_));
    if (type_ == Type::BOOLEAN) return std::get<bool>(data_) ? 1 : 0;
    if (type_ == Type::STRING) {
        try {
            return std::stoi(std::get<std::string>(data_));
        } catch (...) {
            return 0;
        }
    }
    return 0;
}

double Value::asFloat() const {
    if (type_ == Type::FLOAT) return std::get<double>(data_);
    if (type_ == Type::INTEGER) return static_cast<double>(std::get<int>(data_));
    if (type_ == Type::BOOLEAN) return std::get<bool>(data_) ? 1.0 : 0.0;
    if (type_ == Type::STRING) {
        try {
            return std::stod(std::get<std::string>(data_));
        } catch (...) {
            return 0.0;
        }
    }
    return 0.0;
}

double Value::asNumber() const { return asFloat(); }

std::string Value::asString() const {
    if (type_ == Type::STRING) return std::get<std::string>(data_);
    if (type_ == Type::BOOLEAN) return std::get<bool>(data_) ? "true" : "false";
    if (type_ == Type::INTEGER) return std::to_string(std::get<int>(data_));
    if (type_ == Type::FLOAT) return std::to_string(std::get<double>(data_));
    if (type_ == Type::NULL_VALUE) return "null";
    return toString();
}

std::vector<Value> Value::asCollection() const {
    if (type_ == Type::COLLECTION) return std::get<std::vector<Value>>(data_);
    return {};
}

std::vector<Value> Value::asList() const { return asCollection(); }

std::map<std::string, Value> Value::asObject() const {
    if (type_ == Type::OBJECT) return std::get<std::map<std::string, Value>>(data_);
    return {};
}

std::string Value::toString() const {
    switch (type_) {
        case Type::BOOLEAN: return std::get<bool>(data_) ? "true" : "false";
        case Type::INTEGER: return std::to_string(std::get<int>(data_));
        case Type::FLOAT: return std::to_string(std::get<double>(data_));
        case Type::STRING: return std::get<std::string>(data_);
        case Type::COLLECTION: {
            auto coll = std::get<std::vector<Value>>(data_);
            std::string result = "[";
            for (size_t i = 0; i < coll.size(); ++i) {
                if (i > 0) result += ", ";
                result += coll[i].toString();
            }
            result += "]";
            return result;
        }
        case Type::OBJECT: {
            auto obj = std::get<std::map<std::string, Value>>(data_);
            std::string result = "{";
            bool first = true;
            for (const auto& [key, value] : obj) {
                if (!first) result += ", ";
                result += key + ": " + value.toString();
                first = false;
            }
            result += "}";
            return result;
        }
        case Type::NULL_VALUE: return "null";
    }
    return "unknown";
}

size_t Value::size() const {
    if (type_ == Type::COLLECTION) return std::get<std::vector<Value>>(data_).size();
    if (type_ == Type::STRING) return std::get<std::string>(data_).size();
    return 0;
}

Value Value::at(size_t index) const {
    if (type_ == Type::COLLECTION) {
        auto coll = std::get<std::vector<Value>>(data_);
        if (index < coll.size()) return coll[index];
    }
    return Value();
}

Value Value::get(const std::string& key) const {
    if (type_ == Type::OBJECT) {
        auto obj = std::get<std::map<std::string, Value>>(data_);
        auto it = obj.find(key);
        if (it != obj.end()) return it->second;
    }
    return Value();
}

bool Value::contains(const Value& item) const {
    if (type_ == Type::COLLECTION) {
        auto coll = std::get<std::vector<Value>>(data_);
        return std::find(coll.begin(), coll.end(), item) != coll.end();
    }
    if (type_ == Type::STRING && item.isString()) {
        return std::get<std::string>(data_).find(item.asString()) != std::string::npos;
    }
    return false;
}

bool Value::operator==(const Value& other) const {
    if (type_ != other.type_) return false;
    return data_ == other.data_;
}

bool Value::operator!=(const Value& other) const { return !(*this == other); }

bool Value::operator<(const Value& other) const {
    if (type_ != other.type_) return type_ < other.type_;
    return data_ < other.data_;
}

bool Value::operator<=(const Value& other) const { return *this < other || *this == other; }
bool Value::operator>(const Value& other) const { return !(*this <= other); }
bool Value::operator>=(const Value& other) const { return !(*this < other); }

// =============================================================================
// Context Implementation
// =============================================================================

Context::Context() : parent_(nullptr) {
    scopes_.push_back(std::map<std::string, Value>());
}

Context::Context(const Context& parent) : parent_(&parent) {
    scopes_.push_back(std::map<std::string, Value>());
}

void Context::bindVariable(const std::string& name, const Value& value) {
    if (!scopes_.empty()) {
        scopes_.back()[name] = value;
    }
}

void Context::setValue(const std::string& name, const Value& value) {
    bindVariable(name, value);
}

Value Context::getVariable(const std::string& name) const {
    // Search from innermost to outermost scope
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
        auto var_it = it->find(name);
        if (var_it != it->end()) {
            return var_it->second;
        }
    }
    
    // Check parent context
    if (parent_) {
        return parent_->getVariable(name);
    }
    
    return Value(); // null value
}

bool Context::hasVariable(const std::string& name) const {
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
        if (it->find(name) != it->end()) {
            return true;
        }
    }
    if (parent_) {
        return parent_->hasVariable(name);
    }
    return false;
}

void Context::pushScope() {
    scopes_.push_back(std::map<std::string, Value>());
}

void Context::popScope() {
    if (scopes_.size() > 1) {
        scopes_.pop_back();
    }
}

// =============================================================================
// BuiltinFunction Implementation
// =============================================================================

void BuiltinFunction::validateArgs(const std::vector<Value>& args) const {
    auto expected_types = getParameterTypes();
    if (args.size() != expected_types.size()) {
        throw std::runtime_error("Function " + getName() + " expects " + 
                                std::to_string(expected_types.size()) + " arguments, got " + 
                                std::to_string(args.size()));
    }
    
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i].getType() != expected_types[i] && expected_types[i] != Value::Type::NULL_VALUE) {
            // For string functions, try to convert to string
            if (expected_types[i] == Value::Type::STRING) {
                continue; // Allow conversion in asString() method
            }
            throw std::runtime_error("Function " + getName() + " argument " + 
                                    std::to_string(i) + " type mismatch: expected " + 
                                    std::to_string(static_cast<int>(expected_types[i])) + 
                                    " but got " + std::to_string(static_cast<int>(args[i].getType())));
        }
    }
}

// =============================================================================
// PureLogicEngine Implementation
// =============================================================================

PureLogicEngine::PureLogicEngine() 
    : caching_enabled_(false), tracing_enabled_(false), trace_depth_(0) {
}

PureLogicEngine::~PureLogicEngine() = default;

void PureLogicEngine::initialize() {
    // Register all builtin functions
    akao::logic::builtin::registerAllBuiltinFunctions(*this);
    
    // Register advanced mathematical functions
    registerAdvancedMathFunctions();
    
    // Register meta-logic functions
    akao::logic::meta::registerMetaFunctions(*this);
}

Value PureLogicEngine::executeLogic(const akao::core::engine::logic::parser::ASTNode& ast, Context& ctx) {
    if (tracing_enabled_) {
        execution_trace_.push_back(std::string(trace_depth_ * 2, ' ') + "Executing: " + nodeToString(ast));
        trace_depth_++;
    }
    
    Value result;
    
    try {
        // Dispatch based on AST node type
        switch (ast.type) {
            case akao::core::engine::logic::parser::ASTNodeType::PROGRAM:
                result = executeProgram(ast, ctx);
                break;
            case akao::core::engine::logic::parser::ASTNodeType::STATEMENT:
                result = executeStatement(ast, ctx);
                break;
            case akao::core::engine::logic::parser::ASTNodeType::EXPRESSION:
                result = executeExpression(ast, ctx);
                break;
            case akao::core::engine::logic::parser::ASTNodeType::BINARY_OP:
                result = executeBinaryOp(ast, ctx);
                break;
            case akao::core::engine::logic::parser::ASTNodeType::UNARY_OP:
                result = executeUnaryOp(ast, ctx);
                break;
            case akao::core::engine::logic::parser::ASTNodeType::FUNCTION_CALL:
                result = executeFunctionCall(ast, ctx);
                break;
            case akao::core::engine::logic::parser::ASTNodeType::QUANTIFIER:
                result = executeQuantifier(ast, ctx);
                break;
            case akao::core::engine::logic::parser::ASTNodeType::CONDITIONAL:
                result = executeConditional(ast, ctx);
                break;
            case akao::core::engine::logic::parser::ASTNodeType::VARIABLE:
                result = executeVariable(ast, ctx);
                break;
            case akao::core::engine::logic::parser::ASTNodeType::LITERAL:
                result = executeLiteral(ast, ctx);
                break;
            case akao::core::engine::logic::parser::ASTNodeType::COLLECTION:
                result = executeCollection(ast, ctx);
                break;
            default:
                throw std::runtime_error("Unknown AST node type");
        }
        
        metrics_.function_calls++;
        
    } catch (const std::exception& e) {
        if (tracing_enabled_) {
            trace_depth_--;
        }
        throw;
    }
    
    if (tracing_enabled_) {
        trace_depth_--;
        execution_trace_.push_back(std::string(trace_depth_ * 2, ' ') + "Result: " + result.toString());
    }
    
    return result;
}

Value PureLogicEngine::evaluate(const akao::core::engine::logic::parser::ASTNode& ast, Context& ctx) {
    return executeLogic(ast, ctx);
}

Value PureLogicEngine::evaluate(const std::string& akao_code, Context& ctx) {
    return executeAkaoFormat(akao_code, ctx);
}

bool PureLogicEngine::evaluateCondition(const akao::core::engine::logic::parser::ASTNode& condition, Context& ctx) {
    Value result = executeLogic(condition, ctx);
    return result.asBoolean();
}

Value PureLogicEngine::executeAkaoFormat(const std::string& akao_code, Context& ctx) {
    try {
        auto ast = parseAkaoFormat(akao_code);
        if (!ast) {
            throw std::runtime_error("Failed to parse .a format code");
        }
        
        return executeLogic(*ast, ctx);
        
    } catch (const ForallViolationException& e) {
        // Re-throw ForallViolationException unchanged to preserve violation details
        throw;
    } catch (const std::exception& e) {
        throw std::runtime_error("Error executing .a format: " + std::string(e.what()));
    }
}

std::unique_ptr<akao::core::engine::logic::parser::ASTNode> PureLogicEngine::parseAkaoFormat(const std::string& akao_code) {
    try {
        // Use the full .a format parser
        return akao::core::engine::logic::parser::AkaoFormat::parseToAST(akao_code);
    } catch (const std::exception& e) {
        // If parsing fails, try to create a simple literal for basic values
        std::string trimmed = akao_code;
        // Remove leading/trailing whitespace
        trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
        trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
        
        // For simple literals, create a literal node
        if (trimmed == "true" || trimmed == "false" || 
            std::regex_match(trimmed, std::regex("^-?\\d+$")) ||
            (trimmed.front() == '"' && trimmed.back() == '"')) {
            return std::make_unique<akao::core::engine::logic::parser::ASTNode>(
                akao::core::engine::logic::parser::ASTNodeType::LITERAL, trimmed);
        }
        
        // Re-throw the original parsing error for complex expressions
        throw std::runtime_error("Failed to parse .a format: " + std::string(e.what()));
    }
}

bool PureLogicEngine::isAkaoFormat(const std::string& code) const {
    // Simple heuristic: .a format contains operators, function calls, or logical expressions
    return code.find("&&") != std::string::npos || 
           code.find("||") != std::string::npos ||
           code.find("forall") != std::string::npos ||
           code.find("exists") != std::string::npos ||
           code.find("∀") != std::string::npos ||
           code.find("∃") != std::string::npos ||
           code.find("(") != std::string::npos ||
           code.find(".") != std::string::npos;
}

Value PureLogicEngine::executeFunction(const std::string& func, const std::vector<Value>& args) {
    auto it = builtin_functions_.find(func);
    if (it == builtin_functions_.end()) {
        throw std::runtime_error("Unknown function: " + func);
    }
    
    Context dummy_ctx;
    return it->second->execute(args, dummy_ctx);
}

Value PureLogicEngine::executeOperator(const std::string& op, const std::vector<Value>& args) {
    if (op == "and" || op == "&&") {
        bool result = true;
        for (const auto& arg : args) {
            result = result && arg.asBoolean();
            if (!result) break; // Short-circuit
        }
        return Value(result);
    }
    
    if (op == "or" || op == "||") {
        bool result = false;
        for (const auto& arg : args) {
            result = result || arg.asBoolean();
            if (result) break; // Short-circuit
        }
        return Value(result);
    }
    
    if (op == "not" || op == "!") {
        if (args.size() != 1) {
            throw std::runtime_error("NOT operator expects exactly 1 argument");
        }
        return Value(!args[0].asBoolean());
    }
    
    if (op == "equals" || op == "==" || op == "=") {
        if (args.size() != 2) {
            throw std::runtime_error("EQUALS operator expects exactly 2 arguments");
        }
        return Value(args[0] == args[1]);
    }
    
    if (op == "not_equals" || op == "!=" || op == "≠") {
        if (args.size() != 2) {
            throw std::runtime_error("NOT_EQUALS operator expects exactly 2 arguments");
        }
        return Value(args[0] != args[1]);
    }
    
    if (op == "greater" || op == ">") {
        if (args.size() != 2) {
            throw std::runtime_error("GREATER operator expects exactly 2 arguments");
        }
        return Value(args[0] > args[1]);
    }
    
    if (op == "greater_equal" || op == ">=" || op == "≥") {
        if (args.size() != 2) {
            throw std::runtime_error("GREATER_EQUAL operator expects exactly 2 arguments");
        }
        return Value(args[0] >= args[1]);
    }
    
    if (op == "less" || op == "<") {
        if (args.size() != 2) {
            throw std::runtime_error("LESS operator expects exactly 2 arguments");
        }
        return Value(args[0] < args[1]);
    }
    
    if (op == "less_equal" || op == "<=" || op == "≤") {
        if (args.size() != 2) {
            throw std::runtime_error("LESS_EQUAL operator expects exactly 2 arguments");
        }
        
        
        return Value(args[0] <= args[1]);
    }
    
    // Advanced mathematical operations
    if (op == "add" || op == "+") {
        if (args.size() != 2) {
            throw std::runtime_error("ADD operator expects exactly 2 arguments");
        }
        if (args[0].isNumber() && args[1].isNumber()) {
            if (args[0].isFloat() || args[1].isFloat()) {
                return Value(args[0].asFloat() + args[1].asFloat());
            } else {
                return Value(args[0].asInteger() + args[1].asInteger());
            }
        }
        throw std::runtime_error("ADD operator requires numeric arguments");
    }
    
    if (op == "subtract" || op == "-") {
        if (args.size() != 2) {
            throw std::runtime_error("SUBTRACT operator expects exactly 2 arguments");
        }
        if (args[0].isNumber() && args[1].isNumber()) {
            if (args[0].isFloat() || args[1].isFloat()) {
                return Value(args[0].asFloat() - args[1].asFloat());
            } else {
                return Value(args[0].asInteger() - args[1].asInteger());
            }
        }
        throw std::runtime_error("SUBTRACT operator requires numeric arguments");
    }
    
    if (op == "multiply" || op == "*") {
        if (args.size() != 2) {
            throw std::runtime_error("MULTIPLY operator expects exactly 2 arguments");
        }
        if (args[0].isNumber() && args[1].isNumber()) {
            if (args[0].isFloat() || args[1].isFloat()) {
                return Value(args[0].asFloat() * args[1].asFloat());
            } else {
                return Value(args[0].asInteger() * args[1].asInteger());
            }
        }
        throw std::runtime_error("MULTIPLY operator requires numeric arguments");
    }
    
    if (op == "divide" || op == "/") {
        if (args.size() != 2) {
            throw std::runtime_error("DIVIDE operator expects exactly 2 arguments");
        }
        if (args[0].isNumber() && args[1].isNumber()) {
            double divisor = args[1].asFloat();
            if (divisor == 0.0) {
                throw std::runtime_error("Division by zero");
            }
            return Value(args[0].asFloat() / divisor);
        }
        throw std::runtime_error("DIVIDE operator requires numeric arguments");
    }
    
    if (op == "modulo" || op == "%") {
        if (args.size() != 2) {
            throw std::runtime_error("MODULO operator expects exactly 2 arguments");
        }
        if (args[0].isNumber() && args[1].isNumber()) {
            int divisor = args[1].asInteger();
            if (divisor == 0) {
                throw std::runtime_error("Division by zero in modulo");
            }
            return Value(args[0].asInteger() % divisor);
        }
        throw std::runtime_error("MODULO operator requires numeric arguments");
    }
    
    if (op == "power" || op == "**") {
        if (args.size() != 2) {
            throw std::runtime_error("POWER operator expects exactly 2 arguments");
        }
        if (args[0].isNumber() && args[1].isNumber()) {
            double base = args[0].asFloat();
            double exponent = args[1].asFloat();
            return Value(std::pow(base, exponent));
        }
        throw std::runtime_error("POWER operator requires numeric arguments");
    }
    
    throw std::runtime_error("Unknown operator: " + op);
}

void PureLogicEngine::registerFunction(const std::string& name, std::shared_ptr<BuiltinFunction> func) {
    builtin_functions_[name] = func;
    if (tracing_enabled_) {
        std::cout << "✅ Registered function: " << name << std::endl;
    }
}

void PureLogicEngine::registerFunction(std::unique_ptr<BuiltinFunction> func) {
    std::string name = func->getName();
    builtin_functions_[name] = std::shared_ptr<BuiltinFunction>(func.release());
    if (tracing_enabled_) {
        std::cout << "✅ Registered function: " << name << std::endl;
    }
}

bool PureLogicEngine::hasFunction(const std::string& name) const {
    return builtin_functions_.find(name) != builtin_functions_.end();
}

// AST Node Execution Methods

Value PureLogicEngine::executeProgram(const akao::core::engine::logic::parser::ASTNode& program, Context& ctx) {
    Value last_result;
    for (const auto& child : program.children) {
        last_result = executeLogic(*child, ctx);
    }
    return last_result;
}

Value PureLogicEngine::executeStatement(const akao::core::engine::logic::parser::ASTNode& statement, Context& ctx) {
    Value last_result;
    for (const auto& child : statement.children) {
        last_result = executeLogic(*child, ctx);
    }
    return last_result;
}

Value PureLogicEngine::executeExpression(const akao::core::engine::logic::parser::ASTNode& expression, Context& ctx) {
    if (!expression.children.empty()) {
        return executeLogic(*expression.children[0], ctx);
    }
    return Value();
}

Value PureLogicEngine::executeBinaryOp(const akao::core::engine::logic::parser::ASTNode& binary_op, Context& ctx) {
    if (binary_op.children.size() < 2) {
        throw std::runtime_error("Binary operator requires 2 operands");
    }
    
    Value left = executeLogic(*binary_op.children[0], ctx);
    Value right = executeLogic(*binary_op.children[1], ctx);
    
    return executeOperator(binary_op.value, {left, right});
}

Value PureLogicEngine::executeUnaryOp(const akao::core::engine::logic::parser::ASTNode& unary_op, Context& ctx) {
    if (unary_op.children.empty()) {
        throw std::runtime_error("Unary operator requires 1 operand");
    }
    
    Value operand = executeLogic(*unary_op.children[0], ctx);
    
    return executeOperator(unary_op.value, {operand});
}

Value PureLogicEngine::executeFunctionCall(const akao::core::engine::logic::parser::ASTNode& func_call, Context& ctx) {
    std::vector<Value> args;
    for (const auto& child : func_call.children) {
        args.push_back(executeLogic(*child, ctx));
    }
    
    return executeFunction(func_call.value, args);
}

Value PureLogicEngine::executeQuantifier(const akao::core::engine::logic::parser::ASTNode& quantifier, Context& ctx) {
    if (quantifier.children.size() < 2) {
        throw std::runtime_error("Quantifier requires domain and condition");
    }
    
    std::string quantifier_type = quantifier.value;
    
    // Extract variable name from attribute
    std::string var_name = quantifier.getAttribute("variable");
    if (var_name.empty()) {
        throw std::runtime_error("Quantifier variable name cannot be empty");
    }
    
    // Evaluate domain (first child)
    Value domain = executeLogic(*quantifier.children[0], ctx);
    if (!domain.isCollection()) {
        throw std::runtime_error("Quantifier domain must be a collection");
    }
    
    // Get the condition (second child)
    const auto& condition = quantifier.children[1];
    
    // Create new scope for quantifier variable
    ctx.pushScope();
    
    bool result = false;
    auto domain_collection = domain.asCollection();
    
    if (quantifier_type == "forall" || quantifier_type == "∀") {
        // Universal quantifier: all elements must satisfy condition
        result = true;
        std::vector<Value> failing_values;
        
        for (const auto& element : domain_collection) {
            // Bind variable with $ prefix (as it appears in AST)
            ctx.bindVariable(var_name, element);
            
            // Also bind without $ prefix for executeVariable compatibility
            std::string bare_var_name = var_name;
            if (bare_var_name[0] == '$') {
                bare_var_name = bare_var_name.substr(1);
            }
            ctx.bindVariable(bare_var_name, element);
            
            Value condition_result = executeLogic(*condition, ctx);
            if (!condition_result.asBoolean()) {
                failing_values.push_back(element);
                result = false;
                // Continue collecting all failing values instead of breaking
            }
        }
        
        // If any values failed, throw structured exception with details
        if (!failing_values.empty()) {
            std::string message = "Forall violation: " + std::to_string(failing_values.size()) + 
                                " values failed condition for variable " + var_name;
            throw ForallViolationException(message, failing_values, var_name);
        }
    } else if (quantifier_type == "exists" || quantifier_type == "∃") {
        // Existential quantifier: at least one element must satisfy condition
        result = false;
        for (const auto& element : domain_collection) {
            ctx.bindVariable(var_name, element);
            Value condition_result = executeLogic(*condition, ctx);
            if (condition_result.asBoolean()) {
                result = true;
                break; // Short-circuit: if any element succeeds, exists succeeds
            }
        }
    } else {
        ctx.popScope();
        throw std::runtime_error("Unknown quantifier type: " + quantifier_type);
    }
    
    ctx.popScope();
    return Value(result);
}

Value PureLogicEngine::executeConditional(const akao::core::engine::logic::parser::ASTNode& conditional, Context& ctx) {
    if (conditional.children.size() < 3) {
        throw std::runtime_error("Conditional requires condition, then, and else expressions");
    }
    
    Value condition = executeLogic(*conditional.children[0], ctx);
    
    if (condition.asBoolean()) {
        return executeLogic(*conditional.children[1], ctx);
    } else {
        return executeLogic(*conditional.children[2], ctx);
    }
}

Value PureLogicEngine::executeVariable(const akao::core::engine::logic::parser::ASTNode& variable, Context& ctx) {
    std::string var_name = variable.value;
    if (var_name.empty()) {
        throw std::runtime_error("Empty variable name");
    }
    
    // Remove $ prefix if present
    if (var_name[0] == '$') {
        var_name = var_name.substr(1);
    }
    
    // Check if variable exists in context
    if (ctx.hasVariable(var_name)) {
        return ctx.getVariable(var_name);
    }
    
    // If variable not found, treat as literal identifier (for backward compatibility)
    return Value(var_name);
}

Value PureLogicEngine::executeLiteral(const akao::core::engine::logic::parser::ASTNode& literal, Context& ctx) {
    (void)ctx; // Literals don't need context
    
    std::string value = literal.value;
    
    // Check if we have a stored literal type
    std::string literal_type = literal.getAttribute("literal_type");
    if (!literal_type.empty()) {
        int type_int = std::stoi(literal_type);
        auto token_type = static_cast<akao::core::engine::logic::parser::TokenType>(type_int);
        
        switch (token_type) {
            case akao::core::engine::logic::parser::TokenType::TRUE:
                return Value(true);
            case akao::core::engine::logic::parser::TokenType::FALSE:
                return Value(false);
            case akao::core::engine::logic::parser::TokenType::INTEGER:
                return Value(std::stoi(value));
            case akao::core::engine::logic::parser::TokenType::STRING:
                return Value(value); // Value is already without quotes from tokenizer
            default:
                break;
        }
    }
    
    // Fallback to old parsing logic
    // Boolean literals
    if (value == "true") return Value(true);
    if (value == "false") return Value(false);
    
    // Integer literals
    try {
        int int_val = std::stoi(value);
        return Value(int_val);
    } catch (...) {
        // Not an integer, treat as string
    }
    
    // String literals - remove quotes if present
    if (value.length() >= 2 && value.front() == '"' && value.back() == '"') {
        return Value(value.substr(1, value.length() - 2));
    }
    return Value(value);
}

Value PureLogicEngine::executeCollection(const akao::core::engine::logic::parser::ASTNode& collection, Context& ctx) {
    std::vector<Value> elements;
    for (const auto& child : collection.children) {
        elements.push_back(executeLogic(*child, ctx));
    }
    return Value(elements);
}

// Helper methods

std::string PureLogicEngine::nodeToString(const akao::core::engine::logic::parser::ASTNode& node) {
    return node.value.empty() ? "[expression]" : node.value;
}

bool PureLogicEngine::containsVariables(const akao::core::engine::logic::parser::ASTNode& node) {
    if (node.type == akao::core::engine::logic::parser::ASTNodeType::VARIABLE) {
        return true;
    }
    
    for (const auto& child : node.children) {
        if (containsVariables(*child)) return true;
    }
    
    return false;
}

// Performance and metrics methods

void PureLogicEngine::enableCaching(bool enable) {
    caching_enabled_ = enable;
    if (!enable) {
        clearCache();
    }
}

void PureLogicEngine::enableTracing(bool enable) {
    tracing_enabled_ = enable;
    if (enable) {
        std::cout << "✅ .a format execution tracing enabled" << std::endl;
    }
}

void PureLogicEngine::printTrace() const {
    for (const auto& entry : execution_trace_) {
        std::cout << entry << std::endl;
    }
}

size_t PureLogicEngine::getBuiltinFunctionCount() const {
    return builtin_functions_.size();
}

std::vector<std::string> PureLogicEngine::getExecutionTrace() const {
    return execution_trace_;
}

PureLogicEngine::Metrics PureLogicEngine::getMetrics() const {
    return metrics_;
}

void PureLogicEngine::resetMetrics() {
    metrics_ = Metrics{};
}

void PureLogicEngine::clearCache() {
    expression_cache_.clear();
}

// Placeholder implementations for rule execution

std::vector<Value> PureLogicEngine::validateWithRules(const std::vector<std::string>& rule_files, Context& ctx) {
    std::vector<Value> results;
    
    for (const auto& rule_file : rule_files) {
        try {
            Value rule_result = executeRule(rule_file, ctx);
            results.push_back(rule_result);
        } catch (const std::exception& e) {
            // Store error as string value
            results.push_back(Value("ERROR: " + std::string(e.what())));
        }
    }
    
    return results;
}

Value PureLogicEngine::executeRule(const std::string& rule_file, Context& ctx) {
    try {
        // Read rule file
        std::ifstream file(rule_file);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open rule file: " + rule_file);
        }
        
        std::string rule_content((std::istreambuf_iterator<char>(file)), 
                                std::istreambuf_iterator<char>());
        file.close();
        
        // Execute rule as .a format code
        return executeAkaoFormat(rule_content, ctx);
        
    } catch (const std::exception& e) {
        throw std::runtime_error("Rule execution failed for " + rule_file + ": " + e.what());
    }
}

Value PureLogicEngine::executePhilosophy(const std::string& philosophy_file, Context& ctx) {
    try {
        // Read philosophy file
        std::ifstream file(philosophy_file);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open philosophy file: " + philosophy_file);
        }
        
        std::string philosophy_content((std::istreambuf_iterator<char>(file)), 
                                      std::istreambuf_iterator<char>());
        file.close();
        
        // Execute philosophy as .a format code
        return executeAkaoFormat(philosophy_content, ctx);
        
    } catch (const std::exception& e) {
        throw std::runtime_error("Philosophy execution failed for " + philosophy_file + ": " + e.what());
    }
}

bool PureLogicEngine::executePhilosophySelfProof(const std::string& philosophy_file) {
    try {
        Context ctx;
        Value result = executePhilosophy(philosophy_file, ctx);
        return result.asBoolean();
    } catch (const std::exception&) {
        return false;
    }
}

bool PureLogicEngine::executeRuleSelfValidation(const std::string& rule_file) {
    try {
        Context ctx;
        Value result = executeRule(rule_file, ctx);
        return result.asBoolean();
    } catch (const std::exception&) {
        return false;
    }
}

bool PureLogicEngine::executeRuleUnitTests(const std::string& rule_file) {
    try {
        // Look for corresponding test file
        std::string test_file = rule_file;
        size_t last_dot = test_file.find_last_of('.');
        if (last_dot != std::string::npos) {
            test_file = test_file.substr(0, last_dot) + ".test.a";
        } else {
            test_file += ".test.a";
        }
        
        Context ctx;
        Value result = executeRule(test_file, ctx);
        return result.asBoolean();
    } catch (const std::exception&) {
        return false;
    }
}

std::vector<Value> PureLogicEngine::executeAllRuleTests(const std::string& rule_file) {
    std::vector<Value> results;
    
    try {
        // Execute self-validation
        bool self_valid = executeRuleSelfValidation(rule_file);
        results.push_back(Value(self_valid));
        
        // Execute unit tests
        bool unit_tests_pass = executeRuleUnitTests(rule_file);
        results.push_back(Value(unit_tests_pass));
        
        // Execute the rule itself
        Context ctx;
        Value rule_result = executeRule(rule_file, ctx);
        results.push_back(rule_result);
        
    } catch (const std::exception& e) {
        results.push_back(Value("ERROR: " + std::string(e.what())));
    }
    
    return results;
}

// Error handling

void PureLogicEngine::throwLogicError(const std::string& message, const akao::core::engine::logic::parser::ASTNode& context) {
    (void)context;
    throw std::runtime_error("Logic Error: " + message);
}

// Helper methods

bool PureLogicEngine::valuesEqual(const Value& a, const Value& b) {
    return a == b;
}

// Advanced mathematical functions implementation
class SqrtFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override {
        (void)ctx;
        validateArgs(args);
        if (!args[0].isNumber()) {
            throw std::runtime_error("sqrt requires numeric argument");
        }
        double val = args[0].asFloat();
        if (val < 0) {
            throw std::runtime_error("sqrt of negative number");
        }
        return Value(std::sqrt(val));
    }
    
    std::string getName() const override { return "sqrt"; }
    std::vector<Value::Type> getParameterTypes() const override { return {Value::Type::FLOAT}; }
    Value::Type getReturnType() const override { return Value::Type::FLOAT; }
    std::string getDescription() const override { return "Square root function"; }
};

class SinFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override {
        (void)ctx;
        validateArgs(args);
        if (!args[0].isNumber()) {
            throw std::runtime_error("sin requires numeric argument");
        }
        return Value(std::sin(args[0].asFloat()));
    }
    
    std::string getName() const override { return "sin"; }
    std::vector<Value::Type> getParameterTypes() const override { return {Value::Type::FLOAT}; }
    Value::Type getReturnType() const override { return Value::Type::FLOAT; }
    std::string getDescription() const override { return "Sine function"; }
};

class CosFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override {
        (void)ctx;
        validateArgs(args);
        if (!args[0].isNumber()) {
            throw std::runtime_error("cos requires numeric argument");
        }
        return Value(std::cos(args[0].asFloat()));
    }
    
    std::string getName() const override { return "cos"; }
    std::vector<Value::Type> getParameterTypes() const override { return {Value::Type::FLOAT}; }
    Value::Type getReturnType() const override { return Value::Type::FLOAT; }
    std::string getDescription() const override { return "Cosine function"; }
};

class TanFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override {
        (void)ctx;
        validateArgs(args);
        if (!args[0].isNumber()) {
            throw std::runtime_error("tan requires numeric argument");
        }
        return Value(std::tan(args[0].asFloat()));
    }
    
    std::string getName() const override { return "tan"; }
    std::vector<Value::Type> getParameterTypes() const override { return {Value::Type::FLOAT}; }
    Value::Type getReturnType() const override { return Value::Type::FLOAT; }
    std::string getDescription() const override { return "Tangent function"; }
};

class LogFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override {
        (void)ctx;
        validateArgs(args);
        if (!args[0].isNumber()) {
            throw std::runtime_error("log requires numeric argument");
        }
        double val = args[0].asFloat();
        if (val <= 0) {
            throw std::runtime_error("log of non-positive number");
        }
        return Value(std::log(val));
    }
    
    std::string getName() const override { return "log"; }
    std::vector<Value::Type> getParameterTypes() const override { return {Value::Type::FLOAT}; }
    Value::Type getReturnType() const override { return Value::Type::FLOAT; }
    std::string getDescription() const override { return "Natural logarithm function"; }
};

class ExpFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override {
        (void)ctx;
        validateArgs(args);
        if (!args[0].isNumber()) {
            throw std::runtime_error("exp requires numeric argument");
        }
        return Value(std::exp(args[0].asFloat()));
    }
    
    std::string getName() const override { return "exp"; }
    std::vector<Value::Type> getParameterTypes() const override { return {Value::Type::FLOAT}; }
    Value::Type getReturnType() const override { return Value::Type::FLOAT; }
    std::string getDescription() const override { return "Exponential function"; }
};

class AbsFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override {
        (void)ctx;
        validateArgs(args);
        if (!args[0].isNumber()) {
            throw std::runtime_error("abs requires numeric argument");
        }
        if (args[0].isInteger()) {
            return Value(std::abs(args[0].asInteger()));
        } else {
            return Value(std::abs(args[0].asFloat()));
        }
    }
    
    std::string getName() const override { return "abs"; }
    std::vector<Value::Type> getParameterTypes() const override { return {Value::Type::FLOAT}; }
    Value::Type getReturnType() const override { return Value::Type::FLOAT; }
    std::string getDescription() const override { return "Absolute value function"; }
};

void PureLogicEngine::registerAdvancedMathFunctions() {
    registerFunction(std::make_unique<SqrtFunction>());
    registerFunction(std::make_unique<SinFunction>());
    registerFunction(std::make_unique<CosFunction>());
    registerFunction(std::make_unique<TanFunction>());
    registerFunction(std::make_unique<LogFunction>());
    registerFunction(std::make_unique<ExpFunction>());
    registerFunction(std::make_unique<AbsFunction>());
}

} // namespace akao::logic