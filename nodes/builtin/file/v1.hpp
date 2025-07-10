/**
 * @id: akao:class:nodes:builtin:file:v1
 * @doc: Filesystem scanner node for comprehensive file discovery and metadata extraction. Provides recursive directory scanning, pattern matching, file type detection, and metadata collection for workflow automation. Supports filtering by extensions, size, dates, and custom patterns.
 * @specification: Filesystem scanner node with pattern matching and metadata extraction
 * @scope: Built-in filesystem operations node
 * @timeline: 2025-07-10
 * @rationale: Enable powerful filesystem operations for validation and analysis workflows
 * @methodology: High-performance scanning with configurable filters and comprehensive metadata
 * @references: ["akao:interface:core:foundation:interfaces:inode:v1"]
 */

#pragma once

#include "core/foundation/interfaces/inode/v1.hpp"
#include <string>
#include <vector>
#include <regex>
#include <chrono>
#include <filesystem>

namespace akao {
namespace nodes {
namespace builtin {

/**
 * File information structure containing comprehensive metadata
 */
struct FileInfo {
    std::string path;
    std::string name;
    std::string extension;
    std::string directory;
    size_t size;
    std::chrono::system_clock::time_point lastModified;
    std::chrono::system_clock::time_point created;
    bool isDirectory;
    bool isRegularFile;
    bool isSymlink;
    
    FileInfo() : size(0), isDirectory(false), isRegularFile(false), isSymlink(false) {}
};

/**
 * Filesystem scanner configuration
 */
struct ScanConfig {
    std::string rootPath;
    bool recursive = true;
    bool includeDirectories = true;
    bool includeFiles = true;
    bool followSymlinks = false;
    std::vector<std::string> includePatterns;
    std::vector<std::string> excludePatterns;
    std::vector<std::string> extensions;
    size_t maxDepth = SIZE_MAX;
    size_t minSize = 0;
    size_t maxSize = SIZE_MAX;
    bool collectMetadata = true;
};

/**
 * High-performance filesystem scanner node for file discovery and metadata extraction
 */
class FilesystemScannerNode : public foundation::interfaces::INode {
private:
    std::string nodeId_;
    std::string nodeType_;
    std::string version_;
    std::string description_;
    
    // Internal scanning methods
    std::vector<FileInfo> scanDirectory(const ScanConfig& config) const;
    bool matchesPatterns(const std::string& path, const std::vector<std::string>& patterns) const;
    bool shouldIncludeFile(const std::filesystem::directory_entry& entry, const ScanConfig& config) const;
    FileInfo createFileInfo(const std::filesystem::directory_entry& entry) const;
    foundation::types::NodeValue convertFileInfoToNodeValue(const FileInfo& info) const;
    ScanConfig parseConfiguration(const foundation::interfaces::NodeParameters& params) const;
    
public:
    FilesystemScannerNode();
    ~FilesystemScannerNode() = default;
    
    // INode interface implementation
    std::string getNodeId() const override;
    std::string getNodeType() const override;
    std::string getVersion() const override;
    std::string getDescription() const override;
    
    foundation::interfaces::ValidationResult validate(const foundation::interfaces::NodeParameters& params) const override;
    foundation::types::ExecutionResult execute(const foundation::interfaces::NodeContext& context, 
                                             const foundation::interfaces::NodeParameters& params) override;
    foundation::types::NodeValue getParameterSchema() const override;
    
    // Utility methods for direct filesystem operations
    static std::vector<std::string> getFilesInDirectory(const std::string& path, bool recursive = true);
    static std::vector<std::string> getFilesByExtension(const std::string& path, const std::string& extension, bool recursive = true);
    static std::vector<std::string> getCppFiles(const std::string& path, bool recursive = true);
    static std::vector<std::string> getHeaderFiles(const std::string& path, bool recursive = true);
    static bool fileExists(const std::string& path);
    static size_t getFileSize(const std::string& path);
    static std::string getFileContent(const std::string& path);
};

} // namespace builtin
} // namespace nodes
} // namespace akao