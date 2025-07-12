/**
 * @id: akao:node:filesystem:scanner:independent:v1
 * @doc: Independent filesystem scanner node with YAML-RPC communication. Completely self-contained with no core dependencies, providing comprehensive file analysis and directory scanning capabilities. Communicates via YAML-RPC protocol over Unix Domain Sockets, enabling true plugin architecture where the node runs as an external process managed by the core orchestrator.
 * @specification: Independent filesystem scanner with YAML-RPC communication
 * @scope: External filesystem scanning node
 * @timeline: 2025-07-11
 * @rationale: Demonstrate true core-node independence with protocol-based communication
 * @methodology: Self-contained implementation with YAML-RPC server and filesystem analysis
 * @references: ["akao:protocol:yamlrpc:v1"]
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <filesystem>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <functional>
#include <variant>

namespace akao {
namespace nodes {
namespace file {

/**
 * Simple YAML node for internal use (no core dependencies)
 */
class SimpleYamlNode {
public:
    enum class Type { STRING, INTEGER, FLOAT, BOOLEAN, NULL_VALUE, SEQUENCE, MAPPING };
    
private:
    Type type_;
    std::variant<
        std::string,
        int,
        double,
        bool,
        std::nullptr_t,
        std::vector<std::shared_ptr<SimpleYamlNode>>,
        std::map<std::string, std::shared_ptr<SimpleYamlNode>>
    > value_;
    
public:
    SimpleYamlNode(Type type = Type::NULL_VALUE);
    
    // Factory methods
    static std::shared_ptr<SimpleYamlNode> createString(const std::string& value);
    static std::shared_ptr<SimpleYamlNode> createInteger(int value);
    static std::shared_ptr<SimpleYamlNode> createFloat(double value);
    static std::shared_ptr<SimpleYamlNode> createBoolean(bool value);
    static std::shared_ptr<SimpleYamlNode> createNull();
    static std::shared_ptr<SimpleYamlNode> createSequence();
    static std::shared_ptr<SimpleYamlNode> createMapping();
    
    // Accessors
    Type getType() const { return type_; }
    std::string asString() const;
    int asInteger() const;
    double asFloat() const;
    bool asBoolean() const;
    
    // Container operations
    void addToSequence(std::shared_ptr<SimpleYamlNode> node);
    void setMapping(const std::string& key, std::shared_ptr<SimpleYamlNode> node);
    std::shared_ptr<SimpleYamlNode> getMapping(const std::string& key) const;
    bool hasKey(const std::string& key) const;
    
    // Serialization
    std::string toYaml(int indent = 0) const;
    static std::shared_ptr<SimpleYamlNode> fromYaml(const std::string& yaml);
};

/**
 * File information structure
 */
struct FileInfo {
    std::string path;
    std::string name;
    std::string extension;
    std::string type;        // "file", "directory", "symlink"
    size_t size_bytes;
    std::chrono::system_clock::time_point modified_time;
    std::chrono::system_clock::time_point created_time;
    bool is_hidden;
    bool is_executable;
    bool is_readable;
    bool is_writable;
    
    // Content analysis (optional)
    std::string mime_type;
    std::string encoding;
    size_t line_count;
    bool is_binary;
    
    // Convert to YAML
    std::shared_ptr<SimpleYamlNode> toYaml() const;
};

/**
 * Scanning statistics
 */
struct ScanStats {
    size_t total_files;
    size_t total_directories;
    size_t total_size_bytes;
    size_t hidden_files;
    size_t binary_files;
    size_t text_files;
    std::chrono::duration<double> scan_duration;
    std::map<std::string, size_t> extensions_count;
    std::map<std::string, size_t> types_count;
    
    // Convert to YAML
    std::shared_ptr<SimpleYamlNode> toYaml() const;
};

/**
 * Scanning configuration
 */
struct ScanConfig {
    std::string path;
    bool recursive = false;
    std::vector<std::string> extensions;
    bool include_hidden = false;
    int max_depth = 10;
    bool content_analysis = false;
    
    // Filters
    std::function<bool(const std::filesystem::path&)> path_filter;
    std::function<bool(const FileInfo&)> file_filter;
    
    // Parse from YAML
    static ScanConfig fromYaml(const std::shared_ptr<SimpleYamlNode>& yaml);
};

/**
 * Filesystem scanner implementation
 */
class FilesystemScanner {
private:
    ScanConfig config_;
    ScanStats stats_;
    std::atomic<bool> scanning_;
    std::mutex results_mutex_;
    
    std::vector<FileInfo> files_;
    std::vector<FileInfo> directories_;
    
public:
    FilesystemScanner();
    
    // Configuration
    void setConfig(const ScanConfig& config);
    const ScanConfig& getConfig() const { return config_; }
    
    // Scanning
    bool scan();
    bool scanAsync();
    void stop();
    bool isScanning() const { return scanning_; }
    
    // Results
    const std::vector<FileInfo>& getFiles() const;
    const std::vector<FileInfo>& getDirectories() const;
    const ScanStats& getStats() const { return stats_; }
    
    // YAML export
    std::shared_ptr<SimpleYamlNode> getResultsAsYaml() const;
    
private:
    void scanDirectory(const std::filesystem::path& dir, int depth = 0);
    FileInfo createFileInfo(const std::filesystem::path& path);
    void performContentAnalysis(FileInfo& info);
    bool shouldIncludeFile(const std::filesystem::path& path);
    void updateStats();
    
    // Utility methods
    std::string getFileType(const std::filesystem::path& path);
    std::string getMimeType(const std::filesystem::path& path);
    std::string getEncoding(const std::filesystem::path& path);
    size_t countLines(const std::filesystem::path& path);
    bool isBinaryFile(const std::filesystem::path& path);
};

/**
 * YAML-RPC server for the file scanner node
 */
class FileNodeServer {
private:
    std::string socket_path_;
    int server_socket_;
    std::atomic<bool> running_;
    std::thread server_thread_;
    
    FilesystemScanner scanner_;
    
    // Node info
    std::string node_id_;
    std::string node_name_;
    std::string version_;
    
public:
    FileNodeServer(const std::string& socket_path);
    ~FileNodeServer();
    
    // Server lifecycle
    bool start();
    void stop();
    bool isRunning() const { return running_; }
    
    // Node configuration
    void setNodeInfo(const std::string& id, const std::string& name, const std::string& version);
    
private:
    void serverLoop();
    void handleClient(int client_socket);
    std::string processRequest(const std::string& request);
    
    // YAML-RPC method handlers
    std::shared_ptr<SimpleYamlNode> handleNodeInfo();
    std::shared_ptr<SimpleYamlNode> handleNodeValidate(const std::shared_ptr<SimpleYamlNode>& params);
    std::shared_ptr<SimpleYamlNode> handleNodeExecute(const std::shared_ptr<SimpleYamlNode>& params);
    std::shared_ptr<SimpleYamlNode> handleNodeHealth();
    std::shared_ptr<SimpleYamlNode> handleNodeShutdown();
    
    // Utility methods
    bool setupSocket();
    void cleanup();
    std::string receiveMessage(int client_socket);
    bool sendMessage(int client_socket, const std::string& message);
    
    // YAML-RPC response creation
    std::shared_ptr<SimpleYamlNode> createResponse(const std::shared_ptr<SimpleYamlNode>& result, const std::string& id);
    std::shared_ptr<SimpleYamlNode> createError(int code, const std::string& message, const std::string& id);
};

} // namespace file
} // namespace nodes
} // namespace akao