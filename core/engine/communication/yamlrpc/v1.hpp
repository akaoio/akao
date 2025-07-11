/**
 * @id: akao:class:core:engine:communication:yamlrpc:v1
 * @doc: YAML-RPC protocol implementation for core-node communication providing structured message-based RPC communication over Unix Domain Sockets. Implements the complete YAML-RPC v1.0 protocol specification with request-response patterns, error handling, and connection management. Enables true core-node independence by eliminating direct C++ interface dependencies and providing standard communication protocol for external node processes.
 * @specification: YAML-RPC protocol implementation with Unix Domain Sockets transport
 * @scope: Core engine communication protocol implementation
 * @timeline: 2025-07-11
 * @rationale: Enable structured protocol-based communication between core and external nodes
 * @methodology: Message-based RPC protocol using YAML serialization over Unix Domain Sockets
 * @references: ["akao:directory:core:engine:communication:yamlrpc:v1", "akao:class:core:foundation:formats:yaml:v1"]
 */

#pragma once

#include "core/foundation/formats/yaml/v1.hpp"
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <functional>
#include <future>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace akao {
namespace core {
namespace engine {
namespace communication {
namespace yamlrpc {

// Forward declarations
class YamlRpcMessage;
class YamlRpcClient;
class YamlRpcServer;
class UnixSocketTransport;

/**
 * YAML-RPC Message representation
 */
class YamlRpcMessage {
public:
    enum class Type {
        REQUEST,
        RESPONSE,
        ERROR
    };

    struct Request {
        std::string method;
        std::shared_ptr<akao::foundation::formats::yaml::YamlNode> params;
        std::string id;
    };

    struct Response {
        std::shared_ptr<akao::foundation::formats::yaml::YamlNode> result;
        std::string id;
    };

    struct Error {
        int code;
        std::string message;
        std::shared_ptr<akao::foundation::formats::yaml::YamlNode> data;
        std::string id;
    };

private:
    Type type_;
    std::string yamlrpc_version_;
    std::variant<Request, Response, Error> content_;

public:
    YamlRpcMessage(const Request& request);
    YamlRpcMessage(const Response& response);
    YamlRpcMessage(const Error& error);
    
    // Parse from YAML string
    static std::shared_ptr<YamlRpcMessage> fromYaml(const std::string& yaml_content);
    
    // Serialize to YAML string
    std::string toYaml() const;
    
    // Getters
    Type getType() const { return type_; }
    const std::string& getVersion() const { return yamlrpc_version_; }
    
    // Type-specific getters
    const Request& getRequest() const;
    const Response& getResponse() const;
    const Error& getError() const;
    
    // Utility methods
    std::string getId() const;
    bool isRequest() const { return type_ == Type::REQUEST; }
    bool isResponse() const { return type_ == Type::RESPONSE; }
    bool isError() const { return type_ == Type::ERROR; }
    
    // Standard error codes
    static constexpr int PARSE_ERROR = -32700;
    static constexpr int INVALID_REQUEST = -32600;
    static constexpr int METHOD_NOT_FOUND = -32601;
    static constexpr int INVALID_PARAMS = -32602;
    static constexpr int INTERNAL_ERROR = -32603;
    
    // Node-specific error codes
    static constexpr int NODE_INIT_ERROR = -1000;
    static constexpr int NODE_CONFIG_ERROR = -1001;
    static constexpr int NODE_VALIDATION_ERROR = -1002;
    static constexpr int NODE_EXECUTION_TIMEOUT = -1003;
    static constexpr int NODE_RESOURCE_LIMIT = -1004;
    static constexpr int NODE_INTERNAL_ERROR = -1005;
    
    // Factory methods for common messages
    static std::shared_ptr<YamlRpcMessage> createRequest(const std::string& method, 
                                                        const std::shared_ptr<akao::foundation::formats::yaml::YamlNode>& params,
                                                        const std::string& id);
    static std::shared_ptr<YamlRpcMessage> createResponse(const std::shared_ptr<akao::foundation::formats::yaml::YamlNode>& result,
                                                         const std::string& id);
    static std::shared_ptr<YamlRpcMessage> createError(int code, const std::string& message, const std::string& id,
                                                       const std::shared_ptr<akao::foundation::formats::yaml::YamlNode>& data = nullptr);
};

/**
 * Unix Domain Socket Transport Layer
 */
class UnixSocketTransport {
private:
    std::string socket_path_;
    int socket_fd_;
    bool is_connected_;
    std::mutex mutex_;
    
public:
    UnixSocketTransport();
    ~UnixSocketTransport();
    
    // Client methods
    bool connect(const std::string& socket_path);
    void disconnect();
    
    // Server methods
    bool bind(const std::string& socket_path);
    int accept();
    void close();
    
    // Communication methods
    bool send(const std::string& message);
    std::string receive();
    bool send(int fd, const std::string& message);
    std::string receive(int fd);
    
    // Status
    bool isConnected() const { return is_connected_; }
    const std::string& getSocketPath() const { return socket_path_; }
    
private:
    bool sendWithFraming(int fd, const std::string& message);
    std::string receiveWithFraming(int fd);
    void cleanup();
};

/**
 * YAML-RPC Client for core-to-node communication
 */
class YamlRpcClient {
private:
    std::unique_ptr<UnixSocketTransport> transport_;
    std::string node_id_;
    std::atomic<int> request_counter_;
    std::map<std::string, std::promise<std::shared_ptr<YamlRpcMessage>>> pending_requests_;
    std::mutex pending_mutex_;
    std::thread response_thread_;
    std::atomic<bool> running_;
    
public:
    YamlRpcClient();
    ~YamlRpcClient();
    
