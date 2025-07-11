/**
 * @id: akao:class:core:engine:orchestrator:discovery:v1
 * @doc: Node discovery system implementation for dynamic external node detection and registry management. Scans filesystem for node manifests, parses node metadata, validates node capabilities, and integrates with the node registry system. Replaces static node registration with dynamic discovery of external node processes, enabling true plugin-based architecture where nodes can be added/removed at runtime.
 * @specification: Dynamic node discovery implementation with manifest parsing and registry integration
 * @scope: Core engine orchestrator node discovery system implementation
 * @timeline: 2025-07-11
 * @rationale: Enable dynamic plugin-based architecture with runtime node discovery
 * @methodology: Filesystem scanning with YAML manifest parsing and process-based integration
 * @references: ["akao:directory:core:engine:orchestrator:discovery:v1", "akao:class:core:foundation:formats:yaml:v1"]
 */

#include "v1.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <dirent.h>

namespace akao {
namespace core {
namespace engine {
namespace orchestrator {
namespace discovery {

// =============================================================================
// NodeManifest Implementation
// =============================================================================

std::unique_ptr<NodeManifest> NodeManifest::fromYaml(const std::string& yaml_content) {
    try {
        akao::foundation::formats::yaml::YamlParser parser;
        auto root = parser.parse(yaml_content);
        
        if (!root || !root->isMapping()) {
            return nullptr;
        }
        
        auto manifest = std::make_unique<NodeManifest>();
        
        // Basic information
        manifest->id = root->hasKey("id") ? root->operator[]("id")->asString() : "";
        manifest->name = root->hasKey("name") ? root->operator[]("name")->asString() : "";
        manifest->version = root->hasKey("version") ? root->operator[]("version")->asString() : "1.0.0";
        manifest->description = root->hasKey("description") ? root->operator[]("description")->asString() : "";
        
        // Runtime configuration
        if (root->hasKey("runtime")) {
            auto runtime = root->operator[]("runtime");
            manifest->runtime.type = runtime->hasKey("type") ? runtime->operator[]("type")->asString() : "executable";
            manifest->runtime.command = runtime->hasKey("command") ? runtime->operator[]("command")->asString() : "";
            manifest->runtime.working_dir = runtime->hasKey("working_dir") ? runtime->operator[]("working_dir")->asString() : "";
            
            if (runtime->hasKey("args") && runtime->operator[]("args")->isSequence()) {
                for (const auto& arg : runtime->operator[]("args")->asSequence()) {
                    manifest->runtime.args.push_back(arg->asString());
                }
            }
            
            if (runtime->hasKey("env") && runtime->operator[]("env")->isMapping()) {
                for (const auto& key : runtime->operator[]("env")->getKeys()) {
                    manifest->runtime.env[key] = runtime->operator[]("env")->operator[](key)->asString();
                }
            }
        }
        
        // Communication configuration
        if (root->hasKey("communication")) {
            auto comm = root->operator[]("communication");
            manifest->communication.protocol = comm->hasKey("protocol") ? comm->operator[]("protocol")->asString() : "yamlrpc";
            manifest->communication.version = comm->hasKey("version") ? comm->operator[]("version")->asString() : "1.0";
            manifest->communication.socket_path = comm->hasKey("socket_path") ? comm->operator[]("socket_path")->asString() : "";
            
            if (comm->hasKey("methods") && comm->operator[]("methods")->isSequence()) {
                for (const auto& method : comm->operator[]("methods")->asSequence()) {
                    manifest->communication.methods.push_back(method->asString());
                }
            }
        }
        
        // Inputs
        if (root->hasKey("inputs") && root->operator[]("inputs")->isSequence()) {
            for (const auto& input : root->operator[]("inputs")->asSequence()) {
                NodeManifest::Input inp;
                inp.name = input->hasKey("name") ? input->operator[]("name")->asString() : "";
                inp.type = input->hasKey("type") ? input->operator[]("type")->asString() : "string";
                inp.required = input->hasKey("required") ? input->operator[]("required")->asBoolean() : false;
                inp.description = input->hasKey("description") ? input->operator[]("description")->asString() : "";
                manifest->inputs.push_back(inp);
            }
        }
        
        // Outputs
        if (root->hasKey("outputs") && root->operator[]("outputs")->isSequence()) {
            for (const auto& output : root->operator[]("outputs")->asSequence()) {
                NodeManifest::Output out;
                out.name = output->hasKey("name") ? output->operator[]("name")->asString() : "";
                out.type = output->hasKey("type") ? output->operator[]("type")->asString() : "string";
                out.description = output->hasKey("description") ? output->operator[]("description")->asString() : "";
                manifest->outputs.push_back(out);
            }
        }
        
        // Resources
        if (root->hasKey("resources")) {
            auto resources = root->operator[]("resources");
            manifest->resources.memory = resources->hasKey("memory") ? resources->operator[]("memory")->asString() : "128MB";
            manifest->resources.cpu = resources->hasKey("cpu") ? resources->operator[]("cpu")->asString() : "100m";
            manifest->resources.timeout_seconds = resources->hasKey("timeout_seconds") ? resources->operator[]("timeout_seconds")->asInteger() : 30;
            manifest->resources.max_instances = resources->hasKey("max_instances") ? resources->operator[]("max_instances")->asInteger() : 1;
        }
        
        return manifest;
    } catch (const std::exception&) {
        return nullptr;
    }
}

std::unique_ptr<NodeManifest> NodeManifest::fromFile(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        return nullptr;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return fromYaml(content);
}

bool NodeManifest::isValid() const {
    return !id.empty() && !name.empty() && !runtime.command.empty() && 
           communication.protocol == "yamlrpc" && !communication.socket_path.empty();
}

std::vector<std::string> NodeManifest::validate() const {
    std::vector<std::string> errors;
    
    if (id.empty()) {
        errors.push_back("Missing required field: id");
    }
    if (name.empty()) {
        errors.push_back("Missing required field: name");
    }
    if (runtime.command.empty()) {
        errors.push_back("Missing required field: runtime.command");
    }
    if (communication.protocol != "yamlrpc") {
        errors.push_back("Unsupported communication protocol: " + communication.protocol);
    }
    if (communication.socket_path.empty()) {
        errors.push_back("Missing required field: communication.socket_path");
    }
    
    return errors;
}

std::string NodeManifest::getSocketPath() const {
    return communication.socket_path;
}

std::string NodeManifest::getExecutablePath() const {
    return runtime.command;
}

bool NodeManifest::isExecutable() const {
    return runtime.type == "executable";
}

// =============================================================================
// DiscoveredNode Implementation
// =============================================================================

DiscoveredNode::DiscoveredNode() 
    : process_id(-1), is_running(false), restart_count(0), is_healthy(false) {}

DiscoveredNode::~DiscoveredNode() = default;

bool DiscoveredNode::isProcessRunning() const {
    if (process_id <= 0) {
        return false;
    }
    
    // Check if process exists
    return kill(process_id, 0) == 0;
}

bool DiscoveredNode::isConnected() const {
    return client && client->isConnected();
}

std::chrono::duration<double> DiscoveredNode::getUptime() const {
    if (!is_running) {
        return std::chrono::duration<double>(0);
    }
    
    return std::chrono::system_clock::now() - started_at;
}

bool DiscoveredNode::performHealthCheck() {
    if (!client || !client->isConnected()) {
        is_healthy = false;
        return false;
    }
    
    try {
        auto response = client->nodeHealth();
        is_healthy = response && response->isResponse();
        last_health_check = std::chrono::system_clock::now();
        return is_healthy;
    } catch (const std::exception&) {
        is_healthy = false;
        return false;
    }
}

std::string DiscoveredNode::getStatusString() const {
    if (!is_running) {
        return "stopped";
    }
    if (!isConnected()) {
        return "running-disconnected";
    }
    if (!is_healthy) {
        return "running-unhealthy";
    }
    return "running-healthy";
}

// =============================================================================
// NodeDiscoveryScanner Implementation
// =============================================================================

NodeDiscoveryScanner::NodeDiscoveryScanner(const std::string& base_path)
    : base_path_(base_path), scanning_(false), scan_interval_(std::chrono::seconds(10)) {
    search_patterns_.push_back("_.yaml");
    search_patterns_.push_back("manifest.yaml");
    search_patterns_.push_back("node.yaml");
}

NodeDiscoveryScanner::~NodeDiscoveryScanner() {
    stopScanning();
}

void NodeDiscoveryScanner::setBasePath(const std::string& path) {
    base_path_ = path;
}

void NodeDiscoveryScanner::addSearchPattern(const std::string& pattern) {
    search_patterns_.push_back(pattern);
}

void NodeDiscoveryScanner::setScanInterval(std::chrono::seconds interval) {
    scan_interval_ = interval;
}

void NodeDiscoveryScanner::onNodeDiscovered(std::function<void(const std::string&, const DiscoveredNode&)> callback) {
    on_node_discovered_ = callback;
}

void NodeDiscoveryScanner::onNodeLost(std::function<void(const std::string&)> callback) {
    on_node_lost_ = callback;
}

void NodeDiscoveryScanner::onNodeChanged(std::function<void(const std::string&, const DiscoveredNode&)> callback) {
    on_node_changed_ = callback;
}

void NodeDiscoveryScanner::startScanning() {
    if (scanning_) {
        return;
    }
    
    scanning_ = true;
    scan_thread_ = std::thread(&NodeDiscoveryScanner::scanLoop, this);
}

void NodeDiscoveryScanner::stopScanning() {
    scanning_ = false;
    if (scan_thread_.joinable()) {
        scan_thread_.join();
    }
}

void NodeDiscoveryScanner::scanOnce() {
    if (!std::filesystem::exists(base_path_)) {
        return;
    }
    
    for (const auto& entry : std::filesystem::recursive_directory_iterator(base_path_)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            for (const auto& pattern : search_patterns_) {
                if (filename == pattern) {
                    processManifest(entry.path().string());
                    break;
                }
            }
        }
    }
}

void NodeDiscoveryScanner::scanDirectory(const std::string& directory) {
    if (!std::filesystem::exists(directory)) {
        return;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            for (const auto& pattern : search_patterns_) {
                if (filename == pattern) {
                    processManifest(entry.path().string());
                    break;
                }
            }
        }
    }
}

void NodeDiscoveryScanner::scanManifest(const std::string& manifest_path) {
    processManifest(manifest_path);
}

std::vector<std::string> NodeDiscoveryScanner::getDiscoveredNodeIds() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(nodes_mutex_));
    std::vector<std::string> ids;
    for (const auto& pair : discovered_nodes_) {
        ids.push_back(pair.first);
    }
    return ids;
}

const DiscoveredNode* NodeDiscoveryScanner::getNode(const std::string& node_id) const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(nodes_mutex_));
    auto it = discovered_nodes_.find(node_id);
    return it != discovered_nodes_.end() ? it->second.get() : nullptr;
}

