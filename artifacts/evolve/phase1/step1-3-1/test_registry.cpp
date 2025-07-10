/**
 * @id akao.evolve.test_registry.v1
 * @doc Comprehensive test suite for NodeRegistry system including registration, discovery, querying, factory instantiation, and thread-safety validation with real node management scenarios.
 * @specification Executable test program validating all registry operations including singleton pattern, thread safety, node lifecycle management, and comprehensive discovery capabilities.
 * @scope artifacts/evolve/phase1/step1-3-1
 * @timeline 2025-07-10
 * @rationale Ensure NodeRegistry system works correctly with full coverage of registration, discovery, querying, and management functionality.
 * @methodology Unit testing with mock nodes, thread safety validation, and comprehensive registry operation testing.
 * @references akao.evolve.node_registry.v1, akao.evolve.node_definition.v1
 */

#include "node_registry.hpp"
#include "node_definition.hpp"
#include "../step1-2-2/node_parameter.hpp"
#include <iostream>
#include <cassert>
#include <thread>
#include <vector>
#include <chrono>

using namespace akao::evolve;

// Define INode interface for testing (matches the one in node_registry.cpp)
namespace akao {
namespace evolve {
    class INode {
    public:
        virtual ~INode() = default;
        virtual std::string getNodeId() const = 0;
        virtual std::string getNodeType() const = 0;
    };
}
}

// Test result tracking
int tests_passed = 0;
int tests_failed = 0;

#define TEST_ASSERT(condition, message) \
    do { \
        if (condition) { \
            tests_passed++; \
            std::cout << "✅ PASS: " << message << std::endl; \
        } else { \
            tests_failed++; \
            std::cout << "❌ FAIL: " << message << std::endl; \
        } \
    } while(0)

// Mock INode implementation for testing
class MockNode : public INode {
private:
    std::string node_id_;
    std::string node_type_;
    
public:
    MockNode(const std::string& id, const std::string& type) 
        : node_id_(id), node_type_(type) {}
    
    std::string getNodeId() const override { return node_id_; }
    std::string getNodeType() const override { return node_type_; }
};

// Mock node classes for testing
class TestValidatorNode : public INode {
public:
    std::string getNodeId() const override { return "test.validator.v1"; }
    std::string getNodeType() const override { return "test.validator.v1"; }
};

class TestParserNode : public INode {
public:
    std::string getNodeId() const override { return "test.parser.v1"; }
    std::string getNodeType() const override { return "test.parser.v1"; }
};

class TestAnalyzerNode : public INode {
public:
    std::string getNodeId() const override { return "test.analyzer.v1"; }
    std::string getNodeType() const override { return "test.analyzer.v1"; }
};

void test_node_version_functionality() {
    std::cout << "\n=== Testing NodeVersion Functionality ===" << std::endl;
    
    NodeVersion v1(1, 2, 3);
    TEST_ASSERT(v1.toString() == "1.2.3", "Version toString basic");
    
    NodeVersion v2(2, 0, 0, "beta");
    TEST_ASSERT(v2.toString() == "2.0.0-beta", "Version toString with pre-release");
    
    NodeVersion v3(1, 3, 0);
    TEST_ASSERT(v1.isCompatible(v3), "Version compatibility check (same major, higher minor)");
    TEST_ASSERT(!v3.isCompatible(v1), "Version compatibility check (same major, lower minor)");
    
    NodeVersion v4(2, 0, 0);
    TEST_ASSERT(!v1.isCompatible(v4), "Version compatibility check (different major)");
    
    TEST_ASSERT(v1 < v3, "Version comparison operator <");
    TEST_ASSERT(v1 == NodeVersion(1, 2, 3), "Version equality operator");
}

