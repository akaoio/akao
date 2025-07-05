#include "compliance/self_validation_test.hpp"
#include "compliance/philosophy_compliance_test.hpp"
#include <iostream>
#include <string>
#include <vector>

/**
 * @brief Test runner for Akao compliance tests
 * 
 * Runs self-validation and philosophy compliance tests
 * to ensure Akao truly embodies its own principles.
 */

void printUsage(const std::string& program_name) {
    std::cout << "Akao Compliance Test Runner\n";
    std::cout << "Usage: " << program_name << " <test-type>\n\n";
    std::cout << "Test Types:\n";
    std::cout << "  self-validation        Run self-validation tests\n";
    std::cout << "  philosophy-compliance  Run philosophy compliance tests\n";
    std::cout << "  all                    Run all tests\n";
    std::cout << "  --help                 Show this help\n\n";
}

int runSelfValidationTests() {
    std::cout << "=== Akao Self-Validation Tests ===\n\n";
    
    akao::tests::compliance::SelfValidationTest test_suite;
    
    if (!test_suite.initialize()) {
        std::cerr << "Failed to initialize self-validation test suite" << std::endl;
        return 1;
    }
    
    auto results = test_suite.runAllTests();
    
    // Generate report
    std::string report = test_suite.generateReport(results);
    std::cout << report << std::endl;
    
    // Return exit code based on results
    return test_suite.allTestsPassed(results) ? 0 : 1;
}

int runPhilosophyComplianceTests() {
    std::cout << "=== Akao Philosophy Compliance Tests ===\n\n";
    
    akao::tests::compliance::PhilosophyComplianceTest test_suite;
    
    if (!test_suite.initialize()) {
        std::cerr << "Failed to initialize philosophy compliance test suite" << std::endl;
        return 1;
    }
    
    auto results = test_suite.testAllPhilosophies();
    
    // Generate report
    std::string report = test_suite.generateComplianceReport(results);
    std::cout << report << std::endl;
    
    // Return exit code based on results
    return test_suite.allPhilosophiesCompliant(results) ? 0 : 1;
}

int runAllTests() {
    std::cout << "=== Akao Complete Compliance Test Suite ===\n\n";
    
    int self_validation_result = runSelfValidationTests();
    std::cout << "\n" << std::string(80, '=') << "\n\n";
    
    int philosophy_compliance_result = runPhilosophyComplianceTests();
    std::cout << "\n" << std::string(80, '=') << "\n\n";
    
    // Summary
    std::cout << "=== Overall Test Results ===\n";
    std::cout << "Self-Validation: " << (self_validation_result == 0 ? "PASSED" : "FAILED") << "\n";
    std::cout << "Philosophy Compliance: " << (philosophy_compliance_result == 0 ? "PASSED" : "FAILED") << "\n";
    
    bool all_passed = (self_validation_result == 0) && (philosophy_compliance_result == 0);
    
    if (all_passed) {
        std::cout << "\n🎉 ALL TESTS PASSED - Akao fully embodies its own philosophies and rules!\n";
        std::cout << "The 'rules govern rules' principle is satisfied.\n";
    } else {
        std::cout << "\n❌ SOME TESTS FAILED - Akao needs improvements to fully embody its principles.\n";
        std::cout << "The 'rules govern rules' principle requires all tests to pass.\n";
    }
    
    return all_passed ? 0 : 1;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::string test_type(argv[1]);
    
    try {
        if (test_type == "--help" || test_type == "-h") {
            printUsage(argv[0]);
            return 0;
        } else if (test_type == "self-validation") {
            return runSelfValidationTests();
        } else if (test_type == "philosophy-compliance") {
            return runPhilosophyComplianceTests();
        } else if (test_type == "all") {
            return runAllTests();
        } else {
            std::cerr << "Unknown test type: " << test_type << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Test execution failed: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred during test execution" << std::endl;
        return 1;
    }
}
