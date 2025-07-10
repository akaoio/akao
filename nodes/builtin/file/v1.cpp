/**
 * @id: akao:file:nodes:builtin:file:v1
 * @doc: Implementation of clean filesystem scanner node with comprehensive file analysis capabilities. Provides standalone filesystem scanning inspired by legacy architecture but with zero external dependencies and optimized performance for large codebases.
 * @specification: Filesystem scanner implementation with content analysis
 * @scope: Builtin filesystem scanning node implementation
 * @timeline: 2025-07-10
 * @rationale: Concrete implementation of comprehensive filesystem analysis
 * @methodology: Clean architecture with efficient scanning algorithms
 * @references: ["akao:class:nodes:builtin:file:v1"]
 */

#include "v1.hpp"
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>
#include <chrono>

namespace akao {
namespace nodes {
namespace builtin {

FilesystemScannerNode::FilesystemScannerNode()
    : nodeId_("akao:builtin:filesystem-scanner")
    , nodeType_("filesystem")
    , version_("1.0.0")
    , description_("Comprehensive filesystem scanner with content analysis") {
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
        ScanConfig config = parametersToScanConfig(params);
        
        auto startTime = std::chrono::high_resolution_clock::now();
        ScanResult result = scanPath(path, config);
        auto endTime = std::chrono::high_resolution_clock::now();
        
        result.scan_duration_seconds = std::chrono::duration<double>(endTime - startTime).count();
        
        foundation::types::NodeValue resultData = scanResultToNodeValue(result);
        
        // Add execution metadata
        resultData.setMetadata("scannedPath", foundation::types::NodeValue(path));
        resultData.setMetadata("scannerVersion", foundation::types::NodeValue(version_));
        resultData.setMetadata("recursive", foundation::types::NodeValue(config.recursive));
        resultData.setMetadata("analyzeContent", foundation::types::NodeValue(config.analyze_content));
        resultData.setMetadata("totalFiles", foundation::types::NodeValue(static_cast<int64_t>(result.total_files)));
        
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
    
    return foundation::types::NodeValue(schema);
}

// Core scanning functionality
ScanResult FilesystemScannerNode::scanPath(const std::string& path, const ScanConfig& config) const {
    ScanResult result;
    result.root_path = path;
    
    try {
        std::filesystem::path fsPath(path);
        
        if (std::filesystem::is_regular_file(fsPath)) {
            // Single file
            if (shouldIncludeFile(fsPath, config)) {
                FileInfo fileInfo = analyzeFile(fsPath, path, config);
                result.files.push_back(fileInfo);
                result.total_files = 1;
                result.total_size_bytes = fileInfo.size_bytes;
                result.total_lines = fileInfo.line_count;
                result.extension_counts[fileInfo.extension]++;
                result.file_type_counts[fileInfo.type]++;
                for (const auto& lang : fileInfo.languages) {
                    result.language_counts[lang]++;
                }
            }
        } else if (std::filesystem::is_directory(fsPath)) {
            // Directory scanning
            std::function<void(const std::filesystem::path&, size_t)> scanDirectory;
            scanDirectory = [&](const std::filesystem::path& dirPath, size_t depth) {
                if (depth > config.max_depth) {
                    result.warnings.push_back("Maximum depth reached: " + dirPath.string());
                    return;
                }
                
                try {
                    for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
                        if (entry.is_regular_file()) {
                            if (shouldIncludeFile(entry.path(), config)) {
                                FileInfo fileInfo = analyzeFile(entry.path(), path, config);
                                result.files.push_back(fileInfo);
                                result.total_files++;
                                result.total_size_bytes += fileInfo.size_bytes;
                                result.total_lines += fileInfo.line_count;
                                result.extension_counts[fileInfo.extension]++;
                                result.file_type_counts[fileInfo.type]++;
                                for (const auto& lang : fileInfo.languages) {
                                    result.language_counts[lang]++;
                                }
                            }
                        } else if (entry.is_directory() && config.recursive) {
                            if (shouldIncludeDirectory(entry.path(), config)) {
                                result.total_directories++;
                                scanDirectory(entry.path(), depth + 1);
                            }
                        }
                    }
                } catch (const std::filesystem::filesystem_error& e) {
                    result.warnings.push_back("Error accessing directory " + dirPath.string() + ": " + e.what());
                }
            };
            
            scanDirectory(fsPath, 0);
        }
        
        result.success = true;
    } catch (const std::exception& e) {
        result.success = false;
        result.error_message = e.what();
    }
    
