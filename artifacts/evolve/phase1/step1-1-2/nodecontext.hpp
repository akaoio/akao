/**
 * @id: akao:file:core:node:execution:nodecontext:v1
 * @doc: Node execution context and result management system providing batch data processing, parameter access, and seamless integration with Akao Pure Logic Engine for .a format expression execution within node workflows. Enables standardized execution environment for all node types.
 * @specification: Node execution context with batch processing and .a format integration
 * @scope: Core node execution infrastructure
 * @timeline: 2025-07-10
 * @rationale: Provide standardized execution environment for nodes with logic engine integration
 * @methodology: Context management with batch processing and .a format execution capabilities
 * @references: ["akao:class:core:node:execution:nodecontext:v1", "akao:class:core:node:execution:executionresult:v1", "akao:class:core:engine:logic:pure_logic_engine:v1"]
 */

#pragma once

#include "../step1-1-1/nodevalue.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <optional>
#include <chrono>

// Forward declarations for logic engine integration
namespace akao::logic {
    class PureLogicEngine;
    class Context;
    class Value;
}

namespace akao::node {

/**
 * @id: akao:class:core:node:execution:executionstatus:v1
 * @doc: Execution status enumeration for node and workflow execution results providing clear status tracking for success, failure, and partial completion scenarios with detailed state information for debugging and monitoring.
 * @specification: Status enumeration for execution result tracking
 * @scope: Core node execution status management
 * @timeline: 2025-07-10
 * @rationale: Provide clear execution status tracking for nodes and workflows
 * @methodology: Enumeration-based status classification with comprehensive state coverage
 * @references: ["akao:class:core:node:execution:executionresult:v1"]
 */
enum class ExecutionStatus {
    SUCCESS,            // Execution completed successfully
    PARTIAL_SUCCESS,    // Some items processed successfully, others failed
    FAILURE,            // Execution failed completely
    CANCELLED,          // Execution was cancelled
    TIMEOUT,            // Execution timed out
    PENDING             // Execution is still in progress
};

/**
 * @id: akao:class:core:node:execution:nodecontext:v1
 * @doc: Execution context for node operations providing access to input data items, configuration parameters, and Akao Pure Logic Engine for .a format expression execution. Manages batch processing of data items while maintaining parameter access and logic execution capabilities.
 * @specification: Node execution context with batch data and logic engine access
 * @scope: Core node execution environment
 * @timeline: 2025-07-10
 * @rationale: Provide standardized execution environment for all node implementations
 * @methodology: Context object with batch data access and integrated logic engine
 * @references: ["akao:class:core:node:data:nodeitem:v1", "akao:class:core:engine:logic:pure_logic_engine:v1"]
 */
class NodeContext {
public:
    NodeContext();
    explicit NodeContext(const std::vector<NodeItem>& input_items);
    NodeContext(const std::vector<NodeItem>& input_items, 
                const std::map<std::string, NodeValue>& parameters);
    
    // Input data access - N8N-inspired batch processing
    const std::vector<NodeItem>& getInputItems() const { return input_items_; }
    std::vector<NodeItem>& getInputItems() { return input_items_; }
    size_t getInputCount() const { return input_items_.size(); }
    const NodeItem& getInputItem(size_t index) const;
    bool hasInput() const { return !input_items_.empty(); }
    
    // Parameter access with default values and type checking
    NodeValue getParameter(const std::string& name) const;
    NodeValue getParameter(const std::string& name, const NodeValue& default_value) const;
    template<typename T>
    T getParameter(const std::string& name) const;
    template<typename T>
    T getParameter(const std::string& name, const T& default_value) const;
    
    bool hasParameter(const std::string& name) const;
    void setParameter(const std::string& name, const NodeValue& value);
    const std::map<std::string, NodeValue>& getAllParameters() const { return parameters_; }
    
    // Akao Logic Engine integration for .a format execution
    NodeValue executeAkaoLogic(const std::string& akao_expression);
    NodeValue executeAkaoLogic(const std::string& akao_expression, 
                               const std::map<std::string, NodeValue>& variables);
    bool evaluateAkaoCondition(const std::string& akao_condition);
    bool evaluateAkaoCondition(const std::string& akao_condition,
                               const std::map<std::string, NodeValue>& variables);
    
    // Context metadata and tracking
    void setContextId(const std::string& context_id) { context_id_ = context_id; }
    const std::string& getContextId() const { return context_id_; }
    void setNodeId(const std::string& node_id) { node_id_ = node_id; }
    const std::string& getNodeId() const { return node_id_; }
    
    // Execution timing
    void startTiming();
    void stopTiming();
    std::chrono::milliseconds getExecutionTime() const { return execution_time_; }
    
    // Memory tracking
    size_t getMemoryUsage() const;
    
    // Logic engine access (for advanced use cases)
    void setLogicEngine(std::shared_ptr<akao::logic::PureLogicEngine> engine);
    std::shared_ptr<akao::logic::PureLogicEngine> getLogicEngine() const { return logic_engine_; }

private:
    std::vector<NodeItem> input_items_;
    std::map<std::string, NodeValue> parameters_;
    std::shared_ptr<akao::logic::PureLogicEngine> logic_engine_;
    
