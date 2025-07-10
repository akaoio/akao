/**
 * @id: akao:file:nodes:builtin:file:v1
 * @doc: Implementation of filesystem scanner node using legacy directory scanner. Provides adapter interface converting legacy scanner results to NodeValue format while preserving all advanced features and performance optimizations.
 * @specification: Filesystem scanner implementation using legacy scanner
 * @scope: Builtin filesystem scanning node implementation
 * @timeline: 2025-07-10
 * @rationale: Concrete implementation leveraging proven legacy scanner
 * @methodology: Adapter pattern with comprehensive NodeValue conversion
 * @references: ["akao:class:nodes:builtin:file:v1"]
 */

#include "v1.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>

namespace akao {
namespace nodes {
namespace builtin {

FilesystemScannerNode::FilesystemScannerNode()
    : nodeId_("akao:builtin:filesystem-scanner")
    , nodeType_("filesystem")
    , version_("1.0.0")
    , description_("Comprehensive filesystem scanner using legacy directory scanner") {
}

std::string FilesystemScannerNode::getNodeId() const {
    return nodeId_;
}

std::string FilesystemScannerNode::getNodeType() const {
    return nodeType_;
}

std::string FilesystemScannerNode::getVersion() const {
    return version_;
}

std::string FilesystemScannerNode::getDescription() const {
    return description_;
}

foundation::interfaces::ValidationResult FilesystemScannerNode::validate(const foundation::interfaces::NodeParameters& params) const {
    foundation::interfaces::ValidationResult result;
    
    if (!params.hasParameter("path")) {
        result.addError("Missing required parameter 'path'");
        return result;
    }
    
    std::string path = params.getParameter("path").asString();
    if (path.empty()) {
        result.addError("Parameter 'path' cannot be empty");
        return result;
    }
    
    // Validate path exists
    if (!std::filesystem::exists(path)) {
        result.addError("Path does not exist: " + path);
        return result;
    }
    
    return result;
}

foundation::types::ExecutionResult FilesystemScannerNode::execute(const foundation::interfaces::NodeContext& context,
                                                                 const foundation::interfaces::NodeParameters& params) {
    try {
        std::string path = params.getParameter("path").asString();
        
        // Configure scanner options
        akao::core::filesystem::scanner::ScanConfig config;
        config.analyze_content = params.hasParameter("analyze_content") ? params.getParameter("analyze_content").asBoolean() : false;
        config.detect_languages = params.hasParameter("detect_languages") ? params.getParameter("detect_languages").asBoolean() : true;
        config.calculate_metrics = params.hasParameter("calculate_metrics") ? params.getParameter("calculate_metrics").asBoolean() : true;
        
        // Set up filters if provided
        if (params.hasParameter("patterns")) {
            if (params.getParameter("patterns").isArray()) {
                for (const auto& pattern : params.getParameter("patterns").asArray()) {
                    config.include_patterns.push_back(pattern.asString());
                }
            }
        }
        
        if (params.hasParameter("exclude_patterns")) {
            if (params.getParameter("exclude_patterns").isArray()) {
                for (const auto& pattern : params.getParameter("exclude_patterns").asArray()) {
                    config.exclude_patterns.push_back(pattern.asString());
                }
            }
        }
        
        // Create and use legacy scanner
        akao::core::filesystem::scanner::DirectoryScanner scanner(config);
        auto scanResult = scanner.scanDirectory(path);
        
        // Convert result to NodeValue
        foundation::types::NodeValue resultData = scanResultToNodeValue(scanResult);
        
        // Add execution metadata
        resultData.setMetadata("scannedPath", foundation::types::NodeValue(path));
        resultData.setMetadata("scannerVersion", foundation::types::NodeValue(version_));
        resultData.setMetadata("analyzeContent", foundation::types::NodeValue(config.analyze_content));
        resultData.setMetadata("totalFiles", foundation::types::NodeValue(static_cast<int64_t>(scanResult.total_files)));
        
        return foundation::types::ExecutionResult::success(resultData);
        
    } catch (const std::exception& e) {
        return foundation::types::ExecutionResult::error("Filesystem scan failed: " + std::string(e.what()));
    }
}

foundation::types::NodeValue FilesystemScannerNode::getParameterSchema() const {
    foundation::types::NodeValue::Object schema;
    
    // Required path parameter
    foundation::types::NodeValue::Object pathParam;
    pathParam["type"] = foundation::types::NodeValue("string");
    pathParam["required"] = foundation::types::NodeValue(true);
    pathParam["description"] = foundation::types::NodeValue("Path to scan (file or directory)");
    schema["path"] = foundation::types::NodeValue(pathParam);
    
    // Optional recursive parameter
    foundation::types::NodeValue::Object recursiveParam;
    recursiveParam["type"] = foundation::types::NodeValue("boolean");
    recursiveParam["required"] = foundation::types::NodeValue(false);
    recursiveParam["default"] = foundation::types::NodeValue(true);
    recursiveParam["description"] = foundation::types::NodeValue("Whether to scan directories recursively");
    schema["recursive"] = foundation::types::NodeValue(recursiveParam);
    
    // Optional content analysis parameter
    foundation::types::NodeValue::Object analyzeParam;
    analyzeParam["type"] = foundation::types::NodeValue("boolean");
    analyzeParam["required"] = foundation::types::NodeValue(false);
    analyzeParam["default"] = foundation::types::NodeValue(false);
    analyzeParam["description"] = foundation::types::NodeValue("Whether to perform content analysis");
    schema["analyze_content"] = foundation::types::NodeValue(analyzeParam);
    
    // Optional extensions filter
    foundation::types::NodeValue::Object extensionsParam;
    extensionsParam["type"] = foundation::types::NodeValue("array");
    extensionsParam["required"] = foundation::types::NodeValue(false);
    extensionsParam["description"] = foundation::types::NodeValue("File extensions to include (e.g., ['.cpp', '.hpp'])");
    schema["extensions"] = foundation::types::NodeValue(extensionsParam);
    
    // Optional patterns filter
    foundation::types::NodeValue::Object patternsParam;
    patternsParam["type"] = foundation::types::NodeValue("array");
    patternsParam["required"] = foundation::types::NodeValue(false);
    patternsParam["description"] = foundation::types::NodeValue("Include patterns for filenames");
    schema["patterns"] = foundation::types::NodeValue(patternsParam);
    
    return foundation::types::NodeValue(schema);
}

// Conversion utilities
foundation::types::NodeValue FilesystemScannerNode::fileInfoToNodeValue(const akao::core::filesystem::scanner::FileInfo& fileInfo) const {
    foundation::types::NodeValue::Object fileObj;
    
    fileObj["path"] = foundation::types::NodeValue(fileInfo.path);
    fileObj["relative_path"] = foundation::types::NodeValue(fileInfo.relative_path);
    fileObj["filename"] = foundation::types::NodeValue(fileInfo.filename);
    fileObj["extension"] = foundation::types::NodeValue(fileInfo.extension);
    fileObj["type"] = foundation::types::NodeValue(fileInfo.type);
    fileObj["size_bytes"] = foundation::types::NodeValue(static_cast<int64_t>(fileInfo.size_bytes));
    fileObj["line_count"] = foundation::types::NodeValue(static_cast<int64_t>(fileInfo.line_count));
    
    // Convert languages array
    foundation::types::NodeValue::Array languagesArray;
    for (const auto& lang : fileInfo.languages) {
        languagesArray.push_back(foundation::types::NodeValue(lang));
    }
    fileObj["languages"] = foundation::types::NodeValue(languagesArray);
    
    // Convert metadata object
    foundation::types::NodeValue::Object metadataObj;
    for (const auto& pair : fileInfo.metadata) {
        metadataObj[pair.first] = foundation::types::NodeValue(pair.second);
    }
    fileObj["metadata"] = foundation::types::NodeValue(metadataObj);
    
    return foundation::types::NodeValue(fileObj);
}

foundation::types::NodeValue FilesystemScannerNode::directoryInfoToNodeValue(const akao::core::filesystem::scanner::DirectoryInfo& dirInfo) const {
    foundation::types::NodeValue::Object dirObj;
    
    dirObj["path"] = foundation::types::NodeValue(dirInfo.path);
    dirObj["name"] = foundation::types::NodeValue(dirInfo.name);
    dirObj["total_files"] = foundation::types::NodeValue(static_cast<int64_t>(dirInfo.total_files));
    dirObj["total_subdirectories"] = foundation::types::NodeValue(static_cast<int64_t>(dirInfo.total_subdirectories));
    
    // Convert file type counts
    foundation::types::NodeValue::Object typeCountsObj;
    for (const auto& pair : dirInfo.file_type_counts) {
        typeCountsObj[pair.first] = foundation::types::NodeValue(static_cast<int64_t>(pair.second));
    }
    dirObj["file_type_counts"] = foundation::types::NodeValue(typeCountsObj);
    
    // Convert files array
    foundation::types::NodeValue::Array filesArray;
    for (const auto& file : dirInfo.files) {
        filesArray.push_back(fileInfoToNodeValue(file));
    }
    dirObj["files"] = foundation::types::NodeValue(filesArray);
    
    return foundation::types::NodeValue(dirObj);
}

foundation::types::NodeValue FilesystemScannerNode::scanResultToNodeValue(const akao::core::filesystem::scanner::DirectoryScanner::ScanResult& result) const {
    foundation::types::NodeValue::Object resultObj;
    
    resultObj["success"] = foundation::types::NodeValue(result.success);
    resultObj["error_message"] = foundation::types::NodeValue(result.error_message);
    resultObj["total_files"] = foundation::types::NodeValue(static_cast<int64_t>(result.total_files));
    resultObj["total_directories"] = foundation::types::NodeValue(static_cast<int64_t>(result.total_directories));
    resultObj["total_size_bytes"] = foundation::types::NodeValue(static_cast<int64_t>(result.total_size_bytes));
    resultObj["total_lines"] = foundation::types::NodeValue(static_cast<int64_t>(result.total_lines));
    resultObj["scan_duration_seconds"] = foundation::types::NodeValue(result.scan_duration_seconds);
    
    // Convert root directory
    resultObj["root_directory"] = directoryInfoToNodeValue(result.root_directory);
    
    // Convert file type counts
    foundation::types::NodeValue::Object fileTypeCountsObj;
    for (const auto& pair : result.file_type_counts) {
        fileTypeCountsObj[pair.first] = foundation::types::NodeValue(static_cast<int64_t>(pair.second));
    }
    resultObj["file_type_counts"] = foundation::types::NodeValue(fileTypeCountsObj);
    
    // Convert extension counts
    foundation::types::NodeValue::Object extensionCountsObj;
    for (const auto& pair : result.extension_counts) {
        extensionCountsObj[pair.first] = foundation::types::NodeValue(static_cast<int64_t>(pair.second));
    }
    resultObj["extension_counts"] = foundation::types::NodeValue(extensionCountsObj);
    
    // Convert language counts
    foundation::types::NodeValue::Object languageCountsObj;
    for (const auto& pair : result.language_counts) {
        languageCountsObj[pair.first] = foundation::types::NodeValue(static_cast<int64_t>(pair.second));
    }
    resultObj["language_counts"] = foundation::types::NodeValue(languageCountsObj);
    
    // Convert warnings array
    foundation::types::NodeValue::Array warningsArray;
    for (const auto& warning : result.warnings) {
        warningsArray.push_back(foundation::types::NodeValue(warning));
    }
    resultObj["warnings"] = foundation::types::NodeValue(warningsArray);
    
    // Convert skipped files array
    foundation::types::NodeValue::Array skippedArray;
    for (const auto& skipped : result.skipped_files) {
        skippedArray.push_back(foundation::types::NodeValue(skipped));
    }
    resultObj["skipped_files"] = foundation::types::NodeValue(skippedArray);
    
    return foundation::types::NodeValue(resultObj);
}

// Static utility methods for direct filesystem scanning
std::vector<std::string> FilesystemScannerNode::getFilesInDirectory(const std::string& path, bool recursive) {
    std::vector<std::string> files;
    
    try {
        if (recursive) {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
                if (entry.is_regular_file()) {
                    files.push_back(entry.path().string());
                }
            }
        } else {
            for (const auto& entry : std::filesystem::directory_iterator(path)) {
                if (entry.is_regular_file()) {
                    files.push_back(entry.path().string());
                }
            }
        }
    } catch (const std::exception&) {
        // Return empty vector on error
    }
    
