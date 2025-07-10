/**
 * @id akao.evolve.test_builtin_nodes.v1
 * @doc Comprehensive test suite for all builtin core nodes including LoggerNode, DataMapperNode, ConditionalNode, AkaoLogicNode, and FileReaderNode with real data processing and validation.
 * @specification Executable test program validating all core node functionality including parameter validation, execution logic, error handling, and integration with the node system.
 * @scope artifacts/evolve/phase1/step1-3-2
 * @timeline 2025-07-10
 * @rationale Ensure all builtin nodes work correctly with comprehensive coverage of node functionality, parameter handling, and execution scenarios.
 * @methodology Unit testing with real data processing, file I/O testing, and comprehensive validation of all node types and their parameters.
 * @references akao.evolve.builtin_nodes.v1, akao.evolve.inode.v1
 */

#include "builtin_nodes.hpp"
#include "inode.hpp"
#include <iostream>
#include <cassert>
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>

using namespace akao::evolve;
using namespace akao::evolve::builtin;

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

void test_parameter_value_integration() {
    std::cout << "\n=== Testing ParameterValue Integration ===" << std::endl;
    
    // Test NodeItem creation and data access
    NodeItem item(ParameterValue("test data"));
    TEST_ASSERT(item.json.toString() == "test data", "NodeItem data access");
    
    item.addMetadata("key1", ParameterValue("value1"));
    auto meta = item.getMetadata("key1");
    TEST_ASSERT(meta.has_value() && meta->toString() == "value1", "NodeItem metadata access");
    
    // Test binary data
    BinaryData binary({0x48, 0x65, 0x6C, 0x6C, 0x6F}, "text/plain", "hello.txt");
    item.addBinary("attachment", binary);
    TEST_ASSERT(item.hasBinary("attachment"), "NodeItem binary attachment");
    TEST_ASSERT(binary.size() == 5, "Binary data size");
    
    // Test error handling
    item.setError("Test error", "TEST_CODE");
    TEST_ASSERT(item.hasError(), "NodeItem error setting");
}

void test_execution_result() {
    std::cout << "\n=== Testing ExecutionResult ===" << std::endl;
    
    ExecutionResult result;
    TEST_ASSERT(result.isSuccess(), "Initial execution result success");
    TEST_ASSERT(result.items.empty(), "Initial items empty");
    TEST_ASSERT(result.errors.empty(), "Initial errors empty");
    
    result.addItem(ParameterValue("item1"));
    result.addItem(ParameterValue("item2"));
    TEST_ASSERT(result.items.size() == 2, "Added items count");
    
    result.addError("Test error");
    TEST_ASSERT(result.errors.size() == 1, "Added error count");
    TEST_ASSERT(!result.isSuccess(), "Result not success after error");
    
    result.markComplete();
    TEST_ASSERT(result.status == ExecutionStatus::PARTIAL, "Partial status with items and errors");
    
    std::string summary = result.getSummary();
    TEST_ASSERT(summary.find("Items: 2") != std::string::npos, "Summary contains item count");
    TEST_ASSERT(summary.find("Errors: 1") != std::string::npos, "Summary contains error count");
}

void test_node_context() {
    std::cout << "\n=== Testing NodeContext ===" << std::endl;
    
    NodeContext context;
    TEST_ASSERT(context.getInputCount() == 0, "Initial context empty");
    TEST_ASSERT(!context.hasInput(), "Initial context has no input");
    
    context.addInputItem(ParameterValue("input1"));
    context.addInputItem(ParameterValue("input2"));
    TEST_ASSERT(context.getInputCount() == 2, "Context input count");
    TEST_ASSERT(context.hasInput(), "Context has input");
    
    context.setParameter("param1", ParameterValue("value1"));
    TEST_ASSERT(context.hasParameter("param1"), "Context has parameter");
    TEST_ASSERT(context.getParameter("param1").toString() == "value1", "Context parameter value");
    
    // Test Akao logic execution (mock)
    auto logic_result = context.executeAkaoLogic("input.count");
    TEST_ASSERT(logic_result.as<int64_t>() == 2, "Mock Akao logic execution");
}

