/**
 * @id: akao:file:nodes:builtin:logic:v1
 * @doc: Implementation of .a format executor node using legacy Pure Logic Engine. Provides adapter interface converting legacy logic engine results to NodeValue format while preserving all advanced features including 47+ built-in functions and comprehensive .a format support.
 * @specification: .a format executor implementation using legacy Pure Logic Engine
 * @scope: Builtin logic execution node implementation
 * @timeline: 2025-07-10
 * @rationale: Concrete implementation leveraging proven legacy logic engine
 * @methodology: Adapter pattern with comprehensive NodeValue/LogicValue conversion
 * @references: ["akao:class:nodes:builtin:logic:v1"]
 */

#include "v1.hpp"
#include <sstream>

namespace akao {
namespace nodes {
namespace builtin {

AkaoLogicExecutorNode::AkaoLogicExecutorNode()
    : nodeId_("akao:builtin:logic-executor")
    , nodeType_("logic")
    , version_("2.0.0")
    , description_("Enhanced .a format executor using legacy Pure Logic Engine with 47+ built-in functions") {
}

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
    foundation::interfaces::ValidationResult result;
    
    if (!params.hasParameter("expression")) {
        result.addError("Missing required parameter 'expression'");
        return result;
    }
    
    std::string expression = params.getParameter("expression").asString();
    if (expression.empty()) {
        result.addError("Parameter 'expression' cannot be empty");
        return result;
    }
    
    // Validate syntax using legacy logic engine
    auto violations = validateExpression(expression);
    for (const auto& violation : violations) {
        result.addError(violation);
    }
    
