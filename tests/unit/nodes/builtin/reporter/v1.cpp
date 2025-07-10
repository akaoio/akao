/**
 * @id: akao:test:nodes:builtin:reporter:v1
 * @doc: Unit tests for reporter node validating report generation, data analysis, and multi-format output capabilities. Tests Markdown, HTML, JSON, YAML, and text format generation, statistical analysis, and template processing functionality.
 * @specification: Unit tests for reporter node
 * @scope: Unit test for builtin reporter node
 * @timeline: 2025-07-10
 * @rationale: Ensure reliable report generation functionality
 * @methodology: Comprehensive testing with various data types and output formats
 * @references: ["akao:class:nodes:builtin:reporter:v1"]
 */

#include "nodes/builtin/reporter/v1.hpp"
#include <iostream>
#include <cassert>
#include <string>
#include <map>
#include <vector>

namespace akao {
namespace tests {

class ReporterNodeTest {
public:
    void runAllTests() {
        std::cout << "Running ReporterNode tests...\n";
        
        testNodeInfo();
        testReportGenerator();
        testReportSections();
        testReportMetrics();
        testMultiFormatGeneration();
        testDataAnalysis();
        testStaticUtilities();
        
        std::cout << "All ReporterNode tests passed!\n";
    }

private:
    void testNodeInfo() {
        std::cout << "Testing node info...\n";
        
        nodes::builtin::ReporterNode node;
        
        assert(node.getNodeId() == "akao:node:builtin:reporter:v1");
        assert(node.getNodeType() == "reporter");
        assert(node.getVersion() == "1.0.0");
        assert(!node.getDescription().empty());
        
        std::cout << "✓ Node info test passed\n";
    }

    void testReportGenerator() {
        std::cout << "Testing report generator...\n";
        
        nodes::builtin::ReportGenerator generator;
        
        // Test initial state
        assert(generator.getSectionCount() == 0);
        assert(generator.getMetricCount() == 0);
        
        // Test adding sections
        generator.addSection("Test Section", "This is test content");
        assert(generator.getSectionCount() == 1);
        
        auto section = generator.getSection("Test Section");
        assert(section != nullptr);
        assert(section->title == "Test Section");
        assert(section->content == "This is test content");
        
        // Test removing sections
        generator.removeSection("Test Section");
        assert(generator.getSectionCount() == 0);
        
        std::cout << "✓ Report generator test passed\n";
    }

    void testReportSections() {
        std::cout << "Testing report sections...\n";
        
        nodes::builtin::ReportGenerator generator;
        
        // Add multiple sections
        generator.addSection("Introduction", "This is the introduction");
        generator.addSection("Results", "These are the results");
        generator.addSection("Conclusion", "This is the conclusion");
        
        assert(generator.getSectionCount() == 3);
        
        // Test getting specific sections
        auto intro = generator.getSection("Introduction");
        assert(intro != nullptr);
        assert(intro->title == "Introduction");
        
        auto nonexistent = generator.getSection("NonExistent");
        assert(nonexistent == nullptr);
        
        std::cout << "✓ Report sections test passed\n";
    }

    void testReportMetrics() {
        std::cout << "Testing report metrics...\n";
        
        nodes::builtin::ReportGenerator generator;
        
        // Test adding different types of metrics
        generator.addMetric("success_rate", 95.5);
        generator.addMetric("status", "completed");
        generator.addMetric("enabled", true);
        
        assert(generator.getMetricCount() == 3);
        
        // Test adding metrics from map
        std::map<std::string, foundation::types::NodeValue> metrics;
        metrics["total_files"] = foundation::types::NodeValue(static_cast<int64_t>(42));
        metrics["processing_time"] = foundation::types::NodeValue(1.25);
        metrics["valid"] = foundation::types::NodeValue(true);
        
        generator.addMetrics(metrics);
        assert(generator.getMetricCount() == 6);
        
        std::cout << "✓ Report metrics test passed\n";
    }

    void testMultiFormatGeneration() {
        std::cout << "Testing multi-format generation...\n";
        
        nodes::builtin::ReportGenerator generator;
        
        // Set up test data
        generator.addSection("Summary", "Test report summary");
        generator.addMetric("test_metric", 42.0);
        generator.addSummaryStatistics();
        
        // Test supported formats
        auto formats = generator.getSupportedFormats();
        assert(std::find(formats.begin(), formats.end(), "markdown") != formats.end());
        assert(std::find(formats.begin(), formats.end(), "html") != formats.end());
        assert(std::find(formats.begin(), formats.end(), "json") != formats.end());
        assert(std::find(formats.begin(), formats.end(), "yaml") != formats.end());
        assert(std::find(formats.begin(), formats.end(), "text") != formats.end());
        
        // Generate reports in different formats
        std::string markdown = generator.generateMarkdown();
        assert(!markdown.empty());
        assert(markdown.find("# Report") != std::string::npos);
        assert(markdown.find("Summary") != std::string::npos);
        
        std::string html = generator.generateHTML();
        assert(!html.empty());
        assert(html.find("<html>") != std::string::npos);
        assert(html.find("<h1>Report</h1>") != std::string::npos);
        
        std::string json = generator.generateJSON();
        assert(!json.empty());
        assert(json.find("\"title\": \"Report\"") != std::string::npos);
        
        std::string yaml = generator.generateYAML();
        assert(!yaml.empty());
        assert(yaml.find("title: Report") != std::string::npos);
        
        std::string text = generator.generateText();
        assert(!text.empty());
        assert(text.find("REPORT") != std::string::npos);
        
        std::cout << "✓ Multi-format generation test passed\n";
    }

