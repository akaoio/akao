/**
 * @id: akao:file:core:engine:logic_pure:v1
 * @doc: Pure Logic Engine providing zero-hardcoded-domain runtime interpreter for .a format logical expressions with built-in function registry, variable scoping, and execution context management. Core principle: engine only knows .a format parsing and execution without any domain-specific knowledge.
 * @specification: Zero-domain runtime interpreter for pure .a format logical expressions
 * @scope: Core engine pure logic processing subsystem
 * @timeline: 2025-07-09
 * @rationale: Provide domain-agnostic logic execution engine that can interpret .a format expressions without hardcoded business rules
 * @methodology: Pure interpretation with .a format AST, operator execution, and built-in function registry
 * @references: ["akao:class:core:engine:logic:pure_logic_engine:v1", "akao:class:core:engine:logic:context:v1", "akao:class:core:engine:logic:value:v1"]
 */

#pragma once

#include "../components/parser/akao/v1.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <variant>
#include <functional>
#include <stdexcept>

namespace akao::logic {

// Forward declarations
class Context;
class Value;
class BuiltinFunction;

/**
 * @brief Exception for forall violations with specific failing values
 */
class ForallViolationException : public std::runtime_error {
public:
    std::vector<Value> failing_values;
    std::string quantifier_variable;
    
    ForallViolationException(const std::string& message, 
                           const std::vector<Value>& failures,
                           const std::string& var_name)
        : std::runtime_error(message), failing_values(failures), quantifier_variable(var_name) {}
};

/**
 * @brief Pure Logic Engine - A runtime interpreter with ZERO hardcoded domain logic
 * 
 * CORE PRINCIPLE: This engine only knows how to parse and execute .a format expressions.
 * It has NO knowledge about:
 * - "One class per file" rules
 * - Filesystem structure requirements  
 * - Code quality standards
 * - Any domain-specific logic
 * 
 * The engine ONLY knows how to:
 * - Parse .a format syntax natively
 * - Execute operators (forall, exists, and, or, not, equals, etc.)
 * - Call registered built-in functions
 * - Manage variable scoping and contexts
 * 
 * NATIVE FORMAT: .a format is the PRIMARY and ONLY format for this engine
 */
class PureLogicEngine {
public:
    PureLogicEngine();
    ~PureLogicEngine();

    // Core logic execution - .a format AST methods
    Value executeLogic(const akao::core::engine::logic::parser::ASTNode& ast, Context& ctx);
    Value evaluate(const akao::core::engine::logic::parser::ASTNode& ast, Context& ctx); // Alias for convenience  
    Value evaluate(const std::string& akao_code, Context& ctx); // Parse and execute .a format
    bool evaluateCondition(const akao::core::engine::logic::parser::ASTNode& condition, Context& ctx);
    
    // .a format support - PRIMARY NATIVE FORMAT
    Value executeAkaoFormat(const std::string& akao_code, Context& ctx);
    std::unique_ptr<akao::core::engine::logic::parser::ASTNode> parseAkaoFormat(const std::string& akao_code);
    bool isAkaoFormat(const std::string& code) const;
    
    // Function and operator execution - no domain knowledge
    Value executeFunction(const std::string& func, const std::vector<Value>& args);
    Value executeOperator(const std::string& op, const std::vector<Value>& args);
    
    // Built-in function registry management
    void registerFunction(const std::string& name, std::shared_ptr<BuiltinFunction> func);
    void registerFunction(std::unique_ptr<BuiltinFunction> func);
    bool hasFunction(const std::string& name) const;
    
    // Rule and philosophy execution - pure interpretation
    std::vector<Value> validateWithRules(const std::vector<std::string>& rule_files, Context& ctx);
    bool executePhilosophySelfProof(const std::string& philosophy_file);
    bool executeRuleSelfValidation(const std::string& rule_file);
    
    // Enhanced rule system methods
    Value executeRule(const std::string& rule_file, Context& ctx);
    bool executeRuleUnitTests(const std::string& rule_file);
    Value executePhilosophy(const std::string& philosophy_file, Context& ctx);
    std::vector<Value> executeAllRuleTests(const std::string& rule_file);

    // Performance and debugging
    void enableCaching(bool enable = true);
    void enableTracing(bool enable = true);
    void printTrace() const;
    
    // Engine introspection  
    size_t getBuiltinFunctionCount() const;
    std::vector<std::string> getExecutionTrace() const;
    
    // Metrics and diagnostics
    struct Metrics {
        size_t function_calls = 0;
        size_t cache_hits = 0;
        size_t cache_misses = 0;
    };
    Metrics getMetrics() const;
    void resetMetrics();
    
