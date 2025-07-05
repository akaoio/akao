#include "report_generator.hpp"
#include <chrono>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <filesystem>
#include <set>

namespace akao::core::trace::reporter {

ReportGenerator::ReportGenerator(const ReportConfig& config) : config_(config) {
    std::filesystem::create_directories(config_.output_directory);
}

void ReportGenerator::setConfig(const ReportConfig& config) {
    config_ = config;
    std::filesystem::create_directories(config_.output_directory);
}

bool ReportGenerator::initialize() {
    try {
        std::filesystem::create_directories(config_.output_directory);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

const ReportConfig& ReportGenerator::getConfig() const {
    return config_;
}

Report ReportGenerator::generateReport(const tracer::ViolationTracer& tracer, 
                                     const std::string& title) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    Report report = createBaseReport(title);
    
    // Get summary and traces from tracer
    auto summary = tracer.generateSummary();
    auto current_collection = tracer.getCurrentCollection();
    
    populateReportData(report, current_collection.traces);
    report.summary = summary;
    
    // Generate sections based on config
    if (config_.generate_summary) {
        report.sections[ReportSection::EXECUTIVE_SUMMARY] = generateExecutiveSummary(report);
    }
    
    if (config_.generate_details) {
        report.sections[ReportSection::VIOLATION_DETAILS] = generateViolationDetails(report);
    }
    
    if (config_.generate_statistics) {
        report.sections[ReportSection::STATISTICS] = generateStatistics(report);
    }
    
    if (config_.generate_recommendations) {
        report.sections[ReportSection::RECOMMENDATIONS] = generateRecommendations(report);
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time);
    report.generation_time_seconds = duration.count();
    
    updateStats(report);
    
    return report;
}

Report ReportGenerator::generateSummaryReport(const tracer::ViolationTracer& tracer) {
    auto original_config = config_;
    config_.generate_details = false;
    config_.generate_statistics = false;
    config_.include_stack_traces = false;
    
    auto report = generateReport(tracer, "Akao Validation Summary");
    
    config_ = original_config;
    return report;
}

bool ReportGenerator::exportReport(const Report& report, const std::string& output_path) {
    try {
        std::string final_path = output_path;
        if (final_path.empty()) {
            final_path = config_.output_directory + "/" + report.report_id + 
                        "_report." + config_.format;
        }
        
        std::string content;
        if (config_.format == "yaml") {
            content = generateYamlReport(report);
        } else if (config_.format == "json") {
            content = generateJsonReport(report);
        } else if (config_.format == "html") {
            content = generateHtmlReport(report);
        } else if (config_.format == "markdown") {
            content = generateMarkdownReport(report);
        } else if (config_.format == "csv") {
            content = generateCsvReport(report);
        }
        
        std::ofstream file(final_path);
        if (file.is_open()) {
            file << content;
            file.close();
            return true;
        }
        
        return false;
        
    } catch (const std::exception&) {
        return false;
    }
}

const ReportGenerator::GenerationStats& ReportGenerator::getStats() const {
    return stats_;
}

void ReportGenerator::clearStats() {
    stats_ = GenerationStats{};
}

// Private methods implementation
Report ReportGenerator::createBaseReport(const std::string& title) const {
    Report report;
    report.report_id = generateReportId();
    report.title = title;
    report.format = config_.format;
    report.generated_at = std::chrono::system_clock::now();
    
    return report;
}

void ReportGenerator::populateReportData(Report& report, const std::vector<tracer::ViolationTrace>& traces) {
    report.traces = applyFilters(traces);
    report.total_violations = report.traces.size();
    
    // Count unique files
    std::set<std::string> unique_files;
    for (const auto& trace : report.traces) {
        unique_files.insert(trace.file_path);
    }
    report.total_files_analyzed = unique_files.size();
}

std::string ReportGenerator::generateExecutiveSummary(const Report& report) {
    std::stringstream ss;
    
    ss << "# Executive Summary\n\n";
    ss << "**Validation Report**: " << report.title << "\n";
    ss << "**Generated**: " << formatTimestamp(report.generated_at) << "\n";
    ss << "**Total Violations**: " << report.total_violations << "\n";
    ss << "**Files Analyzed**: " << report.total_files_analyzed << "\n\n";
    
    // Severity breakdown
    std::map<std::string, size_t> severity_counts;
    for (const auto& trace : report.traces) {
        severity_counts[trace.violation_severity]++;
    }
    
    ss << "## Violation Breakdown\n\n";
    for (const auto& [severity, count] : severity_counts) {
        ss << "- **" << severity << "**: " << count << " violations\n";
    }
    
    return ss.str();
}

std::string ReportGenerator::generateViolationDetails(const Report& report) {
    std::stringstream ss;
    
    ss << "# Violation Details\n\n";
    
    for (const auto& trace : report.traces) {
        ss << "## Violation: " << trace.trace_id << "\n\n";
        ss << "- **Rule**: " << trace.rule_id << "\n";
        ss << "- **File**: " << trace.file_path << "\n";
        ss << "- **Line**: " << trace.line_number << "\n";
        ss << "- **Severity**: " << trace.violation_severity << "\n";
        ss << "- **Category**: " << trace.violation_category << "\n";
        
        if (!trace.suggested_fix.empty()) {
            ss << "- **Suggested Fix**: " << trace.suggested_fix << "\n";
        }
        
        ss << "\n";
    }
    
    return ss.str();
}

std::string ReportGenerator::generateStatistics(const Report& report) {
    std::stringstream ss;
    
    ss << "# Statistics\n\n";
    
    // Rule distribution
    std::map<std::string, size_t> rule_counts;
    for (const auto& trace : report.traces) {
        rule_counts[trace.rule_id]++;
    }
    
    ss << "## Violations by Rule\n\n";
    for (const auto& [rule, count] : rule_counts) {
        ss << "- " << rule << ": " << count << "\n";
    }
    
    return ss.str();
}

std::string ReportGenerator::generateRecommendations(const Report& report) {
    std::stringstream ss;
    
    ss << "# Recommendations\n\n";
    
    auto recommendations = generateRecommendationsList(report);
    for (const auto& recommendation : recommendations) {
        ss << "- " << recommendation << "\n";
    }
    
    return ss.str();
}

std::string ReportGenerator::generateYamlReport(const Report& report) {
    std::stringstream ss;
    
    ss << "report:\n";
    ss << "  id: \"" << report.report_id << "\"\n";
    ss << "  title: \"" << report.title << "\"\n";
    ss << "  generated_at: \"" << formatTimestamp(report.generated_at) << "\"\n";
    ss << "  total_violations: " << report.total_violations << "\n";
    ss << "  total_files_analyzed: " << report.total_files_analyzed << "\n";
    ss << "  generation_time_seconds: " << report.generation_time_seconds << "\n\n";
    
    // Include sections
    for (const auto& [section_type, content] : report.sections) {
        ss << "# " << static_cast<int>(section_type) << "\n";
        ss << content << "\n";
    }
    
    return ss.str();
}

std::string ReportGenerator::generateMarkdownReport(const Report& report) {
    std::stringstream ss;
    
    ss << "# " << report.title << "\n\n";
    ss << "**Report ID**: " << report.report_id << "\n";
    ss << "**Generated**: " << formatTimestamp(report.generated_at) << "\n\n";
    
    // Include all sections
    for (const auto& [section_type, content] : report.sections) {
        ss << content << "\n\n";
    }
    
    return ss.str();
}

std::string ReportGenerator::generateJsonReport(const Report& report) {
    std::stringstream ss;
    
    ss << "{\n";
    ss << "  \"report_id\": \"" << report.report_id << "\",\n";
    ss << "  \"title\": \"" << report.title << "\",\n";
    ss << "  \"generated_at\": \"" << formatTimestamp(report.generated_at) << "\",\n";
    ss << "  \"total_violations\": " << report.total_violations << ",\n";
    ss << "  \"total_files_analyzed\": " << report.total_files_analyzed << ",\n";
    ss << "  \"generation_time_seconds\": " << report.generation_time_seconds << "\n";
    ss << "}\n";
    
    return ss.str();
}

std::string ReportGenerator::generateHtmlReport(const Report& report) {
    std::stringstream ss;
    
    ss << generateHtmlHeader(report);
    
    ss << "<body>\n";
    ss << "<div class=\"container\">\n";
    ss << "<h1>" << escapeHtml(report.title) << "</h1>\n";
    
    // Include sections
    for (const auto& [section_type, content] : report.sections) {
        ss << "<div class=\"section\">\n";
        ss << content << "\n";
        ss << "</div>\n";
    }
    
    ss << "</div>\n";
    ss << generateHtmlFooter();
    ss << "</body>\n</html>\n";
    
    return ss.str();
}

std::string ReportGenerator::generateCsvReport(const Report& report) {
    std::stringstream ss;
    
    // CSV header
    ss << "trace_id,rule_id,file_path,line_number,severity,category,message\n";
    
    // CSV data
    for (const auto& trace : report.traces) {
        ss << "\"" << trace.trace_id << "\","
           << "\"" << trace.rule_id << "\","
           << "\"" << trace.file_path << "\","
           << trace.line_number << ","
           << "\"" << trace.violation_severity << "\","
           << "\"" << trace.violation_category << "\","
           << "\"" << trace.suggested_fix << "\"\n";
    }
    
    return ss.str();
}

std::vector<tracer::ViolationTrace> ReportGenerator::applyFilters(const std::vector<tracer::ViolationTrace>& traces) {
    std::vector<tracer::ViolationTrace> filtered_traces;
    
    for (const auto& trace : traces) {
        if (passesFilter(trace)) {
            filtered_traces.push_back(trace);
        }
    }
    
    return filtered_traces;
}

bool ReportGenerator::passesFilter(const tracer::ViolationTrace& trace) {
    // Apply severity filter
    if (!config_.severity_filter.empty()) {
        if (std::find(config_.severity_filter.begin(), config_.severity_filter.end(), 
                     trace.violation_severity) == config_.severity_filter.end()) {
            return false;
        }
    }
    
    // Apply rule filter
    if (!config_.rule_filter.empty()) {
        if (std::find(config_.rule_filter.begin(), config_.rule_filter.end(), 
                     trace.rule_id) == config_.rule_filter.end()) {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> ReportGenerator::generateRecommendationsList(const Report& report) {
    std::vector<std::string> recommendations;
    
    if (report.total_violations > 0) {
        recommendations.push_back("Address critical violations first");
        recommendations.push_back("Review file structure organization");
        recommendations.push_back("Consider enabling auto-fix for simple violations");
    }
    
    return recommendations;
}

std::string ReportGenerator::generateHtmlHeader(const Report& report) {
    return "<!DOCTYPE html>\n<html>\n<head>\n<title>" + escapeHtml(report.title) + 
           "</title>\n</head>\n";
}

std::string ReportGenerator::generateHtmlFooter() {
    return "<footer><p>Generated by Akao Framework</p></footer>\n";
}

std::string ReportGenerator::formatTimestamp(const std::chrono::system_clock::time_point& time) {
    auto time_t = std::chrono::system_clock::to_time_t(time);
    auto tm = *std::localtime(&time_t);
    
    std::stringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string ReportGenerator::escapeHtml(const std::string& text) {
    std::string result = text;
    std::replace(result.begin(), result.end(), '<', ' ');
    std::replace(result.begin(), result.end(), '>', ' ');
    return result;
}

std::string ReportGenerator::generateReportId() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << "report_" << time_t;
    return ss.str();
}

Report ReportGenerator::generateComplianceReport(const tracer::ViolationTracer& tracer) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    Report report = createBaseReport("Compliance Report");
    
    // Get data from tracer
    auto summary = tracer.generateSummary();
    auto current_collection = tracer.getCurrentCollection();
    
    // Populate basic report data
    report.total_violations = current_collection.total_violations;
    report.summary = summary;
    report.traces = current_collection.traces;
    
    // Generate compliance-focused sections
    report.sections[ReportSection::EXECUTIVE_SUMMARY] = generateComplianceExecutiveSummary(report);
    report.sections[ReportSection::PHILOSOPHY_COMPLIANCE] = generatePhilosophyComplianceSection(report);
    report.sections[ReportSection::STATISTICS] = generateStatisticsSection(report);
    
    if (config_.generate_recommendations) {
        report.sections[ReportSection::RECOMMENDATIONS] = generateRecommendationsSection(report);
    }
    
    // Calculate generation time
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    report.generation_time_seconds = duration.count() / 1000000.0;
    
    updateStats(report);
    return report;
}

std::string ReportGenerator::generateComplianceExecutiveSummary(const Report& report) {
    std::stringstream ss;
    
    double compliance_percentage = 100.0;
    if (report.total_files_analyzed > 0) {
        compliance_percentage = 100.0 * (1.0 - (static_cast<double>(report.total_violations) / report.total_files_analyzed));
    }
    
    ss << "Compliance Score: " << std::fixed << std::setprecision(1) << compliance_percentage << "%\n";
    ss << "Total Violations: " << report.total_violations << "\n";
    ss << "Files Analyzed: " << report.total_files_analyzed << "\n";
    
    if (report.total_violations == 0) {
        ss << "\nProject is fully compliant with all Akao philosophies and rules.\n";
    } else {
        ss << "\nProject requires attention to achieve full compliance.\n";
    }
    
    return ss.str();
}

std::string ReportGenerator::generatePhilosophyComplianceSection(const Report& report) {
    std::stringstream ss;
    ss << "Philosophy Compliance Analysis:\n\n";
    
    // Analyze violations by philosophy
    std::map<std::string, size_t> philosophy_violations;
    for (const auto& trace : report.traces) {
        if (!trace.philosophy_id.empty()) {
            philosophy_violations[trace.philosophy_id]++;
        }
    }
    
    if (philosophy_violations.empty()) {
        ss << "All philosophies are satisfied.\n";
    } else {
        for (const auto& [philosophy, count] : philosophy_violations) {
            ss << "- " << philosophy << ": " << count << " violations\n";
        }
    }
    
    return ss.str();
}

std::string ReportGenerator::generateRecommendationsSection(const Report& report) {
    std::stringstream ss;
    ss << "Recommendations for Improvement:\n\n";
    
    if (report.total_violations == 0) {
        ss << "No recommendations needed - project is fully compliant.\n";
    } else {
        // Collect unique suggestions from traces
        std::set<std::string> unique_recommendations;
        for (const auto& trace : report.traces) {
            if (!trace.suggested_fix.empty()) {
                unique_recommendations.insert(trace.suggested_fix);
            }
        }
        
        size_t counter = 1;
        for (const auto& recommendation : unique_recommendations) {
            ss << counter++ << ". " << recommendation << "\n";
        }
        
        if (unique_recommendations.empty()) {
            ss << "1. Review violations manually to determine appropriate fixes\n";
            ss << "2. Consider updating project structure to align with Akao philosophies\n";
            ss << "3. Run akao fix command to attempt automatic resolution\n";
        }
    }
    
    return ss.str();
}

void ReportGenerator::updateStats(const Report& report) {
    stats_.total_reports_generated++;
    stats_.total_generation_time += report.generation_time_seconds;
}

std::string ReportGenerator::generateStatisticsSection(const Report& report) {
    std::stringstream ss;
    
    ss << "Validation Statistics:\n";
    ss << "  Total Violations: " << report.total_violations << "\n";
    ss << "  Files Analyzed: " << report.total_files_analyzed << "\n";
    
    if (!report.traces.empty()) {
        // Count violations by severity
        std::map<std::string, size_t> severity_counts;
        for (const auto& trace : report.traces) {
            severity_counts[trace.violation_severity]++;
        }
        
        if (!severity_counts.empty()) {
            ss << "  Breakdown by Severity:\n";
            for (const auto& [severity, count] : severity_counts) {
                ss << "    " << severity << ": " << count << "\n";
            }
        }
        
        // Count violations by category
        std::map<std::string, size_t> category_counts;
        for (const auto& trace : report.traces) {
            category_counts[trace.violation_category]++;
        }
        
        if (!category_counts.empty()) {
            ss << "  Breakdown by Category:\n";
            for (const auto& [category, count] : category_counts) {
                ss << "    " << category << ": " << count << "\n";
            }
        }
    }
    
    ss << "  Generation Time: " << report.generation_time_seconds << " seconds\n";
    
    return ss.str();
}

} // namespace akao::core::trace::reporter
