#include "../core/engine/logic/pure_logic_engine.hpp"
#include <iostream>
#include <cassert>
#include <yaml-cpp/yaml.h>

using namespace akao::logic;

/**
 * @brief Tests for Phase 1, Step 1.2: Core Logic Constructs
 * 
 * Tests:
 * - Quantifiers (forall/exists)
 * - Conditionals (if/then/else)
 * - Complex logic expressions
 */

void test_forall_quantifier() {
    std::cout << "Testing forall quantifier..." << std::endl;
    
    PureLogicEngine engine;
    Context ctx;
    
    // Create test data: collection [1, 2, 3]
    std::vector<Value> numbers = {Value(1), Value(2), Value(3)};
    ctx.bindVariable("numbers", Value(numbers));
    
    // Test: forall x in numbers: x > 0 (should be true)
    YAML::Node forall_positive = YAML::Load(R"(
        forall:
          variable: "x"
          domain:
            var: "numbers"
          condition:
            operator: "greater_than"
            left:
              var: "x"
            right:
              literal: 0
    )");
    
    Value result = engine.executeLogic(forall_positive, ctx);
    assert(result.isBoolean());
    assert(result.asBoolean() == true);
    
    // Test: forall x in numbers: x > 2 (should be false)
    YAML::Node forall_gt_two = YAML::Load(R"(
        forall:
          variable: "x"
          domain:
            var: "numbers"
          condition:
            operator: "greater_than"
            left:
              var: "x"
            right:
              literal: 2
    )");
    
    result = engine.executeLogic(forall_gt_two, ctx);
    assert(result.isBoolean());
    assert(result.asBoolean() == false);
    
    std::cout << "✅ Forall quantifier test passed" << std::endl;
}

void test_exists_quantifier() {
    std::cout << "Testing exists quantifier..." << std::endl;
    
    PureLogicEngine engine;
    Context ctx;
    
    // Create test data: collection [1, 2, 3]
    std::vector<Value> numbers = {Value(1), Value(2), Value(3)};
    ctx.bindVariable("numbers", Value(numbers));
    
    // Test: exists x in numbers: x == 2 (should be true)
    YAML::Node exists_two = YAML::Load(R"(
        exists:
          variable: "x"
          domain:
            var: "numbers"
          condition:
            operator: "equals"
            left:
              var: "x"
            right:
              literal: 2
    )");
    
    Value result = engine.executeLogic(exists_two, ctx);
    assert(result.isBoolean());
    assert(result.asBoolean() == true);
    
    // Test: exists x in numbers: x > 5 (should be false)
    YAML::Node exists_gt_five = YAML::Load(R"(
        exists:
          variable: "x"
          domain:
            var: "numbers"
          condition:
            operator: "greater_than"
            left:
              var: "x"
            right:
              literal: 5
    )");
    
    result = engine.executeLogic(exists_gt_five, ctx);
    assert(result.isBoolean());
    assert(result.asBoolean() == false);
    
    std::cout << "✅ Exists quantifier test passed" << std::endl;
}

void test_conditional_logic() {
    std::cout << "Testing conditional logic..." << std::endl;
    
    PureLogicEngine engine;
    Context ctx;
    
    ctx.bindVariable("x", Value(10));
    ctx.bindVariable("y", Value(5));
    
    // Test: if x > y then "greater" else "not_greater"
    YAML::Node conditional = YAML::Load(R"(
        if:
          operator: "greater_than"
          left:
            var: "x"
          right:
            var: "y"
        then:
          literal: "greater"
        else:
          literal: "not_greater"
    )");
    
    Value result = engine.executeLogic(conditional, ctx);
    assert(result.isString());
    assert(result.asString() == "greater");
    
    // Test the else branch by swapping values
    ctx.bindVariable("x", Value(3));
    ctx.bindVariable("y", Value(8));
    
    result = engine.executeLogic(conditional, ctx);
    assert(result.isString());
    assert(result.asString() == "not_greater");
    
    // Test conditional without else clause
    YAML::Node conditional_no_else = YAML::Load(R"(
        if:
          operator: "greater_than"
          left:
            var: "x"
          right:
            var: "y"
        then:
          literal: "yes"
    )");
    
    result = engine.executeLogic(conditional_no_else, ctx);
    assert(result.isNull()); // Should return null when condition is false and no else
    
    std::cout << "✅ Conditional logic test passed" << std::endl;
}

