/**
 * @id: akao:class:core:engine:orchestrator:workflow:v1
 * @doc: Comprehensive workflow orchestration system providing YAML-based workflow definition, parsing, validation, and execution capabilities. Supports complex node graphs with data flow, parameter substitution, conditional execution, error handling, and parallel processing. Enables sophisticated automation workflows with dependency management and execution monitoring for the Akao node-based architecture.
 * @specification: Workflow definition and orchestration engine
 * @scope: Core engine workflow orchestration system
 * @timeline: 2025-07-10
 * @rationale: Enable powerful workflow automation in node-based architecture
 * @methodology: YAML-based workflow definitions with graph execution and dependency management
 * @references: ["akao:interface:core:foundation:interfaces:inode:v1", "akao:class:core:engine:orchestrator:registry:v1"]
 */

#pragma once

#include "core/foundation/interfaces/inode/v1.hpp"
#include "core/engine/orchestrator/registry/v1.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <variant>
#include <functional>
#include <chrono>
#include <set>
#include <shared_mutex>

namespace akao {
namespace core {
namespace engine {
namespace orchestrator {

/**
 * Forward declarations
 */
class WorkflowDefinition;
class WorkflowParser;
class WorkflowExecutor;

/**
 * Workflow node connection definition
 */
struct NodeConnection {
    std::string from_node_id;
    std::string from_output;
    std::string to_node_id;
    std::string to_input;
    
    // Optional transformation
    std::string transform_expression;
    
    NodeConnection() = default;
    NodeConnection(const std::string& from_node, const std::string& from_out,
                  const std::string& to_node, const std::string& to_in)
        : from_node_id(from_node), from_output(from_out), 
          to_node_id(to_node), to_input(to_in) {}
};

/**
 * Workflow node definition
 */
struct WorkflowNode {
    std::string id;
    std::string type;
    std::string description;
    std::map<std::string, foundation::types::NodeValue> parameters;
    std::map<std::string, std::string> input_mappings;
    std::map<std::string, std::string> output_mappings;
    
    // Execution properties
    bool enabled = true;
    int retry_count = 0;
    std::chrono::milliseconds timeout{30000}; // 30 seconds default
    
    // Dependencies
    std::vector<std::string> depends_on;
    
    WorkflowNode() = default;
    WorkflowNode(const std::string& node_id, const std::string& node_type)
        : id(node_id), type(node_type) {}
};

/**
 * Workflow execution context
 */
struct WorkflowContext {
    std::string workflow_id;
    std::string execution_id;
    std::chrono::system_clock::time_point start_time;
    std::map<std::string, foundation::types::NodeValue> variables;
    std::map<std::string, foundation::types::NodeValue> inputs;
    std::map<std::string, foundation::types::NodeValue> outputs;
    
    // THREAD SAFETY FIX: Add mutex for concurrent access during parallel execution
    mutable std::unique_ptr<std::shared_mutex> context_mutex;
    
    WorkflowContext() : start_time(std::chrono::system_clock::now()), 
                       context_mutex(std::make_unique<std::shared_mutex>()) {}
    
    // Make non-copyable but movable
    WorkflowContext(const WorkflowContext&) = delete;
    WorkflowContext& operator=(const WorkflowContext&) = delete;
    WorkflowContext(WorkflowContext&&) = default;
    WorkflowContext& operator=(WorkflowContext&&) = default;
};

/**
 * Workflow execution result
 */
struct WorkflowExecutionResult {
    bool success = false;
    std::string error_message;
    std::chrono::duration<double> execution_time{0};
    std::map<std::string, foundation::types::ExecutionResult> node_results;
    std::map<std::string, foundation::types::NodeValue> final_outputs;
    
    WorkflowExecutionResult() = default;
    explicit WorkflowExecutionResult(bool success_status) : success(success_status) {}
};

/**
 * Main workflow definition class
 */
class WorkflowDefinition {
private:
    std::string id_;
    std::string name_;
    std::string description_;
    std::string version_;
    std::vector<WorkflowNode> nodes_;
    std::vector<NodeConnection> connections_;
    std::map<std::string, foundation::types::NodeValue> default_parameters_;
    std::map<std::string, std::string> input_schema_;
    std::map<std::string, std::string> output_schema_;

public:
    WorkflowDefinition() = default;
    WorkflowDefinition(const std::string& workflow_id, const std::string& workflow_name)
        : id_(workflow_id), name_(workflow_name) {}

    // Basic properties
    const std::string& getId() const { return id_; }
    const std::string& getName() const { return name_; }
    const std::string& getDescription() const { return description_; }
    const std::string& getVersion() const { return version_; }
    
    void setId(const std::string& id) { id_ = id; }
    void setName(const std::string& name) { name_ = name; }
    void setDescription(const std::string& description) { description_ = description; }
    void setVersion(const std::string& version) { version_ = version; }

