#pragma once

/**
 * @file integration_test_suite.hpp
 * @brief Comprehensive integration test suite for architectural reformation validation
 * @version 1.0
 * @date 2025-08-22
 * 
 * This file implements the comprehensive integration test framework for validating
 * the complete architectural transformation success across all layers and components.
 */

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <functional>
#include <memory>

namespace akao {
namespace integration {

// Forward declarations
class IntegrationTest;

// Test Result Structures
struct TestMetrics {
    std::chrono::milliseconds execution_time;
    size_t memory_usage_kb;
    bool success;
    std::string error_message;
    std::map<std::string, double> custom_metrics;
};

struct PerformanceBenchmark {
    std::string name;
    double target_value;
    double actual_value;
    std::string unit;
    bool meets_target;
    std::string comparison_baseline;
};

struct IntegrationTestResult {
    std::string test_name;
    std::string category;
    TestMetrics metrics;
    std::vector<std::string> validation_steps;
    std::vector<PerformanceBenchmark> benchmarks;
    bool overall_success;
};

// Test Categories
enum class TestCategory {
    END_TO_END_WORKFLOW,
    PERFORMANCE_BENCHMARKING,
    CROSS_PLATFORM_VALIDATION,
    COMPATIBILITY_TESTING,
    TRANSFORMATION_VALIDATION
};

// Integration Test Framework
class IntegrationTestFramework {
private:
    std::vector<std::unique_ptr<IntegrationTest>> test_suite;
    std::string test_data_path;
    std::string output_path;
    bool enable_performance_monitoring;
    
public:
    IntegrationTestFramework(const std::string& data_path, const std::string& output_path);
    
    // Test Registration
    void register_test(std::unique_ptr<IntegrationTest> test);
    void register_test_category(TestCategory category);
    
    // Test Execution
    std::vector<IntegrationTestResult> run_all_tests();
    std::vector<IntegrationTestResult> run_category(TestCategory category);
    IntegrationTestResult run_single_test(const std::string& test_name);
    
    // Performance Monitoring
    void enable_performance_profiling(bool enable);
    PerformanceBenchmark benchmark_against_baseline(
        const std::string& test_name,
        const std::string& baseline_system
    );
    
    // Reporting
    void generate_integration_report(const std::vector<IntegrationTestResult>& results);
    void generate_performance_report(const std::vector<PerformanceBenchmark>& benchmarks);
    void generate_transformation_validation_report();
    
    // Validation
    bool validate_success_criteria(const std::vector<IntegrationTestResult>& results);
    bool validate_performance_targets(const std::vector<PerformanceBenchmark>& benchmarks);
};

// Base Integration Test Class
class IntegrationTest {
protected:
    std::string test_name;
    TestCategory category;
    std::vector<std::string> prerequisites;
    std::map<std::string, std::string> configuration;
    
public:
    IntegrationTest(const std::string& name, TestCategory cat);
    virtual ~IntegrationTest() = default;
    
    // Test Interface
    virtual IntegrationTestResult execute() = 0;
    virtual bool validate_result(const IntegrationTestResult& result) = 0;
    virtual std::vector<PerformanceBenchmark> benchmark() = 0;
    
    // Configuration
    void set_configuration(const std::map<std::string, std::string>& config);
    void add_prerequisite(const std::string& prerequisite);
    
    // Utilities
    std::string get_test_name() const { return test_name; }
    TestCategory get_category() const { return category; }
};

// Specific Test Categories

// End-to-End Workflow Tests
class EndToEndWorkflowTest : public IntegrationTest {
private:
    std::string project_template_path;
    std::string temporary_project_path;
    
public:
    EndToEndWorkflowTest(const std::string& name);
    
    IntegrationTestResult execute() override;
    bool validate_result(const IntegrationTestResult& result) override;
    std::vector<PerformanceBenchmark> benchmark() override;
    
protected:
    // Workflow Steps
    bool test_project_creation();
    bool test_development_workflow();
    bool test_build_and_test_workflow();
    bool test_multi_language_integration();
    
    // Validation Steps
    bool validate_project_structure();
    bool validate_akao_commands();
    bool validate_file_parsing();
    bool validate_build_output();
};

// Performance Benchmarking Tests
class PerformanceBenchmarkTest : public IntegrationTest {
private:
    std::vector<std::string> benchmark_datasets;
    std::map<std::string, std::string> baseline_systems;
    
public:
    PerformanceBenchmarkTest(const std::string& name);
    
    IntegrationTestResult execute() override;
    bool validate_result(const IntegrationTestResult& result) override;
    std::vector<PerformanceBenchmark> benchmark() override;
    
protected:
    // Benchmark Categories
    PerformanceBenchmark benchmark_startup_performance();
    PerformanceBenchmark benchmark_execution_performance();
    PerformanceBenchmark benchmark_build_performance();
    PerformanceBenchmark benchmark_parse_performance();
    
    // Comparison with Baselines
    PerformanceBenchmark compare_with_python();
    PerformanceBenchmark compare_with_nodejs();
    PerformanceBenchmark compare_with_established_tools();
};

// Cross-Platform Validation Tests
class CrossPlatformValidationTest : public IntegrationTest {
private:
    std::vector<std::string> target_platforms;
    std::string platform_specific_config_path;
    
public:
    CrossPlatformValidationTest(const std::string& name);
    