    return result;
}

foundation::types::ExecutionResult AkaoLogicExecutorNode::execute(const foundation::interfaces::NodeContext& context,
                                                                 const foundation::interfaces::NodeParameters& params) {
    try {
        std::string expression = params.getParameter("expression").asString();
        
        // Create execution context using legacy engine
        akao::logic::Context logicContext;
        populateContextFromParameters(logicContext, params);
        
        // Create and configure Pure Logic Engine
        akao::logic::PureLogicEngine engine;
        
        // Enable features based on parameters
        if (params.hasParameter("enable_caching") && params.getParameter("enable_caching").asBoolean()) {
            engine.enableCaching(true);
        }
        
        if (params.hasParameter("enable_tracing") && params.getParameter("enable_tracing").asBoolean()) {
            engine.enableTracing(true);
        }
        
        // Execute expression using legacy Pure Logic Engine
        akao::logic::Value result = engine.evaluate(expression, logicContext);
        
        // Convert result to NodeValue
        foundation::types::NodeValue resultData = logicValueToNodeValue(result);
        
        // Add execution metadata
        resultData.setMetadata("expression", foundation::types::NodeValue(expression));
        resultData.setMetadata("executorVersion", foundation::types::NodeValue(version_));
        resultData.setMetadata("engine", foundation::types::NodeValue("legacy-pure-logic"));
        resultData.setMetadata("isAkaoFormat", foundation::types::NodeValue(engine.isAkaoFormat(expression)));
        
        return foundation::types::ExecutionResult::success(resultData);
        
    } catch (const akao::logic::ForallViolationException& e) {
        // Special handling for forall violations
        std::stringstream ss;
        ss << "Forall violation: " << e.what() << " (variable: " << e.quantifier_variable << ")";
        return foundation::types::ExecutionResult::error(ss.str());
    } catch (const std::exception& e) {
        return foundation::types::ExecutionResult::error("Logic execution failed: " + std::string(e.what()));
    }
}

foundation::types::NodeValue AkaoLogicExecutorNode::getParameterSchema() const {
    foundation::types::NodeValue::Object schema;
    
    // Required expression parameter
    foundation::types::NodeValue::Object expressionParam;
    expressionParam["type"] = foundation::types::NodeValue("string");
    expressionParam["required"] = foundation::types::NodeValue(true);
    expressionParam["description"] = foundation::types::NodeValue(".a format logical expression to evaluate");
    schema["expression"] = foundation::types::NodeValue(expressionParam);
    
    // Optional variables parameter
    foundation::types::NodeValue::Object variablesParam;
    variablesParam["type"] = foundation::types::NodeValue("object");
    variablesParam["required"] = foundation::types::NodeValue(false);
    variablesParam["description"] = foundation::types::NodeValue("Variables to make available in expression context");
    schema["variables"] = foundation::types::NodeValue(variablesParam);
    
    // Optional enable_caching parameter
    foundation::types::NodeValue::Object cachingParam;
    cachingParam["type"] = foundation::types::NodeValue("boolean");
    cachingParam["required"] = foundation::types::NodeValue(false);
    cachingParam["default"] = foundation::types::NodeValue(false);
    cachingParam["description"] = foundation::types::NodeValue("Enable caching for improved performance");
    schema["enable_caching"] = foundation::types::NodeValue(cachingParam);
    
    // Optional enable_tracing parameter
    foundation::types::NodeValue::Object tracingParam;
    tracingParam["type"] = foundation::types::NodeValue("boolean");
    tracingParam["required"] = foundation::types::NodeValue(false);
    tracingParam["default"] = foundation::types::NodeValue(false);
    tracingParam["description"] = foundation::types::NodeValue("Enable tracing for debugging");
    schema["enable_tracing"] = foundation::types::NodeValue(tracingParam);
    
    return foundation::types::NodeValue(schema);
}

// Conversion utilities
akao::logic::Value AkaoLogicExecutorNode::nodeValueToLogicValue(const foundation::types::NodeValue& value) const {
    if (value.isBoolean()) {
        return akao::logic::Value(value.asBoolean());
    } else if (value.isInteger()) {
        return akao::logic::Value(static_cast<int>(value.asInteger()));
    } else if (value.isDouble()) {
        return akao::logic::Value(value.asDouble());
    } else if (value.isString()) {
        return akao::logic::Value(value.asString());
    } else if (value.isArray()) {
        std::vector<akao::logic::Value> collection;
        for (const auto& item : value.asArray()) {
            collection.push_back(nodeValueToLogicValue(item));
        }
        return akao::logic::Value(collection);
    } else if (value.isObject()) {
        std::map<std::string, akao::logic::Value> object;
        for (const auto& pair : value.asObject()) {
            object[pair.first] = nodeValueToLogicValue(pair.second);
        }
        return akao::logic::Value(object);
    } else {
        return akao::logic::Value(); // null value
    }
}

foundation::types::NodeValue AkaoLogicExecutorNode::logicValueToNodeValue(const akao::logic::Value& value) const {
    if (value.isBoolean()) {
        return foundation::types::NodeValue(value.asBoolean());
    } else if (value.isInteger()) {
        return foundation::types::NodeValue(static_cast<int64_t>(value.asInteger()));
    } else if (value.isFloat()) {
        return foundation::types::NodeValue(value.asFloat());
    } else if (value.isString()) {
        return foundation::types::NodeValue(value.asString());
    } else if (value.isCollection()) {
        foundation::types::NodeValue::Array array;
        for (const auto& item : value.asCollection()) {
            array.push_back(logicValueToNodeValue(item));
        }
        return foundation::types::NodeValue(array);
    } else if (value.isObject()) {
        foundation::types::NodeValue::Object object;
        for (const auto& pair : value.asObject()) {
            object[pair.first] = logicValueToNodeValue(pair.second);
        }
        return foundation::types::NodeValue(object);
    } else {
        return foundation::types::NodeValue(); // null value
    }
}

void AkaoLogicExecutorNode::populateContextFromParameters(akao::logic::Context& context, const foundation::interfaces::NodeParameters& params) const {
    // Add variables from parameters
    if (params.hasParameter("variables")) {
        auto variables = params.getParameter("variables");
        if (variables.isObject()) {
            for (const auto& pair : variables.asObject()) {
                akao::logic::Value value = nodeValueToLogicValue(pair.second);
                context.setVariable(pair.first, value);
            }
        }
    }
    
    // Add any other context setup as needed
}

// Static utility methods
bool AkaoLogicExecutorNode::executeExpression(const std::string& expression) {
    try {
        akao::logic::Context context;
        akao::logic::PureLogicEngine engine;
        akao::logic::Value result = engine.evaluate(expression, context);
        return result.asBoolean();
    } catch (const std::exception&) {
        return false;
    }
}

bool AkaoLogicExecutorNode::executeExpressionWithContext(const std::string& expression, 
                                                       const std::map<std::string, foundation::types::NodeValue>& variables) {
    try {
        akao::logic::Context context;
        AkaoLogicExecutorNode executor;
        
        // Add variables to context
        for (const auto& pair : variables) {
            akao::logic::Value value = executor.nodeValueToLogicValue(pair.second);
            context.setVariable(pair.first, value);
        }
        
        akao::logic::PureLogicEngine engine;
        akao::logic::Value result = engine.evaluate(expression, context);
        return result.asBoolean();
    } catch (const std::exception&) {
        return false;
    }
}

foundation::types::NodeValue AkaoLogicExecutorNode::evaluateExpression(const std::string& expression) {
    try {
        akao::logic::Context context;
        akao::logic::PureLogicEngine engine;
        akao::logic::Value result = engine.evaluate(expression, context);
        
        AkaoLogicExecutorNode executor;
        return executor.logicValueToNodeValue(result);
    } catch (const std::exception&) {
        return foundation::types::NodeValue();
    }
}

std::vector<std::string> AkaoLogicExecutorNode::validateExpression(const std::string& expression) {
    std::vector<std::string> violations;
    
    try {
        akao::logic::Context context;
        akao::logic::PureLogicEngine engine;
        // Try to parse the expression - if it succeeds, it's valid
        auto ast = engine.parseAkaoFormat(expression);
        if (!ast) {
            violations.push_back("Failed to parse .a format expression");
        }
    } catch (const std::exception& e) {
        violations.push_back("Syntax error: " + std::string(e.what()));
    }
    
    return violations;
}

bool AkaoLogicExecutorNode::isValidExpression(const std::string& expression) {
    return validateExpression(expression).empty();
}

bool AkaoLogicExecutorNode::isAkaoFormat(const std::string& code) {
    try {
        akao::logic::PureLogicEngine engine;
        return engine.isAkaoFormat(code);
    } catch (const std::exception&) {
        return false;
    }
}

} // namespace builtin
} // namespace nodes
} // namespace akao