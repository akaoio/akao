/**
 * @id: akao:class:nodes:builtin:reporter:v1
 * @doc: Advanced report generation node providing comprehensive data aggregation, analysis, and multi-format output capabilities. Supports Markdown, HTML, JSON, YAML, and custom template-based reporting with statistical analysis, data visualization elements, and flexible formatting options. Enables sophisticated reporting workflows for validation results, metrics analysis, and system monitoring with zero external dependencies.
 * @specification: Report generator node with multi-format output and data aggregation
 * @scope: Builtin report generation and analysis node
 * @timeline: 2025-07-10
 * @rationale: Enable powerful reporting capabilities in node-based workflows
 * @methodology: Template-based report generation with statistical analysis and multi-format output
 * @references: ["akao:interface:core:foundation:interfaces:inode:v1"]
 */

#pragma once

#include "core/foundation/interfaces/inode/v1.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <variant>
#include <chrono>
#include <sstream>

namespace akao {
namespace nodes {
namespace builtin {

/**
 * Report data structures
 */
struct ReportSection {
    std::string title;
    std::string content;
    std::map<std::string, foundation::types::NodeValue> data;
    std::vector<std::shared_ptr<ReportSection>> subsections;
    
    ReportSection(const std::string& t) : title(t) {}
};

struct ReportMetrics {
    std::map<std::string, double> numeric_metrics;
    std::map<std::string, std::string> string_metrics;
    std::map<std::string, bool> boolean_metrics;
    std::chrono::system_clock::time_point generated_at;
    
    ReportMetrics() : generated_at(std::chrono::system_clock::now()) {}
};

struct ReportTemplate {
    std::string name;
    std::string format; // "markdown", "html", "json", "yaml", "text"
    std::string header_template;
    std::string section_template;
    std::string footer_template;
    std::map<std::string, std::string> placeholders;
};

/**
 * Report generator and formatter
 */
class ReportGenerator {
private:
    std::vector<std::shared_ptr<ReportSection>> sections_;
    ReportMetrics metrics_;
    std::map<std::string, ReportTemplate> templates_;

public:
    ReportGenerator();
    
    // Section management
    void addSection(const std::string& title, const std::string& content = "");
    void addSection(std::shared_ptr<ReportSection> section);
    std::shared_ptr<ReportSection> getSection(const std::string& title);
    void removeSection(const std::string& title);
    
    // Metrics management
    void addMetric(const std::string& name, double value);
    void addMetric(const std::string& name, const std::string& value);
    void addMetric(const std::string& name, bool value);
    void addMetrics(const std::map<std::string, foundation::types::NodeValue>& metrics);
    ReportMetrics getMetrics() const;
    
    // Data aggregation
    void aggregateData(const std::vector<foundation::types::NodeValue>& data);
    void calculateStatistics(const std::string& dataset_name, const std::vector<double>& values);
    void addSummaryStatistics();
    
    // Template management
    void registerTemplate(const ReportTemplate& tmpl);
    void setDefaultTemplates();
    ReportTemplate getTemplate(const std::string& format) const;
    
    // Report generation
    std::string generateReport(const std::string& format = "markdown") const;
    std::string generateMarkdown() const;
    std::string generateHTML() const;
    std::string generateJSON() const;
    std::string generateYAML() const;
    std::string generateText() const;
    
    // Utility methods
    void clear();
    size_t getSectionCount() const;
    size_t getMetricCount() const;
    std::vector<std::string> getSupportedFormats() const;
    
private:
    // Internal generation helpers
    std::string processTemplate(const std::string& tmpl, const std::map<std::string, std::string>& vars) const;
    std::string formatSection(const ReportSection& section, const std::string& format, int depth = 0) const;
    std::string formatMetrics(const std::string& format) const;
    std::string escapeHTML(const std::string& text) const;
    std::string escapeMarkdown(const std::string& text) const;
    std::string getCurrentTimestamp() const;
    std::string formatNumber(double value, int precision = 2) const;
};

/**
 * Reporter node for report generation and data analysis
 */
class ReporterNode : public foundation::interfaces::INode {
private:
    std::string nodeId_;
    std::string nodeType_;
    std::string version_;
    std::string description_;

    // Conversion utilities
    foundation::types::NodeValue reportToNodeValue(const ReportGenerator& report) const;
    ReportGenerator nodeValueToReport(const foundation::types::NodeValue& value) const;

public:
    ReporterNode();
    ~ReporterNode() = default;
    
    // INode interface implementation
    std::string getNodeId() const override;
    std::string getNodeType() const override;
    std::string getVersion() const override;
    std::string getDescription() const override;
    
    foundation::interfaces::ValidationResult validate(const foundation::interfaces::NodeParameters& params) const override;
    foundation::types::ExecutionResult execute(const foundation::interfaces::NodeContext& context,
                                             const foundation::interfaces::NodeParameters& params) override;
    foundation::types::NodeValue getParameterSchema() const override;
    
    // Static utility methods for direct reporting
    static std::string generateReport(const foundation::types::NodeValue& data, const std::string& format = "markdown");
    static std::string generateValidationReport(const std::vector<foundation::types::NodeValue>& violations);
    static std::string generateMetricsReport(const std::map<std::string, foundation::types::NodeValue>& metrics);
    static std::string generateSummaryReport(const std::vector<foundation::types::NodeValue>& sections);
    static foundation::types::NodeValue analyzeData(const std::vector<foundation::types::NodeValue>& data);
    static std::vector<std::string> getSupportedFormats();
    
    // Advanced reporting features
    static std::string generateComparisonReport(const foundation::types::NodeValue& before, 
                                              const foundation::types::NodeValue& after);
    static std::string generateTrendReport(const std::vector<foundation::types::NodeValue>& timeSeriesData);
    static std::string generateExecutionReport(const std::vector<foundation::types::NodeValue>& executionResults);
    
    // Report export utilities
    static bool saveReport(const std::string& content, const std::string& filePath);
    static std::string generateTimestampedFilename(const std::string& baseName, const std::string& format);
};

} // namespace builtin
} // namespace nodes
} // namespace akao