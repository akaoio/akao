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

#include "v1.hpp"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <cstring>
#include <sstream>
#include <chrono>
#include <algorithm>

namespace akao {
namespace core {
namespace engine {
namespace communication {
namespace yamlrpc {

// =============================================================================
// YamlRpcMessage Implementation
// =============================================================================

YamlRpcMessage::YamlRpcMessage(const Request& request) 
    : type_(Type::REQUEST), yamlrpc_version_("1.0"), content_(request) {}

YamlRpcMessage::YamlRpcMessage(const Response& response) 
    : type_(Type::RESPONSE), yamlrpc_version_("1.0"), content_(response) {}

YamlRpcMessage::YamlRpcMessage(const Error& error) 
    : type_(Type::ERROR), yamlrpc_version_("1.0"), content_(error) {}

std::shared_ptr<YamlRpcMessage> YamlRpcMessage::fromYaml(const std::string& yaml_content) {
    try {
        akao::foundation::formats::yaml::YamlParser parser;
        auto root = parser.parse(yaml_content);
        
        if (!root || !root->isMapping()) {
            return nullptr;
        }
        
        // Check required yamlrpc field
        if (!root->hasKey("yamlrpc") || root->operator[]("yamlrpc")->asString() != "1.0") {
            return nullptr;
        }
        
        // Determine message type
        if (root->hasKey("method")) {
            // Request
            Request req;
            req.method = root->operator[]("method")->asString();
            req.params = root->hasKey("params") ? root->operator[]("params") : nullptr;
            req.id = root->hasKey("id") ? root->operator[]("id")->asString() : "";
            return std::make_shared<YamlRpcMessage>(req);
        } else if (root->hasKey("result")) {
            // Response
            Response resp;
            resp.result = root->operator[]("result");
            resp.id = root->hasKey("id") ? root->operator[]("id")->asString() : "";
            return std::make_shared<YamlRpcMessage>(resp);
        } else if (root->hasKey("error")) {
            // Error
            Error err;
            auto error_node = root->operator[]("error");
            err.code = error_node->hasKey("code") ? error_node->operator[]("code")->asInteger() : INTERNAL_ERROR;
            err.message = error_node->hasKey("message") ? error_node->operator[]("message")->asString() : "Unknown error";
            err.data = error_node->hasKey("data") ? error_node->operator[]("data") : nullptr;
            err.id = root->hasKey("id") ? root->operator[]("id")->asString() : "";
            return std::make_shared<YamlRpcMessage>(err);
        }
        
        return nullptr;
    } catch (const std::exception&) {
        return nullptr;
    }
}

std::string YamlRpcMessage::toYaml() const {
    auto root = akao::foundation::formats::yaml::YamlNode::createMapping();
    root->setMapping("yamlrpc", akao::foundation::formats::yaml::YamlNode::createString(yamlrpc_version_));
    
    switch (type_) {
        case Type::REQUEST: {
            const auto& req = std::get<Request>(content_);
            root->setMapping("method", akao::foundation::formats::yaml::YamlNode::createString(req.method));
            if (req.params) {
                root->setMapping("params", req.params);
            }
            if (!req.id.empty()) {
                root->setMapping("id", akao::foundation::formats::yaml::YamlNode::createString(req.id));
            }
            break;
        }
        case Type::RESPONSE: {
            const auto& resp = std::get<Response>(content_);
            if (resp.result) {
                root->setMapping("result", resp.result);
            }
            if (!resp.id.empty()) {
                root->setMapping("id", akao::foundation::formats::yaml::YamlNode::createString(resp.id));
            }
            break;
        }
        case Type::ERROR: {
            const auto& err = std::get<Error>(content_);
            auto error_node = akao::foundation::formats::yaml::YamlNode::createMapping();
            error_node->setMapping("code", akao::foundation::formats::yaml::YamlNode::createInteger(err.code));
            error_node->setMapping("message", akao::foundation::formats::yaml::YamlNode::createString(err.message));
            if (err.data) {
                error_node->setMapping("data", err.data);
            }
            root->setMapping("error", error_node);
            if (!err.id.empty()) {
                root->setMapping("id", akao::foundation::formats::yaml::YamlNode::createString(err.id));
            }
            break;
        }
    }
    
    return root->toYaml();
}

const YamlRpcMessage::Request& YamlRpcMessage::getRequest() const {
    return std::get<Request>(content_);
}

const YamlRpcMessage::Response& YamlRpcMessage::getResponse() const {
    return std::get<Response>(content_);
}

const YamlRpcMessage::Error& YamlRpcMessage::getError() const {
    return std::get<Error>(content_);
}

std::string YamlRpcMessage::getId() const {
    switch (type_) {
        case Type::REQUEST: return getRequest().id;
        case Type::RESPONSE: return getResponse().id;
        case Type::ERROR: return getError().id;
    }
    return "";
}

std::shared_ptr<YamlRpcMessage> YamlRpcMessage::createRequest(const std::string& method, 
                                                            const std::shared_ptr<akao::foundation::formats::yaml::YamlNode>& params,
                                                            const std::string& id) {
    Request req;
    req.method = method;
    req.params = params;
    req.id = id;
    return std::make_shared<YamlRpcMessage>(req);
}

std::shared_ptr<YamlRpcMessage> YamlRpcMessage::createResponse(const std::shared_ptr<akao::foundation::formats::yaml::YamlNode>& result,
                                                             const std::string& id) {
    Response resp;
    resp.result = result;
    resp.id = id;
    return std::make_shared<YamlRpcMessage>(resp);
}

std::shared_ptr<YamlRpcMessage> YamlRpcMessage::createError(int code, const std::string& message, const std::string& id,
                                                           const std::shared_ptr<akao::foundation::formats::yaml::YamlNode>& data) {
    Error err;
    err.code = code;
    err.message = message;
    err.data = data;
    err.id = id;
    return std::make_shared<YamlRpcMessage>(err);
}

// =============================================================================
// UnixSocketTransport Implementation
// =============================================================================

UnixSocketTransport::UnixSocketTransport() : socket_fd_(-1), is_connected_(false) {}

UnixSocketTransport::~UnixSocketTransport() {
    cleanup();
}

bool UnixSocketTransport::connect(const std::string& socket_path) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (is_connected_) {
        return true;
    }
    
