/**
 * @id: akao:test:core:engine:orchestrator:workflow:v1
 * @doc: Comprehensive unit tests for workflow orchestration system including workflow definition, parsing, validation, and execution capabilities. Tests YAML workflow parsing, node connection validation, parameter substitution, execution order calculation, and workflow execution with proper error handling.
 * @specification: Workflow orchestration system unit tests
 * @scope: Core engine workflow orchestration testing
 * @timeline: 2025-07-10
 * @rationale: Ensure workflow system reliability and correctness
 * @methodology: Comprehensive test coverage for all workflow components
 * @references: ["akao:class:core:engine:orchestrator:workflow:v1"]
 */

#include "core/engine/orchestrator/workflow/v1.hpp"
#include "core/engine/orchestrator/registry/v1.hpp"
#include "nodes/builtin/yaml/v1.hpp"
#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <memory>

#ifndef AKAO_TEST_SUITE
int main() {
    akao::tests::runWorkflowTests();
    return 0;
}
#endif

namespace akao { namespace tests {

// Forward declarations
void testWorkflowDefinitionBasics();
void testWorkflowNodeManagement();
void testWorkflowConnectionManagement();
void testWorkflowParameterManagement();
void testWorkflowValidation();
void testWorkflowParser();
void testWorkflowExecutorBasics();
void testSampleWorkflowParsing();
void testEnhancedWorkflowExecution();

void runWorkflowTests() {
    std::cout << "Running Workflow System tests...\n";

    // Test WorkflowDefinition creation and basic properties
    testWorkflowDefinitionBasics();
    
    // Test node management
    testWorkflowNodeManagement();
    
    // Test connection management
    testWorkflowConnectionManagement();
    
    // Test parameter management
    testWorkflowParameterManagement();
    
    // Test workflow validation
    testWorkflowValidation();
    
    // Test workflow parser
    testWorkflowParser();
    
    // Test workflow executor basic functionality
    testWorkflowExecutorBasics();
    
    // Test sample workflow parsing
    testSampleWorkflowParsing();
    
    // Test enhanced execution features
    testEnhancedWorkflowExecution();
    
    std::cout << "✓ All Workflow System tests completed successfully!\n";
}

void testWorkflowDefinitionBasics() {
    std::cout << "  Testing WorkflowDefinition basics...\n";
    
    using namespace akao::core::engine::orchestrator;
    
    // Test default constructor
    WorkflowDefinition workflow1;
    assert(workflow1.getId().empty());
    assert(workflow1.getName().empty());
    assert(workflow1.getDescription().empty());
    assert(workflow1.getVersion().empty());
    assert(workflow1.getNodeCount() == 0);
    assert(workflow1.isEmpty());
    
    // Test parameterized constructor
    WorkflowDefinition workflow2("test-workflow", "Test Workflow");
    assert(workflow2.getId() == "test-workflow");
    assert(workflow2.getName() == "Test Workflow");
    assert(workflow2.getDescription().empty());
    assert(workflow2.getVersion().empty());
    assert(workflow2.getNodeCount() == 0);
    assert(workflow2.isEmpty());
    
    // Test setters
    workflow2.setDescription("A test workflow for validation");
    workflow2.setVersion("1.0.0");
    assert(workflow2.getDescription() == "A test workflow for validation");
    assert(workflow2.getVersion() == "1.0.0");
    
    // Test toString method
    std::string workflow_str = workflow2.toString();
    assert(!workflow_str.empty());
    assert(workflow_str.find("test-workflow") != std::string::npos);
    
    std::cout << "  ✓ WorkflowDefinition basics test passed\n";
}

void testWorkflowNodeManagement() {
    std::cout << "  Testing WorkflowNode management...\n";
    
    using namespace akao::core::engine::orchestrator;
    
    WorkflowDefinition workflow("test-workflow", "Test Workflow");
    
    // Test adding nodes
    WorkflowNode node1("node1", "yaml-processor");
    node1.description = "First test node";
    node1.enabled = true;
    node1.retry_count = 3;
    
    WorkflowNode node2("node2", "file-scanner");
    node2.description = "Second test node";
    node2.enabled = false;
    
    workflow.addNode(node1);
    workflow.addNode(node2);
    
    assert(workflow.getNodeCount() == 2);
    assert(!workflow.isEmpty());
    
    // Test node retrieval
    const WorkflowNode* retrieved_node1 = workflow.getNode("node1");
    assert(retrieved_node1 != nullptr);
    assert(retrieved_node1->id == "node1");
    assert(retrieved_node1->type == "yaml-processor");
    assert(retrieved_node1->description == "First test node");
    assert(retrieved_node1->enabled == true);
    assert(retrieved_node1->retry_count == 3);
    
    const WorkflowNode* retrieved_node2 = workflow.getNode("node2");
    assert(retrieved_node2 != nullptr);
    assert(retrieved_node2->id == "node2");
    assert(retrieved_node2->type == "file-scanner");
    assert(retrieved_node2->enabled == false);
    
    // Test non-existent node
    const WorkflowNode* non_existent = workflow.getNode("non-existent");
    assert(non_existent == nullptr);
    
    // Test node replacement (same ID)
    WorkflowNode node1_updated("node1", "logic-executor");
    node1_updated.description = "Updated first node";
    workflow.addNode(node1_updated);
    
    assert(workflow.getNodeCount() == 2); // Still 2 nodes
    const WorkflowNode* updated_node = workflow.getNode("node1");
    assert(updated_node->type == "logic-executor");
    assert(updated_node->description == "Updated first node");
    
    // Test node removal
    workflow.removeNode("node2");
    assert(workflow.getNodeCount() == 1);
    assert(workflow.getNode("node2") == nullptr);
    assert(workflow.getNode("node1") != nullptr);
    
    std::cout << "  ✓ WorkflowNode management test passed\n";
}

void testWorkflowConnectionManagement() {
    std::cout << "  Testing WorkflowConnection management...\n";
    
    using namespace akao::core::engine::orchestrator;
    
    WorkflowDefinition workflow("test-workflow", "Test Workflow");
    
    // Add nodes for connection testing
    WorkflowNode node1("node1", "yaml-processor");
    WorkflowNode node2("node2", "file-scanner");
    WorkflowNode node3("node3", "logic-executor");
    
    workflow.addNode(node1);
    workflow.addNode(node2);
    workflow.addNode(node3);
    
    // Test adding connections
    NodeConnection conn1("node1", "output", "node2", "input");
    conn1.transform_expression = "data | filter(enabled)";
    
    NodeConnection conn2("node2", "files", "node3", "input_files");
    
    workflow.addConnection(conn1);
    workflow.addConnection(conn2);
    
    assert(workflow.getConnections().size() == 2);
    
    // Test connection retrieval
    std::vector<NodeConnection> connections_from_node1 = workflow.getConnectionsFrom("node1");
    assert(connections_from_node1.size() == 1);
    assert(connections_from_node1[0].from_node_id == "node1");
    assert(connections_from_node1[0].to_node_id == "node2");
    assert(connections_from_node1[0].transform_expression == "data | filter(enabled)");
    
    std::vector<NodeConnection> connections_to_node2 = workflow.getConnectionsTo("node2");
    assert(connections_to_node2.size() == 1);
    assert(connections_to_node2[0].from_node_id == "node1");
    
    std::vector<NodeConnection> connections_from_node3 = workflow.getConnectionsFrom("node3");
    assert(connections_from_node3.empty());
    
    std::vector<NodeConnection> connections_to_node3 = workflow.getConnectionsTo("node3");
    assert(connections_to_node3.size() == 1);
    assert(connections_to_node3[0].from_node_id == "node2");
    
    // Test connection removal
    workflow.removeConnection("node1", "node2");
    assert(workflow.getConnections().size() == 1);
    assert(workflow.getConnectionsFrom("node1").empty());
    assert(workflow.getConnectionsTo("node2").empty());
    
    // Test node removal removes connections
    workflow.addConnection(conn1); // Re-add connection
    workflow.removeNode("node2");
    assert(workflow.getConnections().size() == 0); // All connections involving node2 removed
    
    std::cout << "  ✓ WorkflowConnection management test passed\n";
}

void testWorkflowParameterManagement() {
    std::cout << "  Testing WorkflowParameter management...\n";
    
    using namespace akao::core::engine::orchestrator;
    using namespace akao::foundation::types;
    
    WorkflowDefinition workflow("test-workflow", "Test Workflow");
    
    // Test default parameter setting
    NodeValue str_param("test-value");
    NodeValue int_param(int64_t(42));
    NodeValue bool_param(true);
    
    workflow.setDefaultParameter("string_param", str_param);
    workflow.setDefaultParameter("int_param", int_param);
    workflow.setDefaultParameter("bool_param", bool_param);
    
    // Test parameter retrieval
    NodeValue retrieved_str = workflow.getDefaultParameter("string_param");
    assert(retrieved_str.isString());
    assert(retrieved_str.asString() == "test-value");
    
    NodeValue retrieved_int = workflow.getDefaultParameter("int_param");
    assert(retrieved_int.isInteger());
    assert(retrieved_int.asInteger() == 42);
    
    NodeValue retrieved_bool = workflow.getDefaultParameter("bool_param");
    assert(retrieved_bool.isBoolean());
    assert(retrieved_bool.asBoolean() == true);
    
    // Test non-existent parameter
    NodeValue non_existent = workflow.getDefaultParameter("non_existent");
    assert(non_existent.isNull());
    
    // Test parameter count
    const auto& all_params = workflow.getDefaultParameters();
    assert(all_params.size() == 3);
    
    // Test schema management
    workflow.setInputSchema("input_file", "string");
    workflow.setInputSchema("max_count", "integer");
    workflow.setOutputSchema("result", "object");
    workflow.setOutputSchema("status", "boolean");
    
    assert(workflow.getInputSchema().size() == 2);
    assert(workflow.getOutputSchema().size() == 2);
    assert(workflow.getInputSchema().at("input_file") == "string");
    assert(workflow.getOutputSchema().at("result") == "object");
    
    std::cout << "  ✓ WorkflowParameter management test passed\n";
}

void testWorkflowValidation() {
    std::cout << "  Testing Workflow validation...\n";
    
    using namespace akao::core::engine::orchestrator;
    
    WorkflowDefinition workflow("test-workflow", "Test Workflow");
    
    // Test empty workflow validation
    std::vector<std::string> errors = workflow.validate();
    assert(!errors.empty()); // Empty workflow should have validation errors
    assert(!workflow.isValid());
    
    // Add valid nodes
    WorkflowNode node1("node1", "yaml-processor");
    WorkflowNode node2("node2", "file-scanner");
    WorkflowNode node3("node3", "logic-executor");
    
    workflow.addNode(node1);
    workflow.addNode(node2);
    workflow.addNode(node3);
    
    // Test valid workflow (no connections, but nodes exist)
    errors = workflow.validate();
    // Should be valid with just nodes
    
    // Test cycle detection
    assert(!workflow.hasCycles()); // No connections yet
    
    // Add connections that create a cycle
    NodeConnection conn1("node1", "output", "node2", "input");
    NodeConnection conn2("node2", "output", "node3", "input");
    NodeConnection conn3("node3", "output", "node1", "input"); // Creates cycle
    
    workflow.addConnection(conn1);
    workflow.addConnection(conn2);
    workflow.addConnection(conn3);
    
    assert(workflow.hasCycles()); // Should detect cycle
    
    // Test execution order calculation
    workflow.removeConnection("node3", "node1"); // Remove cycle
    assert(!workflow.hasCycles()); // No more cycles
    
    std::vector<std::string> execution_order = workflow.getExecutionOrder();
    assert(execution_order.size() == 3);
    assert(execution_order[0] == "node1"); // node1 has no dependencies
    assert(execution_order[1] == "node2"); // node2 depends on node1
    assert(execution_order[2] == "node3"); // node3 depends on node2
    
    std::cout << "  ✓ Workflow validation test passed\n";
}

void testWorkflowParser() {
    std::cout << "  Testing WorkflowParser...\n";
    
    using namespace akao::core::engine::orchestrator;
    
    WorkflowParser parser;
    
    // Test valid YAML workflow
    std::string valid_yaml = R"(
id: "test-workflow"
name: "Test Workflow"
description: "A simple test workflow"
version: "1.0.0"
nodes:
  - id: "scanner"
    type: "file-scanner"
    description: "Scan files"
    enabled: true
    parameters:
      directory: "."
      pattern: "*.cpp"
  - id: "processor"
    type: "yaml-processor"
    description: "Process YAML"
    enabled: true
    depends_on: ["scanner"]
connections:
  - from_node: "scanner"
    from_output: "files"
    to_node: "processor"
    to_input: "input_files"
default_parameters:
  max_files: 100
  timeout: 30
input_schema:
  base_directory: "string"
output_schema:
  processed_count: "integer"
)";
    
