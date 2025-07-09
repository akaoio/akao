/**
 * @id: akao:file:core:engine:logic_builtin:v1
 * @doc: Builtin function library header providing comprehensive collection of fundamental operations including mathematical functions, string manipulation, collection operations, Peano arithmetic primitives, and testing utilities. Implements essential computational building blocks for Pure Logic engine with type-safe function signatures, parameter validation, and standardized return types for reliable logical computation and system testing.
 * @specification: Header for builtin function library with mathematical operations, string functions, and collection utilities
 * @scope: Core engine builtin function library framework
 * @timeline: 2025-07-09
 * @rationale: Provide essential computational primitives and utility functions for Pure Logic engine operation and testing
 * @methodology: Object-oriented builtin function design with type-safe interfaces and comprehensive operation coverage
 * @references: ["akao:file:core:engine:logic_pure:v1"]
 */

#pragma once

#include "../../core/v1.hpp"
#include "../filesystem/v1.hpp"
#include "../cpp/v1.hpp"

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

class MultiplyFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "math.multiply"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::INTEGER; }
    std::string getDescription() const override { return "Multiplies two numbers"; }
};

class DivideFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "math.divide"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::INTEGER; }
    std::string getDescription() const override { return "Integer division of two numbers"; }
};

class ModuloFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "math.modulo"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::INTEGER; }
    std::string getDescription() const override { return "Modulo operation of two numbers"; }
};

class PowerFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "math.power"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::INTEGER; }
    std::string getDescription() const override { return "Raises first number to the power of second"; }
};

/**
 * @brief Peano arithmetic functions for foundational number theory
 */
class SuccessorFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "peano.successor"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::INTEGER; }
    std::string getDescription() const override { return "Successor function S(n) = n + 1"; }
};

class IsZeroFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "peano.is_zero"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::BOOLEAN; }
    std::string getDescription() const override { return "Checks if number is zero"; }
};

class PredecessorFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "peano.predecessor"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::INTEGER; }
    std::string getDescription() const override { return "Predecessor function (with 0 as base case)"; }
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

class StringStartsWithFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "string.starts_with"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::BOOLEAN; }
    std::string getDescription() const override { return "Checks if string starts with prefix"; }
};

class StringContainsFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "string.contains"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::BOOLEAN; }
    std::string getDescription() const override { return "Checks if string contains substring"; }
};

/**
 * @brief Recursive function definition functions
 */
class RecursiveDefineFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "recursive.define"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::OBJECT; }
    std::string getDescription() const override { return "Defines a recursive function"; }
};

/**
 * @brief Gitignore pattern matching functions
 */
class GitignoreShouldIncludeFileFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "gitignore.should_include_file"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::BOOLEAN; }
    std::string getDescription() const override { return "Checks if file should be included based on gitignore patterns"; }
    
public:
    bool matchGitignorePattern(const std::string& file_path, const std::string& pattern);
    bool matchGitignoreWildcard(const std::string& text, const std::string& pattern);
    bool matchGitignoreDoubleWildcard(const std::string& file_path, const std::string& pattern);
};

class GitignoreParsePatternFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "gitignore.parse_patterns"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::COLLECTION; }
    std::string getDescription() const override { return "Parses gitignore file content into patterns"; }
};

class GitignoreMatchPatternFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "gitignore.match_pattern"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::BOOLEAN; }
    std::string getDescription() const override { return "Matches file path against gitignore pattern"; }
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