    // Initialization
    void initialize();

private:
    // Internal execution methods - pure AST handling
    Value executeProgram(const akao::core::engine::logic::parser::ASTNode& program, Context& ctx);
    Value executeStatement(const akao::core::engine::logic::parser::ASTNode& statement, Context& ctx);
    Value executeExpression(const akao::core::engine::logic::parser::ASTNode& expression, Context& ctx);
    Value executeBinaryOp(const akao::core::engine::logic::parser::ASTNode& binary_op, Context& ctx);
    Value executeUnaryOp(const akao::core::engine::logic::parser::ASTNode& unary_op, Context& ctx);
    Value executeFunctionCall(const akao::core::engine::logic::parser::ASTNode& func_call, Context& ctx);
    Value executeQuantifier(const akao::core::engine::logic::parser::ASTNode& quantifier, Context& ctx);
    Value executeConditional(const akao::core::engine::logic::parser::ASTNode& conditional, Context& ctx);
    Value executeVariable(const akao::core::engine::logic::parser::ASTNode& variable, Context& ctx);
    Value executeLiteral(const akao::core::engine::logic::parser::ASTNode& literal, Context& ctx);
    Value executeCollection(const akao::core::engine::logic::parser::ASTNode& collection, Context& ctx);
    
    // Error handling
    void throwLogicError(const std::string& message, const akao::core::engine::logic::parser::ASTNode& context);
    
    // Function registry - pure computational functions only
    std::map<std::string, std::shared_ptr<BuiltinFunction>> builtin_functions_;

    // Performance optimization
    std::map<std::string, Value> expression_cache_;
    bool caching_enabled_;
    
    // Execution tracing
    bool tracing_enabled_;
    std::vector<std::string> execution_trace_;
    int trace_depth_;
    
    // Metrics
    mutable Metrics metrics_;
    
    // Helper methods
    bool valuesEqual(const Value& a, const Value& b);
    bool containsVariables(const akao::core::engine::logic::parser::ASTNode& node);
    std::string nodeToString(const akao::core::engine::logic::parser::ASTNode& node);
    void clearCache();
    void registerAdvancedMathFunctions();
};

/**
 * @brief Execution context for variable bindings and scoping
 * 
 * Manages variable scopes during logic execution.
 * No domain knowledge - just variable name -> value mappings.
 */
class Context {
public:
    Context();
    Context(const Context& parent);
    
    void bindVariable(const std::string& name, const Value& value);
    void setValue(const std::string& name, const Value& value); // Alias for bindVariable
    Value getVariable(const std::string& name) const;
    bool hasVariable(const std::string& name) const;
    
    // Scope management
    void pushScope();
    void popScope();
    
private:
    std::vector<std::map<std::string, Value>> scopes_;
    const Context* parent_;
};

/**
 * @brief Typed value container for logic expressions
 * 
 * Supports basic types needed for logic evaluation.
 * No domain-specific types - just computational primitives.
 */
class Value {
public:
    enum class Type {
        BOOLEAN,
        INTEGER, 
        FLOAT,
        STRING,
        COLLECTION,
        OBJECT,
        NULL_VALUE
    };
    
    // Constructors for different types
    Value();
    Value(bool value);
    Value(int value);
    Value(double value);
    Value(const char* value);  // Handle string literals
    Value(const std::string& value);
    Value(const std::vector<Value>& collection);
    Value(const std::map<std::string, Value>& object);
    
    // Type checking
    Type getType() const;
    bool isBoolean() const;
    bool isBool() const; // Alias for isBoolean
    bool isInteger() const;
    bool isFloat() const;
    bool isNumber() const; // True for integer or float
    bool isString() const;
    bool isCollection() const;
    bool isList() const; // Alias for isCollection
    bool isObject() const;
    bool isNull() const;
    
    // Value extraction
    bool asBoolean() const;
    bool asBool() const; // Alias for asBoolean
    int asInteger() const;
    double asFloat() const;
    double asNumber() const; // Returns as double (float or integer)
    std::string asString() const;
    std::vector<Value> asCollection() const;
    std::vector<Value> asList() const; // Alias for asCollection
    std::map<std::string, Value> asObject() const;
    
    // String conversion
    std::string toString() const;
    
    // Collection operations
    size_t size() const;
    Value at(size_t index) const;
    Value get(const std::string& key) const;
    bool contains(const Value& item) const;
    
    // Comparison operations
    bool operator==(const Value& other) const;
    bool operator!=(const Value& other) const;
    bool operator<(const Value& other) const;
    bool operator<=(const Value& other) const;
    bool operator>(const Value& other) const;
    bool operator>=(const Value& other) const;
    
private:
    Type type_;
    std::variant<bool, int, double, std::string, std::vector<Value>, std::map<std::string, Value>> data_;
};

/**
 * @brief Abstract base class for built-in functions
 * 
 * Built-in functions provide computational primitives but NO domain logic.
 * Examples: filesystem.get_files(), cpp.count_classes(), collection.count()
 * These are pure computational functions that the logic engine can call.
 */
class BuiltinFunction {
public:
    virtual ~BuiltinFunction() = default;
    
    virtual Value execute(const std::vector<Value>& args, Context& ctx) = 0;
    virtual std::string getName() const = 0;
    virtual std::vector<Value::Type> getParameterTypes() const = 0;
    virtual Value::Type getReturnType() const = 0;
    virtual std::string getDescription() const = 0;
    
protected:
    void validateArgs(const std::vector<Value>& args) const;
};

} // namespace akao::logic