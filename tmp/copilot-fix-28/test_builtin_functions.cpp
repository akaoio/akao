#include "../core/engine/logic/pure_logic_engine.hpp"
#include "../core/engine/logic/builtin_functions.hpp"
#include <iostream>
#include <cassert>
#include <yaml-cpp/yaml.h>

using namespace akao::logic;

/**
 * @brief Tests for Phase 1, Step 1.3: Built-in Function Registry
 * 
 * Tests:
 * - Function registry loading
 * - Built-in function execution
 * - Function calls in logic expressions
 */

void test_function_registry_loading() {
    std::cout << "Testing function registry loading..." << std::endl;
    
    PureLogicEngine engine;
    
    // Load builtin functions
    engine.loadBuiltinFunctions("/workspaces/akao/core/engine/builtin_functions.yaml");
    
    std::cout << "✅ Function registry loading test passed" << std::endl;
}

void test_math_functions() {
    std::cout << "Testing math functions..." << std::endl;
    
    PureLogicEngine engine;
    builtin::registerAllBuiltinFunctions(engine);
    
    // Test addition
    std::vector<Value> args = {Value(5), Value(3)};
    Value result = engine.executeFunction("math.add", args);
    assert(result.isInteger());
    assert(result.asInteger() == 8);
    
    // Test subtraction
    args = {Value(10), Value(4)};
    result = engine.executeFunction("math.subtract", args);
    assert(result.isInteger());
    assert(result.asInteger() == 6);
    
    std::cout << "✅ Math functions test passed" << std::endl;
}

void test_collection_functions() {
    std::cout << "Testing collection functions..." << std::endl;
    
    PureLogicEngine engine;
    builtin::registerAllBuiltinFunctions(engine);
    
    // Create test collection
    std::vector<Value> collection = {Value(1), Value(2), Value(3)};
    
    // Test count function
    std::vector<Value> args = {Value(collection)};
    Value result = engine.executeFunction("collection.count", args);
    assert(result.isInteger());
    assert(result.asInteger() == 3);
    
    // Test contains function  
    args = {Value(collection), Value(2)};
    result = engine.executeFunction("collection.contains", args);
    assert(result.isBoolean());
    assert(result.asBoolean() == true);
    
    args = {Value(collection), Value(5)};
    result = engine.executeFunction("collection.contains", args);
    assert(result.isBoolean());
    assert(result.asBoolean() == false);
    
    std::cout << "✅ Collection functions test passed" << std::endl;
}

void test_string_functions() {
    std::cout << "Testing string functions..." << std::endl;
    
    PureLogicEngine engine;
    builtin::registerAllBuiltinFunctions(engine);
    
    // Test string length
    std::vector<Value> args = {Value("hello")};
    Value result = engine.executeFunction("string.length", args);
    assert(result.isInteger());
    assert(result.asInteger() == 5);
    
    // Test string concatenation
    args = {Value("hello"), Value(" world")};
    result = engine.executeFunction("string.concat", args);
    assert(result.isString());
    assert(result.asString() == "hello world");
    
    std::cout << "✅ String functions test passed" << std::endl;
}

void test_functions_in_logic_expressions() {
    std::cout << "Testing functions in logic expressions..." << std::endl;
    
    PureLogicEngine engine;
    builtin::registerAllBuiltinFunctions(engine);
    Context ctx;
    
    // Create test collection
    std::vector<Value> numbers = {Value(1), Value(2), Value(3), Value(4), Value(5)};
    ctx.bindVariable("numbers", Value(numbers));
    
    // Test: collection.count(numbers) > 3
    YAML::Node count_expr = YAML::Load(R"(
        operator: "greater_than"
        left:
          function: "collection.count"
          argument:
            var: "numbers"
        right:
          literal: 3
    )");
    
    Value result = engine.executeLogic(count_expr, ctx);
    assert(result.isBoolean());
    assert(result.asBoolean() == true);
    
    // Test: exists x in numbers: math.add(x, 1) == 3
    YAML::Node exists_add = YAML::Load(R"(
        exists:
          variable: "x"
          domain:
            var: "numbers"
          condition:
            operator: "equals"
            left:
              function: "math.add"
              arguments:
                - var: "x"
                - literal: 1
            right:
              literal: 3
    )");
    
    result = engine.executeLogic(exists_add, ctx);
    assert(result.isBoolean());
    assert(result.asBoolean() == true); // x=2, 2+1=3
    
    std::cout << "✅ Functions in logic expressions test passed" << std::endl;
}

