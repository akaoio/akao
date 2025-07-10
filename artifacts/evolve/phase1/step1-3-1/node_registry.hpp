/**
 * @id akao.evolve.node_registry.v1
 * @doc Central registry system for managing all available nodes with registration, discovery, instantiation, and validation capabilities. Provides thread-safe access to node definitions and instances.
 * @specification Thread-safe registry implementation supporting node registration, type-based lookup, category filtering, version management, and lifecycle management with comprehensive error handling.
 * @scope artifacts/evolve/phase1/step1-3-1
 * @timeline 2025-07-10
 * @rationale Enable centralized node management for discovery, validation, and instantiation while maintaining type safety and supporting dynamic node loading.
 * @methodology Singleton registry pattern with factory functions, thread-safe operations, and comprehensive node lifecycle management.
 * @references akao.evolve.node_definition.v1, akao.evolve.inode.v1
 */

#ifndef AKAO_EVOLVE_NODE_REGISTRY_V1_HPP
#define AKAO_EVOLVE_NODE_REGISTRY_V1_HPP

#include "node_definition.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <mutex>

// Forward declare INode interface
namespace akao {
namespace evolve {
    class INode;
}
}

namespace akao {
namespace evolve {

/**
 * @brief Registration result information
 */
struct RegistrationResult {
    bool success = false;
    std::string error_message;
    std::string node_id;
    
    RegistrationResult() = default;
    RegistrationResult(bool success_val, const std::string& msg = "", const std::string& id = "")
        : success(success_val), error_message(msg), node_id(id) {}
    
    static RegistrationResult createSuccess(const std::string& node_id) {
        return RegistrationResult(true, "", node_id);
    }
    
    static RegistrationResult createFailure(const std::string& error, const std::string& node_id = "") {
        return RegistrationResult(false, error, node_id);
    }
};

/**
 * @brief Node factory function type for creating instances
 */
using NodeFactory = std::function<std::unique_ptr<INode>()>;

/**
 * @brief Registry entry containing definition and factory
 */
struct RegistryEntry {
    NodeDefinition definition;
    NodeFactory factory;
    size_t instance_count = 0;
    bool is_enabled = true;
    
    RegistryEntry() = default;
    RegistryEntry(NodeDefinition def, NodeFactory fact)
        : definition(std::move(def)), factory(std::move(fact)) {}
    
    RegistryEntry(const RegistryEntry& other) = delete;
    RegistryEntry& operator=(const RegistryEntry& other) = delete;
    
    RegistryEntry(RegistryEntry&& other) noexcept = default;
    RegistryEntry& operator=(RegistryEntry&& other) noexcept = default;
};

/**
 * @brief Query filters for node discovery
 */
struct NodeQuery {
    std::optional<NodeCategory> category;
    std::optional<std::string> tag;
    std::optional<std::string> author;
    std::optional<NodeVersion> min_version;
    std::optional<NodeVersion> max_version;
    bool enabled_only = true;
    
    NodeQuery() = default;
    
    static NodeQuery all() {
        NodeQuery query;
        query.enabled_only = false;
        return query;
    }
    
    static NodeQuery byCategory(NodeCategory cat) {
        NodeQuery query;
        query.category = cat;
        return query;
    }
    
    static NodeQuery byTag(const std::string& tag) {
        NodeQuery query;
        query.tag = tag;
        return query;
    }
    
    static NodeQuery byAuthor(const std::string& author) {
        NodeQuery query;
        query.author = author;
        return query;
    }
};

/**
 * @brief Central registry for all available nodes
 */
class NodeRegistry {
private:
    std::map<std::string, std::unique_ptr<RegistryEntry>> entries_;
    std::map<std::string, std::vector<std::string>> category_index_;
    std::map<std::string, std::vector<std::string>> tag_index_;
    std::map<std::string, std::vector<std::string>> author_index_;
    mutable std::mutex registry_mutex_;
    
    // Singleton instance
    static std::unique_ptr<NodeRegistry> instance_;
    static std::mutex instance_mutex_;
    
    // Private constructor for singleton
    NodeRegistry() = default;
    
public:
    // Singleton access
    static NodeRegistry& getInstance();
    
    // Non-copyable, non-movable
    NodeRegistry(const NodeRegistry&) = delete;
    NodeRegistry& operator=(const NodeRegistry&) = delete;
    NodeRegistry(NodeRegistry&&) = delete;
    NodeRegistry& operator=(NodeRegistry&&) = delete;
    
    /**
     * @brief Register a node with factory function
     */
    RegistrationResult registerNode(NodeDefinition definition, NodeFactory factory);
    
