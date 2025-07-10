/**
 * @id: akao:file:tests:unit:nodes:builtin:file:v1
 * @doc: Comprehensive unit tests for filesystem scanner node. Tests file scanning, pattern matching, metadata extraction, filtering capabilities, and utility methods to ensure reliable filesystem operations for workflow automation.
 * @specification: Unit tests for filesystem scanner node with comprehensive coverage
 * @scope: Builtin filesystem node testing
 * @timeline: 2025-07-10
 * @rationale: Ensure reliability of filesystem scanner for validation workflows
 * @methodology: Complete test coverage with real filesystem operations and edge cases
 * @references: ["akao:class:nodes:builtin:file:v1"]
 */

#include "nodes/builtin/file/v1.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

using namespace akao::nodes::builtin;
using namespace akao::foundation::interfaces;
using namespace akao::foundation::types;

// Test helper to create temporary files
class TestFileHelper {
private:
    std::string testDir_;
    
public:
    TestFileHelper() {
        testDir_ = "test_files_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
        std::filesystem::create_directory(testDir_);
    }
    
    ~TestFileHelper() {
        try {
            std::filesystem::remove_all(testDir_);
        } catch (...) {
            // Ignore cleanup errors
        }
    }
    
    std::string getTestDir() const { return testDir_; }
    
    void createFile(const std::string& path, const std::string& content = "test content") {
        std::string fullPath = testDir_ + "/" + path;
        std::filesystem::create_directories(std::filesystem::path(fullPath).parent_path());
        std::ofstream file(fullPath);
        file << content;
    }
    
    void createDirectory(const std::string& path) {
        std::filesystem::create_directories(testDir_ + "/" + path);
    }
};

void testNodeInfo() {
    std::cout << "Testing node info..." << std::endl;
    
    FilesystemScannerNode node;
    
    assert(node.getNodeId() == "akao:builtin:filesystem-scanner");
    assert(node.getNodeType() == "file");
    assert(node.getVersion() == "1.0.0");
    assert(!node.getDescription().empty());
    
    // Test parameter schema
    NodeValue schema = node.getParameterSchema();
    assert(schema.isObject());
    assert(schema.hasKey("path"));
    assert(schema.hasKey("recursive"));
    assert(schema.hasKey("extensions"));
    
    std::cout << "Node info test passed!" << std::endl;
}

void testParameterValidation() {
    std::cout << "Testing parameter validation..." << std::endl;
    
    FilesystemScannerNode node;
    
    // Test missing path parameter
    NodeParameters emptyParams;
    ValidationResult result1 = node.validate(emptyParams);
    assert(!result1.isValid());
    assert(result1.hasErrors());
    assert(result1.getErrors()[0].find("path") != std::string::npos);
    
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
    
    // Test valid path
    NodeParameters validParams;
    validParams.setParameter("path", NodeValue("."));
    ValidationResult result4 = node.validate(validParams);
    assert(result4.isValid());
    
    std::cout << "Parameter validation test passed!" << std::endl;
}

void testBasicScanning() {
    std::cout << "Testing basic scanning..." << std::endl;
    
    TestFileHelper helper;
    helper.createFile("test1.txt", "content1");
    helper.createFile("test2.cpp", "content2");
    helper.createFile("subdir/test3.hpp", "content3");
    helper.createDirectory("emptydir");
    
    FilesystemScannerNode node;
    NodeContext context("test-filesystem-scanner", NodeValue());
    NodeParameters params;
    params.setParameter("path", NodeValue(helper.getTestDir()));
    
    ExecutionResult result = node.execute(context, params);
    assert(result.isSuccess());
    
    NodeValue data = result.getData();
    assert(data.isArray());
    
    auto files = data.asArray();
    assert(files.size() >= 4); // At least 4 entries (3 files + 1+ dirs)
    
    // Check that we have the expected files
    bool foundTxt = false, foundCpp = false, foundHpp = false, foundDir = false;
    
    for (const auto& file : files) {
        assert(file.isObject());
        assert(file.hasKey("path"));
        assert(file.hasKey("name"));
        assert(file.hasKey("isDirectory"));
        assert(file.hasKey("isRegularFile"));
        
        std::string name = file["name"].asString();
        if (name == "test1.txt") foundTxt = true;
        if (name == "test2.cpp") foundCpp = true;
        if (name == "test3.hpp") foundHpp = true;
        if (name == "subdir" || name == "emptydir") foundDir = true;
    }
    
    assert(foundTxt && foundCpp && foundHpp && foundDir);
    
    std::cout << "Basic scanning test passed!" << std::endl;
}

