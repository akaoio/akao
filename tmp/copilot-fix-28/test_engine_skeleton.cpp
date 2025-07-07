#include "../core/engine/logic/pure_logic_engine.hpp"
#include <iostream>
#include <cassert>

using namespace akao::logic;

/**
 * @brief Basic tests for Pure Logic Engine skeleton
 * 
 * Tests Phase 1, Step 1.1 completion:
 * - Engine can be instantiated
 * - Basic YAML parsing works  
 * - Simple literal evaluation works
 * - Error handling works
 */
void test_engine_instantiation() {
    std::cout << "Testing engine instantiation..." << std::endl;
    
    PureLogicEngine engine;
    Context ctx;
    
    // Test literal evaluation
    YAML::Node literal_node;
    literal_node["literal"] = "true";
    
    Value result = engine.executeLogic(literal_node, ctx);
    assert(result.isBoolean());
    assert(result.asBoolean() == true);
    
    std::cout << "✅ Engine instantiation test passed" << std::endl;
}

void test_basic_operators() {
    std::cout << "Testing basic operators..." << std::endl;
    
    PureLogicEngine engine;
    
    // Test equals operator
    std::vector<Value> args = {Value(5), Value(5)};
    Value result = engine.executeOperator("equals", args);
    assert(result.isBoolean());
    assert(result.asBoolean() == true);
    
    // Test less_than operator  
    args = {Value(3), Value(5)};
    result = engine.executeOperator("less_than", args);
    assert(result.isBoolean());
    assert(result.asBoolean() == true);
    
    // Test and operator
    args = {Value(true), Value(false)};
    result = engine.executeOperator("and", args);
    assert(result.isBoolean());
    assert(result.asBoolean() == false);
    
    std::cout << "✅ Basic operators test passed" << std::endl;
}

void test_value_types() {
    std::cout << "Testing Value types..." << std::endl;
    
    // Test boolean value
    Value bool_val(true);
    assert(bool_val.isBoolean());
    assert(bool_val.asBoolean() == true);
    
    // Test integer value
    Value int_val(42);
    assert(int_val.isInteger());
    assert(int_val.asInteger() == 42);
    
    // Test string value
    Value str_val("hello");
    assert(str_val.isString());
    assert(str_val.asString() == "hello");
    
    // Test collection value
    std::vector<Value> collection = {Value(1), Value(2), Value(3)};
    Value coll_val(collection);
    assert(coll_val.isCollection());
    assert(coll_val.size() == 3);
    assert(coll_val.at(1).asInteger() == 2);
    
    std::cout << "✅ Value types test passed" << std::endl;
}

void test_context_scoping() {
    std::cout << "Testing Context scoping..." << std::endl;
    
    Context ctx;
    
    // Test variable binding and retrieval
    ctx.bindVariable("x", Value(10));
    assert(ctx.hasVariable("x"));
    Value val = ctx.getVariable("x");
    assert(val.isInteger());
    assert(val.asInteger() == 10);
    
    // Test scope pushing/popping
    ctx.pushScope();
    ctx.bindVariable("y", Value(20));
    assert(ctx.hasVariable("x")); // Should see parent scope
    assert(ctx.hasVariable("y")); // Should see current scope
    
    ctx.popScope();
    assert(ctx.hasVariable("x")); // Should still see original scope
    assert(!ctx.hasVariable("y")); // Should not see popped scope
    
    std::cout << "✅ Context scoping test passed" << std::endl;
}

void test_yaml_parsing() {
    std::cout << "Testing YAML logic parsing..." << std::endl;
    
    PureLogicEngine engine;
    Context ctx;
    
    // Test simple operator expression
    YAML::Node expr = YAML::Load(R"(
        operator: "equals"
        left:
          literal: 5
        right:
          literal: 5
    )");
    
    Value result = engine.executeLogic(expr, ctx);
    assert(result.isBoolean());
    assert(result.asBoolean() == true);
    
    std::cout << "✅ YAML parsing test passed" << std::endl;
}

int main() {
    std::cout << "🧪 Running Pure Logic Engine Skeleton Tests (Phase 1, Step 1.1)" << std::endl;
    std::cout << "=================================================================" << std::endl;
    
    try {
        test_engine_instantiation();
        test_basic_operators();
        test_value_types();
        test_context_scoping();
        test_yaml_parsing();
        
        std::cout << std::endl;
        std::cout << "🎉 ALL TESTS PASSED - Phase 1, Step 1.1 Complete!" << std::endl;
        std::cout << "✅ Pure Logic Engine skeleton is working" << std::endl;
        std::cout << "✅ Basic YAML parsing is functional" << std::endl;
        std::cout << "✅ Value types and operators work correctly" << std::endl;
        std::cout << "✅ Context scoping is implemented" << std::endl;
        std::cout << std::endl;
        std::cout << "Ready to proceed to Phase 1, Step 1.2: Implement Core Logic Constructs" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ TEST FAILED: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