    // Node management
    void addNode(const WorkflowNode& node);
    void removeNode(const std::string& node_id);
    WorkflowNode* getNode(const std::string& node_id);
    const WorkflowNode* getNode(const std::string& node_id) const;
    const std::vector<WorkflowNode>& getNodes() const { return nodes_; }
    size_t getNodeCount() const { return nodes_.size(); }

    // Connection management
    void addConnection(const NodeConnection& connection);
    void removeConnection(const std::string& from_node, const std::string& to_node);
    const std::vector<NodeConnection>& getConnections() const { return connections_; }
    std::vector<NodeConnection> getConnectionsFrom(const std::string& node_id) const;
    std::vector<NodeConnection> getConnectionsTo(const std::string& node_id) const;

    // Parameter management
    void setDefaultParameter(const std::string& name, const foundation::types::NodeValue& value);
    foundation::types::NodeValue getDefaultParameter(const std::string& name) const;
    const std::map<std::string, foundation::types::NodeValue>& getDefaultParameters() const;

    // Schema management
    void setInputSchema(const std::string& input_name, const std::string& type);
    void setOutputSchema(const std::string& output_name, const std::string& type);
    const std::map<std::string, std::string>& getInputSchema() const { return input_schema_; }
    const std::map<std::string, std::string>& getOutputSchema() const { return output_schema_; }

    // Validation
    std::vector<std::string> validate() const;
    bool isValid() const;
    std::vector<std::string> getExecutionOrder() const;
    bool hasCycles() const;

    // Serialization
    std::string toYAML() const;
    std::string toJSON() const;
    
    // Utility
    void clear();
    bool isEmpty() const;
    std::string toString() const;
    
private:
    // Internal validation helpers
    bool hasCyclesUtil(const std::string& node_id, std::set<std::string>& visited, std::set<std::string>& rec_stack) const;
};

/**
 * Workflow parser for YAML and JSON formats
 */
class WorkflowParser {
private:
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;

public:
    WorkflowParser() = default;

    // Main parsing methods
    std::shared_ptr<WorkflowDefinition> parseYAML(const std::string& yaml_content);
    std::shared_ptr<WorkflowDefinition> parseYAMLFile(const std::string& file_path);
    std::shared_ptr<WorkflowDefinition> parseJSON(const std::string& json_content);
    std::shared_ptr<WorkflowDefinition> parseJSONFile(const std::string& file_path);

    // Error handling
    const std::vector<std::string>& getErrors() const { return errors_; }
    const std::vector<std::string>& getWarnings() const { return warnings_; }
    bool hasErrors() const { return !errors_.empty(); }
    bool hasWarnings() const { return !warnings_.empty(); }
    void clearErrors() { errors_.clear(); warnings_.clear(); }

    // Validation
    bool validateWorkflow(const WorkflowDefinition& workflow);
    std::vector<std::string> validateNodes(const std::vector<WorkflowNode>& nodes);
    std::vector<std::string> validateConnections(const std::vector<NodeConnection>& connections,
                                                 const std::vector<WorkflowNode>& nodes);
    bool validateParameterTypes(const std::map<std::string, foundation::types::NodeValue>& parameters);

    // Utility methods
    static std::vector<std::string> getSupportedFormats();
    static bool isValidNodeId(const std::string& node_id);
    static bool isValidNodeType(const std::string& node_type);

private:
    // Internal parsing helpers
    WorkflowNode parseNodeFromYAML(const foundation::types::NodeValue& node_data);
    NodeConnection parseConnectionFromYAML(const foundation::types::NodeValue& connection_data);
    std::map<std::string, foundation::types::NodeValue> parseParametersFromYAML(
        const foundation::types::NodeValue& params_data);
    
    // Error reporting
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    
    // Validation helpers
    bool validateNodeReference(const std::string& node_id, const std::vector<WorkflowNode>& nodes);
    bool validateParameterSubstitution(const std::string& param_value);
    std::vector<std::string> extractParameterReferences(const std::string& value);
};

/**
 * Workflow execution engine
 */
class WorkflowExecutor {
private:
    std::shared_ptr<akao::engine::orchestrator::NodeRegistry> registry_;
    std::map<std::string, std::function<foundation::types::NodeValue(const foundation::types::NodeValue&)>> 
        transform_functions_;

public:
    explicit WorkflowExecutor(std::shared_ptr<akao::engine::orchestrator::NodeRegistry> registry);

    // Main execution methods
    WorkflowExecutionResult execute(const WorkflowDefinition& workflow);
    WorkflowExecutionResult execute(const WorkflowDefinition& workflow, 
                                  const WorkflowContext& context);
    WorkflowExecutionResult execute(const WorkflowDefinition& workflow,
                                  const std::map<std::string, foundation::types::NodeValue>& inputs);

