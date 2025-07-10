/**
 * @id: akao:file:core:node:interface:test_inode_interface:v1
 * @doc: Comprehensive test suite for INode interface system validating interface compliance, parameter validation, node definitions, capabilities, registry helpers, and mock node implementations to ensure complete functionality of the node-based workflow foundation.
 * @specification: Complete test suite for INode interface system and supporting components
 * @scope: Core node interface testing
 * @timeline: 2025-07-10
 * @rationale: Verify INode interface system works correctly with comprehensive test coverage
 * @methodology: Unit testing with mock implementations and comprehensive validation scenarios
 * @references: ["akao:class:core:node:interface:inode:v1", "akao:class:core:node:interface:nodedefinition:v1"]
 */

#include "inode.hpp"
#include "node_definition.hpp"
#include "../step1-1-1/nodevalue.hpp"
#include "../step1-1-2/nodecontext.hpp"
#include <iostream>
#include <cassert>
#include <chrono>
#include <memory>

using namespace akao::node;

/**
 * @id: akao:class:core:node:interface:test:mock_node:v1
 * @doc: Mock node implementation for testing INode interface providing simulated execution, validation, and metadata functionality to verify interface compliance and proper behavior of the node system without external dependencies.
 * @specification: Mock node implementation for comprehensive interface testing
 * @scope: Core node interface testing
 * @timeline: 2025-07-10
 * @rationale: Provide concrete implementation for testing abstract INode interface
 * @methodology: Mock implementation with configurable behavior for testing scenarios
 * @references: ["akao:class:core:node:interface:inode:v1"]
 */
class MockNode : public INode {
private:
    std::string node_id_;
    std::string node_type_;
    std::string version_;
    bool should_fail_execution_;
    bool should_fail_validation_;
    
public:
    MockNode(const std::string& id = "test.mock.v1", 
             const std::string& type = "test.mock", 
             const std::string& version = "v1",
             bool fail_execution = false,
             bool fail_validation = false)
        : node_id_(id), node_type_(type), version_(version),
          should_fail_execution_(fail_execution), should_fail_validation_(fail_validation) {}
    
    std::string getNodeId() const override { return node_id_; }
    std::string getNodeType() const override { return node_type_; }
    std::string getVersion() const override { return version_; }
    
    std::string getDisplayName() const override { return "Mock Test Node"; }
    std::string getDescription() const override { return "A mock node for testing purposes"; }
    std::vector<std::string> getCategories() const override { return {"test", "mock"}; }
    
    NodeDefinition getDefinition() const override {
        NodeDefinition def(node_id_, node_type_, version_);
        def.setDisplayName(getDisplayName());
        def.setDescription(getDescription());
        def.setCategories({"test", "mock"});
        def.addTag("testing");
        def.addTag("mock");
        
        // Add some test parameters
        NodeParameter param1("input_text", ParameterType::STRING);
        param1.setDisplayName("Input Text");
        param1.setDescription("Text input for processing");
        ParameterValidation validation1;
        validation1.required = true;
        param1.setValidation(validation1);
        def.addParameter(param1);
        
        NodeParameter param2("count", ParameterType::INTEGER);
        param2.setDisplayName("Count");
        param2.setDescription("Number of iterations");
        param2.setDefaultValue(NodeValue(int64_t(1)));
        def.addParameter(param2);
        
        NodeParameter param3("enabled", ParameterType::BOOLEAN);
        param3.setDisplayName("Enabled");
        param3.setDescription("Whether processing is enabled");
        param3.setDefaultValue(NodeValue(true));
        def.addParameter(param3);
        
        return def;
    }
    
    ExecutionResult execute(const NodeContext& context) override {
        ExecutionResult result;
        
        if (should_fail_execution_) {
            result.setStatus(ExecutionStatus::FAILURE);
            result.addError("Simulated execution failure");
            return result;
        }
        
        // Process input items
        for (const auto& item : context.getInputItems()) {
            NodeItem output_item;
            
            // Create output based on input
            NodeValue output_data = NodeValue::Object{
                {"processed", NodeValue(true)},
                {"original_data", item.getJson()},
                {"node_id", NodeValue(node_id_)},
                {"timestamp", NodeValue(std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count()))}
            };
            
            output_item.setJson(output_data);
            result.addItem(output_item);
        }
        
