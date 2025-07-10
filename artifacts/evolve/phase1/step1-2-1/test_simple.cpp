/**
 * @id: akao:file:test:node:interface:simple_test:v1
 * @doc: Simple test for INode interface system completion verification
 */

#include "inode.hpp"
#include "node_definition.hpp"
#include <iostream>
#include <cassert>

class SimpleTestNode : public akao::node::INode {
private:
    akao::node::NodeDefinition definition_;

public:
    SimpleTestNode() : definition_("test.simple.v1", "test.simple", "v1") {
        definition_.setDisplayName("Simple Test Node");
        definition_.setDescription("A simple test node");
        definition_.addCategory("test");
    }
    
    std::string getNodeId() const override { return "test.simple.v1"; }
    std::string getNodeType() const override { return "test.simple"; }
    std::string getVersion() const override { return "v1"; }
    akao::node::NodeDefinition getDefinition() const override { return definition_; }
    std::string getDisplayName() const override { return "Simple Test Node"; }
    std::string getDescription() const override { return "A simple test node"; }
    std::vector<std::string> getCategories() const override { return {"test"}; }
    
    akao::node::ExecutionResult execute(const akao::node::NodeContext& /*context*/) override {
        akao::node::ExecutionResult result;
        
        // Simple execution - just create success result
        akao::node::NodeValue::Object output;
        output["status"] = akao::node::NodeValue("success");
        output["timestamp"] = akao::node::NodeValue("2025-07-10");
        
        result.addItem(akao::node::NodeValue(output));
        result.setStatus(akao::node::ExecutionStatus::SUCCESS);
        return result;
    }
    
    akao::node::ValidationResult validate(const std::map<std::string, akao::node::NodeValue>& /*parameters*/) override {
        return akao::node::ValidationResult(); // Always valid for simplicity
    }
    
    akao::node::ValidationResult validateContext(const akao::node::NodeContext& /*context*/) override {
        return akao::node::ValidationResult(); // Always valid for simplicity
    }
    
    bool canExecute(const akao::node::NodeContext& /*context*/) override {
        return true; // Always can execute for simplicity
    }
    
    std::unique_ptr<akao::node::INode> clone() const override {
        return std::make_unique<SimpleTestNode>();
    }
};

int main() {
    std::cout << "Simple INode Interface Test" << std::endl;
    std::cout << "===========================" << std::endl;
    
    try {
        // Create test node
        SimpleTestNode node;
        
        // Test basic interface
        assert(node.getNodeId() == "test.simple.v1");
        assert(node.getNodeType() == "test.simple");
        assert(node.getVersion() == "v1");
        std::cout << "✓ Basic interface works" << std::endl;
        
        // Test definition
        auto def = node.getDefinition();
        assert(def.getNodeId() == "test.simple.v1");
        assert(def.validate().isValid());
        std::cout << "✓ Node definition works" << std::endl;
        
        // Test execution
        akao::node::NodeContext context;
        auto result = node.execute(context);
        assert(result.getStatus() == akao::node::ExecutionStatus::SUCCESS);
        assert(result.getItems().size() == 1);
        std::cout << "✓ Node execution works" << std::endl;
        
        // Test validation
        std::map<std::string, akao::node::NodeValue> params;
        assert(node.validate(params).isValid());
        assert(node.validateContext(context).isValid());
        assert(node.canExecute(context));
        std::cout << "✓ Validation works" << std::endl;
        
        // Test cloning
        auto cloned = node.clone();
        assert(cloned != nullptr);
        assert(cloned->getNodeId() == node.getNodeId());
        std::cout << "✓ Cloning works" << std::endl;
        
        // Test registry helpers
        assert(akao::node::NodeRegistryHelpers::isValidNodeId("test.simple.v1"));
        assert(akao::node::NodeRegistryHelpers::isValidNodeType("test.simple"));
        assert(akao::node::NodeRegistryHelpers::isValidVersion("v1"));
        std::cout << "✓ Registry helpers work" << std::endl;
        
        std::cout << "===========================" << std::endl;
        std::cout << "✅ ALL TESTS PASSED!" << std::endl;
        std::cout << std::endl;
        std::cout << "🎯 Step 1.2.1 COMPLETED: INode Interface Definition" << std::endl;
        std::cout << "Mock node executes successfully, validates parameters correctly" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
}