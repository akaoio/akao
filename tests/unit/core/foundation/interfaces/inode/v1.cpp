/**
 * @id: akao:file:tests:unit:core:foundation:interfaces:inode:v1
 * @doc: Comprehensive unit tests for INode interface and support classes. Tests NodeContext, NodeParameters, ValidationResult, and mock node implementation to ensure proper interface compliance and functionality.
 * @specification: Unit tests for INode interface and support classes
 * @scope: Foundation interfaces testing
 * @timeline: 2025-07-10
 * @rationale: Ensure reliability of node interface system
 * @methodology: Complete test coverage with mock implementations
 * @references: ["akao:class:core:foundation:interfaces:inode:v1"]
 */

#include "core/foundation/interfaces/inode/v1.hpp"
#include <cassert>
#include <iostream>

using namespace akao::foundation::interfaces;
using namespace akao::foundation::types;

// Mock node implementation for testing
class MockNode : public INode {
public:
    std::string getNodeId() const override {
        return "mock-node-001";
    }
    
    std::string getNodeType() const override {
        return "processing";
    }
    
    std::string getVersion() const override {
        return "1.0.0";
    }
    
    ValidationResult validate(const NodeParameters& params) const override {
        ValidationResult result;
        
        if (!params.hasParameter("input")) {
            result.addError("Missing required parameter 'input'");
        }
        
        if (params.hasParameter("threshold")) {
            auto threshold = params.getParameter("threshold");
            if (threshold.isInteger() && threshold.asInteger() < 0) {
                result.addError("Threshold must be non-negative");
            }
        }
        
        return result;
    }
    
    ExecutionResult execute(const NodeContext& context, const NodeParameters& params) override {
        // Simple mock execution
        auto input = params.getParameter("input");
        
        if (input.isString()) {
            NodeValue result("Processed: " + input.asString());
            return ExecutionResult::success(result);
        }
        
        return ExecutionResult::error("Invalid input type");
    }
    
    std::string getDescription() const override {
        return "Mock node for testing purposes";
    }
    
    NodeValue getParameterSchema() const override {
        NodeValue::Object schema;
        
        NodeValue::Object inputParam;
        inputParam["type"] = NodeValue("string");
        inputParam["required"] = NodeValue(true);
        inputParam["description"] = NodeValue("Input text to process");
        
        NodeValue::Object thresholdParam;
        thresholdParam["type"] = NodeValue("integer");
        thresholdParam["required"] = NodeValue(false);
        thresholdParam["default"] = NodeValue(static_cast<int64_t>(10));
        thresholdParam["description"] = NodeValue("Processing threshold");
        
        schema["input"] = NodeValue(inputParam);
        schema["threshold"] = NodeValue(thresholdParam);
        
        return NodeValue(schema);
    }
};

void testNodeContext() {
    std::cout << "Testing NodeContext..." << std::endl;
    
    // Basic construction
    NodeValue inputs("test input");
    NodeContext context("test-node", inputs);
    
    assert(context.getNodeId() == "test-node");
    assert(context.getInputs().asString() == "test input");
    
    // Environment operations
    context.setEnvironment("debug", NodeValue(true));
    context.setEnvironment("timeout", NodeValue(static_cast<int64_t>(30)));
    
    assert(context.hasEnvironment("debug"));
    assert(context.getEnvironment("debug").asBoolean() == true);
    assert(context.getEnvironment("timeout").asInteger() == 30);
    assert(!context.hasEnvironment("missing"));
    
    // Workflow identification
    context.setWorkflowId("workflow-123");
    context.setExecutionId("execution-456");
    
    assert(context.getWorkflowId() == "workflow-123");
    assert(context.getExecutionId() == "execution-456");
    
    // Object input access
    NodeValue::Object objInput;
    objInput["name"] = NodeValue("test");
    objInput["count"] = NodeValue(static_cast<int64_t>(5));
    
    context.setInputs(NodeValue(objInput));
    assert(context.hasInput("name"));
    assert(context.getInput("name").asString() == "test");
    assert(context.getInput("count").asInteger() == 5);
    assert(!context.hasInput("missing"));
    
    std::cout << "NodeContext test passed!" << std::endl;
}

void testNodeParameters() {
    std::cout << "Testing NodeParameters..." << std::endl;
    
    NodeParameters params;
    
    // Basic parameter operations
    params.setParameter("input", NodeValue("test data"));
    params.setParameter("threshold", NodeValue(static_cast<int64_t>(15)));
    params.setParameter("enabled", NodeValue(true));
    
    assert(params.hasParameter("input"));
    assert(params.getParameter("input").asString() == "test data");
    assert(params.getParameter("threshold").asInteger() == 15);
    assert(params.getParameter("enabled").asBoolean() == true);
    assert(!params.hasParameter("missing"));
    
    // Parameter types and requirements
    params.setParameterType("input", "string");
    params.setParameterType("threshold", "integer");
    params.setParameterRequired("input", true);
    params.setParameterRequired("threshold", false);
    
    assert(params.getParameterType("input") == "string");
    assert(params.getParameterType("threshold") == "integer");
    assert(params.isParameterRequired("input"));
    assert(!params.isParameterRequired("threshold"));
    
    // Validation with correct parameters
    auto errors = params.validate();
    assert(errors.empty());
    assert(params.isValid());
    
    // Validation with missing required parameter
    NodeParameters invalidParams;
    invalidParams.setParameterRequired("required_param", true);
    
    auto invalidErrors = invalidParams.validate();
    assert(!invalidErrors.empty());
    assert(!invalidParams.isValid());
    
    // Type validation
    NodeParameters typeParams;
    typeParams.setParameter("number", NodeValue("not a number"));
    typeParams.setParameterType("number", "integer");
    
    auto typeErrors = typeParams.validate();
    assert(!typeErrors.empty());
    assert(typeErrors[0].find("must be an integer") != std::string::npos);
    
    std::cout << "NodeParameters test passed!" << std::endl;
}