    /**
     * @brief Register a node with template-based factory
     */
    template<typename NodeType>
    RegistrationResult registerNode(NodeDefinition definition) {
        static_assert(std::is_base_of_v<INode, NodeType>, 
                      "NodeType must inherit from INode");
        
        NodeFactory factory = []() -> std::unique_ptr<INode> {
            return std::make_unique<NodeType>();
        };
        
        return registerNode(std::move(definition), std::move(factory));
    }
    
    /**
     * @brief Unregister a node by ID
     */
    bool unregisterNode(const std::string& node_id);
    
    /**
     * @brief Check if a node type is registered
     */
    bool hasNode(const std::string& node_type) const;
    
    /**
     * @brief Check if a node ID exists
     */
    bool hasNodeId(const std::string& node_id) const;
    
    /**
     * @brief Get node definition by type
     */
    std::optional<NodeDefinition> getDefinition(const std::string& node_type) const;
    
    /**
     * @brief Get node definition by ID
     */
    std::optional<NodeDefinition> getDefinitionById(const std::string& node_id) const;
    
    /**
     * @brief Create node instance by type
     */
    std::unique_ptr<INode> createNode(const std::string& node_type) const;
    
    /**
     * @brief Create node instance by ID
     */
    std::unique_ptr<INode> createNodeById(const std::string& node_id) const;
    
    /**
     * @brief Get all available node definitions
     */
    std::vector<NodeDefinition> getAvailableNodes() const;
    
    /**
     * @brief Query nodes with filters
     */
    std::vector<NodeDefinition> queryNodes(const NodeQuery& query) const;
    
    /**
     * @brief Get nodes by category
     */
    std::vector<NodeDefinition> getNodesByCategory(NodeCategory category) const;
    
    /**
     * @brief Get nodes by tag
     */
    std::vector<NodeDefinition> getNodesByTag(const std::string& tag) const;
    
    /**
     * @brief Get nodes by author
     */
    std::vector<NodeDefinition> getNodesByAuthor(const std::string& author) const;
    
    /**
     * @brief Enable/disable a node
     */
    bool setNodeEnabled(const std::string& node_id, bool enabled);
    
    /**
     * @brief Check if node is enabled
     */
    bool isNodeEnabled(const std::string& node_id) const;
    
    /**
     * @brief Get registry statistics
     */
    struct RegistryStats {
        size_t total_nodes = 0;
        size_t enabled_nodes = 0;
        size_t total_instances = 0;
        std::map<NodeCategory, size_t> nodes_by_category;
        std::vector<std::string> most_used_nodes;
    };
    
    RegistryStats getStats() const;
    
    /**
     * @brief Validate all registered nodes
     */
    std::vector<std::string> validateRegistry() const;
    
    /**
     * @brief Clear all registrations (for testing)
     */
    void clear();
    
    /**
     * @brief Get all registered node types
     */
    std::vector<std::string> getAllNodeTypes() const;
    
    /**
     * @brief Get all registered node IDs
     */
    std::vector<std::string> getAllNodeIds() const;
    
    /**
     * @brief Get available categories
     */
    std::vector<NodeCategory> getAvailableCategories() const;
    
    /**
     * @brief Get available tags
     */
    std::vector<std::string> getAvailableTags() const;
    
    /**
     * @brief Get available authors
     */
    std::vector<std::string> getAvailableAuthors() const;
    
    /**
     * @brief Export registry to JSON-like structure (for debugging)
     */
    std::string exportToString() const;
    
    /**
     * @brief Validate node compatibility with current Akao version
     */
    bool isNodeCompatible(const std::string& node_id, const NodeVersion& akao_version) const;
    
private:
    /**
     * @brief Update indices when node is registered
     */
    void updateIndices(const std::string& node_id, const NodeDefinition& definition);
    
    /**
     * @brief Remove from indices when node is unregistered
     */
    void removeFromIndices(const std::string& node_id, const NodeDefinition& definition);
    
    /**
     * @brief Check if query matches definition
     */
    bool matchesQuery(const NodeDefinition& definition, const NodeQuery& query) const;
    
    /**
     * @brief Validate node definition
     */
    std::vector<std::string> validateDefinition(const NodeDefinition& definition) const;
};

/**
 * @brief Convenience macros for node registration
 */
#define REGISTER_NODE(NodeClass, definition) \
    NodeRegistry::getInstance().registerNode<NodeClass>(std::move(definition))

#define REGISTER_NODE_WITH_FACTORY(definition, factory) \
    NodeRegistry::getInstance().registerNode(std::move(definition), std::move(factory))

} // namespace evolve
} // namespace akao

#endif // AKAO_EVOLVE_NODE_REGISTRY_V1_HPP