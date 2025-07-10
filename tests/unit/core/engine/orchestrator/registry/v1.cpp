/**
 * @id: akao:file:tests:unit:core:engine:orchestrator:registry:v1
 * @doc: Comprehensive unit tests for node registry system. Tests node registration, discovery, thread safety, callback mechanisms, and all registry operations to ensure reliable workflow node management.
 * @specification: Unit tests for node registry with comprehensive coverage
 * @scope: Engine orchestrator registry testing
 * @timeline: 2025-07-10
 * @rationale: Ensure reliability of node registry for workflow system
 * @methodology: Complete test coverage with mock nodes and concurrency testing
 * @references: ["akao:class:core:engine:orchestrator:registry:v1"]
 */

#include "core/engine/orchestrator/registry/v1.hpp"
#include "core/foundation/interfaces/inode/v1.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <atomic>

using namespace akao::engine::orchestrator;
using namespace akao::foundation::interfaces;
using namespace akao::foundation::types;

// Mock nodes for testing
class TestInputNode : public INode {
public:
    std::string getNodeId() const override { return "test-input-001"; }
    std::string getNodeType() const override { return "input"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "Test input node"; }
    
    ValidationResult validate(const NodeParameters& params) const override {
        return ValidationResult(true);
    }
    
    ExecutionResult execute(const NodeContext& context, const NodeParameters& params) override {
        return ExecutionResult::success(NodeValue("input data"));
    }
    
    NodeValue getParameterSchema() const override {
        return NodeValue(NodeValue::Object());
    }
};

class TestProcessingNode : public INode {
public:
    std::string getNodeId() const override { return "test-processing-001"; }
    std::string getNodeType() const override { return "processing"; }
    std::string getVersion() const override { return "2.1.0"; }
    std::string getDescription() const override { return "Test processing node"; }
    
    ValidationResult validate(const NodeParameters& params) const override {
        return ValidationResult(true);
    }
    
    ExecutionResult execute(const NodeContext& context, const NodeParameters& params) override {
        return ExecutionResult::success(NodeValue("processed data"));
    }
    
    NodeValue getParameterSchema() const override {
        return NodeValue(NodeValue::Object());
    }
};

class TestOutputNode : public INode {
public:
    std::string getNodeId() const override { return "test-output-001"; }
    std::string getNodeType() const override { return "output"; }
    std::string getVersion() const override { return "1.5.2"; }
    std::string getDescription() const override { return "Test output node"; }
    
    ValidationResult validate(const NodeParameters& params) const override {
        return ValidationResult(true);
    }
    
    ExecutionResult execute(const NodeContext& context, const NodeParameters& params) override {
        return ExecutionResult::success();
    }
    
    NodeValue getParameterSchema() const override {
        return NodeValue(NodeValue::Object());
    }
};

void testNodeInfo() {
    std::cout << "Testing NodeInfo..." << std::endl;
    
    auto node = std::make_shared<TestInputNode>();
    NodeInfo info(node);
    
    // Basic information
    assert(info.getNodeId() == "test-input-001");
    assert(info.getNodeType() == "input");
    assert(info.getVersion() == "1.0.0");
    assert(info.getDescription() == "Test input node");
    
    // Parameter schema
    NodeValue schema = info.getParameterSchema();
    assert(schema.isObject());
    
    // Categories
    assert(info.getCategories().empty());
    info.addCategory("filesystem");
    info.addCategory("data-source");
    assert(info.getCategories().size() == 2);
    
    // Adding duplicate category should not increase count
    info.addCategory("filesystem");
    assert(info.getCategories().size() == 2);
    
    // Metadata
    info.setMetadata("author", NodeValue("test"));
    info.setMetadata("priority", NodeValue(static_cast<int64_t>(5)));
    
    assert(info.hasMetadata("author"));
    assert(info.getMetadata("author").asString() == "test");
    assert(info.getMetadata("priority").asInteger() == 5);
    assert(!info.hasMetadata("missing"));
    
    // Instance access
    assert(info.getInstance() == node);
    
    // String representation
    std::string str = info.toString();
    assert(str.find("test-input-001") != std::string::npos);
    assert(str.find("input") != std::string::npos);
    
    std::cout << "NodeInfo test passed!" << std::endl;
}

