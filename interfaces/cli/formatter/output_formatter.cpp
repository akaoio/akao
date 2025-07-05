#include "output_formatter.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <chrono>

namespace akao::interfaces::cli::formatter {

OutputFormatter::OutputFormatter(const FormatOptions& options) : options_(options) {}

void OutputFormatter::setOptions(const FormatOptions& options) {
    options_ = options;
}

const FormatOptions& OutputFormatter::getOptions() const {
    return options_;
}

std::string OutputFormatter::formatValidationResult(const core::engine::validator::ValidationResult& result) {
    auto data = validationResultToMap(result);
    
    if (options_.format == "json") {
        return formatAsJson(data);
    } else if (options_.format == "yaml") {
        return formatAsYaml(data);
    } else if (options_.format == "table") {
        std::vector<std::vector<std::string>> rows;
        std::vector<std::string> headers = {"Property", "Value"};
        
        for (const auto& [key, value] : data) {
            rows.push_back({key, value});
        }
        
        return formatTable(rows, headers);
    } else {
        return formatAsText(data);
    }
}

std::string OutputFormatter::formatComplianceReport(const core::trace::reporter::Report& report) {
    auto data = complianceReportToMap(report);
    
    if (options_.format == "json") {
        return formatAsJson(data);
    } else if (options_.format == "yaml") {
        return formatAsYaml(data);
    } else {
        std::stringstream ss;
        
        ss << createHeader("Compliance Report") << "\n";
        ss << formatAsText(data) << "\n";
        
        // Note: Report struct doesn't have recommendations field in current implementation
        // This would need to be extracted from sections if needed
        
        ss << createFooter("Report generated at " + std::to_string(
            std::chrono::duration_cast<std::chrono::seconds>(
                report.generated_at.time_since_epoch()).count()));
        
        return ss.str();
    }
}

std::string OutputFormatter::formatExecutionResult(const executor::ExecutionResult& result) {
    auto data = executionResultToMap(result);
    
    if (options_.format == "json") {
        return formatAsJson(data);
    } else if (options_.format == "yaml") {
        return formatAsYaml(data);
    } else {
        std::stringstream ss;
        
        if (result.success) {
            ss << formatSuccess("Command executed successfully");
        } else {
            ss << formatError("Command execution failed");
        }
        
        if (!result.output_message.empty()) {
            ss << "\n" << formatInfo(result.output_message);
        }
        
        if (!result.error_message.empty()) {
            ss << "\n" << formatError(result.error_message);
        }
        
        if (options_.include_statistics) {
            ss << "\n" << formatAsText(data);
        }
        
        return ss.str();
    }
}

std::string OutputFormatter::formatViolations(const std::vector<core::engine::validator::Violation>& violations) {
    std::stringstream ss;
    
    if (violations.empty()) {
        ss << formatSuccess("No violations found - project is compliant!");
        return ss.str();
    }
    
    ss << createHeader("Violations (" + std::to_string(violations.size()) + " found)") << "\n";
    
    size_t display_count = std::min(violations.size(), options_.max_violations_display);
    
    for (size_t i = 0; i < display_count; ++i) {
        ss << formatViolation(violations[i]);
        if (i < display_count - 1) {
            ss << "\n" << createSeparator(60, '-') << "\n";
        }
    }
    
    if (violations.size() > options_.max_violations_display) {
        ss << "\n" << formatWarning("... and " + 
                                   std::to_string(violations.size() - options_.max_violations_display) + 
                                   " more violations (use --all to show all)");
    }
    
    return ss.str();
}

std::string OutputFormatter::formatViolation(const core::engine::validator::Violation& violation) {
    std::stringstream ss;
    
    ss << formatSeverity(violation.severity) << " " << formatRuleId(violation.rule_id) << "\n";
    ss << "  Message: " << violation.message << "\n";
    ss << "  File: " << formatFilePath(violation.file_path);
    
    if (violation.line_number > 0) {
        ss << " (line " << violation.line_number << ")";
    }
    
    ss << "\n";
    
    if (!violation.code_context.empty()) {
        ss << "  Context: " << violation.code_context << "\n";
    }
    
    return ss.str();
}

std::string OutputFormatter::formatExecutionStats(const executor::CommandExecutor::ExecutionStats& stats) {
    std::stringstream ss;
    
    ss << createHeader("Execution Statistics") << "\n";
    ss << "Total Commands: " << stats.total_commands_executed << "\n";
    ss << "Successful: " << applyColor(std::to_string(stats.successful_commands), "green") << "\n";
    ss << "Failed: " << applyColor(std::to_string(stats.failed_commands), "red") << "\n";
    ss << "Total Time: " << formatDuration(stats.total_execution_time) << "\n";
    
    if (!stats.command_execution_counts.empty()) {
        ss << "\nCommand Breakdown:\n";
        for (const auto& [command, count] : stats.command_execution_counts) {
            double avg_time = stats.command_execution_times.at(command) / count;
            ss << "  " << padString(command, 15) << ": " 
               << padString(std::to_string(count), 5) << " executions (avg: " 
               << formatDuration(avg_time) << ")\n";
        }
    }
    
    return ss.str();
}

std::string OutputFormatter::formatFileList(const std::vector<std::string>& files) {
    std::stringstream ss;
    
    if (files.empty()) {
        ss << formatInfo("No files to display");
        return ss.str();
    }
    
    ss << createHeader("Files (" + std::to_string(files.size()) + " total)") << "\n";
    
    for (const auto& file : files) {
        ss << "  " << formatFilePath(file) << "\n";
    }
    
    return ss.str();
}

std::string OutputFormatter::formatConfiguration(const std::map<std::string, std::string>& config) {
    if (options_.format == "json") {
        return formatAsJson(config);
    } else if (options_.format == "yaml") {
        return formatAsYaml(config);
    } else {
        return formatAsText(config);
    }
}

std::string OutputFormatter::formatProgress(size_t current, size_t total, const std::string& operation) {
    if (!options_.show_progress) {
        return "";
    }
    
    double percentage = total > 0 ? (static_cast<double>(current) / total) * 100.0 : 0.0;
    std::string progress_bar = utils::createProgressBar(percentage / 100.0, 30);
    
    std::stringstream ss;
    ss << "\r" << operation << " " << progress_bar << " " 
       << std::fixed << std::setprecision(1) << percentage << "% "
       << "(" << current << "/" << total << ")";
    
    return ss.str();
}

std::string OutputFormatter::formatTable(const std::vector<std::vector<std::string>>& rows,
                                        const std::vector<std::string>& headers) {
    if (rows.empty()) {
        return "";
    }
    
    // Calculate column widths
    std::vector<size_t> col_widths;
    size_t num_cols = rows[0].size();
    
    // Initialize with header widths
    if (!headers.empty()) {
        for (size_t i = 0; i < headers.size() && i < num_cols; ++i) {
            col_widths.push_back(headers[i].length());
        }
    } else {
        col_widths.resize(num_cols, 0);
    }
    
    // Find maximum width for each column
    for (const auto& row : rows) {
        for (size_t i = 0; i < row.size() && i < num_cols; ++i) {
            if (i >= col_widths.size()) {
                col_widths.push_back(row[i].length());
            } else {
                col_widths[i] = std::max(col_widths[i], row[i].length());
            }
        }
    }
    
    std::stringstream ss;
    
    // Print headers
    if (!headers.empty()) {
        ss << "|";
        for (size_t i = 0; i < headers.size() && i < num_cols; ++i) {
            ss << " " << padString(headers[i], col_widths[i]) << " |";
        }
        ss << "\n";
        
        // Print separator
        ss << "|";
        for (size_t i = 0; i < headers.size() && i < num_cols; ++i) {
            ss << createSeparator(col_widths[i] + 2, '-') << "|";
        }
        ss << "\n";
    }
    
    // Print rows
    for (const auto& row : rows) {
        ss << "|";
        for (size_t i = 0; i < row.size() && i < num_cols; ++i) {
            ss << " " << padString(row[i], col_widths[i]) << " |";
        }
        ss << "\n";
    }
    
    return ss.str();
}

std::string OutputFormatter::formatSuccess(const std::string& message) {
    if (shouldUseColors()) {
        return applyColor("✓ " + message, "green");
    }
    return "[SUCCESS] " + message;
}

std::string OutputFormatter::formatError(const std::string& message) {
    if (shouldUseColors()) {
        return applyColor("✗ " + message, "red");
    }
    return "[ERROR] " + message;
}

std::string OutputFormatter::formatWarning(const std::string& message) {
    if (shouldUseColors()) {
        return applyColor("⚠ " + message, "yellow");
    }
    return "[WARNING] " + message;
}

std::string OutputFormatter::formatInfo(const std::string& message) {
    if (shouldUseColors()) {
        return applyColor("ℹ " + message, "cyan");
    }
    return "[INFO] " + message;
}

std::string OutputFormatter::formatDebug(const std::string& message) {
    if (shouldUseColors()) {
        return applyColor("🐛 " + message, "magenta");
    }
    return "[DEBUG] " + message;
}

std::string OutputFormatter::formatTimestamp(const std::string& timestamp) {
    if (options_.date_format == "ISO8601") {
        return timestamp; // Assume already in ISO8601 format
    }
    // Could add other date format conversions here
    return timestamp;
}

std::string OutputFormatter::formatFilePath(const std::string& path) {
    if (shouldUseColors()) {
        return applyColor(path, "cyan");
    }
    return path;
}

std::string OutputFormatter::formatRuleId(const std::string& rule_id) {
    if (shouldUseColors()) {
        return applyColor(rule_id, "blue");
    }
    return rule_id;
}

std::string OutputFormatter::formatSeverity(const std::string& severity) {
    std::string color = getSeverityColor(severity);
    std::string formatted = "[" + severity + "]";
    
    if (shouldUseColors()) {
        return applyColor(formatted, color);
    }
    return formatted;
}

std::string OutputFormatter::formatPercentage(double percentage) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << percentage << "%";
    return ss.str();
}

