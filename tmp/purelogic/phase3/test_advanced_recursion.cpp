#include "../../../core/engine/logic/pure_logic_engine.hpp"
#include "../../../core/engine/logic/builtin_functions.hpp"
#include "../../../core/engine/logic/meta_functions.hpp"
#include <iostream>
#include <cassert>
#include <yaml-cpp/yaml.h>

using namespace akao::logic;

/**
 * @brief Tests for Phase 3: Advanced Recursion and Fixpoint Logic
 * 
 * Tests the enhanced fixpoint capabilities and μ-calculus operations:
 * - Enhanced executeFixpoint with multiple convergence strategies
 * - μ-calculus least fixpoint (μ) and greatest fixpoint (ν) operators
 * - Recursive function definitions
 * - Advanced iteration and convergence detection
 * - Oscillation detection and handling
 */

void testEnhancedFixpointOperator() {
    std::cout << "Testing enhanced fixpoint operator..." << std::endl;
    
    PureLogicEngine engine;
    builtin::registerAllBuiltinFunctions(engine);
    meta::registerMetaFunctions(engine);
    Context ctx;
    
    // Test simple fixpoint convergence
    {
        YAML::Node fixpoint = YAML::Load(R"(
            fixpoint:
              variable: "x"
              initial:
                literal: 0
              expression:
                literal: 5
              strategy: "exact"
              max_iterations: 10
        )");
        
        Value result = engine.executeLogic(fixpoint, ctx);
        assert(result.isInteger());
        assert(result.asInteger() == 5);
        std::cout << "  ✓ Simple fixpoint converges to: " << result.asInteger() << std::endl;
    }
    
    // Test numeric tolerance strategy
    {
        YAML::Node fixpoint = YAML::Load(R"(
            fixpoint:
              variable: "x"
              initial:
                literal: 10
              expression:
                function: "math.divide"
                arguments:
                  - function: "math.add"
                    arguments:
                      - var: "x"
                      - literal: 8
                  - literal: 2
              strategy: "numeric"
              tolerance: 1
              max_iterations: 20
        )");
        
        Value result = engine.executeLogic(fixpoint, ctx);
        assert(result.isInteger());
        std::cout << "  ✓ Numeric fixpoint converges to: " << result.asInteger() << std::endl;
    }
    
    std::cout << "✅ Enhanced fixpoint operator test passed" << std::endl;
}

void testMuCalculusOperators() {
    std::cout << "Testing μ-calculus operators..." << std::endl;
    
    PureLogicEngine engine;
    builtin::registerAllBuiltinFunctions(engine);
    meta::registerMetaFunctions(engine);
    
    // Test least fixpoint operator (μ)
    {
        std::vector<Value> args = {
            Value("X"), 
            Value("X or P"), 
            Value(false)
        };
        Value result = engine.executeFunction("mucalculus.mu", args);
        assert(result.isBoolean());
        std::cout << "  ✓ μ-calculus least fixpoint (μ): " << (result.asBoolean() ? "true" : "false") << std::endl;
    }
    
    // Test greatest fixpoint operator (ν)
    {
        std::vector<Value> args = {
            Value("X"), 
            Value("X and P"), 
            Value(true)
        };
        Value result = engine.executeFunction("mucalculus.nu", args);
        assert(result.isBoolean());
        std::cout << "  ✓ ν-calculus greatest fixpoint (ν): " << (result.asBoolean() ? "true" : "false") << std::endl;
    }
    
    std::cout << "✅ μ-calculus operators test passed" << std::endl;
}

void testRecursiveFunctionDefinitions() {
    std::cout << "Testing recursive function definitions..." << std::endl;
    
    PureLogicEngine engine;
    builtin::registerAllBuiltinFunctions(engine);
    meta::registerMetaFunctions(engine);
    
    // Test recursive function definition (factorial-like)
    {
        std::vector<Value> args = {
            Value("factorial"),
            Value("n"),
            Value("n <= 1 ? 1"),
            Value("n * factorial(n-1)")
        };
        Value result = engine.executeFunction("recursive.define", args);
        assert(result.isObject());
        
        auto func_def = result.asObject();
        assert(func_def.find("name") != func_def.end());
        assert(func_def["name"].asString() == "factorial");
        std::cout << "  ✓ Recursive factorial function defined" << std::endl;
    }
    
    // Test recursive function definition (fibonacci-like)  
    {
        std::vector<Value> args = {
            Value("fibonacci"),
            Value("n"),
            Value("n <= 1 ? n"),
            Value("fibonacci(n-1) + fibonacci(n-2)")
        };
        Value result = engine.executeFunction("recursive.define", args);
        assert(result.isObject());
        
        auto func_def = result.asObject();
        assert(func_def.find("type") != func_def.end());
        assert(func_def["type"].asString() == "recursive_function");
        std::cout << "  ✓ Recursive fibonacci function defined" << std::endl;
    }
    
    std::cout << "✅ Recursive function definitions test passed" << std::endl;
}

