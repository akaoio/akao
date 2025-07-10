/**
 * @id: akao:file:nodes:builtin:file:v1
 * @doc: Implementation of filesystem scanner node providing comprehensive file discovery and metadata extraction capabilities. Features high-performance recursive scanning, pattern matching, and configurable filtering for workflow automation.
 * @specification: Filesystem scanner node implementation with performance optimization
 * @scope: Built-in filesystem operations node implementation
 * @timeline: 2025-07-10
 * @rationale: Concrete implementation of filesystem scanning for validation workflows
 * @methodology: Optimized filesystem traversal with comprehensive metadata collection
 * @references: ["akao:class:nodes:builtin:file:v1"]
 */

#include "v1.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <sys/stat.h>

namespace akao {
namespace nodes {
namespace builtin {

FilesystemScannerNode::FilesystemScannerNode() 
    : nodeId_("akao:builtin:filesystem-scanner")
    , nodeType_("file")
    , version_("1.0.0")
    , description_("High-performance filesystem scanner with pattern matching and metadata extraction") {
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
    
    // Check required path parameter
    if (!params.hasParameter("path")) {
        result.addError("Missing required parameter 'path'");
        return result;
    }
    
    std::string path = params.getParameter("path").asString();
    if (path.empty()) {
        result.addError("Parameter 'path' cannot be empty");
        return result;
    }
    
    // Check if path exists
    if (!std::filesystem::exists(path)) {
        result.addError("Path does not exist: " + path);
        return result;
    }
    
    // Validate optional parameters
    if (params.hasParameter("maxDepth")) {
        auto depth = params.getParameter("maxDepth");
        if (!depth.isInteger() || depth.asInteger() < 0) {
            result.addError("Parameter 'maxDepth' must be a non-negative integer");
        }
    }
    
    if (params.hasParameter("minSize")) {
        auto size = params.getParameter("minSize");
        if (!size.isInteger() || size.asInteger() < 0) {
            result.addError("Parameter 'minSize' must be a non-negative integer");
        }
    }
    
    if (params.hasParameter("maxSize")) {
        auto size = params.getParameter("maxSize");
        if (!size.isInteger() || size.asInteger() < 0) {
            result.addError("Parameter 'maxSize' must be a non-negative integer");
        }
    }
    
    return result;
}

foundation::types::ExecutionResult FilesystemScannerNode::execute(const foundation::interfaces::NodeContext& context, 
                                                                 const foundation::interfaces::NodeParameters& params) {
    try {
        // Parse configuration from parameters
        ScanConfig config = parseConfiguration(params);
        
        // Perform filesystem scan
        std::vector<FileInfo> files = scanDirectory(config);
        
        // Convert results to NodeValue array
        foundation::types::NodeValue::Array results;
        results.reserve(files.size());
        
        for (const auto& file : files) {
            results.push_back(convertFileInfoToNodeValue(file));
        }
        
        foundation::types::NodeValue resultData(results);
        
        // Add metadata
        resultData.setMetadata("fileCount", foundation::types::NodeValue(static_cast<int64_t>(files.size())));
        resultData.setMetadata("scanPath", foundation::types::NodeValue(config.rootPath));
        resultData.setMetadata("recursive", foundation::types::NodeValue(config.recursive));
        
        return foundation::types::ExecutionResult::success(resultData);
        
    } catch (const std::exception& e) {
        return foundation::types::ExecutionResult::error("Filesystem scan failed: " + std::string(e.what()));
    }
}

foundation::types::NodeValue FilesystemScannerNode::getParameterSchema() const {
    foundation::types::NodeValue::Object schema;
    
    // Required parameters
    foundation::types::NodeValue::Object pathParam;
    pathParam["type"] = foundation::types::NodeValue("string");
    pathParam["required"] = foundation::types::NodeValue(true);
    pathParam["description"] = foundation::types::NodeValue("Root path to scan");
    schema["path"] = foundation::types::NodeValue(pathParam);
    
    // Optional parameters
    foundation::types::NodeValue::Object recursiveParam;
    recursiveParam["type"] = foundation::types::NodeValue("boolean");
    recursiveParam["required"] = foundation::types::NodeValue(false);
    recursiveParam["default"] = foundation::types::NodeValue(true);
    recursiveParam["description"] = foundation::types::NodeValue("Enable recursive directory scanning");
    schema["recursive"] = foundation::types::NodeValue(recursiveParam);
    
    foundation::types::NodeValue::Object includeFilesParam;
    includeFilesParam["type"] = foundation::types::NodeValue("boolean");
    includeFilesParam["required"] = foundation::types::NodeValue(false);
    includeFilesParam["default"] = foundation::types::NodeValue(true);
    includeFilesParam["description"] = foundation::types::NodeValue("Include regular files in results");
    schema["includeFiles"] = foundation::types::NodeValue(includeFilesParam);
    
    foundation::types::NodeValue::Object includeDirsParam;
    includeDirsParam["type"] = foundation::types::NodeValue("boolean");
    includeDirsParam["required"] = foundation::types::NodeValue(false);
    includeDirsParam["default"] = foundation::types::NodeValue(true);
    includeDirsParam["description"] = foundation::types::NodeValue("Include directories in results");
    schema["includeDirectories"] = foundation::types::NodeValue(includeDirsParam);
    
    foundation::types::NodeValue::Object extensionsParam;
    extensionsParam["type"] = foundation::types::NodeValue("array");
    extensionsParam["required"] = foundation::types::NodeValue(false);
    extensionsParam["description"] = foundation::types::NodeValue("Filter by file extensions (e.g., [\".cpp\", \".hpp\"])");
    schema["extensions"] = foundation::types::NodeValue(extensionsParam);
    
    foundation::types::NodeValue::Object includePatternsParam;
    includePatternsParam["type"] = foundation::types::NodeValue("array");
    includePatternsParam["required"] = foundation::types::NodeValue(false);
    includePatternsParam["description"] = foundation::types::NodeValue("Include patterns (regex)");
    schema["includePatterns"] = foundation::types::NodeValue(includePatternsParam);
    
    foundation::types::NodeValue::Object excludePatternsParam;
    excludePatternsParam["type"] = foundation::types::NodeValue("array");
    excludePatternsParam["required"] = foundation::types::NodeValue(false);
    excludePatternsParam["description"] = foundation::types::NodeValue("Exclude patterns (regex)");
    schema["excludePatterns"] = foundation::types::NodeValue(excludePatternsParam);
    
    foundation::types::NodeValue::Object maxDepthParam;
    maxDepthParam["type"] = foundation::types::NodeValue("integer");
    maxDepthParam["required"] = foundation::types::NodeValue(false);
    maxDepthParam["description"] = foundation::types::NodeValue("Maximum directory depth to scan");
    schema["maxDepth"] = foundation::types::NodeValue(maxDepthParam);
    
    foundation::types::NodeValue::Object minSizeParam;
    minSizeParam["type"] = foundation::types::NodeValue("integer");
    minSizeParam["required"] = foundation::types::NodeValue(false);
    minSizeParam["description"] = foundation::types::NodeValue("Minimum file size in bytes");
    schema["minSize"] = foundation::types::NodeValue(minSizeParam);
    
    foundation::types::NodeValue::Object maxSizeParam;
    maxSizeParam["type"] = foundation::types::NodeValue("integer");
    maxSizeParam["required"] = foundation::types::NodeValue(false);
    maxSizeParam["description"] = foundation::types::NodeValue("Maximum file size in bytes");
    schema["maxSize"] = foundation::types::NodeValue(maxSizeParam);
    
    return foundation::types::NodeValue(schema);
}

ScanConfig FilesystemScannerNode::parseConfiguration(const foundation::interfaces::NodeParameters& params) const {
    ScanConfig config;
    
    // Required path
    config.rootPath = params.getParameter("path").asString();
    
    // Optional parameters with defaults
    if (params.hasParameter("recursive")) {
        config.recursive = params.getParameter("recursive").asBoolean();
    }
    
    if (params.hasParameter("includeFiles")) {
        config.includeFiles = params.getParameter("includeFiles").asBoolean();
    }
    
    if (params.hasParameter("includeDirectories")) {
        config.includeDirectories = params.getParameter("includeDirectories").asBoolean();
    }
    
    if (params.hasParameter("followSymlinks")) {
        config.followSymlinks = params.getParameter("followSymlinks").asBoolean();
    }
    
    if (params.hasParameter("extensions")) {
        auto extensions = params.getParameter("extensions");
        if (extensions.isArray()) {
            for (const auto& ext : extensions.asArray()) {
                config.extensions.push_back(ext.asString());
            }
        }
    }
    
    if (params.hasParameter("includePatterns")) {
        auto patterns = params.getParameter("includePatterns");
        if (patterns.isArray()) {
            for (const auto& pattern : patterns.asArray()) {
                config.includePatterns.push_back(pattern.asString());
            }
        }
    }
    
    if (params.hasParameter("excludePatterns")) {
        auto patterns = params.getParameter("excludePatterns");
        if (patterns.isArray()) {
            for (const auto& pattern : patterns.asArray()) {
                config.excludePatterns.push_back(pattern.asString());
            }
        }
    }
    
    if (params.hasParameter("maxDepth")) {
        config.maxDepth = static_cast<size_t>(params.getParameter("maxDepth").asInteger());
    }
    
    if (params.hasParameter("minSize")) {
        config.minSize = static_cast<size_t>(params.getParameter("minSize").asInteger());
    }
    
    if (params.hasParameter("maxSize")) {
        config.maxSize = static_cast<size_t>(params.getParameter("maxSize").asInteger());
    }
    
    return config;
}

std::vector<FileInfo> FilesystemScannerNode::scanDirectory(const ScanConfig& config) const {
    std::vector<FileInfo> results;
    
    try {
        std::filesystem::recursive_directory_iterator iter(config.rootPath);
        std::filesystem::recursive_directory_iterator end;
        
        if (!config.recursive) {
            // Use regular directory iterator for non-recursive scan
            std::filesystem::directory_iterator regularIter(config.rootPath);
            std::filesystem::directory_iterator regularEnd;
            
            for (auto it = regularIter; it != regularEnd; ++it) {
                if (shouldIncludeFile(*it, config)) {
                    results.push_back(createFileInfo(*it));
                }
            }
        } else {
            // Use recursive iterator
            for (auto it = iter; it != end; ++it) {
                // Check depth limit
                if (it.depth() > config.maxDepth) {
                    it.disable_recursion_pending();
                    continue;
                }
                
                // Handle symlinks
                if (!config.followSymlinks && it->is_symlink()) {
                    it.disable_recursion_pending();
                    continue;
                }
                
                if (shouldIncludeFile(*it, config)) {
                    results.push_back(createFileInfo(*it));
                }
            }
        }
        
    } catch (const std::filesystem::filesystem_error& e) {
        throw std::runtime_error("Filesystem error: " + std::string(e.what()));
    }
    
    return results;
}

bool FilesystemScannerNode::shouldIncludeFile(const std::filesystem::directory_entry& entry, const ScanConfig& config) const {
    try {
        // Check file type filters
        if (entry.is_directory() && !config.includeDirectories) {
            return false;
        }
        
        if (entry.is_regular_file() && !config.includeFiles) {
            return false;
        }
        
        // Check size filters for regular files
        if (entry.is_regular_file()) {
            auto size = entry.file_size();
            if (size < config.minSize || size > config.maxSize) {
                return false;
            }
        }
        
        std::string path = entry.path().string();
        std::string filename = entry.path().filename().string();
        
        // Check extension filter
        if (!config.extensions.empty() && entry.is_regular_file()) {
            std::string ext = entry.path().extension().string();
            bool hasMatchingExtension = std::find(config.extensions.begin(), config.extensions.end(), ext) != config.extensions.end();
            if (!hasMatchingExtension) {
                return false;
            }
        }
        
        // Check exclude patterns
        if (!config.excludePatterns.empty()) {
            if (matchesPatterns(path, config.excludePatterns)) {
                return false;
            }
        }
        
        // Check include patterns (if specified, at least one must match)
        if (!config.includePatterns.empty()) {
            if (!matchesPatterns(path, config.includePatterns)) {
                return false;
            }
        }
        
        return true;
        
    } catch (const std::filesystem::filesystem_error&) {
        return false;
    }
}

bool FilesystemScannerNode::matchesPatterns(const std::string& path, const std::vector<std::string>& patterns) const {
    for (const auto& pattern : patterns) {
        try {
            std::regex regex(pattern);
            if (std::regex_search(path, regex)) {
                return true;
            }
        } catch (const std::regex_error&) {
            // Invalid regex pattern, skip
            continue;
        }
    }
    return false;
}

FileInfo FilesystemScannerNode::createFileInfo(const std::filesystem::directory_entry& entry) const {
    FileInfo info;
    
    try {
        info.path = entry.path().string();
        info.name = entry.path().filename().string();
        info.extension = entry.path().extension().string();
        info.directory = entry.path().parent_path().string();
        info.isDirectory = entry.is_directory();
        info.isRegularFile = entry.is_regular_file();
        info.isSymlink = entry.is_symlink();
        
        if (entry.is_regular_file()) {
            info.size = entry.file_size();
        }
        
        // Get file times
        auto lastWrite = entry.last_write_time();
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            lastWrite - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
        info.lastModified = sctp;
        info.created = sctp; // std::filesystem doesn't provide creation time directly
        
    } catch (const std::filesystem::filesystem_error&) {
        // Fill with defaults if error occurs
    }
    
    return info;
}

foundation::types::NodeValue FilesystemScannerNode::convertFileInfoToNodeValue(const FileInfo& info) const {
    foundation::types::NodeValue::Object obj;
    
    obj["path"] = foundation::types::NodeValue(info.path);
    obj["name"] = foundation::types::NodeValue(info.name);
    obj["extension"] = foundation::types::NodeValue(info.extension);
    obj["directory"] = foundation::types::NodeValue(info.directory);
    obj["size"] = foundation::types::NodeValue(static_cast<int64_t>(info.size));
    obj["isDirectory"] = foundation::types::NodeValue(info.isDirectory);
    obj["isRegularFile"] = foundation::types::NodeValue(info.isRegularFile);
    obj["isSymlink"] = foundation::types::NodeValue(info.isSymlink);
    
    // Convert time to timestamp
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(info.lastModified.time_since_epoch()).count();
    obj["lastModified"] = foundation::types::NodeValue(static_cast<int64_t>(timestamp));
    
    return foundation::types::NodeValue(obj);
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
    } catch (const std::filesystem::filesystem_error&) {
        // Return empty vector on error
    }
    
