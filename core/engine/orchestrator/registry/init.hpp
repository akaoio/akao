/**
 * @id: akao:class:core:engine:orchestrator:registry:init:v1
 * @doc: Node registry initialization system for registering all built-in nodes with the workflow orchestration system. Provides centralized registration of filesystem scanner, logic executor, YAML processor, reporter, and other essential nodes for the Akao node-based architecture.
 * @specification: Node registry initialization with built-in node registration
 * @scope: Core engine orchestrator registry initialization
 * @timeline: 2025-07-10
 * @rationale: Enable automatic registration of all built-in nodes for workflow execution
 * @methodology: Centralized initialization with factory pattern for node instantiation
 * @references: ["akao:class:core:engine:orchestrator:registry:v1"]
 */

#pragma once

#include "v1.hpp"
#include <memory>

namespace akao {
namespace core {
namespace engine {
namespace orchestrator {

/**
 * Node registry initialization system
 */
class RegistryInitializer {
public:
    /**
     * @id: akao:function:core:engine:orchestrator:registry:init:registerbuiltin:v1
     * @doc: Registers all built-in nodes with the node registry. Creates instances of filesystem scanner, logic executor, YAML processor, and reporter nodes. Returns the number of successfully registered nodes for validation.
     * @specification: Register all built-in nodes in registry
     * @scope: Node registration
     * @timeline: 2025-07-10
     * @rationale: Enable workflow execution with available built-in nodes
     * @methodology: Sequential registration with error handling
     * @references: ["akao:class:nodes:builtin:file:v1", "akao:class:nodes:builtin:logic:v1"]
     */
    static int registerBuiltinNodes();
    
    /**
     * @id: akao:function:core:engine:orchestrator:registry:init:registernode:v1
     * @doc: Safely registers a single node with the registry. Handles errors gracefully and provides logging for registration status. Used internally by registerBuiltinNodes for individual node registration.
     * @specification: Safe node registration with error handling
     * @scope: Single node registration
     * @timeline: 2025-07-10
     * @rationale: Provide safe registration mechanism for individual nodes
     * @methodology: Error handling with status reporting
     * @references: ["akao:interface:core:foundation:interfaces:inode:v1"]
     */
    static bool registerNode(std::shared_ptr<akao::foundation::interfaces::INode> node, const std::string& nodeName);
    
    /**
     * @id: akao:function:core:engine:orchestrator:registry:init:getregistryinfo:v1
     * @doc: Provides summary information about the current registry state. Returns count of registered nodes, available types, and registration status for debugging and validation purposes.
     * @specification: Registry state information retrieval
     * @scope: Registry status reporting
     * @timeline: 2025-07-10
     * @rationale: Enable registry state validation and debugging
     * @methodology: Registry introspection with summary statistics
     * @references: ["akao:class:core:engine:orchestrator:registry:v1"]
     */
    static std::string getRegistryInfo();
    
    /**
     * @id: akao:function:core:engine:orchestrator:registry:init:isinitialized:v1
     * @doc: Checks if the registry has been properly initialized with built-in nodes. Validates that essential nodes like file scanner and logic executor are available for workflow execution.
     * @specification: Registry initialization status check
     * @scope: Registry validation
     * @timeline: 2025-07-10
     * @rationale: Ensure registry is ready for workflow execution
     * @methodology: Essential node availability verification
     * @references: ["akao:class:core:engine:orchestrator:registry:v1"]
     */
    static bool isInitialized();
    
private:
    static bool initialized_;
};

} // namespace orchestrator
} // namespace engine
} // namespace core
} // namespace akao