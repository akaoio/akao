#pragma once

#include "pure_logic_engine.hpp"
#include <regex>

namespace akao::logic::cpp {

/**
 * @brief C++ code analysis functions
 * 
 * These are PURE COMPUTATIONAL functions for analyzing C++ source code.
 * They contain NO domain logic - just computational primitives for
 * extracting information from C++ code.
 */

class ExtractClassesFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "cpp.extract_classes"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::COLLECTION; }
    std::string getDescription() const override { return "Extracts class names from C++ code"; }
};

class CountClassesFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "cpp.count_classes"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::INTEGER; }
    std::string getDescription() const override { return "Counts the number of classes in C++ code"; }
};

class ExtractFunctionsFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "cpp.extract_functions"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::COLLECTION; }
    std::string getDescription() const override { return "Extracts function names from C++ code"; }
};

class GetIncludesFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "cpp.get_includes"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::COLLECTION; }
    std::string getDescription() const override { return "Extracts include statements from C++ code"; }
};

class HasMainFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "cpp.has_main"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::BOOLEAN; }
    std::string getDescription() const override { return "Checks if C++ code contains a main function"; }
};

/**
 * @brief Register all C++ analysis functions
 */
void registerCppFunctions(PureLogicEngine& engine);

} // namespace akao::logic::cpp
