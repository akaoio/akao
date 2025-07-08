#pragma once

/**
 * @file project_manager.hpp
 * @brief Project lifecycle management for Akao runtime environment
 * @details Header file for project configuration, dependency management, and build coordination
 * 
 * ID: akao:component:core:engine:runtime:project-manager:v1
 * Scope: Project management, configuration, and build coordination
 * Dependencies: core/engine/runtime/akao_runtime.hpp
 */

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <filesystem>
#include <chrono>

// Forward declarations
namespace akao {
namespace core {
namespace engine {
namespace runtime {
    class AkaoRuntime;
    struct RuntimeResult;
}
}
}
}

namespace akao {
namespace core {
namespace engine {
namespace runtime {

/**
 * @brief Project dependencies configuration
 */
struct ProjectDependencies {
    std::vector<std::string> logic_modules;
    std::vector<std::string> system_libraries;
};

/**
 * @brief Build configuration
 */
struct BuildConfig {
    std::string target = "native";
    std::string optimization = "release";
    std::string output_directory = "build/";
};

/**
 * @brief Environment configuration
 */
struct EnvironmentConfig {
    std::map<std::string, std::string> variables;
    std::vector<std::string> paths;
};

/**
 * @brief Complete project configuration
 */
struct ProjectConfig {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    
    ProjectDependencies dependencies;
    std::map<std::string, std::string> scripts;
    BuildConfig build_config;
    EnvironmentConfig environment;
};

/**
 * @brief Script execution result
 */
struct ProjectRunResult {
    bool success = false;
    std::string output;
    std::string error_message;
    int return_code = 0;
    double execution_time = 0.0;
};

/**
 * @brief Build options
 */
struct BuildOptions {
    std::string target = "native";
    std::string optimization = "release";
    bool verbose = false;
    bool clean_build = false;
};

/**
 * @brief Build result
 */
struct BuildResult {
    bool success = false;
    std::string output_file;
    std::vector<std::string> compiled_files;
    std::string error_message;
    double build_time = 0.0;
};

/**
 * @brief Test options
 */
struct TestOptions {
    std::string pattern = "";
    bool coverage = false;
    bool verbose = false;
    int timeout = 30; // seconds
};

/**
 * @brief Test file result
 */
struct TestFileResult {
    std::string file_path;
    bool success = false;
    std::string output;
    std::string error_message;
    double execution_time = 0.0;
};

/**
 * @brief Test execution result
 */
struct TestResult {
    bool success = false;
    int total_tests = 0;
    int passed_tests = 0;
    int failed_tests = 0;
    std::vector<TestFileResult> test_results;
    std::string error_message;
    double total_time = 0.0;
};

/**
 * @brief Compilation result
 */
struct CompilationResult {
    bool success = false;
    std::vector<std::string> object_files;
    std::string error_message;
};

/**
 * @brief Link result
 */
struct LinkResult {
    bool success = false;
    std::string executable_path;
    std::string error_message;
};

/**
 * @brief Dependency resolver
 */
class DependencyResolver {
public:
    explicit DependencyResolver(const std::filesystem::path& project_root);
    
    bool resolve_dependencies(const ProjectDependencies& dependencies);
    std::string get_last_error() const;
    
private:
    std::filesystem::path project_root_;
    std::string last_error_;
    
    bool resolve_logic_modules(const std::vector<std::string>& modules);
    bool resolve_system_libraries(const std::vector<std::string>& libraries);
};

/**
 * @brief Main project manager class
 */
class ProjectManager {
public:
    ProjectManager();
    ~ProjectManager();
    
    /**
     * @brief Initialize project from directory
     * @param project_path Path to project directory
     * @return True if successful
     */
    bool initialize_project(const std::string& project_path);
    
    /**
     * @brief Load project configuration from file
     * @param config_path Path to akao.yaml file
     * @return True if successful
     */
    bool load_project_config(const std::string& config_path);
    
    /**
     * @brief Save project configuration to file
     * @param config_path Path to akao.yaml file
     * @return True if successful
     */
    bool save_project_config(const std::string& config_path);
    
    /**
     * @brief Run project script
     * @param script_name Name of script to run
     * @param arguments Script arguments
     * @return Execution result
     */
    ProjectRunResult run_script(const std::string& script_name, 
                                const std::vector<std::string>& arguments = {});
    
    /**
     * @brief Build project
     * @param options Build options
     * @return Build result
     */
    BuildResult build_project(const BuildOptions& options = {});
    
    /**
     * @brief Discover source files in project
     * @return List of .a source files
     */
    std::vector<std::string> discover_source_files();
    
    /**
     * @brief Run project tests
     * @param options Test options
     * @return Test result
     */
    TestResult run_tests(const TestOptions& options = {});
    
    /**
     * @brief Get project configuration
     * @return Project configuration
     */
    const ProjectConfig& get_project_config() const;
    
    /**
     * @brief Get project root directory
     * @return Project root path
     */
    std::string get_project_root() const;
    
    /**
     * @brief Get last error message
     * @return Error message
     */
    std::string get_last_error() const;
    
private:
    bool initialized_;
    std::filesystem::path project_root_;
    ProjectConfig config_;
    std::unique_ptr<DependencyResolver> dependency_resolver_;
    std::string last_error_;
    
    void create_default_config();
    bool validate_project_structure();
    
    std::string resolve_script_command(const std::string& script, 
                                       const std::vector<std::string>& arguments);
    void setup_runtime_environment(AkaoRuntime& runtime);
    
    bool is_akao_file(const std::string& command);
    RuntimeResult execute_system_command(const std::string& command);
    
    std::vector<std::string> discover_test_files(const std::string& pattern);
    
    CompilationResult compile_sources(const std::vector<std::string>& source_files,
                                      const std::filesystem::path& build_dir,
                                      const BuildOptions& options);
    
    LinkResult link_executable(const std::vector<std::string>& object_files,
                               const std::filesystem::path& build_dir,
                               const BuildOptions& options);
};

} // namespace runtime
} // namespace engine
} // namespace core
} // namespace akao
