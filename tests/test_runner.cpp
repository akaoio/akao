#include "cli/cli_command_tests.hpp"
#include "compliance/self_validation_test.hpp"
#include "unit/universal_validator_test.hpp"
#include <iostream>
#include <iomanip>

void printTestResult(const std::string& test_name, bool passed, const std::string& error = "") {
    std::cout << "  " << std::left << std::setw(40) << test_name << " ";
    if (passed) {
        std::cout << "✓ PASS" << std::endl;
    } else {
        std::cout << "✗ FAIL";
        if (!error.empty()) {
            std::cout << " - " << error;
        }
        std::cout << std::endl;
    }
}

void printSuiteResults(const std::string& suite_name, 
                      int passed, int failed, bool overall_success) {
    std::cout << "\n" << suite_name << " Results:\n";
    std::cout << "  Tests Passed: " << passed << "\n";
    std::cout << "  Tests Failed: " << failed << "\n";
    std::cout << "  Overall: " << (overall_success ? "✓ SUCCESS" : "✗ FAILURE") << "\n\n";
}

int main() {
    std::cout << "🧪 Akao Framework Test Suite\n";
    std::cout << "=============================\n\n";
    
    int total_passed = 0;
    int total_failed = 0;
    
    // Run CLI Command Tests
    std::cout << "Running CLI Command Tests...\n";
    akao::tests::cli::CLICommandTests cli_tests;
    auto cli_results = cli_tests.runAllTests();
    
    for (const auto& result : cli_results.test_results) {
        printTestResult(result.test_name, result.passed, result.error_message);
        
        // Print details for passed tests
        if (result.passed && !result.details.empty()) {
            for (const auto& detail : result.details) {
                std::cout << "    " << detail << "\n";
            }
        }
    }
    
    printSuiteResults("CLI Commands", cli_results.tests_passed, 
                     cli_results.tests_failed, cli_results.overall_success);
    
    total_passed += cli_results.tests_passed;
    total_failed += cli_results.tests_failed;
    
    // Run Self-Validation Tests
    std::cout << "Running Self-Validation Tests...\n";
    akao::tests::compliance::SelfValidationTest self_tests;
    auto self_results = self_tests.runAllTests();
    
    for (const auto& result : self_results.test_results) {
        printTestResult(result.test_name, result.passed, result.error_message);
    }
    
    printSuiteResults("Self-Validation", self_results.tests_passed, 
                     self_results.tests_failed, self_results.overall_success);
    
    total_passed += self_results.tests_passed;
    total_failed += self_results.tests_failed;
    
    // Run Unit Tests
    std::cout << "Running Unit Tests...\n";
    akao::tests::unit::UniversalValidatorTest unit_tests;
    auto unit_results = unit_tests.runAllTests();
    
    for (const auto& result : unit_results) {
        printTestResult(result.test_name, result.passed, result.error_message);
    }
    
    int unit_passed = 0, unit_failed = 0;
    for (const auto& result : unit_results) {
        if (result.passed) unit_passed++;
        else unit_failed++;
    }
    
    printSuiteResults("Unit Tests", unit_passed, unit_failed, unit_failed == 0);
    
    total_passed += unit_passed;
    total_failed += unit_failed;
    
    // Overall Results
    std::cout << "==========================================\n";
    std::cout << "Overall Test Results:\n";
    std::cout << "  Total Tests Passed: " << total_passed << "\n";
    std::cout << "  Total Tests Failed: " << total_failed << "\n";
    std::cout << "  Success Rate: " << std::fixed << std::setprecision(1) 
              << (100.0 * total_passed / (total_passed + total_failed)) << "%\n";
    
    bool overall_success = (total_failed == 0);
    std::cout << "  Overall Status: " << (overall_success ? "✓ ALL TESTS PASSED" : "✗ SOME TESTS FAILED") << "\n";
    
    if (overall_success) {
        std::cout << "\n🎉 Akao Framework is ready for production!\n";
    } else {
        std::cout << "\n⚠️  Some tests failed. Please review and fix before production use.\n";
    }
    
    return overall_success ? 0 : 1;
}
