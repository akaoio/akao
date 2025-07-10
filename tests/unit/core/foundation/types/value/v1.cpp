/**
 * @id: akao:file:tests:unit:core:foundation:types:value:v1
 * @doc: Comprehensive unit tests for NodeValue universal data container. Tests all data types, conversions, metadata operations, and edge cases to ensure robust data exchange in workflow systems.
 * @specification: Unit tests for NodeValue class functionality
 * @scope: Foundation data types testing
 * @timeline: 2025-07-10
 * @rationale: Ensure reliability of core data exchange mechanism
 * @methodology: Comprehensive test coverage with edge cases and error conditions
 * @references: ["akao:class:core:foundation:types:nodevalue:v1"]
 */

#include "core/foundation/types/value/v1.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>

using namespace akao::foundation::types;

void testBasicTypes() {
    std::cout << "Testing basic types..." << std::endl;
    
    // String
    NodeValue str("test");
    assert(str.isString());
    assert(str.asString() == "test");
    assert(str.toString() == "test");
    
    // Integer
    NodeValue integer(static_cast<int64_t>(42));
    assert(integer.isInteger());
    assert(integer.asInteger() == 42);
    assert(integer.toString() == "42");
    
    // Double
    NodeValue dbl(3.14);
    assert(dbl.isDouble());
    assert(dbl.asDouble() == 3.14);
    
    // Boolean
    NodeValue boolean(true);
    assert(boolean.isBoolean());
    assert(boolean.asBoolean() == true);
    assert(boolean.toString() == "true");
    
    // Null
    NodeValue null;
    assert(null.isNull());
    assert(null.toString() == "null");
    
    std::cout << "Basic types test passed!" << std::endl;
}

void testTypeConversions() {
    std::cout << "Testing type conversions..." << std::endl;
    
    // String to integer
    NodeValue str("123");
    assert(str.toInteger() == 123);
    
    // Integer to double
    NodeValue integer(static_cast<int64_t>(42));
    assert(integer.toDouble() == 42.0);
    
    // Boolean conversions
    NodeValue trueBool(true);
    assert(trueBool.toInteger() == 1);
    assert(trueBool.toDouble() == 1.0);
    
    NodeValue falseBool(false);
    assert(falseBool.toInteger() == 0);
    assert(falseBool.toDouble() == 0.0);
    
    // Boolean evaluation
    NodeValue emptyStr("");
    assert(emptyStr.toBoolean() == false);
    
    NodeValue nonEmptyStr("hello");
    assert(nonEmptyStr.toBoolean() == true);
    
    std::cout << "Type conversions test passed!" << std::endl;
}

void testArrayOperations() {
    std::cout << "Testing array operations..." << std::endl;
    
    NodeValue::Array arr;
    arr.push_back(NodeValue("first"));
    arr.push_back(NodeValue(static_cast<int64_t>(42)));
    arr.push_back(NodeValue(true));
    
    NodeValue arrayValue(arr);
    assert(arrayValue.isArray());
    assert(arrayValue.size() == 3);
    
    assert(arrayValue[0].asString() == "first");
    assert(arrayValue[1].asInteger() == 42);
    assert(arrayValue[2].asBoolean() == true);
    
    // Test push operation
    arrayValue.push(NodeValue("fourth"));
    assert(arrayValue.size() == 4);
    assert(arrayValue[3].asString() == "fourth");
    
    std::cout << "Array operations test passed!" << std::endl;
}

void testObjectOperations() {
    std::cout << "Testing object operations..." << std::endl;
    
    NodeValue::Object obj;
    obj["name"] = NodeValue("test");
    obj["count"] = NodeValue(static_cast<int64_t>(5));
    obj["enabled"] = NodeValue(true);
    
    NodeValue objectValue(obj);
    assert(objectValue.isObject());
    assert(objectValue.size() == 3);
    
    assert(objectValue["name"].asString() == "test");
    assert(objectValue["count"].asInteger() == 5);
    assert(objectValue["enabled"].asBoolean() == true);
    
    assert(objectValue.hasKey("name"));
    assert(!objectValue.hasKey("missing"));
    
    std::cout << "Object operations test passed!" << std::endl;
}

void testMetadataOperations() {
    std::cout << "Testing metadata operations..." << std::endl;
    
    NodeValue value("test");
    
    // Set metadata
    value.setMetadata("source", NodeValue("file.txt"));
    value.setMetadata("line", NodeValue(static_cast<int64_t>(42)));
    
    // Get metadata
    assert(value.hasMetadata("source"));
    assert(value.getMetadata("source").asString() == "file.txt");
    assert(value.getMetadata("line").asInteger() == 42);
    
    // Missing metadata
    assert(!value.hasMetadata("missing"));
    NodeValue missing = value.getMetadata("missing");
    assert(missing.isNull());
    
    // Get all metadata
    auto allMetadata = value.getAllMetadata();
    assert(allMetadata.size() == 2);
    
    // Clear metadata
    value.clearMetadata();
    assert(value.getAllMetadata().empty());
    
    std::cout << "Metadata operations test passed!" << std::endl;
}

void testBinaryData() {
    std::cout << "Testing binary data..." << std::endl;
    
    NodeValue::BinaryData data = {0x48, 0x65, 0x6c, 0x6c, 0x6f}; // "Hello"
    NodeValue binaryValue(data);
    
    assert(binaryValue.isBinary());
    assert(binaryValue.asBinary().size() == 5);
    assert(binaryValue.asBinary()[0] == 0x48);
    
    std::cout << "Binary data test passed!" << std::endl;
}

void testErrorHandling() {
    std::cout << "Testing error handling..." << std::endl;
    
    NodeValue str("test");
    
    // Type mismatch errors
    bool caught = false;
    try {
        str.asInteger();
    } catch (const std::runtime_error&) {
        caught = true;
    }
    assert(caught);
    
    // Invalid conversion
    caught = false;
    try {
        NodeValue invalid("not-a-number");
        invalid.toInteger();
    } catch (const std::runtime_error&) {
        caught = true;
    }
    assert(caught);
    
    std::cout << "Error handling test passed!" << std::endl;
}

void testComparisons() {
    std::cout << "Testing comparisons..." << std::endl;
    
    NodeValue str1("test");
    NodeValue str2("test");
    NodeValue str3("different");
    
    assert(str1 == str2);
    assert(str1 != str3);
    
    NodeValue int1(static_cast<int64_t>(42));
    NodeValue int2(static_cast<int64_t>(42));
    NodeValue int3(static_cast<int64_t>(43));
    
    assert(int1 == int2);
    assert(int1 != int3);
    assert(str1 != int1); // Different types
    
    std::cout << "Comparisons test passed!" << std::endl;
}

int main() {
    std::cout << "Running NodeValue unit tests..." << std::endl;
    
    try {
        testBasicTypes();
        testTypeConversions();
        testArrayOperations();
        testObjectOperations();
        testMetadataOperations();
        testBinaryData();
        testErrorHandling();
        testComparisons();
        
        std::cout << "All NodeValue tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
}