    return result;
}

FileInfo FilesystemScannerNode::analyzeFile(const std::filesystem::path& filePath, const std::string& rootPath, const ScanConfig& config) const {
    FileInfo info;
    info.path = filePath.string();
    info.relative_path = getRelativePath(info.path, rootPath);
    info.filename = filePath.filename().string();
    info.extension = filePath.extension().string();
    info.type = detectFileType(filePath);
    info.last_modified = std::filesystem::last_write_time(filePath);
    
    try {
        info.size_bytes = std::filesystem::file_size(filePath);
    } catch (const std::exception&) {
        info.size_bytes = 0;
    }
    
    if (config.detect_languages) {
        info.languages = detectLanguages(filePath);
    }
    
    if (config.analyze_content && isTextFile(info.path) && info.size_bytes <= config.max_file_size_bytes) {
        try {
            std::string content = getFileContent(info.path);
            info.line_count = countLines(content);
            
            // Additional content analysis could go here
            info.metadata["encoding"] = "UTF-8"; // Simplified
            info.metadata["has_content"] = content.empty() ? "false" : "true";
        } catch (const std::exception&) {
            info.line_count = 0;
        }
    }
    
    return info;
}

bool FilesystemScannerNode::shouldIncludeFile(const std::filesystem::path& filePath, const ScanConfig& config) const {
    std::string filename = filePath.filename().string();
    std::string extension = filePath.extension().string();
    
    // Check hidden files
    if (!config.include_hidden_files && !filename.empty() && filename[0] == '.') {
        return false;
    }
    
    // Check extension filter
    if (!config.extension_filter.empty() && 
        config.extension_filter.find(extension) == config.extension_filter.end()) {
        return false;
    }
    
    // Check include patterns
    if (!config.include_patterns.empty()) {
        bool matches = false;
        for (const auto& pattern : config.include_patterns) {
            if (std::regex_match(filename, std::regex(pattern))) {
                matches = true;
                break;
            }
        }
        if (!matches) return false;
    }
    
    // Check exclude patterns
    for (const auto& pattern : config.exclude_patterns) {
        if (std::regex_match(filename, std::regex(pattern))) {
            return false;
        }
    }
    
    return true;
}

bool FilesystemScannerNode::shouldIncludeDirectory(const std::filesystem::path& dirPath, const ScanConfig& config) const {
    std::string dirname = dirPath.filename().string();
    
    // Check hidden directories
    if (!config.include_hidden_files && !dirname.empty() && dirname[0] == '.') {
        return false;
    }
    
    // Check exclude directories
    for (const auto& excludeDir : config.exclude_directories) {
        if (dirname == excludeDir) {
            return false;
        }
    }
    
    return true;
}

std::string FilesystemScannerNode::detectFileType(const std::filesystem::path& filePath) const {
    std::string ext = filePath.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".cpp" || ext == ".hpp" || ext == ".cc" || ext == ".h" || ext == ".cxx" || ext == ".hxx") {
        return "source";
    } else if (ext == ".py" || ext == ".js" || ext == ".ts" || ext == ".java" || ext == ".c") {
        return "source";
    } else if (ext == ".yaml" || ext == ".yml" || ext == ".json" || ext == ".xml" || ext == ".toml") {
        return "config";
    } else if (ext == ".md" || ext == ".txt" || ext == ".rst" || ext == ".adoc") {
        return "docs";
    } else if (ext == ".test.cpp" || ext == ".test.js" || filePath.string().find("test") != std::string::npos) {
        return "test";
    } else if (ext == ".cmake" || ext == ".mk" || filePath.filename() == "Makefile") {
        return "build";
    } else {
        return "other";
    }
}

std::vector<std::string> FilesystemScannerNode::detectLanguages(const std::filesystem::path& filePath) const {
    std::vector<std::string> languages;
    std::string ext = filePath.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".cpp" || ext == ".hpp" || ext == ".cc" || ext == ".h" || ext == ".cxx" || ext == ".hxx") {
        languages.push_back("cpp");
    } else if (ext == ".py") {
        languages.push_back("python");
    } else if (ext == ".js") {
        languages.push_back("javascript");
    } else if (ext == ".ts") {
        languages.push_back("typescript");
    } else if (ext == ".java") {
        languages.push_back("java");
    } else if (ext == ".c") {
        languages.push_back("c");
    } else if (ext == ".yaml" || ext == ".yml") {
        languages.push_back("yaml");
    } else if (ext == ".json") {
        languages.push_back("json");
    } else if (ext == ".md") {
        languages.push_back("markdown");
    }
    
    return languages;
}

size_t FilesystemScannerNode::countLines(const std::string& content) const {
    if (content.empty()) return 0;
    return std::count(content.begin(), content.end(), '\n') + 1;
}

