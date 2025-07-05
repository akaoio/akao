#pragma once

#include "../tracer/violation_tracer.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace akao::core::trace::reporter {

/**
 * @brief Report configuration
 */
struct ReportConfig {
    // Output format settings
    std::string format = "yaml";  // "yaml", "json", "html", "markdown", "csv"
    std::string output_directory = ".akao_reports";
    bool include_stack_traces = true;
    bool include_context_variables = true;
    bool include_rule_chains = true;
    bool include_related_violations = true;
    
    // Content settings
    bool generate_summary = true;
    bool generate_details = true;
    bool generate_statistics = true;
    bool generate_recommendations = true;
    bool include_fix_suggestions = true;
    
    // Filtering settings
    std::vector<std::string> severity_filter;  // empty = all severities
    std::vector<std::string> rule_filter;      // empty = all rules
    std::vector<std::string> philosophy_filter; // empty = all philosophies
    std::vector<std::string> file_filter;      // empty = all files
    
    // Styling (for HTML reports)
    std::string css_theme = "default";
    bool include_charts = true;
    bool include_graphs = true;
    
    // Performance settings
    size_t max_violations_per_report = 1000;
    bool compress_output = false;
};

/**
 * @brief Report section types
 */
enum class ReportSection {
    EXECUTIVE_SUMMARY,
    VIOLATION_DETAILS,
    STATISTICS,
    RULE_ANALYSIS,
    PHILOSOPHY_COMPLIANCE,
    FILE_ANALYSIS,
    TIMELINE,
    RECOMMENDATIONS,
    APPENDIX
};

/**
 * @brief Report data structure
 */
struct Report {
    std::string report_id;
    std::string title;
    std::string description;
    std::string format;
    std::chrono::system_clock::time_point generated_at;
    
    // Metadata
    std::string project_path;
    std::string validation_session_id;
    size_t total_violations = 0;
    size_t total_files_analyzed = 0;
    
    // Content sections
    std::map<ReportSection, std::string> sections;
    
    // Raw data
    std::vector<tracer::ViolationTrace> traces;
    tracer::ViolationTracer::TraceSummary summary;
    
    // Generation metadata
    double generation_time_seconds = 0.0;
    size_t report_size_bytes = 0;
};

/**
 * @brief Universal report generator for Akao framework
 * 
 * Generates comprehensive reports from violation traces in multiple formats.
 * Supports detailed analysis, visualizations, and actionable recommendations.
 * 
 * Philosophy compliance:
 * - akao:philosophy:measurement:observability:v1 - Comprehensive reporting
 * - akao:philosophy:visualization:graph_explainability:v1 - Clear visualizations
 * - akao:philosophy:documentation:generation:v1 - Automated documentation
 */
class ReportGenerator {
public:
    /**
     * @brief Constructor
     */
    explicit ReportGenerator(const ReportConfig& config = ReportConfig{});

    /**
     * @brief Set report configuration
     */
    void setConfig(const ReportConfig& config);

    /**
     * @brief Get current configuration
     */
    const ReportConfig& getConfig() const;

    /**
     * @brief Generate comprehensive report from tracer
     * @param tracer The violation tracer containing traces
     * @param title Report title
     * @return Generated report
     */
    Report generateReport(const tracer::ViolationTracer& tracer, 
                         const std::string& title = "Akao Validation Report");

    /**
     * @brief Generate report from trace collection
     * @param collection The trace collection
     * @param title Report title
     * @return Generated report
     */
    Report generateReport(const tracer::TraceCollection& collection,
                         const std::string& title = "Akao Validation Report");

    /**
     * @brief Generate report from specific traces
     * @param traces Vector of violation traces
     * @param title Report title
     * @return Generated report
     */
    Report generateReport(const std::vector<tracer::ViolationTrace>& traces,
                         const std::string& title = "Akao Validation Report");

    /**
     * @brief Generate executive summary report
     * @param tracer The violation tracer
     * @return Summary report
     */
    Report generateSummaryReport(const tracer::ViolationTracer& tracer);

    /**
     * @brief Generate detailed violation report
     * @param tracer The violation tracer
     * @return Detailed report
     */
    Report generateDetailedReport(const tracer::ViolationTracer& tracer);

    /**
     * @brief Generate statistics report
     * @param tracer The violation tracer
     * @return Statistics report
     */
    Report generateStatisticsReport(const tracer::ViolationTracer& tracer);

    /**
     * @brief Generate compliance report by philosophy
     * @param tracer The violation tracer
     * @return Compliance report
     */
    Report generateComplianceReport(const tracer::ViolationTracer& tracer);

    /**
     * @brief Export report to file
     * @param report The report to export
     * @param output_path Output file path (optional, uses config if empty)
     * @return Success status
     */
    bool exportReport(const Report& report, const std::string& output_path = "");

    /**
     * @brief Generate multiple report formats
     * @param tracer The violation tracer
     * @param formats Vector of formats to generate
     * @return Map of format to report
     */
    std::map<std::string, Report> generateMultiFormatReports(
        const tracer::ViolationTracer& tracer,
        const std::vector<std::string>& formats);