void test_node_capabilities() {
    std::cout << "\n=== Testing NodeCapabilities ===" << std::endl;
    
    auto safe_caps = NodeCapabilities::safe();
    TEST_ASSERT(safe_caps.supports_parallel_execution, "Safe capabilities: parallel execution");
    TEST_ASSERT(!safe_caps.requires_filesystem_access, "Safe capabilities: no filesystem access");
    TEST_ASSERT(!safe_caps.modifies_filesystem, "Safe capabilities: no filesystem modification");
    TEST_ASSERT(safe_caps.is_deterministic, "Safe capabilities: deterministic");
    
    auto fs_caps = NodeCapabilities::filesystem();
    TEST_ASSERT(fs_caps.requires_filesystem_access, "Filesystem capabilities: requires access");
    TEST_ASSERT(!fs_caps.modifies_filesystem, "Filesystem capabilities: no modification by default");
    
    auto akao_caps = NodeCapabilities::akao();
    TEST_ASSERT(akao_caps.requires_akao_engine, "Akao capabilities: requires engine");
    TEST_ASSERT(akao_caps.is_deterministic, "Akao capabilities: deterministic");
}

void test_node_definition_creation() {
    std::cout << "\n=== Testing NodeDefinition Creation ===" << std::endl;
    
    NodeDefinition def("test.node.v1", "test.node.v1", "Test Node");
    TEST_ASSERT(def.node_id == "test.node.v1", "Node definition ID");
    TEST_ASSERT(def.node_type == "test.node.v1", "Node definition type");
    TEST_ASSERT(def.display_name == "Test Node", "Node definition display name");
    TEST_ASSERT(def.isValid(), "Node definition validity");
    
    auto errors = def.getValidationErrors();
    TEST_ASSERT(errors.empty(), "Node definition validation errors");
    
    // Test builder pattern
    def.setCategory(NodeCategory::ANALYSIS)
       .addTag("testing")
       .addTag("analysis")
       .setAuthor("Test Author", "MIT")
       .setVersion(NodeVersion(1, 0, 0))
       .setDocumentation("Test node for unit testing", "example usage");
    
    TEST_ASSERT(def.category == NodeCategory::ANALYSIS, "Node definition category");
    TEST_ASSERT(def.tags.size() == 2, "Node definition tags count");
    TEST_ASSERT(def.tags[0] == "testing", "Node definition first tag");
    TEST_ASSERT(def.author == "Test Author", "Node definition author");
    TEST_ASSERT(def.license == "MIT", "Node definition license");
    TEST_ASSERT(def.version == NodeVersion(1, 0, 0), "Node definition version");
}

void test_node_definition_factory_methods() {
    std::cout << "\n=== Testing NodeDefinition Factory Methods ===" << std::endl;
    
    auto core_node = NodeDefinition::createCoreNode("core.test.v1", "Core Test");
    TEST_ASSERT(core_node.category == NodeCategory::CORE, "Core node factory category");
    TEST_ASSERT(core_node.author == "Akao System", "Core node factory author");
    TEST_ASSERT(!core_node.capabilities.requires_filesystem_access, "Core node capabilities");
    
    auto fs_node = NodeDefinition::createFilesystemNode("fs.test.v1", "Filesystem Test");
    TEST_ASSERT(fs_node.category == NodeCategory::FILESYSTEM, "Filesystem node factory category");
    TEST_ASSERT(fs_node.capabilities.requires_filesystem_access, "Filesystem node capabilities");
    
    auto akao_node = NodeDefinition::createAkaoNode("akao.test.v1", "Akao Test");
    TEST_ASSERT(akao_node.category == NodeCategory::AKAO, "Akao node factory category");
    TEST_ASSERT(akao_node.capabilities.requires_akao_engine, "Akao node capabilities");
    
    auto parser_node = NodeDefinition::createParserNode("parser.test.v1", "Parser Test");
    TEST_ASSERT(parser_node.category == NodeCategory::PARSER, "Parser node factory category");
    
    auto analysis_node = NodeDefinition::createAnalysisNode("analysis.test.v1", "Analysis Test");
    TEST_ASSERT(analysis_node.category == NodeCategory::ANALYSIS, "Analysis node factory category");
    
    auto validation_node = NodeDefinition::createValidationNode("validation.test.v1", "Validation Test");
    TEST_ASSERT(validation_node.category == NodeCategory::VALIDATION, "Validation node factory category");
}

