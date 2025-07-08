/**
 * @file project_manager.cpp
 * @brief Project lifecycle management for Akao runtime environment
 * @details Handles project configuration, dependency management, and build coordination
 * 
 * ID: akao:component:core:engine:runtime:project-manager:v1
 * Scope: Project management, configuration, and build coordination
 * Dependencies: core/engine/runtime/akao_runtime.hpp
 */

#include "project_manager.hpp"
#include "akao_runtime.hpp"
#include "../parser/yaml/parser/v1.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>

namespace akao {
namespace core {
namespace engine {
namespace runtime {

// ProjectManager Implementation

ProjectManager::ProjectManager() 
    : initialized_(false)
    , project_root_()
    , config_()
{
}

ProjectManager::~ProjectManager() = default;

bool ProjectManager::initialize_project(const std::string& project_path) {
    try {
        project_root_ = std::filesystem::canonical(project_path);
        
        // Look for akao.yaml configuration file
        std::string config_path = (project_root_ / "akao.yaml").string();
        if (std::filesystem::exists(config_path)) {
            if (!load_project_config(config_path)) {
                return false;
            }
        } else {
            // Create default configuration
            create_default_config();
        }
        
        // Validate project structure
        if (!validate_project_structure()) {
            return false;
        }
        
        // Initialize dependency resolver
        dependency_resolver_ = std::make_unique<DependencyResolver>(project_root_);
        
        initialized_ = true;
        return true;
        
    } catch (const std::exception& e) {
        last_error_ = "Failed to initialize project: " + std::string(e.what());
        return false;
    }
}

bool ProjectManager::load_project_config(const std::string& config_path) {
    try {
        akao::core::engine::parser::YamlParser parser;
        auto yaml_config = parser.parseFile(config_path);
        
        if (!yaml_config) {
            last_error_ = "Failed to parse YAML configuration file";
            return false;
        }
        
        // Load project information
        if (yaml_config->hasKey("project")) {
            auto project_node = (*yaml_config)["project"];
            if (project_node->hasKey("name")) config_.name = (*project_node)["name"]->asString();
            if (project_node->hasKey("version")) config_.version = (*project_node)["version"]->asString();
            if (project_node->hasKey("description")) config_.description = (*project_node)["description"]->asString();
            if (project_node->hasKey("author")) config_.author = (*project_node)["author"]->asString();
            if (project_node->hasKey("license")) config_.license = (*project_node)["license"]->asString();
        }
        
        // Load dependencies
        if (yaml_config->hasKey("dependencies")) {
            auto deps_node = (*yaml_config)["dependencies"];
            if (deps_node->hasKey("logic_modules")) {
                auto modules_node = (*deps_node)["logic_modules"];
                if (modules_node->isSequence()) {
                    for (const auto& module : modules_node->asSequence()) {
                        config_.dependencies.logic_modules.push_back(module->asString());
                    }
                }
            }
            if (deps_node->hasKey("system_libraries")) {
                auto libs_node = (*deps_node)["system_libraries"];
                if (libs_node->isSequence()) {
                    for (const auto& lib : libs_node->asSequence()) {
                        config_.dependencies.system_libraries.push_back(lib->asString());
                    }
                }
            }
        }
        
        // Load scripts
        if (yaml_config->hasKey("scripts")) {
            auto scripts_node = (*yaml_config)["scripts"];
            if (scripts_node->isMapping()) {
                for (const auto& [key, value] : scripts_node->asMapping()) {
                    config_.scripts[key] = value->asString();
                }
            }
        }
        
        // Load build configuration
        if (yaml_config->hasKey("build")) {
            auto build_node = (*yaml_config)["build"];
            if (build_node->hasKey("target")) config_.build_config.target = (*build_node)["target"]->asString();
            if (build_node->hasKey("optimization")) config_.build_config.optimization = (*build_node)["optimization"]->asString();
            if (build_node->hasKey("output")) config_.build_config.output_directory = (*build_node)["output"]->asString();
        }
        
        // Load environment variables
        if (yaml_config->hasKey("environment")) {
            auto env_node = (*yaml_config)["environment"];
            if (env_node->hasKey("variables")) {
                auto vars_node = (*env_node)["variables"];
                if (vars_node->isMapping()) {
                    for (const auto& [key, value] : vars_node->asMapping()) {
                        config_.environment.variables[key] = value->asString();
                    }
                }
            }
            if (env_node->hasKey("paths")) {
                auto paths_node = (*env_node)["paths"];
                if (paths_node->isSequence()) {
                    for (const auto& path : paths_node->asSequence()) {
                        config_.environment.paths.push_back(path->asString());
                    }
                }
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        last_error_ = "Failed to load project configuration: " + std::string(e.what());
        return false;
    }
}

bool ProjectManager::save_project_config(const std::string& config_path) {
    try {
        auto yaml_config = akao::core::engine::parser::YamlNode::createMapping();
        
        // Save project information
        auto project_node = akao::core::engine::parser::YamlNode::createMapping();
        project_node->setMapping("name", akao::core::engine::parser::YamlNode::createString(config_.name));
        project_node->setMapping("version", akao::core::engine::parser::YamlNode::createString(config_.version));
        project_node->setMapping("description", akao::core::engine::parser::YamlNode::createString(config_.description));
        project_node->setMapping("author", akao::core::engine::parser::YamlNode::createString(config_.author));
        project_node->setMapping("license", akao::core::engine::parser::YamlNode::createString(config_.license));
        yaml_config->setMapping("project", project_node);
        
        // Save dependencies
        auto deps_node = akao::core::engine::parser::YamlNode::createMapping();
        
        auto logic_modules_seq = akao::core::engine::parser::YamlNode::createSequence();
        for (const auto& module : config_.dependencies.logic_modules) {
            logic_modules_seq->addToSequence(akao::core::engine::parser::YamlNode::createString(module));
        }
        deps_node->setMapping("logic_modules", logic_modules_seq);
        
        auto system_libs_seq = akao::core::engine::parser::YamlNode::createSequence();
        for (const auto& lib : config_.dependencies.system_libraries) {
            system_libs_seq->addToSequence(akao::core::engine::parser::YamlNode::createString(lib));
        }
        deps_node->setMapping("system_libraries", system_libs_seq);
        
        yaml_config->setMapping("dependencies", deps_node);
        
        // Save scripts
        auto scripts_node = akao::core::engine::parser::YamlNode::createMapping();
        for (const auto& script : config_.scripts) {
            scripts_node->setMapping(script.first, akao::core::engine::parser::YamlNode::createString(script.second));
        }
        yaml_config->setMapping("scripts", scripts_node);
        
        // Save build configuration
        auto build_node = akao::core::engine::parser::YamlNode::createMapping();
        build_node->setMapping("target", akao::core::engine::parser::YamlNode::createString(config_.build_config.target));
        build_node->setMapping("optimization", akao::core::engine::parser::YamlNode::createString(config_.build_config.optimization));
        build_node->setMapping("output", akao::core::engine::parser::YamlNode::createString(config_.build_config.output_directory));
        yaml_config->setMapping("build", build_node);
        
        // Save environment
        auto env_node = akao::core::engine::parser::YamlNode::createMapping();
        
        auto vars_node = akao::core::engine::parser::YamlNode::createMapping();
        for (const auto& var : config_.environment.variables) {
            vars_node->setMapping(var.first, akao::core::engine::parser::YamlNode::createString(var.second));
        }
        env_node->setMapping("variables", vars_node);
        
        auto paths_seq = akao::core::engine::parser::YamlNode::createSequence();
        for (const auto& path : config_.environment.paths) {
            paths_seq->addToSequence(akao::core::engine::parser::YamlNode::createString(path));
        }
        env_node->setMapping("paths", paths_seq);
        
        yaml_config->setMapping("environment", env_node);
        
        // Write to file
        std::ofstream file(config_path);
        file << yaml_config->toYaml();
        
        return true;
        
    } catch (const std::exception& e) {
        last_error_ = "Failed to save project configuration: " + std::string(e.what());
        return false;
    }
}

ProjectRunResult ProjectManager::run_script(const std::string& script_name, 
                                            const std::vector<std::string>& arguments) {
    ProjectRunResult result;
    
    if (!initialized_) {
        result.success = false;
        result.error_message = "Project manager not initialized";
        return result;
    }
    
    auto script_it = config_.scripts.find(script_name);
    if (script_it == config_.scripts.end()) {
        result.success = false;
        result.error_message = "Script not found: " + script_name;
        return result;
    }
    
    try {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // Resolve script command
        std::string command = resolve_script_command(script_it->second, arguments);
        
        // Set up runtime environment
        AkaoRuntime runtime;
        setup_runtime_environment(runtime);
        
        // Execute script
        RuntimeResult exec_result;
        if (is_akao_file(command)) {
            exec_result = runtime.execute_file(command);
        } else {
            // Execute as system command
            exec_result = execute_system_command(command);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        result.execution_time = std::chrono::duration<double>(end_time - start_time).count();
        
        result.success = exec_result.success;
        result.output = exec_result.output;
        result.error_message = exec_result.error_message;
        result.return_code = exec_result.success ? 0 : 1;
        
    } catch (const std::exception& e) {
        result.success = false;
        result.error_message = "Script execution failed: " + std::string(e.what());
    }
    
    return result;
}

BuildResult ProjectManager::build_project(const BuildOptions& options) {
    BuildResult result;
    
    if (!initialized_) {
        result.success = false;
        result.error_message = "Project manager not initialized";
        return result;
    }
    
    try {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // Discover source files
        std::vector<std::string> source_files = discover_source_files();
        if (source_files.empty()) {
            result.success = false;
            result.error_message = "No .a source files found";
            return result;
        }
        
        // Resolve dependencies
        if (!dependency_resolver_->resolve_dependencies(config_.dependencies)) {
            result.success = false;
            result.error_message = "Dependency resolution failed: " + dependency_resolver_->get_last_error();
            return result;
        }
        
        // Create build directory
        std::filesystem::path build_dir = project_root_ / config_.build_config.output_directory;
        std::filesystem::create_directories(build_dir);
        
        // Compile source files
        CompilationResult comp_result = compile_sources(source_files, build_dir, options);
        if (!comp_result.success) {
            result.success = false;
            result.error_message = comp_result.error_message;
            return result;
        }
        
        // Link executable
        LinkResult link_result = link_executable(comp_result.object_files, build_dir, options);
        if (!link_result.success) {
            result.success = false;
            result.error_message = link_result.error_message;
            return result;
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        result.build_time = std::chrono::duration<double>(end_time - start_time).count();
        
        result.success = true;
        result.output_file = link_result.executable_path;
        result.compiled_files = source_files;
        
    } catch (const std::exception& e) {
        result.success = false;
        result.error_message = "Build failed: " + std::string(e.what());
    }
    
    return result;
}

std::vector<std::string> ProjectManager::discover_source_files() {
    std::vector<std::string> source_files;
    
    try {
        // Look for .a files in src/ directory
        std::filesystem::path src_dir = project_root_ / "src";
        if (std::filesystem::exists(src_dir)) {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(src_dir)) {
                if (entry.path().extension() == ".a") {
                    source_files.push_back(entry.path().string());
                }
            }
        }
        
        // Look for main.a in project root
        std::filesystem::path main_file = project_root_ / "main.a";
        if (std::filesystem::exists(main_file)) {
            source_files.push_back(main_file.string());
        }
        
        // Sort files for consistent build order
        std::sort(source_files.begin(), source_files.end());
        
    } catch (const std::exception& e) {
        last_error_ = "Failed to discover source files: " + std::string(e.what());
    }
    
    return source_files;
}

TestResult ProjectManager::run_tests(const TestOptions& options) {
    TestResult result;
    
    if (!initialized_) {
        result.success = false;
        result.error_message = "Project manager not initialized";
        return result;
    }
    
    try {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // Discover test files
        std::vector<std::string> test_files = discover_test_files(options.pattern);
        if (test_files.empty()) {
            result.success = false;
            result.error_message = "No test files found";
            return result;
        }
        
        // Set up test runtime
        AkaoRuntime runtime;
        runtime.set_execution_mode(ExecutionMode::TESTING);
        setup_runtime_environment(runtime);
        
        // Run tests
        int passed = 0;
        int failed = 0;
        
        for (const auto& test_file : test_files) {
            RuntimeResult test_result = runtime.execute_file(test_file);
            
            TestFileResult file_result;
            file_result.file_path = test_file;
            file_result.success = test_result.success;
            file_result.output = test_result.output;
            file_result.error_message = test_result.error_message;
            file_result.execution_time = test_result.execution_time;
            
            result.test_results.push_back(file_result);
            
            if (test_result.success) {
                passed++;
            } else {
                failed++;
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        result.total_time = std::chrono::duration<double>(end_time - start_time).count();
        
        result.success = (failed == 0);
        result.total_tests = test_files.size();
        result.passed_tests = passed;
        result.failed_tests = failed;
        
    } catch (const std::exception& e) {
        result.success = false;
        result.error_message = "Test execution failed: " + std::string(e.what());
    }
    
    return result;
}

const ProjectConfig& ProjectManager::get_project_config() const {
    return config_;
}

std::string ProjectManager::get_project_root() const {
    return project_root_.string();
}

std::string ProjectManager::get_last_error() const {
    return last_error_;
}

// Private Methods

void ProjectManager::create_default_config() {
    config_.name = project_root_.filename().string();
    config_.version = "1.0.0";
    config_.description = "";
    config_.author = "";
    config_.license = "MIT";
    
    config_.scripts["build"] = "akao build";
    config_.scripts["test"] = "akao test";
    config_.scripts["start"] = "akao main.a";
    
    config_.build_config.target = "native";
    config_.build_config.optimization = "release";
    config_.build_config.output_directory = "build/";
}

bool ProjectManager::validate_project_structure() {
    // Check if project root exists and is accessible
    if (!std::filesystem::exists(project_root_)) {
        last_error_ = "Project root does not exist: " + project_root_.string();
        return false;
    }
    
    // Check for at least one .a file
    bool has_akao_files = false;
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(project_root_)) {
            if (entry.path().extension() == ".a") {
                has_akao_files = true;
                break;
            }
        }
    } catch (const std::exception& e) {
        last_error_ = "Failed to scan project directory: " + std::string(e.what());
        return false;
    }
    
    if (!has_akao_files) {
        last_error_ = "No .a files found in project";
        return false;
    }
    
    return true;
}

std::string ProjectManager::resolve_script_command(const std::string& script, 
                                                   const std::vector<std::string>& arguments) {
    std::string command = script;
    
    // Replace argument placeholders
    for (size_t i = 0; i < arguments.size(); ++i) {
        std::string placeholder = "$" + std::to_string(i + 1);
        size_t pos = command.find(placeholder);
        if (pos != std::string::npos) {
            command.replace(pos, placeholder.length(), arguments[i]);
        }
    }
    
    // Append remaining arguments
    for (const auto& arg : arguments) {
        command += " " + arg;
    }
    
    return command;
}

void ProjectManager::setup_runtime_environment(AkaoRuntime& runtime) {
    // Set environment variables from project config
    for (const auto& var : config_.environment.variables) {
        runtime.set_environment_variable(var.first, var.second);
    }
    
    // Set project-specific variables
    runtime.set_environment_variable("AKAO_PROJECT_ROOT", project_root_.string());
    runtime.set_environment_variable("AKAO_PROJECT_NAME", config_.name);
    runtime.set_environment_variable("AKAO_PROJECT_VERSION", config_.version);
}

bool ProjectManager::is_akao_file(const std::string& command) {
    return command.find(".a") != std::string::npos;
}

RuntimeResult ProjectManager::execute_system_command(const std::string& command) {
    RuntimeResult result;
    
    // This would execute system commands - implementation depends on platform
    // For now, return a placeholder
    result.success = false;
    result.error_message = "System command execution not implemented";
    
    return result;
}

std::vector<std::string> ProjectManager::discover_test_files(const std::string& pattern) {
    std::vector<std::string> test_files;
    
    try {
        // Look for test files in tests/ directory
        std::filesystem::path tests_dir = project_root_ / "tests";
        if (std::filesystem::exists(tests_dir)) {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(tests_dir)) {
                if (entry.path().extension() == ".a") {
                    std::string filename = entry.path().filename().string();
                    if (pattern.empty() || filename.find(pattern) != std::string::npos) {
                        test_files.push_back(entry.path().string());
                    }
                }
            }
        }
        
        // Sort files for consistent execution order
        std::sort(test_files.begin(), test_files.end());
        
    } catch (const std::exception& e) {
        last_error_ = "Failed to discover test files: " + std::string(e.what());
    }
    
    return test_files;
}

CompilationResult ProjectManager::compile_sources(const std::vector<std::string>& source_files,
                                                   const std::filesystem::path& build_dir,
                                                   const BuildOptions& options) {
    CompilationResult result;
    
    // Placeholder implementation - would use the actual .a compiler
    result.success = true;
    
    for (const auto& source_file : source_files) {
        std::filesystem::path obj_file = build_dir / (std::filesystem::path(source_file).stem().string() + ".o");
        result.object_files.push_back(obj_file.string());
    }
    
    return result;
}

LinkResult ProjectManager::link_executable(const std::vector<std::string>& object_files,
                                           const std::filesystem::path& build_dir,
                                           const BuildOptions& options) {
    LinkResult result;
    
    // Placeholder implementation - would use the actual linker
    result.success = true;
    result.executable_path = (build_dir / config_.name).string();
    
    return result;
}

} // namespace runtime
} // namespace engine
} // namespace core
} // namespace akao
