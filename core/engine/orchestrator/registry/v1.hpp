/**
 * @id: akao:file:core:engine:orchestrator:registry:v1
 * @doc: Node registry system for workflow orchestration. Provides centralized registration, discovery, and management of workflow nodes across the entire ecosystem. Supports runtime node registration, type-based queries, and thread-safe operations for concurrent workflow execution.
 * @specification: Node registry with discovery and management capabilities
 * @scope: Core engine orchestrator subsystem
 * @timeline: 2025-07-10
 * @rationale: Enable dynamic node ecosystem with runtime registration and discovery
 * @methodology: Registry pattern with thread-safe operations and metadata indexing
 * @references: ["akao:class:core:engine:orchestrator:registry:v1"]
 */

#pragma once

#include "../../../foundation/interfaces/inode/v1.hpp"
#include "../../../foundation/types/value/v1.hpp"
#include "../../../foundation/types/result/v1.hpp"
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <mutex>
#include <functional>

namespace akao {
namespace engine {
namespace orchestrator {

/**
 * @id: akao:class:core:engine:orchestrator:registry:nodeinfo:v1
 * @doc: Metadata container for registered workflow nodes. Stores node identification, classification, capabilities, and runtime information. Used by the registry to track and categorize nodes for efficient discovery and management.
 * @specification: Node metadata container with identification and capabilities
 * @scope: Node registry system
 * @timeline: 2025-07-10
 * @rationale: Provide comprehensive node metadata for registry operations
 * @methodology: Metadata container with versioning and capability tracking
 * @references: ["akao:class:core:engine:orchestrator:registry:v1"]
 */
class NodeInfo {
private:
    std::string nodeId_;
    std::string nodeType_;
    std::string version_;
    std::string description_;
    foundation::types::NodeValue parameterSchema_;
    std::vector<std::string> categories_;
    std::map<std::string, foundation::types::NodeValue> metadata_;
    std::shared_ptr<foundation::interfaces::INode> instance_;

public:
    // Constructor
    NodeInfo(std::shared_ptr<foundation::interfaces::INode> node);

    // Basic information
    const std::string& getNodeId() const;
    const std::string& getNodeType() const;
    const std::string& getVersion() const;
    const std::string& getDescription() const;

    // Schema and capabilities
    const foundation::types::NodeValue& getParameterSchema() const;
    const std::vector<std::string>& getCategories() const;
    void addCategory(const std::string& category);

    // Metadata
    void setMetadata(const std::string& key, const foundation::types::NodeValue& value);
    foundation::types::NodeValue getMetadata(const std::string& key) const;
    bool hasMetadata(const std::string& key) const;
    const std::map<std::string, foundation::types::NodeValue>& getAllMetadata() const;

    // Node instance access
    std::shared_ptr<foundation::interfaces::INode> getInstance() const;

    // String representation
    std::string toString() const;
};

/**
 * @id: akao:class:core:engine:orchestrator:registry:noderegistry:v1
 * @doc: Central registry for workflow node management and discovery. Provides thread-safe registration, lookup, and categorization of nodes across the entire workflow ecosystem. Supports dynamic node loading, type-based queries, and runtime node management for flexible workflow execution.
 * @specification: Thread-safe node registry with discovery and management
 * @scope: Core engine orchestrator
 * @timeline: 2025-07-10
 * @rationale: Enable dynamic node ecosystem with centralized management
 * @methodology: Registry pattern with thread safety and efficient lookup structures
 * @references: ["akao:class:core:engine:orchestrator:registry:nodeinfo:v1"]
 */
class NodeRegistry {
private:
    mutable std::mutex mutex_;
    std::map<std::string, std::shared_ptr<NodeInfo>> nodes_;
    std::map<std::string, std::vector<std::string>> nodesByType_;
    std::map<std::string, std::vector<std::string>> nodesByCategory_;
    std::vector<std::function<void(const std::string&)>> registrationCallbacks_;
    std::vector<std::function<void(const std::string&)>> unregistrationCallbacks_;

    // Private constructor for singleton
    NodeRegistry() = default;

public:
    // Singleton access
    static NodeRegistry& getInstance();
    
    // Disable copy and move
    NodeRegistry(const NodeRegistry&) = delete;
    NodeRegistry& operator=(const NodeRegistry&) = delete;
    NodeRegistry(NodeRegistry&&) = delete;
    NodeRegistry& operator=(NodeRegistry&&) = delete;

    /**
     * @id: akao:function:core:engine:orchestrator:registry:registernode:v1
     * @doc: Registers a new workflow node in the system registry. Validates node compliance, extracts metadata, and makes the node available for workflow discovery and execution. Thread-safe operation that updates all internal indexes.
     * @specification: Register node instance in registry with validation
     * @scope: Node registration
     * @timeline: 2025-07-10
     * @rationale: Enable dynamic addition of nodes to workflow system
     * @methodology: Validation, metadata extraction, and indexed storage
     * @references: ["akao:class:core:engine:orchestrator:registry:nodeinfo:v1"]
     */
    bool registerNode(std::shared_ptr<foundation::interfaces::INode> node);

    /**
     * @id: akao:function:core:engine:orchestrator:registry:unregisternode:v1
     * @doc: Removes a workflow node from the system registry. Cleans up all internal indexes and notifies callbacks. Thread-safe operation that ensures complete node removal from the system.
     * @specification: Remove node from registry and clean up indexes
     * @scope: Node unregistration
     * @timeline: 2025-07-10
     * @rationale: Enable dynamic removal of nodes from workflow system
     * @methodology: Index cleanup and callback notification
     * @references: ["akao:class:core:engine:orchestrator:registry:nodeinfo:v1"]
     */
    bool unregisterNode(const std::string& nodeId);