void test_node_definition_parameters() {
    std::cout << "\n=== Testing NodeDefinition Parameters ===" << std::endl;
    
    NodeDefinition def("param.test.v1", "param.test.v1", "Parameter Test");
    TEST_ASSERT(!def.hasParameters(), "Initially no parameters");
    
    auto param_set = std::make_unique<ParameterSet>();
    param_set->addParameter(NodeParameter::createString("input", "Input Text", true));
    param_set->addParameter(NodeParameter::createNumber("threshold", "Threshold", false, 0.5));
    
    def.setParameters(std::move(param_set));
    TEST_ASSERT(def.hasParameters(), "Parameters set");
    
    const auto* params = def.getParameters();
    TEST_ASSERT(params != nullptr, "Parameters accessible");
    TEST_ASSERT(params->size() == 2, "Parameter count");
}

void test_category_string_conversion() {
    std::cout << "\n=== Testing Category String Conversion ===" << std::endl;
    
    TEST_ASSERT(categoryToString(NodeCategory::CORE) == "Core", "Core category to string");
    TEST_ASSERT(categoryToString(NodeCategory::FILESYSTEM) == "Filesystem", "Filesystem category to string");
    TEST_ASSERT(categoryToString(NodeCategory::ANALYSIS) == "Analysis", "Analysis category to string");
    
    auto core_cat = stringToCategory("Core");
    TEST_ASSERT(core_cat.has_value() && core_cat.value() == NodeCategory::CORE, "String to core category");
    
    auto fs_cat = stringToCategory("Filesystem");
    TEST_ASSERT(fs_cat.has_value() && fs_cat.value() == NodeCategory::FILESYSTEM, "String to filesystem category");
    
    auto invalid_cat = stringToCategory("Invalid");
    TEST_ASSERT(!invalid_cat.has_value(), "Invalid string to category");
}

void test_registry_singleton() {
    std::cout << "\n=== Testing Registry Singleton ===" << std::endl;
    
    NodeRegistry& registry1 = NodeRegistry::getInstance();
    NodeRegistry& registry2 = NodeRegistry::getInstance();
    
    TEST_ASSERT(&registry1 == &registry2, "Singleton instance consistency");
}

void test_node_registration() {
    std::cout << "\n=== Testing Node Registration ===" << std::endl;
    
    NodeRegistry& registry = NodeRegistry::getInstance();
    registry.clear(); // Start with clean registry
    
    // Test basic registration
    auto def = NodeDefinition::createCoreNode("test.basic.v1", "Basic Test Node");
    NodeFactory factory = []() -> std::unique_ptr<INode> {
        return std::make_unique<MockNode>("test.basic.v1", "test.basic.v1");
    };
    
    auto result = registry.registerNode(std::move(def), std::move(factory));
    TEST_ASSERT(result.success, "Basic node registration success");
    TEST_ASSERT(result.node_id == "test.basic.v1", "Registration result node ID");
    
    // Test duplicate registration
    auto def2 = NodeDefinition::createCoreNode("test.basic.v1", "Duplicate Test Node");
    NodeFactory factory2 = []() -> std::unique_ptr<INode> {
        return std::make_unique<MockNode>("test.basic.v1", "test.basic.v1");
    };
    
    auto result2 = registry.registerNode(std::move(def2), std::move(factory2));
    TEST_ASSERT(!result2.success, "Duplicate registration fails");
    TEST_ASSERT(result2.error_message.find("already registered") != std::string::npos, 
                "Duplicate registration error message");
    
    // Test invalid definition registration
    NodeDefinition invalid_def("", "", ""); // Empty required fields
    NodeFactory factory3 = []() -> std::unique_ptr<INode> {
        return std::make_unique<MockNode>("", "");
    };
    
    auto result3 = registry.registerNode(std::move(invalid_def), std::move(factory3));
    TEST_ASSERT(!result3.success, "Invalid definition registration fails");
    TEST_ASSERT(result3.error_message.find("Validation failed") != std::string::npos,
                "Invalid definition error message");
}

