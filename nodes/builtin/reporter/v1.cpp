/**
 * @id: akao:class:nodes:builtin:reporter:v1
 * @doc: Advanced report generation node implementation providing comprehensive data aggregation, analysis, and multi-format output capabilities. Supports Markdown, HTML, JSON, YAML, and custom template-based reporting with statistical analysis, data visualization elements, and flexible formatting options. Enables sophisticated reporting workflows for validation results, metrics analysis, and system monitoring with zero external dependencies.
 * @specification: Report generator node implementation with multi-format output and data aggregation
 * @scope: Builtin report generation and analysis node implementation
 * @timeline: 2025-07-10
 * @rationale: Enable powerful reporting capabilities in node-based workflows
 * @methodology: Template-based report generation with statistical analysis and multi-format output
 * @references: ["akao:interface:core:foundation:interfaces:inode:v1"]
 */

#include "v1.hpp"
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <fstream>
#include <cmath>

namespace akao {
namespace nodes {
namespace builtin {

// =============================================================================
// ReportGenerator Implementation
// =============================================================================

ReportGenerator::ReportGenerator() {
    setDefaultTemplates();
}

void ReportGenerator::addSection(const std::string& title, const std::string& content) {
    auto section = std::make_shared<ReportSection>(title);
    section->content = content;
    sections_.push_back(section);
}

void ReportGenerator::addSection(std::shared_ptr<ReportSection> section) {
    sections_.push_back(section);
}

std::shared_ptr<ReportSection> ReportGenerator::getSection(const std::string& title) {
    auto it = std::find_if(sections_.begin(), sections_.end(), 
        [&title](const std::shared_ptr<ReportSection>& section) {
            return section->title == title;
        });
    return it != sections_.end() ? *it : nullptr;
}

void ReportGenerator::removeSection(const std::string& title) {
    sections_.erase(
        std::remove_if(sections_.begin(), sections_.end(),
            [&title](const std::shared_ptr<ReportSection>& section) {
                return section->title == title;
            }),
        sections_.end());
}

void ReportGenerator::addMetric(const std::string& name, double value) {
    metrics_.numeric_metrics[name] = value;
}

void ReportGenerator::addMetric(const std::string& name, const std::string& value) {
    metrics_.string_metrics[name] = value;
}

void ReportGenerator::addMetric(const std::string& name, bool value) {
    metrics_.boolean_metrics[name] = value;
}

void ReportGenerator::addMetrics(const std::map<std::string, foundation::types::NodeValue>& metrics) {
    for (const auto& [name, value] : metrics) {
        if (value.isDouble()) {
            addMetric(name, value.asDouble());
        } else if (value.isInteger()) {
            addMetric(name, static_cast<double>(value.asInteger()));
        } else if (value.isBoolean()) {
            addMetric(name, value.asBoolean());
        } else if (value.isString()) {
            addMetric(name, value.asString());
        }
    }
}

ReportMetrics ReportGenerator::getMetrics() const {
    return metrics_;
}

void ReportGenerator::aggregateData(const std::vector<foundation::types::NodeValue>& data) {
    addMetric("total_items", static_cast<double>(data.size()));
    
    size_t strings = 0, numbers = 0, booleans = 0, arrays = 0, objects = 0;
    for (const auto& item : data) {
        if (item.isString()) strings++;
        else if (item.isInteger() || item.isDouble()) numbers++;
        else if (item.isBoolean()) booleans++;
        else if (item.isArray()) arrays++;
        else if (item.isObject()) objects++;
    }
    
    addMetric("string_count", static_cast<double>(strings));
    addMetric("number_count", static_cast<double>(numbers));
    addMetric("boolean_count", static_cast<double>(booleans));
    addMetric("array_count", static_cast<double>(arrays));
    addMetric("object_count", static_cast<double>(objects));
}

void ReportGenerator::calculateStatistics(const std::string& dataset_name, const std::vector<double>& values) {
    if (values.empty()) return;
    
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    double mean = sum / values.size();
    
    addMetric(dataset_name + "_count", static_cast<double>(values.size()));
    addMetric(dataset_name + "_sum", sum);
    addMetric(dataset_name + "_mean", mean);
    addMetric(dataset_name + "_min", *std::min_element(values.begin(), values.end()));
    addMetric(dataset_name + "_max", *std::max_element(values.begin(), values.end()));
    
    // Calculate standard deviation
    double variance = 0.0;
    for (double value : values) {
        variance += (value - mean) * (value - mean);
    }
    variance /= values.size();
    addMetric(dataset_name + "_std_dev", std::sqrt(variance));
    
    // Calculate median
    std::vector<double> sorted_values = values;
    std::sort(sorted_values.begin(), sorted_values.end());
    double median;
    if (sorted_values.size() % 2 == 0) {
        median = (sorted_values[sorted_values.size()/2 - 1] + sorted_values[sorted_values.size()/2]) / 2.0;
    } else {
        median = sorted_values[sorted_values.size()/2];
    }
    addMetric(dataset_name + "_median", median);
}

void ReportGenerator::addSummaryStatistics() {
    addMetric("sections_count", static_cast<double>(sections_.size()));
    addMetric("metrics_count", static_cast<double>(getMetricCount()));
    addMetric("generated_at", getCurrentTimestamp());
}

void ReportGenerator::setDefaultTemplates() {
    // Markdown template
    ReportTemplate mdTemplate;
    mdTemplate.name = "markdown";
    mdTemplate.format = "markdown";
    mdTemplate.header_template = "# {{title}}\n\n*Generated on {{timestamp}}*\n\n";
    mdTemplate.section_template = "{{indent}}## {{title}}\n\n{{content}}\n\n";
    mdTemplate.footer_template = "\n---\n*Report generated by Akao Reporter Node*\n";
    templates_["markdown"] = mdTemplate;
    
    // HTML template
    ReportTemplate htmlTemplate;
    htmlTemplate.name = "html";
    htmlTemplate.format = "html";
    htmlTemplate.header_template = "<!DOCTYPE html><html><head><title>{{title}}</title></head><body><h1>{{title}}</h1><p><em>Generated on {{timestamp}}</em></p>";
    htmlTemplate.section_template = "{{indent}}<h{{level}}>{{title}}</h{{level}}><div>{{content}}</div>";
    htmlTemplate.footer_template = "<hr><p><em>Report generated by Akao Reporter Node</em></p></body></html>";
    templates_["html"] = htmlTemplate;
    
    // Text template
    ReportTemplate textTemplate;
    textTemplate.name = "text";
    textTemplate.format = "text";
    textTemplate.header_template = "{{title}}\n{{underline}}\n\nGenerated on {{timestamp}}\n\n";
    textTemplate.section_template = "{{indent}}{{title}}\n{{indent}}{{section_underline}}\n\n{{content}}\n\n";
    textTemplate.footer_template = "\n" + std::string(50, '-') + "\nReport generated by Akao Reporter Node\n";
    templates_["text"] = textTemplate;
}

void ReportGenerator::registerTemplate(const ReportTemplate& tmpl) {
    templates_[tmpl.format] = tmpl;
}

ReportTemplate ReportGenerator::getTemplate(const std::string& format) const {
    auto it = templates_.find(format);
    return it != templates_.end() ? it->second : templates_.at("markdown");
}

std::string ReportGenerator::generateReport(const std::string& format) const {
    if (format == "markdown") return generateMarkdown();
    else if (format == "html") return generateHTML();
    else if (format == "json") return generateJSON();
    else if (format == "yaml") return generateYAML();
    else if (format == "text") return generateText();
    else return generateMarkdown(); // Default fallback
}

std::string ReportGenerator::generateMarkdown() const {
    std::ostringstream oss;
    
    oss << "# Report\n\n";
    oss << "*Generated on " << getCurrentTimestamp() << "*\n\n";
    
    // Metrics section
    if (getMetricCount() > 0) {
        oss << "## Metrics\n\n";
        oss << formatMetrics("markdown");
        oss << "\n";
    }
    
    // Sections
    for (const auto& section : sections_) {
        oss << formatSection(*section, "markdown");
    }
    
    oss << "\n---\n*Report generated by Akao Reporter Node*\n";
    return oss.str();
}

std::string ReportGenerator::generateHTML() const {
    std::ostringstream oss;
    
    oss << "<!DOCTYPE html>\n<html>\n<head>\n";
    oss << "<title>Report</title>\n";
    oss << "<style>body{font-family:Arial,sans-serif;margin:40px;} table{border-collapse:collapse;width:100%;} th,td{border:1px solid #ddd;padding:8px;text-align:left;} th{background-color:#f2f2f2;}</style>\n";
    oss << "</head>\n<body>\n";
    oss << "<h1>Report</h1>\n";
    oss << "<p><em>Generated on " << escapeHTML(getCurrentTimestamp()) << "</em></p>\n\n";
    
    // Metrics section
    if (getMetricCount() > 0) {
        oss << "<h2>Metrics</h2>\n";
        oss << formatMetrics("html");
        oss << "\n";
    }
    
    // Sections
    for (const auto& section : sections_) {
        oss << formatSection(*section, "html");
    }
    
    oss << "<hr>\n<p><em>Report generated by Akao Reporter Node</em></p>\n";
    oss << "</body>\n</html>\n";
    return oss.str();
}

std::string ReportGenerator::generateJSON() const {
    std::ostringstream oss;
    
    oss << "{\n";
    oss << "  \"title\": \"Report\",\n";
    oss << "  \"generated_at\": \"" << getCurrentTimestamp() << "\",\n";
    
    // Metrics
    oss << "  \"metrics\": {\n";
    bool first = true;
    for (const auto& [name, value] : metrics_.numeric_metrics) {
        if (!first) oss << ",\n";
        oss << "    \"" << name << "\": " << formatNumber(value);
        first = false;
    }
    for (const auto& [name, value] : metrics_.string_metrics) {
        if (!first) oss << ",\n";
        oss << "    \"" << name << "\": \"" << value << "\"";
        first = false;
    }
    for (const auto& [name, value] : metrics_.boolean_metrics) {
        if (!first) oss << ",\n";
        oss << "    \"" << name << "\": " << (value ? "true" : "false");
        first = false;
    }
    oss << "\n  },\n";
    
    // Sections
    oss << "  \"sections\": [\n";
    for (size_t i = 0; i < sections_.size(); ++i) {
        if (i > 0) oss << ",\n";
        oss << "    {\n";
        oss << "      \"title\": \"" << sections_[i]->title << "\",\n";
        oss << "      \"content\": \"" << sections_[i]->content << "\"\n";
        oss << "    }";
    }
    oss << "\n  ]\n";
    oss << "}\n";
    
    return oss.str();
}

std::string ReportGenerator::generateYAML() const {
    std::ostringstream oss;
    
    oss << "title: Report\n";
    oss << "generated_at: \"" << getCurrentTimestamp() << "\"\n\n";
    
    // Metrics
    oss << "metrics:\n";
    for (const auto& [name, value] : metrics_.numeric_metrics) {
        oss << "  " << name << ": " << formatNumber(value) << "\n";
    }
    for (const auto& [name, value] : metrics_.string_metrics) {
        oss << "  " << name << ": \"" << value << "\"\n";
    }
    for (const auto& [name, value] : metrics_.boolean_metrics) {
        oss << "  " << name << ": " << (value ? "true" : "false") << "\n";
    }
    
    // Sections
    oss << "\nsections:\n";
    for (const auto& section : sections_) {
        oss << "  - title: \"" << section->title << "\"\n";
        oss << "    content: \"" << section->content << "\"\n";
    }
    
    return oss.str();
}

std::string ReportGenerator::generateText() const {
    std::ostringstream oss;
    
    oss << "REPORT\n";
    oss << std::string(50, '=') << "\n\n";
    oss << "Generated on " << getCurrentTimestamp() << "\n\n";
    
    // Metrics section
    if (getMetricCount() > 0) {
        oss << "METRICS\n";
        oss << std::string(20, '-') << "\n";
        oss << formatMetrics("text");
        oss << "\n\n";
    }
    
    // Sections
    for (const auto& section : sections_) {
        oss << formatSection(*section, "text");
    }
    
    oss << std::string(50, '-') << "\n";
    oss << "Report generated by Akao Reporter Node\n";
    return oss.str();
}

void ReportGenerator::clear() {
    sections_.clear();
    metrics_ = ReportMetrics();
}

size_t ReportGenerator::getSectionCount() const {
    return sections_.size();
}

size_t ReportGenerator::getMetricCount() const {
    return metrics_.numeric_metrics.size() + 
           metrics_.string_metrics.size() + 
           metrics_.boolean_metrics.size();
}

std::vector<std::string> ReportGenerator::getSupportedFormats() const {
    return {"markdown", "html", "json", "yaml", "text"};
}

std::string ReportGenerator::formatSection(const ReportSection& section, const std::string& format, int depth) const {
    std::ostringstream oss;
    
    if (format == "markdown") {
        std::string prefix(depth + 2, '#');
        oss << prefix << " " << section.title << "\n\n";
        if (!section.content.empty()) {
            oss << section.content << "\n\n";
        }
    } else if (format == "html") {
        int level = std::min(depth + 2, 6);
        oss << "<h" << level << ">" << escapeHTML(section.title) << "</h" << level << ">\n";
        if (!section.content.empty()) {
            oss << "<div>" << escapeHTML(section.content) << "</div>\n";
        }
    } else if (format == "text") {
        std::string indent(depth * 2, ' ');
        oss << indent << section.title << "\n";
        oss << indent << std::string(section.title.length(), '-') << "\n\n";
        if (!section.content.empty()) {
            oss << indent << section.content << "\n\n";
        }
    }
    
    // Process subsections
    for (const auto& subsection : section.subsections) {
        oss << formatSection(*subsection, format, depth + 1);
    }
    
    return oss.str();
}

std::string ReportGenerator::formatMetrics(const std::string& format) const {
    std::ostringstream oss;
    
    if (format == "markdown") {
        oss << "| Metric | Value |\n";
        oss << "|--------|-------|\n";
        for (const auto& [name, value] : metrics_.numeric_metrics) {
            oss << "| " << name << " | " << formatNumber(value) << " |\n";
        }
        for (const auto& [name, value] : metrics_.string_metrics) {
            oss << "| " << name << " | " << value << " |\n";
        }
        for (const auto& [name, value] : metrics_.boolean_metrics) {
            oss << "| " << name << " | " << (value ? "true" : "false") << " |\n";
        }
    } else if (format == "html") {
        oss << "<table>\n<tr><th>Metric</th><th>Value</th></tr>\n";
        for (const auto& [name, value] : metrics_.numeric_metrics) {
            oss << "<tr><td>" << escapeHTML(name) << "</td><td>" << formatNumber(value) << "</td></tr>\n";
        }
        for (const auto& [name, value] : metrics_.string_metrics) {
            oss << "<tr><td>" << escapeHTML(name) << "</td><td>" << escapeHTML(value) << "</td></tr>\n";
        }
        for (const auto& [name, value] : metrics_.boolean_metrics) {
            oss << "<tr><td>" << escapeHTML(name) << "</td><td>" << (value ? "true" : "false") << "</td></tr>\n";
        }
        oss << "</table>\n";
    } else if (format == "text") {
        for (const auto& [name, value] : metrics_.numeric_metrics) {
            oss << name << ": " << formatNumber(value) << "\n";
        }
        for (const auto& [name, value] : metrics_.string_metrics) {
            oss << name << ": " << value << "\n";
        }
        for (const auto& [name, value] : metrics_.boolean_metrics) {
            oss << name << ": " << (value ? "true" : "false") << "\n";
        }
    }
    
    return oss.str();
}

std::string ReportGenerator::escapeHTML(const std::string& text) const {
    std::string result = text;
    std::string::size_type pos = 0;
    while ((pos = result.find("&", pos)) != std::string::npos) {
        result.replace(pos, 1, "&amp;");
        pos += 5;
    }
    pos = 0;
    while ((pos = result.find("<", pos)) != std::string::npos) {
        result.replace(pos, 1, "&lt;");
        pos += 4;
    }
    pos = 0;
    while ((pos = result.find(">", pos)) != std::string::npos) {
        result.replace(pos, 1, "&gt;");
        pos += 4;
    }
    return result;
}

std::string ReportGenerator::escapeMarkdown(const std::string& text) const {
    std::string result = text;
    // Simple escaping for special markdown characters
    std::vector<char> special = {'*', '_', '#', '`', '[', ']', '(', ')', '!', '|'};
    for (char c : special) {
        std::string::size_type pos = 0;
        std::string from(1, c);
        std::string to = "\\" + from;
        while ((pos = result.find(from, pos)) != std::string::npos) {
            result.replace(pos, 1, to);
            pos += 2;
        }
    }
    return result;
}

std::string ReportGenerator::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string ReportGenerator::formatNumber(double value, int precision) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision) << value;
    return oss.str();
}

// =============================================================================
// ReporterNode Implementation
// =============================================================================

ReporterNode::ReporterNode() 
    : nodeId_("akao:node:builtin:reporter:v1")
    , nodeType_("reporter")
    , version_("1.0.0")
    , description_("Advanced report generation and data analysis node") {}

std::string ReporterNode::getNodeId() const { return nodeId_; }
std::string ReporterNode::getNodeType() const { return nodeType_; }
std::string ReporterNode::getVersion() const { return version_; }
std::string ReporterNode::getDescription() const { return description_; }

foundation::interfaces::ValidationResult ReporterNode::validate(const foundation::interfaces::NodeParameters& params) const {
    foundation::interfaces::ValidationResult result(true);
    
    // Validate required parameters
    if (!params.hasParameter("operation")) {
        result.setValid(false);
        result.addError("Missing required parameter: operation");
    }
    
    return result;
}

foundation::types::ExecutionResult ReporterNode::execute(const foundation::interfaces::NodeContext& context,
                                                       const foundation::interfaces::NodeParameters& params) {
    try {
        std::string operation = params.getParameter("operation").asString();
        
        if (operation == "generate") {
            auto data = params.getParameter("data");
            std::string format = params.hasParameter("format") ? 
                params.getParameter("format").asString() : "markdown";
            
            std::string report = generateReport(data, format);
            return foundation::types::ExecutionResult::success(foundation::types::NodeValue(report));
            
        } else if (operation == "analyze") {
            auto dataArray = params.getParameter("data").asArray();
            auto analysis = analyzeData(dataArray);
            return foundation::types::ExecutionResult::success(analysis);
            
        } else if (operation == "validation_report") {
            auto violations = params.getParameter("violations").asArray();
            std::string report = generateValidationReport(violations);
            return foundation::types::ExecutionResult::success(foundation::types::NodeValue(report));
            
        } else if (operation == "metrics_report") {
            auto metrics = params.getParameter("metrics").asObject();
            std::string report = generateMetricsReport(metrics);
            return foundation::types::ExecutionResult::success(foundation::types::NodeValue(report));
            
        } else {
            return foundation::types::ExecutionResult::error("Unsupported operation: " + operation);
        }
    } catch (const std::exception& e) {
        return foundation::types::ExecutionResult::error("Report generation failed: " + std::string(e.what()));
    }
}

foundation::types::NodeValue ReporterNode::getParameterSchema() const {
    std::map<std::string, foundation::types::NodeValue> schema;
    schema["operation"] = foundation::types::NodeValue("string");
    schema["data"] = foundation::types::NodeValue("any");
    schema["format"] = foundation::types::NodeValue("string");
    schema["violations"] = foundation::types::NodeValue("array");
    schema["metrics"] = foundation::types::NodeValue("object");
    return foundation::types::NodeValue(schema);
}

std::string ReporterNode::generateReport(const foundation::types::NodeValue& data, const std::string& format) {
    ReportGenerator generator;
    
    if (data.isObject()) {
        auto obj = data.asObject();
        for (const auto& [key, value] : obj) {
            generator.addSection(key, value.toString());
        }
    } else if (data.isArray()) {
        auto arr = data.asArray();
        generator.aggregateData(arr);
        generator.addSection("Data Summary", "Analyzed " + std::to_string(arr.size()) + " items");
    } else {
        generator.addSection("Data", data.toString());
    }
    
    generator.addSummaryStatistics();
    return generator.generateReport(format);
}

std::string ReporterNode::generateValidationReport(const std::vector<foundation::types::NodeValue>& violations) {
    ReportGenerator generator;
    
    generator.addSection("Validation Report", "Summary of validation violations found");
    generator.addMetric("total_violations", static_cast<double>(violations.size()));
    
    size_t errors = 0, warnings = 0;
    for (const auto& violation : violations) {
        if (violation.isObject()) {
            auto obj = violation.asObject();
            if (obj.find("severity") != obj.end()) {
                std::string severity = obj.at("severity").asString();
                if (severity == "error") errors++;
                else if (severity == "warning") warnings++;
            }
        }
    }
    
    generator.addMetric("errors", static_cast<double>(errors));
    generator.addMetric("warnings", static_cast<double>(warnings));
    
    // Add violations section
    std::ostringstream violations_content;
    for (size_t i = 0; i < violations.size() && i < 10; ++i) { // Limit to 10 violations
        violations_content << "- " << violations[i].toString() << "\n";
    }
    if (violations.size() > 10) {
        violations_content << "... and " << (violations.size() - 10) << " more violations\n";
    }
    
    generator.addSection("Violations", violations_content.str());
    generator.addSummaryStatistics();
    
    return generator.generateReport("markdown");
}

std::string ReporterNode::generateMetricsReport(const std::map<std::string, foundation::types::NodeValue>& metrics) {
    ReportGenerator generator;
    
    generator.addSection("Metrics Report", "System performance and statistics");
    generator.addMetrics(metrics);
    generator.addSummaryStatistics();
    
    return generator.generateReport("markdown");
}

std::string ReporterNode::generateSummaryReport(const std::vector<foundation::types::NodeValue>& sections) {
    ReportGenerator generator;
    
    for (const auto& section : sections) {
        if (section.isObject()) {
            auto obj = section.asObject();
            std::string title = obj.find("title") != obj.end() ? 
                obj.at("title").asString() : "Section";
            std::string content = obj.find("content") != obj.end() ? 
                obj.at("content").asString() : "";
            generator.addSection(title, content);
        }
    }
    
    generator.addSummaryStatistics();
    return generator.generateReport("markdown");
}

foundation::types::NodeValue ReporterNode::analyzeData(const std::vector<foundation::types::NodeValue>& data) {
    std::map<std::string, foundation::types::NodeValue> analysis;
    
    analysis["total_items"] = foundation::types::NodeValue(static_cast<int64_t>(data.size()));
    
    // Type analysis
    size_t strings = 0, numbers = 0, booleans = 0, arrays = 0, objects = 0, nulls = 0;
    std::vector<double> numeric_values;
    
    for (const auto& item : data) {
        if (item.isString()) strings++;
        else if (item.isInteger()) {
            numbers++;
            numeric_values.push_back(static_cast<double>(item.asInteger()));
        } else if (item.isDouble()) {
            numbers++;
            numeric_values.push_back(item.asDouble());
        } else if (item.isBoolean()) booleans++;
        else if (item.isArray()) arrays++;
        else if (item.isObject()) objects++;
        else if (item.isNull()) nulls++;
    }
    
    analysis["string_count"] = foundation::types::NodeValue(static_cast<int64_t>(strings));
    analysis["number_count"] = foundation::types::NodeValue(static_cast<int64_t>(numbers));
    analysis["boolean_count"] = foundation::types::NodeValue(static_cast<int64_t>(booleans));
    analysis["array_count"] = foundation::types::NodeValue(static_cast<int64_t>(arrays));
    analysis["object_count"] = foundation::types::NodeValue(static_cast<int64_t>(objects));
    analysis["null_count"] = foundation::types::NodeValue(static_cast<int64_t>(nulls));
    
    // Numeric statistics
    if (!numeric_values.empty()) {
        double sum = std::accumulate(numeric_values.begin(), numeric_values.end(), 0.0);
        double mean = sum / numeric_values.size();
        double min_val = *std::min_element(numeric_values.begin(), numeric_values.end());
        double max_val = *std::max_element(numeric_values.begin(), numeric_values.end());
        
        analysis["numeric_sum"] = foundation::types::NodeValue(sum);
        analysis["numeric_mean"] = foundation::types::NodeValue(mean);
        analysis["numeric_min"] = foundation::types::NodeValue(min_val);
        analysis["numeric_max"] = foundation::types::NodeValue(max_val);
    }
    
    return foundation::types::NodeValue(analysis);
}

std::vector<std::string> ReporterNode::getSupportedFormats() {
    return {"markdown", "html", "json", "yaml", "text"};
}

std::string ReporterNode::generateComparisonReport(const foundation::types::NodeValue& before, 
                                                  const foundation::types::NodeValue& after) {
    ReportGenerator generator;
    
    generator.addSection("Comparison Report", "Before and after analysis");
    generator.addSection("Before", before.toString());
    generator.addSection("After", after.toString());
    
    generator.addSummaryStatistics();
    return generator.generateReport("markdown");
}

std::string ReporterNode::generateTrendReport(const std::vector<foundation::types::NodeValue>& timeSeriesData) {
    ReportGenerator generator;
    
    generator.addSection("Trend Report", "Time series data analysis");
    generator.addMetric("data_points", static_cast<double>(timeSeriesData.size()));
    
    generator.addSummaryStatistics();
    return generator.generateReport("markdown");
}

std::string ReporterNode::generateExecutionReport(const std::vector<foundation::types::NodeValue>& executionResults) {
    ReportGenerator generator;
    
    generator.addSection("Execution Report", "Analysis of execution results");
    generator.addMetric("executions", static_cast<double>(executionResults.size()));
    
    size_t successes = 0, failures = 0;
    for (const auto& result : executionResults) {
        if (result.isObject()) {
            auto obj = result.asObject();
            if (obj.find("success") != obj.end() && obj.at("success").isBoolean()) {
                if (obj.at("success").asBoolean()) successes++;
                else failures++;
            }
        }
    }
    
    generator.addMetric("successes", static_cast<double>(successes));
    generator.addMetric("failures", static_cast<double>(failures));
    generator.addMetric("success_rate", successes > 0 ? 
        static_cast<double>(successes) / (successes + failures) * 100.0 : 0.0);
    
    generator.addSummaryStatistics();
    return generator.generateReport("markdown");
}

bool ReporterNode::saveReport(const std::string& content, const std::string& filePath) {
    try {
        std::ofstream file(filePath);
        if (!file) return false;
        file << content;
        return true;
    } catch (...) {
        return false;
    }
}

std::string ReporterNode::generateTimestampedFilename(const std::string& baseName, const std::string& format) {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::ostringstream oss;
    oss << baseName << "_" << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S") << "." << format;
    return oss.str();
}

foundation::types::NodeValue ReporterNode::reportToNodeValue(const ReportGenerator& report) const {
    std::map<std::string, foundation::types::NodeValue> result;
    result["section_count"] = foundation::types::NodeValue(static_cast<int64_t>(report.getSectionCount()));
    result["metric_count"] = foundation::types::NodeValue(static_cast<int64_t>(report.getMetricCount()));
    return foundation::types::NodeValue(result);
}

ReportGenerator ReporterNode::nodeValueToReport(const foundation::types::NodeValue& value) const {
    ReportGenerator generator;
    
    if (value.isObject()) {
        auto obj = value.asObject();
        for (const auto& [key, val] : obj) {
            generator.addSection(key, val.toString());
        }
    }
    
    return generator;
}

} // namespace builtin
} // namespace nodes
} // namespace akao