    /**
     * @id: akao:function:core:engine:orchestrator:registry:getnode:v1
     * @doc: Retrieves a registered node instance by its unique identifier. Returns the node instance for workflow execution. Thread-safe lookup operation with efficient index access.
     * @specification: Retrieve node instance by unique identifier
     * @scope: Node retrieval
     * @timeline: 2025-07-10
     * @rationale: Enable workflow access to registered nodes for execution
     * @methodology: Thread-safe indexed lookup
     * @references: ["akao:class:core:engine:orchestrator:registry:nodeinfo:v1"]
     */
    std::shared_ptr<foundation::interfaces::INode> getNode(const std::string& nodeId) const;

    /**
     * @id: akao:function:core:engine:orchestrator:registry:getnodeinfo:v1
     * @doc: Retrieves comprehensive metadata information for a registered node. Returns detailed node information including capabilities, schema, and classification. Used for node discovery and workflow planning.
     * @specification: Retrieve comprehensive node metadata by identifier
     * @scope: Node information retrieval
     * @timeline: 2025-07-10
     * @rationale: Enable detailed node inspection for workflow planning
     * @methodology: Thread-safe metadata lookup
     * @references: ["akao:class:core:engine:orchestrator:registry:nodeinfo:v1"]
     */
    std::shared_ptr<NodeInfo> getNodeInfo(const std::string& nodeId) const;

    /**
     * @id: akao:function:core:engine:orchestrator:registry:hasnode:v1
     * @doc: Checks if a node with the specified identifier is registered in the system. Fast lookup operation for node existence verification. Thread-safe operation with minimal overhead.
     * @specification: Check node existence by unique identifier
     * @scope: Node existence checking
     * @timeline: 2025-07-10
     * @rationale: Enable fast node existence verification for workflows
     * @methodology: Thread-safe existence check
     * @references: ["akao:class:core:engine:orchestrator:registry:v1"]
     */
    bool hasNode(const std::string& nodeId) const;

    /**
     * @id: akao:function:core:engine:orchestrator:registry:getnodesbytype:v1
     * @doc: Retrieves all registered nodes of a specific type classification. Returns list of node identifiers matching the specified type. Used for type-based node discovery and workflow composition.
     * @specification: Retrieve nodes by type classification
     * @scope: Type-based node discovery
     * @timeline: 2025-07-10
     * @rationale: Enable type-based node discovery for workflow composition
     * @methodology: Thread-safe type index lookup
     * @references: ["akao:class:core:engine:orchestrator:registry:v1"]
     */
    std::vector<std::string> getNodesByType(const std::string& nodeType) const;

    /**
     * @id: akao:function:core:engine:orchestrator:registry:getnodesbycategory:v1
     * @doc: Retrieves all registered nodes belonging to a specific category. Returns list of node identifiers for category-based filtering. Used for capability-based node discovery and specialized workflows.
     * @specification: Retrieve nodes by category classification
     * @scope: Category-based node discovery
     * @timeline: 2025-07-10
     * @rationale: Enable category-based node discovery for specialized workflows
     * @methodology: Thread-safe category index lookup
     * @references: ["akao:class:core:engine:orchestrator:registry:v1"]
     */
    std::vector<std::string> getNodesByCategory(const std::string& category) const;

    /**
     * @id: akao:function:core:engine:orchestrator:registry:getallnodes:v1
     * @doc: Retrieves identifiers of all registered nodes in the system. Returns complete list of available nodes for comprehensive discovery. Used for system inspection and complete node enumeration.
     * @specification: Retrieve all registered node identifiers
     * @scope: Complete node enumeration
     * @timeline: 2025-07-10
     * @rationale: Enable complete system inspection and node enumeration
     * @methodology: Thread-safe complete node list generation
     * @references: ["akao:class:core:engine:orchestrator:registry:v1"]
     */
    std::vector<std::string> getAllNodes() const;

    /**
     * @id: akao:function:core:engine:orchestrator:registry:clear:v1
     * @doc: Removes all registered nodes from the system registry. Performs complete system reset with callback notifications. Used for testing, system reinitialization, and cleanup operations.
     * @specification: Clear all registered nodes and reset registry
     * @scope: Registry management
     * @timeline: 2025-07-10
     * @rationale: Enable complete system reset and cleanup
     * @methodology: Thread-safe complete registry clearing
     * @references: ["akao:class:core:engine:orchestrator:registry:v1"]
     */
    void clear();

    /**
     * @id: akao:function:core:engine:orchestrator:registry:getregisteredcount:v1
     * @doc: Returns the total number of nodes currently registered in the system. Provides quick system status information for monitoring and debugging. Thread-safe operation with minimal overhead.
     * @specification: Return count of registered nodes
     * @scope: Registry status
     * @timeline: 2025-07-10
     * @rationale: Enable registry status monitoring and debugging
     * @methodology: Thread-safe count operation
     * @references: ["akao:class:core:engine:orchestrator:registry:v1"]
     */
    size_t getRegisteredCount() const;

    // Callback management
    void addRegistrationCallback(std::function<void(const std::string&)> callback);
    void addUnregistrationCallback(std::function<void(const std::string&)> callback);

    // Statistics and debugging
    std::map<std::string, size_t> getTypeStatistics() const;
    std::map<std::string, size_t> getCategoryStatistics() const;
    std::string getRegistryStatus() const;
};

} // namespace orchestrator
} // namespace engine
} // namespace akao