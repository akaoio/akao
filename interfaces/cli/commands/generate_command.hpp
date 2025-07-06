#pragma once

#include "../executor/command_executor.hpp"
#include <string>
#include <vector>
#include <map>

namespace akao::interfaces::cli::commands {

/**
 * @brief Universe generation command
 * 
 * Generates infinite possibilities - projects, frameworks, languages,
 * platforms, and entire universes of software.
 * 
 * Philosophy compliance:
 * - akao:philosophy:automation:updates:v1 - Automated generation
 * - akao:philosophy:structure:enforcement:v1 - Generated structure compliance
 * - akao:philosophy:documentation:generation:v1 - Auto-generated documentation
 */
class GenerateCommand {
public:
    GenerateCommand();
    ~GenerateCommand() = default;

    /**
     * @brief Execute generate command
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
    struct GenerateOptions {
        std::string generation_type;
        std::string language;
        std::string framework_name;
        std::string template_path;
        std::string target_directory;
        std::string build_system;
        bool full_coverage = false;
        bool include_examples = false;
    };

    /**
     * @brief Parse command arguments
     */
    GenerateOptions parseArguments(const std::vector<std::string>& args);

    /**
     * @brief Generate project
     */
    executor::ExecutionResult generateProject(const GenerateOptions& options,
                                            const executor::ExecutionContext& context);

    /**
     * @brief Generate framework
     */
    executor::ExecutionResult generateFramework(const GenerateOptions& options,
                                               const executor::ExecutionContext& context);

    /**
     * @brief Generate language support
     */
    executor::ExecutionResult generateLanguageSupport(const GenerateOptions& options,
                                                     const executor::ExecutionContext& context);

    /**
     * @brief Generate build system
     */
    executor::ExecutionResult generateBuildSystem(const GenerateOptions& options,
                                                 const executor::ExecutionContext& context);

    /**
     * @brief Generate documentation
     */
    executor::ExecutionResult generateDocumentation(const GenerateOptions& options,
                                                   const executor::ExecutionContext& context);

    /**
     * @brief Generate tests
     */
    executor::ExecutionResult generateTests(const GenerateOptions& options,
                                           const executor::ExecutionContext& context);

    /**
     * @brief Generate universe (complete ecosystem)
     */
    executor::ExecutionResult generateUniverse(const GenerateOptions& options,
                                              const executor::ExecutionContext& context);

    /**
     * @brief Create language parser
     */
    bool createLanguageParser(const std::string& target_dir, const std::string& language);

    /**
     * @brief Create language validator
     */
    bool createLanguageValidator(const std::string& target_dir, const std::string& language);

    /**
     * @brief Create framework core
     */
    bool createFrameworkCore(const std::string& target_dir, const std::string& framework_name);

    /**
     * @brief Create build adapters
     */
    bool createBuildAdapters(const std::string& target_dir, const std::string& build_system);

    /**
     * @brief Generate comprehensive tests
     */
    bool generateComprehensiveTests(const std::string& target_dir, const GenerateOptions& options);

    /**
     * @brief Generate documentation from rules
     */
    bool generateDocumentationFromRules(const std::string& target_dir);

    /**
     * @brief Get available generation types
     */
    std::vector<std::string> getAvailableGenerationTypes() const;

    /**
     * @brief Get supported languages
     */
    std::vector<std::string> getSupportedLanguages() const;

    /**
     * @brief Get supported build systems
     */
    std::vector<std::string> getSupportedBuildSystems() const;
};

} // namespace akao::interfaces::cli::commands
