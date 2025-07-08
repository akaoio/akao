/**
 * @file akao_runtime.cpp
 * @brief Core runtime engine for Akao Pure Logic execution environment
 * @details Provides .a file execution, symbol resolution, and runtime management
 * 
 * ID: akao:component:core:engine:runtime:akao-runtime:v1
 * Scope: Core .a file execution environment and runtime management
 * Dependencies: core/engine/language/pure_logic_grammar.hpp, core/engine/language/symbol_parser.cpp
 */

#include "akao_runtime.hpp"
#include "../language/pure_logic_grammar.hpp"
#include "../language/symbol_parser.cpp"
#include "../validator/universal-framework/v1.hpp"
#include "../compliance/automated_compliance_system.cpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <memory>
#include <algorithm>

namespace akao {
namespace core {
namespace engine {
namespace runtime {

// AkaoRuntime Implementation

AkaoRuntime::AkaoRuntime() 
    : initialized_(false)
    , execution_context_(std::make_unique<ExecutionContext>())
    , symbol_table_(std::make_unique<SymbolTable>())
    , memory_manager_(std::make_unique<MemoryManager>())
{
    initialize_runtime();
}

AkaoRuntime::~AkaoRuntime() {
    cleanup_runtime();
}

RuntimeResult AkaoRuntime::execute_file(const std::string& filepath) {
    RuntimeResult result;
    auto start_time = std::chrono::high_resolution_clock::now();
    
    try {
        // Validate file existence and readability
        if (!validate_file_access(filepath)) {
            result.success = false;
            result.error_message = "Cannot access file: " + filepath;
            return result;
        }
        
        // Load and parse .a file
        std::string source_code = load_file_content(filepath);
        if (source_code.empty()) {
            result.success = false;
            result.error_message = "Empty or unreadable file: " + filepath;
            return result;
        }
        
        // Parse source using Pure Logic grammar
        ParseResult parse_result = parse_source(source_code, filepath);
        if (!parse_result.success) {
            result.success = false;
            result.error_message = "Parse error: " + parse_result.error_message;
            result.error_line = parse_result.error_line;
            result.error_column = parse_result.error_column;
            return result;
        }
        
        // Semantic validation
        ValidationResult validation_result = validate_semantics(parse_result.ast);
        if (!validation_result.success) {
            result.success = false;
            result.error_message = "Semantic error: " + validation_result.error_message;
            return result;
        }
        
        // Execute parsed AST
        ExecutionResult exec_result = execute_ast(parse_result.ast);
        result.success = exec_result.success;
        result.return_value = exec_result.return_value;
        result.output = exec_result.output;
        
        if (!exec_result.success) {
            result.error_message = exec_result.error_message;
        }
        
    } catch (const std::exception& e) {
        result.success = false;
        result.error_message = "Runtime exception: " + std::string(e.what());
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    result.execution_time = std::chrono::duration<double>(end_time - start_time).count();
    
    return result;
}

RuntimeResult AkaoRuntime::execute_source(const std::string& source_code, const std::string& source_name) {
    RuntimeResult result;
    auto start_time = std::chrono::high_resolution_clock::now();
    
    try {
        // Parse source directly
        ParseResult parse_result = parse_source(source_code, source_name);
        if (!parse_result.success) {
            result.success = false;
            result.error_message = "Parse error: " + parse_result.error_message;
            result.error_line = parse_result.error_line;
            result.error_column = parse_result.error_column;
            return result;
        }
        
        // Semantic validation
        ValidationResult validation_result = validate_semantics(parse_result.ast);
        if (!validation_result.success) {
            result.success = false;
            result.error_message = "Semantic error: " + validation_result.error_message;
            return result;
        }
        
        // Execute parsed AST
        ExecutionResult exec_result = execute_ast(parse_result.ast);
        result.success = exec_result.success;
        result.return_value = exec_result.return_value;
        result.output = exec_result.output;
        
        if (!exec_result.success) {
            result.error_message = exec_result.error_message;
        }
        
    } catch (const std::exception& e) {
        result.success = false;
        result.error_message = "Runtime exception: " + std::string(e.what());
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    result.execution_time = std::chrono::duration<double>(end_time - start_time).count();
    
    return result;
}

void AkaoRuntime::set_execution_mode(ExecutionMode mode) {
    execution_context_->mode = mode;
    
    switch (mode) {
        case ExecutionMode::DEVELOPMENT:
            enable_debug_features();
            break;
        case ExecutionMode::PRODUCTION:
            optimize_for_performance();
            break;
        case ExecutionMode::TESTING:
            enable_test_features();
            break;
    }
}

void AkaoRuntime::set_environment_variable(const std::string& name, const std::string& value) {
    execution_context_->environment_variables[name] = value;
}

std::string AkaoRuntime::get_environment_variable(const std::string& name) const {
    auto it = execution_context_->environment_variables.find(name);
    return (it != execution_context_->environment_variables.end()) ? it->second : "";
}

void AkaoRuntime::register_builtin_function(const std::string& name, BuiltinFunction function) {
    execution_context_->builtin_functions[name] = function;
}

RuntimeStats AkaoRuntime::get_runtime_statistics() const {
    RuntimeStats stats;
    stats.memory_usage = memory_manager_->get_current_usage();
    stats.peak_memory_usage = memory_manager_->get_peak_usage();
    stats.total_executions = execution_context_->execution_count;
    stats.average_execution_time = execution_context_->total_execution_time / 
                                   std::max(1u, execution_context_->execution_count);
    stats.symbol_table_size = symbol_table_->size();
    return stats;
}

// Private Methods

void AkaoRuntime::initialize_runtime() {
    if (initialized_) return;
    
    // Initialize execution context
    execution_context_->mode = ExecutionMode::DEVELOPMENT;
    execution_context_->execution_count = 0;
    execution_context_->total_execution_time = 0.0;
    
    // Initialize symbol table with built-in symbols
    initialize_builtin_symbols();
    
    // Initialize memory manager
    memory_manager_->initialize();
    
    // Register built-in functions
    register_builtin_functions();
    
    initialized_ = true;
}

void AkaoRuntime::cleanup_runtime() {
    if (!initialized_) return;
    
    // Clean up symbol table
    symbol_table_->clear();
    
    // Clean up memory manager
    memory_manager_->cleanup();
    
    // Clear execution context
    execution_context_->builtin_functions.clear();
    execution_context_->environment_variables.clear();
    
    initialized_ = false;
}

bool AkaoRuntime::validate_file_access(const std::string& filepath) {
    std::ifstream file(filepath);
    return file.good();
}

std::string AkaoRuntime::load_file_content(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return "";
    }
    
    std::ostringstream content;
    content << file.rdbuf();
    return content.str();
}

ParseResult AkaoRuntime::parse_source(const std::string& source_code, const std::string& source_name) {
    ParseResult result;
    
    try {
        // Use Pure Logic grammar parser
        akao::language::pure_logic::PureLogicGrammar grammar;
        akao::language::pure_logic::SymbolParser parser;
        
        // Set source information
        parser.set_source_name(source_name);
        
        // Parse source code
        auto ast = parser.parse(source_code);
        if (!ast) {
            result.success = false;
            result.error_message = parser.get_last_error();
            result.error_line = parser.get_error_line();
            result.error_column = parser.get_error_column();
            return result;
        }
        
        result.success = true;
        result.ast = std::move(ast);
        
    } catch (const std::exception& e) {
        result.success = false;
        result.error_message = std::string("Parse exception: ") + e.what();
    }
    
    return result;
}

ValidationResult AkaoRuntime::validate_semantics(const std::unique_ptr<ASTNode>& ast) {
    ValidationResult result;
    
    try {
        // Use universal validator
        validator::UniversalValidator validator;
        auto validation_result = validator.validate_ast(ast.get());
        
        result.success = validation_result.is_valid;
        if (!result.success) {
            result.error_message = "Semantic validation failed";
            // Collect validation errors
            for (const auto& error : validation_result.errors) {
                result.error_message += "\n- " + error;
            }
        }
        
    } catch (const std::exception& e) {
        result.success = false;
        result.error_message = std::string("Validation exception: ") + e.what();
    }
    
    return result;
}

ExecutionResult AkaoRuntime::execute_ast(const std::unique_ptr<ASTNode>& ast) {
    ExecutionResult result;
    
    try {
        execution_context_->execution_count++;
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // Execute AST nodes
        RuntimeValue return_value = evaluate_node(ast.get());
        
        auto end_time = std::chrono::high_resolution_clock::now();
        double execution_time = std::chrono::duration<double>(end_time - start_time).count();
        execution_context_->total_execution_time += execution_time;
        
        result.success = true;
        result.return_value = return_value;
        result.output = execution_context_->output_buffer;
        
        // Clear output buffer for next execution
        execution_context_->output_buffer.clear();
        
    } catch (const std::exception& e) {
        result.success = false;
        result.error_message = std::string("Execution exception: ") + e.what();
    }
    
    return result;
}

RuntimeValue AkaoRuntime::evaluate_node(ASTNode* node) {
    if (!node) {
        return RuntimeValue{};
    }
    
    switch (node->type) {
        case ASTNodeType::LITERAL:
            return evaluate_literal(static_cast<LiteralNode*>(node));
            
        case ASTNodeType::SYMBOL:
            return evaluate_symbol(static_cast<SymbolNode*>(node));
            
        case ASTNodeType::FUNCTION_CALL:
            return evaluate_function_call(static_cast<FunctionCallNode*>(node));
            
        case ASTNodeType::ASSIGNMENT:
            return evaluate_assignment(static_cast<AssignmentNode*>(node));
            
        case ASTNodeType::CONDITIONAL:
            return evaluate_conditional(static_cast<ConditionalNode*>(node));
            
        case ASTNodeType::LOOP:
            return evaluate_loop(static_cast<LoopNode*>(node));
            
        case ASTNodeType::BLOCK:
            return evaluate_block(static_cast<BlockNode*>(node));
            
        default:
            throw std::runtime_error("Unknown AST node type");
    }
}

RuntimeValue AkaoRuntime::evaluate_literal(LiteralNode* node) {
    return node->value;
}

RuntimeValue AkaoRuntime::evaluate_symbol(SymbolNode* node) {
    return symbol_table_->get_value(node->name);
}

RuntimeValue AkaoRuntime::evaluate_function_call(FunctionCallNode* node) {
    // Check for built-in functions first
    auto builtin_it = execution_context_->builtin_functions.find(node->function_name);
    if (builtin_it != execution_context_->builtin_functions.end()) {
        std::vector<RuntimeValue> args;
        for (auto& arg : node->arguments) {
            args.push_back(evaluate_node(arg.get()));
        }
        return builtin_it->second(args);
    }
    
    // Look up user-defined function
    auto function = symbol_table_->get_function(node->function_name);
    if (!function) {
        throw std::runtime_error("Undefined function: " + node->function_name);
    }
    
    // Execute user-defined function
    return execute_user_function(function, node->arguments);
}

RuntimeValue AkaoRuntime::evaluate_assignment(AssignmentNode* node) {
    RuntimeValue value = evaluate_node(node->value.get());
    symbol_table_->set_value(node->variable_name, value);
    return value;
}

RuntimeValue AkaoRuntime::evaluate_conditional(ConditionalNode* node) {
    RuntimeValue condition = evaluate_node(node->condition.get());
    
    if (is_truthy(condition)) {
        return evaluate_node(node->true_branch.get());
    } else if (node->false_branch) {
        return evaluate_node(node->false_branch.get());
    }
    
    return RuntimeValue{};
}

RuntimeValue AkaoRuntime::evaluate_loop(LoopNode* node) {
    RuntimeValue last_value{};
    
    while (true) {
        RuntimeValue condition = evaluate_node(node->condition.get());
        if (!is_truthy(condition)) {
            break;
        }
        
        last_value = evaluate_node(node->body.get());
    }
    
    return last_value;
}

RuntimeValue AkaoRuntime::evaluate_block(BlockNode* node) {
    RuntimeValue last_value{};
    
    for (auto& statement : node->statements) {
        last_value = evaluate_node(statement.get());
    }
    
    return last_value;
}

void AkaoRuntime::initialize_builtin_symbols() {
    // Initialize basic constants
    symbol_table_->set_value("true", RuntimeValue{true});
    symbol_table_->set_value("false", RuntimeValue{false});
    symbol_table_->set_value("null", RuntimeValue{});
}

void AkaoRuntime::register_builtin_functions() {
    // Print function
    register_builtin_function("print", [this](const std::vector<RuntimeValue>& args) -> RuntimeValue {
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) execution_context_->output_buffer += " ";
            execution_context_->output_buffer += runtime_value_to_string(args[i]);
        }
        execution_context_->output_buffer += "\n";
        return RuntimeValue{};
    });
    