std::string OutputFormatter::formatFileSize(size_t bytes) {
    return utils::bytesToHuman(bytes);
}

std::string OutputFormatter::formatDuration(double seconds) {
    return utils::secondsToHuman(seconds);
}

std::string OutputFormatter::createSeparator(size_t length, char character) {
    return std::string(length, character);
}

std::string OutputFormatter::createHeader(const std::string& title, size_t width) {
    std::string separator = createSeparator(width, '=');
    std::string centered_title = centerString(title, width);
    
    std::stringstream ss;
    ss << separator << "\n";
    ss << centered_title << "\n";
    ss << separator;
    
    return ss.str();
}

std::string OutputFormatter::createFooter(const std::string& summary, size_t width) {
    std::string separator = createSeparator(width, '-');
    
    std::stringstream ss;
    ss << separator;
    if (!summary.empty()) {
        ss << "\n" << centerString(summary, width);
    }
    
    return ss.str();
}

// Format-specific implementations
std::string OutputFormatter::formatAsYaml(const std::map<std::string, std::string>& data) {
    return utils::mapToYaml(data);
}

std::string OutputFormatter::formatAsJson(const std::map<std::string, std::string>& data) {
    return utils::mapToJson(data, options_.compact_mode ? 0 : 2);
}

std::string OutputFormatter::formatAsText(const std::map<std::string, std::string>& data) {
    std::stringstream ss;
    
    for (const auto& [key, value] : data) {
        ss << key << ": " << value << "\n";
    }
    
    return ss.str();
}

