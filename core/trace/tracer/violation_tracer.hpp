#pragma once

#include "../../engine/validator/validation_result/v1.hpp"
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <memory>

namespace akao::core::trace::tracer {

/**
 * @brief Trace information for a violation
 */
struct ViolationTrace {
    std::string trace_id;
    std::string violation_id;
    std::string rule_id;
    std::string philosophy_id;
    
    // Context information
    std::string project_path;
    std::string file_path;
    size_t line_number = 0;
    size_t column_number = 0;
    
    // Stack trace information
    std::vector<std::string> call_stack;
    std::vector<std::string> rule_chain;
    std::map<std::string, std::string> context_variables;
    
    // Timing information
    std::chrono::system_clock::time_point detected_at;
    std::chrono::system_clock::time_point traced_at;
    
    // Analysis information
    std::string violation_category;
    std::string violation_severity;
    std::string root_cause;
    std::vector<std::string> related_violations;
    
    // Fix information
    bool auto_fix_available = false;
    std::string suggested_fix;
    std::vector<std::string> fix_commands;
    
    // Metadata
    std::map<std::string, std::string> metadata;
};

/**
 * @brief Trace collection for multiple violations
 */
struct TraceCollection {
    std::string collection_id;
    std::string project_path;
    std::string validation_session_id;
    
    std::vector<ViolationTrace> traces;
    
    // Summary information
    size_t total_violations = 0;
    size_t critical_violations = 0;
    size_t warning_violations = 0;
    size_t info_violations = 0;
    
    // Categorization
    std::map<std::string, size_t> violations_by_rule;
    std::map<std::string, size_t> violations_by_philosophy;
    std::map<std::string, size_t> violations_by_file;
    std::map<std::string, size_t> violations_by_category;
    
    // Timeline
    std::chrono::system_clock::time_point collection_start;
    std::chrono::system_clock::time_point collection_end;
    double collection_duration_seconds = 0.0;
};

/**
 * @brief Configuration for violation tracing
 */
struct TracingConfig {
    bool enabled = true;
    bool capture_stack_trace = true;
    bool capture_context_variables = true;
    bool track_rule_dependencies = true;
    bool analyze_root_causes = true;
    
    // Depth limits
    size_t max_stack_depth = 50;
    size_t max_context_variables = 100;
    size_t max_related_violations = 20;
    
    // Storage settings
    std::string trace_output_directory = ".akao_traces";
    bool persist_traces = true;
    bool compress_traces = false;
    
    // Performance settings
    bool async_tracing = true;
    size_t trace_buffer_size = 1000;
};

/**
 * @brief Universal violation tracer for Akao framework
 * 
 * Implements comprehensive violation tracing that captures complete context
 * for every violation detected. Provides deep introspection into rule
 * execution, philosophy compliance, and violation relationships.
 * 
 * Philosophy compliance:
 * - akao:philosophy:measurement:traceability:v1 - Complete traceability
 * - akao:philosophy:measurement:observability:v1 - Comprehensive observability
 * - akao:philosophy:rule:governance:v1 - Rule execution tracking
 */
class ViolationTracer {
public:
    /**
     * @brief Constructor
     */
    explicit ViolationTracer(const TracingConfig& config = TracingConfig{});

    /**
     * @brief Set tracing configuration
     */
    void setConfig(const TracingConfig& config);

    /**
     * @brief Get current configuration
     */
    const TracingConfig& getConfig() const;

    /**
     * @brief Start a new trace collection session
     * @param project_path Path to the project being validated
     * @param session_id Unique session identifier
     * @return Collection ID
     */
    std::string startTraceCollection(const std::string& project_path, 
                                   const std::string& session_id = "");

    /**
     * @brief End the current trace collection session
     */
    void endTraceCollection();

    /**
     * @brief Trace a single violation
     * @param violation The violation to trace
     * @return Trace ID
     */
    std::string traceViolation(const engine::validator::Violation& violation);

    /**
     * @brief Trace multiple violations
     * @param violations The violations to trace
     * @return Vector of trace IDs
     */
    std::vector<std::string> traceViolations(const std::vector<engine::validator::Violation>& violations);

    /**
     * @brief Add context variable to current trace
     */
    void addContextVariable(const std::string& name, const std::string& value);

    /**
     * @brief Add rule to the current rule execution chain
     */
    void pushRule(const std::string& rule_id);

    /**
     * @brief Remove rule from the current rule execution chain
     */
    void popRule();

    /**
     * @brief Get the current trace collection
     */
    const TraceCollection& getCurrentCollection() const;

    /**
     * @brief Get trace by ID
     */
    ViolationTrace getTrace(const std::string& trace_id) const;

