#include "../../../core/engine/logic/pure_logic_engine.hpp"
#include "../../../core/engine/logic/builtin_functions.hpp"
#include <iostream>
#include <cassert>
#include <yaml-cpp/yaml.h>

using namespace akao::logic;

/**
 * @brief Tests for Phase 1: Arithmetic Encoding Foundation
 * 
 * Tests the extended arithmetic capabilities needed for mathematical logic:
 * - Basic arithmetic operations (multiply, divide, modulo, power)
 * - Peano arithmetic primitives (successor, predecessor, is_zero)
 * - Number theory foundations
 */

void testBasicArithmeticOperations() {
    std::cout << "Testing basic arithmetic operations..." << std::endl;
    
    PureLogicEngine engine;
    builtin::registerAllBuiltinFunctions(engine);
    
    // Test multiplication
    {
        std::vector<Value> args = {Value(6), Value(7)};
        Value result = engine.executeFunction("math.multiply", args);
        assert(result.isInteger());
        assert(result.asInteger() == 42);
        std::cout << "  ✓ Multiplication: 6 * 7 = " << result.asInteger() << std::endl;
    }
    
    // Test division
    {
        std::vector<Value> args = {Value(42), Value(6)};
        Value result = engine.executeFunction("math.divide", args);
        assert(result.isInteger());
        assert(result.asInteger() == 7);
        std::cout << "  ✓ Division: 42 / 6 = " << result.asInteger() << std::endl;
    }
    
    // Test modulo
    {
        std::vector<Value> args = {Value(17), Value(5)};
        Value result = engine.executeFunction("math.modulo", args);
        assert(result.isInteger());
        assert(result.asInteger() == 2);
        std::cout << "  ✓ Modulo: 17 % 5 = " << result.asInteger() << std::endl;
    }
    
    // Test power
    {
        std::vector<Value> args = {Value(2), Value(8)};
        Value result = engine.executeFunction("math.power", args);
        assert(result.isInteger());
        assert(result.asInteger() == 256);
        std::cout << "  ✓ Power: 2^8 = " << result.asInteger() << std::endl;
    }
    
    std::cout << "✅ Basic arithmetic operations test passed" << std::endl;
}

void testPeanoArithmetic() {
    std::cout << "Testing Peano arithmetic primitives..." << std::endl;
    
    PureLogicEngine engine;
    builtin::registerAllBuiltinFunctions(engine);
    
    // Test successor function
    {
        std::vector<Value> args = {Value(5)};
        Value result = engine.executeFunction("peano.successor", args);
        assert(result.isInteger());
        assert(result.asInteger() == 6);
        std::cout << "  ✓ Successor: S(5) = " << result.asInteger() << std::endl;
    }
    
    // Test predecessor function
    {
        std::vector<Value> args = {Value(5)};
        Value result = engine.executeFunction("peano.predecessor", args);
        assert(result.isInteger());
        assert(result.asInteger() == 4);
        std::cout << "  ✓ Predecessor: P(5) = " << result.asInteger() << std::endl;
    }
    
    // Test predecessor of zero
    {
        std::vector<Value> args = {Value(0)};
        Value result = engine.executeFunction("peano.predecessor", args);
        assert(result.isInteger());
        assert(result.asInteger() == 0);
        std::cout << "  ✓ Predecessor: P(0) = " << result.asInteger() << std::endl;
    }
    
    // Test is_zero function
    {
        std::vector<Value> args = {Value(0)};
        Value result = engine.executeFunction("peano.is_zero", args);
        assert(result.isBoolean());
        assert(result.asBoolean() == true);
        std::cout << "  ✓ IsZero: isZero(0) = " << (result.asBoolean() ? "true" : "false") << std::endl;
    }
    
    {
        std::vector<Value> args = {Value(5)};
        Value result = engine.executeFunction("peano.is_zero", args);
        assert(result.isBoolean());
        assert(result.asBoolean() == false);
        std::cout << "  ✓ IsZero: isZero(5) = " << (result.asBoolean() ? "true" : "false") << std::endl;
    }
    
    std::cout << "✅ Peano arithmetic primitives test passed" << std::endl;
}

