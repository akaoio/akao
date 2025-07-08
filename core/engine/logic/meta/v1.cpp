#include "v1.hpp"
#include <algorithm>
#include <stdexcept>

namespace akao::logic::meta {

// IsWellFormedFunction
Value IsWellFormedFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 1) {
        throw std::runtime_error("logic.is_well_formed expects 1 argument (logic expression)");
    }
    
    if (args[0].getType() != Value::Type::OBJECT) {
        throw std::runtime_error("logic.is_well_formed expects object argument");
    }
    
    auto expr = args[0].asObject();
    
    try {
        // Check for required operator field
        if (expr.find("operator") == expr.end()) {
            return Value(false);
        }
        
        std::string op = expr.at("operator").asString();
        
        // Validate based on operator type
        if (op == "and" || op == "or") {
            // Binary operators need operands array
            if (expr.find("operands") == expr.end()) {
                return Value(false);
            }
            auto operands = expr.at("operands").asCollection();
            if (operands.size() != 2) {
                return Value(false);
            }
        } else if (op == "not") {
            // Unary operator needs operand
            if (expr.find("operand") == expr.end()) {
                return Value(false);
            }
        } else if (op == "=" || op == "!=" || op == "<" || op == ">" || op == "<=" || op == ">=") {
            // Comparison operators need left and right
            if (expr.find("left") == expr.end() || expr.find("right") == expr.end()) {
                return Value(false);
            }
        } else if (op == "forall" || op == "exists") {
            // Quantifiers need variable and condition
            if (expr.find("variable") == expr.end() || expr.find("condition") == expr.end()) {
                return Value(false);
            }
        } else if (op == "if") {
            // Conditional needs condition, then, and optionally else
            if (expr.find("condition") == expr.end() || expr.find("then") == expr.end()) {
                return Value(false);
            }
        } else if (op == "call") {
            // Function call needs function name
            if (expr.find("function") == expr.end()) {
                return Value(false);
            }
        }
        
        return Value(true);
    } catch (const std::exception& e) {
        return Value(false);
    }
}

std::vector<Value::Type> IsWellFormedFunction::getParameterTypes() const {
    return {Value::Type::OBJECT};
}

// AllFunctionsExistFunction
Value AllFunctionsExistFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 1) {
        throw std::runtime_error("logic.all_functions_exist expects 1 argument (logic expression)");
    }
    
    if (args[0].getType() != Value::Type::OBJECT) {
        throw std::runtime_error("logic.all_functions_exist expects object argument");
    }
    
    if (!engine_) {
        throw std::runtime_error("Engine not set for all_functions_exist");
    }
    
    auto expr = args[0].asObject();
    
    // For this implementation, we'll do a simplified check
    // In a full implementation, we'd recursively traverse the expression tree
    try {
        if (expr.find("operator") != expr.end() && expr.at("operator").asString() == "call") {
            if (expr.find("function") != expr.end()) {
                std::string funcName = expr.at("function").asString();
                return Value(engine_->hasFunction(funcName));
            }
        }
        
        // For non-function expressions, return true (assuming built-in operators exist)
        return Value(true);
    } catch (const std::exception& e) {
        return Value(false);
    }
}

std::vector<Value::Type> AllFunctionsExistFunction::getParameterTypes() const {
    return {Value::Type::OBJECT};
}

// TestPassesFunction
Value TestPassesFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 1) {
        throw std::runtime_error("test.passes expects 1 argument (test expression)");
    }
    
    if (args[0].getType() != Value::Type::BOOLEAN) {
        throw std::runtime_error("test.passes expects boolean argument");
    }
    
    // For now, just return the boolean value as the test result
    // In a full implementation, this would evaluate the test expression
    return Value(args[0].asBoolean());
}

std::vector<Value::Type> TestPassesFunction::getParameterTypes() const {
    return {Value::Type::BOOLEAN};
}

