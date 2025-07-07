#pragma once

#include "../../../executor/command_executor.hpp"
#include <string>
#include <vector>
#include <map>

namespace akao::interfaces::cli::commands {

/**
 * @brief Production build system command
 * 
 * Implements comprehensive build capabilities with dual-mode support
 * for both development and production environments.
 * 
 * Philosophy compliance:
 * - akao:philosophy:build:duality:v1 - Supports both dev and prod modes
 * - akao:philosophy:rule:governance:v1 - Self-validating build process
 * - akao:philosophy:measurement:traceability:v1 - Build traceability
 */
class BuildCommand {
public:
    BuildCommand();
    ~BuildCommand() = default;

    /**
     * @brief Execute build command
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
    /**
     * @brief Parse build-specific arguments
     */
    struct BuildOptions {
        std::string mode = "development";  // development or production
        std::string target = "default";
        bool clean = false;
        bool verbose = false;
        std::string output_dir = "build";
        std::vector<std::string> extra_args;
    };

    BuildOptions parseBuildArgs(const std::vector<std::string>& args);
    
    /**
     * @brief Execute development build
     */
    executor::ExecutionResult executeDevBuild(const executor::ExecutionContext& context,
                                             const BuildOptions& options);
    
    /**
     * @brief Execute production build
     */
    executor::ExecutionResult executeProdBuild(const executor::ExecutionContext& context,
                                              const BuildOptions& options);
    
    /**
     * @brief Validate build configuration compliance
     */
    bool validateBuildCompliance(const executor::ExecutionContext& context,
                                const BuildOptions& options);
    
    /**
     * @brief Auto-detect build system type
     */
    std::string detectBuildSystem(const std::string& project_path);
    
    /**
     * @brief Get build help text
     */
    std::string getHelpText() const;
};

} // namespace akao::interfaces::cli::commands
