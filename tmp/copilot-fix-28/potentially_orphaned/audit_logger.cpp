#include "audit_logger.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <fstream>

namespace akao::core::trace::logger {

AuditLogger::AuditLogger() = default;

void AuditLogger::log(LogLevel level, const std::string& message) {
    log_entries_.push_back(formatLogEntry(level, message));
}

void AuditLogger::logViolation(const std::string& rule_id, const std::string& file_path, const std::string& details) {
    std::string message = "VIOLATION: " + rule_id + " in " + file_path + " - " + details;
    log(LogLevel::Warning, message);
}

void AuditLogger::logValidation(const std::string& target_path, bool success, int violation_count) {
    std::string message = "VALIDATION: " + target_path + " - " + 
                         (success ? "SUCCESS" : "FAILED") + 
                         " (" + std::to_string(violation_count) + " violations)";
    log(success ? LogLevel::Info : LogLevel::Error, message);
}

bool AuditLogger::exportLogs(const std::string& output_path, const std::string& format) {
    // TODO: Implement log export in various formats
    (void)format; // Suppress unused parameter warning
    
    std::ofstream file(output_path);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : log_entries_) {
        file << entry << std::endl;
    }
    
    return true;
}

std::string AuditLogger::getTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string AuditLogger::formatLogEntry(LogLevel level, const std::string& message) const {
    std::string level_str;
    switch (level) {
        case LogLevel::Debug: level_str = "DEBUG"; break;
        case LogLevel::Info: level_str = "INFO"; break;
        case LogLevel::Warning: level_str = "WARNING"; break;
        case LogLevel::Error: level_str = "ERROR"; break;
        case LogLevel::Critical: level_str = "CRITICAL"; break;
    }
    
    return "[" + getTimestamp() + "] [" + level_str + "] " + message;
}

} // namespace akao::core::trace::logger