// Color and styling helpers
std::string OutputFormatter::applyColor(const std::string& text, const std::string& color) {
    if (!shouldUseColors()) {
        return text;
    }
    
    return utils::getColorCode(color) + text + utils::resetFormat();
}

std::string OutputFormatter::applyBold(const std::string& text) {
    if (!shouldUseColors()) {
        return text;
    }
    
    return "\033[1m" + text + "\033[0m";
}

std::string OutputFormatter::applyItalic(const std::string& text) {
    if (!shouldUseColors()) {
        return text;
    }
    
    return "\033[3m" + text + "\033[0m";
}

std::string OutputFormatter::applyUnderline(const std::string& text) {
    if (!shouldUseColors()) {
        return text;
    }
    
    return "\033[4m" + text + "\033[0m";
}

// Utility functions
std::string OutputFormatter::escapeString(const std::string& input) {
    std::string result;
    for (char c : input) {
        switch (c) {
            case '"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c; break;
        }
    }
    return result;
}

std::string OutputFormatter::truncateString(const std::string& input, size_t max_length) {
    if (input.length() <= max_length) {
        return input;
    }
    
    return input.substr(0, max_length - 3) + "...";
}

std::string OutputFormatter::padString(const std::string& input, size_t width, char pad_char) {
    if (input.length() >= width) {
        return input;
    }
    
    return input + std::string(width - input.length(), pad_char);
}

std::string OutputFormatter::centerString(const std::string& input, size_t width) {
    if (input.length() >= width) {
        return input;
    }
    
    size_t padding = (width - input.length()) / 2;
    return std::string(padding, ' ') + input + std::string(width - input.length() - padding, ' ');
}

