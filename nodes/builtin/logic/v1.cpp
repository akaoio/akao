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

#include "v1.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <stdexcept>
#include <filesystem>

namespace akao {
namespace nodes {
namespace builtin {

// LogicContext implementation
LogicContext::LogicContext() {
    // Initialize with empty state
}

void LogicContext::setVariable(const std::string& name, const LogicValue& value) {
    variables_[name] = value;
}

LogicValue LogicContext::getVariable(const std::string& name) const {
    auto it = variables_.find(name);
    if (it != variables_.end()) {
        return it->second;
    }
    throw std::runtime_error("Variable not found: " + name);
}

bool LogicContext::hasVariable(const std::string& name) const {
    return variables_.find(name) != variables_.end();
}

void LogicContext::registerFunction(const std::string& name, std::function<LogicValue(const std::vector<LogicValue>&)> func) {
    functions_[name] = func;
}

LogicValue LogicContext::callFunction(const std::string& name, const std::vector<LogicValue>& args) const {
    auto it = functions_.find(name);
    if (it != functions_.end()) {
        return it->second(args);
    }
    throw std::runtime_error("Function not found: " + name);
}

bool LogicContext::hasFunction(const std::string& name) const {
    return functions_.find(name) != functions_.end();
}

std::vector<std::string> LogicContext::getVariableNames() const {
    std::vector<std::string> names;
    for (const auto& pair : variables_) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> LogicContext::getFunctionNames() const {
    std::vector<std::string> names;
    for (const auto& pair : functions_) {
        names.push_back(pair.first);
    }
    return names;
}

// SimpleAkaoEvaluator implementation
SimpleAkaoEvaluator::SimpleAkaoEvaluator(LogicContext& context) : context_(context) {}

LogicValue SimpleAkaoEvaluator::evaluate(const std::string& expression) {
    return evaluateExpression(expression);
}

LogicValue SimpleAkaoEvaluator::evaluateExpression(const std::string& expr) {
    std::string trimmed = trim(expr);
    
    // Handle parentheses
    if (trimmed.front() == '(' && trimmed.back() == ')') {
        return evaluateExpression(trimmed.substr(1, trimmed.length() - 2));
    }
    
    // Check for function calls
    size_t parenPos = trimmed.find('(');
    if (parenPos != std::string::npos && trimmed.back() == ')') {
        std::string funcName = trim(trimmed.substr(0, parenPos));
        std::string argsStr = trimmed.substr(parenPos + 1, trimmed.length() - parenPos - 2);
        
        // Parse arguments
        std::vector<std::string> argTokens;
        if (!argsStr.empty()) {
            std::stringstream ss(argsStr);
            std::string arg;
            while (std::getline(ss, arg, ',')) {
                argTokens.push_back(trim(arg));
            }
        }
        
        return evaluateFunctionCall(funcName, argTokens);
    }
    
    // Check for binary operators
    std::vector<std::string> tokens = tokenize(trimmed);
    if (tokens.size() == 3 && isBinaryOperator(tokens[1])) {
        return evaluateBinaryOp(tokens[0], tokens[1], tokens[2]);
    }
    
    // Check for variables
    if (context_.hasVariable(trimmed)) {
        return context_.getVariable(trimmed);
    }
    
    // Parse as literal value
    return parseValue(trimmed);
}

LogicValue SimpleAkaoEvaluator::evaluateBinaryOp(const std::string& left, const std::string& op, const std::string& right) {
    LogicValue leftVal = evaluateExpression(left);
    LogicValue rightVal = evaluateExpression(right);
    
    if (op == "==") {
        return leftVal == rightVal;
    } else if (op == "!=") {
        return leftVal != rightVal;
    } else if (op == "&&") {
        return std::get<bool>(leftVal) && std::get<bool>(rightVal);
    } else if (op == "||") {
        return std::get<bool>(leftVal) || std::get<bool>(rightVal);
    } else if (op == "<") {
        if (std::holds_alternative<int64_t>(leftVal) && std::holds_alternative<int64_t>(rightVal)) {
            return std::get<int64_t>(leftVal) < std::get<int64_t>(rightVal);
        } else if (std::holds_alternative<double>(leftVal) && std::holds_alternative<double>(rightVal)) {
            return std::get<double>(leftVal) < std::get<double>(rightVal);
        }
    } else if (op == ">") {
        if (std::holds_alternative<int64_t>(leftVal) && std::holds_alternative<int64_t>(rightVal)) {
            return std::get<int64_t>(leftVal) > std::get<int64_t>(rightVal);
        } else if (std::holds_alternative<double>(leftVal) && std::holds_alternative<double>(rightVal)) {
            return std::get<double>(leftVal) > std::get<double>(rightVal);
        }
    } else if (op == "<=") {
        if (std::holds_alternative<int64_t>(leftVal) && std::holds_alternative<int64_t>(rightVal)) {
            return std::get<int64_t>(leftVal) <= std::get<int64_t>(rightVal);
        } else if (std::holds_alternative<double>(leftVal) && std::holds_alternative<double>(rightVal)) {
            return std::get<double>(leftVal) <= std::get<double>(rightVal);
        }
    } else if (op == ">=") {
        if (std::holds_alternative<int64_t>(leftVal) && std::holds_alternative<int64_t>(rightVal)) {
            return std::get<int64_t>(leftVal) >= std::get<int64_t>(rightVal);
        } else if (std::holds_alternative<double>(leftVal) && std::holds_alternative<double>(rightVal)) {
            return std::get<double>(leftVal) >= std::get<double>(rightVal);
        }
    }
    
    throw std::runtime_error("Unsupported binary operation: " + op);
}

LogicValue SimpleAkaoEvaluator::evaluateFunctionCall(const std::string& name, const std::vector<std::string>& args) {
    std::vector<LogicValue> evaluatedArgs;
    for (const auto& arg : args) {
        evaluatedArgs.push_back(evaluateExpression(arg));
    }
    
    return context_.callFunction(name, evaluatedArgs);
}

LogicValue SimpleAkaoEvaluator::parseValue(const std::string& token) {
    std::string trimmed = trim(token);
    
    // Boolean values
    if (trimmed == "true") return true;
    if (trimmed == "false") return false;
    
    // String values (quoted)
    if (trimmed.length() >= 2 && trimmed.front() == '"' && trimmed.back() == '"') {
        return trimmed.substr(1, trimmed.length() - 2);
    }
    
    // Integer values
    if (std::all_of(trimmed.begin(), trimmed.end(), [](char c) { return std::isdigit(c) || c == '-'; })) {
        return static_cast<int64_t>(std::stoll(trimmed));
    }
    
    // Double values
    if (trimmed.find('.') != std::string::npos) {
        return std::stod(trimmed);
    }
    
    throw std::runtime_error("Cannot parse value: " + token);
}

std::vector<std::string> SimpleAkaoEvaluator::tokenize(const std::string& expr) {
    std::vector<std::string> tokens;
    std::string current;
    bool inQuotes = false;
    
    for (char c : expr) {
        if (c == '"') {
            inQuotes = !inQuotes;
            current += c;
        } else if (!inQuotes && std::isspace(c)) {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        tokens.push_back(current);
    }
    
    return tokens;
}

bool SimpleAkaoEvaluator::isBinaryOperator(const std::string& token) {
    return token == "==" || token == "!=" || token == "<" || token == ">" || 
           token == "<=" || token == ">=" || token == "&&" || token == "||";
}

bool SimpleAkaoEvaluator::isValue(const std::string& token) {
    return token == "true" || token == "false" || 
           (token.front() == '"' && token.back() == '"') ||
           std::all_of(token.begin(), token.end(), [](char c) { return std::isdigit(c) || c == '-' || c == '.'; });
}

std::string SimpleAkaoEvaluator::trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

// AkaoLogicExecutorNode implementation
AkaoLogicExecutorNode::AkaoLogicExecutorNode()
    : nodeId_("akao:node:builtin:logic:v1"),
      nodeType_("logic_executor"),
      version_("1.0.0"),
      description_("Clean .a format executor node for logical expression evaluation") {}

std::string AkaoLogicExecutorNode::getNodeId() const {
    return nodeId_;
}

std::string AkaoLogicExecutorNode::getNodeType() const {
    return nodeType_;
}

std::string AkaoLogicExecutorNode::getVersion() const {
    return version_;
}

std::string AkaoLogicExecutorNode::getDescription() const {
    return description_;
}

foundation::interfaces::ValidationResult AkaoLogicExecutorNode::validate(const foundation::interfaces::NodeParameters& params) const {
    foundation::interfaces::ValidationResult result(true);
    
    // Check required parameters
    if (!params.hasParameter("expression")) {
        result.setValid(false);
        result.addError("Missing required parameter: expression");
        return result;
    }
    
    auto exprParam = params.getParameter("expression");
    if (!exprParam.isString()) {
        result.setValid(false);
        result.addError("Parameter 'expression' must be a string");
        return result;
    }
    
    std::string expression = exprParam.asString();
    if (expression.empty()) {
        result.setValid(false);
        result.addError("Parameter 'expression' cannot be empty");
        return result;
    }
    
    return result;
}

foundation::types::ExecutionResult AkaoLogicExecutorNode::execute(const foundation::interfaces::NodeContext& /* context */,
                                                                const foundation::interfaces::NodeParameters& params) {
    try {
        // Get expression parameter
        std::string expression = params.getParameter("expression").asString();
        
        // Create logic context and initialize built-in functions
        LogicContext logicContext;
        initializeBuiltinFunctions(logicContext);
        
        // Populate context with input parameters
        populateContextFromParameters(logicContext, params);
        
        // Evaluate expression
        SimpleAkaoEvaluator evaluator(logicContext);
        LogicValue logicResult = evaluator.evaluate(expression);
        
        // Convert result back to NodeValue
        foundation::types::NodeValue nodeResult = logicValueToNodeValue(logicResult);
        
        return foundation::types::ExecutionResult::success(nodeResult);
        
    } catch (const std::exception& e) {
        return foundation::types::ExecutionResult::error("Execution failed: " + std::string(e.what()));
    }
}

foundation::types::NodeValue AkaoLogicExecutorNode::getParameterSchema() const {
    foundation::types::NodeValue::Object schema;
    
    foundation::types::NodeValue::Object expressionSchema;
    expressionSchema["type"] = foundation::types::NodeValue(std::string("string"));
    expressionSchema["required"] = foundation::types::NodeValue(true);
    expressionSchema["description"] = foundation::types::NodeValue(std::string("The .a format logical expression to evaluate"));
    schema["expression"] = foundation::types::NodeValue(expressionSchema);
    
    foundation::types::NodeValue::Object variablesSchema;
    variablesSchema["type"] = foundation::types::NodeValue(std::string("object"));
    variablesSchema["required"] = foundation::types::NodeValue(false);
    variablesSchema["description"] = foundation::types::NodeValue(std::string("Variables to make available in the expression context"));
    schema["variables"] = foundation::types::NodeValue(variablesSchema);
    
    return foundation::types::NodeValue(schema);
}

void AkaoLogicExecutorNode::initializeBuiltinFunctions(LogicContext& context) const {
    // String functions
    context.registerFunction("string.length", [this](const std::vector<LogicValue>& args) {
        return func_string_length(args);
    });
    context.registerFunction("string.concat", [this](const std::vector<LogicValue>& args) {
        return func_string_concat(args);
    });
    context.registerFunction("string.contains", [this](const std::vector<LogicValue>& args) {
        return func_string_contains(args);
    });
    
    // Math functions
    context.registerFunction("math.add", [this](const std::vector<LogicValue>& args) {
        return func_math_add(args);
    });
    context.registerFunction("math.subtract", [this](const std::vector<LogicValue>& args) {
        return func_math_subtract(args);
    });
    context.registerFunction("math.multiply", [this](const std::vector<LogicValue>& args) {
        return func_math_multiply(args);
    });
    context.registerFunction("math.divide", [this](const std::vector<LogicValue>& args) {
        return func_math_divide(args);
    });
    
    // Collection functions
    context.registerFunction("collection.count", [this](const std::vector<LogicValue>& args) {
        return func_collection_count(args);
    });
    context.registerFunction("collection.contains", [this](const std::vector<LogicValue>& args) {
        return func_collection_contains(args);
    });
    
    // Filesystem functions
    context.registerFunction("filesystem.exists", [this](const std::vector<LogicValue>& args) {
        return func_filesystem_exists(args);
    });
    context.registerFunction("filesystem.is_file", [this](const std::vector<LogicValue>& args) {
        return func_filesystem_is_file(args);
    });
    context.registerFunction("filesystem.is_directory", [this](const std::vector<LogicValue>& args) {
        return func_filesystem_is_directory(args);
    });
}

// Built-in function implementations
LogicValue AkaoLogicExecutorNode::func_string_length(const std::vector<LogicValue>& args) const {
    if (args.size() != 1) {
        throw std::runtime_error("string.length expects exactly 1 argument");
    }
    
    if (!std::holds_alternative<std::string>(args[0])) {
        throw std::runtime_error("string.length expects a string argument");
    }
    
    return static_cast<int64_t>(std::get<std::string>(args[0]).length());
}

LogicValue AkaoLogicExecutorNode::func_string_concat(const std::vector<LogicValue>& args) const {
    if (args.size() < 2) {
        throw std::runtime_error("string.concat expects at least 2 arguments");
    }
    
    std::string result;
    for (const auto& arg : args) {
        if (!std::holds_alternative<std::string>(arg)) {
            throw std::runtime_error("string.concat expects string arguments");
        }
        result += std::get<std::string>(arg);
    }
    
    return result;
}

LogicValue AkaoLogicExecutorNode::func_string_contains(const std::vector<LogicValue>& args) const {
    if (args.size() != 2) {
        throw std::runtime_error("string.contains expects exactly 2 arguments");
    }
    
    if (!std::holds_alternative<std::string>(args[0]) || !std::holds_alternative<std::string>(args[1])) {
        throw std::runtime_error("string.contains expects string arguments");
    }
    
    std::string haystack = std::get<std::string>(args[0]);
    std::string needle = std::get<std::string>(args[1]);
    
    return haystack.find(needle) != std::string::npos;
}

LogicValue AkaoLogicExecutorNode::func_math_add(const std::vector<LogicValue>& args) const {
    if (args.size() != 2) {
        throw std::runtime_error("math.add expects exactly 2 arguments");
    }
    
    if (std::holds_alternative<int64_t>(args[0]) && std::holds_alternative<int64_t>(args[1])) {
        return std::get<int64_t>(args[0]) + std::get<int64_t>(args[1]);
    } else if (std::holds_alternative<double>(args[0]) && std::holds_alternative<double>(args[1])) {
        return std::get<double>(args[0]) + std::get<double>(args[1]);
    } else {
        throw std::runtime_error("math.add expects numeric arguments");
    }
}

LogicValue AkaoLogicExecutorNode::func_math_subtract(const std::vector<LogicValue>& args) const {
    if (args.size() != 2) {
        throw std::runtime_error("math.subtract expects exactly 2 arguments");
    }
    
    if (std::holds_alternative<int64_t>(args[0]) && std::holds_alternative<int64_t>(args[1])) {
        return std::get<int64_t>(args[0]) - std::get<int64_t>(args[1]);
    } else if (std::holds_alternative<double>(args[0]) && std::holds_alternative<double>(args[1])) {
        return std::get<double>(args[0]) - std::get<double>(args[1]);
    } else {
        throw std::runtime_error("math.subtract expects numeric arguments");
    }
}

LogicValue AkaoLogicExecutorNode::func_math_multiply(const std::vector<LogicValue>& args) const {
    if (args.size() != 2) {
        throw std::runtime_error("math.multiply expects exactly 2 arguments");
    }
    
    if (std::holds_alternative<int64_t>(args[0]) && std::holds_alternative<int64_t>(args[1])) {
        return std::get<int64_t>(args[0]) * std::get<int64_t>(args[1]);
    } else if (std::holds_alternative<double>(args[0]) && std::holds_alternative<double>(args[1])) {
        return std::get<double>(args[0]) * std::get<double>(args[1]);
    } else {
        throw std::runtime_error("math.multiply expects numeric arguments");
    }
}

LogicValue AkaoLogicExecutorNode::func_math_divide(const std::vector<LogicValue>& args) const {
    if (args.size() != 2) {
        throw std::runtime_error("math.divide expects exactly 2 arguments");
    }
    
    if (std::holds_alternative<int64_t>(args[0]) && std::holds_alternative<int64_t>(args[1])) {
        int64_t divisor = std::get<int64_t>(args[1]);
        if (divisor == 0) {
            throw std::runtime_error("Division by zero");
        }
        return std::get<int64_t>(args[0]) / divisor;
    } else if (std::holds_alternative<double>(args[0]) && std::holds_alternative<double>(args[1])) {
        double divisor = std::get<double>(args[1]);
        if (divisor == 0.0) {
            throw std::runtime_error("Division by zero");
        }
        return std::get<double>(args[0]) / divisor;
    } else {
        throw std::runtime_error("math.divide expects numeric arguments");
    }
}

LogicValue AkaoLogicExecutorNode::func_collection_count(const std::vector<LogicValue>& args) const {
    if (args.size() != 1) {
        throw std::runtime_error("collection.count expects exactly 1 argument");
    }
    
    if (std::holds_alternative<std::vector<foundation::types::NodeValue>>(args[0])) {
        return static_cast<int64_t>(std::get<std::vector<foundation::types::NodeValue>>(args[0]).size());
    } else {
        throw std::runtime_error("collection.count expects a collection argument");
    }
}

LogicValue AkaoLogicExecutorNode::func_collection_contains(const std::vector<LogicValue>& args) const {
    if (args.size() != 2) {
        throw std::runtime_error("collection.contains expects exactly 2 arguments");
    }
    
    if (!std::holds_alternative<std::vector<foundation::types::NodeValue>>(args[0])) {
        throw std::runtime_error("collection.contains expects first argument to be a collection");
    }
    
    auto collection = std::get<std::vector<foundation::types::NodeValue>>(args[0]);
    LogicValue searchValue = args[1];
    
    for (const auto& item : collection) {
        LogicValue itemValue = nodeValueToLogicValue(item);
        if (itemValue == searchValue) {
            return true;
        }
    }
    
    return false;
}

LogicValue AkaoLogicExecutorNode::func_filesystem_exists(const std::vector<LogicValue>& args) const {
    if (args.size() != 1) {
        throw std::runtime_error("filesystem.exists expects exactly 1 argument");
    }
    
    if (!std::holds_alternative<std::string>(args[0])) {
        throw std::runtime_error("filesystem.exists expects a string argument");
    }
    
    std::string path = std::get<std::string>(args[0]);
    return std::filesystem::exists(path);
}

LogicValue AkaoLogicExecutorNode::func_filesystem_is_file(const std::vector<LogicValue>& args) const {
    if (args.size() != 1) {
        throw std::runtime_error("filesystem.is_file expects exactly 1 argument");
    }
    
    if (!std::holds_alternative<std::string>(args[0])) {
        throw std::runtime_error("filesystem.is_file expects a string argument");
    }
    
    std::string path = std::get<std::string>(args[0]);
    return std::filesystem::is_regular_file(path);
}

LogicValue AkaoLogicExecutorNode::func_filesystem_is_directory(const std::vector<LogicValue>& args) const {
    if (args.size() != 1) {
        throw std::runtime_error("filesystem.is_directory expects exactly 1 argument");
    }
    
    if (!std::holds_alternative<std::string>(args[0])) {
        throw std::runtime_error("filesystem.is_directory expects a string argument");
    }
    
    std::string path = std::get<std::string>(args[0]);
    return std::filesystem::is_directory(path);
}

// Conversion utilities
LogicValue AkaoLogicExecutorNode::nodeValueToLogicValue(const foundation::types::NodeValue& value) const {
    if (value.isBoolean()) {
        return value.asBoolean();
    } else if (value.isInteger()) {
        return value.asInteger();
    } else if (value.isDouble()) {
        return value.asDouble();
    } else if (value.isString()) {
        return value.asString();
    } else if (value.isArray()) {
        return value.asArray();
    } else {
        throw std::runtime_error("Unsupported NodeValue type conversion");
    }
}

foundation::types::NodeValue AkaoLogicExecutorNode::logicValueToNodeValue(const LogicValue& value) const {
    if (std::holds_alternative<bool>(value)) {
        return foundation::types::NodeValue(std::get<bool>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        return foundation::types::NodeValue(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        return foundation::types::NodeValue(std::get<double>(value));
    } else if (std::holds_alternative<std::string>(value)) {
        return foundation::types::NodeValue(std::get<std::string>(value));
    } else if (std::holds_alternative<std::vector<foundation::types::NodeValue>>(value)) {
        return foundation::types::NodeValue(std::get<std::vector<foundation::types::NodeValue>>(value));
    } else {
        throw std::runtime_error("Unsupported LogicValue type conversion");
    }
}

void AkaoLogicExecutorNode::populateContextFromParameters(LogicContext& context, const foundation::interfaces::NodeParameters& params) const {
    if (params.hasParameter("variables")) {
        auto variablesParam = params.getParameter("variables");
        if (variablesParam.isObject()) {
            auto variables = variablesParam.asObject();
            for (const auto& pair : variables) {
                LogicValue logicValue = nodeValueToLogicValue(pair.second);
                context.setVariable(pair.first, logicValue);
            }
        }
    }
}

// Static utility methods
bool AkaoLogicExecutorNode::executeExpression(const std::string& expression) {
    try {
        LogicContext context;
        AkaoLogicExecutorNode node;
        node.initializeBuiltinFunctions(context);
        
        SimpleAkaoEvaluator evaluator(context);
        LogicValue result = evaluator.evaluate(expression);
        
        if (std::holds_alternative<bool>(result)) {
            return std::get<bool>(result);
        } else {
            throw std::runtime_error("Expression did not evaluate to boolean");
        }
    } catch (const std::exception&) {
        return false;
    }
}

bool AkaoLogicExecutorNode::executeExpressionWithContext(const std::string& expression, const std::map<std::string, foundation::types::NodeValue>& variables) {
    try {
        LogicContext context;
        AkaoLogicExecutorNode node;
        node.initializeBuiltinFunctions(context);
        
        // Populate variables
        for (const auto& pair : variables) {
            LogicValue logicValue = node.nodeValueToLogicValue(pair.second);
            context.setVariable(pair.first, logicValue);
        }
        
        SimpleAkaoEvaluator evaluator(context);
        LogicValue result = evaluator.evaluate(expression);
        
        if (std::holds_alternative<bool>(result)) {
            return std::get<bool>(result);
        } else {
            throw std::runtime_error("Expression did not evaluate to boolean");
        }
    } catch (const std::exception&) {
        return false;
    }
}

foundation::types::NodeValue AkaoLogicExecutorNode::evaluateExpression(const std::string& expression) {
    LogicContext context;
    AkaoLogicExecutorNode node;
    node.initializeBuiltinFunctions(context);
    
    SimpleAkaoEvaluator evaluator(context);
    LogicValue result = evaluator.evaluate(expression);
    
    return node.logicValueToNodeValue(result);
}

std::vector<std::string> AkaoLogicExecutorNode::validateExpression(const std::string& expression) {
    std::vector<std::string> errors;
    
    if (expression.empty()) {
        errors.push_back("Expression cannot be empty");
        return errors;
    }
    
    try {
        LogicContext context;
        AkaoLogicExecutorNode node;
        node.initializeBuiltinFunctions(context);
        
        SimpleAkaoEvaluator evaluator(context);
        evaluator.evaluate(expression);
        
    } catch (const std::exception& e) {
        errors.push_back(e.what());
    }
    
    return errors;
}

bool AkaoLogicExecutorNode::isValidExpression(const std::string& expression) {
    return validateExpression(expression).empty();
}

} // namespace builtin
} // namespace nodes
} // namespace akao