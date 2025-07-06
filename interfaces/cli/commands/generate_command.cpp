#include "generate_command.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace akao::interfaces::cli::commands {

GenerateCommand::GenerateCommand() = default;

executor::ExecutionResult GenerateCommand::execute(const executor::ExecutionContext& context,
                                                  const std::vector<std::string>& args) {
    executor::ExecutionResult result;
    result.success = true;
    result.exit_code = 0;

    try {
        auto options = parseArguments(args);

        if (options.generation_type.empty()) {
            result.success = false;
            result.exit_code = 1;
            result.data["error"] = "Generation type required";
            return result;
        }

        // Route to appropriate generation function
        if (options.generation_type == "project") {
            return generateProject(options, context);
        } else if (options.generation_type == "framework") {
            return generateFramework(options, context);
        } else if (options.generation_type == "language" || options.generation_type == "language-support") {
            return generateLanguageSupport(options, context);
        } else if (options.generation_type == "build" || options.generation_type == "build-system") {
            return generateBuildSystem(options, context);
        } else if (options.generation_type == "docs" || options.generation_type == "documentation") {
            return generateDocumentation(options, context);
        } else if (options.generation_type == "tests") {
            return generateTests(options, context);
        } else if (options.generation_type == "universe") {
            return generateUniverse(options, context);
        } else {
            result.success = false;
            result.exit_code = 1;
            result.data["error"] = "Unknown generation type: " + options.generation_type;
            return result;
        }

    } catch (const std::exception& e) {
        result.success = false;
        result.exit_code = 1;
        result.data["error"] = "Generation failed: " + std::string(e.what());
        return result;
    }
}

GenerateCommand::GenerateOptions GenerateCommand::parseArguments(const std::vector<std::string>& args) {
    GenerateOptions options;

    if (!args.empty()) {
        options.generation_type = args[0];
    }

    for (size_t i = 1; i < args.size(); ++i) {
        const std::string& arg = args[i];

        if (arg == "--lang" && i + 1 < args.size()) {
            options.language = args[++i];
        }
        else if (arg == "--name" && i + 1 < args.size()) {
            // For language generation, --name sets the language
            // For framework generation, --name sets the framework name
            if (!args.empty() && args[0] == "language") {
                options.language = args[++i];
            } else {
                options.framework_name = args[++i];
            }
        }
        else if (arg == "--template" && i + 1 < args.size()) {
            options.template_path = args[++i];
        }
        else if (arg == "--universe" && i + 1 < args.size()) {
            options.target_directory = args[++i];
        }
        else if (arg == "--type" && i + 1 < args.size()) {
            options.build_system = args[++i];
        }
        else if (arg == "--coverage") {
            if (i + 1 < args.size() && args[i + 1] == "full") {
                options.full_coverage = true;
                ++i;
            }
        }
        else if (arg == "--examples") {
            options.include_examples = true;
        }
    }

    if (options.target_directory.empty()) {
        options.target_directory = "./tmp/generated";
    }

    return options;
}