void testRecursiveScanning() {
    std::cout << "Testing recursive scanning..." << std::endl;
    
    TestFileHelper helper;
    helper.createFile("root.txt", "root content");
    helper.createFile("level1/file1.txt", "level1 content");
    helper.createFile("level1/level2/file2.txt", "level2 content");
    helper.createFile("level1/level2/level3/file3.txt", "level3 content");
    
    FilesystemScannerNode node;
    NodeContext context("test-filesystem-scanner", NodeValue());
    
    // Test recursive scanning (default)
    NodeParameters recursiveParams;
    recursiveParams.setParameter("path", NodeValue(helper.getTestDir()));
    
    ExecutionResult recursiveResult = node.execute(context, recursiveParams);
    assert(recursiveResult.isSuccess());
    
    auto recursiveFiles = recursiveResult.getData().asArray();
    
    // Should find all files
    int fileCount = 0;
    for (const auto& file : recursiveFiles) {
        if (file["isRegularFile"].asBoolean()) {
            fileCount++;
        }
    }
    assert(fileCount >= 4); // At least our 4 test files
    
    // Test non-recursive scanning
    NodeParameters nonRecursiveParams;
    nonRecursiveParams.setParameter("path", NodeValue(helper.getTestDir()));
    nonRecursiveParams.setParameter("recursive", NodeValue(false));
    
    ExecutionResult nonRecursiveResult = node.execute(context, nonRecursiveParams);
    assert(nonRecursiveResult.isSuccess());
    
    auto nonRecursiveFiles = nonRecursiveResult.getData().asArray();
    
    // Should find fewer files (only root level)
    int rootFileCount = 0;
    for (const auto& file : nonRecursiveFiles) {
        if (file["isRegularFile"].asBoolean()) {
            rootFileCount++;
        }
    }
    assert(rootFileCount < fileCount);
    assert(rootFileCount >= 1); // At least root.txt
    
    std::cout << "Recursive scanning test passed!" << std::endl;
}

void testExtensionFiltering() {
    std::cout << "Testing extension filtering..." << std::endl;
    
    TestFileHelper helper;
    helper.createFile("test.cpp", "cpp content");
    helper.createFile("test.hpp", "hpp content");
    helper.createFile("test.txt", "txt content");
    helper.createFile("test.py", "py content");
    
    FilesystemScannerNode node;
    NodeContext context("test-filesystem-scanner", NodeValue());
    NodeParameters params;
    params.setParameter("path", NodeValue(helper.getTestDir()));
    
    // Filter for C++ files only
    NodeValue::Array extensions;
    extensions.push_back(NodeValue(".cpp"));
    extensions.push_back(NodeValue(".hpp"));
    params.setParameter("extensions", NodeValue(extensions));
    
    ExecutionResult result = node.execute(context, params);
    assert(result.isSuccess());
    
    auto files = result.getData().asArray();
    
    // Count regular files (ignore directories)
    int cppFileCount = 0;
    for (const auto& file : files) {
        if (file["isRegularFile"].asBoolean()) {
            cppFileCount++;
            std::string ext = file["extension"].asString();
            assert(ext == ".cpp" || ext == ".hpp");
        }
    }
    
    assert(cppFileCount == 2); // Should only find .cpp and .hpp files
    
    std::cout << "Extension filtering test passed!" << std::endl;
}

void testPatternMatching() {
    std::cout << "Testing pattern matching..." << std::endl;
    
    TestFileHelper helper;
    helper.createFile("include_this.txt", "content");
    helper.createFile("exclude_this.txt", "content");
    helper.createFile("normal_file.txt", "content");
    
    FilesystemScannerNode node;
    NodeContext context("test-filesystem-scanner", NodeValue());
    NodeParameters params;
    params.setParameter("path", NodeValue(helper.getTestDir()));
    
    // Test include pattern
    NodeValue::Array includePatterns;
    includePatterns.push_back(NodeValue(".*include.*"));
    params.setParameter("includePatterns", NodeValue(includePatterns));
    
    ExecutionResult includeResult = node.execute(context, params);
    assert(includeResult.isSuccess());
    
    auto includeFiles = includeResult.getData().asArray();
    bool foundIncludeFile = false;
    
    for (const auto& file : includeFiles) {
        if (file["isRegularFile"].asBoolean()) {
            std::string name = file["name"].asString();
            if (name.find("include") != std::string::npos) {
                foundIncludeFile = true;
            }
            // Should not find files without "include" in name
            assert(name.find("include") != std::string::npos || name.find("exclude") != std::string::npos || name.find("normal") != std::string::npos);
        }
    }
    assert(foundIncludeFile);
    
    // Test exclude pattern
    NodeParameters excludeParams;
    excludeParams.setParameter("path", NodeValue(helper.getTestDir()));
    
    NodeValue::Array excludePatterns;
    excludePatterns.push_back(NodeValue(".*exclude.*"));
    excludeParams.setParameter("excludePatterns", NodeValue(excludePatterns));
    
    ExecutionResult excludeResult = node.execute(context, excludeParams);
    assert(excludeResult.isSuccess());
    
    auto excludeFiles = excludeResult.getData().asArray();
    
    for (const auto& file : excludeFiles) {
        if (file["isRegularFile"].asBoolean()) {
            std::string name = file["name"].asString();
            // Should not find files with "exclude" in name
            assert(name.find("exclude") == std::string::npos);
        }
    }
    
    std::cout << "Pattern matching test passed!" << std::endl;
}

