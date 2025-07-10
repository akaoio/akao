/**
 * @id: akao:file:core:engine:orchestrator:registry:v1
 * @doc: Implementation of node registry system for workflow orchestration. Provides thread-safe node management, discovery operations, and efficient indexing for high-performance workflow execution. Supports dynamic node registration and comprehensive metadata tracking.
 * @specification: Node registry implementation with thread safety and indexing
 * @scope: Core engine orchestrator implementation
 * @timeline: 2025-07-10
 * @rationale: Concrete implementation of node registry for workflow system
 * @methodology: Thread-safe operations with efficient lookup structures
 * @references: ["akao:class:core:engine:orchestrator:registry:v1"]
 */

#include "v1.hpp"
#include <sstream>
#include <algorithm>

namespace akao {
namespace engine {
namespace orchestrator {

// NodeInfo implementation
NodeInfo::NodeInfo(std::shared_ptr<foundation::interfaces::INode> node) 
    : instance_(node) {
    if (node) {
        nodeId_ = node->getNodeId();
        nodeType_ = node->getNodeType();
        version_ = node->getVersion();
        description_ = node->getDescription();
        parameterSchema_ = node->getParameterSchema();
    }
}

const std::string& NodeInfo::getNodeId() const {
    return nodeId_;
}

const std::string& NodeInfo::getNodeType() const {
    return nodeType_;
}

const std::string& NodeInfo::getVersion() const {
    return version_;
}

const std::string& NodeInfo::getDescription() const {
    return description_;
}

const foundation::types::NodeValue& NodeInfo::getParameterSchema() const {
    return parameterSchema_;
}

const std::vector<std::string>& NodeInfo::getCategories() const {
    return categories_;
}

void NodeInfo::addCategory(const std::string& category) {
    if (std::find(categories_.begin(), categories_.end(), category) == categories_.end()) {
        categories_.push_back(category);
    }
}

void NodeInfo::setMetadata(const std::string& key, const foundation::types::NodeValue& value) {
    metadata_[key] = value;
}

foundation::types::NodeValue NodeInfo::getMetadata(const std::string& key) const {
    auto it = metadata_.find(key);
    return (it != metadata_.end()) ? it->second : foundation::types::NodeValue();
}

bool NodeInfo::hasMetadata(const std::string& key) const {
    return metadata_.find(key) != metadata_.end();
}

const std::map<std::string, foundation::types::NodeValue>& NodeInfo::getAllMetadata() const {
    return metadata_;
}

std::shared_ptr<foundation::interfaces::INode> NodeInfo::getInstance() const {
    return instance_;
}

std::string NodeInfo::toString() const {
    // Simple string concatenation to avoid potential stringstream issues
    std::string result = "NodeInfo{id=";
    result += nodeId_;
    result += ", type=";
    result += nodeType_;
    result += ", version=";
    result += version_;
    result += ", categories=";
    result += std::to_string(categories_.size());
    result += ", metadata=";
    result += std::to_string(metadata_.size());
    result += "}";
    return result;
}

// NodeRegistry implementation
NodeRegistry& NodeRegistry::getInstance() {
    static NodeRegistry instance;
    return instance;
}

bool NodeRegistry::registerNode(std::shared_ptr<foundation::interfaces::INode> node) {
    if (!node) {
        return false;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    
    std::string nodeId = node->getNodeId();
    if (nodeId.empty()) {
        return false;
    }

    // Check if already registered
    if (nodes_.find(nodeId) != nodes_.end()) {
        return false;
    }

    // Create node info
    auto nodeInfo = std::make_shared<NodeInfo>(node);
    
    // Register in main map
    nodes_[nodeId] = nodeInfo;
    
    // Update type index
    std::string nodeType = node->getNodeType();
    nodesByType_[nodeType].push_back(nodeId);
    
    // Update category indexes (initially empty, can be added later)
    for (const auto& category : nodeInfo->getCategories()) {
        nodesByCategory_[category].push_back(nodeId);
    }
    
    // Notify callbacks
    for (const auto& callback : registrationCallbacks_) {
        try {
            callback(nodeId);
        } catch (...) {
            // Ignore callback errors
        }
    }
    
    return true;
}

bool NodeRegistry::unregisterNode(const std::string& nodeId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = nodes_.find(nodeId);
    if (it == nodes_.end()) {
        return false;
    }
    
    auto nodeInfo = it->second;
    std::string nodeType = nodeInfo->getNodeType();
    
    // Remove from main map
    nodes_.erase(it);
    
    // Remove from type index
    auto typeIt = nodesByType_.find(nodeType);
    if (typeIt != nodesByType_.end()) {
        auto& typeList = typeIt->second;
        typeList.erase(std::remove(typeList.begin(), typeList.end(), nodeId), typeList.end());
        if (typeList.empty()) {
            nodesByType_.erase(typeIt);
        }
    }
    
    // Remove from category indexes
    for (const auto& category : nodeInfo->getCategories()) {
        auto catIt = nodesByCategory_.find(category);
        if (catIt != nodesByCategory_.end()) {
            auto& catList = catIt->second;
            catList.erase(std::remove(catList.begin(), catList.end(), nodeId), catList.end());
            if (catList.empty()) {
                nodesByCategory_.erase(catIt);
            }
        }
    }
    
    // Notify callbacks
    for (const auto& callback : unregistrationCallbacks_) {
        try {
            callback(nodeId);
        } catch (...) {
            // Ignore callback errors
        }
    }
    
    return true;
}

std::shared_ptr<foundation::interfaces::INode> NodeRegistry::getNode(const std::string& nodeId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = nodes_.find(nodeId);
    return (it != nodes_.end()) ? it->second->getInstance() : nullptr;
}

std::shared_ptr<NodeInfo> NodeRegistry::getNodeInfo(const std::string& nodeId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = nodes_.find(nodeId);
    return (it != nodes_.end()) ? it->second : nullptr;
}

bool NodeRegistry::hasNode(const std::string& nodeId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return nodes_.find(nodeId) != nodes_.end();
}

std::vector<std::string> NodeRegistry::getNodesByType(const std::string& nodeType) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = nodesByType_.find(nodeType);
    return (it != nodesByType_.end()) ? it->second : std::vector<std::string>();
}

std::vector<std::string> NodeRegistry::getNodesByCategory(const std::string& category) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = nodesByCategory_.find(category);
    return (it != nodesByCategory_.end()) ? it->second : std::vector<std::string>();
}

std::vector<std::string> NodeRegistry::getAllNodes() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> result;
    result.reserve(nodes_.size());
    