executor::ExecutionResult GenerateCommand::generateProject(const GenerateOptions& options,
                                                          const executor::ExecutionContext& context) {
    executor::ExecutionResult result;
    result.success = true;
    result.exit_code = 0;

    std::string target_dir = options.target_directory;
    std::filesystem::create_directories(target_dir);

    // Generate project based on language
    if (options.language == "cpp") {
        // Create C++ project structure
        std::filesystem::create_directories(target_dir + "/src");
        std::filesystem::create_directories(target_dir + "/include");
        std::filesystem::create_directories(target_dir + "/tests");
        std::filesystem::create_directories(target_dir + "/.akao");

        // Create main.cpp
        std::ofstream main_file(target_dir + "/src/main.cpp");
        main_file << "#include <iostream>\n\n";
        main_file << "int main() {\n";
        main_file << "    std::cout << \"Generated C++ Project\" << std::endl;\n";
        main_file << "    return 0;\n";
        main_file << "}\n";
        main_file.close();

        // Create CMakeLists.txt
        std::ofstream cmake_file(target_dir + "/CMakeLists.txt");
        cmake_file << "cmake_minimum_required(VERSION 3.15)\n";
        cmake_file << "project(GeneratedProject)\n\n";
        cmake_file << "set(CMAKE_CXX_STANDARD 17)\n";
        cmake_file << "add_executable(main src/main.cpp)\n";
        cmake_file.close();

    } else if (options.language == "rust") {
        // Create Rust project
        std::filesystem::create_directories(target_dir + "/src");
        
        std::ofstream cargo_file(target_dir + "/Cargo.toml");
        cargo_file << "[package]\n";
        cargo_file << "name = \"generated-project\"\n";
        cargo_file << "version = \"0.1.0\"\n";
        cargo_file << "edition = \"2021\"\n";
        cargo_file.close();

        std::ofstream main_file(target_dir + "/src/main.rs");
        main_file << "fn main() {\n";
        main_file << "    println!(\"Generated Rust Project\");\n";
        main_file << "}\n";
        main_file.close();

    } else {
        // Generic project
        std::filesystem::create_directories(target_dir + "/src");
        std::ofstream readme(target_dir + "/README.md");
        readme << "# Generated Project\n\n";
        readme << "Language: " << (options.language.empty() ? "Generic" : options.language) << "\n";
        readme.close();
    }

    result.data["generation_type"] = "project";
    result.data["language"] = options.language;
    result.data["target_directory"] = target_dir;
    result.data["status"] = "generated";

    return result;
}

executor::ExecutionResult GenerateCommand::generateFramework(const GenerateOptions& options,
                                                           const executor::ExecutionContext& context) {
    executor::ExecutionResult result;
    result.success = true;
    result.exit_code = 0;

    std::string target_dir = options.target_directory;
    std::string framework_name = options.framework_name.empty() ? "GeneratedFramework" : options.framework_name;

    // Create framework structure
    if (!createFrameworkCore(target_dir, framework_name)) {
        result.success = false;
        result.exit_code = 1;
        result.data["error"] = "Failed to create framework core";
        return result;
    }

    result.data["generation_type"] = "framework";
    result.data["framework_name"] = framework_name;
    result.data["target_directory"] = target_dir;
    result.data["status"] = "generated";

    return result;
}

executor::ExecutionResult GenerateCommand::generateLanguageSupport(const GenerateOptions& options,
                                                                  const executor::ExecutionContext& context) {
    executor::ExecutionResult result;
    result.success = true;
    result.exit_code = 0;

    std::string target_dir = options.target_directory;
    std::string language = options.language;

    if (language.empty()) {
        result.success = false;
        result.exit_code = 1;
        result.data["error"] = "Language required for language support generation";
        return result;
    }

    std::filesystem::create_directories(target_dir);

    // Create language parser
    if (!createLanguageParser(target_dir, language)) {
        result.success = false;
        result.exit_code = 1;
        result.data["error"] = "Failed to create language parser";
        return result;
    }

    // Create language validator
    if (!createLanguageValidator(target_dir, language)) {
        result.success = false;
        result.exit_code = 1;
        result.data["error"] = "Failed to create language validator";
        return result;
    }

    result.data["generation_type"] = "language-support";
    result.data["language"] = language;
    result.data["target_directory"] = target_dir;
    result.data["status"] = "generated";

    return result;
}

executor::ExecutionResult GenerateCommand::generateBuildSystem(const GenerateOptions& options,
                                                              const executor::ExecutionContext& context) {
    executor::ExecutionResult result;
    result.success = true;
    result.exit_code = 0;

    std::string target_dir = options.target_directory;
    std::string build_system = options.build_system.empty() ? "cmake" : options.build_system;

    std::filesystem::create_directories(target_dir);

    // Create build adapters
    if (!createBuildAdapters(target_dir, build_system)) {
        result.success = false;
        result.exit_code = 1;
        result.data["error"] = "Failed to create build system adapters";
        return result;
    }

    result.data["generation_type"] = "build-system";
    result.data["build_system"] = build_system;
    result.data["target_directory"] = target_dir;
    result.data["status"] = "generated";

    return result;
}

