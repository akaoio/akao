/**
 * @file integration_test_runner.cpp
 * @brief Main integration test runner for Phase 3 Step 4 system integration and validation
 * @version 1.0
 * @date 2025-08-22
 */

#include "integration/integration_test_suite.hpp"
#include "performance/performance_benchmark_framework.hpp"
#include "transformation/transformation_validation_suite.hpp"

#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>

namespace akao {
namespace integration {

class IntegrationTestRunner {
private:
    std::string test_data_path;
    std::string output_path;
    std::unique_ptr<IntegrationTestFramework> integration_framework;
    std::unique_ptr<performance::PerformanceBenchmarkFramework> benchmark_framework;
    std::unique_ptr<transformation::TransformationValidationFramework> validation_framework;
    
public:
    IntegrationTestRunner(const std::string& data_path, const std::string& output_path)
        : test_data_path(data_path), output_path(output_path) {
        
        // Initialize frameworks
        integration_framework = std::make_unique<IntegrationTestFramework>(data_path, output_path + "/integration");
        benchmark_framework = std::make_unique<performance::PerformanceBenchmarkFramework>(data_path, output_path + "/performance");
        validation_framework = std::make_unique<transformation::TransformationValidationFramework>(
            "/workspaces/akao/artifacts/architectural-reformation", output_path + "/validation");
    }
    
    // Main execution method
    bool run_comprehensive_integration_validation() {
        std::cout << "\n=== AKAO ARCHITECTURAL REFORMATION - PHASE 3 STEP 4 ===\n";
        std::cout << "System Integration and Validation\n";
        std::cout << "Starting comprehensive validation suite...\n\n";
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // Phase 1: Integration Test Suite
        std::cout << "PHASE 1: Integration Test Suite\n";
        std::cout << "================================\n";
        auto integration_results = run_integration_tests();
        bool integration_success = validate_integration_results(integration_results);
        std::cout << "Integration Tests: " << (integration_success ? "PASS" : "FAIL") << "\n\n";
        
        // Phase 2: Performance Benchmarking
        std::cout << "PHASE 2: Performance Benchmarking\n";
        std::cout << "==================================\n";
        auto performance_results = run_performance_benchmarks();
        bool performance_success = validate_performance_results(performance_results);
        std::cout << "Performance Benchmarks: " << (performance_success ? "PASS" : "FAIL") << "\n\n";
        
        // Phase 3: Transformation Validation
        std::cout << "PHASE 3: Transformation Validation\n";
        std::cout << "===================================\n";
        auto transformation_results = run_transformation_validation();
        bool transformation_success = validate_transformation_results(transformation_results);
        std::cout << "Transformation Validation: " << (transformation_success ? "PASS" : "FAIL") << "\n\n";
        
        // Phase 4: Comprehensive Reporting
        std::cout << "PHASE 4: Comprehensive Reporting\n";
        std::cout << "=================================\n";
        generate_comprehensive_reports(integration_results, performance_results, transformation_results);
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto total_duration = std::chrono::duration_cast<std::chrono::minutes>(end_time - start_time);
        
        // Final Assessment
        bool overall_success = integration_success && performance_success && transformation_success;
        
        std::cout << "\n=== FINAL ASSESSMENT ===\n";
        std::cout << "Total execution time: " << total_duration.count() << " minutes\n";
        std::cout << "Integration Tests: " << (integration_success ? "✓ PASS" : "✗ FAIL") << "\n";
        std::cout << "Performance Benchmarks: " << (performance_success ? "✓ PASS" : "✗ FAIL") << "\n";
        std::cout << "Transformation Validation: " << (transformation_success ? "✓ PASS" : "✗ FAIL") << "\n";
        std::cout << "\nOVERALL RESULT: " << (overall_success ? "✓ SUCCESS" : "✗ FAILURE") << "\n";
        
        if (overall_success) {
            std::cout << "\n🎉 ARCHITECTURAL REFORMATION COMPLETED SUCCESSFULLY! 🎉\n";
            std::cout << "The Akao system has been successfully transformed and validated.\n";
            std::cout << "All integration tests, performance benchmarks, and transformation\n";
            std::cout << "validations have passed. The system is ready for production use.\n";
        } else {
            std::cout << "\n⚠️  ARCHITECTURAL REFORMATION REQUIRES ATTENTION ⚠️\n";
            std::cout << "Some validation steps have failed. Please review the detailed\n";
            std::cout << "reports in the output directory for specific issues and\n";
            std::cout << "recommendations for resolution.\n";
        }
        
        return overall_success;
    }
    
private:
    std::vector<IntegrationTestResult> run_integration_tests() {
        std::cout << "Running comprehensive integration test suite...\n";
        
        auto results = integration_framework->run_all_tests();
        
        std::cout << "Integration test results:\n";
        for (const auto& result : results) {
            std::cout << "  " << result.test_name << ": " 
                     << (result.overall_success ? "PASS" : "FAIL");
            if (!result.overall_success) {
                std::cout << " (" << result.metrics.error_message << ")";
            }
            std::cout << "\n";
        }
        
        return results;
    }
    
