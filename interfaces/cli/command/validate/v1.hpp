#pragma once

#include "../../executor/command_executor.hpp"
#include <string>
#include <vector>
#include <map>

namespace akao::interfaces::cli::commands {

/**
 * @brief Universal validation command - replaces check, trace, self-validate
 * 
 * The central validation engine that works on ANY project type.
 * Implements the Universal Validation philosophy.
 * 
 * Philosophy compliance:
 * - akao:philosophy:validation:universal:v1 - Works on any project
 * - akao:philosophy:measurement:traceability:v1 - Full violation tracing
 * - akao:philosophy:rule:governance:v1 - Self-validation capability
 */
class ValidateCommand {
public:
    ValidateCommand();
    ~ValidateCommand() = default;

    /**
     * @brief Execute validation command
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
    struct ValidationOptions {
        std::string target_path = ".";
        std::vector<std::string> rule_categories;
        std::vector<std::string> specific_rules;
        std::vector<std::string> philosophies;
        bool enable_tracing = false;
        bool enable_fixing = false;
        bool self_validate = false;
        bool dry_run = false;
        std::string export_file;
        bool include_metrics = false;
        bool phase6_integration = false;
    };

    /**
     * @brief Parse command arguments into options
     */
    ValidationOptions parseArguments(const std::vector<std::string>& args);

    /**
     * @brief Validate any project type
     */
    executor::ExecutionResult validateProject(const ValidationOptions& options,
                                             const executor::ExecutionContext& context);

    /**
     * @brief Self-validation of Akao itself
     */
    executor::ExecutionResult selfValidate(const ValidationOptions& options,
                                         const executor::ExecutionContext& context);

    /**
     * @brief Trace specific violation
     */
    executor::ExecutionResult traceViolation(const std::string& violation_id,
                                           const executor::ExecutionContext& context);

    /**
     * @brief Check specific rule categories
     */
    executor::ExecutionResult checkRuleCategories(const ValidationOptions& options,
                                                 const executor::ExecutionContext& context);

    /**
     * @brief Auto-fix violations where possible
     */
    executor::ExecutionResult autoFixViolations(const ValidationOptions& options,
                                               const executor::ExecutionContext& context);

    /**
     * @brief Execute Phase 6 integration testing
     * 
     * Runs complete system integration and final validation
     * as specified in Phase 6 requirements.
     */
    executor::ExecutionResult executePhase6Integration(const ValidationOptions& options,
                                                      const executor::ExecutionContext& context);

    /**
     * @brief Load and validate rules
     */
    std::vector<std::string> loadRuleCategories(const std::vector<std::string>& categories);

    /**
     * @brief Generate violation trace data
     */
    std::map<std::string, std::string> generateTraceData(const std::string& violation_id);

    /**
     * @brief Export validation results
     */
    bool exportResults(const std::string& file_path, const std::map<std::string, std::string>& results);
};

} // namespace akao::interfaces::cli::commands