std::map<std::string, std::string> NodeDiscoveryScanner::getNodeSummary() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(nodes_mutex_));
    std::map<std::string, std::string> summary;
    for (const auto& pair : discovered_nodes_) {
        summary[pair.first] = pair.second->getStatusString();
    }
    return summary;
}

size_t NodeDiscoveryScanner::getDiscoveredCount() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(nodes_mutex_));
    return discovered_nodes_.size();
}

size_t NodeDiscoveryScanner::getRunningCount() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(nodes_mutex_));
    return std::count_if(discovered_nodes_.begin(), discovered_nodes_.end(),
                        [](const auto& pair) { return pair.second->is_running; });
}

size_t NodeDiscoveryScanner::getHealthyCount() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(nodes_mutex_));
    return std::count_if(discovered_nodes_.begin(), discovered_nodes_.end(),
                        [](const auto& pair) { return pair.second->is_healthy; });
}

void NodeDiscoveryScanner::scanLoop() {
    while (scanning_) {
        scanOnce();
        std::this_thread::sleep_for(scan_interval_);
    }
}

void NodeDiscoveryScanner::processManifest(const std::string& manifest_path) {
    auto manifest = NodeManifest::fromFile(manifest_path);
    if (!manifest || !manifest->isValid()) {
        return;
    }
    
    std::string node_id = manifest->id;
    std::string node_directory = std::filesystem::path(manifest_path).parent_path().string();
    
    std::lock_guard<std::mutex> lock(nodes_mutex_);
    auto it = discovered_nodes_.find(node_id);
    
    if (it == discovered_nodes_.end()) {
        // New node discovered
        auto node = std::make_unique<DiscoveredNode>();
        node->manifest = std::move(manifest);
        node->manifest_path = manifest_path;
        node->node_directory = node_directory;
        node->discovered_at = std::chrono::system_clock::now();
        node->last_modified = getFileModificationTime(manifest_path);
        
        discovered_nodes_[node_id] = std::move(node);
        notifyNodeDiscovered(node_id, *discovered_nodes_[node_id]);
    } else {
        // Check if manifest has changed
        if (hasManifestChanged(manifest_path, *it->second)) {
            it->second->manifest = std::move(manifest);
            it->second->last_modified = getFileModificationTime(manifest_path);
            notifyNodeChanged(node_id, *it->second);
        }
    }
}