void test_logger_node() {
    std::cout << "\n=== Testing LoggerNode ===" << std::endl;
    
    LoggerNode logger;
    TEST_ASSERT(logger.getNodeId() == "system.logger.v1", "Logger node ID");
    TEST_ASSERT(logger.getNodeType() == "system.logger.v1", "Logger node type");
    
    auto definition = logger.getDefinition();
    TEST_ASSERT(definition.display_name == "Logger Node", "Logger definition name");
    TEST_ASSERT(definition.hasParameters(), "Logger has parameters");
    
    // Test parameter validation
    std::map<std::string, ParameterValue> valid_params = {
        {"level", ParameterValue("info")},
        {"prefix", ParameterValue("TEST")},
        {"timestamp", ParameterValue(true)}
    };
    
    auto validation = logger.validate(valid_params);
    TEST_ASSERT(validation.valid, "Logger valid parameters");
    
    std::map<std::string, ParameterValue> invalid_params = {
        {"level", ParameterValue("invalid_level")}
    };
    
    validation = logger.validate(invalid_params);
    TEST_ASSERT(!validation.valid, "Logger invalid parameters");
    
    // Test execution
    NodeContext context;
    context.addInputItem(ParameterValue("Log message 1"));
    context.addInputItem(ParameterValue("Log message 2"));
    context.setParameter("level", ParameterValue("info"));
    context.setParameter("prefix", ParameterValue("TEST"));
    context.setParameter("timestamp", ParameterValue(false));
    
    auto result = logger.execute(context);
    TEST_ASSERT(result.isSuccess(), "Logger execution success");
    TEST_ASSERT(result.items.size() == 2, "Logger pass-through items");
}

void test_data_mapper_node() {
    std::cout << "\n=== Testing DataMapperNode ===" << std::endl;
    
    DataMapperNode mapper;
    TEST_ASSERT(mapper.getNodeId() == "data.mapper.v1", "Mapper node ID");
    
    // Test uppercase operation
    NodeContext context;
    context.addInputItem(ParameterValue("hello world"));
    context.addInputItem(ParameterValue("test data"));
    context.setParameter("operation", ParameterValue("uppercase"));
    
    auto result = mapper.execute(context);
    TEST_ASSERT(result.isSuccess(), "Mapper uppercase execution success");
    TEST_ASSERT(result.items.size() == 2, "Mapper output item count");
    TEST_ASSERT(result.items[0].json.toString() == "HELLO WORLD", "Mapper uppercase transformation");
    TEST_ASSERT(result.items[1].json.toString() == "TEST DATA", "Mapper uppercase transformation 2");
    
    // Test length operation
    context = NodeContext();
    context.addInputItem(ParameterValue("hello"));
    context.setParameter("operation", ParameterValue("length"));
    
    result = mapper.execute(context);
    TEST_ASSERT(result.isSuccess(), "Mapper length execution success");
    TEST_ASSERT(result.items[0].json.toString() == "5", "Mapper length transformation");
    
    // Test custom operation with prefix/suffix
    context = NodeContext();
    context.addInputItem(ParameterValue("test"));
    context.setParameter("operation", ParameterValue("custom"));
    context.setParameter("custom_prefix", ParameterValue("PREFIX_"));
    context.setParameter("custom_suffix", ParameterValue("_SUFFIX"));
    context.setParameter("preserve_original", ParameterValue(true));
    
    result = mapper.execute(context);
    TEST_ASSERT(result.isSuccess(), "Mapper custom execution success");
    TEST_ASSERT(result.items[0].json.toString() == "PREFIX_test_SUFFIX", "Mapper custom transformation");
    
    auto original_meta = result.items[0].getMetadata("original");
    TEST_ASSERT(original_meta.has_value() && original_meta->toString() == "test", "Mapper preserve original");
    
    // Test invalid operation
    context = NodeContext();
    context.addInputItem(ParameterValue("test"));
    context.setParameter("operation", ParameterValue("invalid"));
    
    result = mapper.execute(context);
    TEST_ASSERT(!result.isSuccess(), "Mapper invalid operation fails");
    TEST_ASSERT(!result.errors.empty(), "Mapper invalid operation has errors");
}