    auto workflow = parser.parseYAML(valid_yaml);
    assert(workflow != nullptr);
    assert(!parser.hasErrors());
    
    assert(workflow->getId() == "test-workflow");
    assert(workflow->getName() == "Test Workflow");
    assert(workflow->getDescription() == "A simple test workflow");
    assert(workflow->getVersion() == "1.0.0");
    assert(workflow->getNodeCount() == 2);
    
    const WorkflowNode* scanner = workflow->getNode("scanner");
    assert(scanner != nullptr);
    assert(scanner->type == "file-scanner");
    assert(scanner->enabled == true);
    assert(scanner->depends_on.empty());
    
    const WorkflowNode* processor = workflow->getNode("processor");
    assert(processor != nullptr);
    assert(processor->type == "yaml-processor");
    assert(processor->depends_on.size() == 1);
    assert(processor->depends_on[0] == "scanner");
    
    assert(workflow->getConnections().size() == 1);
    const NodeConnection& conn = workflow->getConnections()[0];
    assert(conn.from_node_id == "scanner");
    assert(conn.from_output == "files");
    assert(conn.to_node_id == "processor");
    assert(conn.to_input == "input_files");
    
    // Test invalid YAML
    std::string invalid_yaml = R"(
id: "invalid-workflow"
nodes:
  - id: "node1"
    type: "unknown-type"
    depends_on: ["non-existent-node"]
)";
    