void test_template_registration() {
    std::cout << "\n=== Testing Template Registration ===" << std::endl;
    
    NodeRegistry& registry = NodeRegistry::getInstance();
    
    // Test template-based registration
    auto def = NodeDefinition::createValidationNode("test.validator.v1", "Test Validator");
    auto result = registry.registerNode<TestValidatorNode>(std::move(def));
    
    TEST_ASSERT(result.success, "Template registration success");
    TEST_ASSERT(registry.hasNodeId("test.validator.v1"), "Template registered node exists");
}

void test_node_lookup() {
    std::cout << "\n=== Testing Node Lookup ===" << std::endl;
    
    NodeRegistry& registry = NodeRegistry::getInstance();
    
    // Test existence checks
    TEST_ASSERT(registry.hasNode("test.basic.v1"), "Node type exists");
    TEST_ASSERT(registry.hasNodeId("test.basic.v1"), "Node ID exists");
    TEST_ASSERT(!registry.hasNode("nonexistent.node"), "Nonexistent node type");
    TEST_ASSERT(!registry.hasNodeId("nonexistent.id"), "Nonexistent node ID");
    
    // Test definition retrieval
    auto def = registry.getDefinition("test.basic.v1");
    TEST_ASSERT(def.has_value(), "Definition retrieval by type");
    TEST_ASSERT(def->display_name == "Basic Test Node", "Retrieved definition content");
    
    auto def_by_id = registry.getDefinitionById("test.basic.v1");
    TEST_ASSERT(def_by_id.has_value(), "Definition retrieval by ID");
    TEST_ASSERT(def_by_id->display_name == "Basic Test Node", "Retrieved definition by ID content");
    
    // Test nonexistent definition retrieval
    auto missing_def = registry.getDefinition("missing.node");
    TEST_ASSERT(!missing_def.has_value(), "Missing definition returns nullopt");
}

void test_node_instantiation() {
    std::cout << "\n=== Testing Node Instantiation ===" << std::endl;
    
    NodeRegistry& registry = NodeRegistry::getInstance();
    
    // Test node creation by type
    auto node = registry.createNode("test.basic.v1");
    TEST_ASSERT(node != nullptr, "Node creation by type");
    TEST_ASSERT(node->getNodeId() == "test.basic.v1", "Created node ID");
    TEST_ASSERT(node->getNodeType() == "test.basic.v1", "Created node type");
    
    // Test node creation by ID
    auto node_by_id = registry.createNodeById("test.validator.v1");
    TEST_ASSERT(node_by_id != nullptr, "Node creation by ID");
    TEST_ASSERT(node_by_id->getNodeId() == "test.validator.v1", "Created node by ID");
    
    // Test creation of nonexistent node
    auto missing_node = registry.createNode("missing.node");
    TEST_ASSERT(missing_node == nullptr, "Missing node creation returns nullptr");
}

void test_registry_discovery() {
    std::cout << "\n=== Testing Registry Discovery ===" << std::endl;
    
    NodeRegistry& registry = NodeRegistry::getInstance();
    
    // Register additional nodes for discovery testing
    auto parser_def = NodeDefinition::createParserNode("test.parser.v1", "Test Parser")
        .addTag("parser").addTag("testing");
    registry.registerNode<TestParserNode>(std::move(parser_def));
    
    auto analyzer_def = NodeDefinition::createAnalysisNode("test.analyzer.v1", "Test Analyzer")
        .addTag("analysis").addTag("testing").setAuthor("Test Team");
    registry.registerNode<TestAnalyzerNode>(std::move(analyzer_def));
    
    // Test get all nodes
    auto all_nodes = registry.getAvailableNodes();
    TEST_ASSERT(all_nodes.size() >= 4, "All available nodes count"); // At least our registered nodes
    
    // Test category filtering
    auto core_nodes = registry.getNodesByCategory(NodeCategory::CORE);
    TEST_ASSERT(core_nodes.size() >= 1, "Core nodes found");
    
    auto validation_nodes = registry.getNodesByCategory(NodeCategory::VALIDATION);
    TEST_ASSERT(validation_nodes.size() >= 1, "Validation nodes found");
    
    // Test tag filtering
    auto testing_nodes = registry.getNodesByTag("testing");
    TEST_ASSERT(testing_nodes.size() >= 2, "Testing tagged nodes");
    
    auto parser_nodes = registry.getNodesByTag("parser");
    TEST_ASSERT(parser_nodes.size() >= 1, "Parser tagged nodes");
    
    // Test author filtering
    auto test_team_nodes = registry.getNodesByAuthor("Test Team");
    TEST_ASSERT(test_team_nodes.size() >= 1, "Test Team authored nodes");
    
    auto akao_nodes = registry.getNodesByAuthor("Akao System");
    TEST_ASSERT(akao_nodes.size() >= 1, "Akao System authored nodes");
}