    /**
     * @brief Register custom report template
     * @param name Template name
     * @param template_content Template content
     */
    void registerTemplate(const std::string& name, const std::string& template_content);

    /**
     * @brief Get available report templates
     */
    std::vector<std::string> getAvailableTemplates() const;

    /**
     * @brief Generate report using custom template
     * @param tracer The violation tracer
     * @param template_name Template to use
     * @return Generated report
     */
    Report generateFromTemplate(const tracer::ViolationTracer& tracer,
                               const std::string& template_name);

    /**
     * @brief Initialize report generator
     */
    bool initialize();

    /**
     * @brief Get generation statistics
     */
    struct GenerationStats {
        size_t total_reports_generated = 0;
        size_t reports_by_format[4] = {0}; // yaml, json, html, markdown
        double total_generation_time = 0.0;
        size_t total_output_size_bytes = 0;
    };

    const GenerationStats& getStats() const;

    /**
     * @brief Clear statistics
     */
    void clearStats();

private:
    ReportConfig config_;
    GenerationStats stats_;
    std::map<std::string, std::string> custom_templates_;

    // Core generation methods
    Report createBaseReport(const std::string& title) const;
    void populateReportData(Report& report, const std::vector<tracer::ViolationTrace>& traces);
    
    // Section generators
    std::string generateExecutiveSummary(const Report& report);
    std::string generateViolationDetails(const Report& report);
    std::string generateStatistics(const Report& report);
    std::string generateStatisticsSection(const Report& report);
    std::string generateRuleAnalysis(const Report& report);
    std::string generatePhilosophyCompliance(const Report& report);
    std::string generatePhilosophyComplianceSection(const Report& report);
    std::string generateFileAnalysis(const Report& report);
    std::string generateTimeline(const Report& report);
    std::string generateRecommendations(const Report& report);
    std::string generateRecommendationsSection(const Report& report);
    std::string generateComplianceExecutiveSummary(const Report& report);
    std::string generateAppendix(const Report& report);
    
    // Format-specific generators
    std::string generateYamlReport(const Report& report);
    std::string generateJsonReport(const Report& report);
    std::string generateHtmlReport(const Report& report);
    std::string generateMarkdownReport(const Report& report);
    std::string generateCsvReport(const Report& report);
    
    // HTML helpers
    std::string generateHtmlHeader(const Report& report);
    std::string generateHtmlFooter();
    std::string generateHtmlChart(const std::string& chart_type, 
                                 const std::map<std::string, size_t>& data);
    std::string generateHtmlTable(const std::vector<std::vector<std::string>>& data,
                                 const std::vector<std::string>& headers);
    
    // Markdown helpers
    std::string generateMarkdownTable(const std::vector<std::vector<std::string>>& data,
                                     const std::vector<std::string>& headers);
    std::string generateMarkdownChart(const std::string& chart_type,
                                     const std::map<std::string, size_t>& data);
    
    // Data analysis helpers
    std::map<std::string, size_t> analyzeViolationTrends(const std::vector<tracer::ViolationTrace>& traces);
    std::vector<std::string> generateRecommendationsList(const Report& report);
    std::map<std::string, double> calculateComplianceScores(const std::vector<tracer::ViolationTrace>& traces);
    
    // Filtering helpers
    std::vector<tracer::ViolationTrace> applyFilters(const std::vector<tracer::ViolationTrace>& traces);
    bool passesFilter(const tracer::ViolationTrace& trace);
    
    // Utility methods
    std::string formatTimestamp(const std::chrono::system_clock::time_point& time);
    std::string formatDuration(double seconds);
    std::string formatFileSize(size_t bytes);
    std::string escapeHtml(const std::string& text);
    std::string escapeMarkdown(const std::string& text);
    std::string generateReportId() const;
    
    void updateStats(const Report& report);
};

/**
 * @brief Report template utilities
 */
namespace templates {

/**
 * @brief Get default YAML report template
 */
std::string getDefaultYamlTemplate();

/**
 * @brief Get default HTML report template
 */
std::string getDefaultHtmlTemplate();

/**
 * @brief Get default Markdown report template
 */
std::string getDefaultMarkdownTemplate();

/**
 * @brief Get default executive summary template
 */
std::string getExecutiveSummaryTemplate();

/**
 * @brief Process template with data substitution
 */
std::string processTemplate(const std::string& template_content,
                           const std::map<std::string, std::string>& variables);

} // namespace templates

/**
 * @brief Report visualization utilities
 */
namespace visualization {

/**
 * @brief Generate ASCII bar chart
 */
std::string generateAsciiBarChart(const std::map<std::string, size_t>& data,
                                 size_t max_width = 50);

/**
 * @brief Generate ASCII pie chart representation
 */
std::string generateAsciiPieChart(const std::map<std::string, size_t>& data);

/**
 * @brief Generate HTML chart (Chart.js)
 */
std::string generateHtmlChart(const std::string& chart_id,
                             const std::string& chart_type,
                             const std::map<std::string, size_t>& data);

/**
 * @brief Generate trend analysis
 */
std::string generateTrendAnalysis(const std::vector<tracer::ViolationTrace>& traces);

} // namespace visualization

} // namespace akao::core::trace::reporter
