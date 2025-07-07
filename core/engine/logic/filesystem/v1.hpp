#pragma once

#include "../pure/v1.hpp"
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

// Phase 2: Additional filesystem functions
class ListSubdirsFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "filesystem.list_subdirs"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::COLLECTION; }
    std::string getDescription() const override { return "Lists all subdirectories in a directory"; }
};

class FileExistsFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "filesystem.file_exists"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::BOOLEAN; }
    std::string getDescription() const override { return "Checks if a file or directory exists"; }
};

class IsDirectoryFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "filesystem.is_directory"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::BOOLEAN; }
    std::string getDescription() const override { return "Checks if a path is a directory"; }
};

class GetFileNameFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "filesystem.get_filename"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::STRING; }
    std::string getDescription() const override { return "Extracts the filename from a path"; }
};

/**
 * @brief Register all filesystem functions
 */
void registerFilesystemFunctions(PureLogicEngine& engine);

} // namespace akao::logic::filesystem