void test_conditional_node() {
    std::cout << "\n=== Testing ConditionalNode ===" << std::endl;
    
    ConditionalNode conditional;
    TEST_ASSERT(conditional.getNodeId() == "control.conditional.v1", "Conditional node ID");
    
    // Test filter mode with length condition
    NodeContext context;
    context.addInputItem(ParameterValue("short"));
    context.addInputItem(ParameterValue("this is a longer string"));
    context.addInputItem(ParameterValue("mid"));
    context.setParameter("condition", ParameterValue("length > 10"));
    context.setParameter("mode", ParameterValue("filter"));
    
    auto result = conditional.execute(context);
    TEST_ASSERT(result.isSuccess(), "Conditional filter execution success");
    TEST_ASSERT(result.items.size() == 1, "Conditional filter result count");
    TEST_ASSERT(result.items[0].json.toString() == "this is a longer string", "Conditional filter result");
    
    // Test split mode
    context = NodeContext();
    context.addInputItem(ParameterValue("hello"));
    context.addInputItem(ParameterValue("world"));
    context.setParameter("condition", ParameterValue("contains('o')"));
    context.setParameter("mode", ParameterValue("split"));
    
    result = conditional.execute(context);
    TEST_ASSERT(result.isSuccess(), "Conditional split execution success");
    TEST_ASSERT(result.items.size() == 2, "Conditional split result count");
    
    auto branch1 = result.items[0].getMetadata("branch");
    auto branch2 = result.items[1].getMetadata("branch");
    TEST_ASSERT(branch1.has_value() && branch1->toString() == "true", "Conditional split branch 1");
    TEST_ASSERT(branch2.has_value() && branch2->toString() == "true", "Conditional split branch 2");
    
    // Test count mode
    context = NodeContext();
    context.addInputItem(ParameterValue("test1"));
    context.addInputItem(ParameterValue("test2"));
    context.addInputItem(ParameterValue(""));
    context.setParameter("condition", ParameterValue("true"));
    context.setParameter("mode", ParameterValue("count"));
    
    result = conditional.execute(context);
    TEST_ASSERT(result.isSuccess(), "Conditional count execution success");
    TEST_ASSERT(result.items.size() == 1, "Conditional count result item");
    
    auto passed_count = result.items[0].getMetadata("passed_count");
    TEST_ASSERT(passed_count.has_value() && passed_count->as<int64_t>() == 3, "Conditional count passed");
}

void test_akao_logic_node() {
    std::cout << "\n=== Testing AkaoLogicNode ===" << std::endl;
    
    AkaoLogicNode akao_logic;
    TEST_ASSERT(akao_logic.getNodeId() == "akao.logic_executor.v1", "AkaoLogic node ID");
    
    // Test basic expression execution
    NodeContext context;
    context.addInputItem(ParameterValue("input1"));
    context.addInputItem(ParameterValue("input2"));
    context.setParameter("expression", ParameterValue("input.count"));
    context.setParameter("result_format", ParameterValue("value"));
    
    auto result = akao_logic.execute(context);
    TEST_ASSERT(result.isSuccess(), "AkaoLogic execution success");
    TEST_ASSERT(result.items.size() == 1, "AkaoLogic result item count");
    TEST_ASSERT(result.items[0].json.as<int64_t>() == 2, "AkaoLogic input count result");
    
    // Test boolean format
    context = NodeContext();
    context.setParameter("expression", ParameterValue("true"));
    context.setParameter("result_format", ParameterValue("boolean"));
    
    result = akao_logic.execute(context);
    TEST_ASSERT(result.isSuccess(), "AkaoLogic boolean execution success");
    TEST_ASSERT(result.items[0].json.as<bool>() == true, "AkaoLogic boolean result");
    
    // Test detailed format
    context = NodeContext();
    context.addInputItem(ParameterValue("test"));
    context.setParameter("expression", ParameterValue("test_expr"));
    context.setParameter("result_format", ParameterValue("detailed"));
    
    result = akao_logic.execute(context);
    TEST_ASSERT(result.isSuccess(), "AkaoLogic detailed execution success");
    
    auto expr_meta = result.items[0].getMetadata("expression");
    auto input_count_meta = result.items[0].getMetadata("input_count");
    TEST_ASSERT(expr_meta.has_value() && expr_meta->toString() == "test_expr", "AkaoLogic detailed expression metadata");
    TEST_ASSERT(input_count_meta.has_value() && input_count_meta->as<int64_t>() == 1, "AkaoLogic detailed input count metadata");
}