std::string OutputFormatter::getSeverityColor(const std::string& severity) {
    if (severity == "error" || severity == "critical") {
        return "red";
    } else if (severity == "warning") {
        return "yellow";
    } else if (severity == "info") {
        return "cyan";
    } else if (severity == "debug") {
        return "magenta";
    }
    return "default";
}

bool OutputFormatter::shouldUseColors() const {
    return options_.colored_output && utils::terminalSupportsColors();
}

bool OutputFormatter::isCompactFormat() const {
    return options_.compact_mode;
}

// Data conversion helpers
std::map<std::string, std::string> OutputFormatter::validationResultToMap(
    const core::engine::validator::ValidationResult& result) {
    
    std::map<std::string, std::string> data;
    
    data["files_processed"] = std::to_string(result.getFilesProcessed());
    data["total_violations"] = std::to_string(result.getViolations().size());
    data["execution_time"] = formatDuration(result.getExecutionTime());
    data["success"] = result.getViolations().empty() ? "true" : "false";
    
    if (options_.include_metadata) {
        auto validation_time = result.getValidationTime();
        auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
            validation_time.time_since_epoch()).count();
        data["timestamp"] = std::to_string(timestamp);
        data["akao_version"] = "1.0.0"; // Could be made configurable
    }
    
    return data;
}

std::map<std::string, std::string> OutputFormatter::complianceReportToMap(
    const core::trace::reporter::Report& report) {
    
    std::map<std::string, std::string> data;
    
    data["project_path"] = report.project_path;
    data["total_violations"] = std::to_string(report.total_violations);
    // Note: compliance_percentage not available in current Report struct
    data["compliance_percentage"] = "0.0"; // Would need to be calculated
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        report.generated_at.time_since_epoch()).count();
    data["timestamp"] = std::to_string(timestamp);
    
    // Note: violation_summary not available in current Report struct
    // Would need to be extracted from traces if needed
    
    return data;
}

std::map<std::string, std::string> OutputFormatter::executionResultToMap(
    const executor::ExecutionResult& result) {
    
    std::map<std::string, std::string> data;
    
    data["success"] = result.success ? "true" : "false";
    data["exit_code"] = std::to_string(result.exit_code);
    data["execution_time"] = formatDuration(result.execution_time_seconds);
    data["files_processed"] = std::to_string(result.files_processed);
    data["violations_found"] = std::to_string(result.violations_found);
    data["violations_fixed"] = std::to_string(result.violations_fixed);
    
    if (!result.output_message.empty()) {
        data["output_message"] = result.output_message;
    }
    
    if (!result.error_message.empty()) {
        data["error_message"] = result.error_message;
    }
    
    return data;
}

// Utility functions implementation
namespace utils {

std::string bytesToHuman(size_t bytes) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    size_t unit_index = 0;
    double size = static_cast<double>(bytes);
    
    while (size >= 1024.0 && unit_index < 4) {
        size /= 1024.0;
        unit_index++;
    }
    
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << size << " " << units[unit_index];
    return ss.str();
}

std::string secondsToHuman(double seconds) {
    if (seconds < 0.001) {
        return std::to_string(static_cast<int>(seconds * 1000000)) + "μs";
    } else if (seconds < 1.0) {
        return std::to_string(static_cast<int>(seconds * 1000)) + "ms";
    } else if (seconds < 60.0) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << seconds << "s";
        return ss.str();
    } else if (seconds < 3600.0) {
        int minutes = static_cast<int>(seconds / 60);
        int secs = static_cast<int>(seconds) % 60;
        return std::to_string(minutes) + "m " + std::to_string(secs) + "s";
    } else {
        int hours = static_cast<int>(seconds / 3600);
        int minutes = static_cast<int>(seconds / 60) % 60;
        return std::to_string(hours) + "h " + std::to_string(minutes) + "m";
    }
}

std::string getColorCode(const std::string& color) {
    if (color == "red") return "\033[31m";
    else if (color == "green") return "\033[32m";
    else if (color == "yellow") return "\033[33m";
    else if (color == "blue") return "\033[34m";
    else if (color == "magenta") return "\033[35m";
    else if (color == "cyan") return "\033[36m";
    else if (color == "white") return "\033[37m";
    else if (color == "bright_red") return "\033[91m";
    else if (color == "bright_green") return "\033[92m";
    else if (color == "bright_yellow") return "\033[93m";
    else if (color == "bright_blue") return "\033[94m";
    else if (color == "bright_magenta") return "\033[95m";
    else if (color == "bright_cyan") return "\033[96m";
    return "\033[0m"; // default/reset
}