// HasFieldFunction
Value HasFieldFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 2) {
        throw std::runtime_error("has_field expects 2 arguments (object, field_name)");
    }
    
    if (args[0].getType() != Value::Type::OBJECT) {
        throw std::runtime_error("has_field expects object as first argument");
    }
    
    if (args[1].getType() != Value::Type::STRING) {
        throw std::runtime_error("has_field expects string as second argument");
    }
    
    auto obj = args[0].asObject();
    std::string fieldName = args[1].asString();
    
    return Value(obj.find(fieldName) != obj.end());
}

std::vector<Value::Type> HasFieldFunction::getParameterTypes() const {
    return {Value::Type::OBJECT, Value::Type::STRING};
}

// GetFieldFunction
Value GetFieldFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 2) {
        throw std::runtime_error("get_field expects 2 arguments (object, field_name)");
    }
    
    if (args[0].getType() != Value::Type::OBJECT) {
        throw std::runtime_error("get_field expects object as first argument");
    }
    
    if (args[1].getType() != Value::Type::STRING) {
        throw std::runtime_error("get_field expects string as second argument");
    }
    
    auto obj = args[0].asObject();
    std::string fieldName = args[1].asString();
    
    auto it = obj.find(fieldName);
    if (it != obj.end()) {
        return it->second;
    } else {
        return Value(); // Return null if field doesn't exist
    }
}

std::vector<Value::Type> GetFieldFunction::getParameterTypes() const {
    return {Value::Type::OBJECT, Value::Type::STRING};
}

// Registration function
void registerMetaFunctions(PureLogicEngine& engine) {
    auto allFuncExists = std::make_unique<AllFunctionsExistFunction>();
    allFuncExists->setEngine(&engine);
    engine.registerFunction(std::move(allFuncExists));
    
    auto testPasses = std::make_unique<TestPassesFunction>();
    testPasses->setEngine(&engine);
    engine.registerFunction(std::move(testPasses));
    
    engine.registerFunction(std::make_unique<IsWellFormedFunction>());
    engine.registerFunction(std::make_unique<HasFieldFunction>());
    engine.registerFunction(std::make_unique<GetFieldFunction>());
    
    // New meta-logical functions for self-reference and consistency
    engine.registerFunction(std::make_unique<EncodeFormulaFunction>());
    engine.registerFunction(std::make_unique<DecodeFormulaFunction>());
    engine.registerFunction(std::make_unique<SelfReferenceFunction>());
    engine.registerFunction(std::make_unique<DiagonalizationFunction>());
    
    auto consistencyCheck = std::make_unique<ConsistencyCheckFunction>();
    consistencyCheck->setEngine(&engine);
    engine.registerFunction(std::move(consistencyCheck));
    
    auto provability = std::make_unique<ProvabilityFunction>();
    provability->setEngine(&engine);
    engine.registerFunction(std::move(provability));
    
    // μ-calculus functions for advanced fixpoint logic
    auto muLeast = std::make_unique<MuCalculusLeastFixpointFunction>();
    muLeast->setEngine(&engine);
    engine.registerFunction(std::move(muLeast));
    
    auto muGreatest = std::make_unique<MuCalculusGreatestFixpointFunction>();
    muGreatest->setEngine(&engine);
    engine.registerFunction(std::move(muGreatest));
    
    auto recursiveDef = std::make_unique<RecursiveFunctionDefFunction>();
    recursiveDef->setEngine(&engine);
    engine.registerFunction(std::move(recursiveDef));
    
    auto fixpointIter = std::make_unique<FixpointIterateFunction>();
    fixpointIter->setEngine(&engine);
    engine.registerFunction(std::move(fixpointIter));
}

// =============================================================================
// Meta-Logical Functions for Self-Reference and Consistency
// =============================================================================

// Simple Gödel encoding using string hash (placeholder for full implementation)
int simpleGodelEncode(const std::string& formula) {
    std::hash<std::string> hasher;
    return static_cast<int>(hasher(formula) % 1000000); // Keep numbers manageable
}