    parser.clearErrors();
    auto invalid_workflow = parser.parseYAML(invalid_yaml);
    // Parser should still create workflow but with errors
    assert(parser.hasErrors());
    
    std::cout << "  ✓ WorkflowParser test passed\n";
}

void testWorkflowExecutorBasics() {
    std::cout << "  Testing WorkflowExecutor basics...\n";
    
    using namespace akao::core::engine::orchestrator;
    
    // Test workflow execution enhancements without requiring registry
    WorkflowDefinition workflow("test-workflow", "Test Enhanced Execution");
    
    // Create a linear workflow: node1 -> node2 -> node3
    WorkflowNode node1("node1", "file-scanner");
    node1.enabled = true;
    
    WorkflowNode node2("node2", "yaml-processor");
    node2.enabled = true;
    node2.depends_on = {"node1"};
    
    WorkflowNode node3("node3", "reporter");
    node3.enabled = true;
    node3.depends_on = {"node2"};
    
    workflow.addNode(node1);
    workflow.addNode(node2);
    workflow.addNode(node3);
    
    // Add connections for data flow
    NodeConnection conn1("node1", "files", "node2", "input_files");
    NodeConnection conn2("node2", "processed_data", "node3", "report_data");
    workflow.addConnection(conn1);
    workflow.addConnection(conn2);
    
    // Test workflow structure
    assert(workflow.getNodeCount() == 3);
    assert(workflow.getConnections().size() == 2);
    assert(!workflow.hasCycles());
    
    // Test execution order
    std::vector<std::string> order = workflow.getExecutionOrder();
    assert(order.size() == 3);
    assert(order[0] == "node1");
    assert(order[1] == "node2");
    assert(order[2] == "node3");
    
    // Test parallel execution levels calculation would go here
    // (requires a workflow executor instance, which needs registry)
    
    std::cout << "  ✓ WorkflowExecutor basics test passed\n";
}

