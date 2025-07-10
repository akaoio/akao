/**
 * @id: akao:file:main:v1
 * @doc: Simplified main entry point for new node-based workflow system. Provides basic command-line interface for testing core data types and future workflow execution.
 * @specification: Main entry point for node-based workflow dispatcher
 * @scope: Application entry point
 * @timeline: 2025-07-10
 * @rationale: Provide command-line interface for revolutionary workflow system
 * @methodology: Simple dispatcher with extensible command structure
 * @references: ["akao:class:core:foundation:types:nodevalue:v1"]
 */

#include "core/foundation/types/value/v1.hpp"
#include "core/foundation/types/result/v1.hpp"
#include <iostream>
#include <string>
#include <vector>

using namespace akao::foundation::types;

void testCoreTypes() {
    std::cout << "Testing core data types..." << std::endl;
    
    // Test NodeValue
    NodeValue str("Hello, Akao!");
    NodeValue num(static_cast<int64_t>(42));
    NodeValue flag(true);
    
    std::cout << "String: " << str.toString() << std::endl;
    std::cout << "Number: " << num.toString() << std::endl;
    std::cout << "Boolean: " << flag.toString() << std::endl;
    
    // Test ExecutionResult
    auto result = ExecutionResult::success(str);
    std::cout << "Result status: " << result.getStatusString() << std::endl;
    std::cout << "Result data: " << result.getData().toString() << std::endl;
    
    auto errorResult = ExecutionResult::error("TEST_ERROR", "This is a test error");
    std::cout << "Error result: " << errorResult.toString() << std::endl;
    
    std::cout << "Core types test completed successfully!" << std::endl;
}

void printUsage(const std::string& programName) {
    std::cout << "Akao Node-Based Workflow System" << std::endl;
    std::cout << "Usage: " << programName << " [command] [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  test        Run core type tests" << std::endl;
    std::cout << "  version     Show version information" << std::endl;
    std::cout << "  help        Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Phase 1 (Foundation) - Core data types implemented" << std::endl;
}

int main(int argc, char* argv[]) {
    std::vector<std::string> args(argv, argv + argc);
    
    if (argc < 2) {
        printUsage(args[0]);
        return 0;
    }
    
    std::string command = args[1];
    
    if (command == "test") {
        try {
            testCoreTypes();
            return 0;
        } catch (const std::exception& e) {
            std::cerr << "Test failed: " << e.what() << std::endl;
            return 1;
        }
    } else if (command == "version" || command == "--version") {
        std::cout << "Akao Node-Based Workflow System v1.0.0" << std::endl;
        std::cout << "Phase 1: Foundation - Core data types implemented" << std::endl;
        return 0;
    } else if (command == "help" || command == "--help") {
        printUsage(args[0]);
        return 0;
    } else {
        std::cerr << "Unknown command: " << command << std::endl;
        printUsage(args[0]);
        return 1;
    }
    
    return 0;
}