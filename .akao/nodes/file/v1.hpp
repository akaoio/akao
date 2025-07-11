/**
 * @id: akao:class:nodes:builtin:file:v1
 * @doc: Clean filesystem scanner node providing comprehensive file analysis and directory scanning. Inspired by legacy scanner architecture but implemented as standalone component with zero external dependencies, configurable filtering, content analysis, and performance optimization for large codebases.
 * @specification: Filesystem scanner node with comprehensive analysis capabilities
 * @scope: Builtin filesystem scanning node
 * @timeline: 2025-07-10
 * @rationale: Enable powerful filesystem analysis for node-based validation workflows
 * @methodology: Clean implementation learning from legacy scanner patterns
 * @references: ["akao:interface:core:foundation:interfaces:inode:v1"]
 */

#pragma once

#include "core/foundation/interfaces/inode/v1.hpp"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <filesystem>
#include <functional>

namespace akao {
namespace nodes {
namespace builtin {

/**
 * File information structure
 */
struct FileInfo {
    std::string path;
    std::string relative_path;
    std::string filename;
    std::string extension;
    std::string type;  // "source", "config", "build", "docs", "test", "other"
    size_t size_bytes;
    std::filesystem::file_time_type last_modified;
    
    // Content analysis
    size_t line_count = 0;
    std::vector<std::string> languages;
    std::map<std::string, std::string> metadata;
};

/**
 * Directory scanning configuration
 */
struct ScanConfig {
    bool recursive = true;
    bool analyze_content = false;
    bool detect_languages = true;
    bool include_hidden_files = false;
    bool follow_symlinks = false;
    
    std::vector<std::string> include_patterns;
    std::vector<std::string> exclude_patterns;
    std::vector<std::string> exclude_directories = {".git", ".svn", "node_modules", "__pycache__", "target", "build"};
    std::set<std::string> extension_filter;
    
    size_t max_file_size_bytes = 100 * 1024 * 1024; // 100MB
    size_t max_depth = 50;
};

/**
 * Scanning result
 */
struct ScanResult {
    bool success = false;
    std::string error_message;
    std::string root_path;
    
    std::vector<FileInfo> files;
    size_t total_files = 0;
    size_t total_directories = 0;
    size_t total_size_bytes = 0;
    size_t total_lines = 0;
    
    std::map<std::string, size_t> file_type_counts;
    std::map<std::string, size_t> extension_counts;
    std::map<std::string, size_t> language_counts;
    
    double scan_duration_seconds = 0.0;
    std::vector<std::string> warnings;
    std::vector<std::string> skipped_files;
};

/**
 * Clean filesystem scanner node
 */
class FilesystemScannerNode : public foundation::interfaces::INode {
private:
    std::string nodeId_;
    std::string nodeType_;
    std::string version_;
    std::string description_;
    
    // Core scanning functionality
    ScanResult scanPath(const std::string& path, const ScanConfig& config) const;
    FileInfo analyzeFile(const std::filesystem::path& filePath, const std::string& rootPath, const ScanConfig& config) const;
    bool shouldIncludeFile(const std::filesystem::path& filePath, const ScanConfig& config) const;
    bool shouldIncludeDirectory(const std::filesystem::path& dirPath, const ScanConfig& config) const;
    std::string detectFileType(const std::filesystem::path& filePath) const;
    std::vector<std::string> detectLanguages(const std::filesystem::path& filePath) const;
    size_t countLines(const std::string& content) const;
    
    // Conversion utilities
    foundation::types::NodeValue fileInfoToNodeValue(const FileInfo& fileInfo) const;
    foundation::types::NodeValue scanResultToNodeValue(const ScanResult& result) const;
    ScanConfig parametersToScanConfig(const foundation::interfaces::NodeParameters& params) const;
    
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
    
    // Static utility methods for direct filesystem operations
    static std::vector<std::string> getFilesInDirectory(const std::string& path, bool recursive = true);
    static std::vector<std::string> getCppFiles(const std::string& path, bool recursive = true);
    static std::vector<std::string> getFilesByExtension(const std::string& path, const std::string& extension, bool recursive = true);
    static std::string getFileContent(const std::string& filePath);
    static bool fileExists(const std::string& path);
    static size_t getFileSize(const std::string& path);
    static std::string getFileType(const std::string& path);
    static bool isTextFile(const std::string& path);
    static std::string getRelativePath(const std::string& fullPath, const std::string& basePath);
};

} // namespace builtin
} // namespace nodes
} // namespace akao