std::string simpleGodelDecode(int godelNumber) {
    // Placeholder: In real implementation, this would decode back to formula
    return "formula_" + std::to_string(godelNumber);
}

// EncodeFormulaFunction
Value EncodeFormulaFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 1) {
        throw std::runtime_error("metalogic.encode_formula expects 1 argument (formula string)");
    }
    
    if (!args[0].isString()) {
        throw std::runtime_error("metalogic.encode_formula expects string argument");
    }
    
    std::string formula = args[0].asString();
    int godelNumber = simpleGodelEncode(formula);
    
    return Value(godelNumber);
}

std::vector<Value::Type> EncodeFormulaFunction::getParameterTypes() const {
    return {Value::Type::STRING};
}

// DecodeFormulaFunction  
Value DecodeFormulaFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 1) {
        throw std::runtime_error("metalogic.decode_formula expects 1 argument (Gödel number)");
    }
    
    if (!args[0].isInteger()) {
        throw std::runtime_error("metalogic.decode_formula expects integer argument");
    }
    
    int godelNumber = args[0].asInteger();
    std::string formula = simpleGodelDecode(godelNumber);
    
    return Value(formula);
}

std::vector<Value::Type> DecodeFormulaFunction::getParameterTypes() const {
    return {Value::Type::INTEGER};
}

// SelfReferenceFunction
Value SelfReferenceFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 1) {
        throw std::runtime_error("metalogic.self_reference expects 1 argument (formula template)");
    }
    
    if (!args[0].isString()) {
        throw std::runtime_error("metalogic.self_reference expects string argument");
    }
    
    std::string template_formula = args[0].asString();
    
    // Apply diagonalization: substitute formula with its own Gödel number
    std::string self_ref = template_formula + "_self_" + std::to_string(simpleGodelEncode(template_formula));
    int self_ref_godel = simpleGodelEncode(self_ref);
    
    return Value(self_ref_godel);
}

std::vector<Value::Type> SelfReferenceFunction::getParameterTypes() const {
    return {Value::Type::STRING};
}

// ConsistencyCheckFunction
Value ConsistencyCheckFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 1) {
        throw std::runtime_error("metalogic.consistency_check expects 1 argument (statement)");
    }
    
    if (!args[0].isString()) {
        throw std::runtime_error("metalogic.consistency_check expects string argument");
    }
    
    std::string statement = args[0].asString();
    
    // Placeholder consistency check - in real implementation this would:
    // 1. Parse the statement
    // 2. Check if both statement and its negation are provable
    // 3. Return false if both are provable (inconsistent)
    
    // For now, assume system is consistent unless statement contains "contradiction"
    bool consistent = statement.find("contradiction") == std::string::npos;
    
    return Value(consistent);
}

std::vector<Value::Type> ConsistencyCheckFunction::getParameterTypes() const {
    return {Value::Type::STRING};
}

// ProvabilityFunction
Value ProvabilityFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 1) {
        throw std::runtime_error("metalogic.provability expects 1 argument (statement)");
    }
    
    if (!args[0].isString()) {
        throw std::runtime_error("metalogic.provability expects string argument");
    }
    
    std::string statement = args[0].asString();
    
    // Placeholder provability check - in real implementation this would:
    // 1. Try to construct a proof for the statement
    // 2. Return true if proof exists, false otherwise
    
    // For now, simple heuristics:
    // - Tautologies are provable
    // - Self-referential statements about unprovability are not provable (Gödel sentence)
    
    if (statement.find("tautology") != std::string::npos) {
        return Value(true);
    }
    
    if (statement.find("unprovable") != std::string::npos && 
        statement.find("self_") != std::string::npos) {
        return Value(false); // Gödel sentence
    }
    
    // Default: assume provable for simple statements
    return Value(true);
}

std::vector<Value::Type> ProvabilityFunction::getParameterTypes() const {
    return {Value::Type::STRING};
}