bool NodeDiscoveryScanner::hasManifestChanged(const std::string& manifest_path, const DiscoveredNode& node) {
    auto last_modified = getFileModificationTime(manifest_path);
    return last_modified > node.last_modified;
}

std::chrono::system_clock::time_point NodeDiscoveryScanner::getFileModificationTime(const std::string& file_path) {
    struct stat file_stat;
    if (stat(file_path.c_str(), &file_stat) == 0) {
        return std::chrono::system_clock::from_time_t(file_stat.st_mtime);
    }
    return std::chrono::system_clock::now();
}

void NodeDiscoveryScanner::notifyNodeDiscovered(const std::string& node_id, const DiscoveredNode& node) {
    if (on_node_discovered_) {
        on_node_discovered_(node_id, node);
    }
}

void NodeDiscoveryScanner::notifyNodeLost(const std::string& node_id) {
    if (on_node_lost_) {
        on_node_lost_(node_id);
    }
}

void NodeDiscoveryScanner::notifyNodeChanged(const std::string& node_id, const DiscoveredNode& node) {
    if (on_node_changed_) {
        on_node_changed_(node_id, node);
    }
}

// =============================================================================
// NodeRegistry Implementation
// =============================================================================

NodeRegistry::NodeRegistry() 
    : communicator_(std::make_unique<akao::core::engine::communication::yamlrpc::NodeCommunicator>()),
      monitoring_(false), health_interval_(std::chrono::seconds(30)) {}

