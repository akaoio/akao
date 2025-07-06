#include "pure_logic_engine.hpp"
#include "builtin_functions.hpp"
#include "filesystem_functions.hpp"
#include "cpp_functions.hpp"
#include "meta_functions.hpp"
#include <fstream>
#include <stdexcept>
#include <iostream>

namespace akao::logic {

// =============================================================================
// PureLogicEngine Implementation
// =============================================================================

PureLogicEngine::PureLogicEngine() : caching_enabled_(true), tracing_enabled_(false), trace_depth_(0) {
    // Pure engine initialization - register computational primitives
    builtin::registerAllBuiltinFunctions(*this);
    filesystem::registerFilesystemFunctions(*this);
    cpp::registerCppFunctions(*this);
    meta::registerMetaFunctions(*this);
}

PureLogicEngine::~PureLogicEngine() = default;

Value PureLogicEngine::executeLogic(const YAML::Node& logic, Context& ctx) {
    if (!logic.IsDefined() || logic.IsNull()) {
        return Value(); // null value
    }
    
    // Add to trace if enabled
    if (tracing_enabled_) {
        std::string indent(trace_depth_ * 2, ' ');
        execution_trace_.push_back(indent + "EVAL: " + nodeToString(logic).substr(0, 100));
        trace_depth_++;
    }
    
    // Check cache if enabled (only for expressions without variables)
    std::string cache_key;
    if (caching_enabled_ && !containsVariables(logic)) {
        cache_key = nodeToString(logic);
        auto cache_it = expression_cache_.find(cache_key);
        if (cache_it != expression_cache_.end()) {
            metrics_.cache_hits++;
            if (tracing_enabled_) {
                std::string indent(trace_depth_ * 2, ' ');
                execution_trace_.push_back(indent + "CACHE HIT");
                trace_depth_--;
            }
            return cache_it->second;
        } else {
            metrics_.cache_misses++;
        }
    }
    
    Value result;
    
    try {
        // Detect expression type and delegate to appropriate handler
        if (isQuantifier(logic)) {
            result = executeQuantifier(logic, ctx);
        }
        else if (isOperator(logic)) {
            result = executeOperator(logic["operator"].as<std::string>(), {
                executeLogic(logic["left"], ctx),
                executeLogic(logic["right"], ctx)
            });
        }
        else if (isFunction(logic)) {
            std::vector<Value> args;
            if (logic["argument"].IsDefined()) {
                args.push_back(executeLogic(logic["argument"], ctx));
            }
            else if (logic["arguments"].IsDefined()) {
                for (const auto& arg : logic["arguments"]) {
                    args.push_back(executeLogic(arg, ctx));
                }
            }
            result = executeFunction(logic["function"].as<std::string>(), args);
        }
        else if (isConditional(logic)) {
            result = executeConditional(logic, ctx);
        }
        else if (logic["fixpoint"].IsDefined()) {
            result = executeFixpoint(logic, ctx);
        }
        else if (isVariable(logic)) {
            result = executeVariable(logic["var"].as<std::string>(), ctx);
        }
        else if (isLiteral(logic)) {
            result = executeLiteral(logic);
        }
        else {
            throwLogicError("Unknown logic expression type", logic);
        }
        
        // Cache the result if enabled and expression doesn't contain variables
        if (caching_enabled_ && !cache_key.empty() && !containsVariables(logic)) {
            expression_cache_[cache_key] = result;
        }
        
        // Add result to trace if enabled
        if (tracing_enabled_) {
            std::string indent(trace_depth_ * 2, ' ');
            std::string result_str = result.isNull() ? "null" : 
                                   (result.isBoolean() ? (result.asBoolean() ? "true" : "false") :
                                   (result.isInteger() ? std::to_string(result.asInteger()) :
                                   (result.isString() ? result.asString() : "complex")));
            execution_trace_.push_back(indent + "RESULT: " + result_str);
            trace_depth_--;
        }
        
        return result;
    }
    catch (const std::exception& e) {
        throwLogicError("Logic execution failed: " + std::string(e.what()), logic);
    }
    
    return Value(); // Should never reach here
}

Value PureLogicEngine::evaluate(const YAML::Node& logic, Context& ctx) {
    return executeLogic(logic, ctx);
}

Value PureLogicEngine::evaluate(const std::string& logic_str, Context& ctx) {
    try {
        YAML::Node logic = YAML::Load(logic_str);
        return executeLogic(logic, ctx);
    } catch (const YAML::Exception& e) {
        throw std::runtime_error("Failed to parse logic expression: " + std::string(e.what()));
    }
}

bool PureLogicEngine::evaluateCondition(const YAML::Node& condition, Context& ctx) {
    Value result = executeLogic(condition, ctx);
    if (!result.isBoolean()) {
        throwLogicError("Condition must evaluate to boolean", condition);
    }
    return result.asBoolean();
}

Value PureLogicEngine::executeFunction(const std::string& func, const std::vector<Value>& args) {
    auto it = builtin_functions_.find(func);
    if (it == builtin_functions_.end()) {
        throw std::runtime_error("Unknown function: " + func);
    }
    
    // Track metrics
    metrics_.function_calls++;
    
    Context dummy_ctx; // Functions don't need context for now
    return it->second->execute(args, dummy_ctx);
}

Value PureLogicEngine::executeOperator(const std::string& op, const std::vector<Value>& args) {
    // Pure logical and comparison operators - no domain knowledge
    if (op == "and") {
        if (args.size() != 2) throw std::runtime_error("'and' requires 2 arguments");
        return Value(args[0].asBoolean() && args[1].asBoolean());
    }
    else if (op == "or") {
        if (args.size() != 2) throw std::runtime_error("'or' requires 2 arguments");
        return Value(args[0].asBoolean() || args[1].asBoolean());
    }
    else if (op == "not") {
        if (args.size() != 1) throw std::runtime_error("'not' requires 1 argument");
        return Value(!args[0].asBoolean());
    }
    else if (op == "equals") {
        if (args.size() != 2) throw std::runtime_error("'equals' requires 2 arguments");
        return Value(args[0] == args[1]);
    }
    else if (op == "less_than") {
        if (args.size() != 2) throw std::runtime_error("'less_than' requires 2 arguments");
        return Value(args[0] < args[1]);
    }
    else if (op == "less_equal") {
        if (args.size() != 2) throw std::runtime_error("'less_equal' requires 2 arguments");
        return Value(args[0] <= args[1]);
    }
    else if (op == "greater_than") {
        if (args.size() != 2) throw std::runtime_error("'greater_than' requires 2 arguments");
        return Value(args[0] > args[1]);
    }
    else if (op == "greater_equal") {
        if (args.size() != 2) throw std::runtime_error("'greater_equal' requires 2 arguments");
        return Value(args[0] >= args[1]);
    }
    else {
        throw std::runtime_error("Unknown operator: " + op);
    }
}

void PureLogicEngine::loadBuiltinFunctions(const std::string& builtin_yaml_path) {
    try {
        // Load and validate the YAML structure
        YAML::Node builtin_config = YAML::LoadFile(builtin_yaml_path);
        
        if (!builtin_config["builtin_functions"].IsDefined()) {
            throw std::runtime_error("YAML file missing 'builtin_functions' section");
        }
        
        // Register all programmatic builtin functions
        builtin::registerAllBuiltinFunctions(*this);
        
        std::cout << "✅ Loaded builtin functions from: " << builtin_yaml_path << std::endl;
        std::cout << "   Registered " << builtin_functions_.size() << " functions" << std::endl;
        
        // Print registered functions for debugging
        for (const auto& [name, func] : builtin_functions_) {
            std::cout << "   - " << name << ": " << func->getDescription() << std::endl;
        }
        
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Failed to load builtin functions: " + std::string(e.what()));
    }
}

void PureLogicEngine::registerFunction(const std::string& name, std::shared_ptr<BuiltinFunction> func) {
    builtin_functions_[name] = func;
}

void PureLogicEngine::registerFunction(std::unique_ptr<BuiltinFunction> func) {
    std::string name = func->getName();
    builtin_functions_[name] = std::shared_ptr<BuiltinFunction>(func.release());
}

bool PureLogicEngine::hasFunction(const std::string& name) const {
    return builtin_functions_.find(name) != builtin_functions_.end();
}

std::vector<Value> PureLogicEngine::validateWithRules(const std::vector<std::string>& rule_files, Context& ctx) {
    std::vector<Value> violations;
    
    for (const auto& rule_file : rule_files) {
        try {
            YAML::Node rule = YAML::LoadFile(rule_file);
            if (rule["rule"] && rule["rule"]["logic"]) {
                Value result = executeLogic(rule["rule"]["logic"], ctx);
                // TODO: Process result and generate violations
                // This will be implemented in Phase 3
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Failed to validate rule " << rule_file << ": " << e.what() << std::endl;
        }
    }
    
    return violations;
}

bool PureLogicEngine::executePhilosophySelfProof(const std::string& philosophy_file) {
    try {
        YAML::Node philosophy = YAML::LoadFile(philosophy_file);
        if (philosophy["philosophy"] && philosophy["philosophy"]["self_proof"]) {
            Context ctx;
            
            // Bind the philosophy itself to context for self-reference
            ctx.bindVariable("philosophy", convertYamlToValue(philosophy["philosophy"]));
            ctx.bindVariable("formal_logic", convertYamlToValue(philosophy["philosophy"]["formal_logic"]));
            
            // Create mock context variables that philosophies might reference
            std::map<std::string, Value> file_organization;
            file_organization["one_class_per_file"] = Value(true);
            file_organization["consistent_naming"] = Value(true);
            ctx.bindVariable("file_organization", Value(file_organization));
            
            std::map<std::string, Value> code_structure;
            code_structure["clear_organization"] = Value(true);
            ctx.bindVariable("code_structure", Value(code_structure));
            
            std::map<std::string, Value> code_quality;
            code_quality["low_cognitive_load"] = Value(true);
            code_quality["high_maintainability"] = Value(true);
            ctx.bindVariable("code_quality", Value(code_quality));
            
            return evaluateCondition(philosophy["philosophy"]["self_proof"]["logic"], ctx);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to execute philosophy self-proof " << philosophy_file << ": " << e.what() << std::endl;
    }
    return false;
}

bool PureLogicEngine::executeRuleSelfValidation(const std::string& rule_file) {
    try {
        YAML::Node rule = YAML::LoadFile(rule_file);
        if (rule["rule"] && rule["rule"]["self_validation"]) {
            Context ctx;
            
            // Bind the rule itself to context for self-reference
            ctx.bindVariable("rule", convertYamlToValue(rule["rule"]));
            ctx.bindVariable("rule_logic", convertYamlToValue(rule["rule"]["logic"]));
            
            return evaluateCondition(rule["rule"]["self_validation"]["logic"], ctx);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to execute rule self-validation " << rule_file << ": " << e.what() << std::endl;
    }
    return false;
}

// Enhanced rule system methods
Value PureLogicEngine::executeRule(const std::string& rule_file, Context& ctx) {
    try {
        YAML::Node rule = YAML::LoadFile(rule_file);
        if (rule["rule"] && rule["rule"]["logic"]) {
            // Bind the rule itself to context for self-reference
            ctx.bindVariable("rule", convertYamlToValue(rule["rule"]));
            ctx.bindVariable("rule_logic", convertYamlToValue(rule["rule"]["logic"]));
            
            return executeLogic(rule["rule"]["logic"], ctx);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to execute rule " << rule_file << ": " << e.what() << std::endl;
    }
    return Value(false);
}

bool PureLogicEngine::executeRuleUnitTests(const std::string& rule_file) {
    try {
        YAML::Node rule = YAML::LoadFile(rule_file);
        if (rule["rule"] && rule["rule"]["unit_tests"]) {
            for (const auto& test : rule["rule"]["unit_tests"]) {
                Context testCtx;
                
                // Bind test data to context
                if (test["test_data"]) {
                    for (const auto& data : test["test_data"]) {
                        testCtx.bindVariable(data.first.as<std::string>(), 
                                           convertYamlToValue(data.second));
                    }
                }
                
                // Execute test logic
                Value result = executeLogic(test["test_logic"], testCtx);
                bool expected = test["expected_result"].as<bool>();
                
                if (result.isBoolean() && result.asBoolean() != expected) {
                    std::cerr << "Unit test failed: " << test["name"].as<std::string>() << std::endl;
                    return false;
                }
            }
            return true;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to execute rule unit tests " << rule_file << ": " << e.what() << std::endl;
    }
    return false;
}

Value PureLogicEngine::executePhilosophy(const std::string& philosophy_file, Context& ctx) {
    try {
        YAML::Node philosophy = YAML::LoadFile(philosophy_file);
        if (philosophy["philosophy"] && philosophy["philosophy"]["formal_logic"]) {
            // Bind the philosophy itself to context for self-reference
            ctx.bindVariable("philosophy", convertYamlToValue(philosophy["philosophy"]));
            ctx.bindVariable("formal_logic", convertYamlToValue(philosophy["philosophy"]["formal_logic"]));
            
            // Create mock context variables that philosophies might reference
            std::map<std::string, Value> file_organization;
            file_organization["one_class_per_file"] = Value(true);
            file_organization["consistent_naming"] = Value(true);
            ctx.bindVariable("file_organization", Value(file_organization));
            
            std::map<std::string, Value> code_structure;
            code_structure["clear_organization"] = Value(true);
            ctx.bindVariable("code_structure", Value(code_structure));
            
            std::map<std::string, Value> code_quality;
            code_quality["low_cognitive_load"] = Value(true);
            code_quality["high_maintainability"] = Value(true);
            ctx.bindVariable("code_quality", Value(code_quality));
            
            // Execute the conclusion logic
            if (philosophy["philosophy"]["formal_logic"]["conclusion"]) {
                return executeLogic(philosophy["philosophy"]["formal_logic"]["conclusion"]["logic"], ctx);
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to execute philosophy " << philosophy_file << ": " << e.what() << std::endl;
    }
    return Value(false);
}

std::vector<Value> PureLogicEngine::executeAllRuleTests(const std::string& rule_file) {
    std::vector<Value> results;
    try {
        YAML::Node rule = YAML::LoadFile(rule_file);
        if (rule["rule"] && rule["rule"]["unit_tests"]) {
            for (const auto& test : rule["rule"]["unit_tests"]) {
                Context testCtx;
                
                // Bind test data to context
                if (test["test_data"]) {
                    for (const auto& data : test["test_data"]) {
                        testCtx.bindVariable(data.first.as<std::string>(), 
                                           convertYamlToValue(data.second));
                    }
                }
                
                // Execute test logic and store result
                Value result = executeLogic(test["test_logic"], testCtx);
                results.push_back(result);
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to execute all rule tests " << rule_file << ": " << e.what() << std::endl;
    }
    return results;
}

// =============================================================================
// Private Implementation Methods
// =============================================================================

Value PureLogicEngine::executeQuantifier(const YAML::Node& quantifier, Context& ctx) {
    if (quantifier["forall"].IsDefined()) {
        // Universal quantification: ∀x ∈ domain : condition
        const auto& forall_node = quantifier["forall"];
        
        if (!forall_node["variable"].IsDefined() || 
            !forall_node["domain"].IsDefined() || 
            !forall_node["condition"].IsDefined()) {
            throwLogicError("'forall' requires 'variable', 'domain', and 'condition'", quantifier);
        }
        
        std::string var_name = forall_node["variable"].as<std::string>();
        
        // Evaluate domain to get collection
        Value domain_value = executeLogic(forall_node["domain"], ctx);
        if (!domain_value.isCollection()) {
            throwLogicError("'forall' domain must evaluate to a collection", forall_node["domain"]);
        }
        
        auto collection = domain_value.asCollection();
        
        // Create new scope for the quantified variable
        ctx.pushScope();
        
        // Check condition for each item in domain
        bool result = true;
        for (const auto& item : collection) {
            ctx.bindVariable(var_name, item);
            
            if (!evaluateCondition(forall_node["condition"], ctx)) {
                result = false;
                break; // Early termination for efficiency
            }
        }
        
        ctx.popScope();
        return Value(result);
    }
    else if (quantifier["exists"].IsDefined()) {
        // Existential quantification: ∃x ∈ domain : condition
        const auto& exists_node = quantifier["exists"];
        
        if (!exists_node["variable"].IsDefined() || 
            !exists_node["domain"].IsDefined() || 
            !exists_node["condition"].IsDefined()) {
            throwLogicError("'exists' requires 'variable', 'domain', and 'condition'", quantifier);
        }
        
        std::string var_name = exists_node["variable"].as<std::string>();
        
        // Evaluate domain to get collection
        Value domain_value = executeLogic(exists_node["domain"], ctx);
        if (!domain_value.isCollection()) {
            throwLogicError("'exists' domain must evaluate to a collection", exists_node["domain"]);
        }
        
        auto collection = domain_value.asCollection();
        
        // Create new scope for the quantified variable
        ctx.pushScope();
        
        // Check condition for each item in domain
        bool result = false;
        for (const auto& item : collection) {
            ctx.bindVariable(var_name, item);
            
            if (evaluateCondition(exists_node["condition"], ctx)) {
                result = true;
                break; // Early termination for efficiency
            }
        }
        
        ctx.popScope();
        return Value(result);
    }
    else {
        throwLogicError("Unknown quantifier type", quantifier);
    }
    
    return Value(); // Should never reach here
}

Value PureLogicEngine::executeConditional(const YAML::Node& conditional, Context& ctx) {
    if (!conditional["if"].IsDefined() || !conditional["then"].IsDefined()) {
        throwLogicError("Conditional requires 'if' and 'then' clauses", conditional);
    }
    
    // Evaluate the condition
    bool condition_result = evaluateCondition(conditional["if"], ctx);
    
    if (condition_result) {
        // Execute 'then' branch
        return executeLogic(conditional["then"], ctx);
    }
    else if (conditional["else"].IsDefined()) {
        // Execute 'else' branch if it exists
        return executeLogic(conditional["else"], ctx);
    }
    else {
        // No 'else' branch, return null/false for failed condition
        return Value();
    }
}

Value PureLogicEngine::executeFixpoint(const YAML::Node& fixpoint, Context& ctx) {
    // Enhanced fixpoint operator for complex recursion and μ-calculus
    // Supports multiple fixpoint algorithms and convergence strategies
    
    if (!fixpoint["fixpoint"].IsDefined()) {
        throwLogicError("Fixpoint node missing 'fixpoint' field", fixpoint);
    }
    
    const auto& fp = fixpoint["fixpoint"];
    
    if (!fp["variable"].IsDefined() || !fp["expression"].IsDefined()) {
        throwLogicError("Fixpoint needs 'variable' and 'expression' fields", fixpoint);
    }
    
    std::string var_name = fp["variable"].as<std::string>();
    const auto& expr = fp["expression"];
    
    // Enhanced configuration options
    int max_iterations = 1000;
    if (fp["max_iterations"].IsDefined()) {
        max_iterations = fp["max_iterations"].as<int>();
    }
    
    std::string strategy = "standard";
    if (fp["strategy"].IsDefined()) {
        strategy = fp["strategy"].as<std::string>();
    }
    
    // Enhanced convergence checking
    double tolerance = 0.0001;
    if (fp["tolerance"].IsDefined()) {
        tolerance = fp["tolerance"].as<double>();
    }
    
    // Start with initial value
    Value current_value;
    if (fp["initial"].IsDefined()) {
        current_value = executeLogic(fp["initial"], ctx);
    } else {
        // Default initial value based on strategy
        if (strategy == "least" || strategy == "mu") {
            current_value = Value(false); // Bottom element for least fixpoint
        } else if (strategy == "greatest" || strategy == "nu") {
            current_value = Value(true);  // Top element for greatest fixpoint
        } else {
            current_value = Value(); // Null for standard fixpoint
        }
    }
    
    // Track convergence history for oscillation detection
    std::vector<Value> history;
    
    // Enhanced iteration with multiple strategies
    for (int i = 0; i < max_iterations; ++i) {
        // Bind current value to the variable
        Context new_ctx(ctx);
        new_ctx.bindVariable(var_name, current_value);
        
        // Add iteration context for advanced recursion
        new_ctx.bindVariable("__iteration", Value(i));
        new_ctx.bindVariable("__strategy", Value(strategy));
        
        // Evaluate the expression with current binding
        Value next_value;
        try {
            next_value = executeLogic(expr, new_ctx);
        } catch (const std::exception& e) {
            // Enhanced error handling for recursive definitions
            if (i == 0) {
                throw; // Re-throw if first iteration fails
            } else {
                // For later iterations, might be convergence issue
                throwLogicError("Fixpoint evaluation failed at iteration " + std::to_string(i) + ": " + e.what(), fixpoint);
            }
        }
        
        // Enhanced convergence checking
        bool converged = false;
        
        if (strategy == "exact") {
            // Exact equality for discrete domains
            converged = valuesEqual(current_value, next_value);
        } else if (strategy == "numeric" && current_value.isInteger() && next_value.isInteger()) {
            // Numeric tolerance for continuous domains
            converged = std::abs(current_value.asInteger() - next_value.asInteger()) < tolerance;
        } else {
            // Standard equality check
            converged = valuesEqual(current_value, next_value);
        }
        
        if (converged) {
            // Add convergence info to context for debugging
            ctx.bindVariable("__converged_at", Value(i));
            return next_value;
        }
        
        // Oscillation detection for non-converging sequences
        if (history.size() >= 10) {
            // Check if we're oscillating between values
            bool oscillating = false;
            for (size_t j = 0; j < std::min(size_t(5), history.size()); ++j) {
                if (valuesEqual(next_value, history[history.size() - 1 - j * 2])) {
                    oscillating = true;
                    break;
                }
            }
            
            if (oscillating && strategy == "standard") {
                // Switch to averaging or return current value for oscillating sequences
                ctx.bindVariable("__oscillating", Value(true));
                return current_value;
            }
        }
        
        // Add to history and update current value
        history.push_back(current_value);
        current_value = next_value;
        
        // Limit history size to prevent memory issues
        if (history.size() > 100) {
            history.erase(history.begin());
        }
    }
    
    // Enhanced error reporting for non-convergence
    std::string error_msg = "Fixpoint did not converge within " + std::to_string(max_iterations) + 
                           " iterations using strategy '" + strategy + "'";
    
    if (!history.empty()) {
        error_msg += ". Last values: ";
        for (size_t i = std::max(0, int(history.size()) - 3); i < history.size(); ++i) {
            error_msg += history[i].toString() + " ";
        }
        error_msg += "-> " + current_value.toString();
    }
    
    throwLogicError(error_msg, fixpoint);
    return Value();
}

Value PureLogicEngine::executeLiteral(const YAML::Node& literal) {
    if (literal["literal"].IsDefined()) {
        const auto& lit = literal["literal"];
        if (lit.IsScalar()) {
            std::string str_val = lit.as<std::string>();
            
            // Try to parse as boolean
            if (str_val == "true") return Value(true);
            if (str_val == "false") return Value(false);
            
            // Try to parse as integer
            try {
                int int_val = std::stoi(str_val);
                return Value(int_val);
            } catch (...) {
                // Not an integer, treat as string
                return Value(str_val);
            }
        }
    }
    else if (literal.IsScalar()) {
        // Direct literal value
        std::string str_val = literal.as<std::string>();
        if (str_val == "true") return Value(true);
        if (str_val == "false") return Value(false);
        
        try {
            int int_val = std::stoi(str_val);
            return Value(int_val);
        } catch (...) {
            return Value(str_val);
        }
    }
    
    throwLogicError("Invalid literal format", literal);
    return Value();
}

Value PureLogicEngine::executeVariable(const std::string& var_name, Context& ctx) {
    if (!ctx.hasVariable(var_name)) {
        throw std::runtime_error("Undefined variable: " + var_name);
    }
    return ctx.getVariable(var_name);
}

// =============================================================================
// Expression Type Detection
// =============================================================================

bool PureLogicEngine::isQuantifier(const YAML::Node& node) {
    return node["forall"].IsDefined() || node["exists"].IsDefined();
}

bool PureLogicEngine::isOperator(const YAML::Node& node) {
    return node["operator"].IsDefined() && node["left"].IsDefined() && node["right"].IsDefined();
}

bool PureLogicEngine::isFunction(const YAML::Node& node) {
    return node["function"].IsDefined();
}

bool PureLogicEngine::isConditional(const YAML::Node& node) {
    return node["if"].IsDefined() && node["then"].IsDefined();
}

bool PureLogicEngine::isVariable(const YAML::Node& node) {
    return node["var"].IsDefined();
}

bool PureLogicEngine::isLiteral(const YAML::Node& node) {
    return node["literal"].IsDefined() || node.IsScalar();
}

void PureLogicEngine::throwLogicError(const std::string& message, const YAML::Node& context) {
    std::string error_msg = "Logic Error: " + message;
    if (context.IsDefined()) {
        error_msg += " at: " + YAML::Dump(context);
    }
    throw std::runtime_error(error_msg);
}

// =============================================================================
// Context Implementation
// =============================================================================

Context::Context() : parent_(nullptr) {
    scopes_.emplace_back(); // Root scope
}

Context::Context(const Context& parent) : parent_(&parent) {
    scopes_.emplace_back(); // New scope inheriting from parent
}

void Context::bindVariable(const std::string& name, const Value& value) {
    scopes_.back()[name] = value;
}

void Context::setValue(const std::string& name, const Value& value) {
    bindVariable(name, value); // Alias for convenience
}

Value Context::getVariable(const std::string& name) const {
    // Search from current scope up to root
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
        auto var_it = it->find(name);
        if (var_it != it->end()) {
            return var_it->second;
        }
    }
    
    // Check parent context if exists
    if (parent_) {
        return parent_->getVariable(name);
    }
    
    throw std::runtime_error("Variable not found: " + name);
}

bool Context::hasVariable(const std::string& name) const {
    // Search from current scope up to root
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
        if (it->find(name) != it->end()) {
            return true;
        }
    }
    
    // Check parent context if exists
    if (parent_) {
        return parent_->hasVariable(name);
    }
    
    return false;
}

void Context::pushScope() {
    scopes_.emplace_back();
}

void Context::popScope() {
    if (scopes_.size() > 1) {
        scopes_.pop_back();
    }
}

// =============================================================================
// Value Implementation  
// =============================================================================

Value::Value() : type_(Type::NULL_VALUE) {}

Value::Value(bool value) : type_(Type::BOOLEAN), data_(value) {}

Value::Value(int value) : type_(Type::INTEGER), data_(value) {}

Value::Value(const char* value) : type_(Type::STRING), data_(std::string(value)) {}

Value::Value(const std::string& value) : type_(Type::STRING), data_(value) {}

Value::Value(const std::vector<Value>& collection) : type_(Type::COLLECTION), data_(collection) {}

Value::Value(const std::map<std::string, Value>& object) : type_(Type::OBJECT), data_(object) {}

Value::Type Value::getType() const {
    return type_;
}

bool Value::isBoolean() const { return type_ == Type::BOOLEAN; }
bool Value::isBool() const { return isBoolean(); }
bool Value::isInteger() const { return type_ == Type::INTEGER; }
bool Value::isNumber() const { return isInteger(); }
bool Value::isString() const { return type_ == Type::STRING; }
bool Value::isCollection() const { return type_ == Type::COLLECTION; }
bool Value::isList() const { return isCollection(); }
bool Value::isObject() const { return type_ == Type::OBJECT; }
bool Value::isNull() const { return type_ == Type::NULL_VALUE; }

bool Value::asBoolean() const {
    if (!isBoolean()) throw std::runtime_error("Value is not boolean");
    return std::get<bool>(data_);
}

bool Value::asBool() const {
    return asBoolean();
}

int Value::asInteger() const {
    if (!isInteger()) throw std::runtime_error("Value is not integer");
    return std::get<int>(data_);
}

double Value::asNumber() const {
    if (!isInteger()) throw std::runtime_error("Value is not number");
    return static_cast<double>(std::get<int>(data_));
}

std::string Value::asString() const {
    if (!isString()) throw std::runtime_error("Value is not string");
    return std::get<std::string>(data_);
}

std::vector<Value> Value::asCollection() const {
    if (!isCollection()) throw std::runtime_error("Value is not collection");
    return std::get<std::vector<Value>>(data_);
}

std::vector<Value> Value::asList() const {
    return asCollection();
}

std::map<std::string, Value> Value::asObject() const {
    if (!isObject()) throw std::runtime_error("Value is not object");
    return std::get<std::map<std::string, Value>>(data_);
}

std::string Value::toString() const {
    switch (type_) {
        case Type::NULL_VALUE:
            return "null";
        case Type::BOOLEAN:
            return asBoolean() ? "true" : "false";
        case Type::INTEGER:
            return std::to_string(asInteger());
        case Type::STRING:
            return asString();
        case Type::COLLECTION:
            {
                std::string result = "[";
                auto coll = asCollection();
                for (size_t i = 0; i < coll.size(); ++i) {
                    if (i > 0) result += ", ";
                    result += coll[i].toString();
                }
                result += "]";
                return result;
            }
        case Type::OBJECT:
            {
                std::string result = "{";
                auto obj = asObject();
                bool first = true;
                for (const auto& pair : obj) {
                    if (!first) result += ", ";
                    result += "\"" + pair.first + "\": " + pair.second.toString();
                    first = false;
                }
                result += "}";
                return result;
            }
    }
    return "unknown";
}

size_t Value::size() const {
    if (isCollection()) {
        return asCollection().size();
    }
    else if (isObject()) {
        return asObject().size();
    }
    else if (isString()) {
        return asString().size();
    }
    return 0;
}

Value Value::at(size_t index) const {
    if (!isCollection()) throw std::runtime_error("Value is not collection");
    const auto& coll = asCollection();
    if (index >= coll.size()) throw std::runtime_error("Index out of bounds");
    return coll[index];
}

Value Value::get(const std::string& key) const {
    if (!isObject()) throw std::runtime_error("Value is not object");
    const auto& obj = asObject();
    auto it = obj.find(key);
    if (it == obj.end()) return Value(); // null
    return it->second;
}

bool Value::contains(const Value& item) const {
    if (isCollection()) {
        const auto& coll = asCollection();
        return std::find(coll.begin(), coll.end(), item) != coll.end();
    }
    else if (isString() && item.isString()) {
        return asString().find(item.asString()) != std::string::npos;
    }
    return false;
}

bool Value::operator==(const Value& other) const {
    if (type_ != other.type_) return false;
    return data_ == other.data_;
}

bool Value::operator!=(const Value& other) const {
    return !(*this == other);
}

bool Value::operator<(const Value& other) const {
    if (type_ != other.type_) return false;
    
    switch (type_) {
        case Type::INTEGER:
            return asInteger() < other.asInteger();
        case Type::STRING:
            return asString() < other.asString();
        default:
            throw std::runtime_error("Comparison not supported for this type");
    }
}

bool Value::operator<=(const Value& other) const {
    return *this == other || *this < other;
}

bool Value::operator>(const Value& other) const {
    return !(*this <= other);
}

bool Value::operator>=(const Value& other) const {
    return !(*this < other);
}

// =============================================================================
// BuiltinFunction Implementation
// =============================================================================

void BuiltinFunction::validateArgs(const std::vector<Value>& args) const {
    const auto& expected_types = getParameterTypes();
    if (args.size() != expected_types.size()) {
        throw std::runtime_error("Function " + getName() + " expects " + 
                                std::to_string(expected_types.size()) + " arguments, got " + 
                                std::to_string(args.size()));
    }
    
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i].getType() != expected_types[i]) {
            throw std::runtime_error("Function " + getName() + " argument " + 
                                    std::to_string(i) + " type mismatch");
        }
    }
}

// =============================================================================
// YAML to Value Conversion
// =============================================================================

Value PureLogicEngine::convertYamlToValue(const YAML::Node& node) {
    if (!node.IsDefined() || node.IsNull()) {
        return Value(); // null value
    }
    
    if (node.IsScalar()) {
        std::string str = node.as<std::string>();
        
        // Try to convert to appropriate types
        if (str == "true" || str == "false") {
            return Value(str == "true");
        }
        
        // Try integer conversion
        try {
            if (str.find('.') == std::string::npos) {
                int intVal = std::stoi(str);
                return Value(intVal);
            }
        } catch (...) {
            // Not an integer, continue
        }
        
        // Try double conversion (convert to integer if no decimal part)
        try {
            double doubleVal = std::stod(str);
            if (doubleVal == std::floor(doubleVal)) {
                return Value(static_cast<int>(doubleVal));
            }
            // For now, treat decimal numbers as strings since Value doesn't support doubles
            return Value(str);
        } catch (...) {
            // Not a double, treat as string
        }
        
        return Value(str);
    }
    
    if (node.IsSequence()) {
        std::vector<Value> vec;
        for (const auto& item : node) {
            vec.push_back(convertYamlToValue(item));
        }
        return Value(vec);
    }
    
    if (node.IsMap()) {
        std::map<std::string, Value> map;
        for (const auto& pair : node) {
            map[pair.first.as<std::string>()] = convertYamlToValue(pair.second);
        }
        return Value(map);
    }
    
    return Value(); // fallback to null
}

// Performance and debugging methods
void PureLogicEngine::enableCaching(bool enable) {
    caching_enabled_ = enable;
    if (!enable) {
        clearCache();
    }
}

void PureLogicEngine::enableTracing(bool enable) {
    tracing_enabled_ = enable;
    if (!enable) {
        execution_trace_.clear();
        trace_depth_ = 0;
    }
}

void PureLogicEngine::printTrace() const {
    std::cout << "=== Execution Trace ===" << std::endl;
    for (const auto& entry : execution_trace_) {
        std::cout << entry << std::endl;
    }
    std::cout << "======================" << std::endl;
}

// Performance optimization methods
std::string PureLogicEngine::nodeToString(const YAML::Node& node) {
    try {
        return YAML::Dump(node);
    } catch (...) {
        return "cache_key_" + std::to_string(reinterpret_cast<uintptr_t>(&node));
    }
}

void PureLogicEngine::clearCache() {
    expression_cache_.clear();
}

// Helper method to detect if expression contains variables
bool PureLogicEngine::containsVariables(const YAML::Node& node) {
    if (!node.IsDefined() || node.IsNull()) {
        return false;
    }
    
    // Direct variable reference
    if (node.IsMap() && node["var"].IsDefined()) {
        return true;
    }
    
    // Check recursively in compound expressions
    if (node.IsMap()) {
        for (const auto& pair : node) {
            if (containsVariables(pair.second)) {
                return true;
            }
        }
    } else if (node.IsSequence()) {
        for (const auto& item : node) {
            if (containsVariables(item)) {
                return true;
            }
        }
    }
    
    return false;
}

// Helper method for fixpoint convergence checking
bool PureLogicEngine::valuesEqual(const Value& a, const Value& b) {
    if (a.getType() != b.getType()) {
        return false;
    }
    
    switch (a.getType()) {
        case Value::Type::NULL_VALUE:
            return true;
        case Value::Type::BOOLEAN:
            return a.asBoolean() == b.asBoolean();
        case Value::Type::INTEGER:
            return a.asInteger() == b.asInteger();
        case Value::Type::STRING:
            return a.asString() == b.asString();
        case Value::Type::COLLECTION:
            {
                auto a_coll = a.asCollection();
                auto b_coll = b.asCollection();
                if (a_coll.size() != b_coll.size()) return false;
                for (size_t i = 0; i < a_coll.size(); ++i) {
                    if (!valuesEqual(a_coll[i], b_coll[i])) return false;
                }
                return true;
            }
        case Value::Type::OBJECT:
            {
                auto a_obj = a.asObject();
                auto b_obj = b.asObject();
                if (a_obj.size() != b_obj.size()) return false;
                for (const auto& pair : a_obj) {
                    auto it = b_obj.find(pair.first);
                    if (it == b_obj.end() || !valuesEqual(pair.second, it->second)) {
                        return false;
                    }
                }
                return true;
            }
    }
    return false;
}

void PureLogicEngine::initialize() {
    // Load built-in functions from YAML
    try {
        loadBuiltinFunctions("core/engine/builtin_functions.yaml");
    } catch (const std::exception& e) {
        // If YAML file doesn't exist, functions are already registered in constructor
        // This is not an error - just means we're using the hardcoded registration
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

} // namespace akao::logic
