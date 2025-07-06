#pragma once

#include "../executor/command_executor.hpp"
#include <string>
#include <vector>
#include <map>

namespace akao::interfaces::cli::commands {

/**
 * @brief Build command with duality support
 * 
 * Supports building with environment duality (development/production)
 * and multiple build system adapters.
 * 
 * Philosophy compliance:
 * - akao:philosophy:build:duality:v1 - Environment duality support
 * - akao:philosophy:automation:updates:v1 - Automated build processes
 * - akao:philosophy:structure:enforcement:v1 - Build structure validation
 */
class BuildCommand {
public:
    BuildCommand();
    ~BuildCommand() = default;

    executor::ExecutionResult execute(const executor::ExecutionContext& context,
                                    const std::vector<std::string>& args);

    std::string getHelp() const;
    std::string getUsage() const;
    std::map<std::string, std::string> getSupportedOptions() const;
    std::map<std::string, std::string> getSupportedFlags() const;

private:
    struct BuildOptions {
        std::string mode = "development";  // development, production
        std::string target = "default";
        std::string adapter = "auto";      // cmake, make, cargo, npm, auto
        bool watch_mode = false;
        bool validate_before_build = false;
        bool clean_before_build = false;
        std::string output_directory;
    };

    BuildOptions parseArguments(const std::vector<std::string>& args);
    executor::ExecutionResult buildWithAdapter(const BuildOptions& options,
                                              const executor::ExecutionContext& context);
    executor::ExecutionResult buildCMake(const BuildOptions& options,
                                        const executor::ExecutionContext& context);
    executor::ExecutionResult buildMake(const BuildOptions& options,
                                      const executor::ExecutionContext& context);
    executor::ExecutionResult buildCargo(const BuildOptions& options,
                                        const executor::ExecutionContext& context);
    
    std::string detectBuildSystem(const std::string& project_path);
    bool validateProjectBeforeBuild(const std::string& project_path);
};

} // namespace akao::interfaces::cli::commands