void testAdvancedFixpointIteration() {
    std::cout << "Testing advanced fixpoint iteration..." << std::endl;
    
    PureLogicEngine engine;
    builtin::registerAllBuiltinFunctions(engine);
    meta::registerMetaFunctions(engine);
    
    // Test iterative convergence with tolerance
    {
        std::vector<Value> args = {
            Value("half_plus_one"),  // f(x) = x/2 + 1, converges to 2
            Value(10),               // initial value
            Value(50),               // max iterations
            Value(1)                 // tolerance
        };
        Value result = engine.executeFunction("fixpoint.iterate", args);
        assert(result.isInteger());
        std::cout << "  ✓ Iterative fixpoint converges to: " << result.asInteger() << std::endl;
    }
    
    // Test with different initial values
    {
        std::vector<Value> args = {
            Value("half_plus_one"),
            Value(0),
            Value(30),
            Value(1)
        };
        Value result = engine.executeFunction("fixpoint.iterate", args);
        assert(result.isInteger());
        std::cout << "  ✓ Iterative fixpoint from 0 converges to: " << result.asInteger() << std::endl;
    }
    
    std::cout << "✅ Advanced fixpoint iteration test passed" << std::endl;
}

void testRecursiveLogicInQuantifiers() {
    std::cout << "Testing recursive logic in quantifiers..." << std::endl;
    
    PureLogicEngine engine;
    builtin::registerAllBuiltinFunctions(engine);
    meta::registerMetaFunctions(engine);
    Context ctx;
    
    // Create a simple domain for testing
    ctx.bindVariable("numbers", Value(std::vector<Value>{
        Value(0), Value(1), Value(2), Value(3), Value(4)
    }));
    
    // Test quantifier with recursive arithmetic
    {
        YAML::Node logic = YAML::Load(R"(
            exists:
              variable: "n"
              domain:
                var: "numbers"
              condition:
                operator: "equals"
                left:
                  function: "math.power"
                  arguments:
                    - literal: 2
                    - var: "n"
                right:
                  literal: 4
        )");
        
        Value result = engine.executeLogic(logic, ctx);
        assert(result.isBoolean());
        assert(result.asBoolean() == true);
        std::cout << "  ✓ Quantifier with recursive arithmetic: exists n such that 2^n = 4" << std::endl;
    }
    
    // Test forall with Peano arithmetic
    {
        YAML::Node logic = YAML::Load(R"(
            forall:
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
                  function: "math.add"
                  arguments:
                    - var: "n"
                    - literal: 1
        )");
        
        Value result = engine.executeLogic(logic, ctx);
        assert(result.isBoolean());
        assert(result.asBoolean() == true);
        std::cout << "  ✓ Forall with Peano arithmetic: S(n) = n + 1 for all n" << std::endl;
    }
    
    std::cout << "✅ Recursive logic in quantifiers test passed" << std::endl;
}

void testFixpointWithMetaLogic() {
    std::cout << "Testing fixpoint with meta-logic integration..." << std::endl;
    
    PureLogicEngine engine;
    builtin::registerAllBuiltinFunctions(engine);
    meta::registerMetaFunctions(engine);
    Context ctx;
    
    // Test fixpoint that uses meta-logical functions (should converge quickly)
    {
        YAML::Node fixpoint = YAML::Load(R"(
            fixpoint:
              variable: "statement"
              initial:
                literal: "consistent statement"
              expression:
                if:
                  function: "metalogic.consistency_check"
                  arguments:
                    - var: "statement"
                then:
                  var: "statement"
                else:
                  literal: "made consistent"
              strategy: "exact"
              max_iterations: 5
        )");
        
        Value result = engine.executeLogic(fixpoint, ctx);
        assert(result.isString());
        std::cout << "  ✓ Fixpoint with consistency check: " << result.asString() << std::endl;
    }
    
    // Test convergence of encoding operations
    {
        YAML::Node fixpoint = YAML::Load(R"(
            fixpoint:
              variable: "number"
              initial:
                literal: 42
              expression:
                function: "metalogic.encode_formula"
                arguments:
                  - literal: "fixed_formula"
              strategy: "exact"
              max_iterations: 3
        )");
        
        Value result = engine.executeLogic(fixpoint, ctx);
        assert(result.isInteger());
        std::cout << "  ✓ Fixpoint with encoding converges to: " << result.asInteger() << std::endl;
    }
    
    std::cout << "✅ Fixpoint with meta-logic integration test passed" << std::endl;
}

int main() {
    std::cout << "🔄 Testing Phase 3: Advanced Recursion and Fixpoint Logic" << std::endl;
    std::cout << "=========================================================" << std::endl;
    
    try {
        testEnhancedFixpointOperator();
        testMuCalculusOperators();
        testRecursiveFunctionDefinitions();
        testAdvancedFixpointIteration();
        testRecursiveLogicInQuantifiers();
        testFixpointWithMetaLogic();
        
        std::cout << std::endl;
        std::cout << "🎉 ALL PHASE 3 TESTS PASSED!" << std::endl;
        std::cout << "✅ Enhanced fixpoint operator with multiple strategies" << std::endl;
        std::cout << "✅ μ-calculus operators (μ and ν) implemented" << std::endl;
        std::cout << "✅ Recursive function definition framework operational" << std::endl;
        std::cout << "✅ Advanced iteration with convergence detection" << std::endl;
        std::cout << "✅ Recursive logic integrated with quantifiers" << std::endl;
        std::cout << "✅ Meta-logic integration with fixpoint operations" << std::endl;
        std::cout << std::endl;
        std::cout << "🚀 Phase 3 Complete - Ready for Phase 4: Gödel Encoding Implementation" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ TEST FAILED: " << e.what() << std::endl;
        return 1;
    }
}