void testValidationResult() {
    std::cout << "Testing ValidationResult..." << std::endl;
    
    // Valid result
    ValidationResult validResult(true);
    assert(validResult.isValid());
    assert(!validResult.hasErrors());
    assert(!validResult.hasWarnings());
    
    // Invalid result with errors
    ValidationResult invalidResult(false);
    invalidResult.addError("Test error 1");
    invalidResult.addError("Test error 2");
    
    assert(!invalidResult.isValid());
    assert(invalidResult.hasErrors());
    assert(invalidResult.getErrors().size() == 2);
    assert(invalidResult.getErrors()[0] == "Test error 1");
    
    // Warnings
    ValidationResult warningResult(true);
    warningResult.addWarning("Test warning");
    
    assert(warningResult.hasWarnings());
    assert(warningResult.getWarnings().size() == 1);
    assert(warningResult.getWarnings()[0] == "Test warning");
    
    // Adding error should invalidate
    warningResult.addError("Test error");
    assert(!warningResult.isValid());
    
    // Merge validation results
    ValidationResult result1(true);
    result1.addWarning("Warning 1");
    
    ValidationResult result2(false);
    result2.addError("Error 1");
    result2.addWarning("Warning 2");
    
    result1.merge(result2);
    assert(!result1.isValid());
    assert(result1.getErrors().size() == 1);
    assert(result1.getWarnings().size() == 2);
    
    // String representation
    std::string str = invalidResult.toString();
    assert(str.find("valid=false") != std::string::npos);
    assert(str.find("errors=") != std::string::npos);
    
    std::cout << "ValidationResult test passed!" << std::endl;
}

void testMockNode() {
    std::cout << "Testing MockNode implementation..." << std::endl;
    
    MockNode node;
    
    // Basic node information
    assert(node.getNodeId() == "mock-node-001");
    assert(node.getNodeType() == "processing");
    assert(node.getVersion() == "1.0.0");
    assert(node.getDescription() == "Mock node for testing purposes");
    
    // Parameter schema
    NodeValue schema = node.getParameterSchema();
    assert(schema.isObject());
    assert(schema.hasKey("input"));
    assert(schema.hasKey("threshold"));
    
    // Valid parameter validation
    NodeParameters validParams;
    validParams.setParameter("input", NodeValue("test data"));
    
    ValidationResult validResult = node.validate(validParams);
    assert(validResult.isValid());
    
    // Invalid parameter validation (missing required)
    NodeParameters invalidParams;
    ValidationResult invalidResult = node.validate(invalidParams);
    assert(!invalidResult.isValid());
    assert(invalidResult.hasErrors());
    
    // Threshold validation
    NodeParameters thresholdParams;
    thresholdParams.setParameter("input", NodeValue("test"));
    thresholdParams.setParameter("threshold", NodeValue(static_cast<int64_t>(-5)));
    
    ValidationResult thresholdResult = node.validate(thresholdParams);
    assert(!thresholdResult.isValid());
    
    // Successful execution
    NodeContext context("mock-node-001");
    NodeParameters execParams;
    execParams.setParameter("input", NodeValue("Hello"));
    
    ExecutionResult execResult = node.execute(context, execParams);
    assert(execResult.isSuccess());
    assert(execResult.getData().asString() == "Processed: Hello");
    
    // Failed execution
    NodeParameters failParams;
    failParams.setParameter("input", NodeValue(static_cast<int64_t>(123)));
    
    ExecutionResult failResult = node.execute(context, failParams);
    assert(failResult.isError());
    
    std::cout << "MockNode test passed!" << std::endl;
}

void testInterfaceCompliance() {
    std::cout << "Testing interface compliance..." << std::endl;
    
    // Test polymorphic usage
    std::unique_ptr<INode> node = std::make_unique<MockNode>();
    
    assert(node->getNodeId() == "mock-node-001");
    assert(node->getNodeType() == "processing");
    
    NodeParameters params;
    params.setParameter("input", NodeValue("test"));
    
    ValidationResult result = node->validate(params);
    assert(result.isValid());
    
    NodeContext context("test");
    ExecutionResult execResult = node->execute(context, params);
    assert(execResult.isSuccess());
    
    std::cout << "Interface compliance test passed!" << std::endl;
}

int main() {
    std::cout << "Running INode interface unit tests..." << std::endl;
    
    try {
        testNodeContext();
        testNodeParameters();
        testValidationResult();
        testMockNode();
        testInterfaceCompliance();
        
        std::cout << "All INode interface tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
}