        // If no input items, create a default output
        if (context.getInputCount() == 0) {
            NodeItem default_item;
            default_item.setJson(NodeValue::Object{
                {"message", NodeValue("Mock node executed successfully")},
                {"node_id", NodeValue(node_id_)},
                {"parameters_count", NodeValue(int64_t(context.getAllParameters().size()))}
            });
            result.addItem(default_item);
        }
        
        result.setStatus(ExecutionStatus::SUCCESS);
        return result;
    }
    
    ValidationResult validate(const std::map<std::string, NodeValue>& parameters) override {
        ValidationResult result;
        
        if (should_fail_validation_) {
            result.addError("parameters", "Simulated validation failure", ValidationResult::Severity::ERROR);
            return result;
        }
        
        // Validate required parameters
        auto it = parameters.find("input_text");
        if (it == parameters.end() || it->second.isNull()) {
            result.addError("input_text", "Required parameter 'input_text' is missing", ValidationResult::Severity::ERROR);
        } else if (!it->second.isString()) {
            result.addError("input_text", "Parameter 'input_text' must be a string", ValidationResult::Severity::ERROR);
        }
        
        // Validate optional parameters
        auto count_it = parameters.find("count");
        if (count_it != parameters.end() && !count_it->second.isNull()) {
            if (!count_it->second.isInteger()) {
                result.addError("count", "Parameter 'count' must be an integer", ValidationResult::Severity::ERROR);
            } else {
                int64_t count_value = count_it->second.asInteger();
                if (count_value < 0) {
                    result.addError("count", "Parameter 'count' must be non-negative", ValidationResult::Severity::ERROR);
                } else if (count_value > 1000) {
                    result.addError("count", "Parameter 'count' exceeds maximum value of 1000", ValidationResult::Severity::WARNING);
                }
            }
        }
        
        auto enabled_it = parameters.find("enabled");
        if (enabled_it != parameters.end() && !enabled_it->second.isNull()) {
            if (!enabled_it->second.isBoolean()) {
                result.addError("enabled", "Parameter 'enabled' must be a boolean", ValidationResult::Severity::ERROR);
            }
        }
        
        return result;
    }
    
    ValidationResult validateContext(const NodeContext& context) override {
        ValidationResult result;
        
        // Validate that we have some input or parameters
        if (context.getInputCount() == 0 && context.getAllParameters().empty()) {
            result.addError("context", "Node requires either input items or parameters", ValidationResult::Severity::WARNING);
        }
        
        // Validate parameters using the parameter validation
        auto param_result = validate(context.getAllParameters());
        if (!param_result.isValid()) {
            for (const auto& error : param_result.getErrors()) {
                result.addError(error);
            }
        }
        
        return result;
    }
    
    bool canExecute(const NodeContext& context) override {
        if (should_fail_execution_) {
            return false;
        }
        
        auto validation = validateContext(context);
        return validation.getFatalErrorCount() == 0;
    }
    
    std::unique_ptr<INode> clone() const override {
        return std::make_unique<MockNode>(node_id_, node_type_, version_, 
                                         should_fail_execution_, should_fail_validation_);
    }
};

// Test function declarations
void testValidationResult();
void testNodeCapabilities();
void testNodeDefinition();
void testMockNodeImplementation();
void testNodeRegistryHelpers();
void testParameterValidation();
void testNodeExamples();
void testNodeCloning();

