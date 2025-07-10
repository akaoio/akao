/**
 * @id: akao:file:tests:unit:core:foundation:types:result:v1
 * @doc: Comprehensive unit tests for ExecutionResult container. Tests status handling, error aggregation, timing operations, and result composition for reliable workflow execution tracking.
 * @specification: Unit tests for ExecutionResult class functionality
 * @scope: Foundation data types testing
 * @timeline: 2025-07-10
 * @rationale: Ensure reliability of execution result tracking mechanism
 * @methodology: Complete test coverage with error scenarios and edge cases
 * @references: ["akao:class:core:foundation:types:executionresult:v1"]
 */

#include "core/foundation/types/result/v1.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>

using namespace akao::foundation::types;

void testBasicConstruction() {
    std::cout << "Testing basic construction..." << std::endl;
    
    // Default constructor
    ExecutionResult result1;
    assert(result1.getStatus() == ExecutionResult::Status::SUCCESS);
    assert(result1.isSuccess());
    assert(!result1.isError());
    
    // Status constructor
    ExecutionResult result2(ExecutionResult::Status::ERROR);
    assert(result2.getStatus() == ExecutionResult::Status::ERROR);
    assert(!result2.isSuccess());
    assert(result2.isError());
    
    // Status with data
    NodeValue data("test data");
    ExecutionResult result3(ExecutionResult::Status::SUCCESS, data);
    assert(result3.isSuccess());
    assert(result3.getData().asString() == "test data");
    
    std::cout << "Basic construction test passed!" << std::endl;
}

void testStaticFactoryMethods() {
    std::cout << "Testing static factory methods..." << std::endl;
    
    // Success
    auto success1 = ExecutionResult::success();
    assert(success1.isSuccess());
    
    NodeValue data(static_cast<int64_t>(42));
    auto success2 = ExecutionResult::success(data);
    assert(success2.isSuccess());
    assert(success2.getData().asInteger() == 42);
    
    // Error
    auto error1 = ExecutionResult::error("Something went wrong");
    assert(error1.isError());
    assert(error1.getErrors().size() == 1);
    assert(error1.getErrors()[0].message == "Something went wrong");
    
    auto error2 = ExecutionResult::error("ERR_001", "Detailed error");
    assert(error2.isError());
    assert(error2.getErrors()[0].code == "ERR_001");
    assert(error2.getErrors()[0].message == "Detailed error");
    
    auto error3 = ExecutionResult::error("ERR_002", "Error with details", "Stack trace here");
    assert(error3.isError());
    assert(error3.getErrors()[0].details == "Stack trace here");
    
    // Warning
    auto warning = ExecutionResult::warning(data, "This is a warning");
    assert(warning.getStatus() == ExecutionResult::Status::WARNING);
    assert(warning.hasWarnings());
    assert(warning.getWarnings()[0] == "This is a warning");
    
    // Skipped
    auto skipped = ExecutionResult::skipped("Not applicable");
    assert(skipped.getStatus() == ExecutionResult::Status::SKIPPED);
    
    // Timeout
    auto timeout = ExecutionResult::timeout();
    assert(timeout.getStatus() == ExecutionResult::Status::TIMEOUT);
    
    // Cancelled
    auto cancelled = ExecutionResult::cancelled();
    assert(cancelled.getStatus() == ExecutionResult::Status::CANCELLED);
    
    std::cout << "Static factory methods test passed!" << std::endl;
}

void testErrorOperations() {
    std::cout << "Testing error operations..." << std::endl;
    
    ExecutionResult result;
    
    // Add errors
    result.addError("First error");
    assert(result.isError());
    assert(result.getErrors().size() == 1);
    
    result.addError("ERR_001", "Second error");
    result.addError("ERR_002", "Third error", "Detailed info");
    assert(result.getErrors().size() == 3);
    
    // Error summary
    std::string summary = result.getErrorSummary();
    assert(summary.find("ERR_001") != std::string::npos);
    assert(summary.find("ERR_002") != std::string::npos);
    
    // Clear errors
    result.clearErrors();
    assert(result.getErrors().empty());
    
    std::cout << "Error operations test passed!" << std::endl;
}