void test_mock_collection_function() {
    std::cout << "Testing mock collection function..." << std::endl;
    
    PureLogicEngine engine;
    builtin::registerAllBuiltinFunctions(engine);
    Context ctx;
    
    // Test creating mock number collection
    YAML::Node mock_expr = YAML::Load(R"(
        function: "test.mock_collection"
        arguments:
          - literal: "numbers"
          - literal: 5
    )");
    
    Value result = engine.executeLogic(mock_expr, ctx);
    assert(result.isCollection());
    assert(result.size() == 5);
    assert(result.at(0).asInteger() == 1);
    assert(result.at(4).asInteger() == 5);
    
    // Use mock collection in quantifier
    ctx.bindVariable("mock_numbers", result);
    
    YAML::Node forall_mock = YAML::Load(R"(
        forall:
          variable: "x"
          domain:
            var: "mock_numbers"
          condition:
            operator: "greater_than"
            left:
              var: "x"
            right:
              literal: 0
    )");
    
    result = engine.executeLogic(forall_mock, ctx);
    assert(result.isBoolean());
    assert(result.asBoolean() == true);
    
    std::cout << "✅ Mock collection function test passed" << std::endl;
}

void test_complex_function_expressions() {
    std::cout << "Testing complex function expressions..." << std::endl;
    
    PureLogicEngine engine;
    builtin::registerAllBuiltinFunctions(engine);
    Context ctx;
    
    // Test nested function calls: string.length(string.concat("hello", " world")) == 11
    YAML::Node nested_functions = YAML::Load(R"(
        operator: "equals"
        left:
          function: "string.length"
          argument:
            function: "string.concat"
            arguments:
              - literal: "hello"
              - literal: " world"
        right:
          literal: 11
    )");
    
    Value result = engine.executeLogic(nested_functions, ctx);
    assert(result.isBoolean());
    assert(result.asBoolean() == true);
    
    std::cout << "✅ Complex function expressions test passed" << std::endl;
}

int main() {
    std::cout << "🧪 Running Built-in Function Registry Tests (Phase 1, Step 1.3)" << std::endl;
    std::cout << "================================================================" << std::endl;
    
    try {
        test_function_registry_loading();
        test_math_functions();
        test_collection_functions();
        test_string_functions();
        test_functions_in_logic_expressions();
        test_mock_collection_function();
        test_complex_function_expressions();
        
        std::cout << std::endl;
        std::cout << "🎉 ALL TESTS PASSED - Phase 1, Step 1.3 Complete!" << std::endl;
        std::cout << "✅ Function registry loading working" << std::endl;
        std::cout << "✅ Math functions implemented and tested" << std::endl;
        std::cout << "✅ Collection functions implemented and tested" << std::endl;
        std::cout << "✅ String functions implemented and tested" << std::endl;
        std::cout << "✅ Functions work correctly in logic expressions" << std::endl;
        std::cout << "✅ Mock functions support testing" << std::endl;
        std::cout << "✅ Complex nested function calls work" << std::endl;
        std::cout << std::endl;
        std::cout << "📋 Phase 1, Step 1.3 Requirements Met:" << std::endl;
        std::cout << "   ✓ Create core/engine/builtin_functions.yaml with function definitions" << std::endl;
        std::cout << "   ✓ Implement function registry loading in engine" << std::endl;
        std::cout << "   ✓ Implement executeFunction() for calling built-in functions" << std::endl;
        std::cout << "   ✓ Test: Engine can call registered functions" << std::endl;
        std::cout << std::endl;
        std::cout << "🎉 PHASE 1 COMPLETE: Engine Foundation is Ready!" << std::endl;
        std::cout << "🚀 Ready to proceed to Phase 2: Domain Functions" << std::endl;
        std::cout << "   Next: Implement filesystem functions" << std::endl;
        std::cout << "   Next: Implement code analysis functions" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ TEST FAILED: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
