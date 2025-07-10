/**
 * @id: akao:class:core:engine:orchestrator:registry:registrar:v1
 * @doc: Node self-registration utility enabling automatic node registration through static initialization. Provides RAII-based registration pattern for built-in and external nodes. Eliminates hardcoded node dependencies and enables extensible plugin architecture.
 * @specification: Self-registering node utility with automatic initialization
 * @scope: Core engine orchestrator registry self-registration
 * @timeline: 2025-07-10
 * @rationale: Enable extensible node architecture without hardcoded dependencies
 * @methodology: Static initialization with RAII pattern for automatic registration
 * @references: ["akao:class:core:engine:orchestrator:registry:v1"]
 */

#pragma once

#include "v1.hpp"
#include <memory>
#include <functional>
#include <string>

namespace akao {
namespace core {
namespace engine {
namespace orchestrator {

/**
 * @id: akao:class:core:engine:orchestrator:registry:registrar:noderegistrar:v1
 * @doc: RAII utility class for automatic node registration. Creates and registers node instances during static initialization. Provides error handling and registration status reporting for robust plugin loading.
 * @specification: RAII node registration with automatic initialization
 * @scope: Node self-registration utility
 * @timeline: 2025-07-10
 * @rationale: Enable automatic node registration without manual intervention
 * @methodology: RAII pattern with constructor-based registration
 * @references: ["akao:interface:core:foundation:interfaces:inode:v1"]
 */
template<typename NodeType>
class NodeRegistrar {
public:
    /**
     * @id: akao:function:core:engine:orchestrator:registry:registrar:constructor:v1
     * @doc: Automatic node registration constructor. Creates node instance and registers it with the global registry. Handles registration errors gracefully and provides optional registration verification.
     * @specification: Constructor-based automatic node registration
     * @scope: Node registration
     * @timeline: 2025-07-10
     * @rationale: Enable automatic registration during static initialization
     * @methodology: Constructor creates and registers node instance
     * @references: ["akao:class:core:engine:orchestrator:registry:v1"]
     */
    NodeRegistrar() {
        try {
            auto node = std::make_shared<NodeType>();
            if (node) {
                auto& registry = akao::engine::orchestrator::NodeRegistry::getInstance();
                bool success = registry.registerNode(node);
                
                if (success) {
                    registrationSuccessful_ = true;
                    nodeId_ = node->getNodeId();
                    nodeType_ = node->getNodeType();
                } else {
                    // Registration failed - node might already be registered
                    registrationSuccessful_ = false;
                }
            }
        } catch (const std::exception& e) {
            // Node creation or registration failed
            registrationSuccessful_ = false;
            errorMessage_ = e.what();
        }
    }
    
    /**
     * @id: akao:function:core:engine:orchestrator:registry:registrar:wassuccessful:v1
     * @doc: Returns whether the node registration was successful. Used for verification and debugging of the registration process. Enables detection of registration failures during startup.
     * @specification: Registration success status check
     * @scope: Registration verification
     * @timeline: 2025-07-10
     * @rationale: Enable verification of successful node registration
     * @methodology: Boolean status return with error information
     * @references: ["akao:class:core:engine:orchestrator:registry:registrar:v1"]
     */
    bool wasSuccessful() const { return registrationSuccessful_; }
    
    /**
     * @id: akao:function:core:engine:orchestrator:registry:registrar:getnodeid:v1
     * @doc: Returns the registered node's unique identifier. Available only after successful registration. Used for verification and logging of registered nodes.
     * @specification: Registered node ID retrieval
     * @scope: Node identification
     * @timeline: 2025-07-10
     * @rationale: Enable identification of successfully registered nodes
     * @methodology: String ID return after successful registration
     * @references: ["akao:interface:core:foundation:interfaces:inode:v1"]
     */
    const std::string& getNodeId() const { return nodeId_; }
    
    /**
     * @id: akao:function:core:engine:orchestrator:registry:registrar:getnodetype:v1
     * @doc: Returns the registered node's type classification. Available only after successful registration. Used for verification and categorization of registered nodes.
     * @specification: Registered node type retrieval
     * @scope: Node type identification
     * @timeline: 2025-07-10
     * @rationale: Enable type-based verification of registered nodes
     * @methodology: String type return after successful registration
     * @references: ["akao:interface:core:foundation:interfaces:inode:v1"]
     */
    const std::string& getNodeType() const { return nodeType_; }
    
    /**
     * @id: akao:function:core:engine:orchestrator:registry:registrar:geterrormessage:v1
     * @doc: Returns error message if registration failed. Provides detailed error information for debugging registration failures during static initialization.
     * @specification: Registration error message retrieval
     * @scope: Error reporting
     * @timeline: 2025-07-10
     * @rationale: Enable debugging of registration failures
     * @methodology: String error message return for failed registrations
     * @references: ["akao:class:core:engine:orchestrator:registry:registrar:v1"]
     */
    const std::string& getErrorMessage() const { return errorMessage_; }

private:
    bool registrationSuccessful_ = false;
    std::string nodeId_;
    std::string nodeType_;
    std::string errorMessage_;
};

/**
 * @id: akao:macro:core:engine:orchestrator:registry:registrar:registernode:v1
 * @doc: Convenience macro for automatic node registration. Creates static NodeRegistrar instance that automatically registers the specified node type during program initialization. Simplifies node registration to a single line.
 * @specification: Macro for simplified node registration
 * @scope: Node registration convenience
 * @timeline: 2025-07-10
 * @rationale: Simplify node registration to single macro call
 * @methodology: Static instance creation with automatic registration
 * @references: ["akao:class:core:engine:orchestrator:registry:registrar:noderegistrar:v1"]
 */
#define REGISTER_NODE(NodeType) \
    namespace { \
        static ::akao::core::engine::orchestrator::NodeRegistrar<NodeType> \
            g_node_registrar_instance; \
    }

/**
 * @id: akao:macro:core:engine:orchestrator:registry:registrar:registernamednode:v1
 * @doc: Named convenience macro for automatic node registration. Like REGISTER_NODE but allows custom variable naming to avoid conflicts. Useful when registering multiple variants of the same node type.
 * @specification: Named macro for node registration with custom identifier
 * @scope: Node registration convenience with naming
 * @timeline: 2025-07-10
 * @rationale: Enable conflict-free registration of multiple node variants
 * @methodology: Named static instance creation with automatic registration
 * @references: ["akao:class:core:engine:orchestrator:registry:registrar:noderegistrar:v1"]
 */
#define REGISTER_NAMED_NODE(NodeType, Name) \
    namespace { \
        static ::akao::core::engine::orchestrator::NodeRegistrar<NodeType> \
            g_node_registrar_##Name; \
    }

} // namespace orchestrator
} // namespace engine
} // namespace core
} // namespace akao