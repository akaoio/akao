#include "v1.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace akao::interfaces::cli::commands {

InitCommand::InitCommand() = default;

executor::ExecutionResult InitCommand::execute(const executor::ExecutionContext& context,
                                              const std::vector<std::string>& args) {
    executor::ExecutionResult result;
    result.success = true;
    result.exit_code = 0;

    try {
        auto options = parseArguments(args);

        // Handle interactive mode
        if (options.interactive) {
            return interactiveInit(options, context);
        }

        // Validate target directory
        if (!validateTargetDirectory(options.target_path, options.force_overwrite)) {
            result.success = false;
            result.exit_code = 1;
            result.data["error"] = "Target directory validation failed";
            return result;
        }

        // Initialize based on project type
        // Support both "cpp" and "cpp-project" for backward compatibility
        if (options.project_type == "cpp" || options.project_type == "cpp-project") {
            return initCppProject(options, context);
        } else if (options.project_type == "rust" || options.project_type == "rust-project") {
            return initRustProject(options, context);
        } else if (options.project_type == "framework") {
            return initFramework(options, context);
        } else if (options.project_type == "language-adapter") {
            return initLanguageAdapter(options, context);
        } else {
            result.success = false;
            result.exit_code = 1;
            result.data["error"] = "Unknown project type: " + options.project_type;
            return result;
        }

    } catch (const std::exception& e) {
        result.success = false;
        result.exit_code = 1;
        result.data["error"] = "Initialization failed: " + std::string(e.what());
        return result;
    }
}

InitCommand::InitOptions InitCommand::parseArguments(const std::vector<std::string>& args) {
    InitOptions options;

    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];

        if (arg == "--type" && i + 1 < args.size()) {
            options.project_type = args[++i];
        }
        else if (arg == "--template" && i + 1 < args.size()) {
            options.template_type = args[++i];
        }
        else if (arg == "--rules" && i + 1 < args.size()) {
            options.custom_rules = args[++i];
        }
        else if (arg == "--philosophy" && i + 1 < args.size()) {
            options.philosophy_level = args[++i];
        }
        else if (arg == "--interactive") {
            options.interactive = true;
        }
        else if (arg == "--force") {
            options.force_overwrite = true;
        }
        else if (arg.find("--") != 0 && options.target_path == ".") {
            // First non-option argument is the target path
            options.target_path = arg;
        }
    }

    return options;
}

executor::ExecutionResult InitCommand::interactiveInit(const InitOptions& options,
                                                      const executor::ExecutionContext& context) {
    executor::ExecutionResult result;
    result.success = true;
    result.exit_code = 0;

    std::cout << "🚀 Welcome to Akao Interactive Project Setup\n\n";

    // Get project type
    auto project_types = getAvailableProjectTypes();
    std::cout << "Available project types:\n";
    for (size_t i = 0; i < project_types.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << project_types[i] << "\n";
    }
    std::cout << "Select project type (1-" << project_types.size() << "): ";
    
    int selection;
    std::cin >> selection;
    
    if (selection < 1 || selection > static_cast<int>(project_types.size())) {
        result.success = false;
        result.exit_code = 1;
        result.data["error"] = "Invalid project type selection";
        return result;
    }

    InitOptions interactive_options = options;
    interactive_options.project_type = project_types[selection - 1];
    interactive_options.interactive = false; // Prevent infinite loop

    result.data["interactive"] = "true";
    result.data["selected_type"] = interactive_options.project_type;

    // Continue with selected project type
    return execute(context, {interactive_options.project_type});
}