void test_file_reader_node() {
    std::cout << "\n=== Testing FileReaderNode ===" << std::endl;
    
    FileReaderNode file_reader;
    TEST_ASSERT(file_reader.getNodeId() == "io.file_reader.v1", "FileReader node ID");
    
    // Create test files
    std::string test_file1 = "test_file1.txt";
    std::string test_file2 = "test_file2.txt";
    
    std::ofstream out1(test_file1);
    out1 << "Line 1\nLine 2\nLine 3";
    out1.close();
    
    std::ofstream out2(test_file2);
    out2 << "Paragraph 1\n\nParagraph 2\nwith multiple lines\n\nParagraph 3";
    out2.close();
    
    try {
        // Test reading entire file
        NodeContext context;
        context.setParameter("file_path", ParameterValue(test_file1));
        context.setParameter("split_mode", ParameterValue("none"));
        context.setParameter("include_metadata", ParameterValue(true));
        
        auto result = file_reader.execute(context);
        TEST_ASSERT(result.isSuccess(), "FileReader whole file execution success");
        TEST_ASSERT(result.items.size() == 1, "FileReader whole file item count");
        TEST_ASSERT(result.items[0].json.toString() == "Line 1\nLine 2\nLine 3", "FileReader whole file content");
        
        auto file_path_meta = result.items[0].getMetadata("file_path");
        TEST_ASSERT(file_path_meta.has_value() && file_path_meta->toString() == test_file1, "FileReader file path metadata");
        
        // Test reading by lines
        context = NodeContext();
        context.setParameter("file_path", ParameterValue(test_file1));
        context.setParameter("split_mode", ParameterValue("lines"));
        context.setParameter("include_metadata", ParameterValue(true));
        
        result = file_reader.execute(context);
        TEST_ASSERT(result.isSuccess(), "FileReader lines execution success");
        TEST_ASSERT(result.items.size() == 3, "FileReader lines item count");
        TEST_ASSERT(result.items[0].json.toString() == "Line 1", "FileReader line 1 content");
        TEST_ASSERT(result.items[2].json.toString() == "Line 3", "FileReader line 3 content");
        
        auto line_num_meta = result.items[1].getMetadata("line_number");
        TEST_ASSERT(line_num_meta.has_value() && line_num_meta->as<int64_t>() == 2, "FileReader line number metadata");
        
        // Test reading by paragraphs
        context = NodeContext();
        context.setParameter("file_path", ParameterValue(test_file2));
        context.setParameter("split_mode", ParameterValue("paragraphs"));
        context.setParameter("include_metadata", ParameterValue(true));
        
        result = file_reader.execute(context);
        TEST_ASSERT(result.isSuccess(), "FileReader paragraphs execution success");
        TEST_ASSERT(result.items.size() == 3, "FileReader paragraphs item count");
        TEST_ASSERT(result.items[0].json.toString() == "Paragraph 1", "FileReader paragraph 1 content");
        TEST_ASSERT(result.items[1].json.toString() == "Paragraph 2\nwith multiple lines", "FileReader paragraph 2 content");
        
        // Test reading from input items (file paths as input)
        context = NodeContext();
        context.addInputItem(ParameterValue(test_file1));
        context.setParameter("split_mode", ParameterValue("none"));
        context.setParameter("include_metadata", ParameterValue(false));
        
        result = file_reader.execute(context);
        TEST_ASSERT(result.isSuccess(), "FileReader input paths execution success");
        TEST_ASSERT(result.items.size() == 1, "FileReader input paths item count");
        
        // Test nonexistent file
        context = NodeContext();
        context.setParameter("file_path", ParameterValue("nonexistent_file.txt"));
        
        result = file_reader.execute(context);
        TEST_ASSERT(!result.isSuccess(), "FileReader nonexistent file fails");
        TEST_ASSERT(!result.errors.empty(), "FileReader nonexistent file has errors");
        
    } catch (const std::exception& e) {
        TEST_ASSERT(false, "FileReader test exception: " + std::string(e.what()));
    }
    
    // Clean up test files
    std::filesystem::remove(test_file1);
    std::filesystem::remove(test_file2);
}

