/**
 * @id: akao:file:core:foundation:interfaces:inode:v1
 * @doc: Standard interface for all workflow nodes in the Akao ecosystem. Defines the contract that all nodes must implement, including execution methods, validation capabilities, and metadata access. Enables a unified approach to node management and execution across different languages and implementations.
 * @specification: Standard node interface with execution and validation methods
 * @scope: Core foundation interfaces subsystem
 * @timeline: 2025-07-10
 * @rationale: Establish standard contract for heterogeneous node ecosystem
 * @methodology: Abstract interface with context-based execution model
 * @references: ["akao:class:core:foundation:interfaces:inode:v1"]
 */

#pragma once

#include "../../types/value/v1.hpp"
#include "../../types/result/v1.hpp"
#include <string>
#include <map>
#include <memory>
#include <vector>

namespace akao {
namespace foundation {
namespace interfaces {

// Forward declarations
class NodeContext;
class NodeParameters;
class ValidationResult;

/**
 * @id: akao:class:core:foundation:interfaces:nodecontext:v1
 * @doc: Execution context container for workflow nodes. Provides access to input data, workflow metadata, execution environment, and inter-node communication facilities. Used to pass runtime information and resources to nodes during execution.
 * @specification: Node execution context with input data and environment
 * @scope: Core foundation interfaces
 * @timeline: 2025-07-10
 * @rationale: Provide execution context and resources to workflow nodes
 * @methodology: Context object with input data and environment access
 * @references: ["akao:class:core:foundation:interfaces:inode:v1"]
 */
class NodeContext {
private:
    types::NodeValue inputs_;
    std::map<std::string, types::NodeValue> environment_;
    std::string workflowId_;
    std::string executionId_;
    std::string nodeId_;

public:
    // Constructor
    NodeContext(const std::string& nodeId, const types::NodeValue& inputs = types::NodeValue());

    // Input data access
    const types::NodeValue& getInputs() const;
    void setInputs(const types::NodeValue& inputs);

    // Environment access
    types::NodeValue getEnvironment(const std::string& key) const;
    void setEnvironment(const std::string& key, const types::NodeValue& value);
    bool hasEnvironment(const std::string& key) const;
    const std::map<std::string, types::NodeValue>& getAllEnvironment() const;

    // Workflow identification
    const std::string& getWorkflowId() const;
    void setWorkflowId(const std::string& workflowId);
    
    const std::string& getExecutionId() const;
    void setExecutionId(const std::string& executionId);
    
    const std::string& getNodeId() const;
    void setNodeId(const std::string& nodeId);

    // Convenience methods
    types::NodeValue getInput(const std::string& key) const;
    bool hasInput(const std::string& key) const;
};

/**
 * @id: akao:class:core:foundation:interfaces:nodeparameters:v1
 * @doc: Configuration parameters container for workflow nodes. Stores node-specific configuration, runtime parameters, and validation rules. Used to configure node behavior and validate parameter correctness before execution.
 * @specification: Node configuration parameters with validation support
 * @scope: Core foundation interfaces
 * @timeline: 2025-07-10
 * @rationale: Provide type-safe node configuration and parameter validation
 * @methodology: Parameter container with validation rules and type checking
 * @references: ["akao:class:core:foundation:interfaces:inode:v1"]
 */
class NodeParameters {
private:
    std::map<std::string, types::NodeValue> parameters_;
    std::map<std::string, std::string> parameterTypes_;
    std::map<std::string, bool> requiredParameters_;

public:
    // Parameter access
    void setParameter(const std::string& key, const types::NodeValue& value);
    types::NodeValue getParameter(const std::string& key) const;
    bool hasParameter(const std::string& key) const;
    const std::map<std::string, types::NodeValue>& getAllParameters() const;

    // Parameter types and validation
    void setParameterType(const std::string& key, const std::string& type);
    std::string getParameterType(const std::string& key) const;
    void setParameterRequired(const std::string& key, bool required = true);
    bool isParameterRequired(const std::string& key) const;

    // Validation
    std::vector<std::string> validate() const;
    bool isValid() const;

    // Convenience methods
    template<typename T>
    T getParameterAs(const std::string& key) const;
};

/**
 * @id: akao:class:core:foundation:interfaces:validationresult:v1
 * @doc: Result container for node parameter validation. Tracks validation status, collects validation errors, and provides detailed feedback for parameter configuration issues. Used by nodes to report configuration problems before execution.
 * @specification: Validation result with error tracking and status reporting
 * @scope: Core foundation interfaces
 * @timeline: 2025-07-10
 * @rationale: Provide detailed validation feedback for node configuration
 * @methodology: Result container with error collection and status tracking
 * @references: ["akao:class:core:foundation:interfaces:inode:v1"]
 */
class ValidationResult {
private:
    bool valid_;
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;

public:
    // Constructor
    ValidationResult(bool valid = true);

    // Status
    bool isValid() const;
    void setValid(bool valid);

    // Errors
    void addError(const std::string& error);
    const std::vector<std::string>& getErrors() const;
    bool hasErrors() const;