void test_node_queries() {
    std::cout << "\n=== Testing Node Queries ===" << std::endl;
    
    NodeRegistry& registry = NodeRegistry::getInstance();
    
    // Test category query
    auto category_query = NodeQuery::byCategory(NodeCategory::ANALYSIS);
    auto analysis_results = registry.queryNodes(category_query);
    TEST_ASSERT(analysis_results.size() >= 1, "Category query results");
    
    // Test tag query
    auto tag_query = NodeQuery::byTag("testing");
    auto tag_results = registry.queryNodes(tag_query);
    TEST_ASSERT(tag_results.size() >= 2, "Tag query results");
    
    // Test author query
    auto author_query = NodeQuery::byAuthor("Akao System");
    auto author_results = registry.queryNodes(author_query);
    TEST_ASSERT(author_results.size() >= 1, "Author query results");
    
    // Test version query
    NodeQuery version_query;
    version_query.min_version = NodeVersion(0, 9, 0);
    version_query.max_version = NodeVersion(2, 0, 0);
    auto version_results = registry.queryNodes(version_query);
    TEST_ASSERT(version_results.size() >= 1, "Version query results");
    
    // Test complex query
    NodeQuery complex_query;
    complex_query.category = NodeCategory::ANALYSIS;
    complex_query.tag = "testing";
    auto complex_results = registry.queryNodes(complex_query);
    TEST_ASSERT(complex_results.size() >= 1, "Complex query results");
}

void test_node_enable_disable() {
    std::cout << "\n=== Testing Node Enable/Disable ===" << std::endl;
    
    NodeRegistry& registry = NodeRegistry::getInstance();
    
    // Test initial enabled state
    TEST_ASSERT(registry.isNodeEnabled("test.basic.v1"), "Node initially enabled");
    
    // Test disabling node
    bool disable_result = registry.setNodeEnabled("test.basic.v1", false);
    TEST_ASSERT(disable_result, "Node disable operation success");
    TEST_ASSERT(!registry.isNodeEnabled("test.basic.v1"), "Node disabled state");
    
    // Test that disabled node is not found in enabled-only queries
    auto enabled_nodes = registry.getAvailableNodes();
    bool found_disabled = false;
    for (const auto& node : enabled_nodes) {
        if (node.node_id == "test.basic.v1") {
            found_disabled = true;
            break;
        }
    }
    TEST_ASSERT(!found_disabled, "Disabled node not in enabled-only results");
    
    // Test re-enabling node
    bool enable_result = registry.setNodeEnabled("test.basic.v1", true);
    TEST_ASSERT(enable_result, "Node enable operation success");
    TEST_ASSERT(registry.isNodeEnabled("test.basic.v1"), "Node re-enabled state");
    
    // Test operation on nonexistent node
    bool invalid_result = registry.setNodeEnabled("nonexistent.node", false);
    TEST_ASSERT(!invalid_result, "Invalid node enable/disable fails");
}

