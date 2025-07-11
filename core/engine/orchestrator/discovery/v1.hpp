/**
 * @id: akao:class:core:engine:orchestrator:discovery:v1
 * @doc: Node discovery system for dynamic external node detection and registry management. Scans filesystem for node manifests, parses node metadata, validates node capabilities, and integrates with the node registry system. Replaces static node registration with dynamic discovery of external node processes, enabling true plugin-based architecture where nodes can be added/removed at runtime.
 * @specification: Dynamic node discovery with manifest parsing and registry integration
 * @scope: Core engine orchestrator node discovery system
 * @timeline: 2025-07-11
 * @rationale: Enable dynamic plugin-based architecture with runtime node discovery
 * @methodology: Filesystem scanning with YAML manifest parsing and process-based integration
 * @references: ["akao:directory:core:engine:orchestrator:discovery:v1", "akao:class:core:foundation:formats:yaml:v1"]
 */

#pragma once

#include "core/foundation/formats/yaml/v1.hpp"
#include "core/foundation/types/value/v1.hpp"
#include "core/engine/communication/yamlrpc/v1.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <filesystem>
#include <mutex>
#include <thread>
#include <atomic>
#include <chrono>

namespace akao {
namespace core {
namespace engine {
namespace orchestrator {
namespace discovery {

/**
 * Node manifest representation
 */
struct NodeManifest {
    std::string id;
    std::string name;
    std::string version;
    std::string description;
    
    struct Runtime {
        std::string type;           // "executable", "script", "library"
        std::string command;        // Path to executable
        std::vector<std::string> args; // Command arguments
        std::string working_dir;    // Working directory
        std::map<std::string, std::string> env; // Environment variables
    } runtime;
    
    struct Communication {
        std::string protocol;       // "yamlrpc"
        std::string version;        // "1.0"
        std::string socket_path;    // Unix socket path
        std::vector<std::string> methods; // Supported methods
    } communication;
    
    struct Input {
        std::string name;
        std::string type;
        bool required;
        std::string description;
        akao::foundation::formats::yaml::YamlNode default_value;
    };
    
    struct Output {
        std::string name;
        std::string type;
        std::string description;
    };
    
    std::vector<Input> inputs;
    std::vector<Output> outputs;
    
    struct Resources {
        std::string memory;         // "128MB"
        std::string cpu;           // "100m"
        int timeout_seconds;       // 30
        int max_instances;         // 1
    } resources;
    
    struct Dependencies {
        std::vector<std::string> system; // System dependencies
        std::vector<std::string> nodes;  // Other node dependencies
    } dependencies;
    
    std::map<std::string, akao::foundation::formats::yaml::YamlNode> metadata;
    
    // Parsing
    static std::unique_ptr<NodeManifest> fromYaml(const std::string& yaml_content);
    static std::unique_ptr<NodeManifest> fromFile(const std::string& file_path);
    
    // Validation
    bool isValid() const;
    std::vector<std::string> validate() const;
    
    // Utilities
    std::string getSocketPath() const;
    std::string getExecutablePath() const;
    bool isExecutable() const;
};

/**
 * Discovered node information
 */
struct DiscoveredNode {
    std::unique_ptr<NodeManifest> manifest;
    std::string manifest_path;
    std::string node_directory;
    std::chrono::system_clock::time_point discovered_at;
    std::chrono::system_clock::time_point last_modified;
    
    // Process management
    pid_t process_id;
    bool is_running;
    std::chrono::system_clock::time_point started_at;
    int restart_count;
    
    // Communication
    std::unique_ptr<akao::core::engine::communication::yamlrpc::YamlRpcClient> client;
    std::chrono::system_clock::time_point last_health_check;
    bool is_healthy;
    
    DiscoveredNode();
    ~DiscoveredNode();
    
    // Status
    bool isProcessRunning() const;
    bool isConnected() const;
    std::chrono::duration<double> getUptime() const;
    
    // Health
    bool performHealthCheck();
    std::string getStatusString() const;
};

/**
 * Node discovery scanner
 */
class NodeDiscoveryScanner {
private:
    std::string base_path_;
    std::vector<std::string> search_patterns_;
    std::map<std::string, std::unique_ptr<DiscoveredNode>> discovered_nodes_;
    std::mutex nodes_mutex_;
    
    // Scanning
    std::thread scan_thread_;
    std::atomic<bool> scanning_;
    std::chrono::seconds scan_interval_;
    
    // Callbacks
    std::function<void(const std::string&, const DiscoveredNode&)> on_node_discovered_;
    std::function<void(const std::string&)> on_node_lost_;
    std::function<void(const std::string&, const DiscoveredNode&)> on_node_changed_;
    
public:
    NodeDiscoveryScanner(const std::string& base_path = ".akao/nodes");
    ~NodeDiscoveryScanner();
    
    // Configuration
    void setBasePath(const std::string& path);
    void addSearchPattern(const std::string& pattern);
    void setScanInterval(std::chrono::seconds interval);
    
    // Callbacks
    void onNodeDiscovered(std::function<void(const std::string&, const DiscoveredNode&)> callback);
    void onNodeLost(std::function<void(const std::string&)> callback);
    void onNodeChanged(std::function<void(const std::string&, const DiscoveredNode&)> callback);
    