void test_nested_logic() {
    std::cout << "Testing nested logic expressions..." << std::endl;
    
    PureLogicEngine engine;
    Context ctx;
    
    // Create test data: collection of numbers
    std::vector<Value> numbers = {Value(1), Value(4), Value(7), Value(2)};
    ctx.bindVariable("numbers", Value(numbers));
    
    // Test: exists x in numbers: if x > 5 then true else false
    YAML::Node nested_logic = YAML::Load(R"(
        exists:
          variable: "x"
          domain:
            var: "numbers"
          condition:
            if:
              operator: "greater_than"
              left:
                var: "x"
              right:
                literal: 5
            then:
              literal: true
            else:
              literal: false
    )");
    
    Value result = engine.executeLogic(nested_logic, ctx);
    assert(result.isBoolean());
    assert(result.asBoolean() == true); // 7 > 5
    
    std::cout << "✅ Nested logic test passed" << std::endl;
}

void test_variable_scoping_in_quantifiers() {
    std::cout << "Testing variable scoping in quantifiers..." << std::endl;
    
    PureLogicEngine engine;
    Context ctx;
    
    // Set up outer scope variable
    ctx.bindVariable("x", Value(100));
    
    // Create collection
    std::vector<Value> numbers = {Value(1), Value(2), Value(3)};
    ctx.bindVariable("numbers", Value(numbers));
    
    // Test that quantifier variable doesn't affect outer scope
    YAML::Node forall_scoping = YAML::Load(R"(
        forall:
          variable: "x"
          domain:
            var: "numbers"
          condition:
            operator: "less_than"
            left:
              var: "x"
            right:
              literal: 10
    )");
    
    Value result = engine.executeLogic(forall_scoping, ctx);
    assert(result.isBoolean());
    assert(result.asBoolean() == true);
    
    // Verify outer scope variable is unchanged
    assert(ctx.getVariable("x").asInteger() == 100);
    
    std::cout << "✅ Variable scoping test passed" << std::endl;
}

int main() {
    std::cout << "🧪 Running Core Logic Constructs Tests (Phase 1, Step 1.2)" << std::endl;
    std::cout << "============================================================" << std::endl;
    
    try {
        test_forall_quantifier();
        test_exists_quantifier();
        test_conditional_logic();
        test_nested_logic();
        test_variable_scoping_in_quantifiers();
        
        std::cout << std::endl;
        std::cout << "🎉 ALL TESTS PASSED - Phase 1, Step 1.2 Complete!" << std::endl;
        std::cout << "✅ Forall quantifier working correctly" << std::endl;
        std::cout << "✅ Exists quantifier working correctly" << std::endl;
        std::cout << "✅ Conditional logic (if/then/else) implemented" << std::endl;
        std::cout << "✅ Nested logic expressions supported" << std::endl;
        std::cout << "✅ Variable scoping in quantifiers working" << std::endl;
        std::cout << std::endl;
        std::cout << "📋 Phase 1, Step 1.2 Requirements Met:" << std::endl;
        std::cout << "   ✓ Implement executeOperator() for basic operators" << std::endl;
        std::cout << "   ✓ Implement executeQuantifier() for forall/exists" << std::endl;
        std::cout << "   ✓ Implement executeConditional() for if/then/else" << std::endl;
        std::cout << "   ✓ Test: Engine can execute simple logical expressions" << std::endl;
        std::cout << std::endl;
        std::cout << "🚀 Ready to proceed to Phase 1, Step 1.3: Built-in Function Registry" << std::endl;
        std::cout << "   Next: Create core/engine/builtin_functions.yaml" << std::endl;
        std::cout << "   Next: Implement function registry loading" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ TEST FAILED: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