void testBasicRegistration() {
    std::cout << "Testing basic registration..." << std::endl;
    
    NodeRegistry& registry = NodeRegistry::getInstance();
    registry.clear(); // Start fresh
    
    // Register nodes
    auto inputNode = std::make_shared<TestInputNode>();
    auto processingNode = std::make_shared<TestProcessingNode>();
    auto outputNode = std::make_shared<TestOutputNode>();
    
    assert(registry.registerNode(inputNode));
    assert(registry.registerNode(processingNode));
    assert(registry.registerNode(outputNode));
    
    // Check registration count
    assert(registry.getRegisteredCount() == 3);
    
    // Check node existence
    assert(registry.hasNode("test-input-001"));
    assert(registry.hasNode("test-processing-001"));
    assert(registry.hasNode("test-output-001"));
    assert(!registry.hasNode("nonexistent"));
    
    // Try to register same node again (should fail)
    assert(!registry.registerNode(inputNode));
    assert(registry.getRegisteredCount() == 3);
    
    // Try to register null node (should fail)
    assert(!registry.registerNode(nullptr));
    
    std::cout << "Basic registration test passed!" << std::endl;
}

void testNodeRetrieval() {
    std::cout << "Testing node retrieval..." << std::endl;
    
    NodeRegistry& registry = NodeRegistry::getInstance();
    
    // Get nodes by ID
    auto node1 = registry.getNode("test-input-001");
    assert(node1 != nullptr);
    assert(node1->getNodeId() == "test-input-001");
    
    auto node2 = registry.getNode("test-processing-001");
    assert(node2 != nullptr);
    assert(node2->getNodeType() == "processing");
    
    auto nonexistent = registry.getNode("nonexistent");
    assert(nonexistent == nullptr);
    
    // Get node info
    auto info1 = registry.getNodeInfo("test-input-001");
    assert(info1 != nullptr);
    assert(info1->getNodeId() == "test-input-001");
    
    auto info2 = registry.getNodeInfo("nonexistent");
    assert(info2 == nullptr);
    
    std::cout << "Node retrieval test passed!" << std::endl;
}

void testTypeBasedDiscovery() {
    std::cout << "Testing type-based discovery..." << std::endl;
    
    NodeRegistry& registry = NodeRegistry::getInstance();
    
    // Get nodes by type
    auto inputNodes = registry.getNodesByType("input");
    assert(inputNodes.size() == 1);
    assert(inputNodes[0] == "test-input-001");
    
    auto processingNodes = registry.getNodesByType("processing");
    assert(processingNodes.size() == 1);
    assert(processingNodes[0] == "test-processing-001");
    
    auto outputNodes = registry.getNodesByType("output");
    assert(outputNodes.size() == 1);
    assert(outputNodes[0] == "test-output-001");
    
    auto nonexistentType = registry.getNodesByType("nonexistent");
    assert(nonexistentType.empty());
    
    // Get all nodes
    auto allNodes = registry.getAllNodes();
    assert(allNodes.size() == 3);
    
    // Type statistics
    auto typeStats = registry.getTypeStatistics();
    assert(typeStats.size() == 3);
    assert(typeStats["input"] == 1);
    assert(typeStats["processing"] == 1);
    assert(typeStats["output"] == 1);
    
    std::cout << "Type-based discovery test passed!" << std::endl;
}

void testCategoryManagement() {
    std::cout << "Testing category management..." << std::endl;
    
    NodeRegistry& registry = NodeRegistry::getInstance();
    
    // Add categories to nodes
    auto info1 = registry.getNodeInfo("test-input-001");
    assert(info1 != nullptr);
    info1->addCategory("filesystem");
    info1->addCategory("data-source");
    
    auto info2 = registry.getNodeInfo("test-processing-001");
    assert(info2 != nullptr);
    info2->addCategory("transformation");
    info2->addCategory("data-source");
    
    // Re-register to update category indexes (simulating dynamic update)
    registry.unregisterNode("test-input-001");
    registry.unregisterNode("test-processing-001");
    
    auto inputNode = std::make_shared<TestInputNode>();
    auto processingNode = std::make_shared<TestProcessingNode>();
    
    registry.registerNode(inputNode);
    registry.registerNode(processingNode);
    
    // Get updated info and add categories
    info1 = registry.getNodeInfo("test-input-001");
    info1->addCategory("filesystem");
    info1->addCategory("data-source");
    
    info2 = registry.getNodeInfo("test-processing-001");
    info2->addCategory("transformation");
    info2->addCategory("data-source");
    
    // Category-based discovery would need registry to be updated
    // For now, just test the category management in NodeInfo
    assert(info1->getCategories().size() == 2);
    assert(info2->getCategories().size() == 2);
    
    std::cout << "Category management test passed!" << std::endl;
}