    // Warnings
    void addWarning(const std::string& warning);
    const std::vector<std::string>& getWarnings() const;
    bool hasWarnings() const;

    // Combination
    void merge(const ValidationResult& other);
    
    // String representation
    std::string toString() const;
};

/**
 * @id: akao:class:core:foundation:interfaces:inode:v1
 * @doc: Abstract base interface for all workflow nodes in the Akao system. Defines the standard contract that every node must implement, including unique identification, parameter validation, and execution methods. Enables polymorphic node management and consistent execution patterns across the entire workflow ecosystem.
 * @specification: Abstract interface for workflow nodes with execution contract
 * @scope: Core foundation interfaces
 * @timeline: 2025-07-10
 * @rationale: Standard contract for heterogeneous node ecosystem
 * @methodology: Pure virtual interface with execution and validation methods
 * @references: ["akao:class:core:foundation:interfaces:nodecontext:v1"]
 */
class INode {
public:
    virtual ~INode() = default;

    /**
     * @id: akao:function:core:foundation:interfaces:getnodeid:v1
     * @doc: Returns the unique identifier for this node instance. Used by the workflow engine to identify and reference nodes during execution, debugging, and result tracking.
     * @specification: Return unique node identifier string
     * @scope: Node identification
     * @timeline: 2025-07-10
     * @rationale: Enable node identification and reference in workflow system
     * @methodology: Return constant string identifier
     * @references: ["akao:class:core:foundation:interfaces:inode:v1"]
     */
    virtual std::string getNodeId() const = 0;

    /**
     * @id: akao:function:core:foundation:interfaces:getnodetype:v1
     * @doc: Returns the type classification of this node (e.g., 'input', 'processing', 'output'). Used by the workflow engine for node categorization, visualization, and execution planning.
     * @specification: Return node type classification string
     * @scope: Node classification
     * @timeline: 2025-07-10
     * @rationale: Enable node categorization and workflow planning
     * @methodology: Return constant type string
     * @references: ["akao:class:core:foundation:interfaces:inode:v1"]
     */
    virtual std::string getNodeType() const = 0;

    /**
     * @id: akao:function:core:foundation:interfaces:getversion:v1
     * @doc: Returns the version string of this node implementation. Used for compatibility checking, debugging, and ensuring correct node versions in workflows.
     * @specification: Return node implementation version string
     * @scope: Node versioning
     * @timeline: 2025-07-10
     * @rationale: Enable version tracking and compatibility management
     * @methodology: Return semantic version string
     * @references: ["akao:class:core:foundation:interfaces:inode:v1"]
     */
    virtual std::string getVersion() const = 0;

    /**
     * @id: akao:function:core:foundation:interfaces:validate:v1
     * @doc: Validates the provided parameters for this node before execution. Checks parameter completeness, type correctness, and constraint compliance. Returns detailed validation results with error descriptions.
     * @specification: Validate node parameters and return validation result
     * @scope: Parameter validation
     * @timeline: 2025-07-10
     * @rationale: Ensure parameter correctness before node execution
     * @methodology: Parameter checking with detailed error reporting
     * @references: ["akao:class:core:foundation:interfaces:nodeparameters:v1"]
     */
    virtual ValidationResult validate(const NodeParameters& params) const = 0;

    /**
     * @id: akao:function:core:foundation:interfaces:execute:v1
     * @doc: Executes the node's primary function with the provided context and parameters. Performs the actual work of the node and returns execution results including output data, status, and any errors encountered.
     * @specification: Execute node logic and return execution result
     * @scope: Node execution
     * @timeline: 2025-07-10
     * @rationale: Perform primary node function with context awareness
     * @methodology: Context-based execution with comprehensive result tracking
     * @references: ["akao:class:core:foundation:interfaces:nodecontext:v1"]
     */
    virtual types::ExecutionResult execute(const NodeContext& context, const NodeParameters& params) = 0;

    /**
     * @id: akao:function:core:foundation:interfaces:getdescription:v1
     * @doc: Returns a human-readable description of what this node does. Used for documentation generation, user interfaces, and workflow visualization.
     * @specification: Return human-readable node description
     * @scope: Node documentation
     * @timeline: 2025-07-10
     * @rationale: Provide user-friendly node documentation
     * @methodology: Return descriptive text string
     * @references: ["akao:class:core:foundation:interfaces:inode:v1"]
     */
    virtual std::string getDescription() const = 0;

    /**
     * @id: akao:function:core:foundation:interfaces:getparameterschema:v1
     * @doc: Returns the parameter schema definition for this node. Describes expected parameters, their types, default values, and validation rules. Used for UI generation and parameter validation.
     * @specification: Return parameter schema definition
     * @scope: Parameter schema
     * @timeline: 2025-07-10
     * @rationale: Enable automatic parameter validation and UI generation
     * @methodology: Return structured parameter definitions
     * @references: ["akao:class:core:foundation:interfaces:nodeparameters:v1"]
     */
    virtual types::NodeValue getParameterSchema() const = 0;
};

} // namespace interfaces
} // namespace foundation
} // namespace akao