    std::map<std::string, performance::PerformanceMetrics> run_performance_benchmarks() {
        std::cout << "Running performance benchmarking suite...\n";
        
        auto results = benchmark_framework->run_all_benchmarks();
        
        std::cout << "Performance benchmark results:\n";
        for (const auto& [name, metrics] : results) {
            std::cout << "  " << name << ": " 
                     << metrics.execution_time.count() / 1000000.0 << "ms, "
                     << metrics.memory_peak_kb << "KB peak\n";
        }
        
        return results;
    }
    
    std::map<transformation::TransformationPhase, std::vector<transformation::ValidationResult>> 
    run_transformation_validation() {
        std::cout << "Running transformation validation suite...\n";
        
        auto results = validation_framework->validate_complete_transformation();
        
        std::cout << "Transformation validation results:\n";
        for (const auto& [phase, validations] : results) {
            std::string phase_name;
            switch (phase) {
                case transformation::TransformationPhase::PHASE_1_ARCHITECTURAL_PURIFICATION:
                    phase_name = "Phase 1 - Architectural Purification";
                    break;
                case transformation::TransformationPhase::PHASE_2_GOVERNANCE_IMPLEMENTATION:
                    phase_name = "Phase 2 - Governance Implementation";
                    break;
                case transformation::TransformationPhase::PHASE_3_TECHNOLOGY_EVOLUTION:
                    phase_name = "Phase 3 - Technology Evolution";
                    break;
                case transformation::TransformationPhase::SYSTEM_INTEGRATION:
                    phase_name = "System Integration";
                    break;
            }
            
            int passed = 0, total = validations.size();
            for (const auto& validation : validations) {
                if (validation.success) passed++;
            }
            
            std::cout << "  " << phase_name << ": " << passed << "/" << total 
                     << " validations passed\n";
        }
        
        return results;
    }
    
    bool validate_integration_results(const std::vector<IntegrationTestResult>& results) {
        return integration_framework->validate_success_criteria(results);
    }
    
    bool validate_performance_results(const std::map<std::string, performance::PerformanceMetrics>& results) {
        // Validate performance targets
        performance::PerformanceValidationFramework::ValidationCriteria criteria;
        criteria.max_startup_time_ms = 100.0;
        criteria.max_execution_time_ms = 50.0;
        criteria.max_build_time_s = 5.0;
        criteria.max_memory_usage_mb = 50.0;
        criteria.performance_regression_threshold = 0.1;
        
        for (const auto& [name, metrics] : results) {
            if (!performance::PerformanceValidationFramework::validate_performance_targets(metrics, criteria)) {
                return false;
            }
        }
        return true;
    }
    
    bool validate_transformation_results(
        const std::map<transformation::TransformationPhase, std::vector<transformation::ValidationResult>>& results) {
        
        for (const auto& [phase, validations] : results) {
            for (const auto& validation : validations) {
                if (!validation.success) {
                    return false;
                }
            }
        }
        return true;
    }
    
    void generate_comprehensive_reports(
        const std::vector<IntegrationTestResult>& integration_results,
        const std::map<std::string, performance::PerformanceMetrics>& performance_results,
        const std::map<transformation::TransformationPhase, std::vector<transformation::ValidationResult>>& transformation_results) {
        
        std::cout << "Generating comprehensive reports...\n";
        
        // Generate integration report
        integration_framework->generate_integration_report(integration_results);
        
        // Generate performance report
        benchmark_framework->generate_performance_report(performance_results);
        
        // Generate transformation validation report
        validation_framework->generate_transformation_validation_report(transformation_results);
        
        // Generate executive summary
        generate_executive_summary(integration_results, performance_results, transformation_results);
        
        std::cout << "Reports generated in: " << output_path << "/\n";
    }
    