void test_registry_statistics() {
    std::cout << "\n=== Testing Registry Statistics ===" << std::endl;
    
    NodeRegistry& registry = NodeRegistry::getInstance();
    
    auto stats = registry.getStats();
    TEST_ASSERT(stats.total_nodes >= 4, "Total nodes count in stats");
    TEST_ASSERT(stats.enabled_nodes >= 3, "Enabled nodes count in stats");
    TEST_ASSERT(stats.nodes_by_category.size() >= 3, "Categories in stats");
    
    // Check specific category counts
    TEST_ASSERT(stats.nodes_by_category.at(NodeCategory::CORE) >= 1, "Core nodes in stats");
    TEST_ASSERT(stats.nodes_by_category.at(NodeCategory::VALIDATION) >= 1, "Validation nodes in stats");
    
    // Test instance counting (from previous creations)
    TEST_ASSERT(stats.total_instances >= 2, "Instance count in stats");
}

void test_registry_validation() {
    std::cout << "\n=== Testing Registry Validation ===" << std::endl;
    
    NodeRegistry& registry = NodeRegistry::getInstance();
    
    auto validation_errors = registry.validateRegistry();
    TEST_ASSERT(validation_errors.empty(), "Registry validation passes");
    
    // Test compatibility checking
    NodeVersion current_akao(1, 0, 0);
    bool compatible = registry.isNodeCompatible("test.basic.v1", current_akao);
    TEST_ASSERT(compatible, "Node compatibility check");
    
    NodeVersion future_akao(2, 0, 0);
    // This should still be compatible since our test nodes don't set specific version requirements
    bool future_compatible = registry.isNodeCompatible("test.basic.v1", future_akao);
    TEST_ASSERT(future_compatible, "Future version compatibility");
}

void test_registry_utility_functions() {
    std::cout << "\n=== Testing Registry Utility Functions ===" << std::endl;
    
    NodeRegistry& registry = NodeRegistry::getInstance();
    
    // Test getting all types and IDs
    auto all_types = registry.getAllNodeTypes();
    TEST_ASSERT(all_types.size() >= 3, "All node types count");
    
    auto all_ids = registry.getAllNodeIds();
    TEST_ASSERT(all_ids.size() >= 3, "All node IDs count");
    
    // Test getting available categories, tags, authors
    auto categories = registry.getAvailableCategories();
    TEST_ASSERT(categories.size() >= 3, "Available categories count");
    
    auto tags = registry.getAvailableTags();
    TEST_ASSERT(tags.size() >= 2, "Available tags count");
    
    auto authors = registry.getAvailableAuthors();
    TEST_ASSERT(authors.size() >= 2, "Available authors count");
    
    // Test export to string
    std::string export_str = registry.exportToString();
    TEST_ASSERT(!export_str.empty(), "Registry export not empty");
    TEST_ASSERT(export_str.find("NodeRegistry Export") == 0, "Registry export format");
    TEST_ASSERT(export_str.find("test.basic.v1") != std::string::npos, "Registry export contains nodes");
}

void test_node_unregistration() {
    std::cout << "\n=== Testing Node Unregistration ===" << std::endl;
    
    NodeRegistry& registry = NodeRegistry::getInstance();
    
    // Count nodes before unregistration
    size_t initial_count = registry.getAvailableNodes().size();
    
    // Test unregistration
    bool unregister_result = registry.unregisterNode("test.analyzer.v1");
    TEST_ASSERT(unregister_result, "Node unregistration success");
    
    // Verify node is gone
    TEST_ASSERT(!registry.hasNodeId("test.analyzer.v1"), "Unregistered node not found");
    
    size_t final_count = registry.getAvailableNodes().size();
    TEST_ASSERT(final_count == initial_count - 1, "Node count decreased after unregistration");
    
    // Test unregistration of nonexistent node
    bool invalid_unregister = registry.unregisterNode("nonexistent.node");
    TEST_ASSERT(!invalid_unregister, "Nonexistent node unregistration fails");
}

