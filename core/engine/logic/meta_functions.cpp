#include "meta_functions.hpp"
#include <yaml-cpp/yaml.h>
#include <algorithm>

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
}

} // namespace akao::logic::meta