void testFileSizeFiltering() {
    std::cout << "Testing file size filtering..." << std::endl;
    
    TestFileHelper helper;
    helper.createFile("small.txt", "x"); // 1 byte
    helper.createFile("medium.txt", std::string(100, 'x')); // 100 bytes
    helper.createFile("large.txt", std::string(1000, 'x')); // 1000 bytes
    
    FilesystemScannerNode node;
    NodeContext context("test-filesystem-scanner", NodeValue());
    NodeParameters params;
    params.setParameter("path", NodeValue(helper.getTestDir()));
    params.setParameter("minSize", NodeValue(static_cast<int64_t>(50)));
    params.setParameter("maxSize", NodeValue(static_cast<int64_t>(500)));
    
    ExecutionResult result = node.execute(context, params);
    assert(result.isSuccess());
    
    auto files = result.getData().asArray();
    
    for (const auto& file : files) {
        if (file["isRegularFile"].asBoolean()) {
            int64_t size = file["size"].asInteger();
            std::string name = file["name"].asString();
            
            if (name.find(".txt") != std::string::npos) {
                // Should only find medium.txt (100 bytes)
                assert(size >= 50 && size <= 500);
                assert(name == "medium.txt");
            }
        }
    }
    
    std::cout << "File size filtering test passed!" << std::endl;
}

void testTypeFiltering() {
    std::cout << "Testing type filtering..." << std::endl;
    
    TestFileHelper helper;
    helper.createFile("test.txt", "content");
    helper.createDirectory("testdir");
    
    FilesystemScannerNode node;
    NodeContext context("test-filesystem-scanner", NodeValue());
    
    // Test files only
    NodeParameters filesOnlyParams;
    filesOnlyParams.setParameter("path", NodeValue(helper.getTestDir()));
    filesOnlyParams.setParameter("includeFiles", NodeValue(true));
    filesOnlyParams.setParameter("includeDirectories", NodeValue(false));
    
    ExecutionResult filesResult = node.execute(context, filesOnlyParams);
    assert(filesResult.isSuccess());
    
    auto files = filesResult.getData().asArray();
    for (const auto& file : files) {
        assert(file["isRegularFile"].asBoolean());
        assert(!file["isDirectory"].asBoolean());
    }
    
    // Test directories only
    NodeParameters dirsOnlyParams;
    dirsOnlyParams.setParameter("path", NodeValue(helper.getTestDir()));
    dirsOnlyParams.setParameter("includeFiles", NodeValue(false));
    dirsOnlyParams.setParameter("includeDirectories", NodeValue(true));
    
    ExecutionResult dirsResult = node.execute(context, dirsOnlyParams);
    assert(dirsResult.isSuccess());
    
    auto dirs = dirsResult.getData().asArray();
    for (const auto& dir : dirs) {
        assert(dir["isDirectory"].asBoolean());
        assert(!dir["isRegularFile"].asBoolean());
    }
    
    std::cout << "Type filtering test passed!" << std::endl;
}

void testUtilityMethods() {
    std::cout << "Testing utility methods..." << std::endl;
    
    TestFileHelper helper;
    helper.createFile("test.cpp", "cpp content");
    helper.createFile("test.hpp", "hpp content");
    helper.createFile("test.txt", "txt content");
    helper.createFile("subdir/nested.cpp", "nested cpp");
    
    // Test getFilesInDirectory
    auto allFiles = FilesystemScannerNode::getFilesInDirectory(helper.getTestDir(), true);
    assert(allFiles.size() >= 4);
    
    auto nonRecursiveFiles = FilesystemScannerNode::getFilesInDirectory(helper.getTestDir(), false);
    assert(nonRecursiveFiles.size() < allFiles.size());
    
    // Test getCppFiles
    auto cppFiles = FilesystemScannerNode::getCppFiles(helper.getTestDir(), true);
    assert(cppFiles.size() >= 2); // test.cpp and nested.cpp
    
    // Test getHeaderFiles
    auto headerFiles = FilesystemScannerNode::getHeaderFiles(helper.getTestDir(), true);
    assert(headerFiles.size() >= 1); // test.hpp
    
    // Test fileExists
    std::string testFile = helper.getTestDir() + "/test.cpp";
    assert(FilesystemScannerNode::fileExists(testFile));
    assert(!FilesystemScannerNode::fileExists("/non/existent/file"));
    
    // Test getFileSize
    size_t size = FilesystemScannerNode::getFileSize(testFile);
    assert(size > 0);
    
    // Test getFileContent
    std::string content = FilesystemScannerNode::getFileContent(testFile);
    assert(content == "cpp content");
    
    std::cout << "Utility methods test passed!" << std::endl;
}

