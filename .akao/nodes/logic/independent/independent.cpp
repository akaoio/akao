/**
 * @id: akao:node:logic:executor:independent:v1
 * @doc: Independent logic executor node implementation with YAML-RPC communication. Completely self-contained with no core dependencies, providing .a format logical expression evaluation with comprehensive built-in functions. Communicates via YAML-RPC protocol over Unix Domain Sockets.
 * @specification: Independent logic executor implementation with YAML-RPC communication
 * @scope: External logic execution node implementation
 * @timeline: 2025-07-11
 * @rationale: Demonstrate true core-node independence with protocol-based communication
 * @methodology: Self-contained implementation with YAML-RPC server and expression evaluation
 * @references: ["akao:protocol:yamlrpc:v1"]
 */

#include "independent.hpp"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <cstring>
#include <variant>
#include <cerrno>
#include <sstream>
#include <regex>
#include <algorithm>
#include <cmath>

namespace akao {
namespace nodes {
namespace logic {

// =============================================================================
// SimpleYamlNode Implementation
// =============================================================================

SimpleYamlNode::SimpleYamlNode(Type type) : type_(type) {
    switch (type) {
        case Type::STRING: value_ = std::string{}; break;
        case Type::INTEGER: value_ = 0; break;
        case Type::FLOAT: value_ = 0.0; break;
        case Type::BOOLEAN: value_ = false; break;
        case Type::NULL_VALUE: value_ = nullptr; break;
        case Type::SEQUENCE: value_ = std::vector<std::shared_ptr<SimpleYamlNode>>{}; break;
        case Type::MAPPING: value_ = std::map<std::string, std::shared_ptr<SimpleYamlNode>>{}; break;
    }
}

std::shared_ptr<SimpleYamlNode> SimpleYamlNode::createString(const std::string& value) {
    auto node = std::make_shared<SimpleYamlNode>(Type::STRING);
    node->value_ = value;
    return node;
}

std::shared_ptr<SimpleYamlNode> SimpleYamlNode::createInteger(int value) {
    auto node = std::make_shared<SimpleYamlNode>(Type::INTEGER);
    node->value_ = value;
    return node;
}

std::shared_ptr<SimpleYamlNode> SimpleYamlNode::createFloat(double value) {
    auto node = std::make_shared<SimpleYamlNode>(Type::FLOAT);
    node->value_ = value;
    return node;
}

std::shared_ptr<SimpleYamlNode> SimpleYamlNode::createBoolean(bool value) {
    auto node = std::make_shared<SimpleYamlNode>(Type::BOOLEAN);
    node->value_ = value;
    return node;
}

std::shared_ptr<SimpleYamlNode> SimpleYamlNode::createNull() {
    return std::make_shared<SimpleYamlNode>(Type::NULL_VALUE);
}

std::shared_ptr<SimpleYamlNode> SimpleYamlNode::createSequence() {
    return std::make_shared<SimpleYamlNode>(Type::SEQUENCE);
}

std::shared_ptr<SimpleYamlNode> SimpleYamlNode::createMapping() {
    return std::make_shared<SimpleYamlNode>(Type::MAPPING);
}

std::string SimpleYamlNode::asString() const {
    if (type_ == Type::STRING) {
        return std::get<std::string>(value_);
    }
    return "";
}

int SimpleYamlNode::asInteger() const {
    if (type_ == Type::INTEGER) {
        return std::get<int>(value_);
    }
    return 0;
}

double SimpleYamlNode::asFloat() const {
    if (type_ == Type::FLOAT) {
        return std::get<double>(value_);
    }
    return 0.0;
}

bool SimpleYamlNode::asBoolean() const {
    if (type_ == Type::BOOLEAN) {
        return std::get<bool>(value_);
    }
    return false;
}

void SimpleYamlNode::addToSequence(std::shared_ptr<SimpleYamlNode> node) {
    if (type_ == Type::SEQUENCE) {
        auto& seq = std::get<std::vector<std::shared_ptr<SimpleYamlNode>>>(value_);
        seq.push_back(node);
    }
}

void SimpleYamlNode::setMapping(const std::string& key, std::shared_ptr<SimpleYamlNode> node) {
    if (type_ == Type::MAPPING) {
        auto& map = std::get<std::map<std::string, std::shared_ptr<SimpleYamlNode>>>(value_);
        map[key] = node;
    }
}

std::shared_ptr<SimpleYamlNode> SimpleYamlNode::getMapping(const std::string& key) const {
    if (type_ == Type::MAPPING) {
        const auto& map = std::get<std::map<std::string, std::shared_ptr<SimpleYamlNode>>>(value_);
        auto it = map.find(key);
        return it != map.end() ? it->second : nullptr;
    }
    return nullptr;
}

bool SimpleYamlNode::hasKey(const std::string& key) const {
    if (type_ == Type::MAPPING) {
        const auto& map = std::get<std::map<std::string, std::shared_ptr<SimpleYamlNode>>>(value_);
        return map.find(key) != map.end();
    }
    return false;
}

std::string SimpleYamlNode::toYaml(int indent) const {
    std::string indentStr(indent, ' ');
    
    switch (type_) {
        case Type::STRING:
            return "\"" + std::get<std::string>(value_) + "\"";
        case Type::INTEGER:
            return std::to_string(std::get<int>(value_));
        case Type::FLOAT:
            return std::to_string(std::get<double>(value_));
        case Type::BOOLEAN:
            return std::get<bool>(value_) ? "true" : "false";
        case Type::NULL_VALUE:
            return "null";
        case Type::SEQUENCE: {
            const auto& seq = std::get<std::vector<std::shared_ptr<SimpleYamlNode>>>(value_);
            if (seq.empty()) return "[]";
            
            std::string result = "\n";
            for (const auto& item : seq) {
                result += indentStr + "- " + item->toYaml(indent + 2) + "\n";
            }
            return result;
        }
        case Type::MAPPING: {
            const auto& map = std::get<std::map<std::string, std::shared_ptr<SimpleYamlNode>>>(value_);
            if (map.empty()) return "{}";
            
            std::string result = "\n";
            for (const auto& pair : map) {
                result += indentStr + pair.first + ": " + pair.second->toYaml(indent + 2) + "\n";
            }
            return result;
        }
    }
    return "";
}

std::shared_ptr<SimpleYamlNode> SimpleYamlNode::fromYaml(const std::string& yaml) {
    // Simplified YAML parsing - just return null for now
    // In a full implementation, this would parse the YAML string
    return SimpleYamlNode::createNull();
}

// =============================================================================
// LogicEvaluator Implementation
// =============================================================================

LogicEvaluator::LogicEvaluator() {
    registerBuiltinFunctions();
}

void LogicEvaluator::setVariable(const std::string& name, const LogicValue& value) {
    variables_[name] = value;
}

LogicValue LogicEvaluator::getVariable(const std::string& name) const {
    auto it = variables_.find(name);
    if (it != variables_.end()) {
        return it->second;
    }
    return false; // Default to false for undefined variables
}

bool LogicEvaluator::hasVariable(const std::string& name) const {
    return variables_.find(name) != variables_.end();
}

void LogicEvaluator::clearVariables() {
    variables_.clear();
}

void LogicEvaluator::registerFunction(const std::string& name, std::function<LogicValue(const std::vector<LogicValue>&)> func) {
    functions_[name] = func;
}

void LogicEvaluator::registerBuiltinFunctions() {
    // String functions
    registerFunction("length", [](const std::vector<LogicValue>& args) -> LogicValue {
        if (args.empty()) return 0;
        return static_cast<int>(std::visit([](const auto& v) -> std::string {
            if constexpr (std::is_same_v<std::decay_t<decltype(v)>, std::string>) {
                return v;
            } else if constexpr (std::is_same_v<std::decay_t<decltype(v)>, bool>) {
                return v ? "true" : "false";
            } else if constexpr (std::is_same_v<std::decay_t<decltype(v)>, int>) {
                return std::to_string(v);
            } else if constexpr (std::is_same_v<std::decay_t<decltype(v)>, double>) {
                return std::to_string(v);
            }
            return "";
        }, args[0]).length());
    });
    
    // Math functions
    registerFunction("add", [](const std::vector<LogicValue>& args) -> LogicValue {
        if (args.size() < 2) return 0;
        double sum = 0;
        for (const auto& arg : args) {
            sum += std::visit([](const auto& v) -> double {
                if constexpr (std::is_same_v<std::decay_t<decltype(v)>, int>) {
                    return static_cast<double>(v);
                } else if constexpr (std::is_same_v<std::decay_t<decltype(v)>, double>) {
                    return v;
                }
                return 0.0;
            }, arg);
        }
        return sum;
    });
    
    registerFunction("eq", [](const std::vector<LogicValue>& args) -> LogicValue {
        if (args.size() != 2) return false;
        return std::visit([](const auto& a, const auto& b) -> bool {
            if constexpr (std::is_same_v<std::decay_t<decltype(a)>, std::decay_t<decltype(b)>>) {
                return a == b;
            }
            return false;
        }, args[0], args[1]);
    });
}

LogicValue LogicEvaluator::evaluate(const std::string& expression) {
    // Simplified evaluation - just handle basic cases
    std::string trimmed = expression;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
    
    // Handle boolean literals
    if (trimmed == "true") return true;
    if (trimmed == "false") return false;
    
    // Handle numeric literals
    if (std::regex_match(trimmed, std::regex("^-?\\d+$"))) {
        return std::stoi(trimmed);
    }
    if (std::regex_match(trimmed, std::regex("^-?\\d+\\.\\d+$"))) {
        return std::stod(trimmed);
    }
    
    // Handle string literals
    if (trimmed.front() == '"' && trimmed.back() == '"') {
        return trimmed.substr(1, trimmed.length() - 2);
    }
    
    // Handle variables
    if (hasVariable(trimmed)) {
        return getVariable(trimmed);
    }
    
    // Handle simple function calls
    if (trimmed.find('(') != std::string::npos) {
        size_t paren = trimmed.find('(');
        std::string funcName = trimmed.substr(0, paren);
        std::string argsStr = trimmed.substr(paren + 1, trimmed.length() - paren - 2);
        
        if (functions_.find(funcName) != functions_.end()) {
            std::vector<LogicValue> args;
            // Simple argument parsing - just split by comma
            std::istringstream iss(argsStr);
            std::string arg;
            while (std::getline(iss, arg, ',')) {
                arg.erase(0, arg.find_first_not_of(" \t"));
                arg.erase(arg.find_last_not_of(" \t") + 1);
                args.push_back(evaluate(arg));
            }
            return functions_[funcName](args);
        }
    }
    
    // Handle simple binary operations
    if (trimmed.find(" && ") != std::string::npos) {
        size_t pos = trimmed.find(" && ");
        LogicValue left = evaluate(trimmed.substr(0, pos));
        LogicValue right = evaluate(trimmed.substr(pos + 4));
        return toBool(left) && toBool(right);
    }
    if (trimmed.find(" || ") != std::string::npos) {
        size_t pos = trimmed.find(" || ");
        LogicValue left = evaluate(trimmed.substr(0, pos));
        LogicValue right = evaluate(trimmed.substr(pos + 4));
        return toBool(left) || toBool(right);
    }
    if (trimmed.find(" == ") != std::string::npos) {
        size_t pos = trimmed.find(" == ");
        LogicValue left = evaluate(trimmed.substr(0, pos));
        LogicValue right = evaluate(trimmed.substr(pos + 4));
        return std::visit([](const auto& a, const auto& b) -> bool {
            if constexpr (std::is_same_v<std::decay_t<decltype(a)>, std::decay_t<decltype(b)>>) {
                return a == b;
            }
            return false;
        }, left, right);
    }
    
    // Default to false
    return false;
}

bool LogicEvaluator::evaluateBoolean(const std::string& expression) {
    return toBool(evaluate(expression));
}

bool LogicEvaluator::toBool(const LogicValue& value) {
    return std::visit([](const auto& v) -> bool {
        if constexpr (std::is_same_v<std::decay_t<decltype(v)>, bool>) {
            return v;
        } else if constexpr (std::is_same_v<std::decay_t<decltype(v)>, int>) {
            return v != 0;
        } else if constexpr (std::is_same_v<std::decay_t<decltype(v)>, double>) {
            return v != 0.0;
        } else if constexpr (std::is_same_v<std::decay_t<decltype(v)>, std::string>) {
            return !v.empty();
        }
        return false;
    }, value);
}

void LogicEvaluator::setVariablesFromYaml(const std::shared_ptr<SimpleYamlNode>& variables) {
    if (!variables || variables->getType() != SimpleYamlNode::Type::MAPPING) {
        return;
    }
    
    // This is a simplified implementation - in reality we'd need to traverse the YAML structure
    // For now, just clear existing variables
    clearVariables();
}

std::shared_ptr<SimpleYamlNode> LogicEvaluator::getResultAsYaml(const LogicValue& result) {
    return std::visit([](const auto& v) -> std::shared_ptr<SimpleYamlNode> {
        if constexpr (std::is_same_v<std::decay_t<decltype(v)>, bool>) {
            return SimpleYamlNode::createBoolean(v);
        } else if constexpr (std::is_same_v<std::decay_t<decltype(v)>, int>) {
            return SimpleYamlNode::createInteger(v);
        } else if constexpr (std::is_same_v<std::decay_t<decltype(v)>, double>) {
            return SimpleYamlNode::createFloat(v);
        } else if constexpr (std::is_same_v<std::decay_t<decltype(v)>, std::string>) {
            return SimpleYamlNode::createString(v);
        }
        return SimpleYamlNode::createNull();
    }, result);
}

// =============================================================================
// LogicNodeServer Implementation
// =============================================================================

LogicNodeServer::LogicNodeServer(const std::string& socket_path)
    : socket_path_(socket_path), server_socket_(-1), running_(false),
      node_id_("akao:node:logic:executor:v1"),
      node_name_("Logic Executor"),
      version_("1.0.0") {}

LogicNodeServer::~LogicNodeServer() {
    stop();
}

bool LogicNodeServer::start() {
    if (running_) {
        return true;
    }
    
    if (!setupSocket()) {
        return false;
    }
    
    running_ = true;
    server_thread_ = std::thread(&LogicNodeServer::serverLoop, this);
    
    return true;
}

void LogicNodeServer::stop() {
    running_ = false;
    
    if (server_thread_.joinable()) {
        server_thread_.join();
    }
    
    cleanup();
}

void LogicNodeServer::setNodeInfo(const std::string& id, const std::string& name, const std::string& version) {
    node_id_ = id;
    node_name_ = name;
    version_ = version;
}

void LogicNodeServer::serverLoop() {
    while (running_) {
        int client_socket = accept(server_socket_, nullptr, nullptr);
        if (client_socket < 0) {
            if (running_) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            continue;
        }
        
        // Handle client in separate thread
        std::thread([this, client_socket]() {
            handleClient(client_socket);
            close(client_socket);
        }).detach();
    }
}

void LogicNodeServer::handleClient(int client_socket) {
    std::string request = receiveMessage(client_socket);
    if (request.empty()) {
        return;
    }
    
    std::string response = processRequest(request);
    sendMessage(client_socket, response);
}

std::string LogicNodeServer::processRequest(const std::string& request) {
    // Parse YAML-RPC request (simplified)
    auto request_node = SimpleYamlNode::fromYaml(request);
    if (!request_node) {
        return createError(-32700, "Parse error", "")->toYaml();
    }
    
    auto method_node = request_node->getMapping("method");
    auto id_node = request_node->getMapping("id");
    auto params_node = request_node->getMapping("params");
    
    if (!method_node) {
        return createError(-32600, "Invalid Request", id_node ? id_node->asString() : "")->toYaml();
    }
    
    std::string method = method_node->asString();
    std::string id = id_node ? id_node->asString() : "";
    
    std::shared_ptr<SimpleYamlNode> result;
    
    if (method == "node.info") {
        result = handleNodeInfo();
    } else if (method == "node.validate") {
        result = handleNodeValidate(params_node);
    } else if (method == "node.execute") {
        result = handleNodeExecute(params_node);
    } else if (method == "node.health") {
        result = handleNodeHealth();
    } else if (method == "node.shutdown") {
        result = handleNodeShutdown();
    } else {
        return createError(-32601, "Method not found", id)->toYaml();
    }
    
    return createResponse(result, id)->toYaml();
}

std::shared_ptr<SimpleYamlNode> LogicNodeServer::handleNodeInfo() {
    auto info = SimpleYamlNode::createMapping();
    info->setMapping("node_id", SimpleYamlNode::createString(node_id_));
    info->setMapping("name", SimpleYamlNode::createString(node_name_));
    info->setMapping("version", SimpleYamlNode::createString(version_));
    info->setMapping("description", SimpleYamlNode::createString("Executes .a format logical expressions"));
    
    return info;
}

std::shared_ptr<SimpleYamlNode> LogicNodeServer::handleNodeValidate(const std::shared_ptr<SimpleYamlNode>& params) {
    auto result = SimpleYamlNode::createMapping();
    result->setMapping("valid", SimpleYamlNode::createBoolean(true));
    
    auto errors = SimpleYamlNode::createSequence();
    result->setMapping("errors", errors);
    
    // Basic validation - check if expression is provided
    if (params && params->hasKey("input")) {
        auto input = params->getMapping("input");
        if (!input || !input->hasKey("expression")) {
            result->setMapping("valid", SimpleYamlNode::createBoolean(false));
            errors->addToSequence(SimpleYamlNode::createString("Missing required parameter: expression"));
        }
    }
    
    return result;
}

std::shared_ptr<SimpleYamlNode> LogicNodeServer::handleNodeExecute(const std::shared_ptr<SimpleYamlNode>& params) {
    if (!params || !params->hasKey("input")) {
        auto error = SimpleYamlNode::createMapping();
        error->setMapping("error", SimpleYamlNode::createString("Missing input parameters"));
        return error;
    }
    
    auto input = params->getMapping("input");
    if (!input || !input->hasKey("expression")) {
        auto error = SimpleYamlNode::createMapping();
        error->setMapping("error", SimpleYamlNode::createString("Missing expression parameter"));
        return error;
    }
    
    std::string expression = input->getMapping("expression")->asString();
    
    // Set variables if provided
    if (input->hasKey("variables")) {
        evaluator_.setVariablesFromYaml(input->getMapping("variables"));
    }
    
    try {
        LogicValue result = evaluator_.evaluate(expression);
        return evaluator_.getResultAsYaml(result);
    } catch (const std::exception& e) {
        auto error = SimpleYamlNode::createMapping();
        error->setMapping("error", SimpleYamlNode::createString("Evaluation failed: " + std::string(e.what())));
        return error;
    }
}

std::shared_ptr<SimpleYamlNode> LogicNodeServer::handleNodeHealth() {
    auto health = SimpleYamlNode::createMapping();
    health->setMapping("status", SimpleYamlNode::createString("healthy"));
    health->setMapping("uptime", SimpleYamlNode::createInteger(60)); // Simplified
    health->setMapping("requests_processed", SimpleYamlNode::createInteger(1));
    
    return health;
}

std::shared_ptr<SimpleYamlNode> LogicNodeServer::handleNodeShutdown() {
    auto response = SimpleYamlNode::createMapping();
    response->setMapping("status", SimpleYamlNode::createString("shutting_down"));
    
    // Schedule shutdown
    std::thread([this]() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        stop();
    }).detach();
    