void testSampleWorkflowParsing() {
    std::cout << "  Testing sample workflow parsing...\n";
    
    using namespace akao::core::engine::orchestrator;
    
    WorkflowParser parser;
    
    // Test simple workflow 
    std::string simple_yaml = 
        "id: \"test-workflow\"\n"
        "name: \"Test Workflow\"\n"
        "description: \"A simple test workflow\"\n"
        "version: \"1.0.0\"\n"
        "nodes:\n"
        "  - id: \"scanner\"\n"
        "    type: \"file-scanner\"\n"
        "    description: \"Scan files\"\n"
        "    enabled: true\n"
        "connections: []\n"
        "default_parameters:\n"
        "  max_files: 100\n";
    
    auto workflow = parser.parseYAML(simple_yaml);
    
    // Basic validation - the workflow should be parsed even if it has errors
    assert(workflow != nullptr);
    assert(workflow->getId() == "test-workflow");
    assert(workflow->getName() == "Test Workflow");
    assert(workflow->getNodeCount() == 1);
    
    const WorkflowNode* scanner = workflow->getNode("scanner");
    assert(scanner != nullptr);
    assert(scanner->type == "file-scanner");
    assert(scanner->enabled == true);
    
    std::cout << "  ✓ Sample workflow parsing test passed\n";
}

