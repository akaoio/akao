#include "violation_tracer.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <random>
#include <iomanip>
#include <filesystem>

namespace akao::core::trace::tracer {

ViolationTracer::ViolationTracer(const TracingConfig& config) : config_(config) {
    if (config_.persist_traces) {
        std::filesystem::create_directories(config_.trace_output_directory);
    }
}

void ViolationTracer::setConfig(const TracingConfig& config) {
    config_ = config;
}

bool ViolationTracer::initialize() {
    try {
        // Initialize tracing system
        stats_ = TracingStats{};
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

const TracingConfig& ViolationTracer::getConfig() const {
    return config_;
}

std::string ViolationTracer::startTraceCollection(const std::string& project_path, 
                                                 const std::string& session_id) {
    current_collection_ = std::make_unique<TraceCollection>();
    current_collection_->collection_id = generateCollectionId(project_path);
    current_collection_->project_path = project_path;
    current_collection_->validation_session_id = session_id.empty() ? 
        utils::generateUniqueId() : session_id;
    current_collection_->collection_start = std::chrono::system_clock::now();
    
    // Clear current state
    current_rule_chain_.clear();
    current_context_.clear();
    
    stats_.total_collections++;
    stats_.traces_by_session[current_collection_->validation_session_id] = 0;
    
    return current_collection_->collection_id;
}

void ViolationTracer::endTraceCollection() {
    if (!current_collection_) {
        return;
    }
    
    current_collection_->collection_end = std::chrono::system_clock::now();
    current_collection_->collection_duration_seconds = utils::calculateDuration(
        current_collection_->collection_start, current_collection_->collection_end);
    
    updateCollectionStats(*current_collection_);
    
    if (config_.persist_traces) {
        persistCollection(*current_collection_);
    }
    
    completed_collections_.push_back(std::move(current_collection_));
    current_collection_.reset();
}

std::string ViolationTracer::traceViolation(const engine::validator::Violation& violation) {
    if (!config_.enabled) {
        return "";
    }
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    ViolationTrace trace = createTrace(violation);
    
    // Store trace
    traces_[trace.trace_id] = trace;
    
    // Add to current collection if active
    if (current_collection_) {
        current_collection_->traces.push_back(trace);
        current_collection_->total_violations++;
        
        // Update categorization
        current_collection_->violations_by_rule[violation.rule_id]++;
        current_collection_->violations_by_philosophy[violation.philosophy_id]++;
        current_collection_->violations_by_file[violation.file_path]++;
        current_collection_->violations_by_category[trace.violation_category]++;
        
        // Update severity counts
        if (violation.severity == "error" || violation.severity == "critical") {
            current_collection_->critical_violations++;
        } else if (violation.severity == "warning") {
            current_collection_->warning_violations++;
        } else {
            current_collection_->info_violations++;
        }
        
        stats_.traces_by_session[current_collection_->validation_session_id]++;
    }
    
    // Persist trace if configured
    if (config_.persist_traces) {
        persistTrace(trace);
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time);
    stats_.total_tracing_time += duration.count();
    
    updateStats(trace);
    
    return trace.trace_id;
}

std::vector<std::string> ViolationTracer::traceViolations(const std::vector<engine::validator::Violation>& violations) {
    std::vector<std::string> trace_ids;
    trace_ids.reserve(violations.size());
    
    for (const auto& violation : violations) {
        trace_ids.push_back(traceViolation(violation));
    }
    
    return trace_ids;
}

void ViolationTracer::addContextVariable(const std::string& name, const std::string& value) {
    if (config_.capture_context_variables && 
        current_context_.size() < config_.max_context_variables) {
        current_context_[name] = value;
    }
}

void ViolationTracer::pushRule(const std::string& rule_id) {
    if (config_.track_rule_dependencies && 
        current_rule_chain_.size() < config_.max_stack_depth) {
        current_rule_chain_.push_back(rule_id);
    }
}

void ViolationTracer::popRule() {
    if (!current_rule_chain_.empty()) {
        current_rule_chain_.pop_back();
    }
}

const TraceCollection& ViolationTracer::getCurrentCollection() const {
    static TraceCollection empty_collection;
    return current_collection_ ? *current_collection_ : empty_collection;
}

ViolationTrace ViolationTracer::getTrace(const std::string& trace_id) const {
    auto it = traces_.find(trace_id);
    if (it != traces_.end()) {
        return it->second;
    }
    return ViolationTrace{};
}

std::vector<ViolationTrace> ViolationTracer::getTracesByRule(const std::string& rule_id) const {
    std::vector<ViolationTrace> matching_traces;
    
    for (const auto& [id, trace] : traces_) {
        if (trace.rule_id == rule_id) {
            matching_traces.push_back(trace);
        }
    }
    
    return matching_traces;
}

std::vector<ViolationTrace> ViolationTracer::getTracesByPhilosophy(const std::string& philosophy_id) const {
    std::vector<ViolationTrace> matching_traces;
    
    for (const auto& [id, trace] : traces_) {
        if (trace.philosophy_id == philosophy_id) {
            matching_traces.push_back(trace);
        }
    }
    
    return matching_traces;
}

std::vector<ViolationTrace> ViolationTracer::getTracesByFile(const std::string& file_path) const {
    std::vector<ViolationTrace> matching_traces;
    
    for (const auto& [id, trace] : traces_) {
        if (trace.file_path == file_path) {
            matching_traces.push_back(trace);
        }
    }
    
    return matching_traces;
}

std::map<std::string, std::vector<std::string>> ViolationTracer::analyzeRootCauses() const {
    std::map<std::string, std::vector<std::string>> root_causes;
    
    for (const auto& [id, trace] : traces_) {
        if (!trace.root_cause.empty()) {
            root_causes[trace.root_cause].push_back(trace.trace_id);
        }
    }
    
    return root_causes;
}

std::vector<std::string> ViolationTracer::findRelatedViolations(const std::string& trace_id) const {
    auto target_trace_it = traces_.find(trace_id);
    if (target_trace_it == traces_.end()) {
        return {};
    }
    
    const auto& target_trace = target_trace_it->second;
    std::vector<std::string> related_traces;
    
    for (const auto& [id, trace] : traces_) {
        if (id != trace_id && utils::areViolationsRelated(target_trace, trace)) {
            related_traces.push_back(id);
        }
    }
    
    return related_traces;
}

ViolationTracer::TraceSummary ViolationTracer::generateSummary() const {
    TraceSummary summary;
    summary.total_traces = traces_.size();
    
    std::map<std::string, size_t> file_violation_counts;
    std::map<std::string, size_t> violation_frequency;
    
    for (const auto& [id, trace] : traces_) {
        // Count by severity
        summary.traces_by_severity[trace.violation_severity]++;
        
        // Count by rule
        summary.traces_by_rule[trace.rule_id]++;
        
        // Count by philosophy
        summary.traces_by_philosophy[trace.philosophy_id]++;
        
        // Count by file
        summary.traces_by_file[trace.file_path]++;
        file_violation_counts[trace.file_path]++;
        
        // Track violation frequency
        std::string violation_key = trace.rule_id + ":" + trace.violation_category;
        violation_frequency[violation_key]++;
    }
    
    // Calculate average violations per file
    if (!file_violation_counts.empty()) {
        size_t total_violations = 0;
        for (const auto& [file, count] : file_violation_counts) {
            total_violations += count;
        }
        summary.average_violations_per_file = 
            static_cast<double>(total_violations) / file_violation_counts.size();
    }
    
    // Find most common violations
    std::vector<std::pair<std::string, size_t>> sorted_violations(
        violation_frequency.begin(), violation_frequency.end());
    
    std::sort(sorted_violations.begin(), sorted_violations.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    for (size_t i = 0; i < std::min(size_t(10), sorted_violations.size()); ++i) {
        summary.most_common_violations.push_back(sorted_violations[i].first);
    }
    
    // Find critical issues
    for (const auto& [id, trace] : traces_) {
        if (trace.violation_severity == "error" || trace.violation_severity == "critical") {
            summary.critical_issues.push_back(trace.trace_id);
        }
    }
    
    return summary;
}

bool ViolationTracer::exportTraces(const std::string& output_path, const std::string& format) const {
    try {
        std::ofstream output(output_path);
        if (!output.is_open()) {
            return false;
        }
        
        if (format == "yaml") {
            output << "traces:\n";
            for (const auto& [id, trace] : traces_) {
                output << "  - trace_id: \"" << trace.trace_id << "\"\n";
                output << "    violation_id: \"" << trace.violation_id << "\"\n";
                output << "    rule_id: \"" << trace.rule_id << "\"\n";
                output << "    philosophy_id: \"" << trace.philosophy_id << "\"\n";
                output << "    file_path: \"" << trace.file_path << "\"\n";
                output << "    line_number: " << trace.line_number << "\n";
                output << "    violation_category: \"" << trace.violation_category << "\"\n";
                output << "    violation_severity: \"" << trace.violation_severity << "\"\n";
                output << "    root_cause: \"" << trace.root_cause << "\"\n";
                output << "    suggested_fix: \"" << trace.suggested_fix << "\"\n";
                output << "    detected_at: \"" << utils::formatTimestamp(trace.detected_at) << "\"\n";
                output << "    traced_at: \"" << utils::formatTimestamp(trace.traced_at) << "\"\n";
                
                if (!trace.call_stack.empty()) {
                    output << "    call_stack:\n";
                    for (const auto& frame : trace.call_stack) {
                        output << "      - \"" << frame << "\"\n";
                    }
                }
                
                if (!trace.rule_chain.empty()) {
                    output << "    rule_chain:\n";
                    for (const auto& rule : trace.rule_chain) {
                        output << "      - \"" << rule << "\"\n";
                    }
                }
                
                output << "\n";
            }
        } else if (format == "csv") {
            // CSV header
            output << "trace_id,violation_id,rule_id,philosophy_id,file_path,line_number,"
                   << "violation_category,violation_severity,root_cause,suggested_fix,"
                   << "detected_at,traced_at\n";
            
            // CSV data
            for (const auto& [id, trace] : traces_) {
                output << "\"" << trace.trace_id << "\","
                       << "\"" << trace.violation_id << "\","
                       << "\"" << trace.rule_id << "\","
                       << "\"" << trace.philosophy_id << "\","
                       << "\"" << trace.file_path << "\","
                       << trace.line_number << ","
                       << "\"" << trace.violation_category << "\","
                       << "\"" << trace.violation_severity << "\","
                       << "\"" << trace.root_cause << "\","
                       << "\"" << trace.suggested_fix << "\","
                       << "\"" << utils::formatTimestamp(trace.detected_at) << "\","
                       << "\"" << utils::formatTimestamp(trace.traced_at) << "\"\n";
            }
        }
        
        output.close();
        stats_.traces_exported += traces_.size();
        return true;
        
    } catch (const std::exception&) {
        return false;
    }
}

bool ViolationTracer::importTraces(const std::string& input_path) {
    // Simplified import implementation
    // In a real implementation, this would parse YAML/JSON/CSV files
    stats_.traces_imported++;
    return true;
}

void ViolationTracer::clearTraces() {
    traces_.clear();
    completed_collections_.clear();
    current_collection_.reset();
    current_rule_chain_.clear();
    current_context_.clear();
}

const ViolationTracer::TracingStats& ViolationTracer::getStats() const {
    return stats_;
}

void ViolationTracer::clearStats() {
    stats_ = TracingStats{};
}

// Private methods implementation
ViolationTrace ViolationTracer::createTrace(const engine::validator::Violation& violation) {
    ViolationTrace trace;
    
    trace.trace_id = generateTraceId(violation);
    trace.violation_id = violation.id;
    trace.rule_id = violation.rule_id;
    trace.philosophy_id = violation.philosophy_id;
    trace.project_path = current_collection_ ? current_collection_->project_path : "";
    trace.file_path = violation.file_path;
    trace.line_number = violation.line_number;
    trace.column_number = violation.column_number;
    trace.detected_at = violation.detected_at;
    trace.traced_at = std::chrono::system_clock::now();
    trace.violation_category = analyzeViolationCategory(violation);
    trace.violation_severity = violation.severity;
    trace.auto_fix_available = violation.auto_fix_available;
    trace.suggested_fix = violation.suggestion;
    
    // Capture call stack if enabled
    if (config_.capture_stack_trace) {
        trace.call_stack = captureStackTrace();
    }
    
    // Copy current rule chain
    if (config_.track_rule_dependencies) {
        trace.rule_chain = current_rule_chain_;
    }
    
    // Capture context variables
    if (config_.capture_context_variables) {
        captureContextVariables(trace);
    }
    
    // Analyze root cause if enabled
    if (config_.analyze_root_causes) {
        trace.root_cause = determineRootCause(trace);
    }
    
    // Find related violations
    trace.related_violations = findPatternMatches(trace);
    
    return trace;
}

std::string ViolationTracer::generateTraceId(const engine::validator::Violation& violation) {
    return "trace:" + violation.rule_id + ":" + 
           std::filesystem::path(violation.file_path).filename().string() + ":" +
           std::to_string(violation.line_number) + ":" + utils::generateUniqueId();
}

std::string ViolationTracer::generateCollectionId(const std::string& project_path) {
    return "collection:" + utils::sanitizeForPath(project_path) + ":" + utils::generateUniqueId();
}

std::vector<std::string> ViolationTracer::captureStackTrace() {
    std::vector<std::string> stack_trace;
    
    // Simplified stack trace capture
    // In a real implementation, this would use platform-specific APIs
    stack_trace.push_back("ViolationTracer::traceViolation");
    stack_trace.push_back("UniversalValidator::executeValidation");
    stack_trace.push_back("RuleExecutor::executeRule");
    
    return stack_trace;
}

void ViolationTracer::captureContextVariables(ViolationTrace& trace) {
    trace.context_variables = current_context_;
    
    // Add some automatic context variables
    trace.context_variables["project_path"] = trace.project_path;
    trace.context_variables["validation_time"] = utils::formatTimestamp(trace.traced_at);
    
    if (current_collection_) {
        trace.context_variables["session_id"] = current_collection_->validation_session_id;
        trace.context_variables["collection_id"] = current_collection_->collection_id;
    }
}

std::string ViolationTracer::analyzeViolationCategory(const engine::validator::Violation& violation) {
    if (violation.rule_category == "structure") {
        return "structural";
    } else if (violation.rule_category == "interface") {
        return "interface";
    } else if (violation.rule_category == "language") {
        return "language";
    } else if (violation.rule_category == "security") {
        return "security";
    } else if (violation.rule_category == "testing") {
        return "testing";
    }
    
    return "general";
}

std::string ViolationTracer::determineRootCause(const ViolationTrace& trace) {
    // Simplified root cause analysis
    if (trace.violation_category == "structural") {
        return "Project structure does not follow Akao conventions";
    } else if (trace.violation_category == "interface") {
        return "Interface design violates consistency rules";
    } else if (trace.violation_category == "language") {
        return "Language usage violates isolation principles";
    }
    
    return "Unknown root cause";
}

std::vector<std::string> ViolationTracer::findPatternMatches(const ViolationTrace& trace) const {
    std::vector<std::string> related_traces;
    
    // Look for violations in the same file
    for (const auto& [id, other_trace] : traces_) {
        if (other_trace.file_path == trace.file_path && 
            other_trace.trace_id != trace.trace_id) {
            related_traces.push_back(other_trace.trace_id);
        }
    }
    
    return related_traces;
}

bool ViolationTracer::persistTrace(const ViolationTrace& trace) {
    if (!config_.persist_traces) {
        return true;
    }
    
    try {
        std::string trace_file = getTraceFilePath(trace.trace_id);
        std::ofstream file(trace_file);
        
        if (file.is_open()) {
            file << serializeTrace(trace, "yaml");
            file.close();
            return true;
        }
        
        return false;
        
    } catch (const std::exception&) {
        return false;
    }
}

bool ViolationTracer::persistCollection(const TraceCollection& collection) {
    if (!config_.persist_traces) {
        return true;
    }
    
    try {
        std::string collection_file = getCollectionFilePath(collection.collection_id);
        std::ofstream file(collection_file);
        
        if (file.is_open()) {
            file << "collection_id: \"" << collection.collection_id << "\"\n";
            file << "project_path: \"" << collection.project_path << "\"\n";
            file << "validation_session_id: \"" << collection.validation_session_id << "\"\n";
            file << "total_violations: " << collection.total_violations << "\n";
            file << "critical_violations: " << collection.critical_violations << "\n";
            file << "warning_violations: " << collection.warning_violations << "\n";
            file << "info_violations: " << collection.info_violations << "\n";
            file << "collection_duration_seconds: " << collection.collection_duration_seconds << "\n";
            
            file.close();
            return true;
        }
        
        return false;
        
    } catch (const std::exception&) {
        return false;
    }
}

std::string ViolationTracer::getTraceFilePath(const std::string& trace_id) const {
    return config_.trace_output_directory + "/" + utils::sanitizeForPath(trace_id) + ".yaml";
}

std::string ViolationTracer::getCollectionFilePath(const std::string& collection_id) const {
    return config_.trace_output_directory + "/" + utils::sanitizeForPath(collection_id) + "_collection.yaml";
}

void ViolationTracer::updateStats(const ViolationTrace& trace) {
    stats_.total_traces_created++;
}

void ViolationTracer::updateCollectionStats(TraceCollection& collection) {
    // Stats are already updated in real-time during trace creation
}

std::string ViolationTracer::serializeTrace(const ViolationTrace& trace, const std::string& format) const {
    std::stringstream ss;
    
    if (format == "yaml") {
        ss << "trace_id: \"" << trace.trace_id << "\"\n";
        ss << "violation_id: \"" << trace.violation_id << "\"\n";
        ss << "rule_id: \"" << trace.rule_id << "\"\n";
        ss << "philosophy_id: \"" << trace.philosophy_id << "\"\n";
        ss << "file_path: \"" << trace.file_path << "\"\n";
        ss << "line_number: " << trace.line_number << "\n";
        ss << "violation_category: \"" << trace.violation_category << "\"\n";
        ss << "violation_severity: \"" << trace.violation_severity << "\"\n";
        ss << "root_cause: \"" << trace.root_cause << "\"\n";
        ss << "suggested_fix: \"" << trace.suggested_fix << "\"\n";
        ss << "detected_at: \"" << utils::formatTimestamp(trace.detected_at) << "\"\n";
        ss << "traced_at: \"" << utils::formatTimestamp(trace.traced_at) << "\"\n";
    }
    
    return ss.str();
}

ViolationTrace ViolationTracer::deserializeTrace(const std::string& data, const std::string& format) const {
    // Simplified deserialization
    return ViolationTrace{};
}

// Utility functions implementation
namespace utils {

std::string generateUniqueId() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    
    std::string id;
    id.reserve(16);
    
    for (int i = 0; i < 16; ++i) {
        int val = dis(gen);
        id += (val < 10) ? ('0' + val) : ('a' + val - 10);
    }
    
    return id;
}

std::string formatTimestamp(const std::chrono::system_clock::time_point& time) {
    auto time_t = std::chrono::system_clock::to_time_t(time);
    auto tm = *std::gmtime(&time_t);
    
    std::stringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    return ss.str();
}

double calculateDuration(const std::chrono::system_clock::time_point& start,
                        const std::chrono::system_clock::time_point& end) {
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
    return duration.count();
}

std::string sanitizeForPath(const std::string& input) {
    std::string result = input;
    
    // Replace problematic characters with underscores
    std::replace_if(result.begin(), result.end(), 
                   [](char c) { return c == '/' || c == '\\' || c == ':' || c == '*' || 
                               c == '?' || c == '"' || c == '<' || c == '>' || c == '|'; }, 
                   '_');
    
    return result;
}

std::string extractFunctionName(const std::string& stack_entry) {
    // Simplified function name extraction
    size_t start = stack_entry.find_last_of(':');
    if (start != std::string::npos) {
        return stack_entry.substr(start + 1);
    }
    return stack_entry;
}

bool areViolationsRelated(const ViolationTrace& trace1, const ViolationTrace& trace2) {
    // Check if violations are in the same file
    if (trace1.file_path == trace2.file_path) {
        return true;
    }
    
    // Check if violations have the same rule ID
    if (trace1.rule_id == trace2.rule_id) {
        return true;
    }
    
    // Check if violations have the same philosophy ID
    if (trace1.philosophy_id == trace2.philosophy_id) {
        return true;
    }
    
    // Check if violations have the same root cause
    if (!trace1.root_cause.empty() && trace1.root_cause == trace2.root_cause) {
        return true;
    }
    
    return false;
}

} // namespace utils

} // namespace akao::core::trace::tracer