    // Execution control
    void registerTransformFunction(const std::string& name, 
                                 std::function<foundation::types::NodeValue(const foundation::types::NodeValue&)> func);
    void setExecutionTimeout(std::chrono::milliseconds timeout);
    void setParallelExecutionEnabled(bool enabled);
    void setMaxRetryAttempts(int max_retries);
    void setErrorRecoveryStrategy(const std::string& strategy);

    // Execution monitoring
    struct ExecutionStatus {
        std::string current_node;
        size_t completed_nodes = 0;
        size_t total_nodes = 0;
        bool is_running = false;
        std::chrono::duration<double> elapsed_time{0};
    };
    
    ExecutionStatus getExecutionStatus() const;

private:
    // Internal execution methods
    foundation::types::ExecutionResult executeNode(const WorkflowNode& node, 
                                                  const WorkflowContext& context);
    std::map<std::string, foundation::types::NodeValue> prepareNodeInputs(
        const WorkflowNode& node, const WorkflowContext& context);
    void updateContextWithOutputs(WorkflowContext& context, const std::string& node_id,
                                 const foundation::types::ExecutionResult& result);
    foundation::types::NodeValue applyTransformation(const std::string& transform_expr,
                                                    const foundation::types::NodeValue& input);
    
    // Execution order and dependencies
    std::vector<std::string> calculateExecutionOrder(const WorkflowDefinition& workflow);
    bool canExecuteNode(const WorkflowNode& node, const std::set<std::string>& completed_nodes);
    
    // Parameter substitution
    foundation::types::NodeValue substituteParameters(const foundation::types::NodeValue& value,
                                                     const WorkflowContext& context);
    std::string substituteStringParameters(const std::string& template_str,
                                          const WorkflowContext& context);
    
    // Node registry integration
    std::shared_ptr<foundation::interfaces::INode> getNodeFromRegistry(const std::string& node_id);
    foundation::types::NodeValue createNodeValueFromInputs(
        const std::map<std::string, foundation::types::NodeValue>& inputs);
    foundation::types::ExecutionResult applyOutputTransformation(
        const foundation::types::ExecutionResult& result, const std::string& transform_expr);
    
    // Error handling and recovery
    bool shouldRetryNode(const WorkflowNode& node, int attempt_count);
    void handleNodeError(const std::string& node_id, const std::string& error_message,
                        WorkflowExecutionResult& result);

    // Execution state
    mutable ExecutionStatus current_status_;
    mutable std::mutex status_mutex_;  // THREAD SAFETY FIX: Protect execution status
    std::chrono::milliseconds default_timeout_{30000};
    bool parallel_execution_enabled_{false};
    int max_retry_attempts_{3};
    std::string error_recovery_strategy_{"fail_fast"};
    
    // Enhanced execution methods
    WorkflowExecutionResult executeSequential(const WorkflowDefinition& workflow, WorkflowContext& context);
    WorkflowExecutionResult executeParallel(const WorkflowDefinition& workflow, WorkflowContext& context);
    std::vector<std::vector<std::string>> calculateExecutionLevels(const WorkflowDefinition& workflow);
    bool canExecuteNodeNow(const std::string& node_id, const WorkflowDefinition& workflow, 
                          const std::set<std::string>& completed_nodes);
    
    // Error recovery
    WorkflowExecutionResult handleNodeFailure(const std::string& node_id, 
                                             const foundation::types::ExecutionResult& node_result,
                                             const WorkflowDefinition& workflow,
                                             WorkflowContext& context);
    bool shouldContinueAfterError(const std::string& strategy, const std::string& failed_node_id);
    
    // Data flow enhancements
    void propagateDataBetweenNodes(const std::string& from_node_id, const std::string& to_node_id,
                                  const WorkflowDefinition& workflow, WorkflowContext& context);
    foundation::types::NodeValue resolveNodeInput(const std::string& input_key, const WorkflowNode& node,
                                                  const WorkflowContext& context);
};

/**
 * Workflow validation utilities
 */
namespace workflow_utils {
    bool isValidWorkflowYAML(const std::string& yaml_content);
    std::vector<std::string> extractNodeTypes(const WorkflowDefinition& workflow);
    std::vector<std::string> findUnusedNodes(const WorkflowDefinition& workflow);
    std::vector<std::string> findDisconnectedNodes(const WorkflowDefinition& workflow);
    std::map<std::string, int> calculateNodeDepths(const WorkflowDefinition& workflow);
    bool hasParallelExecution(const WorkflowDefinition& workflow);
    std::string generateWorkflowVisualization(const WorkflowDefinition& workflow);
}

} // namespace orchestrator
} // namespace engine
} // namespace core
} // namespace akao