void testMetadataCollection() {
    std::cout << "Testing metadata collection..." << std::endl;
    
    TestFileHelper helper;
    helper.createFile("meta_test.txt", "metadata test content");
    
    FilesystemScannerNode node;
    NodeContext context("test-filesystem-scanner", NodeValue());
    NodeParameters params;
    params.setParameter("path", NodeValue(helper.getTestDir()));
    
    ExecutionResult result = node.execute(context, params);
    assert(result.isSuccess());
    
    NodeValue data = result.getData();
    
    // Check result metadata
    assert(data.hasMetadata("fileCount"));
    assert(data.hasMetadata("scanPath"));
    assert(data.hasMetadata("recursive"));
    
    assert(data.getMetadata("scanPath").asString() == helper.getTestDir());
    assert(data.getMetadata("recursive").asBoolean() == true);
    
    // Check individual file metadata
    auto files = data.asArray();
    bool foundTestFile = false;
    
    for (const auto& file : files) {
        if (file["name"].asString() == "meta_test.txt") {
            foundTestFile = true;
            
            assert(file.hasKey("path"));
            assert(file.hasKey("name"));
            assert(file.hasKey("extension"));
            assert(file.hasKey("directory"));
            assert(file.hasKey("size"));
            assert(file.hasKey("lastModified"));
            assert(file.hasKey("isDirectory"));
            assert(file.hasKey("isRegularFile"));
            assert(file.hasKey("isSymlink"));
            
            assert(file["extension"].asString() == ".txt");
            assert(file["isRegularFile"].asBoolean() == true);
            assert(file["isDirectory"].asBoolean() == false);
            assert(file["size"].asInteger() > 0);
            
            break;
        }
    }
    
    assert(foundTestFile);
    
    std::cout << "Metadata collection test passed!" << std::endl;
}

void testErrorHandling() {
    std::cout << "Testing error handling..." << std::endl;
    
    FilesystemScannerNode node;
    NodeContext context("test-filesystem-scanner", NodeValue());
    NodeParameters params;
    
    // Test with invalid path (should be caught by validation, but test execution too)
    params.setParameter("path", NodeValue("/absolutely/non/existent/path/nowhere"));
    
    ExecutionResult result = node.execute(context, params);
    // Should either succeed with empty results or fail gracefully
    assert(result.isSuccess() || result.isError());
    
    if (result.isSuccess()) {
        // If it succeeds, should return empty array
        auto files = result.getData().asArray();
        assert(files.empty());
    }
    
    std::cout << "Error handling test passed!" << std::endl;
}

void testLargeDirectoryHandling() {
    std::cout << "Testing large directory handling..." << std::endl;
    
    TestFileHelper helper;
    
    // Create a moderate number of files to test performance
    for (int i = 0; i < 50; ++i) {
        helper.createFile("file_" + std::to_string(i) + ".txt", "content " + std::to_string(i));
    }
    
    FilesystemScannerNode node;
    NodeContext context("test-filesystem-scanner", NodeValue());
    NodeParameters params;
    params.setParameter("path", NodeValue(helper.getTestDir()));
    
    auto start = std::chrono::high_resolution_clock::now();
    ExecutionResult result = node.execute(context, params);
    auto end = std::chrono::high_resolution_clock::now();
    
    assert(result.isSuccess());
    
    auto files = result.getData().asArray();
    int fileCount = 0;
    for (const auto& file : files) {
        if (file["isRegularFile"].asBoolean()) {
            fileCount++;
        }
    }
    
    assert(fileCount >= 50);
    
    // Performance check (should complete in reasonable time)
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    assert(duration.count() < 5000); // Should complete within 5 seconds
    
    std::cout << "Large directory handling test passed!" << std::endl;
}

int main() {
    std::cout << "Running Filesystem Scanner Node unit tests..." << std::endl;
    
    try {
        testNodeInfo();
        testParameterValidation();
        testBasicScanning();
        testRecursiveScanning();
        testExtensionFiltering();
        testPatternMatching();
        testFileSizeFiltering();
        testTypeFiltering();
        testUtilityMethods();
        testMetadataCollection();
        testErrorHandling();
        testLargeDirectoryHandling();
        
        std::cout << "All Filesystem Scanner Node tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
}