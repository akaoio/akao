/**
 * @id: akao:test:nodes:builtin:logic:v1
 * @doc: Simplified unit tests for clean .a format executor node validating core functionality. Tests static utilities, expression evaluation, and built-in functions using simple assertions.
 * @specification: Unit tests for .a format executor node
 * @scope: Unit test for builtin logic node
 * @timeline: 2025-07-10
 * @rationale: Ensure reliable logical expression evaluation functionality
 * @methodology: Simple tests focused on core functionality
 * @references: ["akao:class:nodes:builtin:logic:v1"]
 */

#include "nodes/builtin/logic/v1.hpp"
#include <iostream>
#include <cassert>
#include <string>
#include <map>

namespace akao {
namespace tests {

class AkaoLogicExecutorNodeTest {
public:
    void runAllTests() {
        std::cout << "Running AkaoLogicExecutorNode tests...\n";
        
        testNodeInfo();
        testStaticUtilities();
        testLogicContext();
        testExpressionEvaluator();
        testBuiltinFunctions();
        
        std::cout << "All AkaoLogicExecutorNode tests passed!\n";
    }

private:
    void testNodeInfo() {
        std::cout << "Testing node info...\n";
        
        nodes::builtin::AkaoLogicExecutorNode node;
        
        assert(node.getNodeId() == "akao:node:builtin:logic:v1");
        assert(node.getNodeType() == "logic_executor");
        assert(node.getVersion() == "1.0.0");
        assert(!node.getDescription().empty());
        
        std::cout << "✓ Node info test passed\n";
    }
    
    void testStaticUtilities() {
        std::cout << "Testing static utilities...\n";
        
        // Test executeExpression
        assert(nodes::builtin::AkaoLogicExecutorNode::executeExpression("true") == true);
        assert(nodes::builtin::AkaoLogicExecutorNode::executeExpression("false") == false);
        assert(nodes::builtin::AkaoLogicExecutorNode::executeExpression("5 > 3") == true);
        assert(nodes::builtin::AkaoLogicExecutorNode::executeExpression("5 < 3") == false);
        assert(nodes::builtin::AkaoLogicExecutorNode::executeExpression("invalid_expression") == false);
        
        // Test executeExpressionWithContext
        std::map<std::string, foundation::types::NodeValue> variables;
        variables["x"] = static_cast<int64_t>(10);
        variables["y"] = static_cast<int64_t>(5);
        
        assert(nodes::builtin::AkaoLogicExecutorNode::executeExpressionWithContext("x > y", variables) == true);
        assert(nodes::builtin::AkaoLogicExecutorNode::executeExpressionWithContext("x == y", variables) == false);
        assert(nodes::builtin::AkaoLogicExecutorNode::executeExpressionWithContext("x < y", variables) == false);
        
        // Test evaluateExpression
        auto result = nodes::builtin::AkaoLogicExecutorNode::evaluateExpression("42");
        assert(std::holds_alternative<int64_t>(result));
        assert(std::get<int64_t>(result) == 42);
        
        result = nodes::builtin::AkaoLogicExecutorNode::evaluateExpression("\"hello\"");
        assert(std::holds_alternative<std::string>(result));
        assert(std::get<std::string>(result) == "hello");
        
        // Test isValidExpression
        assert(nodes::builtin::AkaoLogicExecutorNode::isValidExpression("true"));
        assert(nodes::builtin::AkaoLogicExecutorNode::isValidExpression("5 > 3"));
        assert(nodes::builtin::AkaoLogicExecutorNode::isValidExpression("42"));
        assert(nodes::builtin::AkaoLogicExecutorNode::isValidExpression("\"hello\""));
        assert(!nodes::builtin::AkaoLogicExecutorNode::isValidExpression(""));
        
        // Test validateExpression
        auto errors = nodes::builtin::AkaoLogicExecutorNode::validateExpression("true");
        assert(errors.empty());
        
        errors = nodes::builtin::AkaoLogicExecutorNode::validateExpression("");
        assert(!errors.empty());
        
        std::cout << "✓ Static utilities test passed\n";
    }
    
