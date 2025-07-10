/**
 * @id: akao:class:core:engine:orchestrator:registry:init:v1
 * @doc: Implementation of node registry initialization system. Handles registration of all built-in nodes including filesystem scanner, logic executor, YAML processor, and reporter nodes. Provides centralized node registration with comprehensive error handling and status reporting.
 * @specification: Node registry initialization implementation
 * @scope: Core engine orchestrator registry initialization implementation
 * @timeline: 2025-07-10
 * @rationale: Concrete implementation of built-in node registration system
 * @methodology: Sequential node registration with error handling and validation
 * @references: ["akao:class:core:engine:orchestrator:registry:init:v1"]
 */

#include "init.hpp"
#include "nodes/builtin/file/v1.hpp"
#include "nodes/builtin/logic/v1.hpp"
#include "nodes/builtin/yaml/v1.hpp"
#include "nodes/builtin/reporter/v1.hpp"
#include <iostream>
#include <sstream>

namespace akao {
namespace core {
namespace engine {
namespace orchestrator {

bool RegistryInitializer::initialized_ = false;

int RegistryInitializer::registerBuiltinNodes() {
    if (initialized_) {
        return 0; // Already initialized
    }
    
    int registeredCount = 0;
    
    // Register Filesystem Scanner Node
    auto fileScanner = std::make_shared<nodes::builtin::FilesystemScannerNode>();
    if (registerNode(fileScanner, "Filesystem Scanner")) {
        registeredCount++;
    }
    
    // Register Logic Executor Node
    auto logicExecutor = std::make_shared<nodes::builtin::AkaoLogicExecutorNode>();
    if (registerNode(logicExecutor, "Logic Executor")) {
        registeredCount++;
    }
    
    // Register YAML Processor Node
    auto yamlProcessor = std::make_shared<nodes::builtin::YamlProcessorNode>();
    if (registerNode(yamlProcessor, "YAML Processor")) {
        registeredCount++;
    }
    
    // Register Reporter Node
    auto reporter = std::make_shared<nodes::builtin::ReporterNode>();
    if (registerNode(reporter, "Reporter")) {
        registeredCount++;
    }
    
    if (registeredCount > 0) {
        initialized_ = true;
        std::cout << "✓ Registry initialized with " << registeredCount << " built-in nodes\n";
    } else {
        std::cout << "❌ Failed to register any built-in nodes\n";
    }
    
    return registeredCount;
}

bool RegistryInitializer::registerNode(std::shared_ptr<akao::foundation::interfaces::INode> node, const std::string& nodeName) {
    if (!node) {
        std::cout << "❌ Failed to create " << nodeName << " node instance\n";
        return false;
    }
    
    auto& registry = akao::engine::orchestrator::NodeRegistry::getInstance();
    bool success = registry.registerNode(node);
    
    if (success) {
        std::cout << "✓ Registered " << nodeName << " node (ID: " << node->getNodeId() 
                  << ", Type: " << node->getNodeType() << ")\n";
    } else {
        std::cout << "❌ Failed to register " << nodeName << " node (ID: " << node->getNodeId() << ")\n";
    }
    
    return success;
}

std::string RegistryInitializer::getRegistryInfo() {
    auto& registry = akao::engine::orchestrator::NodeRegistry::getInstance();
    
    auto allNodeIds = registry.getAllNodes();
    auto typeStats = registry.getTypeStatistics();
    
    std::ostringstream info;
    info << "Registry Status:\n";
    info << "  Total Nodes: " << allNodeIds.size() << "\n";
    info << "  Node Types: " << typeStats.size() << "\n";
    info << "  Initialized: " << (initialized_ ? "Yes" : "No") << "\n";
    
    if (!allNodeIds.empty()) {
        info << "  Registered Nodes:\n";
        for (const auto& nodeId : allNodeIds) {
            auto nodeInfo = registry.getNodeInfo(nodeId);
            if (nodeInfo) {
                info << "    - " << nodeInfo->getNodeType() << " (" << nodeInfo->getNodeId() << ")\n";
            }
        }
    }
    
    if (!typeStats.empty()) {
        info << "  Available Types: ";
        size_t i = 0;
        for (const auto& [type, count] : typeStats) {
            info << type << "(" << count << ")";
            if (i < typeStats.size() - 1) info << ", ";
            i++;
        }
        info << "\n";
    }
    
    return info.str();
}

bool RegistryInitializer::isInitialized() {
    if (!initialized_) {
        return false;
    }
    
    auto& registry = akao::engine::orchestrator::NodeRegistry::getInstance();
    
    // Check for essential node types (based on actual registered types)
    std::vector<std::string> essentialTypes = {
        "filesystem",          // FilesystemScannerNode
        "logic_executor",      // AkaoLogicExecutorNode
        "yaml_processor",      // YamlProcessorNode
        "reporter"             // ReporterNode
    };
    
    for (const auto& type : essentialTypes) {
        auto nodes = registry.getNodesByType(type);
        if (nodes.empty()) {
            return false;
        }
    }
    
    return true;
}

} // namespace orchestrator
} // namespace engine
} // namespace core
} // namespace akao