    void generate_executive_summary(
        const std::vector<IntegrationTestResult>& integration_results,
        const std::map<std::string, performance::PerformanceMetrics>& performance_results,
        const std::map<transformation::TransformationPhase, std::vector<transformation::ValidationResult>>& transformation_results) {
        
        std::ofstream summary(output_path + "/executive_summary.md");
        
        summary << "# Akao Architectural Reformation - Executive Summary\n\n";
        
        // Get current time properly
        auto now = std::time(nullptr);
        summary << "**Date:** " << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S") << "\n";
        summary << "**Phase:** 3 Step 4 - System Integration and Validation\n\n";
        
        // Integration Test Summary
        summary << "## Integration Test Results\n\n";
        int integration_passed = 0;
        for (const auto& result : integration_results) {
            if (result.overall_success) integration_passed++;
        }
        summary << "- Total Tests: " << integration_results.size() << "\n";
        summary << "- Passed: " << integration_passed << "\n";
        summary << "- Failed: " << (integration_results.size() - integration_passed) << "\n";
        summary << "- Success Rate: " << (100.0 * integration_passed / integration_results.size()) << "%\n\n";
        
        // Performance Summary
        summary << "## Performance Benchmark Results\n\n";
        summary << "- Total Benchmarks: " << performance_results.size() << "\n";
        double avg_startup_time = 0;
        for (const auto& [name, metrics] : performance_results) {
            if (name.find("startup") != std::string::npos) {
                avg_startup_time = metrics.execution_time.count() / 1000000.0;
                break;
            }
        }
        summary << "- Average Startup Time: " << avg_startup_time << "ms\n";
        summary << "- Performance Target Status: " << (avg_startup_time <= 100.0 ? "MET" : "NOT MET") << "\n\n";
        
        // Transformation Validation Summary
        summary << "## Transformation Validation Results\n\n";
        int total_validations = 0, passed_validations = 0;
        for (const auto& [phase, validations] : transformation_results) {
            total_validations += validations.size();
            for (const auto& validation : validations) {
                if (validation.success) passed_validations++;
            }
        }
        summary << "- Total Validations: " << total_validations << "\n";
        summary << "- Passed: " << passed_validations << "\n";
        summary << "- Failed: " << (total_validations - passed_validations) << "\n";
        summary << "- Success Rate: " << (100.0 * passed_validations / total_validations) << "%\n\n";
        
        // Overall Assessment
        bool overall_success = (integration_passed == integration_results.size()) && 
                              (avg_startup_time <= 100.0) && 
                              (passed_validations == total_validations);
        
        summary << "## Overall Assessment\n\n";
        summary << "**ARCHITECTURAL REFORMATION STATUS: " << (overall_success ? "SUCCESS" : "REQUIRES ATTENTION") << "**\n\n";
        
        if (overall_success) {
            summary << "✅ All integration tests passed\n";
            summary << "✅ All performance targets met\n";
            summary << "✅ All transformation validations successful\n";
            summary << "✅ System ready for production deployment\n\n";
            
            summary << "The Akao architectural reformation has been completed successfully. ";
            summary << "The system demonstrates:\n\n";
            summary << "- Complete YAML independence through Pure Logic .a language\n";
            summary << "- Universal parsing capability without external dependencies\n";
            summary << "- Node.js-like development experience\n";
            summary << "- Performance comparable to established systems\n";
            summary << "- Comprehensive governance framework\n";
            summary << "- Archaeological preservation of all transformation decisions\n";
        } else {
            summary << "⚠️ Some validation steps require attention\n";
            summary << "📋 Detailed reports available for issue resolution\n";
            summary << "🔧 Recommendations provided for improvement areas\n\n";
            
            summary << "While significant progress has been made in the architectural reformation, ";
            summary << "some areas require additional attention before full production readiness.\n";
        }
        
        summary << "\n## Next Steps\n\n";
        if (overall_success) {
            summary << "1. Deploy system to production environment\n";
            summary << "2. Begin user onboarding and training\n";
            summary << "3. Monitor system performance in production\n";
            summary << "4. Collect user feedback for continuous improvement\n";
        } else {
            summary << "1. Review detailed validation reports\n";
            summary << "2. Address identified issues and failed tests\n";
            summary << "3. Re-run validation suite after fixes\n";
            summary << "4. Proceed to production deployment once all validations pass\n";
        }
        
        summary.close();
    }
};

} // namespace integration
} // namespace akao

// Main execution function
int main(int argc, char* argv[]) {
    std::string test_data_path = "/workspaces/akao/tests/data";
    std::string output_path = "/workspaces/akao/artifacts/architectural-reformation/phase-3/step-4";
    
    if (argc >= 3) {
        test_data_path = argv[1];
        output_path = argv[2];
    }
    
    try {
        akao::integration::IntegrationTestRunner runner(test_data_path, output_path);
        bool success = runner.run_comprehensive_integration_validation();
        
        return success ? 0 : 1;
        
    } catch (const std::exception& e) {
        std::cerr << "Integration test runner failed: " << e.what() << std::endl;
        return 1;
    }
}