NodeRegistry::~NodeRegistry() {
    stopHealthMonitoring();
    disableDiscovery();
}

void NodeRegistry::enableDiscovery(const std::string& base_path) {
    if (scanner_) {
        return;
    }
    
    scanner_ = std::make_unique<NodeDiscoveryScanner>(base_path);
    scanner_->onNodeDiscovered([this](const std::string& node_id, const DiscoveredNode& node) {
        onNodeDiscovered(node_id, node);
    });
    scanner_->onNodeLost([this](const std::string& node_id) {
        onNodeLost(node_id);
    });
    scanner_->onNodeChanged([this](const std::string& node_id, const DiscoveredNode& node) {
        onNodeChanged(node_id, node);
    });
    
    scanner_->startScanning();
}

void NodeRegistry::disableDiscovery() {
    if (scanner_) {
        scanner_->stopScanning();
        scanner_.reset();
    }
}

bool NodeRegistry::isDiscoveryEnabled() const {
    return scanner_ && scanner_->isScanning();
}

void NodeRegistry::registerNode(const std::string& node_id, std::unique_ptr<DiscoveredNode> node) {
    std::lock_guard<std::mutex> lock(registry_mutex_);
    registered_nodes_[node_id] = std::move(node);
}

void NodeRegistry::unregisterNode(const std::string& node_id) {
    std::lock_guard<std::mutex> lock(registry_mutex_);
    auto it = registered_nodes_.find(node_id);
    if (it != registered_nodes_.end()) {
        stopNodeProcess(*it->second);
        registered_nodes_.erase(it);
    }
}

void NodeRegistry::unregisterAll() {
    std::lock_guard<std::mutex> lock(registry_mutex_);
    for (auto& pair : registered_nodes_) {
        stopNodeProcess(*pair.second);
    }
    registered_nodes_.clear();
}

std::vector<std::string> NodeRegistry::getRegisteredNodeIds() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(registry_mutex_));
    std::vector<std::string> ids;
    for (const auto& pair : registered_nodes_) {
        ids.push_back(pair.first);
    }
    return ids;
}