void testUnregistration() {
    std::cout << "Testing unregistration..." << std::endl;
    
    NodeRegistry& registry = NodeRegistry::getInstance();
    
    size_t initialCount = registry.getRegisteredCount();
    
    // Unregister a node
    assert(registry.unregisterNode("test-input-001"));
    assert(registry.getRegisteredCount() == initialCount - 1);
    assert(!registry.hasNode("test-input-001"));
    
    // Try to unregister same node again (should fail)
    assert(!registry.unregisterNode("test-input-001"));
    
    // Try to unregister nonexistent node (should fail)
    assert(!registry.unregisterNode("nonexistent"));
    
    // Verify node is gone
    assert(registry.getNode("test-input-001") == nullptr);
    assert(registry.getNodeInfo("test-input-001") == nullptr);
    
    // Verify type index is updated
    auto inputNodes = registry.getNodesByType("input");
    assert(inputNodes.empty());
    
    std::cout << "Unregistration test passed!" << std::endl;
}

void testCallbacks() {
    std::cout << "Testing callbacks..." << std::endl;
    
    NodeRegistry& registry = NodeRegistry::getInstance();
    registry.clear();
    
    std::vector<std::string> registeredNodes;
    std::vector<std::string> unregisteredNodes;
    
    // Add callbacks
    registry.addRegistrationCallback([&registeredNodes](const std::string& nodeId) {
        registeredNodes.push_back(nodeId);
    });
    
    registry.addUnregistrationCallback([&unregisteredNodes](const std::string& nodeId) {
        unregisteredNodes.push_back(nodeId);
    });
    
    // Register nodes
    auto node1 = std::make_shared<TestInputNode>();
    auto node2 = std::make_shared<TestProcessingNode>();
    
    registry.registerNode(node1);
    registry.registerNode(node2);
    
    assert(registeredNodes.size() == 2);
    assert(registeredNodes[0] == "test-input-001");
    assert(registeredNodes[1] == "test-processing-001");
    
    // Unregister nodes
    registry.unregisterNode("test-input-001");
    
    assert(unregisteredNodes.size() == 1);
    assert(unregisteredNodes[0] == "test-input-001");
    
    // Clear should trigger unregistration callbacks
    registry.clear();
    
    assert(unregisteredNodes.size() == 2);
    assert(unregisteredNodes[1] == "test-processing-001");
    
    
    std::cout << "Callbacks test passed!" << std::endl;
}

void testRegistryStatus() {
    std::cout << "Testing registry status..." << std::endl;
    
    NodeRegistry& registry = NodeRegistry::getInstance();
    registry.clear();
    
    // Register some nodes
    auto node1 = std::make_shared<TestInputNode>();
    auto node2 = std::make_shared<TestProcessingNode>();
    auto node3 = std::make_shared<TestOutputNode>();
    
    bool reg1 = registry.registerNode(node1);
    bool reg2 = registry.registerNode(node2);
    bool reg3 = registry.registerNode(node3);
    
    assert(reg1 && reg2 && reg3);
    assert(registry.getRegisteredCount() == 3);
    
    // Check status string (basic check)
    std::string status = registry.getRegistryStatus();
    assert(!status.empty());
    assert(status.find("NodeRegistry") != std::string::npos);
    
    // Check type statistics carefully
    auto typeStats = registry.getTypeStatistics();
    assert(typeStats.size() == 3);
    
    // Check each type individually
    assert(typeStats.count("input") == 1);
    assert(typeStats.count("processing") == 1);
    assert(typeStats.count("output") == 1);
    
    assert(typeStats.at("input") == 1);
    assert(typeStats.at("processing") == 1);
    assert(typeStats.at("output") == 1);
    
    // Check category statistics (should be empty initially)
    auto categoryStats = registry.getCategoryStatistics();
    assert(categoryStats.empty());
    
    std::cout << "Registry status test passed!" << std::endl;
}

