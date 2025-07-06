#pragma once

#include "pure_logic_engine.hpp"
#include <filesystem>

namespace akao::logic::filesystem {

/**
 * @brief Filesystem operation functions
 * 
 * These are PURE COMPUTATIONAL functions for filesystem inspection.
 * They contain NO domain logic - just computational primitives for
 * examining files and directories.
 */

class GetFilesFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "filesystem.get_files"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::COLLECTION; }
    std::string getDescription() const override { return "Returns all files in a directory"; }
};

class GetCppFilesFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "filesystem.get_cpp_files"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::COLLECTION; }
    std::string getDescription() const override { return "Returns all .cpp and .hpp files in a directory"; }
};

class HasExtensionFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "filesystem.has_extension"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::BOOLEAN; }
    std::string getDescription() const override { return "Checks if a file has a specific extension"; }
};

class ReadFileFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "filesystem.read_file"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::STRING; }
    std::string getDescription() const override { return "Reads the content of a file"; }
};

class CurrentDirectoryFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "filesystem.current_directory"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::STRING; }
    std::string getDescription() const override { return "Returns the current working directory"; }
};

/**
 * @brief Register all filesystem functions
 */
void registerFilesystemFunctions(PureLogicEngine& engine);

} // namespace akao::logic::filesystem