    for (const auto& pair : nodes_) {
        result.push_back(pair.first);
    }
    
    return result;
}

void NodeRegistry::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Notify unregistration callbacks for all nodes
    for (const auto& pair : nodes_) {
        for (const auto& callback : unregistrationCallbacks_) {
            try {
                callback(pair.first);
            } catch (...) {
                // Ignore callback errors
            }
        }
    }
    
    nodes_.clear();
    nodesByType_.clear();
    nodesByCategory_.clear();
    
    // Clear callbacks to prevent memory corruption in tests
    registrationCallbacks_.clear();
    unregistrationCallbacks_.clear();
}

size_t NodeRegistry::getRegisteredCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return nodes_.size();
}

void NodeRegistry::addRegistrationCallback(std::function<void(const std::string&)> callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    registrationCallbacks_.push_back(callback);
}

void NodeRegistry::addUnregistrationCallback(std::function<void(const std::string&)> callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    unregistrationCallbacks_.push_back(callback);
}

std::map<std::string, size_t> NodeRegistry::getTypeStatistics() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::map<std::string, size_t> stats;
    for (const auto& pair : nodesByType_) {
        stats[pair.first] = pair.second.size();
    }
    return stats;
}

std::map<std::string, size_t> NodeRegistry::getCategoryStatistics() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::map<std::string, size_t> stats;
    for (const auto& pair : nodesByCategory_) {
        stats[pair.first] = pair.second.size();
    }
    return stats;
}

std::string NodeRegistry::getRegistryStatus() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Simple string concatenation to avoid potential stringstream issues
    std::string result = "NodeRegistry{nodes=";
    result += std::to_string(nodes_.size());
    result += ", types=";
    result += std::to_string(nodesByType_.size());
    result += ", categories=";
    result += std::to_string(nodesByCategory_.size());
    result += ", callbacks=";
    result += std::to_string(registrationCallbacks_.size() + unregistrationCallbacks_.size());
    result += "}";
    
    return result;
}

} // namespace orchestrator
} // namespace engine
} // namespace akao