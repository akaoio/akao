#pragma once

#include "pure_logic_engine.hpp"

namespace akao::logic::meta {

/**
 * @brief Meta-logic functions for validating logic expressions
 * 
 * These are PURE COMPUTATIONAL functions for meta-analysis of logic.
 * They contain NO domain logic - just computational primitives for
 * validating logic expressions and testing.
 */

class IsWellFormedFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "logic.is_well_formed"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::BOOLEAN; }
    std::string getDescription() const override { return "Checks if a logic expression is well-formed"; }
};

class AllFunctionsExistFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "logic.all_functions_exist"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::BOOLEAN; }
    std::string getDescription() const override { return "Checks if all referenced functions exist in the engine"; }
private:
    PureLogicEngine* engine_;
public:
    void setEngine(PureLogicEngine* engine) { engine_ = engine; }
};

class TestPassesFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "test.passes"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::BOOLEAN; }
    std::string getDescription() const override { return "Executes a test expression and returns whether it passes"; }
private:
    PureLogicEngine* engine_;
public:
    void setEngine(PureLogicEngine* engine) { engine_ = engine; }
};

class HasFieldFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "has_field"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::BOOLEAN; }
    std::string getDescription() const override { return "Checks if an object has a specific field"; }
};

class GetFieldFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "get_field"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::OBJECT; } // Can return any type
    std::string getDescription() const override { return "Gets the value of a field from an object"; }
};

/**
 * @brief Register all meta-logic functions
 */
void registerMetaFunctions(PureLogicEngine& engine);

} // namespace akao::logic::meta
