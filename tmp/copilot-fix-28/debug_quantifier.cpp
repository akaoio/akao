#include "../core/engine/logic/pure_logic_engine.hpp"
#include <iostream>
#include <cassert>

void debugQuantifierTest() {
    std::cout << "Debugging quantifier test..." << std::endl;
    
    akao::logic::PureLogicEngine engine;
    akao::logic::Context ctx;
    
    // Create a collection where not all elements satisfy condition
    std::vector<akao::logic::Value> collection = {
        akao::logic::Value(1),
        akao::logic::Value(2),
        akao::logic::Value(15), // This should cause early termination
        akao::logic::Value(3),
        akao::logic::Value(4)
    };
    ctx.bindVariable("numbers", akao::logic::Value(collection));
    
    // First, let's test the condition directly for each number
    std::cout << "Testing condition for each number:" << std::endl;
    for (int i = 0; i < 5; ++i) {
        akao::logic::Context test_ctx(ctx);
        test_ctx.bindVariable("num", collection[i]);
        
        YAML::Node condition = YAML::Load(R"(
            operator: "less_than"
            left:
              var: "num"
            right:
              literal: 10
        )");
        
        auto result = engine.executeLogic(condition, test_ctx);
        std::cout << "  " << collection[i].asInteger() << " < 10 = " << 
                     (result.asBoolean() ? "true" : "false") << std::endl;
    }
    
    // Now test the full forall
    engine.enableTracing(true);
    
    YAML::Node forall_expr = YAML::Load(R"(
        forall:
          variable: "num"
          domain:
            var: "numbers"
          condition:
            operator: "less_than"
            left:
              var: "num"
            right:
              literal: 10
    )");
    
    std::cout << "\nExecuting forall with tracing:" << std::endl;
    auto result = engine.executeLogic(forall_expr, ctx);
    std::cout << "Forall result: " << (result.asBoolean() ? "true" : "false") << std::endl;
    
    engine.printTrace();
    engine.enableTracing(false);
}

int main() {
    std::cout << "=== Debugging Quantifier Issue ===" << std::endl;
    
    try {
        debugQuantifierTest();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
}