void testWarningOperations() {
    std::cout << "Testing warning operations..." << std::endl;
    
    ExecutionResult result;
    
    // Add warnings
    result.addWarning("First warning");
    result.addWarning("Second warning");
    
    assert(result.hasWarnings());
    assert(result.getWarnings().size() == 2);
    assert(result.getWarnings()[0] == "First warning");
    assert(result.getWarnings()[1] == "Second warning");
    
    // Clear warnings
    result.clearWarnings();
    assert(!result.hasWarnings());
    assert(result.getWarnings().empty());
    
    std::cout << "Warning operations test passed!" << std::endl;
}

void testTimingOperations() {
    std::cout << "Testing timing operations..." << std::endl;
    
    ExecutionResult result;
    
    // Start timing
    result.startTiming();
    
    // Simulate some work
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    // End timing
    result.endTiming();
    
    auto duration = result.getDuration();
    assert(duration.count() >= 10); // At least 10ms
    assert(duration.count() < 1000); // Less than 1 second
    
    std::cout << "Timing operations test passed!" << std::endl;
}

void testDataOperations() {
    std::cout << "Testing data operations..." << std::endl;
    
    ExecutionResult result;
    
    // Set data
    NodeValue data("test result");
    result.setData(data);
    assert(result.getData().asString() == "test result");
    
    // Mutable data
    result.getMutableData() = NodeValue(static_cast<int64_t>(42));
    assert(result.getData().asInteger() == 42);
    
    std::cout << "Data operations test passed!" << std::endl;
}

void testNodeIdOperations() {
    std::cout << "Testing node ID operations..." << std::endl;
    
    ExecutionResult result;
    
    // Set node ID
    result.setNodeId("test-node-123");
    assert(result.getNodeId() == "test-node-123");
    
    std::cout << "Node ID operations test passed!" << std::endl;
}

void testResultComposition() {
    std::cout << "Testing result composition..." << std::endl;
    
    ExecutionResult result1;
    result1.addError("Error 1");
    result1.addWarning("Warning 1");
    
    ExecutionResult result2;
    result2.addError("Error 2");
    result2.addWarning("Warning 2");
    
    // Merge
    result1.merge(result2);
    assert(result1.getErrors().size() == 2);
    assert(result1.getWarnings().size() == 2);
    assert(result1.isError());
    
    // Combine
    ExecutionResult result3;
    result3.addWarning("Warning 3");
    
    ExecutionResult result4;
    result4.addWarning("Warning 4");
    
    auto combined = result3.combine(result4);
    assert(combined.getWarnings().size() == 2);
    assert(combined.getStatus() == ExecutionResult::Status::WARNING);
    
    std::cout << "Result composition test passed!" << std::endl;
}

void testStringRepresentation() {
    std::cout << "Testing string representation..." << std::endl;
    
    ExecutionResult result;
    result.setNodeId("test-node");
    result.addError("Test error");
    result.addWarning("Test warning");
    
    std::string str = result.toString();
    assert(str.find("ERROR") != std::string::npos);
    assert(str.find("test-node") != std::string::npos);
    
    assert(result.getStatusString() == "ERROR");
    
    std::cout << "String representation test passed!" << std::endl;
}

void testOperators() {
    std::cout << "Testing operators..." << std::endl;
    
    ExecutionResult success1 = ExecutionResult::success();
    ExecutionResult success2 = ExecutionResult::success();
    ExecutionResult error1 = ExecutionResult::error("Test error");
    
    // Equality
    assert(success1 == success2);
    assert(success1 != error1);
    
    // Boolean conversion
    assert(static_cast<bool>(success1) == true);
    assert(static_cast<bool>(error1) == false);
    
    std::cout << "Operators test passed!" << std::endl;
}

int main() {
    std::cout << "Running ExecutionResult unit tests..." << std::endl;
    
    try {
        testBasicConstruction();
        testStaticFactoryMethods();
        testErrorOperations();
        testWarningOperations();
        testTimingOperations();
        testDataOperations();
        testNodeIdOperations();
        testResultComposition();
        testStringRepresentation();
        testOperators();
        
        std::cout << "All ExecutionResult tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
}