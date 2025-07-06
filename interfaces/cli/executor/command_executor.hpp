#pragma once

#include "../parser/command_parser.hpp"
#include "../../../core/engine/validator/universal_validator.hpp"
#include "../../../core/filesystem/enforcer/structure_enforcer.hpp"
#include "../../../core/trace/tracer/violation_tracer.hpp"
#include "../../../core/trace/reporter/report_generator.hpp"
#include <memory>

namespace akao::interfaces::cli::executor {

/**
 * @brief Execution context for CLI commands
 */
struct ExecutionContext {
    std::string current_directory;
    std::string config_file_path;
    std::map<std::string, std::string> global_config;
    bool verbose_mode = false;
    bool debug_mode = false;
    bool quiet_mode = false;
    std::string output_format = "yaml";
};

/**
 * @brief Command execution result
 */
struct ExecutionResult {
    int exit_code = 0;
    std::string output_message;
    std::string error_message;
    bool success = true;
    
    // Performance metrics
    double execution_time_seconds = 0.0;
    size_t files_processed = 0;
    size_t violations_found = 0;
    size_t violations_fixed = 0;
};

/**
 * @brief Universal command executor for Akao CLI
 * 
 * Implements execution logic for all Akao CLI commands, providing
 * consistent interface and error handling across the framework.
 * 
 * Philosophy compliance:
 * - akao:philosophy:interface:consistency:v1 - Consistent command interface
 * - akao:philosophy:automation:updates:v1 - Automated command execution
 * - akao:philosophy:measurement:observability:v1 - Command execution metrics
 */
class CommandExecutor {
public:
    /**
     * @brief Constructor
     */
    CommandExecutor();

    /**
     * @brief Initialize executor with components
     */
    bool initialize();

    /**
     * @brief Set execution context
     */
    void setContext(const ExecutionContext& context);

    /**
     * @brief Get current execution context
     */
    const ExecutionContext& getContext() const;

    /**
     * @brief Execute parsed command
     * @param parse_result Parsed command from CommandParser
     * @return Execution result
     */
    ExecutionResult execute(const parser::ParseResult& parse_result);

    /**
     * @brief Execute validate command
     */
    ExecutionResult executeValidate(const std::map<std::string, std::string>& options,
                                   const std::map<std::string, bool>& flags,
                                   const std::vector<std::string>& args);

    /**
     * @brief Execute init command
     */
    ExecutionResult executeInit(const std::map<std::string, std::string>& options,
                               const std::map<std::string, bool>& flags,
                               const std::vector<std::string>& args);

    /**
     * @brief Execute generate command
     */
    ExecutionResult executeGenerate(const std::map<std::string, std::string>& options,
                                   const std::map<std::string, bool>& flags,
                                   const std::vector<std::string>& args);

    /**
     * @brief Execute check command
     */
    ExecutionResult executeCheck(const std::map<std::string, std::string>& options,
                                const std::map<std::string, bool>& flags,
                                const std::vector<std::string>& args);

    /**
     * @brief Execute trace command
     */
    ExecutionResult executeTrace(const std::map<std::string, std::string>& options,
                                const std::map<std::string, bool>& flags,
                                const std::vector<std::string>& args);

    /**
     * @brief Execute report command
     */
    ExecutionResult executeReport(const std::map<std::string, std::string>& options,
                                 const std::map<std::string, bool>& flags,
                                 const std::vector<std::string>& args);

    /**
     * @brief Execute fix command
     */
    ExecutionResult executeFix(const std::map<std::string, std::string>& options,
                              const std::map<std::string, bool>& flags,
                              const std::vector<std::string>& args);

    /**
     * @brief Execute config command
     */
    ExecutionResult executeConfig(const std::map<std::string, std::string>& options,
                                 const std::map<std::string, bool>& flags,
                                 const std::vector<std::string>& args);

    /**
     * @brief Execute status command
     */
    ExecutionResult executeStatus(const std::map<std::string, std::string>& options,
                                 const std::map<std::string, bool>& flags,
                                 const std::vector<std::string>& args);

    /**
     * @brief Get execution statistics
     */
    struct ExecutionStats {
        size_t total_commands_executed = 0;
        size_t successful_commands = 0;
        size_t failed_commands = 0;
        double total_execution_time = 0.0;
        std::map<std::string, size_t> command_execution_counts;
        std::map<std::string, double> command_execution_times;
    };

    const ExecutionStats& getStats() const;

    /**
     * @brief Clear execution statistics
     */
    void clearStats();

private:
    ExecutionContext context_;
    ExecutionStats stats_;

    // Core components
    std::unique_ptr<core::engine::validator::UniversalValidator> validator_;
    std::unique_ptr<core::filesystem::enforcer::StructureEnforcer> enforcer_;
    std::unique_ptr<core::trace::tracer::ViolationTracer> tracer_;
    std::unique_ptr<core::trace::reporter::ReportGenerator> reporter_;

    // Command handlers map
    std::map<std::string, std::function<ExecutionResult(const std::map<std::string, std::string>&,
                                                        const std::map<std::string, bool>&,
                                                        const std::vector<std::string>&)>> command_handlers_;

    // Helper methods
    ExecutionResult createSuccessResult(const std::string& message = "");
    ExecutionResult createErrorResult(const std::string& error, int exit_code = 1);
    
    void logExecution(const std::string& command, const ExecutionResult& result);
    void updateStats(const std::string& command, const ExecutionResult& result);
    
    // Validation helpers
    std::string resolveTargetPath(const std::vector<std::string>& args);
    std::string resolveOutputPath(const std::map<std::string, std::string>& options);
    std::string resolveOutputFormat(const std::map<std::string, std::string>& options);
    
    // Configuration helpers
    bool loadConfiguration(const std::string& config_path);
    void applyGlobalOptions(const std::map<std::string, std::string>& options);
    void applyGlobalFlags(const std::map<std::string, bool>& flags);
    
    // Output formatting
    std::string formatValidationResult(const core::engine::validator::ValidationResult& result);
    std::string formatComplianceReport(const core::trace::reporter::Report& report);
    std::string formatExecutionSummary(const ExecutionResult& result);
    
    // Error handling
    void handleValidationError(const std::exception& e, ExecutionResult& result);
    void handleFileSystemError(const std::exception& e, ExecutionResult& result);
    void handleConfigurationError(const std::exception& e, ExecutionResult& result);
};

/**
 * @brief CLI utility functions
 */
namespace utils {

/**
 * @brief Print colored output (if terminal supports it)
 */
void printColored(const std::string& message, const std::string& color = "default");

/**
 * @brief Print success message
 */
void printSuccess(const std::string& message);

/**
 * @brief Print error message
 */
void printError(const std::string& message);

/**
 * @brief Print warning message
 */
void printWarning(const std::string& message);

/**
 * @brief Print info message
 */
void printInfo(const std::string& message);

/**
 * @brief Print debug message (only if debug mode enabled)
 */
void printDebug(const std::string& message, bool debug_enabled);

/**
 * @brief Format execution time
 */
std::string formatExecutionTime(double seconds);

/**
 * @brief Format file count
 */
std::string formatFileCount(size_t count);

/**
 * @brief Format violation count with severity
 */
std::string formatViolationCount(size_t count, const std::string& severity);

/**
 * @brief Check if output should be JSON format
 */
bool isJsonOutput(const std::string& format);

/**
 * @brief Check if output should be YAML format
 */
bool isYamlOutput(const std::string& format);

/**
 * @brief Check if terminal supports colors
 */
bool supportsColors();

} // namespace utils

} // namespace akao::interfaces::cli::executor