void test_thread_safety() {
    std::cout << "\n=== Testing Thread Safety ===" << std::endl;
    
    NodeRegistry& registry = NodeRegistry::getInstance();
    
    const int num_threads = 4;
    const int operations_per_thread = 100;
    std::vector<std::thread> threads;
    std::atomic<int> successful_operations(0);
    
    // Test concurrent registrations and lookups
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&registry, &successful_operations, i, operations_per_thread]() {
            for (int j = 0; j < operations_per_thread; ++j) {
                // Alternate between different operations
                if (j % 3 == 0) {
                    // Registration test
                    std::string node_id = "thread.test." + std::to_string(i) + "." + std::to_string(j);
                    auto def = NodeDefinition::createCoreNode(node_id, "Thread Test " + std::to_string(i));
                    NodeFactory factory = [node_id]() -> std::unique_ptr<INode> {
                        return std::make_unique<MockNode>(node_id, node_id);
                    };
                    
                    auto result = registry.registerNode(std::move(def), std::move(factory));
                    if (result.success) {
                        successful_operations++;
                    }
                } else if (j % 3 == 1) {
                    // Lookup test
                    auto nodes = registry.getAvailableNodes();
                    if (!nodes.empty()) {
                        successful_operations++;
                    }
                } else {
                    // Query test
                    auto core_nodes = registry.getNodesByCategory(NodeCategory::CORE);
                    if (!core_nodes.empty()) {
                        successful_operations++;
                    }
                }
                
                // Small delay to increase chance of race conditions
                std::this_thread::sleep_for(std::chrono::microseconds(1));
            }
        });
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    int expected_min_operations = num_threads * operations_per_thread * 0.5; // At least 50% should succeed
    TEST_ASSERT(successful_operations >= expected_min_operations, 
                "Thread safety test: " + std::to_string(successful_operations.load()) + 
                " successful operations out of " + std::to_string(num_threads * operations_per_thread));
}

void test_performance() {
    std::cout << "\n=== Testing Performance ===" << std::endl;
    
    NodeRegistry& registry = NodeRegistry::getInstance();
    
    const int lookup_iterations = 1000;
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Perform many lookup operations
    for (int i = 0; i < lookup_iterations; ++i) {
        auto nodes = registry.getAvailableNodes();
        auto core_nodes = registry.getNodesByCategory(NodeCategory::CORE);
        auto test_node = registry.getDefinition("test.basic.v1");
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    TEST_ASSERT(duration.count() < 1000, // Should complete in less than 1 second
                "Performance test: " + std::to_string(lookup_iterations) + 
                " lookups completed in " + std::to_string(duration.count()) + "ms");
}

int main() {
    std::cout << "🚀 Starting NodeRegistry System Tests" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    test_node_version_functionality();
    test_node_capabilities();
    test_node_definition_creation();
    test_node_definition_factory_methods();
    test_node_definition_parameters();
    test_category_string_conversion();
    test_registry_singleton();
    test_node_registration();
    test_template_registration();
    test_node_lookup();
    test_node_instantiation();
    test_registry_discovery();
    test_node_queries();
    test_node_enable_disable();
    test_registry_statistics();
    test_registry_validation();
    test_registry_utility_functions();
    test_node_unregistration();
    test_thread_safety();
    test_performance();
    
    std::cout << "\n=====================================" << std::endl;
    std::cout << "📊 Test Results Summary:" << std::endl;
    std::cout << "✅ Tests Passed: " << tests_passed << std::endl;
    std::cout << "❌ Tests Failed: " << tests_failed << std::endl;
    std::cout << "📈 Success Rate: " << (tests_passed * 100 / (tests_passed + tests_failed)) << "%" << std::endl;
    
    if (tests_failed == 0) {
        std::cout << "\n🎉 ALL TESTS PASSED! NodeRegistry system is working correctly." << std::endl;
        std::cout << "✅ Singleton pattern implementation verified" << std::endl;
        std::cout << "✅ Node registration and unregistration works" << std::endl;
        std::cout << "✅ Discovery and querying functionality complete" << std::endl;
        std::cout << "✅ Thread safety validated" << std::endl;
        std::cout << "✅ Performance requirements met" << std::endl;
        return 0;
    } else {
        std::cout << "\n❌ SOME TESTS FAILED! Please review the implementation." << std::endl;
        return 1;
    }
}