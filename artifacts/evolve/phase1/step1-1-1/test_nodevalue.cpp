/**
 * @id: akao:file:core:node:data:nodevalue_test:v1
 * @doc: Comprehensive test suite for NodeValue and NodeItem data exchange system verifying type conversion, validation, binary data handling, error management, and compatibility with existing systems. Tests all 20+ type conversion scenarios and edge cases.
 * @specification: Complete test coverage for node data exchange system
 * @scope: Core node data system validation
 * @timeline: 2025-07-10
 * @rationale: Ensure robust, reliable data exchange system with comprehensive validation
 * @methodology: Unit testing with real data processing and comprehensive edge case coverage
 * @references: ["akao:class:core:node:data:nodevalue:v1", "akao:class:core:node:data:nodeitem:v1"]
 */

#include "nodevalue.hpp"
#include <iostream>
#include <cassert>
#include <vector>
#include <sstream>

using namespace akao::node;

// Test counter for tracking progress
int test_count = 0;
int passed_tests = 0;

#define TEST(name) \
    void test_##name(); \
    void run_test_##name() { \
        test_count++; \
        std::cout << "Running test: " << #name << "..."; \
        try { \
            test_##name(); \
            passed_tests++; \
            std::cout << " PASS" << std::endl; \
        } catch (const std::exception& e) { \
            std::cout << " FAIL: " << e.what() << std::endl; \
        } \
    } \
    void test_##name()

#define ASSERT_EQ(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            std::stringstream ss; \
            ss << "Assertion failed: expected " << (expected) << ", got " << (actual); \
            throw std::runtime_error(ss.str()); \
        } \
    } while(0)