    return files;
}

std::vector<std::string> FilesystemScannerNode::getFilesByExtension(const std::string& path, const std::string& extension, bool recursive) {
    std::vector<std::string> files;
    
    try {
        if (recursive) {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
                if (entry.is_regular_file() && entry.path().extension() == extension) {
                    files.push_back(entry.path().string());
                }
            }
        } else {
            for (const auto& entry : std::filesystem::directory_iterator(path)) {
                if (entry.is_regular_file() && entry.path().extension() == extension) {
                    files.push_back(entry.path().string());
                }
            }
        }
    } catch (const std::filesystem::filesystem_error&) {
        // Return empty vector on error
    }
    
    return files;
}

std::vector<std::string> FilesystemScannerNode::getCppFiles(const std::string& path, bool recursive) {
    std::vector<std::string> files;
    std::vector<std::string> cppFiles = getFilesByExtension(path, ".cpp", recursive);
    std::vector<std::string> ccFiles = getFilesByExtension(path, ".cc", recursive);
    std::vector<std::string> cxxFiles = getFilesByExtension(path, ".cxx", recursive);
    
    files.insert(files.end(), cppFiles.begin(), cppFiles.end());
    files.insert(files.end(), ccFiles.begin(), ccFiles.end());
    files.insert(files.end(), cxxFiles.begin(), cxxFiles.end());
    
    return files;
}

std::vector<std::string> FilesystemScannerNode::getHeaderFiles(const std::string& path, bool recursive) {
    std::vector<std::string> files;
    std::vector<std::string> hppFiles = getFilesByExtension(path, ".hpp", recursive);
    std::vector<std::string> hFiles = getFilesByExtension(path, ".h", recursive);
    std::vector<std::string> hxxFiles = getFilesByExtension(path, ".hxx", recursive);
    
    files.insert(files.end(), hppFiles.begin(), hppFiles.end());
    files.insert(files.end(), hFiles.begin(), hFiles.end());
    files.insert(files.end(), hxxFiles.begin(), hxxFiles.end());
    
    return files;
}

bool FilesystemScannerNode::fileExists(const std::string& path) {
    return std::filesystem::exists(path);
}

size_t FilesystemScannerNode::getFileSize(const std::string& path) {
    try {
        return std::filesystem::file_size(path);
    } catch (const std::filesystem::filesystem_error&) {
        return 0;
    }
}

std::string FilesystemScannerNode::getFileContent(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

} // namespace builtin
} // namespace nodes
} // namespace akao