executor::ExecutionResult GenerateCommand::generateDocumentation(const GenerateOptions& options,
                                                                const executor::ExecutionContext& context) {
    executor::ExecutionResult result;
    result.success = true;
    result.exit_code = 0;

    std::string target_dir = options.target_directory;
    std::filesystem::create_directories(target_dir);

    // Generate documentation from rules
    if (!generateDocumentationFromRules(target_dir)) {
        result.success = false;
        result.exit_code = 1;
        result.data["error"] = "Failed to generate documentation";
        return result;
    }

    result.data["generation_type"] = "documentation";
    result.data["target_directory"] = target_dir;
    result.data["status"] = "generated";

    return result;
}

executor::ExecutionResult GenerateCommand::generateTests(const GenerateOptions& options,
                                                        const executor::ExecutionContext& context) {
    executor::ExecutionResult result;
    result.success = true;
    result.exit_code = 0;

    std::string target_dir = options.target_directory;
    std::filesystem::create_directories(target_dir);

    // Generate comprehensive tests
    if (!generateComprehensiveTests(target_dir, options)) {
        result.success = false;
        result.exit_code = 1;
        result.data["error"] = "Failed to generate tests";
        return result;
    }

    result.data["generation_type"] = "tests";
    result.data["full_coverage"] = options.full_coverage ? "true" : "false";
    result.data["target_directory"] = target_dir;
    result.data["status"] = "generated";

    return result;
}

executor::ExecutionResult GenerateCommand::generateUniverse(const GenerateOptions& options,
                                                           const executor::ExecutionContext& context) {
    executor::ExecutionResult result;
    result.success = true;
    result.exit_code = 0;

    std::string target_dir = options.target_directory;
    std::filesystem::create_directories(target_dir);

    // Generate complete universe - projects, frameworks, languages, tools
    
    // Create universe structure
    std::filesystem::create_directories(target_dir + "/projects");
    std::filesystem::create_directories(target_dir + "/frameworks");
    std::filesystem::create_directories(target_dir + "/languages");
    std::filesystem::create_directories(target_dir + "/tools");
    std::filesystem::create_directories(target_dir + "/platforms");

    // Generate sample projects
    GenerateOptions project_opts = options;
    project_opts.generation_type = "project";
    project_opts.language = "cpp";
    project_opts.target_directory = target_dir + "/projects/cpp-project";
    generateProject(project_opts, context);

    project_opts.language = "rust";
    project_opts.target_directory = target_dir + "/projects/rust-project";
    generateProject(project_opts, context);

    // Generate sample framework
    GenerateOptions framework_opts = options;
    framework_opts.generation_type = "framework";
    framework_opts.framework_name = "UniverseFramework";
    framework_opts.target_directory = target_dir + "/frameworks/universe-framework";
    generateFramework(framework_opts, context);

    // Create universe manifest
    std::ofstream manifest(target_dir + "/universe.yaml");
    manifest << "universe:\n";
    manifest << "  name: Generated Universe\n";
    manifest << "  version: 1.0.0\n";
    manifest << "  components:\n";
    manifest << "    projects:\n";
    manifest << "      - cpp-project\n";
    manifest << "      - rust-project\n";
    manifest << "    frameworks:\n";
    manifest << "      - universe-framework\n";
    manifest << "    languages:\n";
    manifest << "      - cpp\n";
    manifest << "      - rust\n";
    manifest.close();

    result.data["generation_type"] = "universe";
    result.data["target_directory"] = target_dir;
    result.data["components"] = "projects, frameworks, languages, tools, platforms";
    result.data["status"] = "generated";

    return result;
}