// Conversion utilities
foundation::types::NodeValue FilesystemScannerNode::fileInfoToNodeValue(const FileInfo& fileInfo) const {
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

foundation::types::NodeValue FilesystemScannerNode::scanResultToNodeValue(const ScanResult& result) const {
    foundation::types::NodeValue::Object resultObj;
    
    resultObj["success"] = foundation::types::NodeValue(result.success);
    resultObj["error_message"] = foundation::types::NodeValue(result.error_message);
    resultObj["root_path"] = foundation::types::NodeValue(result.root_path);
    resultObj["total_files"] = foundation::types::NodeValue(static_cast<int64_t>(result.total_files));
    resultObj["total_directories"] = foundation::types::NodeValue(static_cast<int64_t>(result.total_directories));
    resultObj["total_size_bytes"] = foundation::types::NodeValue(static_cast<int64_t>(result.total_size_bytes));
    resultObj["total_lines"] = foundation::types::NodeValue(static_cast<int64_t>(result.total_lines));
    resultObj["scan_duration_seconds"] = foundation::types::NodeValue(result.scan_duration_seconds);
    
    // Convert files array
    foundation::types::NodeValue::Array filesArray;
    for (const auto& file : result.files) {
        filesArray.push_back(fileInfoToNodeValue(file));
    }
    resultObj["files"] = foundation::types::NodeValue(filesArray);
    
    // Convert counts objects
    foundation::types::NodeValue::Object fileTypeCountsObj;
    for (const auto& pair : result.file_type_counts) {
        fileTypeCountsObj[pair.first] = foundation::types::NodeValue(static_cast<int64_t>(pair.second));
    }
    resultObj["file_type_counts"] = foundation::types::NodeValue(fileTypeCountsObj);
    
    foundation::types::NodeValue::Object extensionCountsObj;
    for (const auto& pair : result.extension_counts) {
        extensionCountsObj[pair.first] = foundation::types::NodeValue(static_cast<int64_t>(pair.second));
    }
    resultObj["extension_counts"] = foundation::types::NodeValue(extensionCountsObj);
    
    foundation::types::NodeValue::Object languageCountsObj;
    for (const auto& pair : result.language_counts) {
        languageCountsObj[pair.first] = foundation::types::NodeValue(static_cast<int64_t>(pair.second));
    }
    resultObj["language_counts"] = foundation::types::NodeValue(languageCountsObj);
    
    // Convert warnings and skipped files
    foundation::types::NodeValue::Array warningsArray;
    for (const auto& warning : result.warnings) {
        warningsArray.push_back(foundation::types::NodeValue(warning));
    }
    resultObj["warnings"] = foundation::types::NodeValue(warningsArray);
    
    foundation::types::NodeValue::Array skippedArray;
    for (const auto& skipped : result.skipped_files) {
        skippedArray.push_back(foundation::types::NodeValue(skipped));
    }
    resultObj["skipped_files"] = foundation::types::NodeValue(skippedArray);
    
    return foundation::types::NodeValue(resultObj);
}

ScanConfig FilesystemScannerNode::parametersToScanConfig(const foundation::interfaces::NodeParameters& params) const {
    ScanConfig config;
    
    if (params.hasParameter("recursive")) {
        config.recursive = params.getParameter("recursive").asBoolean();
    }
    
    if (params.hasParameter("analyze_content")) {
        config.analyze_content = params.getParameter("analyze_content").asBoolean();
    }
    
    if (params.hasParameter("detect_languages")) {
        config.detect_languages = params.getParameter("detect_languages").asBoolean();
    }
    
    if (params.hasParameter("include_hidden_files")) {
        config.include_hidden_files = params.getParameter("include_hidden_files").asBoolean();
    }
    
    if (params.hasParameter("extensions")) {
        if (params.getParameter("extensions").isArray()) {
            for (const auto& ext : params.getParameter("extensions").asArray()) {
                config.extension_filter.insert(ext.asString());
            }
        }
    }
    
    if (params.hasParameter("include_patterns")) {
        if (params.getParameter("include_patterns").isArray()) {
            for (const auto& pattern : params.getParameter("include_patterns").asArray()) {
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
    
    return config;
}

// Static utility methods
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
    FilesystemScannerNode scanner;
    return scanner.detectFileType(std::filesystem::path(path));
}

bool FilesystemScannerNode::isTextFile(const std::string& path) {
    std::string ext = std::filesystem::path(path).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    std::vector<std::string> textExtensions = {
        ".txt", ".md", ".cpp", ".hpp", ".h", ".c", ".py", ".js", ".ts", 
        ".java", ".yaml", ".yml", ".json", ".xml", ".html", ".css", 
        ".sh", ".bat", ".cmake", ".make", ".rst", ".adoc"
    };
    
    return std::find(textExtensions.begin(), textExtensions.end(), ext) != textExtensions.end();
}

std::string FilesystemScannerNode::getRelativePath(const std::string& fullPath, const std::string& basePath) {
    try {
        std::filesystem::path full(fullPath);
        std::filesystem::path base(basePath);
        return std::filesystem::relative(full, base).string();
    } catch (const std::exception&) {
        return fullPath;
    }
}

} // namespace builtin
} // namespace nodes
} // namespace akao