std::string resetFormat() {
    return "\033[0m";
}

bool terminalSupportsColors() {
    const char* term = std::getenv("TERM");
    if (!term) return false;
    
    std::string term_str(term);
    return term_str.find("color") != std::string::npos ||
           term_str.find("xterm") != std::string::npos ||
           term_str.find("screen") != std::string::npos ||
           term_str == "linux";
}

size_t getTerminalWidth() {
    // Default to 80 columns
    // In a full implementation, we could use ioctl() or similar
    return 80;
}

std::string wordWrap(const std::string& text, size_t width) {
    std::stringstream ss;
    std::string word;
    size_t current_line_length = 0;
    
    for (char c : text) {
        if (c == ' ' || c == '\n') {
            if (current_line_length + word.length() > width) {
                ss << "\n" << word;
                current_line_length = word.length();
            } else {
                if (current_line_length > 0) ss << " ";
                ss << word;
                current_line_length += word.length() + (current_line_length > 0 ? 1 : 0);
            }
            word.clear();
            
            if (c == '\n') {
                ss << "\n";
                current_line_length = 0;
            }
        } else {
            word += c;
        }
    }
    
    // Handle last word
    if (!word.empty()) {
        if (current_line_length + word.length() > width) {
            ss << "\n" << word;
        } else {
            if (current_line_length > 0) ss << " ";
            ss << word;
        }
    }
    
    return ss.str();
}

std::string createBox(const std::string& content, size_t padding) {
    std::vector<std::string> lines;
    std::stringstream ss(content);
    std::string line;
    
    while (std::getline(ss, line)) {
        lines.push_back(line);
    }
    
    if (lines.empty()) return "";
    
    // Find maximum line width
    size_t max_width = 0;
    for (const auto& line : lines) {
        max_width = std::max(max_width, line.length());
    }
    
    size_t box_width = max_width + 2 * padding + 2; // +2 for borders
    
    std::stringstream result;
    
    // Top border
    result << "┌" << std::string(box_width - 2, '─') << "┐\n";
    
    // Content lines
    for (const auto& line : lines) {
        result << "│" << std::string(padding, ' ') << line 
               << std::string(max_width - line.length() + padding, ' ') << "│\n";
    }
    
    // Bottom border
    result << "└" << std::string(box_width - 2, '─') << "┘";
    
    return result.str();
}

std::string createProgressBar(double percentage, size_t width) {
    size_t filled = static_cast<size_t>(percentage * width);
    size_t empty = width - filled;
    
    return "[" + std::string(filled, '█') + std::string(empty, '░') + "]";
}

std::string sanitizeForFormat(const std::string& text, const std::string& format) {
    if (format == "json") {
        std::string result;
        for (char c : text) {
            switch (c) {
                case '"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default: result += c; break;
            }
        }
        return result;
    } else if (format == "yaml") {
        // Basic YAML escaping
        if (text.find(':') != std::string::npos || 
            text.find('#') != std::string::npos ||
            text.find('\n') != std::string::npos) {
            return "\"" + text + "\"";
        }
    }
    return text;
}

std::string mapToYaml(const std::map<std::string, std::string>& data, size_t indent) {
    std::stringstream ss;
    std::string indent_str(indent, ' ');
    
    for (const auto& [key, value] : data) {
        ss << indent_str << key << ": " << sanitizeForFormat(value, "yaml") << "\n";
    }
    
    return ss.str();
}

std::string mapToJson(const std::map<std::string, std::string>& data, size_t indent) {
    std::stringstream ss;
    std::string indent_str(indent, ' ');
    std::string next_indent_str(indent + 2, ' ');
    
    ss << "{\n";
    
    auto it = data.begin();
    while (it != data.end()) {
        ss << next_indent_str << "\"" << it->first << "\": \"" 
           << sanitizeForFormat(it->second, "json") << "\"";
        
        ++it;
        if (it != data.end()) {
            ss << ",";
        }
        ss << "\n";
    }
    
    ss << indent_str << "}";
    
    return ss.str();
}

std::string generateOutputId() {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    
    std::stringstream ss;
    ss << "akao_output_" << timestamp;
    return ss.str();
}

} // namespace utils

} // namespace akao::interfaces::cli::formatter