bool GenerateCommand::createLanguageParser(const std::string& target_dir, const std::string& language) {
    try {
        std::filesystem::create_directories(target_dir + "/parsers");

        std::ofstream parser_hpp(target_dir + "/parsers/" + language + "_parser.hpp");
        parser_hpp << "#pragma once\n\n";
        parser_hpp << "#include <string>\n";
        parser_hpp << "#include <vector>\n\n";
        parser_hpp << "namespace akao::languages::" << language << " {\n\n";
        parser_hpp << "class " << language << "Parser {\n";
        parser_hpp << "public:\n";
        parser_hpp << "    " << language << "Parser();\n";
        parser_hpp << "    ~" << language << "Parser() = default;\n\n";
        parser_hpp << "    bool parseFile(const std::string& file_path);\n";
        parser_hpp << "    std::vector<std::string> getClasses() const;\n";
        parser_hpp << "    std::vector<std::string> getFunctions() const;\n";
        parser_hpp << "};\n\n";
        parser_hpp << "} // namespace akao::languages::" << language << "\n";
        parser_hpp.close();

        std::ofstream parser_cpp(target_dir + "/parsers/" + language + "_parser.cpp");
        parser_cpp << "#include \"" << language << "_parser.hpp\"\n\n";
        parser_cpp << "namespace akao::languages::" << language << " {\n\n";
        parser_cpp << language << "Parser::" << language << "Parser() = default;\n\n";
        parser_cpp << "bool " << language << "Parser::parseFile(const std::string& file_path) {\n";
        parser_cpp << "    // TODO: Implement " << language << " parsing logic\n";
        parser_cpp << "    return true;\n";
        parser_cpp << "}\n\n";
        parser_cpp << "std::vector<std::string> " << language << "Parser::getClasses() const {\n";
        parser_cpp << "    return {};\n";
        parser_cpp << "}\n\n";
        parser_cpp << "std::vector<std::string> " << language << "Parser::getFunctions() const {\n";
        parser_cpp << "    return {};\n";
        parser_cpp << "}\n\n";
        parser_cpp << "} // namespace akao::languages::" << language << "\n";
        parser_cpp.close();

        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool GenerateCommand::createLanguageValidator(const std::string& target_dir, const std::string& language) {
    try {
        std::filesystem::create_directories(target_dir + "/validators");

        std::ofstream validator_hpp(target_dir + "/validators/" + language + "_validator.hpp");
        validator_hpp << "#pragma once\n\n";
        validator_hpp << "#include <string>\n";
        validator_hpp << "#include <vector>\n\n";
        validator_hpp << "namespace akao::languages::" << language << " {\n\n";
        validator_hpp << "struct ValidationResult {\n";
        validator_hpp << "    bool success;\n";
        validator_hpp << "    std::vector<std::string> violations;\n";
        validator_hpp << "};\n\n";
        validator_hpp << "class " << language << "Validator {\n";
        validator_hpp << "public:\n";
        validator_hpp << "    " << language << "Validator();\n";
        validator_hpp << "    ~" << language << "Validator() = default;\n\n";
        validator_hpp << "    ValidationResult validate(const std::string& file_path);\n";
        validator_hpp << "};\n\n";
        validator_hpp << "} // namespace akao::languages::" << language << "\n";
        validator_hpp.close();

        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool GenerateCommand::createFrameworkCore(const std::string& target_dir, const std::string& framework_name) {
    try {
        std::filesystem::create_directories(target_dir + "/core");
        std::filesystem::create_directories(target_dir + "/interfaces");
        std::filesystem::create_directories(target_dir + "/adapters");

        // Create framework manifest
        std::ofstream manifest(target_dir + "/framework.yaml");
        manifest << "framework:\n";
        manifest << "  name: " << framework_name << "\n";
        manifest << "  version: 1.0.0\n";
        manifest << "  type: universal\n";
        manifest << "  philosophy: akao-compliant\n";
        manifest << "  components:\n";
        manifest << "    - core\n";
        manifest << "    - interfaces\n";
        manifest << "    - adapters\n";
        manifest.close();

        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool GenerateCommand::createBuildAdapters(const std::string& target_dir, const std::string& build_system) {
    try {
        std::filesystem::create_directories(target_dir + "/adapters");

        std::ofstream adapter_hpp(target_dir + "/adapters/" + build_system + "_adapter.hpp");
        adapter_hpp << "#pragma once\n\n";
        adapter_hpp << "#include <string>\n\n";
        adapter_hpp << "namespace akao::build::" << build_system << " {\n\n";
        adapter_hpp << "class " << build_system << "Adapter {\n";
        adapter_hpp << "public:\n";
        adapter_hpp << "    " << build_system << "Adapter();\n";
        adapter_hpp << "    ~" << build_system << "Adapter() = default;\n\n";
        adapter_hpp << "    bool configure(const std::string& project_path);\n";
        adapter_hpp << "    bool build(const std::string& mode);\n";
        adapter_hpp << "};\n\n";
        adapter_hpp << "} // namespace akao::build::" << build_system << "\n";
        adapter_hpp.close();

        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool GenerateCommand::generateComprehensiveTests(const std::string& target_dir, const GenerateOptions& options) {
    try {
        std::filesystem::create_directories(target_dir + "/unit");
        std::filesystem::create_directories(target_dir + "/integration");
        std::filesystem::create_directories(target_dir + "/compliance");

        // Create test manifest
        std::ofstream manifest(target_dir + "/test_manifest.yaml");
        manifest << "tests:\n";
        manifest << "  coverage: " << (options.full_coverage ? "full" : "standard") << "\n";
        manifest << "  types:\n";
        manifest << "    - unit\n";
        manifest << "    - integration\n";
        manifest << "    - compliance\n";
        manifest.close();

        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool GenerateCommand::generateDocumentationFromRules(const std::string& target_dir) {
    try {
        std::filesystem::create_directories(target_dir + "/rules");
        std::filesystem::create_directories(target_dir + "/philosophies");

        // Create documentation index
        std::ofstream index(target_dir + "/index.md");
        index << "# Generated Documentation\n\n";
        index << "## Rules Documentation\n\n";
        index << "- [Structure Rules](rules/structure.md)\n";
        index << "- [Interface Rules](rules/interface.md)\n\n";
        index << "## Philosophy Documentation\n\n";
        index << "- [Core Philosophies](philosophies/core.md)\n";
        index.close();

        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::vector<std::string> GenerateCommand::getAvailableGenerationTypes() const {
    return {
        "project",
        "framework", 
        "language-support",
        "build-system",
        "documentation",
        "tests",
        "universe"
    };
}

std::vector<std::string> GenerateCommand::getSupportedLanguages() const {
    return {
        "cpp",
        "rust",
        "python",
        "javascript",
        "go",
        "java",
        "any"
    };
}

std::vector<std::string> GenerateCommand::getSupportedBuildSystems() const {
    return {
        "cmake",
        "make",
        "cargo",
        "npm",
        "gradle",
        "meson"
    };
}

std::string GenerateCommand::getHelp() const {
    return R"(akao generate - Universe Generation

USAGE:
    akao generate <type> [OPTIONS]

DESCRIPTION:
    Generate infinite possibilities - projects, frameworks, languages,
    platforms, and entire universes of software.

GENERATION TYPES:
    project             Generate language-specific project
    framework           Generate new framework with Akao compliance
    language-support    Generate language adapter and parser
    build-system        Generate build system integration
    documentation       Generate docs from rules and philosophies
    tests               Generate comprehensive test suites
    universe            Generate complete software ecosystem

OPTIONS:
    --lang <language>       Target language (cpp, rust, python, javascript, etc.)
    --name <name>          Framework or project name
    --template <path>      Use custom template
    --universe <dir>       Target universe directory
    --type <type>          Build system type (cmake, make, cargo, etc.)
    --coverage full        Generate full test coverage
    --examples             Include usage examples

EXAMPLES:
    akao generate project --lang cpp               # Generate C++ project
    akao generate framework --name MyFramework     # Generate new framework
    akao generate language-support --lang rust     # Generate Rust adapter
    akao generate build-system --type cmake        # Generate CMake integration
    akao generate documentation                    # Generate docs from rules
    akao generate tests --coverage full            # Generate comprehensive tests
    akao generate universe --universe ./my-world   # Generate complete ecosystem

PHILOSOPHY:
    Embodies Universal Genesis - infinite generation capability
    with Akao compliance built-in from the start.
)";
}

std::string GenerateCommand::getUsage() const {
    return "akao generate <type> [OPTIONS]";
}

std::map<std::string, std::string> GenerateCommand::getSupportedOptions() const {
    return {
        {"lang", "Target language (cpp, rust, python, etc.)"},
        {"name", "Framework or project name"},
        {"template", "Custom template path"},
        {"universe", "Target universe directory"},
        {"type", "Build system type"}
    };
}

std::map<std::string, std::string> GenerateCommand::getSupportedFlags() const {
    return {
        {"help", "Show command help"},
        {"examples", "Include usage examples"},
        {"coverage", "Enable full test coverage generation"}
    };
}

} // namespace akao::interfaces::cli::commands