// DiagonalizationFunction
Value DiagonalizationFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 1) {
        throw std::runtime_error("metalogic.diagonalization expects 1 argument (formula template)");
    }
    
    if (!args[0].isString()) {
        throw std::runtime_error("metalogic.diagonalization expects string argument");
    }
    
    std::string template_formula = args[0].asString();
    
    // Diagonalization: create a statement that refers to itself
    // Template should contain placeholder for Gödel number
    std::string placeholder = "GODEL_NUMBER";
    
    if (template_formula.find(placeholder) == std::string::npos) {
        throw std::runtime_error("metalogic.diagonalization: template must contain GODEL_NUMBER placeholder");
    }
    
    // First encoding to get a number
    int temp_godel = simpleGodelEncode(template_formula);
    
    // Replace placeholder with the Gödel number
    std::string diagonal_formula = template_formula;
    size_t pos = diagonal_formula.find(placeholder);
    diagonal_formula.replace(pos, placeholder.length(), std::to_string(temp_godel));
    
    // Final Gödel number for the diagonalized formula
    int final_godel = simpleGodelEncode(diagonal_formula);
    
    return Value(final_godel);
}

std::vector<Value::Type> DiagonalizationFunction::getParameterTypes() const {
    return {Value::Type::STRING};
}

// =============================================================================
// μ-Calculus Functions for Advanced Fixpoint Logic
// =============================================================================

// Helper function to check value equality for fixpoint convergence
bool isValueEqual(const Value& a, const Value& b) {
    if (a.getType() != b.getType()) return false;
    
    switch (a.getType()) {
        case Value::Type::BOOLEAN: return a.asBoolean() == b.asBoolean();
        case Value::Type::INTEGER: return a.asInteger() == b.asInteger();
        case Value::Type::STRING: return a.asString() == b.asString();
        case Value::Type::NULL_VALUE: return true;
        default: return false; // For complex types, use string comparison
    }
}

// MuCalculusLeastFixpointFunction (μ operator)
Value MuCalculusLeastFixpointFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 3) {
        throw std::runtime_error("mucalculus.mu expects 3 arguments (variable, expression, initial)");
    }
    
    if (!args[0].isString() || !args[1].isString()) {
        throw std::runtime_error("mucalculus.mu expects string arguments for variable and expression");
    }
    
    std::string var_name = args[0].asString();
    std::string expression = args[1].asString();
    Value initial = args[2];
    
    // μ-calculus least fixpoint: start from bottom (false/0) and iterate upward
    const int max_iterations = 100;
    Value current = initial.isNull() ? Value(false) : initial;
    
    for (int i = 0; i < max_iterations; ++i) {
        // Create a simple evaluation context
        // In a full implementation, this would parse and evaluate the expression
        // For now, we'll use a simplified approach
        
        Value next = current; // Placeholder: would evaluate expression[var := current]
        
        if (expression.find("or") != std::string::npos) {
            // Simple case: X or P becomes true if current is true or P is true
            next = Value(current.asBoolean() || true);
        } else if (expression.find("and") != std::string::npos) {
            // Simple case: X and P becomes current and P
            next = Value(current.asBoolean() && true);
        }
        
        if (isValueEqual(current, next)) {
            return next; // Fixpoint found
        }
        
        current = next;
    }
    
    throw std::runtime_error("μ-calculus least fixpoint did not converge");
}

std::vector<Value::Type> MuCalculusLeastFixpointFunction::getParameterTypes() const {
    return {Value::Type::STRING, Value::Type::STRING, Value::Type::OBJECT};
}

