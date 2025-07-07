#include "v1.hpp"
#include <fstream>
#include <sstream>

namespace akao::logic::filesystem {

// GetFilesFunction
Value GetFilesFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 1) {
        throw std::runtime_error("filesystem.get_files expects 1 argument (directory path)");
    }
    
    if (args[0].getType() != Value::Type::STRING) {
        throw std::runtime_error("filesystem.get_files expects string argument");
    }
    
    std::string directory = args[0].asString();
    std::vector<Value> files;
    
    try {
        if (std::filesystem::exists(directory) && std::filesystem::is_directory(directory)) {
            for (const auto& entry : std::filesystem::directory_iterator(directory)) {
                if (entry.is_regular_file()) {
                    files.emplace_back(entry.path().string());
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        // Return empty collection on filesystem errors
    }
    
    return Value(files);
}

std::vector<Value::Type> GetFilesFunction::getParameterTypes() const {
    return {Value::Type::STRING};
}

// GetCppFilesFunction
Value GetCppFilesFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 1) {
        throw std::runtime_error("filesystem.get_cpp_files expects 1 argument (directory path)");
    }
    
    if (args[0].getType() != Value::Type::STRING) {
        throw std::runtime_error("filesystem.get_cpp_files expects string argument");
    }
    
    std::string directory = args[0].asString();
    std::vector<Value> cppFiles;
    
    try {
        if (std::filesystem::exists(directory) && std::filesystem::is_directory(directory)) {
            for (const auto& entry : std::filesystem::directory_iterator(directory)) {
                if (entry.is_regular_file()) {
                    std::string extension = entry.path().extension().string();
                    if (extension == ".cpp" || extension == ".hpp" || 
                        extension == ".cc" || extension == ".h" ||
                        extension == ".cxx" || extension == ".hxx") {
                        cppFiles.emplace_back(entry.path().string());
                    }
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        // Return empty collection on filesystem errors
    }
    
    return Value(cppFiles);
}

std::vector<Value::Type> GetCppFilesFunction::getParameterTypes() const {
    return {Value::Type::STRING};
}

// HasExtensionFunction
Value HasExtensionFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 2) {
        throw std::runtime_error("filesystem.has_extension expects 2 arguments (file_path, extension)");
    }
    
    if (args[0].getType() != Value::Type::STRING || args[1].getType() != Value::Type::STRING) {
        throw std::runtime_error("filesystem.has_extension expects string arguments");
    }
    
    std::string filePath = args[0].asString();
    std::string expectedExtension = args[1].asString();
    
    // Ensure extension starts with a dot
    if (!expectedExtension.empty() && expectedExtension[0] != '.') {
        expectedExtension = "." + expectedExtension;
    }
    
    try {
        std::filesystem::path path(filePath);
        std::string actualExtension = path.extension().string();
        return Value(actualExtension == expectedExtension);
    } catch (const std::filesystem::filesystem_error& e) {
        return Value(false);
    }
}

std::vector<Value::Type> HasExtensionFunction::getParameterTypes() const {
    return {Value::Type::STRING, Value::Type::STRING};
}

// ReadFileFunction
Value ReadFileFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 1) {
        throw std::runtime_error("filesystem.read_file expects 1 argument (file path)");
    }
    
    if (args[0].getType() != Value::Type::STRING) {
        throw std::runtime_error("filesystem.read_file expects string argument");
    }
    
    std::string filePath = args[0].asString();
    
    try {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            return Value(""); // Return empty string if file cannot be opened
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        return Value(buffer.str());
    } catch (const std::exception& e) {
        return Value(""); // Return empty string on any error
    }
}

std::vector<Value::Type> ReadFileFunction::getParameterTypes() const {
    return {Value::Type::STRING};
}

// CurrentDirectoryFunction
Value CurrentDirectoryFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 0) {
        throw std::runtime_error("filesystem.current_directory expects no arguments");
    }
    
    try {
        return Value(std::filesystem::current_path().string());
    } catch (const std::filesystem::filesystem_error& e) {
        return Value(""); // Return empty string on error
    }
}

std::vector<Value::Type> CurrentDirectoryFunction::getParameterTypes() const {
    return {};
}

// Phase 2: Additional filesystem functions implementation
Value ListSubdirsFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 1) {
        throw std::runtime_error("filesystem.list_subdirs expects 1 argument (directory path)");
    }
    
    if (args[0].getType() != Value::Type::STRING) {
        throw std::runtime_error("filesystem.list_subdirs expects string argument");
    }
    
    std::string directory = args[0].asString();
    std::vector<Value> subdirs;
    
    try {
        if (std::filesystem::exists(directory) && std::filesystem::is_directory(directory)) {
            for (const auto& entry : std::filesystem::directory_iterator(directory)) {
                if (entry.is_directory()) {
                    subdirs.emplace_back(entry.path().string());
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        // Return empty collection on filesystem errors
    }
    
    return Value(subdirs);
}

std::vector<Value::Type> ListSubdirsFunction::getParameterTypes() const {
    return {Value::Type::STRING};
}

Value FileExistsFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 1) {
        throw std::runtime_error("filesystem.file_exists expects 1 argument (file path)");
    }
    
    if (args[0].getType() != Value::Type::STRING) {
        throw std::runtime_error("filesystem.file_exists expects string argument");
    }
    
    std::string path = args[0].asString();
    
    try {
        return Value(std::filesystem::exists(path));
    } catch (const std::filesystem::filesystem_error& e) {
        return Value(false);
    }
}

std::vector<Value::Type> FileExistsFunction::getParameterTypes() const {
    return {Value::Type::STRING};
}

Value IsDirectoryFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 1) {
        throw std::runtime_error("filesystem.is_directory expects 1 argument (path)");
    }
    
    if (args[0].getType() != Value::Type::STRING) {
        throw std::runtime_error("filesystem.is_directory expects string argument");
    }
    
    std::string path = args[0].asString();
    
    try {
        return Value(std::filesystem::exists(path) && std::filesystem::is_directory(path));
    } catch (const std::filesystem::filesystem_error& e) {
        return Value(false);
    }
}

std::vector<Value::Type> IsDirectoryFunction::getParameterTypes() const {
    return {Value::Type::STRING};
}

Value GetFileNameFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 1) {
        throw std::runtime_error("filesystem.get_filename expects 1 argument (file path)");
    }
    
    if (args[0].getType() != Value::Type::STRING) {
        throw std::runtime_error("filesystem.get_filename expects string argument");
    }
    
    std::string path = args[0].asString();
    
    try {
        std::filesystem::path fs_path(path);
        return Value(fs_path.filename().string());
    } catch (const std::filesystem::filesystem_error& e) {
        return Value("");
    }
}

std::vector<Value::Type> GetFileNameFunction::getParameterTypes() const {
    return {Value::Type::STRING};
}

// Registration function
void registerFilesystemFunctions(PureLogicEngine& engine) {
    engine.registerFunction(std::make_unique<GetFilesFunction>());
    engine.registerFunction(std::make_unique<GetCppFilesFunction>());
    engine.registerFunction(std::make_unique<HasExtensionFunction>());
    engine.registerFunction(std::make_unique<ReadFileFunction>());
    engine.registerFunction(std::make_unique<CurrentDirectoryFunction>());
    
    // Phase 2: Additional functions
    engine.registerFunction(std::make_unique<ListSubdirsFunction>());
    engine.registerFunction(std::make_unique<FileExistsFunction>());
    engine.registerFunction(std::make_unique<IsDirectoryFunction>());
    engine.registerFunction(std::make_unique<GetFileNameFunction>());
}

} // namespace akao::logic::filesystem
