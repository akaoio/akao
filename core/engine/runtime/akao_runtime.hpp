#pragma once

/**
 * @file akao_runtime.hpp
 * @brief Core runtime engine for Akao Pure Logic execution environment
 * @details Header file for .a file execution, symbol resolution, and runtime management
 * 
 * ID: akao:component:core:engine:runtime:akao-runtime:v1
 * Scope: Core .a file execution environment and runtime management
 * Dependencies: core/engine/language/pure_logic_grammar.hpp, core/engine/language/symbol_parser.cpp
 */

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <chrono>
#include "ast_nodes.hpp"

namespace akao {
namespace core {
namespace engine {
namespace runtime {

/**
 * @brief Runtime value representation
 */
class RuntimeValue {
public:
    enum Type { NIL, BOOLEAN, INTEGER, FLOAT, STRING, LIST, OBJECT };
    
    RuntimeValue() : type_(NIL) {}
    RuntimeValue(bool value) : type_(BOOLEAN), bool_value_(value) {}
    RuntimeValue(int value) : type_(INTEGER), int_value_(value) {}
    RuntimeValue(double value) : type_(FLOAT), float_value_(value) {}
    RuntimeValue(const std::string& value) : type_(STRING), string_value_(value) {}
    
    Type get_type() const { return type_; }
    bool is_true() const;
    std::string to_string() const;
    std::string get_type_name() const;
    int get_length() const;
    
private:
    Type type_;
    bool bool_value_ = false;
    int int_value_ = 0;
    double float_value_ = 0.0;
    std::string string_value_;
    std::vector<RuntimeValue> list_value_;
    std::map<std::string, RuntimeValue> object_value_;
};

/**
 * @brief Execution modes
 */
enum class ExecutionMode {
    DEVELOPMENT,
    PRODUCTION,
    TESTING
};

/**
 * @brief Runtime execution result
 */
struct RuntimeResult {
    bool success = false;
    RuntimeValue return_value;
    std::string output;
    std::string error_message;
    int error_line = 0;
    int error_column = 0;
    double execution_time = 0.0;
};

/**
 * @brief Parse result structure
 */
struct ParseResult {
    bool success = false;
    std::unique_ptr<ASTNode> ast;
    std::string error_message;
    int error_line = 0;
    int error_column = 0;
};

/**
 * @brief Validation result structure
 */
struct ValidationResult {
    bool success = false;
    std::string error_message;
};

/**
 * @brief Execution result structure
 */
struct ExecutionResult {
    bool success = false;
    RuntimeValue return_value;
    std::string output;
    std::string error_message;
};

/**
 * @brief Runtime statistics
 */
struct RuntimeStats {
    size_t memory_usage = 0;
    size_t peak_memory_usage = 0;
    uint32_t total_executions = 0;
    double average_execution_time = 0.0;
    size_t symbol_table_size = 0;
};

/**
 * @brief Built-in function type
 */
using BuiltinFunction = std::function<RuntimeValue(const std::vector<RuntimeValue>&)>;

/**
 * @brief Execution context
 */
struct ExecutionContext {
    ExecutionMode mode = ExecutionMode::DEVELOPMENT;
    bool debug_mode = false;
    bool trace_execution = false;
    bool capture_output = false;
    uint32_t execution_count = 0;
    double total_execution_time = 0.0;
    std::string output_buffer;
    std::map<std::string, std::string> environment_variables;
    std::map<std::string, BuiltinFunction> builtin_functions;
};

/**
 * @brief Symbol table for variable and function storage
 */
class SymbolTable {
public:
    void set_value(const std::string& name, const RuntimeValue& value);
    RuntimeValue get_value(const std::string& name) const;
    UserFunction* get_function(const std::string& name) const;
    void push_scope();
    void pop_scope();
    void clear();
    size_t size() const;
    
private:
    std::vector<std::map<std::string, RuntimeValue>> scopes_;
    std::map<std::string, std::unique_ptr<UserFunction>> functions_;
};

/**
 * @brief Memory manager for runtime
 */
class MemoryManager {
public:
    void initialize();
    void cleanup();
    void enable_optimizations();
    size_t get_current_usage() const;
    size_t get_peak_usage() const;
    
private:
    size_t current_usage_ = 0;
    size_t peak_usage_ = 0;
    bool optimizations_enabled_ = false;
};

/**
 * @brief Main Akao runtime engine
 */
class AkaoRuntime {
public:
    AkaoRuntime();
    ~AkaoRuntime();
    
    /**
     * @brief Execute .a file
     * @param filepath Path to .a file
     * @return Execution result
     */
    RuntimeResult execute_file(const std::string& filepath);
    
    /**
     * @brief Execute source code directly
     * @param source_code .a source code
     * @param source_name Name for error reporting
     * @return Execution result
     */
    RuntimeResult execute_source(const std::string& source_code, 
                                 const std::string& source_name = "<string>");
    
    /**
     * @brief Set execution mode
     * @param mode Execution mode
     */
    void set_execution_mode(ExecutionMode mode);
    
    /**
     * @brief Set environment variable
     * @param name Variable name
     * @param value Variable value
     */
    void set_environment_variable(const std::string& name, const std::string& value);
    
    /**
     * @brief Get environment variable
     * @param name Variable name
     * @return Variable value or empty string
     */
    std::string get_environment_variable(const std::string& name) const;
    
    /**
     * @brief Register built-in function
     * @param name Function name
     * @param function Function implementation
     */
    void register_builtin_function(const std::string& name, BuiltinFunction function);
    
    /**
     * @brief Get runtime statistics
     * @return Runtime statistics
     */
    RuntimeStats get_runtime_statistics() const;
    
private:
    bool initialized_;
    std::unique_ptr<ExecutionContext> execution_context_;
    std::unique_ptr<SymbolTable> symbol_table_;
    std::unique_ptr<MemoryManager> memory_manager_;
    
    void initialize_runtime();
    void cleanup_runtime();
    
    bool validate_file_access(const std::string& filepath);
    std::string load_file_content(const std::string& filepath);
    
    ParseResult parse_source(const std::string& source_code, const std::string& source_name);
    ValidationResult validate_semantics(const std::unique_ptr<ASTNode>& ast);
    ExecutionResult execute_ast(const std::unique_ptr<ASTNode>& ast);
    
    RuntimeValue evaluate_node(ASTNode* node);
    RuntimeValue evaluate_literal(LiteralNode* node);
    RuntimeValue evaluate_symbol(SymbolNode* node);
    RuntimeValue evaluate_function_call(FunctionCallNode* node);
    RuntimeValue evaluate_assignment(AssignmentNode* node);
    RuntimeValue evaluate_conditional(ConditionalNode* node);
    RuntimeValue evaluate_loop(LoopNode* node);
    RuntimeValue evaluate_block(BlockNode* node);
    
    void initialize_builtin_symbols();
    void register_builtin_functions();
    void enable_debug_features();
    void optimize_for_performance();
    void enable_test_features();
    
    RuntimeValue execute_user_function(UserFunction* function, 
                                       const std::vector<std::unique_ptr<ASTNode>>& arguments);
    
    bool is_truthy(const RuntimeValue& value);
    std::string runtime_value_to_string(const RuntimeValue& value);
    std::string get_runtime_value_type(const RuntimeValue& value);
    int get_runtime_value_length(const RuntimeValue& value);
};

} // namespace runtime
} // namespace engine
} // namespace core
} // namespace akao
