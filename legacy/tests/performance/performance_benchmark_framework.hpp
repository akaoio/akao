/**
 * @file performance_benchmark_framework.hpp
 * @brief Comprehensive performance benchmarking framework for architectural reformation validation
 * @version 1.0
 * @date 2025-08-22
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <functional>
#include <memory>

namespace akao {
namespace performance {

// Performance Metrics Structure
struct PerformanceMetrics {
    std::chrono::nanoseconds execution_time;
    size_t memory_peak_kb;
    size_t memory_baseline_kb;
    double cpu_utilization_percent;
    size_t operations_per_second;
    double throughput_mbps;
    std::map<std::string, double> custom_metrics;
};

// Benchmark Category
enum class BenchmarkCategory {
    STARTUP_PERFORMANCE,
    EXECUTION_PERFORMANCE,
    BUILD_PERFORMANCE,
    PARSE_PERFORMANCE,
    MEMORY_EFFICIENCY,
    THROUGHPUT_ANALYSIS
};

// Benchmark Test Case
struct BenchmarkTestCase {
    std::string name;
    std::string description;
    BenchmarkCategory category;
    std::function<void()> test_function;
    std::string baseline_system;
    double target_performance;
    std::string performance_unit;
    std::vector<std::string> test_datasets;
};

// Comparison Baseline
struct ComparisonBaseline {
    std::string system_name;
    std::string version;
    PerformanceMetrics baseline_metrics;
    std::string measurement_conditions;
    std::chrono::system_clock::time_point measurement_date;
};

// Performance Benchmark Framework
class PerformanceBenchmarkFramework {
private:
    std::vector<BenchmarkTestCase> benchmark_suite;
    std::map<std::string, ComparisonBaseline> baselines;
    std::string benchmark_data_path;
    std::string results_output_path;
    bool enable_detailed_profiling;
    
public:
    PerformanceBenchmarkFramework(const std::string& data_path, const std::string& output_path);
    
    // Benchmark Registration
    void register_benchmark(const BenchmarkTestCase& test_case);
    void register_baseline(const std::string& name, const ComparisonBaseline& baseline);
    
    // Benchmark Execution
    PerformanceMetrics run_single_benchmark(const std::string& benchmark_name);
    std::vector<PerformanceMetrics> run_category_benchmarks(BenchmarkCategory category);
    std::map<std::string, PerformanceMetrics> run_all_benchmarks();
    
    // Comparison Analysis
    double calculate_performance_ratio(const PerformanceMetrics& actual, const PerformanceMetrics& baseline);
    std::map<std::string, double> compare_against_baselines(const PerformanceMetrics& metrics);
    bool meets_performance_targets(const PerformanceMetrics& metrics, const BenchmarkTestCase& test_case);
    
    // Profiling and Analysis
    void enable_profiling(bool enable);
    PerformanceMetrics profile_execution(std::function<void()> function);
    std::vector<std::string> identify_performance_bottlenecks(const PerformanceMetrics& metrics);
    
    // Reporting
    void generate_performance_report(const std::map<std::string, PerformanceMetrics>& results);
    void generate_comparison_report(const std::map<std::string, PerformanceMetrics>& results);
    void generate_regression_analysis(const std::map<std::string, PerformanceMetrics>& current_results);
};

// Specific Benchmark Categories

// Startup Performance Benchmarks
class StartupPerformanceBenchmarks {
public:
    static BenchmarkTestCase cold_start_benchmark();
    static BenchmarkTestCase warm_start_benchmark();
    static BenchmarkTestCase runtime_initialization_benchmark();
    static BenchmarkTestCase command_interface_startup_benchmark();
    
private:
    static void measure_cold_start();
    static void measure_warm_start();
    static void measure_runtime_initialization();
    static void measure_command_interface_startup();
};

// Execution Performance Benchmarks
class ExecutionPerformanceBenchmarks {
public:
    static BenchmarkTestCase logic_operation_benchmark();
    static BenchmarkTestCase symbol_resolution_benchmark();
    static BenchmarkTestCase ast_traversal_benchmark();
    static BenchmarkTestCase type_checking_benchmark();
    
private:
    static void measure_logic_operations();
    static void measure_symbol_resolution();
    static void measure_ast_traversal();
    static void measure_type_checking();
};

// Build Performance Benchmarks
class BuildPerformanceBenchmarks {
public:
    static BenchmarkTestCase small_project_build_benchmark();
    static BenchmarkTestCase medium_project_build_benchmark();
    static BenchmarkTestCase large_project_build_benchmark();
    static BenchmarkTestCase incremental_build_benchmark();
    
private:
    static void measure_small_project_build();
    static void measure_medium_project_build();
    static void measure_large_project_build();
    static void measure_incremental_build();
};

// Parse Performance Benchmarks
class ParsePerformanceBenchmarks {
public:
    static BenchmarkTestCase pure_logic_parsing_benchmark();
    static BenchmarkTestCase multi_language_parsing_benchmark();
    static BenchmarkTestCase large_file_parsing_benchmark();
    static BenchmarkTestCase concurrent_parsing_benchmark();
    
private:
    static void measure_pure_logic_parsing();
    static void measure_multi_language_parsing();
    static void measure_large_file_parsing();
    static void measure_concurrent_parsing();
};

// Memory Efficiency Benchmarks
class MemoryEfficiencyBenchmarks {
public:
    static BenchmarkTestCase memory_allocation_benchmark();
    static BenchmarkTestCase memory_leak_detection_benchmark();
    static BenchmarkTestCase garbage_collection_benchmark();
    static BenchmarkTestCase memory_fragmentation_benchmark();
    
private:
    static void measure_memory_allocation();
    static void measure_memory_leaks();
    static void measure_garbage_collection();
    static void measure_memory_fragmentation();
};

// Performance Measurement Utilities
class PerformanceMeasurement {
public:
    // Time Measurement
    static std::chrono::nanoseconds measure_execution_time(std::function<void()> function);
    static std::chrono::nanoseconds measure_average_execution_time(std::function<void()> function, int iterations);
    
    // Memory Measurement
    static size_t measure_memory_usage();
    static size_t measure_peak_memory_usage(std::function<void()> function);
    static bool detect_memory_leaks(std::function<void()> function);
    
    // CPU Measurement
    static double measure_cpu_utilization(std::function<void()> function);
    static std::map<int, double> measure_per_core_utilization(std::function<void()> function);
    
    // Throughput Measurement
    static double measure_throughput(std::function<void()> function, size_t data_size_bytes);
    static size_t measure_operations_per_second(std::function<void()> function);
    
    // System Resource Monitoring
    static std::map<std::string, double> get_system_resource_usage();
    static bool is_system_under_load();
    static void wait_for_system_idle();
};

// Baseline Comparison Systems
class BaselineComparisons {
public:
    // Python Comparison
    static ComparisonBaseline measure_python_startup();
    static ComparisonBaseline measure_python_execution();
    static ComparisonBaseline measure_python_parsing();
    
    // Node.js Comparison
    static ComparisonBaseline measure_nodejs_startup();
    static ComparisonBaseline measure_nodejs_execution();
    static ComparisonBaseline measure_nodejs_build();
    
    // GCC/Clang Comparison
    static ComparisonBaseline measure_gcc_compilation();
    static ComparisonBaseline measure_clang_compilation();
    
    // Established Parsers Comparison
    static ComparisonBaseline measure_antlr_parsing();
    static ComparisonBaseline measure_tree_sitter_parsing();
    
private:
    static PerformanceMetrics execute_external_benchmark(const std::string& command);
    static PerformanceMetrics parse_benchmark_output(const std::string& output);
};

// Performance Test Data Generator
class PerformanceTestDataGenerator {
public:
    // Generate test files of various sizes
    static void generate_small_test_files(const std::string& output_dir);
    static void generate_medium_test_files(const std::string& output_dir);
    static void generate_large_test_files(const std::string& output_dir);
    
    // Generate Pure Logic test content
    static std::string generate_pure_logic_content(size_t target_size_kb);
    static std::string generate_complex_logic_expressions(int count);
    static std::string generate_nested_quantifiers(int depth);
    
    // Generate multi-language test projects
    static void generate_multi_language_project(const std::string& project_path);
    static void generate_realistic_project_structure(const std::string& project_path);
    
    // Generate stress test scenarios
    static void generate_stress_test_files(const std::string& output_dir);
    static void generate_concurrent_access_test(const std::string& output_dir);
};

// Performance Report Generator
class PerformanceReportGenerator {
private:
    std::string output_directory;
    std::string report_format;
    
public:
    PerformanceReportGenerator(const std::string& output_dir, const std::string& format);
    
    // Report Generation
    void generate_comprehensive_performance_report(
        const std::map<std::string, PerformanceMetrics>& results,
        const std::map<std::string, ComparisonBaseline>& baselines
    );
    
    void generate_performance_comparison_charts(
        const std::map<std::string, PerformanceMetrics>& results
    );
    
    void generate_performance_trend_analysis(
        const std::vector<std::map<std::string, PerformanceMetrics>>& historical_results
    );
    
    void generate_bottleneck_analysis_report(
        const std::map<std::string, PerformanceMetrics>& results
    );
    
private:
    // Format-specific generators
    void generate_html_performance_report(const std::string& content, const std::string& filename);
    void generate_csv_performance_data(const std::map<std::string, PerformanceMetrics>& results);
    void generate_json_performance_summary(const std::map<std::string, PerformanceMetrics>& results);
    
    // Chart and visualization generators
    std::string generate_performance_comparison_chart(const std::map<std::string, PerformanceMetrics>& results);
    std::string generate_memory_usage_chart(const std::map<std::string, PerformanceMetrics>& results);
    std::string generate_execution_time_chart(const std::map<std::string, PerformanceMetrics>& results);
};

// Performance Validation Framework
class PerformanceValidationFramework {
public:
    // Validation Criteria
    struct ValidationCriteria {
        double max_startup_time_ms;
        double max_execution_time_ms;
        double max_build_time_s;
        double max_parse_speed_mb_s;
        size_t max_memory_usage_mb;
        double min_operations_per_second;
        double performance_regression_threshold;
    };
    
    // Validation Methods
    static bool validate_performance_targets(
        const PerformanceMetrics& metrics, 
        const ValidationCriteria& criteria
    );
    
    static bool validate_against_baselines(
        const PerformanceMetrics& metrics,
        const std::map<std::string, ComparisonBaseline>& baselines,
        double acceptable_performance_ratio = 1.2
    );
    
    static std::vector<std::string> identify_performance_regressions(
        const std::map<std::string, PerformanceMetrics>& current_results,
        const std::map<std::string, PerformanceMetrics>& previous_results,
        double regression_threshold = 0.1
    );
    
    static bool validate_system_integration_performance(
        const std::map<std::string, PerformanceMetrics>& integration_results
    );
    
    // Performance Quality Gates
    static bool passes_startup_performance_gate(const PerformanceMetrics& metrics);
    static bool passes_execution_performance_gate(const PerformanceMetrics& metrics);
    static bool passes_build_performance_gate(const PerformanceMetrics& metrics);
    static bool passes_memory_efficiency_gate(const PerformanceMetrics& metrics);
    
    // Continuous Performance Monitoring
    static void setup_performance_monitoring();
    static std::map<std::string, PerformanceMetrics> collect_runtime_performance_metrics();
    static void alert_on_performance_degradation(const PerformanceMetrics& metrics);
};

} // namespace performance
} // namespace akao
