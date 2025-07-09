#pragma once

#include "../../core/engine/validator/universal/v1.hpp"
#include <chrono>
#include <string>
#include <vector>
#include <map>

namespace akao {
namespace tests {
namespace production {

/**
 * @brief Performance benchmark for Pure Logic Engine mathematical operations
 */
class PerformanceBenchmark {
public:
    struct BenchmarkResult {
        std::string operation_name;
        double execution_time_ms;
        size_t operations_performed;
        double operations_per_second;
        bool passed_threshold;
    };

    /**
     * @brief Run comprehensive performance benchmarks
     */
    std::vector<BenchmarkResult> runBenchmarks();

    /**
     * @brief Benchmark mathematical proof operations
     */
    BenchmarkResult benchmarkMathematicalProofs();

    /**
     * @brief Benchmark Gödel encoding operations
     */
    BenchmarkResult benchmarkGodelEncoding();

    /**
     * @brief Benchmark consistency checking
     */
    BenchmarkResult benchmarkConsistencyChecking();

    /**
     * @brief Benchmark self-proving capabilities
     */
    BenchmarkResult benchmarkSelfProving();

public:
    static constexpr size_t DEFAULT_ITERATIONS = 10000;
    static constexpr double ACCEPTABLE_TIME_MS = 1000.0; // 1 second for 10k operations

private:
    
    core::engine::validation::UniversalValidator validator_;
};

/**
 * @brief Integration testing for philosophy-rule-ruleset interactions
 */
class IntegrationTest {
public:
    struct TestResult {
        std::string test_name;
        bool passed;
        std::string error_message;
        double execution_time_ms;
    };

    /**
     * @brief Run all integration tests
     */
    std::vector<TestResult> runAllTests();

    /**
     * @brief Test philosophy-rule linkage
     */
    TestResult testPhilosophyRuleLinkage();

    /**
     * @brief Test ruleset dependency validation
     */
    TestResult testRulesetDependencies();

    /**
     * @brief Test runtime enable/disable capabilities
     */
    TestResult testRuntimeControl();

    /**
     * @brief Test cascade disable functionality
     */
    TestResult testCascadeDisable();

private:
    std::chrono::high_resolution_clock::time_point start_time_;
    
    void startTimer();
    double getElapsedTimeMs();
};

/**
 * @brief Production readiness validation framework
 */
class ProductionValidator {
public:
    struct ValidationReport {
        bool production_ready;
        std::string overall_status;
        std::vector<std::string> blocking_issues;
        std::vector<std::string> warnings;
        std::map<std::string, double> performance_metrics;
        double overall_score;
    };

    /**
     * @brief Complete production readiness validation
     */
    ValidationReport validateProductionReadiness();

    /**
     * @brief Generate detailed test report
     */
    std::string generateTestReport(const ValidationReport& report);

private:
    PerformanceBenchmark benchmark_;
    IntegrationTest integration_;

    /**
     * @brief Calculate overall readiness score
     */
    double calculateReadinessScore(const std::vector<PerformanceBenchmark::BenchmarkResult>& perf_results,
                                 const std::vector<IntegrationTest::TestResult>& integration_results);
};

} // namespace production
} // namespace tests
} // namespace akao