    IntegrationTestResult execute() override;
    bool validate_result(const IntegrationTestResult& result) override;
    std::vector<PerformanceBenchmark> benchmark() override;
    
protected:
    // Platform Testing
    bool test_linux_compatibility();
    bool test_windows_compatibility();
    bool test_macos_compatibility();
    
    // Cross-Platform Features
    bool test_file_system_compatibility();
    bool test_path_handling();
    bool test_environment_variables();
    bool test_process_management();
};

// Compatibility Testing
class CompatibilityTest : public IntegrationTest {
private:
    std::string legacy_project_path;
    std::vector<std::string> configuration_files;
    
public:
    CompatibilityTest(const std::string& name);
    
    IntegrationTestResult execute() override;
    bool validate_result(const IntegrationTestResult& result) override;
    std::vector<PerformanceBenchmark> benchmark() override;
    
protected:
    // Backward Compatibility
    bool test_cmake_integration();
    bool test_makefile_compatibility();
    bool test_file_system_structure();
    bool test_configuration_compatibility();
    
    // Forward Compatibility
    bool test_extension_capability();
    bool test_upgrade_procedures();
};

// Transformation Validation Tests
class TransformationValidationTest : public IntegrationTest {
private:
    std::string transformation_artifact_path;
    std::vector<std::string> success_criteria;
    
public:
    TransformationValidationTest(const std::string& name);
    
    IntegrationTestResult execute() override;
    bool validate_result(const IntegrationTestResult& result) override;
    std::vector<PerformanceBenchmark> benchmark() override;
    
protected:
    // Phase Validation
    bool validate_phase_1_transformation();
    bool validate_phase_2_transformation();
    bool validate_phase_3_transformation();
    
    // Quality Metrics
    bool validate_code_quality();
    bool validate_architectural_quality();
    bool validate_operational_quality();
    
    // Transformation Objectives
    bool validate_technical_objectives();
    bool validate_governance_objectives();
    bool validate_architectural_objectives();
};

// Utilities and Helpers
class TestUtilities {
public:
    // File System Operations
    static bool create_temporary_directory(const std::string& path);
    static bool copy_test_data(const std::string& source, const std::string& destination);
    static bool cleanup_temporary_files(const std::string& path);
    
    // Process Execution
    static std::pair<int, std::string> execute_command(const std::string& command);
    static std::pair<int, std::string> execute_akao_command(const std::string& args);
    
    // Performance Measurement
    static TestMetrics measure_execution_time(std::function<bool()> test_function);
    static size_t measure_memory_usage();
    static double measure_throughput(std::function<bool()> operation, size_t data_size);
    
    // Validation Helpers
    static bool validate_file_exists(const std::string& path);
    static bool validate_directory_structure(const std::string& path, const std::vector<std::string>& expected_files);
    static bool validate_json_schema(const std::string& json_file, const std::string& schema_file);
    static bool validate_yaml_structure(const std::string& yaml_file);
    
    // Comparison Utilities
    static double calculate_performance_ratio(double actual, double baseline);
    static bool performance_meets_target(double actual, double target, double tolerance = 0.1);
    static std::string format_performance_result(const PerformanceBenchmark& benchmark);
};

// Test Data Manager
class TestDataManager {
private:
    std::string base_data_path;
    std::map<std::string, std::string> dataset_registry;
    
public:
    TestDataManager(const std::string& data_path);
    
    // Dataset Management
    void register_dataset(const std::string& name, const std::string& path);
    std::string get_dataset_path(const std::string& name);
    std::vector<std::string> list_available_datasets();
    
    // Test File Generation
    void generate_test_project(const std::string& project_name, const std::string& language);
    void generate_performance_test_files(const std::string& size_category);
    void generate_multi_language_project();
    
    // Validation Data
    std::vector<std::string> get_validation_files();
    std::map<std::string, std::string> get_expected_outputs();
    std::vector<std::string> get_error_test_cases();
};

// Report Generator
class IntegrationReportGenerator {
private:
    std::string output_directory;
    std::string report_format; // "html", "markdown", "json"
    
public:
    IntegrationReportGenerator(const std::string& output_dir, const std::string& format);
    
    // Report Generation
    void generate_comprehensive_report(
        const std::vector<IntegrationTestResult>& test_results,
        const std::vector<PerformanceBenchmark>& benchmarks
    );
    
    void generate_performance_comparison_report(
        const std::vector<PerformanceBenchmark>& benchmarks
    );
    
    void generate_transformation_validation_report(
        const TransformationValidationTest& validation_test
    );
    
    void generate_executive_summary(
        const std::vector<IntegrationTestResult>& results,
        bool transformation_successful
    );
    
private:
    // Format-Specific Generators
    void generate_html_report(const std::string& content, const std::string& filename);
    void generate_markdown_report(const std::string& content, const std::string& filename);
    void generate_json_report(const std::map<std::string, std::string>& data, const std::string& filename);
    
    // Content Formatters
    std::string format_test_results_table(const std::vector<IntegrationTestResult>& results);
    std::string format_performance_charts(const std::vector<PerformanceBenchmark>& benchmarks);
    std::string format_success_criteria_matrix();
};

} // namespace integration
} // namespace akao
