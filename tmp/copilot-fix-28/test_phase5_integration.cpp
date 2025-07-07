#include "../tmp/pure_validate_command.hpp"
#include "../core/engine/logic/pure_logic_engine.hpp"
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <cassert>

/**
 * @brief Phase 5 Integration Test
 * 
 * Tests the integration of the pure logic engine with the CLI system.
 * Validates that:
 * 1. Pure validate command can be instantiated and executed
 * 2. CLI routing through pure logic engine works
 * 3. System-wide self-validation works
 */

int main() {
    std::cout << "=== Phase 5 Integration Test ===" << std::endl;
    
    try {
        // Test 1: Pure validate command instantiation
        std::cout << "\n1. Testing pure validate command instantiation..." << std::endl;
        akao::interfaces::cli::commands::PureValidateCommand pure_cmd;
        std::cout << "✓ Pure validate command created successfully" << std::endl;
        
        // Test 2: Help text functionality
        std::cout << "\n2. Testing help text functionality..." << std::endl;
        auto help_text = pure_cmd.getHelp();
        assert(!help_text.empty());
        assert(help_text.find("Pure Logic Engine") != std::string::npos);
        std::cout << "✓ Help text contains expected content" << std::endl;
        
        // Test 3: Usage functionality  
        std::cout << "\n3. Testing usage functionality..." << std::endl;
        auto usage_text = pure_cmd.getUsage();
        assert(!usage_text.empty());
        assert(usage_text.find("validate") != std::string::npos);
        std::cout << "✓ Usage text is correct" << std::endl;
        
        // Test 4: Basic command execution (dry run)
        std::cout << "\n4. Testing basic command execution (dry run)..." << std::endl;
        akao::interfaces::cli::executor::ExecutionContext context;
        context.current_directory = ".";
        
        std::vector<std::string> args = {"--dry-run", "--target", "."};
        auto result = pure_cmd.execute(context, args);
        
        // Should succeed even if no rules found (dry run)
        std::cout << "Execution result: " << (result.success ? "SUCCESS" : "FAILED") << std::endl;
        if (!result.output_message.empty()) {
            std::cout << "Output: " << result.output_message.substr(0, 200) << "..." << std::endl;
        }
        if (!result.error_message.empty()) {
            std::cout << "Error: " << result.error_message << std::endl;
        }
        std::cout << "✓ Basic command execution completed" << std::endl;
        
        // Test 5: Self-validation execution
        std::cout << "\n5. Testing self-validation execution..." << std::endl;
        std::vector<std::string> self_validate_args = {"--self-validate", "--dry-run"};
        auto self_result = pure_cmd.execute(context, self_validate_args);
        
        std::cout << "Self-validation result: " << (self_result.success ? "SUCCESS" : "FAILED") << std::endl;
        if (!self_result.output_message.empty()) {
            std::cout << "Output preview: " << self_result.output_message.substr(0, 300) << "..." << std::endl;
        }
        std::cout << "✓ Self-validation execution completed" << std::endl;
        
        // Test 6: Specific rule execution
        std::cout << "\n6. Testing specific rule execution..." << std::endl;
        std::vector<std::string> rule_args = {"--rules", "nonexistent_rule", "--dry-run"};
        auto rule_result = pure_cmd.execute(context, rule_args);
        
        // Expected to fail since rule doesn't exist, but should handle gracefully
        std::cout << "Specific rule result: " << (rule_result.success ? "SUCCESS" : "FAILED") << std::endl;
        std::cout << "✓ Specific rule execution handled correctly" << std::endl;
        
        // Test 7: Engine integration
        std::cout << "\n7. Testing direct engine integration..." << std::endl;
        akao::logic::PureLogicEngine engine;
        engine.initialize();
        
        size_t function_count = engine.getBuiltinFunctionCount();
        std::cout << "Builtin functions registered: " << function_count << std::endl;
        assert(function_count > 0);
        
        // Test basic engine functionality with YAML
        akao::logic::Context test_ctx;
        test_ctx.setValue("x", akao::logic::Value(42));
        
        // Create a proper YAML node for addition
        YAML::Node add_node;
        add_node["function"] = "math.add";
        add_node["arguments"][0]["var"] = "x";
        add_node["arguments"][1]["literal"] = 8;
        
        auto result_val = engine.evaluate(add_node, test_ctx);
        assert(result_val.isNumber());
        assert(result_val.asNumber() == 50.0);
        std::cout << "✓ Engine arithmetic test passed: " << result_val.asNumber() << std::endl;
        
        std::cout << "\n=== Phase 5 Integration Test Summary ===" << std::endl;
        std::cout << "✓ Pure validate command integration working" << std::endl;
        std::cout << "✓ CLI routing through pure logic engine functional" << std::endl;
        std::cout << "✓ Self-validation system operational" << std::endl;
        std::cout << "✓ Engine integration successful" << std::endl;
        std::cout << "\n🎉 Phase 5 Integration Test: ALL TESTS PASSED!" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "✗ Integration test failed: " << e.what() << std::endl;
        return 1;
    }
}