    // Connection management
    bool connect(const std::string& socket_path);
    void disconnect();
    
    // Standard node methods
    std::shared_ptr<YamlRpcMessage> nodeInfo();
    std::shared_ptr<YamlRpcMessage> nodeValidate(const std::shared_ptr<akao::foundation::formats::yaml::YamlNode>& input);
    std::shared_ptr<YamlRpcMessage> nodeExecute(const std::shared_ptr<akao::foundation::formats::yaml::YamlNode>& input,
                                               const std::shared_ptr<akao::foundation::formats::yaml::YamlNode>& context);
    std::shared_ptr<YamlRpcMessage> nodeHealth();
    std::shared_ptr<YamlRpcMessage> nodeShutdown(int timeout_seconds = 10);
    
    // Generic method call
    std::shared_ptr<YamlRpcMessage> call(const std::string& method,
                                        const std::shared_ptr<akao::foundation::formats::yaml::YamlNode>& params = nullptr);
    
    // Async method call
    std::future<std::shared_ptr<YamlRpcMessage>> callAsync(const std::string& method,
                                                          const std::shared_ptr<akao::foundation::formats::yaml::YamlNode>& params = nullptr);
    
    // Status
    bool isConnected() const { return transport_ && transport_->isConnected(); }
    const std::string& getNodeId() const { return node_id_; }
    
private:
    std::string generateRequestId();
    void handleResponses();
    void processResponse(const std::shared_ptr<YamlRpcMessage>& response);
};

/**
 * YAML-RPC Server for node-to-core communication
 */
class YamlRpcServer {
public:
    using MethodHandler = std::function<std::shared_ptr<YamlRpcMessage>(const std::shared_ptr<YamlRpcMessage>&)>;
    
private:
    std::unique_ptr<UnixSocketTransport> transport_;
    std::string socket_path_;
    std::map<std::string, MethodHandler> method_handlers_;
    std::atomic<bool> running_;
    std::thread server_thread_;
    std::vector<std::thread> worker_threads_;
    std::mutex handlers_mutex_;
    
    // Default handlers
    std::shared_ptr<YamlRpcMessage> handleNodeInfo(const std::shared_ptr<YamlRpcMessage>& request);
    std::shared_ptr<YamlRpcMessage> handleNodeValidate(const std::shared_ptr<YamlRpcMessage>& request);
    std::shared_ptr<YamlRpcMessage> handleNodeExecute(const std::shared_ptr<YamlRpcMessage>& request);
    std::shared_ptr<YamlRpcMessage> handleNodeHealth(const std::shared_ptr<YamlRpcMessage>& request);
    std::shared_ptr<YamlRpcMessage> handleNodeShutdown(const std::shared_ptr<YamlRpcMessage>& request);
    
public:
    YamlRpcServer();
    ~YamlRpcServer();
    
    // Server lifecycle
    bool start(const std::string& socket_path);
    void stop();
    
    // Method registration
    void registerMethod(const std::string& method, MethodHandler handler);
    void unregisterMethod(const std::string& method);
    
    // Standard node methods registration
    void registerStandardMethods();
    
    // Status
    bool isRunning() const { return running_; }
    const std::string& getSocketPath() const { return socket_path_; }
    
private:
    void serverLoop();
    void handleClient(int client_fd);
    void processRequest(int client_fd, const std::shared_ptr<YamlRpcMessage>& request);
    void sendResponse(int client_fd, const std::shared_ptr<YamlRpcMessage>& response);
    void cleanup();
};

/**
 * Node Communication Manager
 */
class NodeCommunicator {
private:
    std::map<std::string, std::unique_ptr<YamlRpcClient>> clients_;
    std::mutex clients_mutex_;
    
public:
    NodeCommunicator();
    ~NodeCommunicator();
    
    // Client management
    bool connectToNode(const std::string& node_id, const std::string& socket_path);
    void disconnectFromNode(const std::string& node_id);
    void disconnectAll();
    
    // Standard node operations
    std::shared_ptr<YamlRpcMessage> nodeInfo(const std::string& node_id);
    std::shared_ptr<YamlRpcMessage> nodeValidate(const std::string& node_id, 
                                                const std::shared_ptr<akao::foundation::formats::yaml::YamlNode>& input);
    std::shared_ptr<YamlRpcMessage> nodeExecute(const std::string& node_id,
                                               const std::shared_ptr<akao::foundation::formats::yaml::YamlNode>& input,
                                               const std::shared_ptr<akao::foundation::formats::yaml::YamlNode>& context);
    std::shared_ptr<YamlRpcMessage> nodeHealth(const std::string& node_id);
    void nodeShutdown(const std::string& node_id, int timeout_seconds = 10);
    
    // Batch operations
    std::map<std::string, std::shared_ptr<YamlRpcMessage>> healthCheckAll();
    void shutdownAll(int timeout_seconds = 10);
    
    // Status
    bool isConnected(const std::string& node_id) const;
    std::vector<std::string> getConnectedNodes() const;
    size_t getConnectedCount() const;
    
private:
    YamlRpcClient* getClient(const std::string& node_id);
};

} // namespace yamlrpc
} // namespace communication
} // namespace engine
} // namespace core
} // namespace akao