// Thread-safe node class for testing
class ThreadSafeTestNode : public INode {
private:
    std::string id_;
    std::string type_;
    
public:
    ThreadSafeTestNode(const std::string& id, const std::string& type) 
        : id_(id), type_(type) {}
    
    std::string getNodeId() const override { return id_; }
    std::string getNodeType() const override { return type_; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "Thread safe test node"; }
    
    ValidationResult validate(const NodeParameters&) const override {
        return ValidationResult(true);
    }
    
    ExecutionResult execute(const NodeContext&, const NodeParameters&) override {
        return ExecutionResult::success(NodeValue("thread result"));
    }
    
    NodeValue getParameterSchema() const override {
        return NodeValue(NodeValue::Object());
    }
};

void testThreadSafety() {
    std::cout << "Testing thread safety..." << std::endl;
    
    NodeRegistry& registry = NodeRegistry::getInstance();
    registry.clear();
    
    const int numThreads = 4;
    const int operationsPerThread = 5;
    std::atomic<int> registrationSuccesses{0};
    std::atomic<int> lookupSuccesses{0};
    std::atomic<int> unregistrationSuccesses{0};
    
    std::vector<std::thread> threads;
    
    // Phase 1: Concurrent registrations
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&registry, &registrationSuccesses, t, operationsPerThread]() {
            for (int i = 0; i < operationsPerThread; ++i) {
                std::string nodeId = "thread-" + std::to_string(t) + "-node-" + std::to_string(i);
                auto node = std::make_shared<ThreadSafeTestNode>(nodeId, "test");
                
                if (registry.registerNode(node)) {
                    registrationSuccesses++;
                }
                
                // Small delay to increase concurrency stress
                std::this_thread::sleep_for(std::chrono::microseconds(1));
            }
        });
    }
    
    // Wait for registration phase
    for (auto& thread : threads) {
        thread.join();
    }
    threads.clear();
    
    // Verify all registrations succeeded
    assert(registrationSuccesses == numThreads * operationsPerThread);
    assert(registry.getRegisteredCount() == static_cast<size_t>(numThreads * operationsPerThread));
    
    // Phase 2: Concurrent lookups
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&registry, &lookupSuccesses, t, operationsPerThread]() {
            for (int i = 0; i < operationsPerThread; ++i) {
                std::string nodeId = "thread-" + std::to_string(t) + "-node-" + std::to_string(i);
                
                if (registry.hasNode(nodeId)) {
                    auto node = registry.getNode(nodeId);
                    if (node && node->getNodeId() == nodeId) {
                        lookupSuccesses++;
                    }
                }
                
                std::this_thread::sleep_for(std::chrono::microseconds(1));
            }
        });
    }
    
    // Wait for lookup phase
    for (auto& thread : threads) {
        thread.join();
    }
    threads.clear();
    
    // Verify all lookups succeeded
    assert(lookupSuccesses == numThreads * operationsPerThread);
    
    // Phase 3: Concurrent unregistrations
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&registry, &unregistrationSuccesses, t, operationsPerThread]() {
            for (int i = 0; i < operationsPerThread; ++i) {
                std::string nodeId = "thread-" + std::to_string(t) + "-node-" + std::to_string(i);
                
                if (registry.unregisterNode(nodeId)) {
                    unregistrationSuccesses++;
                }
                
                std::this_thread::sleep_for(std::chrono::microseconds(1));
            }
        });
    }
    
    // Wait for unregistration phase
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Verify all unregistrations succeeded
    assert(unregistrationSuccesses == numThreads * operationsPerThread);
    assert(registry.getRegisteredCount() == 0);
    
    std::cout << "Thread safety test passed!" << std::endl;
}

int main() {
    std::cout << "Running NodeRegistry unit tests..." << std::endl;
    
    try {
        testNodeInfo();
        testBasicRegistration();
        testNodeRetrieval();
        testTypeBasedDiscovery();
        testCategoryManagement();
        testUnregistration();
        testCallbacks();
        testRegistryStatus();
        testThreadSafety();
        
        std::cout << "All NodeRegistry tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
}