    // Type checking function
    register_builtin_function("type", [](const std::vector<RuntimeValue>& args) -> RuntimeValue {
        if (args.empty()) return RuntimeValue{"unknown"};
        return RuntimeValue{get_runtime_value_type(args[0])};
    });
    
    // Length function
    register_builtin_function("length", [](const std::vector<RuntimeValue>& args) -> RuntimeValue {
        if (args.empty()) return RuntimeValue{0};
        return RuntimeValue{get_runtime_value_length(args[0])};
    });
}

void AkaoRuntime::enable_debug_features() {
    execution_context_->debug_mode = true;
    execution_context_->trace_execution = true;
}

void AkaoRuntime::optimize_for_performance() {
    execution_context_->debug_mode = false;
    execution_context_->trace_execution = false;
    memory_manager_->enable_optimizations();
}

void AkaoRuntime::enable_test_features() {
    execution_context_->debug_mode = true;
    execution_context_->trace_execution = false;
    execution_context_->capture_output = true;
}

RuntimeValue AkaoRuntime::execute_user_function(UserFunction* function, 
                                                 const std::vector<std::unique_ptr<ASTNode>>& arguments) {
    // Create new scope for function execution
    symbol_table_->push_scope();
    
    try {
        // Bind arguments to parameters
        for (size_t i = 0; i < function->parameters.size() && i < arguments.size(); ++i) {
            RuntimeValue arg_value = evaluate_node(arguments[i].get());
            symbol_table_->set_value(function->parameters[i], arg_value);
        }
        
        // Execute function body
        RuntimeValue result = evaluate_node(function->body.get());
        
        // Restore previous scope
        symbol_table_->pop_scope();
        
        return result;
        
    } catch (...) {
        // Restore previous scope on exception
        symbol_table_->pop_scope();
        throw;
    }
}

bool AkaoRuntime::is_truthy(const RuntimeValue& value) {
    return value.is_true();
}

std::string AkaoRuntime::runtime_value_to_string(const RuntimeValue& value) {
    return value.to_string();
}

std::string AkaoRuntime::get_runtime_value_type(const RuntimeValue& value) {
    return value.get_type_name();
}

int AkaoRuntime::get_runtime_value_length(const RuntimeValue& value) {
    return value.get_length();
}

} // namespace runtime
} // namespace engine
} // namespace core
} // namespace akao