    socket_fd_ = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socket_fd_ < 0) {
        return false;
    }
    
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path.c_str(), sizeof(addr.sun_path) - 1);
    
    if (::connect(socket_fd_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        ::close(socket_fd_);
        socket_fd_ = -1;
        return false;
    }
    
    socket_path_ = socket_path;
    is_connected_ = true;
    return true;
}

void UnixSocketTransport::disconnect() {
    std::lock_guard<std::mutex> lock(mutex_);
    cleanup();
}

bool UnixSocketTransport::bind(const std::string& socket_path) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    socket_fd_ = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socket_fd_ < 0) {
        return false;
    }
    
    // Remove existing socket file
    unlink(socket_path.c_str());
    
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path.c_str(), sizeof(addr.sun_path) - 1);
    
    if (::bind(socket_fd_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        ::close(socket_fd_);
        socket_fd_ = -1;
        return false;
    }
    
    if (listen(socket_fd_, 5) < 0) {
        ::close(socket_fd_);
        socket_fd_ = -1;
        return false;
    }
    
    socket_path_ = socket_path;
    is_connected_ = true;
    return true;
}

int UnixSocketTransport::accept() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!is_connected_ || socket_fd_ < 0) {
        return -1;
    }
    
    return ::accept(socket_fd_, nullptr, nullptr);
}

void UnixSocketTransport::close() {
    std::lock_guard<std::mutex> lock(mutex_);
    cleanup();
}

bool UnixSocketTransport::send(const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    return sendWithFraming(socket_fd_, message);
}

std::string UnixSocketTransport::receive() {
    std::lock_guard<std::mutex> lock(mutex_);
    return receiveWithFraming(socket_fd_);
}

bool UnixSocketTransport::send(int fd, const std::string& message) {
    return sendWithFraming(fd, message);
}

std::string UnixSocketTransport::receive(int fd) {
    return receiveWithFraming(fd);
}

bool UnixSocketTransport::sendWithFraming(int fd, const std::string& message) {
    if (fd < 0) {
        return false;
    }
    
    // Send message length first (4 bytes)
    uint32_t length = message.length();
    if (::send(fd, &length, sizeof(length), 0) != sizeof(length)) {
        return false;
    }
    
    // Send message content
    size_t sent = 0;
    while (sent < message.length()) {
        ssize_t result = ::send(fd, message.data() + sent, message.length() - sent, 0);
        if (result <= 0) {
            return false;
        }
        sent += result;
    }
    
    return true;
}

