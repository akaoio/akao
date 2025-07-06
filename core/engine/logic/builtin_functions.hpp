#pragma once

#include "pure_logic_engine.hpp"

namespace akao::logic::builtin {

/**
 * @brief Collection operation functions
 */
class CountFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "collection.count"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::INTEGER; }
    std::string getDescription() const override { return "Returns the number of items in a collection"; }
};

class ContainsFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "collection.contains"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::BOOLEAN; }
    std::string getDescription() const override { return "Checks if collection contains item"; }
};

/**
 * @brief Math operation functions  
 */
class AddFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "math.add"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::INTEGER; }
    std::string getDescription() const override { return "Adds two numbers"; }
};

class SubtractFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "math.subtract"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::INTEGER; }
    std::string getDescription() const override { return "Subtracts second number from first"; }
};

/**
 * @brief String operation functions
 */
class StringLengthFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "string.length"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::INTEGER; }
    std::string getDescription() const override { return "Returns the length of a string"; }
};

class StringConcatFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "string.concat"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::STRING; }
    std::string getDescription() const override { return "Concatenates two strings"; }
};

/**
 * @brief Test operation functions
 */
class MockCollectionFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "test.mock_collection"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::COLLECTION; }
    std::string getDescription() const override { return "Creates a mock collection for testing"; }
};

/**
 * @brief Factory function to create all builtin functions
 */
void registerAllBuiltinFunctions(PureLogicEngine& engine);

} // namespace akao::logic::builtin
