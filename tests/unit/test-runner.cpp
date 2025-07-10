/**
 * @id: akao:test:unit:test-runner:v1
 * @doc: Unified test runner for all unit tests in the Akao system. Executes comprehensive test suites for core components, foundation types, interfaces, and node implementations. Provides centralized test execution with proper error reporting.
 * @specification: Unified test runner with comprehensive test coverage
 * @scope: Unit test execution framework
 * @timeline: 2025-07-10
 * @rationale: Centralized test execution for all system components
 * @methodology: Single main function calling all test suites
 * @references: ["akao:test:nodes:builtin:file:v1", "akao:test:nodes:builtin:logic:v1"]
 */

#include <iostream>
#include <exception>

// Test function declarations for all node tests
namespace akao { namespace tests {
void runFilesystemScannerTests();
void runAkaoLogicExecutorTests();
void runYamlProcessorTests();
void runReporterTests();
}}

int main() {
    std::cout << "===============================================\n";
    std::cout << "         Akao Unit Test Suite\n";
    std::cout << "===============================================\n\n";
    
    int testsPassed = 0;
    int testsFailed = 0;
    
    try {
        std::cout << "Running Filesystem Scanner Node tests...\n";
        akao::tests::runFilesystemScannerTests();
        testsPassed++;
        std::cout << "✓ Filesystem Scanner Node tests PASSED\n\n";
    } catch (const std::exception& e) {
        testsFailed++;
        std::cout << "✗ Filesystem Scanner Node tests FAILED: " << e.what() << "\n\n";
    }
    
    try {
        std::cout << "Running Akao Logic Executor Node tests...\n";
        akao::tests::runAkaoLogicExecutorTests();
        testsPassed++;
        std::cout << "✓ Akao Logic Executor Node tests PASSED\n\n";
    } catch (const std::exception& e) {
        testsFailed++;
        std::cout << "✗ Akao Logic Executor Node tests FAILED: " << e.what() << "\n\n";
    }
    
    try {
        std::cout << "Running YAML Processor Node tests...\n";
        akao::tests::runYamlProcessorTests();
        testsPassed++;
        std::cout << "✓ YAML Processor Node tests PASSED\n\n";
    } catch (const std::exception& e) {
        testsFailed++;
        std::cout << "✗ YAML Processor Node tests FAILED: " << e.what() << "\n\n";
    }
    
    try {
        std::cout << "Running Reporter Node tests...\n";
        akao::tests::runReporterTests();
        testsPassed++;
        std::cout << "✓ Reporter Node tests PASSED\n\n";
    } catch (const std::exception& e) {
        testsFailed++;
        std::cout << "✗ Reporter Node tests FAILED: " << e.what() << "\n\n";
    }
    
    std::cout << "===============================================\n";
    std::cout << "            Test Summary\n";
    std::cout << "===============================================\n";
    std::cout << "Tests Passed: " << testsPassed << "\n";
    std::cout << "Tests Failed: " << testsFailed << "\n";
    
    if (testsFailed == 0) {
        std::cout << "🎉 ALL TESTS PASSED!\n";
        return 0;
    } else {
        std::cout << "❌ SOME TESTS FAILED!\n";
        return 1;
    }
}