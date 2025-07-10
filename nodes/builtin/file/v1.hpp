/**
 * @id: akao:class:nodes:builtin:file:v1
 * @doc: Filesystem scanner node using legacy directory scanner for comprehensive file analysis. Adapts the proven legacy filesystem scanner for node-based architecture while preserving all performance optimizations and advanced features including content analysis, language detection, and project pattern recognition.
 * @specification: Filesystem scanner node using legacy directory scanner
 * @scope: Builtin filesystem scanning node
 * @timeline: 2025-07-10
 * @rationale: Leverage existing high-performance filesystem scanner for node workflows
 * @methodology: Adapter pattern converting legacy scanner results to NodeValue format
 * @references: ["akao:interface:core:foundation:interfaces:inode:v1", "akao:file:core:filesystem:scanner_directory_scanner:v1"]
 */

#pragma once

#include "core/foundation/interfaces/inode/v1.hpp"
#include "legacy/core/filesystem/scanner/v1.hpp"
#include <string>
#include <vector>
#include <map>

namespace akao {
namespace nodes {
namespace builtin {

/**
 * Filesystem scanner node using legacy directory scanner
 */
class FilesystemScannerNode : public foundation::interfaces::INode {
private:
    std::string nodeId_;
    std::string nodeType_;
    std::string version_;
    std::string description_;
    
    // Conversion utilities
    foundation::types::NodeValue fileInfoToNodeValue(const akao::core::filesystem::scanner::FileInfo& fileInfo) const;
    foundation::types::NodeValue directoryInfoToNodeValue(const akao::core::filesystem::scanner::DirectoryInfo& dirInfo) const;
    foundation::types::NodeValue scanResultToNodeValue(const akao::core::filesystem::scanner::DirectoryScanner::ScanResult& result) const;
    
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
    
    // Static utility methods for direct filesystem scanning
    static std::vector<std::string> getFilesInDirectory(const std::string& path, bool recursive = true);
    static std::vector<std::string> getCppFiles(const std::string& path, bool recursive = true);
    static std::vector<std::string> getFilesByExtension(const std::string& path, const std::string& extension, bool recursive = true);
    static std::string getFileContent(const std::string& filePath);
    static bool fileExists(const std::string& path);
    static size_t getFileSize(const std::string& path);
    static std::string getFileType(const std::string& path);
};

} // namespace builtin
} // namespace nodes
} // namespace akao