void test_registry_integration() {
    std::cout << "\n=== Testing Registry Integration ===" << std::endl;
    
    NodeRegistry& registry = NodeRegistry::getInstance();
    registry.clear();
    
    // Register all builtin nodes
    registerBuiltinNodes(registry);
    
    // Test that all nodes are registered
    TEST_ASSERT(registry.hasNodeId("system.logger.v1"), "Logger registered in registry");
    TEST_ASSERT(registry.hasNodeId("data.mapper.v1"), "DataMapper registered in registry");
    TEST_ASSERT(registry.hasNodeId("control.conditional.v1"), "Conditional registered in registry");
    TEST_ASSERT(registry.hasNodeId("akao.logic_executor.v1"), "AkaoLogic registered in registry");
    TEST_ASSERT(registry.hasNodeId("io.file_reader.v1"), "FileReader registered in registry");
    
    // Test node creation through registry
    auto logger = registry.createNodeById("system.logger.v1");
    TEST_ASSERT(logger != nullptr, "Registry creates logger node");
    TEST_ASSERT(logger->getNodeId() == "system.logger.v1", "Registry created node has correct ID");
    
    auto mapper = registry.createNodeById("data.mapper.v1");
    TEST_ASSERT(mapper != nullptr, "Registry creates mapper node");
    
    // Test getting node definitions
    auto logger_def = registry.getDefinitionById("system.logger.v1");
    TEST_ASSERT(logger_def.has_value(), "Registry returns logger definition");
    TEST_ASSERT(logger_def->display_name == "Logger Node", "Registry definition has correct name");
    
    // Test category filtering
    auto core_nodes = registry.getNodesByCategory(NodeCategory::CORE);
    TEST_ASSERT(core_nodes.size() >= 3, "Registry has core nodes"); // Logger, Mapper, Conditional
    
    auto filesystem_nodes = registry.getNodesByCategory(NodeCategory::FILESYSTEM);
    TEST_ASSERT(filesystem_nodes.size() >= 1, "Registry has filesystem nodes"); // FileReader
    
    auto akao_nodes = registry.getNodesByCategory(NodeCategory::AKAO);
    TEST_ASSERT(akao_nodes.size() >= 1, "Registry has Akao nodes"); // AkaoLogic
}