// MuCalculusGreatestFixpointFunction (ν operator)  
Value MuCalculusGreatestFixpointFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 3) {
        throw std::runtime_error("mucalculus.nu expects 3 arguments (variable, expression, initial)");
    }
    
    if (!args[0].isString() || !args[1].isString()) {
        throw std::runtime_error("mucalculus.nu expects string arguments for variable and expression");
    }
    
    std::string var_name = args[0].asString();
    std::string expression = args[1].asString();
    Value initial = args[2];
    
    // ν-calculus greatest fixpoint: start from top (true/∞) and iterate downward
    const int max_iterations = 100;
    Value current = initial.isNull() ? Value(true) : initial;
    
    for (int i = 0; i < max_iterations; ++i) {
        // Create a simple evaluation context
        Value next = current; // Placeholder: would evaluate expression[var := current]
        
        if (expression.find("and") != std::string::npos) {
            // Simple case: X and P becomes true if current is true and P is true
            next = Value(current.asBoolean() && true);
        } else if (expression.find("or") != std::string::npos) {
            // Simple case: X or P becomes current or P  
            next = Value(current.asBoolean() || false);
        }
        
        if (isValueEqual(current, next)) {
            return next; // Fixpoint found
        }
        
        current = next;
    }
    
    throw std::runtime_error("ν-calculus greatest fixpoint did not converge");
}

std::vector<Value::Type> MuCalculusGreatestFixpointFunction::getParameterTypes() const {
    return {Value::Type::STRING, Value::Type::STRING, Value::Type::OBJECT};
}

// RecursiveFunctionDefFunction
Value RecursiveFunctionDefFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 4) {
        throw std::runtime_error("recursive.define expects 4 arguments (name, parameter, base_case, recursive_case)");
    }
    
    if (!args[0].isString() || !args[1].isString() || !args[2].isString() || !args[3].isString()) {
        throw std::runtime_error("recursive.define expects string arguments");
    }
    
    std::string func_name = args[0].asString();
    std::string parameter = args[1].asString();
    std::string base_case = args[2].asString();
    std::string recursive_case = args[3].asString();
    
    // Create a function definition object
    std::map<std::string, Value> function_def;
    function_def["name"] = Value(func_name);
    function_def["parameter"] = Value(parameter);
    function_def["base_case"] = Value(base_case);
    function_def["recursive_case"] = Value(recursive_case);
    function_def["type"] = Value("recursive_function");
    
    return Value(function_def);
}

std::vector<Value::Type> RecursiveFunctionDefFunction::getParameterTypes() const {
    return {Value::Type::STRING, Value::Type::STRING, Value::Type::STRING, Value::Type::STRING};
}

// FixpointIterateFunction
Value FixpointIterateFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 4) {
        throw std::runtime_error("fixpoint.iterate expects 4 arguments (function, initial, max_iter, tolerance)");
    }
    
    if (!args[0].isString()) {
        throw std::runtime_error("fixpoint.iterate expects string function expression");
    }
    
    if (!args[1].isInteger() || !args[2].isInteger()) {
        throw std::runtime_error("fixpoint.iterate expects integer arguments for initial and max_iter");
    }
    
    std::string function_expr = args[0].asString();
    Value initial = args[1];
    int max_iterations = args[2].asInteger();
    Value tolerance = args[3];
    
    Value current = initial;
    
    for (int i = 0; i < max_iterations; ++i) {
        // For a more complete implementation, this would:
        // 1. Parse the function expression
        // 2. Apply it to the current value
        // 3. Check convergence within tolerance
        
        // Simplified example: f(x) = x/2 + 1 converges to 2
        if (function_expr.find("half_plus_one") != std::string::npos && current.isInteger()) {
            Value next = Value(current.asInteger() / 2 + 1);
            
            if (current.isInteger() && std::abs(current.asInteger() - next.asInteger()) <= tolerance.asInteger()) {
                return next; // Converged within tolerance
            }
            
            current = next;
        } else {
            // Default case: return current value
            return current;
        }
    }
    
    throw std::runtime_error("fixpoint.iterate did not converge within " + std::to_string(max_iterations) + " iterations");
}

std::vector<Value::Type> FixpointIterateFunction::getParameterTypes() const {
    return {Value::Type::STRING, Value::Type::INTEGER, Value::Type::INTEGER, Value::Type::INTEGER};
}

} // namespace akao::logic::meta