void testArithmeticInLogicExpressions() {
    std::cout << "Testing arithmetic in logic expressions..." << std::endl;
    
    PureLogicEngine engine;
    builtin::registerAllBuiltinFunctions(engine);
    Context ctx;
    
    // Test arithmetic in conditional logic
    {
        YAML::Node logic = YAML::Load(R"(
            if:
              operator: "greater_than"
              left:
                function: "math.multiply"
                arguments:
                  - literal: 6
                  - literal: 7
              right:
                literal: 40
            then:
              literal: "multiplication works"
            else:
              literal: "multiplication failed"
        )");
        
        Value result = engine.executeLogic(logic, ctx);
        assert(result.isString());
        assert(result.asString() == "multiplication works");
        std::cout << "  ✓ Arithmetic in conditionals: " << result.asString() << std::endl;
    }
    
    // Test Peano arithmetic in quantifiers
    {
        // Create domain collection first
        ctx.bindVariable("numbers", Value(std::vector<Value>{
            Value(0), Value(1), Value(2), Value(3)
        }));
        
        YAML::Node logic = YAML::Load(R"(
            exists:
              variable: "n"
              domain:
                var: "numbers"
              condition:
                operator: "equals"
                left:
                  function: "peano.successor"
                  arguments:
                    - var: "n"
                right:
                  literal: 3
        )");
        
        Value result = engine.executeLogic(logic, ctx);
        assert(result.isBoolean());
        assert(result.asBoolean() == true);
        std::cout << "  ✓ Peano arithmetic in quantifiers: exists n such that S(n) = 3" << std::endl;
    }
    
    std::cout << "✅ Arithmetic in logic expressions test passed" << std::endl;
}

void testNumberTheoryFoundations() {
    std::cout << "Testing number theory foundations..." << std::endl;
    
    PureLogicEngine engine;
    builtin::registerAllBuiltinFunctions(engine);
    Context ctx;
    
    // Test divisibility check using modulo
    {
        YAML::Node logic = YAML::Load(R"(
            function: "peano.is_zero"
            arguments:
              - function: "math.modulo"
                arguments:
                  - literal: 12
                  - literal: 3
        )");
        
        Value result = engine.executeLogic(logic, ctx);
        assert(result.isBoolean());
        assert(result.asBoolean() == true);
        std::cout << "  ✓ Divisibility: 12 is divisible by 3" << std::endl;
    }
    
    // Test prime-like properties (simple check for 2)
    {
        // Create domain collection first
        ctx.bindVariable("divisors", Value(std::vector<Value>{
            Value(1), Value(2)
        }));
        
        YAML::Node logic = YAML::Load(R"(
            forall:
              variable: "d"
              domain:
                var: "divisors"
              condition:
                operator: "or"
                left:
                  operator: "equals"
                  left:
                    var: "d"
                  right:
                    literal: 1
                right:
                  operator: "equals"
                  left:
                    var: "d"
                  right:
                    literal: 2
        )");
        
        Value result = engine.executeLogic(logic, ctx);
        assert(result.isBoolean());
        assert(result.asBoolean() == true);
        std::cout << "  ✓ Prime-like property check for 2" << std::endl;
    }
    
    std::cout << "✅ Number theory foundations test passed" << std::endl;
}

int main() {
    std::cout << "🧮 Testing Phase 1: Arithmetic Encoding Foundation" << std::endl;
    std::cout << "=================================================" << std::endl;
    
    try {
        testBasicArithmeticOperations();
        testPeanoArithmetic();
        testArithmeticInLogicExpressions();
        testNumberTheoryFoundations();
        
        std::cout << std::endl;
        std::cout << "🎉 ALL PHASE 1 TESTS PASSED!" << std::endl;
        std::cout << "✅ Extended arithmetic functions implemented" << std::endl;
        std::cout << "✅ Peano arithmetic primitives working" << std::endl;
        std::cout << "✅ Arithmetic integrated with logic expressions" << std::endl;
        std::cout << "✅ Number theory foundations ready" << std::endl;
        std::cout << std::endl;
        std::cout << "🚀 Phase 1 Complete - Ready for Phase 2: Meta-Logical Infrastructure" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ TEST FAILED: " << e.what() << std::endl;
        return 1;
    }
}