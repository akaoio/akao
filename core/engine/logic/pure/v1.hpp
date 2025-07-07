#pragma once

#include "../../parser/yaml_node/v1.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <variant>
#include <functional>

// Integration with AKAO's Enhanced YAML Library
// Use AKAO's production-ready internal YAML implementation

namespace akao::logic {

// Forward declarations
class Context;
class Value;
class BuiltinFunction;

/**
 * @brief Pure Logic Engine - A runtime interpreter with ZERO hardcoded domain logic
 * 
 * CORE PRINCIPLE: This engine only knows how to parse and execute logic expressions.
 * It has NO knowledge about:
 * - "One class per file" rules
 * - Filesystem structure requirements  
 * - Code quality standards
 * - Any domain-specific logic
 * 
 * The engine ONLY knows how to:
 * - Parse YAML logic syntax
 * - Execute operators (forall, exists, and, or, not, equals, etc.)
 * - Call registered built-in functions
 * - Manage variable scoping and contexts
 */
class PureLogicEngine {
public:
    PureLogicEngine();
    ~PureLogicEngine();

    // Core logic execution - pure interpreter methods
    Value executeLogic(const akao::core::engine::parser::YamlNode& logic, Context& ctx);
    Value evaluate(const akao::core::engine::parser::YamlNode& logic, Context& ctx); // Alias for convenience  
    Value evaluate(const std::string& logic_str, Context& ctx); // String convenience method
    bool evaluateCondition(const akao::core::engine::parser::YamlNode& condition, Context& ctx);
    
    // Function and operator execution - no domain knowledge
    Value executeFunction(const std::string& func, const std::vector<Value>& args);
    Value executeOperator(const std::string& op, const std::vector<Value>& args);
    
    // Built-in function registry management
    void loadBuiltinFunctions(const std::string& builtin_yaml_path);
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
    // Internal execution methods - pure syntax handling
    Value executeQuantifier(const akao::core::engine::parser::YamlNode& quantifier, Context& ctx);
    Value executeConditional(const akao::core::engine::parser::YamlNode& conditional, Context& ctx);
    Value executeFixpoint(const akao::core::engine::parser::YamlNode& fixpoint, Context& ctx);
    Value executeLiteral(const akao::core::engine::parser::YamlNode& literal);
    Value executeVariable(const std::string& var_name, Context& ctx);
    
    // Expression type detection - syntax only
    bool isQuantifier(const akao::core::engine::parser::YamlNode& node);
    bool isOperator(const akao::core::engine::parser::YamlNode& node);
    bool isFunction(const akao::core::engine::parser::YamlNode& node);
    bool isConditional(const akao::core::engine::parser::YamlNode& node);
    bool isVariable(const akao::core::engine::parser::YamlNode& node);
    bool isLiteral(const akao::core::engine::parser::YamlNode& node);
    
    // Error handling
    void throwLogicError(const std::string& message, const akao::core::engine::parser::YamlNode& context);
    
    // YAML-Value conversion helpers
    Value convertYamlToValue(const akao::core::engine::parser::YamlNode& node);

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
    bool containsVariables(const akao::core::engine::parser::YamlNode& node);
    std::string nodeToString(const akao::core::engine::parser::YamlNode& node);
    void clearCache();
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
        STRING,
        COLLECTION,
        OBJECT,
        NULL_VALUE
    };
    
    // Constructors for different types
    Value();
    Value(bool value);
    Value(int value);
    Value(const char* value);  // Handle string literals
    Value(const std::string& value);
    Value(const std::vector<Value>& collection);
    Value(const std::map<std::string, Value>& object);
    
    // Type checking
    Type getType() const;
    bool isBoolean() const;
    bool isBool() const; // Alias for isBoolean
    bool isInteger() const;
    bool isNumber() const; // Alias for isInteger
    bool isString() const;
    bool isCollection() const;
    bool isList() const; // Alias for isCollection
    bool isObject() const;
    bool isNull() const;
    
    // Value extraction
    bool asBoolean() const;
    bool asBool() const; // Alias for asBoolean
    int asInteger() const;
    double asNumber() const; // Alias for asInteger but returns double
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
    std::variant<bool, int, std::string, std::vector<Value>, std::map<std::string, Value>> data_;
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
