#pragma once

#include "../../runtime/v1.hpp"
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

// Phase 2: Enhanced C++ analysis functions
class CountFunctionsFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "cpp.count_functions"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::INTEGER; }
    std::string getDescription() const override { return "Counts the number of functions in C++ code"; }
};

class ExtractNamespacesFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "cpp.extract_namespaces"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::COLLECTION; }
    std::string getDescription() const override { return "Extracts namespace declarations from C++ code"; }
};

class HasClassFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "cpp.has_class"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::BOOLEAN; }
    std::string getDescription() const override { return "Checks if C++ code contains a specific class"; }
};

class GetLineCountFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "cpp.get_line_count"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::INTEGER; }
    std::string getDescription() const override { return "Counts the number of lines in C++ code"; }
};

class SuggestClassSplitFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "cpp.suggest_class_split"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::STRING; }
    std::string getDescription() const override { return "Suggests how to split classes with multiple definitions"; }
};

// Phase 3: Additional analysis functions
class UsesModernCppFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "cpp.uses_modern_cpp"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::BOOLEAN; }
    std::string getDescription() const override { return "Checks if C++ code uses modern C++ features"; }
};

class HasProperHeadersFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "cpp.has_proper_headers"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::BOOLEAN; }
    std::string getDescription() const override { return "Checks if C++ code has proper header guards"; }
};

class UsesDeprecatedFeaturesFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "cpp.uses_deprecated_features"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::BOOLEAN; }
    std::string getDescription() const override { return "Checks if C++ code uses deprecated features"; }
};

class FollowsNamingConventionsFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "cpp.follows_naming_conventions"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::BOOLEAN; }
    std::string getDescription() const override { return "Checks if C++ code follows naming conventions"; }
};

/**
 * @brief Register all C++ analysis functions
 */
void registerCppFunctions(PureLogicEngine& engine);

} // namespace akao::logic::cpp