std::string UnixSocketTransport::receiveWithFraming(int fd) {
    if (fd < 0) {
        return "";
    }
    
    // Receive message length first
    uint32_t length;
    if (recv(fd, &length, sizeof(length), MSG_WAITALL) != sizeof(length)) {
        return "";
    }
    
    // Receive message content
    std::string message(length, '\0');
    if (recv(fd, &message[0], length, MSG_WAITALL) != static_cast<ssize_t>(length)) {
        return "";
    }
    
    return message;
}

void UnixSocketTransport::cleanup() {
    if (socket_fd_ >= 0) {
        ::close(socket_fd_);
        socket_fd_ = -1;
    }
    
    if (!socket_path_.empty()) {
        unlink(socket_path_.c_str());
        socket_path_.clear();
    }
    
    is_connected_ = false;
}

// =============================================================================
// YamlRpcClient Implementation
// =============================================================================

YamlRpcClient::YamlRpcClient() : transport_(std::make_unique<UnixSocketTransport>()), request_counter_(0), running_(false) {}

YamlRpcClient::~YamlRpcClient() {
    disconnect();
}

bool YamlRpcClient::connect(const std::string& socket_path) {
    if (transport_->connect(socket_path)) {
        running_ = true;
        response_thread_ = std::thread(&YamlRpcClient::handleResponses, this);
        return true;
    }
    return false;
}

void YamlRpcClient::disconnect() {
    if (running_) {
        running_ = false;
        transport_->disconnect();
        if (response_thread_.joinable()) {
            response_thread_.join();
        }
    }
}

std::shared_ptr<YamlRpcMessage> YamlRpcClient::call(const std::string& method,
                                                   const std::shared_ptr<akao::foundation::formats::yaml::YamlNode>& params) {
    if (!transport_->isConnected()) {
        return YamlRpcMessage::createError(YamlRpcMessage::INTERNAL_ERROR, "Not connected", "");
    }
    
    std::string request_id = generateRequestId();
    auto request = YamlRpcMessage::createRequest(method, params, request_id);
    
    // Set up promise for response
    std::promise<std::shared_ptr<YamlRpcMessage>> promise;
    auto future = promise.get_future();
    
    {
        std::lock_guard<std::mutex> lock(pending_mutex_);
        pending_requests_[request_id] = std::move(promise);
    }
    
    // Send request
    if (!transport_->send(request->toYaml())) {
        std::lock_guard<std::mutex> lock(pending_mutex_);
        pending_requests_.erase(request_id);
        return YamlRpcMessage::createError(YamlRpcMessage::INTERNAL_ERROR, "Send failed", request_id);
    }
    
    // Wait for response (with timeout)
    if (future.wait_for(std::chrono::seconds(30)) == std::future_status::timeout) {
        std::lock_guard<std::mutex> lock(pending_mutex_);
        pending_requests_.erase(request_id);
        return YamlRpcMessage::createError(YamlRpcMessage::NODE_EXECUTION_TIMEOUT, "Request timeout", request_id);
    }
    
    return future.get();
}

std::shared_ptr<YamlRpcMessage> YamlRpcClient::nodeInfo() {
    return call("node.info");
}

std::shared_ptr<YamlRpcMessage> YamlRpcClient::nodeValidate(const std::shared_ptr<akao::foundation::formats::yaml::YamlNode>& input) {
    auto params = akao::foundation::formats::yaml::YamlNode::createMapping();
    params->setMapping("input", input);
    return call("node.validate", params);
}

std::shared_ptr<YamlRpcMessage> YamlRpcClient::nodeExecute(const std::shared_ptr<akao::foundation::formats::yaml::YamlNode>& input,
                                                          const std::shared_ptr<akao::foundation::formats::yaml::YamlNode>& context) {
    auto params = akao::foundation::formats::yaml::YamlNode::createMapping();
    params->setMapping("input", input);
    params->setMapping("context", context);
    return call("node.execute", params);
}

std::shared_ptr<YamlRpcMessage> YamlRpcClient::nodeHealth() {
    return call("node.health");
}