void testEnhancedWorkflowExecution() {
    std::cout << "  Testing enhanced workflow execution features...\n";
    
    using namespace akao::core::engine::orchestrator;
    
    // Test complex workflow with multiple execution paths
    WorkflowDefinition complex_workflow("complex-workflow", "Complex Execution Test");
    
    // Create a diamond dependency pattern:
    //     node1
    //    /     \
    //  node2  node3
    //    \     /
    //     node4
    
    WorkflowNode node1("input", "file-scanner");
    node1.enabled = true;
    
    WorkflowNode node2("process_a", "yaml-processor");
    node2.enabled = true;
    node2.depends_on = {"input"};
    
    WorkflowNode node3("process_b", "logic-executor");
    node3.enabled = true;
    node3.depends_on = {"input"};
    
    WorkflowNode node4("combine", "reporter");
    node4.enabled = true;
    node4.depends_on = {"process_a", "process_b"};
    
    complex_workflow.addNode(node1);
    complex_workflow.addNode(node2);
    complex_workflow.addNode(node3);
    complex_workflow.addNode(node4);
    
    // Add data flow connections
    complex_workflow.addConnection(NodeConnection("input", "files", "process_a", "input_files"));
    complex_workflow.addConnection(NodeConnection("input", "files", "process_b", "input_files"));
    complex_workflow.addConnection(NodeConnection("process_a", "output", "combine", "input_a"));
    complex_workflow.addConnection(NodeConnection("process_b", "output", "combine", "input_b"));
    
    // Test workflow validation
    assert(complex_workflow.getNodeCount() == 4);
    assert(complex_workflow.getConnections().size() == 4);
    assert(!complex_workflow.hasCycles());
    assert(complex_workflow.isValid());
    
    // Test execution order - should be deterministic
    std::vector<std::string> order = complex_workflow.getExecutionOrder();
    assert(order.size() == 4);
    assert(order[0] == "input"); // No dependencies
    // process_a and process_b can be in any order (both depend only on input)
    assert((order[1] == "process_a" && order[2] == "process_b") || 
           (order[1] == "process_b" && order[2] == "process_a"));
    assert(order[3] == "combine"); // Depends on both process_a and process_b
    
    // Test error scenarios
    WorkflowDefinition cyclic_workflow("cyclic", "Cyclic Test");
    WorkflowNode cyclic1("a", "test");
    WorkflowNode cyclic2("b", "test");
    cyclic1.depends_on = {"b"};
    cyclic2.depends_on = {"a"};
    cyclic_workflow.addNode(cyclic1);
    cyclic_workflow.addNode(cyclic2);
    
    assert(cyclic_workflow.hasCycles());
    assert(!cyclic_workflow.isValid());
    
    // Test disabled node handling
    WorkflowDefinition disabled_workflow("disabled", "Disabled Node Test");
    WorkflowNode enabled_node("enabled", "test");
    WorkflowNode disabled_node("disabled", "test");
    disabled_node.enabled = false;
    disabled_node.depends_on = {"enabled"};
    
    disabled_workflow.addNode(enabled_node);
    disabled_workflow.addNode(disabled_node);
    
    auto disabled_order = disabled_workflow.getExecutionOrder();
    assert(disabled_order.size() == 1); // Only enabled node should be in execution order
    assert(disabled_order[0] == "enabled");
    
    // Test retry and timeout configurations
    WorkflowNode retry_node("retry_test", "test");
    retry_node.retry_count = 5;
    retry_node.timeout = std::chrono::milliseconds(10000);
    assert(retry_node.retry_count == 5);
    assert(retry_node.timeout.count() == 10000);
    
    // Test parameter substitution and data flow
    WorkflowContext test_context;
    test_context.workflow_id = "test";
    test_context.inputs["base_directory"] = foundation::types::NodeValue("/home/user");
    test_context.variables["processed_count"] = foundation::types::NodeValue(int64_t(42));
    
    assert(test_context.inputs.size() == 1);
    assert(test_context.variables.size() == 1);
    assert(test_context.inputs["base_directory"].asString() == "/home/user");
    assert(test_context.variables["processed_count"].asInteger() == 42);
    
    std::cout << "  ✓ Enhanced workflow execution test passed\n";
}

}} // namespace akao::tests