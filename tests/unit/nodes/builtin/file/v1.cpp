/**
 * @id: akao:file:tests:unit:nodes:builtin:file:v1
 * @doc: Unit tests for clean filesystem scanner node with comprehensive file analysis capabilities. Tests node interface implementation, parameter validation, filesystem scanning functionality, and static utility methods.
 * @specification: Unit tests for clean filesystem scanner node
 * @scope: Builtin filesystem scanner node testing
 * @timeline: 2025-07-10
 * @rationale: Ensure reliable filesystem scanner node functionality
 * @methodology: Comprehensive testing with real filesystem operations
 * @references: ["akao:class:nodes:builtin:file:v1"]
 */

#include "nodes/builtin/file/v1.hpp"
#include <cassert>
#include <iostream>
#include <filesystem>
#include <fstream>

using namespace akao::nodes::builtin;
using namespace akao::foundation::interfaces;
using namespace akao::foundation::types;

namespace akao {
namespace tests {

void testNodeInfo() {
    std::cout << "Testing node info..." << std::endl;
    
    FilesystemScannerNode node;
    
    assert(node.getNodeId() == "akao:builtin:filesystem-scanner");
    assert(node.getNodeType() == "filesystem");
    assert(node.getVersion() == "1.0.0");
    assert(!node.getDescription().empty());
    
    // Test parameter schema
    NodeValue schema = node.getParameterSchema();
    assert(schema.isObject());
    assert(schema.hasKey("path"));
    assert(schema.hasKey("recursive"));
    assert(schema.hasKey("analyze_content"));
    assert(schema.hasKey("extensions"));
    
    std::cout << "Node info test passed!" << std::endl;
}

void testParameterValidation() {
    std::cout << "Testing parameter validation..." << std::endl;
    
    FilesystemScannerNode node;
    
    // Test missing path
    NodeParameters emptyParams;
    ValidationResult result1 = node.validate(emptyParams);
    assert(!result1.isValid());
    
    // Test empty path
    NodeParameters emptyPathParams;
    emptyPathParams.setParameter("path", NodeValue(""));
    ValidationResult result2 = node.validate(emptyPathParams);
    assert(!result2.isValid());
    
    // Test non-existent path
    NodeParameters invalidPathParams;
    invalidPathParams.setParameter("path", NodeValue("/non/existent/path"));
    ValidationResult result3 = node.validate(invalidPathParams);
    assert(!result3.isValid());
    
    // Test valid path (current directory)
    NodeParameters validParams;
    validParams.setParameter("path", NodeValue("."));
    ValidationResult result4 = node.validate(validParams);
    assert(result4.isValid());
    
    std::cout << "Parameter validation test passed!" << std::endl;
}

void testStaticUtilityMethods() {
    std::cout << "Testing static utility methods..." << std::endl;
    
    // Test fileExists
    bool exists1 = FilesystemScannerNode::fileExists(".");
    assert(exists1 == true);
    
    bool exists2 = FilesystemScannerNode::fileExists("/non/existent/file");
    assert(exists2 == false);
    
    // Test getFileType
    std::string type1 = FilesystemScannerNode::getFileType("test.cpp");
    assert(type1 == "source");
    
    std::string type2 = FilesystemScannerNode::getFileType("test.py");
    assert(type2 == "source");
    
    std::string type3 = FilesystemScannerNode::getFileType("test.yaml");
    assert(type3 == "config");
    
    std::string type4 = FilesystemScannerNode::getFileType("test.md");
    assert(type4 == "docs");
    
    // Test isTextFile
    bool text1 = FilesystemScannerNode::isTextFile("test.cpp");
    assert(text1 == true);
    
    bool text2 = FilesystemScannerNode::isTextFile("test.bin");
    assert(text2 == false);
    
    // Test getFilesInDirectory (current directory)
    auto files = FilesystemScannerNode::getFilesInDirectory(".", false);
    assert(!files.empty()); // Should find some files in current directory
    
    // Test getRelativePath
    std::string rel = FilesystemScannerNode::getRelativePath("/home/user/project/file.cpp", "/home/user");
    assert(rel == "project/file.cpp");
    
    std::cout << "Static utility methods test passed!" << std::endl;
}

void testBasicScanning() {
    std::cout << "Testing basic scanning..." << std::endl;
    
    // Create a temporary test directory structure
    std::string testDir = "test_scan_dir";
    std::filesystem::create_directory(testDir);
    
    // Create some test files
    std::ofstream file1(testDir + "/test1.cpp");
    file1 << "#include <iostream>\nint main() {\n    std::cout << \"Hello\" << std::endl;\n    return 0;\n}";
    file1.close();
    
    std::ofstream file2(testDir + "/test2.hpp");
    file2 << "#pragma once\nclass Test {\npublic:\n    void run();\n};";
    file2.close();
    
    std::ofstream file3(testDir + "/readme.md");
    file3 << "# Test Directory\nThis is a test directory.\n\n## Purpose\nTesting filesystem scanner.";
    file3.close();
    
    std::ofstream file4(testDir + "/config.yaml");
    file4 << "name: test\nversion: 1.0.0\nsettings:\n  enabled: true";
    file4.close();
    
    try {
        FilesystemScannerNode node;
        NodeContext context("test-scanner", NodeValue());
        
        // Test basic scanning
        NodeParameters params;
        params.setParameter("path", NodeValue(testDir));
        params.setParameter("recursive", NodeValue(false));
        params.setParameter("analyze_content", NodeValue(true));
        
        ExecutionResult result = node.execute(context, params);
        assert(result.isSuccess());
        
        NodeValue data = result.getData();
        assert(data.isObject());
        assert(data.hasKey("success"));
        assert(data.hasKey("total_files"));
        assert(data.hasKey("files"));
        assert(data.hasKey("file_type_counts"));
        assert(data.hasKey("extension_counts"));
        assert(data.hasKey("language_counts"));
        
        // Check that we found our test files
        assert(data["total_files"].asInteger() == 4);
        assert(data["success"].asBoolean() == true);
        
        // Check file type counts
        NodeValue fileTypeCounts = data["file_type_counts"];
        assert(fileTypeCounts.hasKey("source"));
        assert(fileTypeCounts.hasKey("docs"));
        assert(fileTypeCounts.hasKey("config"));
        assert(fileTypeCounts["source"].asInteger() == 2); // .cpp and .hpp
        assert(fileTypeCounts["docs"].asInteger() == 1);   // .md
        assert(fileTypeCounts["config"].asInteger() == 1); // .yaml
        
        // Check language counts
        NodeValue languageCounts = data["language_counts"];
        assert(languageCounts.hasKey("cpp"));
        assert(languageCounts.hasKey("markdown"));
        assert(languageCounts.hasKey("yaml"));
        assert(languageCounts["cpp"].asInteger() == 2);
        
        // Check files array
        NodeValue files = data["files"];
        assert(files.isArray());
        assert(files.asArray().size() == 4);
        
        // Check metadata
        assert(data.hasMetadata("scannedPath"));
        assert(data.hasMetadata("scannerVersion"));
        assert(data.getMetadata("scannedPath").asString() == testDir);
        
        std::cout << "Basic scanning test passed!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        assert(false);
    }
    
    // Cleanup
    std::filesystem::remove_all(testDir);
}

void testContentAnalysis() {
    std::cout << "Testing content analysis..." << std::endl;
    
    // Create a test file
    std::string testFile = "test_content.cpp";
    std::ofstream file(testFile);
    file << "#include <iostream>\n";
    file << "#include <string>\n";
    file << "\n";
    file << "int main() {\n";
    file << "    std::string message = \"Hello, World!\";\n";
    file << "    std::cout << message << std::endl;\n";
    file << "    return 0;\n";
    file << "}\n";
    file.close();
    
    try {
        FilesystemScannerNode node;
        NodeContext context("test-scanner", NodeValue());
        
        NodeParameters params;
        params.setParameter("path", NodeValue(testFile));
        params.setParameter("analyze_content", NodeValue(true));
        
        ExecutionResult result = node.execute(context, params);
        assert(result.isSuccess());
        
        NodeValue data = result.getData();
        NodeValue files = data["files"];
        assert(files.asArray().size() == 1);
        
        NodeValue fileInfo = files.asArray()[0];
        assert(fileInfo.hasKey("line_count"));
        assert(fileInfo.hasKey("languages"));
        assert(fileInfo.hasKey("metadata"));
        
        // Should count lines (check that content analysis worked)
        int64_t lineCount = fileInfo["line_count"].asInteger();
        std::cout << "Detected line count: " << lineCount << std::endl;
        assert(lineCount > 0); // Just check that content was analyzed
        
        // Should detect C++ language
        NodeValue languages = fileInfo["languages"];
        assert(languages.isArray());
        assert(languages.asArray().size() == 1);
        assert(languages.asArray()[0].asString() == "cpp");
        
        std::cout << "Content analysis test passed!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        assert(false);
    }
    
    // Cleanup
    std::filesystem::remove(testFile);
}

void testCppFileFiltering() {
    std::cout << "Testing C++ file filtering..." << std::endl;
    
    // Test getCppFiles static method
    auto cppFiles = FilesystemScannerNode::getCppFiles(".", true);
    
    // Should find some C++ files in the project
    bool foundCppFile = false;
    for (const auto& file : cppFiles) {
        std::string ext = std::filesystem::path(file).extension().string();
        if (ext == ".cpp" || ext == ".hpp") {
            foundCppFile = true;
            break;
        }
    }
    assert(foundCppFile);
    
    std::cout << "C++ file filtering test passed!" << std::endl;
}

void testExtensionFiltering() {
    std::cout << "Testing extension filtering..." << std::endl;
    
    // Create test directory with various file types
    std::string testDir = "test_ext_dir";
    std::filesystem::create_directory(testDir);
    
    std::ofstream(testDir + "/file1.cpp").close();
    std::ofstream(testDir + "/file2.hpp").close();
    std::ofstream(testDir + "/file3.py").close();
    std::ofstream(testDir + "/file4.txt").close();
    
    try {
        FilesystemScannerNode node;
        NodeContext context("test-scanner", NodeValue());
        
        // Test with extension filter
        NodeParameters params;
        params.setParameter("path", NodeValue(testDir));
        params.setParameter("recursive", NodeValue(false));
        
        NodeValue::Array extensions;
        extensions.push_back(NodeValue(".cpp"));
        extensions.push_back(NodeValue(".hpp"));
        params.setParameter("extensions", NodeValue(extensions));
        
        std::cout << "Set up extension filtering parameters" << std::endl;
        
        std::cout << "About to validate parameters" << std::endl;
        
        // First validate parameters
        ValidationResult validation = node.validate(params);
        if (!validation.isValid()) {
            std::cerr << "Parameter validation failed for extension filtering" << std::endl;
            for (const auto& error : validation.getErrors()) {
                std::cerr << "Error: " << error << std::endl;
            }
        }
        assert(validation.isValid());
        
        ExecutionResult result = node.execute(context, params);
        if (!result.isSuccess()) {
            std::cerr << "Extension filtering failed: " << result.getErrorSummary() << std::endl;
        }
        assert(result.isSuccess());
        
        NodeValue data = result.getData();
        // Should only find .cpp and .hpp files
        std::cout << "Found " << data["total_files"].asInteger() << " files with extension filter" << std::endl;
        assert(data["total_files"].asInteger() == 2);
        
        std::cout << "Extension filtering test passed!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        assert(false);
    }
    
    // Cleanup
    std::filesystem::remove_all(testDir);
}

void testErrorHandling() {
    std::cout << "Testing error handling..." << std::endl;
    
    FilesystemScannerNode node;
    NodeContext context("test-scanner", NodeValue());
    
    // Test scanning non-existent directory
    NodeParameters params;
    params.setParameter("path", NodeValue("/definitely/non/existent/path"));
    
    ExecutionResult result = node.execute(context, params);
    // Should return error result for non-existent path
    assert(result.isError());
    
    std::cout << "Error handling test passed!" << std::endl;
}

// Export test runner function for main test runner
void runFilesystemScannerTests() {
    std::cout << "Running Clean Filesystem Scanner Node unit tests..." << std::endl;
    
    testNodeInfo();
    testParameterValidation();
    testStaticUtilityMethods();
    testBasicScanning();
    testContentAnalysis();
    testCppFileFiltering();
    testExtensionFiltering();
    testErrorHandling();
    
    std::cout << "All Clean Filesystem Scanner tests passed!" << std::endl;
}

} // namespace tests
} // namespace akao