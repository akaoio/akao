#ifndef AKAO_CORE_TRACE_LOGGER_AUDIT_LOGGER_HPP
#define AKAO_CORE_TRACE_LOGGER_AUDIT_LOGGER_HPP

#include <string>
#include <vector>

namespace akao::core::trace::logger {

class AuditLogger {
public:
    enum class LogLevel {
        Debug,
        Info,
        Warning,
        Error,
        Critical
    };
    
    AuditLogger();
    
    void log(LogLevel level, const std::string& message);
    void logViolation(const std::string& rule_id, const std::string& file_path, const std::string& details);
    void logValidation(const std::string& target_path, bool success, int violation_count);
    bool exportLogs(const std::string& output_path, const std::string& format = "json");
    
private:
    std::vector<std::string> log_entries_;
    std::string getTimestamp() const;
    std::string formatLogEntry(LogLevel level, const std::string& message) const;
};

} // namespace akao::core::trace::logger

#endif // AKAO_CORE_TRACE_LOGGER_AUDIT_LOGGER_HPP
