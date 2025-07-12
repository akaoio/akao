/**
 * @id: akao:node:logic:executor:independent:v1
 * @doc: Independent logic executor node with YAML-RPC communication. Completely self-contained with no core dependencies, providing .a format logical expression evaluation with comprehensive built-in functions. Communicates via YAML-RPC protocol over Unix Domain Sockets.
 * @specification: Independent logic executor with YAML-RPC communication
 * @scope: External logic execution node
 * @timeline: 2025-07-11
 * @rationale: Demonstrate core-node independence for logic processing
 * @methodology: Self-contained implementation with YAML-RPC server and expression evaluation
 * @references: ["akao:protocol:yamlrpc:v1"]
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <variant>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <stdexcept>

namespace akao {
namespace nodes {
namespace logic {

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
 * Logic value type for simplified expression evaluation
 */
using LogicValue = std::variant<bool, int, double, std::string>;

/**
 * Simplified expression evaluator for .a format
 */
class LogicEvaluator {
private:
    std::map<std::string, LogicValue> variables_;
    std::map<std::string, std::function<LogicValue(const std::vector<LogicValue>&)>> functions_;
    
    // Expression parsing and evaluation
    LogicValue parseExpression(const std::string& expr);
    LogicValue evaluateToken(const std::string& token);
    LogicValue evaluateBinaryOp(const std::string& op, const LogicValue& left, const LogicValue& right);
    LogicValue evaluateUnaryOp(const std::string& op, const LogicValue& operand);
    LogicValue evaluateFunction(const std::string& name, const std::vector<LogicValue>& args);
    
    // Helper methods
    std::vector<std::string> tokenize(const std::string& expr);
    bool isOperator(const std::string& token);
    bool isFunction(const std::string& token);
    bool isVariable(const std::string& token);
    bool isLiteral(const std::string& token);
    
    // Type conversion
    bool toBool(const LogicValue& value);
    int toInt(const LogicValue& value);
    double toFloat(const LogicValue& value);
    std::string toString(const LogicValue& value);
    
public:
    LogicEvaluator();
    
    // Variable management
    void setVariable(const std::string& name, const LogicValue& value);
    LogicValue getVariable(const std::string& name) const;
    bool hasVariable(const std::string& name) const;
    void clearVariables();
    
    // Function management
    void registerFunction(const std::string& name, std::function<LogicValue(const std::vector<LogicValue>&)> func);
    void registerBuiltinFunctions();
    
    // Main evaluation
    LogicValue evaluate(const std::string& expression);
    bool evaluateBoolean(const std::string& expression);
    
    // Convert from/to YAML
    void setVariablesFromYaml(const std::shared_ptr<SimpleYamlNode>& variables);
    std::shared_ptr<SimpleYamlNode> getResultAsYaml(const LogicValue& result);
};

/**
 * YAML-RPC server for the logic executor node
 */
class LogicNodeServer {
private:
    std::string socket_path_;
    int server_socket_;
    std::atomic<bool> running_;
    std::thread server_thread_;
    
    LogicEvaluator evaluator_;
    
    // Node info
    std::string node_id_;
    std::string node_name_;
    std::string version_;
    
public:
    LogicNodeServer(const std::string& socket_path);
    ~LogicNodeServer();
    
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

} // namespace logic
} // namespace nodes
} // namespace akao