    // Scanning control
    void startScanning();
    void stopScanning();
    bool isScanning() const { return scanning_; }
    
    // Manual operations
    void scanOnce();
    void scanDirectory(const std::string& directory);
    void scanManifest(const std::string& manifest_path);
    
    // Node access
    std::vector<std::string> getDiscoveredNodeIds() const;
    const DiscoveredNode* getNode(const std::string& node_id) const;
    std::map<std::string, std::string> getNodeSummary() const;
    
    // Statistics
    size_t getDiscoveredCount() const;
    size_t getRunningCount() const;
    size_t getHealthyCount() const;
    
private:
    void scanLoop();
    void processManifest(const std::string& manifest_path);
    void updateNode(const std::string& node_id, std::unique_ptr<NodeManifest> manifest, const std::string& manifest_path);
    bool hasManifestChanged(const std::string& manifest_path, const DiscoveredNode& node);
    void notifyNodeDiscovered(const std::string& node_id, const DiscoveredNode& node);
    void notifyNodeLost(const std::string& node_id);
    void notifyNodeChanged(const std::string& node_id, const DiscoveredNode& node);
    
    // Filesystem utilities
    std::vector<std::string> findManifestFiles(const std::string& directory);
    std::chrono::system_clock::time_point getFileModificationTime(const std::string& file_path);
};

/**
 * Node registry with discovery integration
 */
class NodeRegistry {
private:
    std::unique_ptr<NodeDiscoveryScanner> scanner_;
    std::map<std::string, std::unique_ptr<DiscoveredNode>> registered_nodes_;
    std::mutex registry_mutex_;
    
    // Node management
    std::unique_ptr<akao::core::engine::communication::yamlrpc::NodeCommunicator> communicator_;
    
    // Health monitoring
    std::thread health_thread_;
    std::atomic<bool> monitoring_;
    std::chrono::seconds health_interval_;
    
public:
    NodeRegistry();
    ~NodeRegistry();
    
    // Discovery integration
    void enableDiscovery(const std::string& base_path = ".akao/nodes");
    void disableDiscovery();
    bool isDiscoveryEnabled() const;
    
    // Registration
    void registerNode(const std::string& node_id, std::unique_ptr<DiscoveredNode> node);
    void unregisterNode(const std::string& node_id);
    void unregisterAll();
    
    // Node access
    std::vector<std::string> getRegisteredNodeIds() const;
    const DiscoveredNode* getNode(const std::string& node_id) const;
    std::vector<std::string> getNodesByType(const std::string& type) const;
    
    // Node operations
    bool startNode(const std::string& node_id);
    bool stopNode(const std::string& node_id);
    bool restartNode(const std::string& node_id);
    bool connectToNode(const std::string& node_id);
    void disconnectFromNode(const std::string& node_id);
    
    // Communication
    std::shared_ptr<akao::core::engine::communication::yamlrpc::YamlRpcMessage> 
        callNode(const std::string& node_id, const std::string& method, 
                const std::shared_ptr<akao::foundation::formats::yaml::YamlNode>& params = nullptr);
    
    // Standard node operations
    std::shared_ptr<akao::core::engine::communication::yamlrpc::YamlRpcMessage> 
        getNodeInfo(const std::string& node_id);
    std::shared_ptr<akao::core::engine::communication::yamlrpc::YamlRpcMessage> 
        validateNodeInput(const std::string& node_id, const std::shared_ptr<akao::foundation::formats::yaml::YamlNode>& input);
    std::shared_ptr<akao::core::engine::communication::yamlrpc::YamlRpcMessage> 
        executeNode(const std::string& node_id, const std::shared_ptr<akao::foundation::formats::yaml::YamlNode>& input,
                   const std::shared_ptr<akao::foundation::formats::yaml::YamlNode>& context);
    
    // Health monitoring
    void startHealthMonitoring(std::chrono::seconds interval = std::chrono::seconds(30));
    void stopHealthMonitoring();
    bool isHealthMonitoringEnabled() const { return monitoring_; }
    
    // Batch operations
    std::map<std::string, bool> startAllNodes();
    std::map<std::string, bool> stopAllNodes();
    std::map<std::string, bool> healthCheckAll();
    
    // Statistics
    size_t getRegisteredCount() const;
    size_t getRunningCount() const;
    size_t getHealthyCount() const;
    std::map<std::string, std::string> getRegistryStatus() const;
    
private:
    // Discovery callbacks
    void onNodeDiscovered(const std::string& node_id, const DiscoveredNode& node);
    void onNodeLost(const std::string& node_id);
    void onNodeChanged(const std::string& node_id, const DiscoveredNode& node);
    
    // Health monitoring
    void healthMonitoringLoop();
    void performHealthCheck(const std::string& node_id);
    
    // Process management
    bool startNodeProcess(DiscoveredNode& node);
    bool stopNodeProcess(DiscoveredNode& node);
    bool isProcessRunning(pid_t pid);
    
    // Utilities
    std::string generateSocketPath(const std::string& node_id);
    DiscoveredNode* getMutableNode(const std::string& node_id);
};

} // namespace discovery
} // namespace orchestrator
} // namespace engine
} // namespace core
} // namespace akao