#pragma once

#include "../../../executor/command_executor.hpp"
#include <string>
#include <vector>
#include <map>

namespace akao::interfaces::cli::commands {

/**
 * @brief Universal project initialization command
 * 
 * Creates new Akao-compliant projects of any type with proper structure
 * and configuration. Implements Universal Genesis philosophy.
 * 
 * Philosophy compliance:
 * - akao:philosophy:structure:enforcement:v1 - Proper project structure
 * - akao:philosophy:rule:governance:v1 - Rule-compliant initialization
 * - akao:philosophy:automation:updates:v1 - Automated project setup
 */
class InitCommand {
public:
    InitCommand();
    ~InitCommand() = default;

    /**
     * @brief Execute init command
     */
    executor::ExecutionResult execute(const executor::ExecutionContext& context,
                                    const std::vector<std::string>& args);

    /**
     * @brief Get command help text
     */
    std::string getHelp() const;

    /**
     * @brief Get command usage
     */
    std::string getUsage() const;

    /**
     * @brief Get supported options
     */
    std::map<std::string, std::string> getSupportedOptions() const;

    /**
     * @brief Get supported flags
     */
    std::map<std::string, std::string> getSupportedFlags() const;

private:
    struct InitOptions {
        std::string project_type = "cpp-project";
        std::string target_path = ".";
        std::string template_type = "standard";
        std::string custom_rules;
        std::string philosophy_level = "standard";
        bool interactive = false;
        bool force_overwrite = false;
    };

    /**
     * @brief Parse command arguments
     */
    InitOptions parseArguments(const std::vector<std::string>& args);

    /**
     * @brief Initialize interactive project setup
     */
    executor::ExecutionResult interactiveInit(const InitOptions& options,
                                             const executor::ExecutionContext& context);

    /**
     * @brief Initialize C++ project
     */
    executor::ExecutionResult initCppProject(const InitOptions& options,
                                           const executor::ExecutionContext& context);

    /**
     * @brief Initialize Rust project
     */
    executor::ExecutionResult initRustProject(const InitOptions& options,
                                            const executor::ExecutionContext& context);

    /**
     * @brief Initialize framework project
     */
    executor::ExecutionResult initFramework(const InitOptions& options,
                                          const executor::ExecutionContext& context);

    /**
     * @brief Initialize language adapter
     */
    executor::ExecutionResult initLanguageAdapter(const InitOptions& options,
                                                 const executor::ExecutionContext& context);

    /**
     * @brief Create project structure
     */
    bool createProjectStructure(const std::string& base_path, const std::string& project_type);

    /**
     * @brief Create Akao configuration files
     */
    bool createAkaoConfiguration(const std::string& base_path, const InitOptions& options);

    /**
     * @brief Create build system files
     */
    bool createBuildSystem(const std::string& base_path, const std::string& project_type);

    /**
     * @brief Create initial source files
     */
    bool createSourceFiles(const std::string& base_path, const std::string& project_type);

    /**
     * @brief Create test structure
     */
    bool createTestStructure(const std::string& base_path, const std::string& project_type);

    /**
     * @brief Create documentation structure
     */
    bool createDocumentationStructure(const std::string& base_path);

    /**
     * @brief Validate target directory
     */
    bool validateTargetDirectory(const std::string& path, bool force_overwrite);

    /**
     * @brief Get available project types
     */
    std::vector<std::string> getAvailableProjectTypes() const;

    /**
     * @brief Get available templates
     */
    std::vector<std::string> getAvailableTemplates() const;
};

} // namespace akao::interfaces::cli::commands