    void testDataAnalysis() {
        std::cout << "Testing data analysis...\n";
        
        nodes::builtin::ReportGenerator generator;
        
        // Test data aggregation
        std::vector<foundation::types::NodeValue> testData = {
            foundation::types::NodeValue("string1"),
            foundation::types::NodeValue("string2"),
            foundation::types::NodeValue(static_cast<int64_t>(42)),
            foundation::types::NodeValue(3.14),
            foundation::types::NodeValue(true),
            foundation::types::NodeValue(false)
        };
        
        generator.aggregateData(testData);
        
        // Verify aggregation results
        auto metrics = generator.getMetrics();
        assert(metrics.numeric_metrics.find("total_items") != metrics.numeric_metrics.end());
        assert(metrics.numeric_metrics.at("total_items") == 6.0);
        assert(metrics.numeric_metrics.at("string_count") == 2.0);
        assert(metrics.numeric_metrics.at("number_count") == 2.0);
        assert(metrics.numeric_metrics.at("boolean_count") == 2.0);
        
        // Test statistics calculation
        std::vector<double> values = {1.0, 2.0, 3.0, 4.0, 5.0};
        generator.calculateStatistics("test_dataset", values);
        
        assert(metrics.numeric_metrics.find("test_dataset_count") != metrics.numeric_metrics.end());
        assert(metrics.numeric_metrics.find("test_dataset_mean") != metrics.numeric_metrics.end());
        assert(metrics.numeric_metrics.find("test_dataset_min") != metrics.numeric_metrics.end());
        assert(metrics.numeric_metrics.find("test_dataset_max") != metrics.numeric_metrics.end());
        
        std::cout << "✓ Data analysis test passed\n";
    }

    void testStaticUtilities() {
        std::cout << "Testing static utility methods...\n";
        
        // Test basic report generation
        foundation::types::NodeValue testData = foundation::types::NodeValue("Test content");
        std::string report = nodes::builtin::ReporterNode::generateReport(testData, "markdown");
        assert(!report.empty());
        assert(report.find("# Report") != std::string::npos);
        
        // Test validation report
        std::vector<foundation::types::NodeValue> violations = {
            foundation::types::NodeValue("Violation 1"),
            foundation::types::NodeValue("Violation 2")
        };
        std::string validationReport = nodes::builtin::ReporterNode::generateValidationReport(violations);
        assert(!validationReport.empty());
        assert(validationReport.find("Validation Report") != std::string::npos);
        
        // Test metrics report
        std::map<std::string, foundation::types::NodeValue> metrics = {
            {"metric1", foundation::types::NodeValue(42.0)},
            {"metric2", foundation::types::NodeValue("test")}
        };
        std::string metricsReport = nodes::builtin::ReporterNode::generateMetricsReport(metrics);
        assert(!metricsReport.empty());
        assert(metricsReport.find("Metrics Report") != std::string::npos);
        
        // Test data analysis
        std::vector<foundation::types::NodeValue> analysisData = {
            foundation::types::NodeValue(static_cast<int64_t>(1)),
            foundation::types::NodeValue(static_cast<int64_t>(2)),
            foundation::types::NodeValue(static_cast<int64_t>(3))
        };
        auto analysis = nodes::builtin::ReporterNode::analyzeData(analysisData);
        assert(analysis.isObject());
        
        auto analysisObj = analysis.asObject();
        assert(analysisObj.find("total_items") != analysisObj.end());
        assert(analysisObj.at("total_items").asInteger() == 3);
        
        // Test supported formats
        auto formats = nodes::builtin::ReporterNode::getSupportedFormats();
        assert(formats.size() >= 5);
        assert(std::find(formats.begin(), formats.end(), "markdown") != formats.end());
        
        std::cout << "✓ Static utility methods test passed\n";
    }
};

// Test runner function for integration with main test suite
void runReporterTests() {
    ReporterNodeTest test;
    test.runAllTests();
}

} // namespace tests
} // namespace akao

// Standalone test runner (only when compiled standalone)
#ifndef AKAO_TEST_SUITE
int main() {
    try {
        akao::tests::runReporterTests();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
#endif