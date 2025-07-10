/**
 * @id: akao:class:nodes:builtin:logic:v1
 * @doc: Clean .a format executor node providing logical expression evaluation capabilities. Inspired by legacy Pure Logic Engine architecture but implemented as standalone component with simplified syntax support, built-in functions, and zero external dependencies for powerful validation workflows.
 * @specification: .a format executor node with logical expression evaluation
 * @scope: Builtin logic execution node
 * @timeline: 2025-07-10
 * @rationale: Enable sophisticated logical expression evaluation for validation rules
 * @methodology: Clean implementation learning from legacy Pure Logic Engine patterns
 * @references: ["akao:interface:core:foundation:interfaces:inode:v1"]
 */

#pragma once

#include "core/foundation/interfaces/inode/v1.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <variant>
#include <functional>

namespace akao {
namespace nodes {
namespace builtin {

/**
 * Value type for logic execution
 */
using LogicValue = std::variant<
    bool,
    int64_t,
    double,
    std::string,
    std::vector<foundation::types::NodeValue>
>;

/**
 * Execution context for variable scoping
 */
class LogicContext {
private:
    std::map<std::string, LogicValue> variables_;
    std::map<std::string, std::function<LogicValue(const std::vector<LogicValue>&)>> functions_;
    
public:
    LogicContext();
    ~LogicContext() = default;
    
    // Variable operations
    void setVariable(const std::string& name, const LogicValue& value);
    LogicValue getVariable(const std::string& name) const;
    bool hasVariable(const std::string& name) const;
    
    // Function operations
    void registerFunction(const std::string& name, std::function<LogicValue(const std::vector<LogicValue>&)> func);
    LogicValue callFunction(const std::string& name, const std::vector<LogicValue>& args) const;
    bool hasFunction(const std::string& name) const;
    
    // Utility
    std::vector<std::string> getVariableNames() const;
    std::vector<std::string> getFunctionNames() const;
};

/**
 * Simple expression evaluator for .a format
 */
class SimpleAkaoEvaluator {
private:
    LogicContext& context_;
    
    // Core evaluation
    LogicValue evaluateExpression(const std::string& expr);
    LogicValue evaluateBinaryOp(const std::string& left, const std::string& op, const std::string& right);
    LogicValue evaluateFunctionCall(const std::string& name, const std::vector<std::string>& args);
    LogicValue parseValue(const std::string& token);
    
    // Parsing helpers
    std::vector<std::string> tokenize(const std::string& expr);
    bool isBinaryOperator(const std::string& token);
    bool isValue(const std::string& token);
    std::string trim(const std::string& str);
    
public:
    SimpleAkaoEvaluator(LogicContext& context);
    LogicValue evaluate(const std::string& expression);
};

/**
 * Clean .a format executor node
 */
class AkaoLogicExecutorNode : public foundation::interfaces::INode {
    // Built-in function library
    void initializeBuiltinFunctions(LogicContext& context) const;
    
    // Conversion utilities
    LogicValue nodeValueToLogicValue(const foundation::types::NodeValue& value) const;
    foundation::types::NodeValue logicValueToNodeValue(const LogicValue& value) const;
    void populateContextFromParameters(LogicContext& context, const foundation::interfaces::NodeParameters& params) const;

private:
    std::string nodeId_;
    std::string nodeType_;
    std::string version_;
    std::string description_;

public:
    // Built-in functions (made public for testing)
    LogicValue func_string_length(const std::vector<LogicValue>& args) const;
    LogicValue func_string_concat(const std::vector<LogicValue>& args) const;
    LogicValue func_string_contains(const std::vector<LogicValue>& args) const;
    LogicValue func_math_add(const std::vector<LogicValue>& args) const;
    LogicValue func_math_subtract(const std::vector<LogicValue>& args) const;
    LogicValue func_math_multiply(const std::vector<LogicValue>& args) const;
    LogicValue func_math_divide(const std::vector<LogicValue>& args) const;
    LogicValue func_collection_count(const std::vector<LogicValue>& args) const;
    LogicValue func_collection_contains(const std::vector<LogicValue>& args) const;
    LogicValue func_filesystem_exists(const std::vector<LogicValue>& args) const;
    LogicValue func_filesystem_is_file(const std::vector<LogicValue>& args) const;
    LogicValue func_filesystem_is_directory(const std::vector<LogicValue>& args) const;
    
public:
    AkaoLogicExecutorNode();
    ~AkaoLogicExecutorNode() = default;
    
    // INode interface implementation
    std::string getNodeId() const override;
    std::string getNodeType() const override;
    std::string getVersion() const override;
    std::string getDescription() const override;
    
    foundation::interfaces::ValidationResult validate(const foundation::interfaces::NodeParameters& params) const override;
    foundation::types::ExecutionResult execute(const foundation::interfaces::NodeContext& context,
                                             const foundation::interfaces::NodeParameters& params) override;
    foundation::types::NodeValue getParameterSchema() const override;
    
    // Static utility methods for direct .a format execution
    static bool executeExpression(const std::string& expression);
    static bool executeExpressionWithContext(const std::string& expression, const std::map<std::string, foundation::types::NodeValue>& variables);
    static foundation::types::NodeValue evaluateExpression(const std::string& expression);
    static std::vector<std::string> validateExpression(const std::string& expression);
    static bool isValidExpression(const std::string& expression);
};

} // namespace builtin
} // namespace nodes
} // namespace akao