int main() {
    std::cout << "=== INode Interface Test Suite ===" << std::endl;
    
    try {
        std::cout << "\n1. Testing ValidationResult..." << std::endl;
        testValidationResult();
        std::cout << "✓ ValidationResult tests passed" << std::endl;
        
        std::cout << "\n2. Testing NodeCapabilities..." << std::endl;
        testNodeCapabilities();
        std::cout << "✓ NodeCapabilities tests passed" << std::endl;
        
        std::cout << "\n3. Testing NodeDefinition..." << std::endl;
        testNodeDefinition();
        std::cout << "✓ NodeDefinition tests passed" << std::endl;
        
        std::cout << "\n4. Testing MockNode Implementation..." << std::endl;
        testMockNodeImplementation();
        std::cout << "✓ MockNode implementation tests passed" << std::endl;
        
        std::cout << "\n5. Testing NodeRegistryHelpers..." << std::endl;
        testNodeRegistryHelpers();
        std::cout << "✓ NodeRegistryHelpers tests passed" << std::endl;
        
        std::cout << "\n6. Testing Parameter Validation..." << std::endl;
        testParameterValidation();
        std::cout << "✓ Parameter validation tests passed" << std::endl;
        
        std::cout << "\n7. Testing Node Examples..." << std::endl;
        testNodeExamples();
        std::cout << "✓ Node examples tests passed" << std::endl;
        
        std::cout << "\n8. Testing Node Cloning..." << std::endl;
        testNodeCloning();
        std::cout << "✓ Node cloning tests passed" << std::endl;
        
        std::cout << "\n=== ALL TESTS PASSED ===" << std::endl;
        std::cout << "✓ INode interface system working correctly" << std::endl;
        std::cout << "✓ Mock node executes successfully" << std::endl;
        std::cout << "✓ Parameters validate correctly" << std::endl;
        std::cout << "✓ Node definitions are complete and valid" << std::endl;
        std::cout << "✓ Registry helpers function properly" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}

void testValidationResult() {
    // Test empty validation result
    ValidationResult result1;
    assert(result1.isValid());
    assert(!result1.hasErrors());
    assert(!result1.hasWarnings());
    assert(result1.getErrorCount() == 0);
    
    // Test adding errors
    ValidationResult result2;
    result2.addError("param1", "Test error", ValidationResult::Severity::ERROR);
    assert(!result2.isValid());
    assert(result2.hasErrors());
    assert(result2.getErrorCount() == 1);
    
    // Test adding warnings
    ValidationResult result3;
    result3.addError("param2", "Test warning", ValidationResult::Severity::WARNING);
    assert(result3.isValid()); // Should still be valid with warnings only
    assert(result3.hasWarnings());
    assert(result3.getWarningCount() == 1);
    
    // Test severity filtering
    ValidationResult result4;
    result4.addError("param1", "Error 1", ValidationResult::Severity::ERROR);
    result4.addError("param2", "Warning 1", ValidationResult::Severity::WARNING);
    result4.addError("param3", "Fatal 1", ValidationResult::Severity::FATAL);
    result4.addError("param4", "Info 1", ValidationResult::Severity::INFO);
    
    assert(result4.getErrorsBySeverity(ValidationResult::Severity::ERROR).size() == 1);
    assert(result4.getErrorsBySeverity(ValidationResult::Severity::WARNING).size() == 1);
    assert(result4.getErrorsBySeverity(ValidationResult::Severity::FATAL).size() == 1);
    assert(result4.getErrorsBySeverity(ValidationResult::Severity::INFO).size() == 1);
    assert(result4.getFatalErrorCount() == 1);
    
    // Test toString
    std::string result_str = result4.toString();
    assert(result_str.find("ValidationResult") != std::string::npos);
    assert(result_str.find("valid: false") != std::string::npos);
}

void testNodeCapabilities() {
    NodeCapabilities caps;
    
    // Test defaults
    assert(!caps.supports_streaming);
    assert(caps.supports_batch_processing);
    assert(!caps.supports_parallel_execution);
    assert(caps.supports_retry);
    assert(caps.supports_akao_logic);
    assert(caps.max_memory_mb == 256);
    assert(caps.max_execution_time_seconds == 300);
    
    // Test modification
    caps.supports_streaming = true;
    caps.supports_parallel_execution = true;
    caps.max_memory_mb = 512;
    
    assert(caps.supports_streaming);
    assert(caps.supports_parallel_execution);
    assert(caps.max_memory_mb == 512);
    
    // Test toString
    std::string caps_str = caps.toString();
    assert(caps_str.find("NodeCapabilities") != std::string::npos);
    assert(caps_str.find("streaming: true") != std::string::npos);
    assert(caps_str.find("512MB") != std::string::npos);
}

void testNodeDefinition() {
    NodeDefinition def("test.node.v1", "test.node", "v1");
    
    // Test basic properties
    assert(def.getNodeId() == "test.node.v1");
    assert(def.getNodeType() == "test.node");
    assert(def.getVersion() == "v1");
    assert(def.getDisplayName() == "test.node"); // Default
    
    // Test setters
    def.setDisplayName("Test Node");
    def.setDescription("A test node for validation");
    def.setAuthor("Test Author");
    def.addCategory("testing");
    def.addTag("unit-test");
    
    assert(def.getDisplayName() == "Test Node");
    assert(def.getDescription() == "A test node for validation");
    assert(def.getAuthor() == "Test Author");
    assert(def.getCategories().size() == 1);
    assert(def.getCategories()[0] == "testing");
    assert(def.getTags().size() == 1);
    assert(def.getTags()[0] == "unit-test");
    
    // Test validation
    ValidationResult validation = def.validate();
    assert(validation.isValid());
    
    // Test invalid definition
    NodeDefinition invalid_def("", "", "");
    ValidationResult invalid_validation = invalid_def.validate();
    assert(!invalid_validation.isValid());
    assert(invalid_validation.getErrorCount() >= 3); // Should have errors for empty fields
    
    // Test JSON serialization
    std::string json = def.toJson();
    assert(json.find("test.node.v1") != std::string::npos);
    assert(json.find("Test Node") != std::string::npos);
    
    // Test toString
    std::string def_str = def.toString();
    assert(def_str.find("NodeDefinition") != std::string::npos);
    assert(def_str.find("test.node.v1") != std::string::npos);
}

void testMockNodeImplementation() {
    MockNode node;
    
    // Test basic interface
    assert(node.getNodeId() == "test.mock.v1");
    assert(node.getNodeType() == "test.mock");
    assert(node.getVersion() == "v1");
    assert(node.getDisplayName() == "Mock Test Node");
    assert(!node.getDescription().empty());
    assert(node.getCategories().size() == 2);
    
    // Test node definition
    NodeDefinition def = node.getDefinition();
    assert(def.getNodeId() == node.getNodeId());
    assert(def.getNodeType() == node.getNodeType());
    assert(def.getVersion() == node.getVersion());
    assert(def.getParameters().size() >= 3); // Should have test parameters
    
    // Test execution with empty context
    NodeContext empty_context;
    bool can_execute = node.canExecute(empty_context);
    assert(can_execute); // Should be able to execute with empty context
    
    ExecutionResult result = node.execute(empty_context);
    assert(result.getStatus() == ExecutionStatus::SUCCESS);
    assert(result.getItems().size() == 1); // Should create default output
    
    // Test execution with input data
    NodeItem input_item;
    input_item.setJson(NodeValue::Object{{"test", NodeValue("data")}});
    // Create context with input data using constructor
    std::vector<NodeItem> input_items = {input_item};
    NodeContext context_with_data(input_items);
    
    ExecutionResult result_with_data = node.execute(context_with_data);
    assert(result_with_data.getStatus() == ExecutionStatus::SUCCESS);
    assert(result_with_data.getItems().size() == 1);
    
    // Verify output structure
    const NodeItem& output = result_with_data.getItems()[0];
    assert(output.getJson().isObject());
    NodeValue::Object output_obj = output.getJson().asObject();
    assert(output_obj.find("processed") != output_obj.end());
    assert(output_obj["processed"].asBoolean() == true);
    assert(output_obj.find("node_id") != output_obj.end());
    assert(output_obj["node_id"].asString() == "test.mock.v1");
    
    // Test failure scenarios
    MockNode failing_node("test.fail.v1", "test.fail", "v1", true, false);
    ExecutionResult fail_result = failing_node.execute(empty_context);
    assert(fail_result.getStatus() == ExecutionStatus::FAILURE);
    assert(!fail_result.getErrors().empty());
    
    assert(!failing_node.canExecute(empty_context));
}

void testNodeRegistryHelpers() {
    // Test node ID validation
    assert(NodeRegistryHelpers::isValidNodeId("system.logger.v1"));
    assert(NodeRegistryHelpers::isValidNodeId("data.mapper.v2"));
    assert(NodeRegistryHelpers::isValidNodeId("complex.multi_part.name.v10"));
    assert(!NodeRegistryHelpers::isValidNodeId(""));
    assert(!NodeRegistryHelpers::isValidNodeId("invalid-id"));
    assert(!NodeRegistryHelpers::isValidNodeId("system.logger"));
    assert(!NodeRegistryHelpers::isValidNodeId("System.Logger.v1")); // Uppercase
    
    // Test node type validation
    assert(NodeRegistryHelpers::isValidNodeType("system.logger"));
    assert(NodeRegistryHelpers::isValidNodeType("data.mapper"));
    assert(NodeRegistryHelpers::isValidNodeType("complex.multi_part.name"));
    assert(!NodeRegistryHelpers::isValidNodeType(""));
    assert(!NodeRegistryHelpers::isValidNodeType("system.logger.v1")); // Should not have version
    assert(!NodeRegistryHelpers::isValidNodeType("System.Logger")); // Uppercase
    
    // Test version validation
    assert(NodeRegistryHelpers::isValidVersion("v1"));
    assert(NodeRegistryHelpers::isValidVersion("v10"));
    assert(NodeRegistryHelpers::isValidVersion("1.0.0"));
    assert(NodeRegistryHelpers::isValidVersion("2.5.1"));
    assert(!NodeRegistryHelpers::isValidVersion(""));
    assert(!NodeRegistryHelpers::isValidVersion("1"));
    assert(!NodeRegistryHelpers::isValidVersion("V1")); // Uppercase
    assert(!NodeRegistryHelpers::isValidVersion("1.0")); // Incomplete semantic version
    
    // Test node factory registration
    auto& factories = NodeRegistryHelpers::getNodeFactories();
    size_t initial_count = factories.size();
    
    NodeRegistryHelpers::registerNodeFactory("test.mock", []() {
        return std::make_unique<MockNode>();
    });
    
    assert(factories.size() == initial_count + 1);
    assert(factories.find("test.mock") != factories.end());
    
    // Test factory creation
    auto factory = factories["test.mock"];
    auto created_node = factory();
    assert(created_node != nullptr);
    assert(created_node->getNodeType() == "test.mock");
    
    // Test node type discovery
    std::vector<std::string> available_types = NodeRegistryHelpers::discoverAvailableNodeTypes();
    assert(std::find(available_types.begin(), available_types.end(), "test.mock") != available_types.end());
    
    // Test node definition validation
    MockNode test_node;
    NodeDefinition def = test_node.getDefinition();
    ValidationResult def_validation = NodeRegistryHelpers::validateNodeDefinition(def);
    assert(def_validation.isValid());
    
    // Test node implementation validation
    ValidationResult impl_validation = NodeRegistryHelpers::validateNodeImplementation(test_node);
    assert(impl_validation.isValid());
    
    // Test failing validation
    MockNode failing_node("", "", "", false, true);
    ValidationResult failing_validation = NodeRegistryHelpers::validateNodeImplementation(failing_node);
    assert(!failing_validation.isValid());
    
    // Test loading node definitions
    std::vector<NodeDefinition> definitions = NodeRegistryHelpers::loadNodeDefinitions("/test/path");
    assert(definitions.size() >= 2); // Should return at least the built-in definitions
    
    bool found_logger = false, found_mapper = false;
    for (const auto& def : definitions) {
        if (def.getNodeId() == "system.logger.v1") found_logger = true;
        if (def.getNodeId() == "data.mapper.v1") found_mapper = true;
    }
    assert(found_logger && found_mapper);
}

void testParameterValidation() {
    MockNode node;
    
    // Test valid parameters
    std::map<std::string, NodeValue> valid_params = {
        {"input_text", NodeValue("Hello World")},
        {"count", NodeValue(int64_t(5))},
        {"enabled", NodeValue(true)}
    };
    
    ValidationResult valid_result = node.validate(valid_params);
    assert(valid_result.isValid());
    assert(valid_result.getErrorCount() == 0);
    
    // Test missing required parameter
    std::map<std::string, NodeValue> missing_required = {
        {"count", NodeValue(int64_t(5))},
        {"enabled", NodeValue(true)}
    };
    
    ValidationResult missing_result = node.validate(missing_required);
    assert(!missing_result.isValid());
    assert(missing_result.getErrorCount() >= 1);
    
    // Test wrong parameter types
    std::map<std::string, NodeValue> wrong_types = {
        {"input_text", NodeValue(int64_t(123))}, // Should be string
        {"count", NodeValue("not_a_number")},    // Should be integer
        {"enabled", NodeValue("not_a_boolean")}  // Should be boolean
    };
    
    ValidationResult wrong_type_result = node.validate(wrong_types);
    assert(!wrong_type_result.isValid());
    assert(wrong_type_result.getErrorCount() >= 3);
    
    // Test boundary values
    std::map<std::string, NodeValue> boundary_params = {
        {"input_text", NodeValue("Valid text")},
        {"count", NodeValue(int64_t(-1))}, // Should trigger error (negative)
        {"enabled", NodeValue(true)}
    };
    
    ValidationResult boundary_result = node.validate(boundary_params);
    assert(!boundary_result.isValid());
    
    // Test warning conditions
    std::map<std::string, NodeValue> warning_params = {
        {"input_text", NodeValue("Valid text")},
        {"count", NodeValue(int64_t(1500))}, // Should trigger warning (too large)
        {"enabled", NodeValue(true)}
    };
    
    ValidationResult warning_result = node.validate(warning_params);
    assert(warning_result.hasWarnings());
    assert(warning_result.getWarningCount() >= 1);
    
    // Test context validation
    NodeContext context;
    context.setParameter("input_text", NodeValue("Test"));
    context.setParameter("count", NodeValue(int64_t(3)));
    
    ValidationResult context_result = node.validateContext(context);
    assert(context_result.isValid());
    
    // Test failing validation node
    MockNode failing_node("test.fail.v1", "test.fail", "v1", false, true);
    ValidationResult fail_result = failing_node.validate(valid_params);
    assert(!fail_result.isValid());
}

void testNodeExamples() {
    NodeExample example("Basic Usage", "Shows how to use the mock node");
    
    assert(example.title == "Basic Usage");
    assert(example.description == "Shows how to use the mock node");
    assert(example.input_parameters.empty());
    assert(example.sample_input_data.empty());
    assert(example.expected_output_data.empty());
    
    // Add some example data
    example.input_parameters["input_text"] = NodeValue("Example text");
    example.input_parameters["count"] = NodeValue(int64_t(2));
    
    NodeItem sample_input;
    sample_input.setJson(NodeValue::Object{{"data", NodeValue("sample")}});
    example.sample_input_data.push_back(sample_input);
    
    NodeItem expected_output;
    expected_output.setJson(NodeValue::Object{
        {"processed", NodeValue(true)},
        {"result", NodeValue("processed sample")}
    });
    example.expected_output_data.push_back(expected_output);
    
    example.use_case_description = "Use this node to process text data";
    
    assert(example.input_parameters.size() == 2);
    assert(example.sample_input_data.size() == 1);
    assert(example.expected_output_data.size() == 1);
    assert(!example.use_case_description.empty());
}

void testNodeCloning() {
    MockNode original("original.node.v1", "original.node", "v1");
    
    // Test cloning
    auto cloned = original.clone();
    assert(cloned != nullptr);
    assert(cloned.get() != &original); // Different instances
    
    // Test that cloned node has same properties
    assert(cloned->getNodeId() == original.getNodeId());
    assert(cloned->getNodeType() == original.getNodeType());
    assert(cloned->getVersion() == original.getVersion());
    assert(cloned->getDisplayName() == original.getDisplayName());
    assert(cloned->getDescription() == original.getDescription());
    
    // Test that cloned node functions correctly
    NodeContext context;
    context.setParameter("input_text", NodeValue("test"));
    
    ExecutionResult original_result = original.execute(context);
    ExecutionResult cloned_result = cloned->execute(context);
    
    assert(original_result.getStatus() == cloned_result.getStatus());
    assert(original_result.getItems().size() == cloned_result.getItems().size());
    
    // Test cloning with different configurations
    MockNode failing_original("fail.node.v1", "fail.node", "v1", true, true);
    auto failing_cloned = failing_original.clone();
    
    assert(!failing_cloned->canExecute(context));
    
    ValidationResult original_validation = failing_original.validate(context.getAllParameters());
    ValidationResult cloned_validation = failing_cloned->validate(context.getAllParameters());
    
    assert(original_validation.isValid() == cloned_validation.isValid());
}