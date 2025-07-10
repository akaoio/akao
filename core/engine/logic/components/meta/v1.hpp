/**
 * @id: akao:file:core:engine:logic_meta:v1
 * @doc: Meta-logic framework header providing advanced logical analysis capabilities including well-formedness checking, μ-calculus fixpoint operators, self-referential logic, Gödel encoding, diagonalization, and consistency verification. Implements computational primitives for meta-analysis of logic expressions, recursive function definitions, and provability predicates essential for formal system analysis and incompleteness theorem demonstrations.
 * @specification: Header for meta-logic framework with fixpoint operators, self-reference, and consistency checking
 * @scope: Core engine meta-logic processing framework
 * @timeline: 2025-07-09
 * @rationale: Provide advanced meta-logical capabilities for formal system analysis and self-referential reasoning
 * @methodology: Object-oriented meta-logic design with μ-calculus operators, Gödel encoding, and consistency verification algorithms
 * @references: ["akao:file:core:engine:logic_pure:v1"]
 */

#pragma once

#include "../../runtime/v1.hpp"

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

/**
 * @brief μ-calculus (mu-calculus) functions for advanced fixpoint logic
 */

class MuCalculusLeastFixpointFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "mucalculus.mu"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::OBJECT; }
    std::string getDescription() const override { return "Least fixpoint operator (μ) for recursive definitions"; }
private:
    PureLogicEngine* engine_;
public:
    void setEngine(PureLogicEngine* engine) { engine_ = engine; }
};

class MuCalculusGreatestFixpointFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "mucalculus.nu"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::OBJECT; }
    std::string getDescription() const override { return "Greatest fixpoint operator (ν) for recursive definitions"; }
private:
    PureLogicEngine* engine_;
public:
    void setEngine(PureLogicEngine* engine) { engine_ = engine; }
};

class RecursiveFunctionDefFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "recursive.define"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::OBJECT; }
    std::string getDescription() const override { return "Defines a recursive function with base case and recursive case"; }
private:
    PureLogicEngine* engine_;
public:
    void setEngine(PureLogicEngine* engine) { engine_ = engine; }
};

class FixpointIterateFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "fixpoint.iterate"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::OBJECT; }
    std::string getDescription() const override { return "Iterates a function to find fixpoints with custom convergence"; }
private:
    PureLogicEngine* engine_;
public:
    void setEngine(PureLogicEngine* engine) { engine_ = engine; }
};

/**
 * @brief Meta-logical functions for self-referential logic and consistency checking
 */

class EncodeFormulaFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "metalogic.encode_formula"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::INTEGER; }
    std::string getDescription() const override { return "Encodes a logic formula as a Gödel number"; }
};

class DecodeFormulaFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "metalogic.decode_formula"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::STRING; }
    std::string getDescription() const override { return "Decodes a Gödel number back to a logic formula"; }
};

class SelfReferenceFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "metalogic.self_reference"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::INTEGER; }
    std::string getDescription() const override { return "Creates self-referential statement using diagonalization"; }
};

class ConsistencyCheckFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "metalogic.consistency_check"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::BOOLEAN; }
    std::string getDescription() const override { return "Checks consistency of logical system"; }
private:
    PureLogicEngine* engine_;
public:
    void setEngine(PureLogicEngine* engine) { engine_ = engine; }
};

class ProvabilityFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "metalogic.provability"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::BOOLEAN; }
    std::string getDescription() const override { return "Checks if a statement is provable in the system"; }
private:
    PureLogicEngine* engine_;
public:
    void setEngine(PureLogicEngine* engine) { engine_ = engine; }
};

class DiagonalizationFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "metalogic.diagonalization"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::INTEGER; }
    std::string getDescription() const override { return "Applies diagonalization to create self-referential statements"; }
};

} // namespace akao::logic::meta
