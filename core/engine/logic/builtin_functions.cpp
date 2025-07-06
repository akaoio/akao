#include "builtin_functions.hpp"
#include <memory>

namespace akao::logic::builtin {

// =============================================================================
// Collection Functions
// =============================================================================

Value CountFunction::execute(const std::vector<Value>& args, Context& ctx) {
    validateArgs(args);
    return Value(static_cast<int>(args[0].size()));
}

std::vector<Value::Type> CountFunction::getParameterTypes() const {
    return {Value::Type::COLLECTION};
}

Value ContainsFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 2) {
        throw std::runtime_error("collection.contains requires 2 arguments");
    }
    if (!args[0].isCollection()) {
        throw std::runtime_error("collection.contains first argument must be collection");
    }
    return Value(args[0].contains(args[1]));
}

std::vector<Value::Type> ContainsFunction::getParameterTypes() const {
    // Return empty to skip strict type checking for now (allow any type as second param)
    return {};
}

// =============================================================================
// Math Functions
// =============================================================================

Value AddFunction::execute(const std::vector<Value>& args, Context& ctx) {
    validateArgs(args);
    return Value(args[0].asInteger() + args[1].asInteger());
}

std::vector<Value::Type> AddFunction::getParameterTypes() const {
    return {Value::Type::INTEGER, Value::Type::INTEGER};
}

Value SubtractFunction::execute(const std::vector<Value>& args, Context& ctx) {
    validateArgs(args);
    return Value(args[0].asInteger() - args[1].asInteger());
}

std::vector<Value::Type> SubtractFunction::getParameterTypes() const {
    return {Value::Type::INTEGER, Value::Type::INTEGER};
}

// =============================================================================
// String Functions
// =============================================================================

Value StringLengthFunction::execute(const std::vector<Value>& args, Context& ctx) {
    validateArgs(args);
    return Value(static_cast<int>(args[0].asString().length()));
}

std::vector<Value::Type> StringLengthFunction::getParameterTypes() const {
    return {Value::Type::STRING};
}

Value StringConcatFunction::execute(const std::vector<Value>& args, Context& ctx) {
    validateArgs(args);
    return Value(args[0].asString() + args[1].asString());
}

std::vector<Value::Type> StringConcatFunction::getParameterTypes() const {
    return {Value::Type::STRING, Value::Type::STRING};
}

// =============================================================================
// Test Functions
// =============================================================================

Value MockCollectionFunction::execute(const std::vector<Value>& args, Context& ctx) {
    validateArgs(args);
    
    std::string type = args[0].asString();
    int count = args[1].asInteger();
    
    std::vector<Value> collection;
    
    if (type == "numbers") {
        for (int i = 1; i <= count; ++i) {
            collection.push_back(Value(i));
        }
    }
    else if (type == "strings") {
        for (int i = 1; i <= count; ++i) {
            collection.push_back(Value("item" + std::to_string(i)));
        }
    }
    else {
        throw std::runtime_error("Unknown mock collection type: " + type);
    }
    
    return Value(collection);
}

std::vector<Value::Type> MockCollectionFunction::getParameterTypes() const {
    return {Value::Type::STRING, Value::Type::INTEGER};
}

// =============================================================================
// Registration Function
// =============================================================================

void registerAllBuiltinFunctions(PureLogicEngine& engine) {
    // Collection functions
    engine.registerFunction("collection.count", std::make_shared<CountFunction>());
    engine.registerFunction("collection.contains", std::make_shared<ContainsFunction>());
    
    // Math functions
    engine.registerFunction("math.add", std::make_shared<AddFunction>());
    engine.registerFunction("math.subtract", std::make_shared<SubtractFunction>());
    
    // String functions
    engine.registerFunction("string.length", std::make_shared<StringLengthFunction>());
    engine.registerFunction("string.concat", std::make_shared<StringConcatFunction>());
    
    // Test functions
    engine.registerFunction("test.mock_collection", std::make_shared<MockCollectionFunction>());
}

} // namespace akao::logic::builtin