std::shared_ptr<YamlRpcMessage> YamlRpcClient::nodeShutdown(int timeout_seconds) {
    auto params = akao::foundation::formats::yaml::YamlNode::createMapping();
    params->setMapping("timeout_seconds", akao::foundation::formats::yaml::YamlNode::createInteger(timeout_seconds));
    return call("node.shutdown", params);
}

std::string YamlRpcClient::generateRequestId() {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    return "req-" + std::to_string(timestamp) + "-" + std::to_string(request_counter_++);
}

void YamlRpcClient::handleResponses() {
    while (running_) {
        std::string response_yaml = transport_->receive();
        if (response_yaml.empty()) {
            continue;
        }
        
        auto response = YamlRpcMessage::fromYaml(response_yaml);
        if (response) {
            processResponse(response);
        }
    }
}

void YamlRpcClient::processResponse(const std::shared_ptr<YamlRpcMessage>& response) {
    std::string id = response->getId();
    if (id.empty()) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pending_mutex_);
    auto it = pending_requests_.find(id);
    if (it != pending_requests_.end()) {
        it->second.set_value(response);
        pending_requests_.erase(it);
    }
}

// =============================================================================
// NodeCommunicator Implementation
// =============================================================================

NodeCommunicator::NodeCommunicator() {}

NodeCommunicator::~NodeCommunicator() {
    disconnectAll();
}

bool NodeCommunicator::connectToNode(const std::string& node_id, const std::string& socket_path) {
    std::lock_guard<std::mutex> lock(clients_mutex_);
    
    auto client = std::make_unique<YamlRpcClient>();
    if (client->connect(socket_path)) {
        clients_[node_id] = std::move(client);
        return true;
    }
    return false;
}

void NodeCommunicator::disconnectFromNode(const std::string& node_id) {
    std::lock_guard<std::mutex> lock(clients_mutex_);
    clients_.erase(node_id);
}

void NodeCommunicator::disconnectAll() {
    std::lock_guard<std::mutex> lock(clients_mutex_);
    clients_.clear();
}

std::shared_ptr<YamlRpcMessage> NodeCommunicator::nodeInfo(const std::string& node_id) {
    auto client = getClient(node_id);
    return client ? client->nodeInfo() : 
           YamlRpcMessage::createError(YamlRpcMessage::INTERNAL_ERROR, "Node not connected", "");
}

std::shared_ptr<YamlRpcMessage> NodeCommunicator::nodeValidate(const std::string& node_id, 
                                                              const std::shared_ptr<akao::foundation::formats::yaml::YamlNode>& input) {
    auto client = getClient(node_id);
    return client ? client->nodeValidate(input) : 
           YamlRpcMessage::createError(YamlRpcMessage::INTERNAL_ERROR, "Node not connected", "");
}

std::shared_ptr<YamlRpcMessage> NodeCommunicator::nodeExecute(const std::string& node_id,
                                                             const std::shared_ptr<akao::foundation::formats::yaml::YamlNode>& input,
                                                             const std::shared_ptr<akao::foundation::formats::yaml::YamlNode>& context) {
    auto client = getClient(node_id);
    return client ? client->nodeExecute(input, context) : 
           YamlRpcMessage::createError(YamlRpcMessage::INTERNAL_ERROR, "Node not connected", "");
}

std::shared_ptr<YamlRpcMessage> NodeCommunicator::nodeHealth(const std::string& node_id) {
    auto client = getClient(node_id);
    return client ? client->nodeHealth() : 
           YamlRpcMessage::createError(YamlRpcMessage::INTERNAL_ERROR, "Node not connected", "");
}

void NodeCommunicator::nodeShutdown(const std::string& node_id, int timeout_seconds) {
    auto client = getClient(node_id);
    if (client) {
        client->nodeShutdown(timeout_seconds);
    }
}

bool NodeCommunicator::isConnected(const std::string& node_id) const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(clients_mutex_));
    auto it = clients_.find(node_id);
    return it != clients_.end() && it->second->isConnected();
}

YamlRpcClient* NodeCommunicator::getClient(const std::string& node_id) {
    std::lock_guard<std::mutex> lock(clients_mutex_);
    auto it = clients_.find(node_id);
    return it != clients_.end() ? it->second.get() : nullptr;
}

} // namespace yamlrpc
} // namespace communication
} // namespace engine
} // namespace core
} // namespace akao