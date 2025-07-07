#include "v1.hpp"
#include <memory>
#include <stdexcept>

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

Value MultiplyFunction::execute(const std::vector<Value>& args, Context& ctx) {
    validateArgs(args);
    return Value(args[0].asInteger() * args[1].asInteger());
}

std::vector<Value::Type> MultiplyFunction::getParameterTypes() const {
    return {Value::Type::INTEGER, Value::Type::INTEGER};
}

Value DivideFunction::execute(const std::vector<Value>& args, Context& ctx) {
    validateArgs(args);
    if (args[1].asInteger() == 0) {
        throw std::runtime_error("Division by zero");
    }
    return Value(args[0].asInteger() / args[1].asInteger());
}

std::vector<Value::Type> DivideFunction::getParameterTypes() const {
    return {Value::Type::INTEGER, Value::Type::INTEGER};
}

Value ModuloFunction::execute(const std::vector<Value>& args, Context& ctx) {
    validateArgs(args);
    if (args[1].asInteger() == 0) {
        throw std::runtime_error("Modulo by zero");
    }
    return Value(args[0].asInteger() % args[1].asInteger());
}

std::vector<Value::Type> ModuloFunction::getParameterTypes() const {
    return {Value::Type::INTEGER, Value::Type::INTEGER};
}

Value PowerFunction::execute(const std::vector<Value>& args, Context& ctx) {
    validateArgs(args);
    int base = args[0].asInteger();
    int exponent = args[1].asInteger();
    
    if (exponent < 0) {
        throw std::runtime_error("Negative exponents not supported in integer arithmetic");
    }
    
    int result = 1;
    for (int i = 0; i < exponent; ++i) {
        result *= base;
    }
    return Value(result);
}

std::vector<Value::Type> PowerFunction::getParameterTypes() const {
    return {Value::Type::INTEGER, Value::Type::INTEGER};
}

// =============================================================================
// Peano Arithmetic Functions
// =============================================================================

Value SuccessorFunction::execute(const std::vector<Value>& args, Context& ctx) {
    validateArgs(args);
    return Value(args[0].asInteger() + 1);
}

std::vector<Value::Type> SuccessorFunction::getParameterTypes() const {
    return {Value::Type::INTEGER};
}

Value IsZeroFunction::execute(const std::vector<Value>& args, Context& ctx) {
    validateArgs(args);
    return Value(args[0].asInteger() == 0);
}

std::vector<Value::Type> IsZeroFunction::getParameterTypes() const {
    return {Value::Type::INTEGER};
}

Value PredecessorFunction::execute(const std::vector<Value>& args, Context& ctx) {
    validateArgs(args);
    int n = args[0].asInteger();
    return Value(n > 0 ? n - 1 : 0);  // Predecessor of 0 is 0 in Peano arithmetic
}

std::vector<Value::Type> PredecessorFunction::getParameterTypes() const {
    return {Value::Type::INTEGER};
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
    engine.registerFunction("math.multiply", std::make_shared<MultiplyFunction>());
    engine.registerFunction("math.divide", std::make_shared<DivideFunction>());
    engine.registerFunction("math.modulo", std::make_shared<ModuloFunction>());
    engine.registerFunction("math.power", std::make_shared<PowerFunction>());
    
    // Peano arithmetic functions
    engine.registerFunction("peano.successor", std::make_shared<SuccessorFunction>());
    engine.registerFunction("peano.is_zero", std::make_shared<IsZeroFunction>());
    engine.registerFunction("peano.predecessor", std::make_shared<PredecessorFunction>());
    
    // String functions
    engine.registerFunction("string.length", std::make_shared<StringLengthFunction>());
    engine.registerFunction("string.concat", std::make_shared<StringConcatFunction>());
    
    // Test functions
    engine.registerFunction("test.mock_collection", std::make_shared<MockCollectionFunction>());
}

} // namespace akao::logic::builtin