void test_node_workflow() {
    std::cout << "\n=== Testing Node Workflow ===" << std::endl;
    
    // Create a simple workflow: FileReader -> DataMapper -> Logger
    
    // Step 1: Create test file
    std::string test_file = "workflow_test.txt";
    std::ofstream out(test_file);
    out << "hello world\ntest data\nfinal line";
    out.close();
    
    try {
        // Step 2: FileReader to read lines
        FileReaderNode file_reader;
        NodeContext context1;
        context1.setParameter("file_path", ParameterValue(test_file));
        context1.setParameter("split_mode", ParameterValue("lines"));
        context1.setParameter("include_metadata", ParameterValue(false));
        
        auto result1 = file_reader.execute(context1);
        TEST_ASSERT(result1.isSuccess(), "Workflow step 1: FileReader success");
        TEST_ASSERT(result1.items.size() == 3, "Workflow step 1: FileReader item count");
        
        // Step 3: DataMapper to uppercase
        DataMapperNode mapper;
        NodeContext context2;
        for (const auto& item : result1.items) {
            context2.addInputItem(item);
        }
        context2.setParameter("operation", ParameterValue("uppercase"));
        
        auto result2 = mapper.execute(context2);
        TEST_ASSERT(result2.isSuccess(), "Workflow step 2: DataMapper success");
        TEST_ASSERT(result2.items.size() == 3, "Workflow step 2: DataMapper item count");
        TEST_ASSERT(result2.items[0].json.toString() == "HELLO WORLD", "Workflow step 2: DataMapper transformation");
        
        // Step 4: Logger to output
        LoggerNode logger;
        NodeContext context3;
        for (const auto& item : result2.items) {
            context3.addInputItem(item);
        }
        context3.setParameter("level", ParameterValue("info"));
        context3.setParameter("prefix", ParameterValue("WORKFLOW"));
        context3.setParameter("timestamp", ParameterValue(false));
        
        auto result3 = logger.execute(context3);
        TEST_ASSERT(result3.isSuccess(), "Workflow step 3: Logger success");
        TEST_ASSERT(result3.items.size() == 3, "Workflow step 3: Logger pass-through count");
        
        std::cout << "🔄 Workflow completed successfully: File -> Transform -> Log" << std::endl;
        
    } catch (const std::exception& e) {
        TEST_ASSERT(false, "Workflow test exception: " + std::string(e.what()));
    }
    
    // Clean up test file
    std::filesystem::remove(test_file);
}

void test_performance() {
    std::cout << "\n=== Testing Performance ===" << std::endl;
    
    // Test DataMapper performance with many items
    DataMapperNode mapper;
    NodeContext context;
    
    const int item_count = 1000;
    for (int i = 0; i < item_count; ++i) {
        context.addInputItem(ParameterValue("test item " + std::to_string(i)));
    }
    context.setParameter("operation", ParameterValue("uppercase"));
    
    auto start_time = std::chrono::high_resolution_clock::now();
    auto result = mapper.execute(context);
    auto end_time = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    TEST_ASSERT(result.isSuccess(), "Performance test: DataMapper success");
    TEST_ASSERT(result.items.size() == item_count, "Performance test: DataMapper item count");
    TEST_ASSERT(duration.count() < 1000, "Performance test: DataMapper under 1 second for 1000 items");
    
    std::cout << "⚡ Performance: Processed " << item_count << " items in " << duration.count() << "ms" << std::endl;
}

int main() {
    std::cout << "🚀 Starting Builtin Nodes Tests" << std::endl;
    std::cout << "================================" << std::endl;
    
    test_parameter_value_integration();
    test_execution_result();
    test_node_context();
    test_logger_node();
    test_data_mapper_node();
    test_conditional_node();
    test_akao_logic_node();
    test_file_reader_node();
    test_registry_integration();
    test_node_workflow();
    test_performance();
    
    std::cout << "\n================================" << std::endl;
    std::cout << "📊 Test Results Summary:" << std::endl;
    std::cout << "✅ Tests Passed: " << tests_passed << std::endl;
    std::cout << "❌ Tests Failed: " << tests_failed << std::endl;
    std::cout << "📈 Success Rate: " << (tests_passed * 100 / (tests_passed + tests_failed)) << "%" << std::endl;
    
    if (tests_failed == 0) {
        std::cout << "\n🎉 ALL TESTS PASSED! Builtin nodes are working correctly." << std::endl;
        std::cout << "✅ All 5 core nodes implemented and tested" << std::endl;
        std::cout << "✅ LoggerNode: system logging and output" << std::endl;
        std::cout << "✅ DataMapperNode: data transformation" << std::endl;
        std::cout << "✅ ConditionalNode: flow control and filtering" << std::endl;
        std::cout << "✅ AkaoLogicNode: Akao Pure Logic execution" << std::endl;
        std::cout << "✅ FileReaderNode: file I/O operations" << std::endl;
        std::cout << "✅ Registry integration complete" << std::endl;
        std::cout << "✅ Node workflow execution verified" << std::endl;
        std::cout << "✅ Performance requirements met" << std::endl;
        return 0;
    } else {
        std::cout << "\n❌ SOME TESTS FAILED! Please review the implementation." << std::endl;
        return 1;
    }
}