executor::ExecutionResult InitCommand::initCppProject(const InitOptions& options,
                                                     const executor::ExecutionContext& context) {
    executor::ExecutionResult result;
    result.success = true;
    result.exit_code = 0;

    std::string base_path = options.target_path;
    
    // Create project structure
    if (!createProjectStructure(base_path, "cpp")) {
        result.success = false;
        result.exit_code = 1;
        result.data["error"] = "Failed to create C++ project structure";
        return result;
    }

    // Create Akao configuration
    if (!createAkaoConfiguration(base_path, options)) {
        result.success = false;
        result.exit_code = 1;
        result.data["error"] = "Failed to create Akao configuration";
        return result;
    }

    // Create build system
    if (!createBuildSystem(base_path, "cpp")) {
        result.success = false;
        result.exit_code = 1;
        result.data["error"] = "Failed to create build system";
        return result;
    }

    // Create source files
    if (!createSourceFiles(base_path, "cpp")) {
        result.success = false;
        result.exit_code = 1;
        result.data["error"] = "Failed to create source files";
        return result;
    }

    // Create test structure
    if (!createTestStructure(base_path, "cpp")) {
        result.success = false;
        result.exit_code = 1;
        result.data["error"] = "Failed to create test structure";
        return result;
    }

    // Create documentation
    if (!createDocumentationStructure(base_path)) {
        result.success = false;
        result.exit_code = 1;
        result.data["error"] = "Failed to create documentation structure";
        return result;
    }

    result.data["project_type"] = "cpp-project";
    result.data["target_path"] = base_path;
    result.data["template"] = options.template_type;
    result.data["status"] = "created";

    return result;
}

executor::ExecutionResult InitCommand::initRustProject(const InitOptions& options,
                                                      const executor::ExecutionContext& context) {
    executor::ExecutionResult result;
    result.success = true;
    result.exit_code = 0;

    std::string base_path = options.target_path;
    
    // Create Rust project structure
    if (!createProjectStructure(base_path, "rust")) {
        result.success = false;
        result.exit_code = 1;
        result.data["error"] = "Failed to create Rust project structure";
        return result;
    }

    result.data["project_type"] = "rust-project";
    result.data["target_path"] = base_path;
    result.data["status"] = "created";

    return result;
}

executor::ExecutionResult InitCommand::initFramework(const InitOptions& options,
                                                    const executor::ExecutionContext& context) {
    executor::ExecutionResult result;
    result.success = true;
    result.exit_code = 0;

    std::string base_path = options.target_path;
    
    // Create framework structure
    if (!createProjectStructure(base_path, "framework")) {
        result.success = false;
        result.exit_code = 1;
        result.data["error"] = "Failed to create framework structure";
        return result;
    }

    result.data["project_type"] = "framework";
    result.data["target_path"] = base_path;
    result.data["status"] = "created";

    return result;
}

executor::ExecutionResult InitCommand::initLanguageAdapter(const InitOptions& options,
                                                          const executor::ExecutionContext& context) {
    executor::ExecutionResult result;
    result.success = true;
    result.exit_code = 0;

    std::string base_path = options.target_path;
    
    // Create language adapter structure
    if (!createProjectStructure(base_path, "language-adapter")) {
        result.success = false;
        result.exit_code = 1;
        result.data["error"] = "Failed to create language adapter structure";
        return result;
    }

    result.data["project_type"] = "language-adapter";
    result.data["target_path"] = base_path;
    result.data["status"] = "created";

    return result;
}