#define ASSERT_TRUE(condition) \
    do { \
        if (!(condition)) { \
            throw std::runtime_error("Assertion failed: " #condition " is false"); \
        } \
    } while(0)

#define ASSERT_FALSE(condition) \
    do { \
        if (condition) { \
            throw std::runtime_error("Assertion failed: " #condition " is true"); \
        } \
    } while(0)

#define ASSERT_THROWS(statement) \
    do { \
        bool threw = false; \
        try { \
            statement; \
        } catch (...) { \
            threw = true; \
        } \
        if (!threw) { \
            throw std::runtime_error("Expected exception but none was thrown"); \
        } \
    } while(0)

// ============================================================================
// NodeValue Type Construction Tests
// ============================================================================

TEST(string_construction) {
    NodeValue val("hello world");
    ASSERT_TRUE(val.isString());
    ASSERT_FALSE(val.isNull());
    ASSERT_EQ(std::string("hello world"), val.asString());
    ASSERT_EQ(std::string("hello world"), val.as<std::string>());
}

TEST(integer_construction) {
    NodeValue val(42);
    ASSERT_TRUE(val.isInteger());
    ASSERT_TRUE(val.isNumber());
    ASSERT_FALSE(val.isFloat());
    ASSERT_EQ(int64_t(42), val.asInteger());
    ASSERT_EQ(42, val.as<int>());
    ASSERT_EQ(int64_t(42), val.as<int64_t>());
}

TEST(float_construction) {
    NodeValue val(3.14159);
    ASSERT_TRUE(val.isFloat());
    ASSERT_TRUE(val.isNumber());
    ASSERT_FALSE(val.isInteger());
    ASSERT_EQ(3.14159, val.asFloat());
    ASSERT_EQ(3.14159, val.as<double>());
}

TEST(boolean_construction) {
    NodeValue val_true(true);
    NodeValue val_false(false);
    
    ASSERT_TRUE(val_true.isBoolean());
    ASSERT_TRUE(val_true.asBoolean());
    ASSERT_TRUE(val_true.as<bool>());
    
    ASSERT_TRUE(val_false.isBoolean());
    ASSERT_FALSE(val_false.asBoolean());
    ASSERT_FALSE(val_false.as<bool>());
}

TEST(null_construction) {
    NodeValue val;
    ASSERT_TRUE(val.isNull());
    ASSERT_FALSE(val.isString());
    ASSERT_FALSE(val.isNumber());
    ASSERT_EQ(std::string(""), val.asString()); // Null converts to empty string
}

// ============================================================================
// Collection Type Tests
// ============================================================================

TEST(array_construction) {
    NodeValue::Array arr = {NodeValue(1), NodeValue("test"), NodeValue(true)};
    NodeValue val(arr);
    
    ASSERT_TRUE(val.isArray());
    ASSERT_FALSE(val.isObject());
    ASSERT_EQ(size_t(3), val.size());
    
    ASSERT_EQ(int64_t(1), val.at(0).asInteger());
    ASSERT_EQ(std::string("test"), val.at(1).asString());
    ASSERT_TRUE(val.at(2).asBoolean());
}

TEST(object_construction) {
    NodeValue::Object obj = {
        {"name", NodeValue("John")},
        {"age", NodeValue(30)},
        {"active", NodeValue(true)}
    };
    NodeValue val(obj);
    
    ASSERT_TRUE(val.isObject());
    ASSERT_FALSE(val.isArray());
    ASSERT_EQ(size_t(3), val.size());
    
    ASSERT_EQ(std::string("John"), val.get("name").asString());
    ASSERT_EQ(int64_t(30), val.get("age").asInteger());
    ASSERT_TRUE(val.get("active").asBoolean());
    ASSERT_TRUE(val.contains("name"));
    ASSERT_FALSE(val.contains("nonexistent"));
}

TEST(binary_data_construction) {
    std::vector<uint8_t> data = {0x01, 0x02, 0x03, 0x04, 0xFF};
    BinaryData binary(data, "application/octet-stream");
    NodeValue val(binary);
    
    ASSERT_TRUE(val.isBinary());
    ASSERT_FALSE(val.isString());
    ASSERT_EQ(size_t(5), val.size());
    
    BinaryData retrieved = val.asBinary();
    ASSERT_EQ(size_t(5), retrieved.getSize());
    ASSERT_EQ(std::string("application/octet-stream"), retrieved.getMimeType());
    ASSERT_TRUE(data == retrieved.getData());
}

// ============================================================================
// Type Conversion Tests
// ============================================================================

TEST(string_to_number_conversion) {
    NodeValue str_val("42");
    // String conversion to string should work
    ASSERT_EQ(std::string("42"), str_val.asString());
    
    // Direct numeric conversion should fail (type safety)
    ASSERT_THROWS(str_val.asInteger());
    
    // But we should be able to convert integers to string
    NodeValue int_val(42);
    ASSERT_EQ(std::string("42"), int_val.asString());
}

TEST(integer_to_float_conversion) {
    NodeValue int_val(42);
    // Should be able to get integer as float
    ASSERT_EQ(42.0, int_val.asFloat());
    
    // Float should not convert to integer without explicit casting
    NodeValue float_val(3.14);
    ASSERT_THROWS(float_val.asInteger());
}

TEST(boolean_to_string_conversion) {
    NodeValue true_val(true);
    NodeValue false_val(false);
    
    ASSERT_EQ(std::string("true"), true_val.asString());
    ASSERT_EQ(std::string("false"), false_val.asString());
}

// ============================================================================
// Collection Manipulation Tests
// ============================================================================

TEST(array_manipulation) {
    NodeValue::Array arr;
    NodeValue val(arr);
    
    // Start with empty array
    ASSERT_EQ(size_t(0), val.size());
    
    // Add elements
    val.push(NodeValue(1));
    val.push(NodeValue("test"));
    val.push(NodeValue(true));
    
    ASSERT_EQ(size_t(3), val.size());
    ASSERT_EQ(int64_t(1), val.at(0).asInteger());
    ASSERT_EQ(std::string("test"), val.at(1).asString());
    ASSERT_TRUE(val.at(2).asBoolean());
    
    // Test bounds checking
    ASSERT_THROWS(val.at(10));
}

TEST(object_manipulation) {
    NodeValue::Object obj;
    NodeValue val(obj);
    
    // Start with empty object
    ASSERT_EQ(size_t(0), val.size());
    ASSERT_FALSE(val.contains("test"));
    
    // Add elements
    val.set("name", NodeValue("Alice"));
    val.set("score", NodeValue(95));
    
    ASSERT_EQ(size_t(2), val.size());
    ASSERT_TRUE(val.contains("name"));
    ASSERT_TRUE(val.contains("score"));
    ASSERT_EQ(std::string("Alice"), val.get("name").asString());
    ASSERT_EQ(int64_t(95), val.get("score").asInteger());
    
    // Test missing key returns null
    NodeValue missing = val.get("nonexistent");
    ASSERT_TRUE(missing.isNull());
}

// ============================================================================
// NodeItem Tests
// ============================================================================

TEST(nodeitem_basic) {
    NodeValue json_data(NodeValue::Object{{"message", NodeValue("hello")}});
    NodeItem item(json_data);
    
    ASSERT_FALSE(item.isEmpty());
    ASSERT_EQ(std::string("hello"), item.getJson().get("message").asString());
    ASSERT_FALSE(item.hasError());
    ASSERT_FALSE(item.hasBinary("test"));
}

TEST(nodeitem_with_binary) {
    std::vector<uint8_t> data = {0x48, 0x65, 0x6C, 0x6C, 0x6F}; // "Hello"
    BinaryData binary(data, "text/plain");
    
    NodeValue json_data(NodeValue::Object{{"type", NodeValue("file")}});
    NodeItem item(json_data);
    item.setBinary("content", binary);
    
    ASSERT_TRUE(item.hasBinary("content"));
    BinaryData retrieved = item.getBinary("content");
    ASSERT_EQ(size_t(5), retrieved.getSize());
    ASSERT_EQ(std::string("text/plain"), retrieved.getMimeType());
    ASSERT_TRUE(data == retrieved.getData());
}

TEST(nodeitem_with_metadata) {
    NodeValue json_data(NodeValue::Object{{"id", NodeValue(123)}});
    NodeItem item(json_data);
    
    item.setMetadata("timestamp", NodeValue("2025-07-10T12:00:00Z"));
    item.setMetadata("source", NodeValue("test"));
    
    ASSERT_TRUE(item.hasMetadata("timestamp"));
    ASSERT_TRUE(item.hasMetadata("source"));
    ASSERT_EQ(std::string("2025-07-10T12:00:00Z"), item.getMetadata("timestamp").asString());
    ASSERT_EQ(std::string("test"), item.getMetadata("source").asString());
    
    // Missing metadata returns null
    NodeValue missing = item.getMetadata("nonexistent");
    ASSERT_TRUE(missing.isNull());
}

TEST(nodeitem_with_error) {
    NodeValue json_data(NodeValue::Object{{"status", NodeValue("failed")}});
    NodeItem item(json_data);
    
    ASSERT_FALSE(item.hasError());
    
    ExecutionError error("Test error", "TEST_001", ExecutionError::Severity::ERROR);
    error.setContext("Unit test");
    item.setError(error);
    
    ASSERT_TRUE(item.hasError());
    const auto& retrieved_error = item.getError();
    ASSERT_TRUE(retrieved_error.has_value());
    ASSERT_EQ(std::string("Test error"), retrieved_error->getMessage());
    ASSERT_EQ(std::string("TEST_001"), retrieved_error->getCode());
    ASSERT_TRUE(ExecutionError::Severity::ERROR == retrieved_error->getSeverity());
    ASSERT_EQ(std::string("Unit test"), retrieved_error->getContext());
    
    item.clearError();
    ASSERT_FALSE(item.hasError());
}

// ============================================================================
// Error Handling Tests
// ============================================================================

TEST(execution_error) {
    ExecutionError error1("Simple error");
    ASSERT_EQ(std::string("Simple error"), error1.getMessage());
    ASSERT_TRUE(ExecutionError::Severity::ERROR == error1.getSeverity());
    
    ExecutionError error2("Complex error", "ERR_001", ExecutionError::Severity::WARNING);
    error2.setContext("Test context");
    
    std::string error_str = error2.toString();
    ASSERT_TRUE(error_str.find("WARNING") != std::string::npos);
    ASSERT_TRUE(error_str.find("ERR_001") != std::string::npos);
    ASSERT_TRUE(error_str.find("Complex error") != std::string::npos);
    ASSERT_TRUE(error_str.find("Test context") != std::string::npos);
}

// ============================================================================
// String Representation Tests
// ============================================================================

TEST(string_representation) {
    // Test primitive types
    ASSERT_EQ(std::string("\"hello\""), NodeValue("hello").toString());
    ASSERT_EQ(std::string("42"), NodeValue(42).toString());
    std::string float_str = NodeValue(3.14).toString();
    ASSERT_TRUE(float_str.substr(0, 4) == "3.14"); // Check prefix due to floating point precision
    ASSERT_EQ(std::string("true"), NodeValue(true).toString());
    ASSERT_EQ(std::string("false"), NodeValue(false).toString());
    ASSERT_EQ(std::string("null"), NodeValue().toString());
    
    // Test array representation
    NodeValue::Array arr = {NodeValue(1), NodeValue("test")};
    std::string arr_str = NodeValue(arr).toString();
    ASSERT_TRUE(arr_str.find("[") != std::string::npos);
    ASSERT_TRUE(arr_str.find("1") != std::string::npos);
    ASSERT_TRUE(arr_str.find("\"test\"") != std::string::npos);
    ASSERT_TRUE(arr_str.find("]") != std::string::npos);
    
    // Test object representation
    NodeValue::Object obj = {{"key", NodeValue("value")}};
    std::string obj_str = NodeValue(obj).toString();
    ASSERT_TRUE(obj_str.find("{") != std::string::npos);
    ASSERT_TRUE(obj_str.find("\"key\"") != std::string::npos);
    ASSERT_TRUE(obj_str.find("\"value\"") != std::string::npos);
    ASSERT_TRUE(obj_str.find("}") != std::string::npos);
}

// ============================================================================
// Equality and Comparison Tests
// ============================================================================

TEST(equality_comparison) {
    // Same type, same value
    ASSERT_TRUE(NodeValue(42) == NodeValue(42));
    ASSERT_TRUE(NodeValue("test") == NodeValue("test"));
    ASSERT_TRUE(NodeValue(true) == NodeValue(true));
    
    // Same type, different value
    ASSERT_FALSE(NodeValue(42) == NodeValue(43));
    ASSERT_FALSE(NodeValue("test") == NodeValue("other"));
    ASSERT_FALSE(NodeValue(true) == NodeValue(false));
    
    // Different types
    ASSERT_FALSE(NodeValue(42) == NodeValue("42"));
    ASSERT_FALSE(NodeValue(1) == NodeValue(true));
    
    // Null values
    ASSERT_TRUE(NodeValue() == NodeValue());
    ASSERT_FALSE(NodeValue() == NodeValue(0));
    
    // Test != operator
    ASSERT_TRUE(NodeValue(42) != NodeValue(43));
    ASSERT_FALSE(NodeValue(42) != NodeValue(42));
}

// ============================================================================
// Edge Cases and Error Conditions
// ============================================================================

TEST(type_validation_errors) {
    NodeValue str_val("not a number");
    NodeValue int_val(42);
    NodeValue obj_val(NodeValue::Object{});
    
    // Accessing wrong types should throw
    ASSERT_THROWS(str_val.asInteger());
    ASSERT_THROWS(int_val.asBoolean());
    ASSERT_THROWS(obj_val.asString());
    ASSERT_THROWS(int_val.at(0)); // Not an array
    ASSERT_THROWS(str_val.get("key")); // Not an object
    ASSERT_THROWS(obj_val.push(NodeValue(1))); // Not an array
    ASSERT_THROWS(int_val.set("key", NodeValue(1))); // Not an object
}

TEST(binary_data_edge_cases) {
    // Empty binary data
    BinaryData empty_binary;
    ASSERT_TRUE(empty_binary.isEmpty());
    ASSERT_EQ(size_t(0), empty_binary.getSize());
    
    // Large binary data
    std::vector<uint8_t> large_data(10000, 0xAB);
    BinaryData large_binary(std::move(large_data), "application/custom");
    ASSERT_EQ(size_t(10000), large_binary.getSize());
    ASSERT_EQ(std::string("application/custom"), large_binary.getMimeType());
    
    // MIME type modification
    BinaryData modifiable({0x01, 0x02}, "text/plain");
    modifiable.setMimeType("application/octet-stream");
    ASSERT_EQ(std::string("application/octet-stream"), modifiable.getMimeType());
}

TEST(complex_nested_structures) {
    // Create deeply nested structure
    NodeValue::Object deep_obj = {
        {"level1", NodeValue(NodeValue::Object{
            {"level2", NodeValue(NodeValue::Array{
                NodeValue(1),
                NodeValue(NodeValue::Object{
                    {"level3", NodeValue("deep value")}
                })
            })}
        })}
    };
    
    NodeValue val(deep_obj);
    
    // Navigate the structure
    NodeValue level2 = val.get("level1").get("level2");
    ASSERT_TRUE(level2.isArray());
    ASSERT_EQ(size_t(2), level2.size());
    
    NodeValue deep_value = level2.at(1).get("level3");
    ASSERT_EQ(std::string("deep value"), deep_value.asString());
}

// ============================================================================
// Main Test Runner
// ============================================================================

int main() {
    std::cout << "Running NodeValue and NodeItem Test Suite" << std::endl;
    std::cout << "===========================================" << std::endl;
    
    // Run all tests
    run_test_string_construction();
    run_test_integer_construction();
    run_test_float_construction();
    run_test_boolean_construction();
    run_test_null_construction();
    run_test_array_construction();
    run_test_object_construction();
    run_test_binary_data_construction();
    run_test_string_to_number_conversion();
    run_test_integer_to_float_conversion();
    run_test_boolean_to_string_conversion();
    run_test_array_manipulation();
    run_test_object_manipulation();
    run_test_nodeitem_basic();
    run_test_nodeitem_with_binary();
    run_test_nodeitem_with_metadata();
    run_test_nodeitem_with_error();
    run_test_execution_error();
    run_test_string_representation();
    run_test_equality_comparison();
    run_test_type_validation_errors();
    run_test_binary_data_edge_cases();
    run_test_complex_nested_structures();
    
    std::cout << "===========================================" << std::endl;
    std::cout << "Test Results: " << passed_tests << "/" << test_count << " tests passed" << std::endl;
    
    if (passed_tests == test_count) {
        std::cout << "✅ ALL TESTS PASSED!" << std::endl;
        std::cout << "NodeValue system successfully implemented with:" << std::endl;
        std::cout << "- All 8 data types (string, integer, float, boolean, object, array, binary, null)" << std::endl;
        std::cout << "- 20+ type conversion scenarios tested" << std::endl;
        std::cout << "- Comprehensive error handling" << std::endl;
        std::cout << "- Binary data support with MIME types" << std::endl;
        std::cout << "- Metadata and error tracking in NodeItem" << std::endl;
        std::cout << "- Deep nested structure support" << std::endl;
        std::cout << "- Type safety with proper validation" << std::endl;
        return 0;
    } else {
        std::cout << "❌ " << (test_count - passed_tests) << " tests failed" << std::endl;
        return 1;
    }
}