const DiscoveredNode* NodeRegistry::getNode(const std::string& node_id) const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(registry_mutex_));
    auto it = registered_nodes_.find(node_id);
    return it != registered_nodes_.end() ? it->second.get() : nullptr;
}

bool NodeRegistry::startNode(const std::string& node_id) {
    auto node = getMutableNode(node_id);
    return node ? startNodeProcess(*node) : false;
}

bool NodeRegistry::stopNode(const std::string& node_id) {
    auto node = getMutableNode(node_id);
    return node ? stopNodeProcess(*node) : false;
}

bool NodeRegistry::connectToNode(const std::string& node_id) {
    auto node = getMutableNode(node_id);
    if (!node || !node->manifest) {
        return false;
    }
    
    std::string socket_path = node->manifest->getSocketPath();
    return communicator_->connectToNode(node_id, socket_path);
}

void NodeRegistry::disconnectFromNode(const std::string& node_id) {
    communicator_->disconnectFromNode(node_id);
}

std::shared_ptr<akao::core::engine::communication::yamlrpc::YamlRpcMessage> 
NodeRegistry::getNodeInfo(const std::string& node_id) {
    return communicator_->nodeInfo(node_id);
}

size_t NodeRegistry::getRegisteredCount() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(registry_mutex_));
    return registered_nodes_.size();
}

void NodeRegistry::onNodeDiscovered(const std::string& node_id, const DiscoveredNode& node) {
    // Create a copy of the discovered node for registration
    auto node_copy = std::make_unique<DiscoveredNode>();
    node_copy->manifest = std::make_unique<NodeManifest>(*node.manifest);
    node_copy->manifest_path = node.manifest_path;
    node_copy->node_directory = node.node_directory;
    node_copy->discovered_at = node.discovered_at;
    node_copy->last_modified = node.last_modified;
    
    registerNode(node_id, std::move(node_copy));
}

void NodeRegistry::onNodeLost(const std::string& node_id) {
    unregisterNode(node_id);
}

void NodeRegistry::onNodeChanged(const std::string& node_id, const DiscoveredNode& node) {
    // Update the registered node
    auto registered_node = getMutableNode(node_id);
    if (registered_node) {
        registered_node->manifest = std::make_unique<NodeManifest>(*node.manifest);
        registered_node->last_modified = node.last_modified;
    }
}

bool NodeRegistry::startNodeProcess(DiscoveredNode& node) {
    if (!node.manifest || node.is_running) {
        return false;
    }
    
    std::string command = node.manifest->getExecutablePath();
    
    // Simple process spawning (in production, use proper process management)
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        execl(command.c_str(), command.c_str(), nullptr);
        exit(1);
    } else if (pid > 0) {
        // Parent process
        node.process_id = pid;
        node.is_running = true;
        node.started_at = std::chrono::system_clock::now();
        return true;
    }
    
    return false;
}

bool NodeRegistry::stopNodeProcess(DiscoveredNode& node) {
    if (!node.is_running || node.process_id <= 0) {
        return false;
    }
    
    // Send SIGTERM, then SIGKILL if necessary
    if (kill(node.process_id, SIGTERM) == 0) {
        // Wait for process to exit
        int status;
        if (waitpid(node.process_id, &status, WNOHANG) == node.process_id) {
            node.is_running = false;
            node.process_id = -1;
            return true;
        }
        
        // If still running, force kill
        sleep(2);
        if (kill(node.process_id, SIGKILL) == 0) {
            waitpid(node.process_id, &status, 0);
            node.is_running = false;
            node.process_id = -1;
            return true;
        }
    }
    
    return false;
}

DiscoveredNode* NodeRegistry::getMutableNode(const std::string& node_id) {
    std::lock_guard<std::mutex> lock(registry_mutex_);
    auto it = registered_nodes_.find(node_id);
    return it != registered_nodes_.end() ? it->second.get() : nullptr;
}

std::string NodeRegistry::generateSocketPath(const std::string& node_id) {
    return "/tmp/akao-node-" + node_id + ".sock";
}

} // namespace discovery
} // namespace orchestrator
} // namespace engine
} // namespace core
} // namespace akao