bool InitCommand::createProjectStructure(const std::string& base_path, const std::string& project_type) {
    try {
        // Create base directory
        std::filesystem::create_directories(base_path);

        if (project_type == "cpp") {
            // C++ project structure
            std::filesystem::create_directories(base_path + "/src");
            std::filesystem::create_directories(base_path + "/include");
            std::filesystem::create_directories(base_path + "/tests");
            std::filesystem::create_directories(base_path + "/docs");
            std::filesystem::create_directories(base_path + "/build");
            std::filesystem::create_directories(base_path + "/.akao");
        } else if (project_type == "rust") {
            // Rust project structure
            std::filesystem::create_directories(base_path + "/src");
            std::filesystem::create_directories(base_path + "/tests");
            std::filesystem::create_directories(base_path + "/docs");
            std::filesystem::create_directories(base_path + "/.akao");
        } else if (project_type == "framework") {
            // Framework structure
            std::filesystem::create_directories(base_path + "/core");
            std::filesystem::create_directories(base_path + "/interfaces");
            std::filesystem::create_directories(base_path + "/adapters");
            std::filesystem::create_directories(base_path + "/rules");
            std::filesystem::create_directories(base_path + "/philosophies");
            std::filesystem::create_directories(base_path + "/tests");
            std::filesystem::create_directories(base_path + "/.akao");
        } else if (project_type == "language-adapter") {
            // Language adapter structure
            std::filesystem::create_directories(base_path + "/adapters");
            std::filesystem::create_directories(base_path + "/parsers");
            std::filesystem::create_directories(base_path + "/validators");
            std::filesystem::create_directories(base_path + "/tests");
            std::filesystem::create_directories(base_path + "/.akao");
        }

        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool InitCommand::createAkaoConfiguration(const std::string& base_path, const InitOptions& options) {
    try {
        std::string config_dir = base_path + "/.akao";
        std::filesystem::create_directories(config_dir);

        // Create main configuration file
        std::ofstream config_file(config_dir + "/config.yaml");
        config_file << "akao:\n";
        config_file << "  version: 1.0.0\n";
        config_file << "  project_type: " << options.project_type << "\n";
        config_file << "  template: " << options.template_type << "\n";
        config_file << "  philosophy_level: " << options.philosophy_level << "\n";
        config_file << "  rules:\n";
        config_file << "    enforce_structure: true\n";
        config_file << "    enforce_interface: true\n";
        config_file << "    enforce_testing: true\n";
        config_file.close();

        // Create rules configuration
        std::ofstream rules_file(config_dir + "/rules.yaml");
        rules_file << "rules:\n";
        rules_file << "  structure:\n";
        rules_file << "    - class_separation\n";
        rules_file << "    - file_organization\n";
        rules_file << "  interface:\n";
        rules_file << "    - parameter_consistency\n";
        rules_file << "    - output_format\n";
        rules_file << "  testing:\n";
        rules_file << "    - coverage_enforcement\n";
        rules_file.close();

        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool InitCommand::createBuildSystem(const std::string& base_path, const std::string& project_type) {
    try {
        if (project_type == "cpp") {
            // Create CMakeLists.txt
            std::ofstream cmake_file(base_path + "/CMakeLists.txt");
            cmake_file << "cmake_minimum_required(VERSION 3.15)\n";
            cmake_file << "project(AkaoProject)\n\n";
            cmake_file << "set(CMAKE_CXX_STANDARD 17)\n";
            cmake_file << "set(CMAKE_CXX_STANDARD_REQUIRED ON)\n\n";
            cmake_file << "add_executable(main src/main.cpp)\n\n";
            cmake_file << "# Add tests\n";
            cmake_file << "enable_testing()\n";
            cmake_file << "add_subdirectory(tests)\n";
            cmake_file.close();

            // Create Makefile
            std::ofstream makefile(base_path + "/Makefile");
            makefile << ".PHONY: all build test clean\n\n";
            makefile << "all: build\n\n";
            makefile << "build:\n";
            makefile << "\tcmake -B build\n";
            makefile << "\tcmake --build build\n\n";
            makefile << "test: build\n";
            makefile << "\tcd build && ctest\n\n";
            makefile << "clean:\n";
            makefile << "\trm -rf build\n";
            makefile.close();
        }

        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool InitCommand::createSourceFiles(const std::string& base_path, const std::string& project_type) {
    try {
        if (project_type == "cpp") {
            // Create main.cpp
            std::ofstream main_file(base_path + "/src/main.cpp");
            main_file << "#include <iostream>\n\n";
            main_file << "/**\n";
            main_file << " * @brief Main entry point for Akao-compliant C++ project\n";
            main_file << " * \n";
            main_file << " * Philosophy compliance:\n";
            main_file << " * - akao:philosophy:structure:isolation:v1 - Single responsibility\n";
            main_file << " * - akao:philosophy:interface:consistency:v1 - Consistent interface\n";
            main_file << " */\n";
            main_file << "int main() {\n";
            main_file << "    std::cout << \"Hello, Akao World!\" << std::endl;\n";
            main_file << "    return 0;\n";
            main_file << "}\n";
            main_file.close();
        }

        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool InitCommand::createTestStructure(const std::string& base_path, const std::string& project_type) {
    try {
        if (project_type == "cpp") {
            // Create test CMakeLists.txt
            std::ofstream test_cmake(base_path + "/tests/CMakeLists.txt");
            test_cmake << "add_executable(test_main test_main.cpp)\n";
            test_cmake << "add_test(NAME MainTest COMMAND test_main)\n";
            test_cmake.close();

            // Create test file
            std::ofstream test_file(base_path + "/tests/test_main.cpp");
            test_file << "#include <cassert>\n";
            test_file << "#include <iostream>\n\n";
            test_file << "/**\n";
            test_file << " * @brief Test for main functionality\n";
            test_file << " * \n";
            test_file << " * Philosophy compliance:\n";
            test_file << " * - akao:philosophy:testing:coverage:v1 - Comprehensive testing\n";
            test_file << " */\n";
            test_file << "int main() {\n";
            test_file << "    // Basic test placeholder\n";
            test_file << "    assert(true);\n";
            test_file << "    std::cout << \"All tests passed!\" << std::endl;\n";
            test_file << "    return 0;\n";
            test_file << "}\n";
            test_file.close();
        }

        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool InitCommand::createDocumentationStructure(const std::string& base_path) {
    try {
        std::filesystem::create_directories(base_path + "/docs");

        // Create README.md
        std::ofstream readme(base_path + "/README.md");
        readme << "# Akao Project\n\n";
        readme << "Universal validation and enforcement framework project.\n\n";
        readme << "## Philosophy Compliance\n\n";
        readme << "This project follows Akao philosophies:\n";
        readme << "- Structure Isolation: One class per file\n";
        readme << "- Interface Consistency: Uniform interfaces\n";
        readme << "- Testing Coverage: Comprehensive testing\n\n";
        readme << "## Build\n\n";
        readme << "```bash\n";
        readme << "make build\n";
        readme << "```\n\n";
        readme << "## Test\n\n";
        readme << "```bash\n";
        readme << "make test\n";
        readme << "```\n\n";
        readme << "## Validate\n\n";
        readme << "```bash\n";
        readme << "akao validate\n";
        readme << "```\n";
        readme.close();

        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool InitCommand::validateTargetDirectory(const std::string& path, bool force_overwrite) {
    if (std::filesystem::exists(path)) {
        if (!std::filesystem::is_directory(path)) {
            return false; // Path exists but is not a directory
        }
        
        if (!force_overwrite && !std::filesystem::is_empty(path)) {
            return false; // Directory is not empty and force not specified
        }
    }
    
    return true;
}

std::vector<std::string> InitCommand::getAvailableProjectTypes() const {
    return {
        "cpp-project",
        "rust-project", 
        "framework",
        "language-adapter"
    };
}

std::vector<std::string> InitCommand::getAvailableTemplates() const {
    return {
        "minimal",
        "standard",
        "full"
    };
}

std::string InitCommand::getHelp() const {
    return R"(akao init - Universal Project Genesis

USAGE:
    akao init [OPTIONS] [PATH]

DESCRIPTION:
    Initialize new Akao-compliant projects of any type with proper structure
    and configuration.

OPTIONS:
    --type <type>           Project type (cpp-project, rust-project, framework, language-adapter)
    --template <template>   Template type (minimal, standard, full)
    --rules <file>          Custom rules file
    --philosophy <level>    Philosophy compliance level (strict, standard, minimal)
    --interactive          Interactive project setup
    --force                Force overwrite existing directory

EXAMPLES:
    akao init                              # Interactive setup in current directory
    akao init my-project                   # Create project in my-project directory
    akao init --type cpp-project ./cpp-app # Create C++ project
    akao init --type framework ./my-fw     # Create new framework
    akao init --interactive                # Interactive mode

PROJECT TYPES:
    cpp-project      - C++ project with CMake/Make build system
    rust-project     - Rust project with Cargo integration  
    framework        - New framework with Akao compliance
    language-adapter - Language adapter template

PHILOSOPHY:
    Embodies Universal Genesis - creates properly structured projects
    that follow Akao philosophies from the start.
)";
}

std::string InitCommand::getUsage() const {
    return "akao init [OPTIONS] [PATH]";
}

std::map<std::string, std::string> InitCommand::getSupportedOptions() const {
    return {
        {"type", "Project type (cpp-project, rust-project, framework, language-adapter)"},
        {"template", "Template type (minimal, standard, full)"},
        {"rules", "Custom rules file"},
        {"philosophy", "Philosophy compliance level"}
    };
}

std::map<std::string, std::string> InitCommand::getSupportedFlags() const {
    return {
        {"help", "Show command help"},
        {"interactive", "Interactive project setup"},
        {"force", "Force overwrite existing directory"}
    };
}

} // namespace akao::interfaces::cli::commands
