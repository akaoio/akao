#pragma once

#include "../interfaces/cli/executor/command_executor.hpp"
#include "../core/engine/logic/pure_logic_engine.hpp"
#include <string>
#include <vector>
#include <map>

namespace akao::interfaces::cli::commands {

/**
 * @brief Pure Logic Engine-based validation command
 * 
 * Routes all validation through the pure logic engine using YAML-defined rules.
 * This completely replaces hardcoded validation logic with data-driven validation.
 * 
 * Philosophy compliance:
 * - akao:philosophy:validation:universal:v1 - Works on any project
 * - akao:philosophy:rule:governance:v1 - Self-validation capability  
 * - Zero hardcoded domain logic - all rules in YAML files
 */
class PureValidateCommand {
public:
    PureValidateCommand();
    ~PureValidateCommand() = default;

    /**
     * @brief Execute validation using pure logic engine
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

private:
    struct ValidationOptions {
        std::string target_path = ".";
        std::vector<std::string> rule_categories;
        std::vector<std::string> philosophies;
        std::vector<std::string> specific_rules;
        bool enable_tracing = false;
        bool enable_fixing = false;
        bool self_validate = false;
        bool dry_run = false;
        bool include_metrics = false;
        std::string export_file;
        bool show_passing = false;
        bool engine_trace = false;
    };

    ValidationOptions parseArguments(const std::vector<std::string>& args);
    
    executor::ExecutionResult validateWithRules(const ValidationOptions& options,
                                               const executor::ExecutionContext& context);
    
    executor::ExecutionResult systemSelfValidation(const ValidationOptions& options,
                                                  const executor::ExecutionContext& context);
    
    executor::ExecutionResult validateSpecificRules(const ValidationOptions& options,
                                                   const executor::ExecutionContext& context);

    std::vector<std::string> discoverRuleFiles(const std::vector<std::string>& categories);
    std::vector<std::string> discoverPhilosophyFiles(const std::vector<std::string>& philosophies);
    
    void formatValidationResults(const std::vector<akao::logic::Value>& results,
                               const executor::ExecutionContext& context,
                               executor::ExecutionResult& result);

    akao::logic::PureLogicEngine engine_;
};

} // namespace akao::interfaces::cli::commands