    // Context metadata
    std::string context_id_;
    std::string node_id_;
    
    // Performance tracking
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time_;
    std::chrono::milliseconds execution_time_{0};
    bool timing_active_ = false;
    
    // Helper methods for logic engine integration (temporarily commented out)
    // These will be implemented when full logic engine integration is available
    /*
    akao::logic::Value nodeValueToLogicValue(const NodeValue& node_value);
    NodeValue logicValueToNodeValue(const akao::logic::Value& logic_value);
    void populateLogicContext(akao::logic::Context& logic_ctx, 
                             const std::map<std::string, NodeValue>& variables);
    */
};

/**
 * @id: akao:class:core:node:execution:executionresult:v1
 * @doc: Node execution result container with batch item processing, error tracking, performance metrics, and comprehensive status information. Supports N8N-style item-based result handling with enhanced error reporting and metadata for workflow debugging and monitoring.
 * @specification: Comprehensive execution result with batch processing and error tracking
 * @scope: Core node execution result management
 * @timeline: 2025-07-10
 * @rationale: Provide comprehensive result tracking for node execution with batch processing support
 * @methodology: Result container with items, errors, and performance metrics
 * @references: ["akao:class:core:node:data:nodeitem:v1", "akao:class:core:node:execution:nodecontext:v1"]
 */
class ExecutionResult {
public:
    ExecutionResult();
    explicit ExecutionResult(ExecutionStatus status);
    ExecutionResult(const std::vector<NodeItem>& items, ExecutionStatus status = ExecutionStatus::SUCCESS);
    
    // Status management
    ExecutionStatus getStatus() const { return status_; }
    void setStatus(ExecutionStatus status) { status_ = status; }
    bool isSuccess() const { return status_ == ExecutionStatus::SUCCESS; }
    bool isPartialSuccess() const { return status_ == ExecutionStatus::PARTIAL_SUCCESS; }
    bool isFailure() const { return status_ == ExecutionStatus::FAILURE; }
    bool hasErrors() const { return !errors_.empty(); }
    
    // Item management - N8N-inspired batch processing
    const std::vector<NodeItem>& getItems() const { return items_; }
    std::vector<NodeItem>& getItems() { return items_; }
    size_t getItemCount() const { return items_.size(); }
    const NodeItem& getItem(size_t index) const;
    
    void addItem(const NodeValue& json_data);
    void addItem(const NodeItem& item);
    void addItem(const NodeValue& json_data, const std::map<std::string, BinaryData>& binary_data);
    void setItems(const std::vector<NodeItem>& items) { items_ = items; }
    void clearItems() { items_.clear(); }
    
    // Error management
    const std::vector<ExecutionError>& getErrors() const { return errors_; }
    void addError(const ExecutionError& error);
    void addError(const std::string& message, 
                  ExecutionError::Severity severity = ExecutionError::Severity::ERROR);
    void addError(const std::string& message, const std::string& code,
                  ExecutionError::Severity severity = ExecutionError::Severity::ERROR);
    void clearErrors() { errors_.clear(); }
    
    // Metadata management
    const std::map<std::string, NodeValue>& getMetadata() const { return metadata_; }
    void setMetadata(const std::string& key, const NodeValue& value) { metadata_[key] = value; }
    NodeValue getMetadata(const std::string& key) const;
    bool hasMetadata(const std::string& key) const { return metadata_.find(key) != metadata_.end(); }
    
    // Performance metrics
    void setExecutionTime(std::chrono::milliseconds time) { execution_time_ = time; }
    std::chrono::milliseconds getExecutionTime() const { return execution_time_; }
    void setMemoryUsage(size_t bytes) { memory_usage_ = bytes; }
    size_t getMemoryUsage() const { return memory_usage_; }
    
    // Processing statistics
    size_t getSuccessfulItemCount() const;
    size_t getFailedItemCount() const;
    double getSuccessRate() const;
    
    // Result combination (for parallel processing)
    void mergeResult(const ExecutionResult& other);
    
    // String representation for debugging
    std::string toString() const;
    std::string getStatusString() const;
    
    // Validation
    bool isEmpty() const { return items_.empty() && errors_.empty(); }
    bool isValid() const;

private:
    ExecutionStatus status_;
    std::vector<NodeItem> items_;
    std::vector<ExecutionError> errors_;
    std::map<std::string, NodeValue> metadata_;
    
    // Performance metrics
    std::chrono::milliseconds execution_time_{0};
    size_t memory_usage_ = 0;
    
    // Helper methods
    void updateStatusBasedOnItems();
};

// Template implementations for NodeContext parameter access
template<typename T>
T NodeContext::getParameter(const std::string& name) const {
    auto it = parameters_.find(name);
    if (it != parameters_.end()) {
        return it->second.as<T>();
    }
    throw std::runtime_error("Parameter not found: " + name);
}

template<typename T>
T NodeContext::getParameter(const std::string& name, const T& default_value) const {
    auto it = parameters_.find(name);
    if (it != parameters_.end()) {
        return it->second.as<T>();
    }
    return default_value;
}

} // namespace akao::node