    void testLogicContext() {
        std::cout << "Testing logic context...\n";
        
        nodes::builtin::LogicContext context;
        
        // Test variable operations
        context.setVariable("test_var", std::string("hello"));
        assert(context.hasVariable("test_var"));
        
        auto value = context.getVariable("test_var");
        assert(std::holds_alternative<std::string>(value));
        assert(std::get<std::string>(value) == "hello");
        
        // Test integer variables
        context.setVariable("number", static_cast<int64_t>(42));
        assert(context.hasVariable("number"));
        
        auto numValue = context.getVariable("number");
        assert(std::holds_alternative<int64_t>(numValue));
        assert(std::get<int64_t>(numValue) == 42);
        
        // Test boolean variables
        context.setVariable("flag", true);
        assert(context.hasVariable("flag"));
        
        auto flagValue = context.getVariable("flag");
        assert(std::holds_alternative<bool>(flagValue));
        assert(std::get<bool>(flagValue) == true);
        
        // Test function operations
        context.registerFunction("test_func", [](const std::vector<nodes::builtin::LogicValue>& args) {
            return static_cast<int64_t>(42);
        });
        
        assert(context.hasFunction("test_func"));
        
        auto funcResult = context.callFunction("test_func", {});
        assert(std::holds_alternative<int64_t>(funcResult));
        assert(std::get<int64_t>(funcResult) == 42);
        
        // Test utility methods
        auto varNames = context.getVariableNames();
        assert(varNames.size() >= 3); // test_var, number, flag
        
        auto funcNames = context.getFunctionNames();
        assert(funcNames.size() >= 1); // test_func
        
        std::cout << "✓ Logic context test passed\n";
    }
    
    void testExpressionEvaluator() {
        std::cout << "Testing expression evaluator...\n";
        
        nodes::builtin::LogicContext context;
        context.setVariable("x", static_cast<int64_t>(10));
        context.setVariable("greeting", std::string("hello"));
        context.setVariable("active", true);
        
        nodes::builtin::AkaoRuntime runtime(context);
        
        // Test literal values
        auto result = runtime.execute("42");
        assert(std::holds_alternative<int64_t>(result));
        assert(std::get<int64_t>(result) == 42);
        
        result = runtime.execute("\"world\"");
        assert(std::holds_alternative<std::string>(result));
        assert(std::get<std::string>(result) == "world");
        
        result = runtime.execute("true");
        assert(std::holds_alternative<bool>(result));
        assert(std::get<bool>(result) == true);
        
        result = runtime.execute("false");
        assert(std::holds_alternative<bool>(result));
        assert(std::get<bool>(result) == false);
        
        // Test variable evaluation
        result = runtime.execute("x");
        assert(std::holds_alternative<int64_t>(result));
        assert(std::get<int64_t>(result) == 10);
        
        result = runtime.execute("greeting");
        assert(std::holds_alternative<std::string>(result));
        assert(std::get<std::string>(result) == "hello");
        
        result = runtime.execute("active");
        assert(std::holds_alternative<bool>(result));
        assert(std::get<bool>(result) == true);
        
        // Test binary operations
        result = runtime.execute("x > 5");
        assert(std::holds_alternative<bool>(result));
        assert(std::get<bool>(result) == true);
        
        result = runtime.execute("x == 10");
        assert(std::holds_alternative<bool>(result));
        assert(std::get<bool>(result) == true);
        
        result = runtime.execute("x != 5");
        assert(std::holds_alternative<bool>(result));
        assert(std::get<bool>(result) == true);
        
        result = runtime.execute("true && active");
        assert(std::holds_alternative<bool>(result));
        assert(std::get<bool>(result) == true);
        
        result = runtime.execute("false || active");
        assert(std::holds_alternative<bool>(result));
        assert(std::get<bool>(result) == true);
        
        std::cout << "✓ Expression evaluator test passed\n";
    }
    
    void testBuiltinFunctions() {
        std::cout << "Testing built-in functions...\n";
        
        nodes::builtin::LogicContext context;
        nodes::builtin::AkaoLogicExecutorNode node;
        
        // Initialize built-in functions using the node's method
        node.initializeBuiltinFunctions(context);
        
        nodes::builtin::AkaoRuntime runtime(context);
        
        // Test string functions
        auto result = runtime.execute("string.length(\"hello\")");
        assert(std::holds_alternative<int64_t>(result));
        assert(std::get<int64_t>(result) == 5);
        
        result = runtime.execute("string.concat(\"hello\", \"world\")");
        assert(std::holds_alternative<std::string>(result));
        assert(std::get<std::string>(result) == "helloworld");
        
        result = runtime.execute("string.contains(\"hello world\", \"world\")");
        assert(std::holds_alternative<bool>(result));
        assert(std::get<bool>(result) == true);
        
        result = runtime.execute("string.contains(\"hello world\", \"xyz\")");
        assert(std::holds_alternative<bool>(result));
        assert(std::get<bool>(result) == false);
        
        // Test math functions
        result = runtime.execute("math.add(5, 3)");
        assert(std::holds_alternative<int64_t>(result));
        assert(std::get<int64_t>(result) == 8);
        
        result = runtime.execute("math.multiply(4, 7)");
        assert(std::holds_alternative<int64_t>(result));
        assert(std::get<int64_t>(result) == 28);
        
        std::cout << "✓ Built-in functions test passed\n";
    }
};

// Export test runner function for main test runner
void runAkaoLogicExecutorTests() {
    AkaoLogicExecutorNodeTest test;
    test.runAllTests();
}

} // namespace tests
} // namespace akao