    return files;
}

std::vector<std::string> FilesystemScannerNode::getCppFiles(const std::string& path, bool recursive) {
    std::vector<std::string> cppFiles;
    auto allFiles = getFilesInDirectory(path, recursive);
    
    for (const auto& file : allFiles) {
        std::string ext = std::filesystem::path(file).extension().string();
        if (ext == ".cpp" || ext == ".hpp" || ext == ".cc" || ext == ".h" || ext == ".cxx" || ext == ".hxx") {
            cppFiles.push_back(file);
        }
    }
    
    return cppFiles;
}

std::vector<std::string> FilesystemScannerNode::getFilesByExtension(const std::string& path, const std::string& extension, bool recursive) {
    std::vector<std::string> matchingFiles;
    auto allFiles = getFilesInDirectory(path, recursive);
    
    for (const auto& file : allFiles) {
        if (std::filesystem::path(file).extension().string() == extension) {
            matchingFiles.push_back(file);
        }
    }
    
    return matchingFiles;
}

std::string FilesystemScannerNode::getFileContent(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool FilesystemScannerNode::fileExists(const std::string& path) {
    return std::filesystem::exists(path);
}

size_t FilesystemScannerNode::getFileSize(const std::string& path) {
    try {
        return std::filesystem::file_size(path);
    } catch (const std::exception&) {
        return 0;
    }
}

std::string FilesystemScannerNode::getFileType(const std::string& path) {
    std::string ext = std::filesystem::path(path).extension().string();
    
    if (ext == ".cpp" || ext == ".hpp" || ext == ".cc" || ext == ".h" || ext == ".cxx" || ext == ".hxx") {
        return "cpp";
    } else if (ext == ".py") {
        return "python";
    } else if (ext == ".js" || ext == ".ts") {
        return "javascript";
    } else if (ext == ".yaml" || ext == ".yml") {
        return "yaml";
    } else if (ext == ".json") {
        return "json";
    } else if (ext == ".md") {
        return "markdown";
    } else if (ext == ".txt") {
        return "text";
    } else {
        return "unknown";
    }
}

} // namespace builtin
} // namespace nodes
} // namespace akao