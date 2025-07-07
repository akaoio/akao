#pragma once

#include "../../../core/engine/validator/validation_result/v1.hpp"
#include "../../../core/trace/reporter/report_generator.hpp"
#include "../executor/command_executor.hpp"
#include <string>
#include <vector>
#include <map>

namespace akao::interfaces::cli::formatter {

/**
 * @brief Output formatting options
 */
struct FormatOptions {
    std::string format = "yaml";           // yaml, json, text, table
    bool colored_output = true;            // Enable ANSI color codes
    bool include_metadata = true;          // Include timestamps, paths, etc.
    bool compact_mode = false;             // Compact output format
    bool include_statistics = true;        // Include execution statistics
    bool show_progress = false;            // Show progress indicators
    size_t max_violations_display = 50;    // Maximum violations to display
    std::string date_format = "ISO8601";   // Date format preference
};

/**
 * @brief Universal output formatter for Akao CLI
 * 
 * Provides consistent formatting across all command outputs,
 * supporting multiple output formats and styling options.
 * 
 * Philosophy compliance:
 * - akao:philosophy:interface:consistency:v1 - Consistent output formatting
 * - akao:philosophy:visualization:graph_explainability:v1 - Clear data presentation
 * - akao:philosophy:measurement:observability:v1 - Observable output metrics
 */
class OutputFormatter {
public:
    /**
     * @brief Constructor with format options
     */
    explicit OutputFormatter(const FormatOptions& options = FormatOptions{});

    /**
     * @brief Set format options
     */
    void setOptions(const FormatOptions& options);

    /**
     * @brief Get current format options
     */
    const FormatOptions& getOptions() const;

    /**
     * @brief Format validation result
     */
    std::string formatValidationResult(const core::engine::validator::ValidationResult& result);

    /**
     * @brief Format compliance report
     */
    std::string formatComplianceReport(const core::trace::reporter::Report& report);

    /**
     * @brief Format command execution result
     */
    std::string formatExecutionResult(const executor::ExecutionResult& result);

    /**
     * @brief Format violation list
     */
    std::string formatViolations(const std::vector<core::engine::validator::Violation>& violations);

    /**
     * @brief Format single violation
     */
    std::string formatViolation(const core::engine::validator::Violation& violation);

    /**
     * @brief Format execution statistics
     */
    std::string formatExecutionStats(const executor::CommandExecutor::ExecutionStats& stats);

    /**
     * @brief Format file list
     */
    std::string formatFileList(const std::vector<std::string>& files);

    /**
     * @brief Format configuration data
     */
    std::string formatConfiguration(const std::map<std::string, std::string>& config);

    /**
     * @brief Format progress indicator
     */
    std::string formatProgress(size_t current, size_t total, const std::string& operation = "");

    /**
     * @brief Format table data
     */
    std::string formatTable(const std::vector<std::vector<std::string>>& rows,
                           const std::vector<std::string>& headers = {});

    /**
     * @brief Format success message
     */
    std::string formatSuccess(const std::string& message);

    /**
     * @brief Format error message
     */
    std::string formatError(const std::string& message);

    /**
     * @brief Format warning message
     */
    std::string formatWarning(const std::string& message);

    /**
     * @brief Format info message
     */
    std::string formatInfo(const std::string& message);

    /**
     * @brief Format debug message
     */
    std::string formatDebug(const std::string& message);

    /**
     * @brief Format timestamp
     */
    std::string formatTimestamp(const std::string& timestamp);

    /**
     * @brief Format file path (with highlighting)
     */
    std::string formatFilePath(const std::string& path);

    /**
     * @brief Format rule ID (with highlighting)
     */
    std::string formatRuleId(const std::string& rule_id);

    /**
     * @brief Format severity level
     */
    std::string formatSeverity(const std::string& severity);

    /**
     * @brief Format percentage value
     */
    std::string formatPercentage(double percentage);

    /**
     * @brief Format file size
     */
    std::string formatFileSize(size_t bytes);

    /**
     * @brief Format duration
     */
    std::string formatDuration(double seconds);

    /**
     * @brief Create horizontal separator
     */
    std::string createSeparator(size_t length = 80, char character = '-');

    /**
     * @brief Create header with title
     */
    std::string createHeader(const std::string& title, size_t width = 80);

    /**
     * @brief Create footer with summary
     */
    std::string createFooter(const std::string& summary = "", size_t width = 80);

private:
    FormatOptions options_;

    // Format-specific implementations
    std::string formatAsYaml(const std::map<std::string, std::string>& data);
    std::string formatAsJson(const std::map<std::string, std::string>& data);
    std::string formatAsText(const std::map<std::string, std::string>& data);

    // Color and styling helpers
    std::string applyColor(const std::string& text, const std::string& color);
    std::string applyBold(const std::string& text);
    std::string applyItalic(const std::string& text);
    std::string applyUnderline(const std::string& text);

    // Utility functions
    std::string escapeString(const std::string& input);
    std::string truncateString(const std::string& input, size_t max_length);
    std::string padString(const std::string& input, size_t width, char pad_char = ' ');
    std::string centerString(const std::string& input, size_t width);
    
    // Severity color mapping
    std::string getSeverityColor(const std::string& severity);
    
    // Format detection
    bool shouldUseColors() const;
    bool isCompactFormat() const;
    
    // Data conversion helpers
    std::map<std::string, std::string> validationResultToMap(
        const core::engine::validator::ValidationResult& result);
    std::map<std::string, std::string> complianceReportToMap(
        const core::trace::reporter::Report& report);
    std::map<std::string, std::string> executionResultToMap(
        const executor::ExecutionResult& result);
};

/**
 * @brief Formatter utility functions
 */
namespace utils {

/**
 * @brief Convert bytes to human-readable format
 */
std::string bytesToHuman(size_t bytes);

/**
 * @brief Convert seconds to human-readable duration
 */
std::string secondsToHuman(double seconds);

/**
 * @brief Get color code for ANSI terminal
 */
std::string getColorCode(const std::string& color);

/**
 * @brief Reset ANSI formatting
 */
std::string resetFormat();

/**
 * @brief Check if terminal supports colors
 */
bool terminalSupportsColors();

/**
 * @brief Get terminal width
 */
size_t getTerminalWidth();

/**
 * @brief Word wrap text to specified width
 */
std::string wordWrap(const std::string& text, size_t width);

/**
 * @brief Create ASCII art box around text
 */
std::string createBox(const std::string& content, size_t padding = 1);

/**
 * @brief Create progress bar
 */
std::string createProgressBar(double percentage, size_t width = 40);

/**
 * @brief Sanitize text for output format
 */
std::string sanitizeForFormat(const std::string& text, const std::string& format);

/**
 * @brief Convert map to YAML string
 */
std::string mapToYaml(const std::map<std::string, std::string>& data, size_t indent = 0);

/**
 * @brief Convert map to JSON string
 */
std::string mapToJson(const std::map<std::string, std::string>& data, size_t indent = 2);

/**
 * @brief Generate unique identifier for output tracking
 */
std::string generateOutputId();

} // namespace utils

} // namespace akao::interfaces::cli::formatter