    /**
     * @brief Get all traces for a specific rule
     */
    std::vector<ViolationTrace> getTracesByRule(const std::string& rule_id) const;

    /**
     * @brief Get all traces for a specific philosophy
     */
    std::vector<ViolationTrace> getTracesByPhilosophy(const std::string& philosophy_id) const;

    /**
     * @brief Get all traces for a specific file
     */
    std::vector<ViolationTrace> getTracesByFile(const std::string& file_path) const;

    /**
     * @brief Analyze trace patterns to find root causes
     */
    std::map<std::string, std::vector<std::string>> analyzeRootCauses() const;

    /**
     * @brief Find related violations based on patterns
     */
    std::vector<std::string> findRelatedViolations(const std::string& trace_id) const;

    /**
     * @brief Generate trace summary
     */
    struct TraceSummary {
        size_t total_traces = 0;
        std::map<std::string, size_t> traces_by_severity;
        std::map<std::string, size_t> traces_by_rule;
        std::map<std::string, size_t> traces_by_philosophy;
        std::map<std::string, size_t> traces_by_file;
        std::vector<std::string> most_common_violations;
        std::vector<std::string> critical_issues;
        double average_violations_per_file = 0.0;
    };

    TraceSummary generateSummary() const;

    /**
     * @brief Export traces to file
     * @param output_path Output file path
     * @param format Output format ("yaml", "json", "csv")
     * @return Success status
     */
    bool exportTraces(const std::string& output_path, const std::string& format = "yaml") const;

    /**
     * @brief Import traces from file
     * @param input_path Input file path
     * @return Success status
     */
    bool importTraces(const std::string& input_path);

    /**
     * @brief Clear all traces
     */
    void clearTraces();

    /**
     * @brief Get tracing statistics
     */
    struct TracingStats {
        size_t total_traces_created = 0;
        size_t total_collections = 0;
        double total_tracing_time = 0.0;
        size_t traces_exported = 0;
        size_t traces_imported = 0;
        std::map<std::string, size_t> traces_by_session;
    };

    const TracingStats& getStats() const;

    /**
     * @brief Clear statistics
     */
    void clearStats();

    /**
     * @brief Initialize tracer
     */
    bool initialize();

private:
    TracingConfig config_;
    mutable TracingStats stats_;
    
    // Current collection state
    std::unique_ptr<TraceCollection> current_collection_;
    std::vector<std::string> current_rule_chain_;
    std::map<std::string, std::string> current_context_;
    
    // Trace storage
    std::map<std::string, ViolationTrace> traces_;
    std::vector<std::unique_ptr<TraceCollection>> completed_collections_;
    
    // Trace creation
    ViolationTrace createTrace(const engine::validator::Violation& violation);
    std::string generateTraceId(const engine::validator::Violation& violation);
    std::string generateCollectionId(const std::string& project_path);
    
    // Stack trace capture
    std::vector<std::string> captureStackTrace();
    void captureContextVariables(ViolationTrace& trace);
    
    // Analysis methods
    std::string analyzeViolationCategory(const engine::validator::Violation& violation);
    std::string determineRootCause(const ViolationTrace& trace);
    std::vector<std::string> findPatternMatches(const ViolationTrace& trace) const;
    
    // Persistence
    bool persistTrace(const ViolationTrace& trace);
    bool persistCollection(const TraceCollection& collection);
    std::string getTraceFilePath(const std::string& trace_id) const;
    std::string getCollectionFilePath(const std::string& collection_id) const;
    
    // Utilities
    void updateStats(const ViolationTrace& trace);
    void updateCollectionStats(TraceCollection& collection);
    std::string serializeTrace(const ViolationTrace& trace, const std::string& format) const;
    ViolationTrace deserializeTrace(const std::string& data, const std::string& format) const;
};

/**
 * @brief Utility functions for tracing
 */
namespace utils {

/**
 * @brief Generate unique trace ID
 */
std::string generateUniqueId();

/**
 * @brief Format timestamp for tracing
 */
std::string formatTimestamp(const std::chrono::system_clock::time_point& time);

/**
 * @brief Calculate time difference in seconds
 */
double calculateDuration(const std::chrono::system_clock::time_point& start,
                        const std::chrono::system_clock::time_point& end);

/**
 * @brief Sanitize string for file paths
 */
std::string sanitizeForPath(const std::string& input);

/**
 * @brief Extract function name from stack trace entry
 */
std::string extractFunctionName(const std::string& stack_entry);

/**
 * @brief Check if two violations are related
 */
bool areViolationsRelated(const ViolationTrace& trace1, const ViolationTrace& trace2);

} // namespace utils

} // namespace akao::core::trace::tracer