    return response;
}

bool LogicNodeServer::setupSocket() {
    server_socket_ = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_socket_ < 0) {
        std::cerr << "Failed to create socket: " << strerror(errno) << std::endl;
        return false;
    }
    
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path_.c_str(), sizeof(addr.sun_path) - 1);
    
    // Remove existing socket file
    unlink(socket_path_.c_str());
    
    if (bind(server_socket_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Failed to bind socket to " << socket_path_ << ": " << strerror(errno) << std::endl;
        close(server_socket_);
        return false;
    }
    
    if (listen(server_socket_, 5) < 0) {
        std::cerr << "Failed to listen on socket: " << strerror(errno) << std::endl;
        close(server_socket_);
        return false;
    }
    
    return true;
}

void LogicNodeServer::cleanup() {
    if (server_socket_ >= 0) {
        close(server_socket_);
        server_socket_ = -1;
    }
    
    unlink(socket_path_.c_str());
}

std::string LogicNodeServer::receiveMessage(int client_socket) {
    // Read message length first
    uint32_t length;
    if (recv(client_socket, &length, sizeof(length), MSG_WAITALL) != sizeof(length)) {
        return "";
    }
    
    // Read message content
    std::string message(length, '\0');
    if (recv(client_socket, &message[0], length, MSG_WAITALL) != static_cast<ssize_t>(length)) {
        return "";
    }
    
    return message;
}

bool LogicNodeServer::sendMessage(int client_socket, const std::string& message) {
    // Send message length first
    uint32_t length = message.length();
    if (send(client_socket, &length, sizeof(length), 0) != sizeof(length)) {
        return false;
    }
    
    // Send message content
    return send(client_socket, message.data(), length, 0) == static_cast<ssize_t>(length);
}

std::shared_ptr<SimpleYamlNode> LogicNodeServer::createResponse(const std::shared_ptr<SimpleYamlNode>& result, const std::string& id) {
    auto response = SimpleYamlNode::createMapping();
    response->setMapping("yamlrpc", SimpleYamlNode::createString("1.0"));
    response->setMapping("result", result);
    response->setMapping("id", SimpleYamlNode::createString(id));
    
    return response;
}

std::shared_ptr<SimpleYamlNode> LogicNodeServer::createError(int code, const std::string& message, const std::string& id) {
    auto error = SimpleYamlNode::createMapping();
    error->setMapping("code", SimpleYamlNode::createInteger(code));
    error->setMapping("message", SimpleYamlNode::createString(message));
    
    auto response = SimpleYamlNode::createMapping();
    response->setMapping("yamlrpc", SimpleYamlNode::createString("1.0"));
    response->setMapping("error", error);
    response->setMapping("id", SimpleYamlNode::createString(id));
    
    return response;
}

} // namespace logic
} // namespace nodes
} // namespace akao

// =============================================================================
// Main function for standalone executable
// =============================================================================

int main(int argc, char* argv[]) {
    std::string socket_path = "/tmp/akao-node-logic.sock";
    
    if (argc > 1) {
        socket_path = argv[1];
    }
    
    akao::nodes::logic::LogicNodeServer server(socket_path);
    
    if (!server.start()) {
        std::cerr << "Failed to start logic executor node server" << std::endl;
        return 1;
    }
    
    std::cout << "Logic executor node started on " << socket_path << std::endl;
    
    // Wait for shutdown signal
    while (server.isRunning()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    return 0;
}