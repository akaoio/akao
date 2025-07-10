/**
 * @id: akao:file:core:foundation:interfaces:inode:v1
 * @doc: Implementation of standard workflow node interface classes. Provides concrete implementations for NodeContext, NodeParameters, and ValidationResult to support node execution and parameter management across the workflow ecosystem.
 * @specification: Implementation of node interface support classes
 * @scope: Core foundation interfaces implementation
 * @timeline: 2025-07-10
 * @rationale: Concrete implementation of node interface support classes
 * @methodology: Standard class implementations with error handling
 * @references: ["akao:class:core:foundation:interfaces:inode:v1"]
 */

#include "v1.hpp"
#include <sstream>
#include <algorithm>

namespace akao {
namespace foundation {
namespace interfaces {

// NodeContext implementation
NodeContext::NodeContext(const std::string& nodeId, const types::NodeValue& inputs) 
    : inputs_(inputs), nodeId_(nodeId) {}

const types::NodeValue& NodeContext::getInputs() const {
    return inputs_;
}

void NodeContext::setInputs(const types::NodeValue& inputs) {
    inputs_ = inputs;
}

types::NodeValue NodeContext::getEnvironment(const std::string& key) const {
    auto it = environment_.find(key);
    return (it != environment_.end()) ? it->second : types::NodeValue();
}

void NodeContext::setEnvironment(const std::string& key, const types::NodeValue& value) {
    environment_[key] = value;
}

bool NodeContext::hasEnvironment(const std::string& key) const {
    return environment_.find(key) != environment_.end();
}

const std::map<std::string, types::NodeValue>& NodeContext::getAllEnvironment() const {
    return environment_;
}

const std::string& NodeContext::getWorkflowId() const {
    return workflowId_;
}

void NodeContext::setWorkflowId(const std::string& workflowId) {
    workflowId_ = workflowId;
}

const std::string& NodeContext::getExecutionId() const {
    return executionId_;
}

void NodeContext::setExecutionId(const std::string& executionId) {
    executionId_ = executionId;
}

const std::string& NodeContext::getNodeId() const {
    return nodeId_;
}

void NodeContext::setNodeId(const std::string& nodeId) {
    nodeId_ = nodeId;
}

types::NodeValue NodeContext::getInput(const std::string& key) const {
    if (inputs_.isObject()) {
        return inputs_.hasKey(key) ? inputs_[key] : types::NodeValue();
    }
    return types::NodeValue();
}

bool NodeContext::hasInput(const std::string& key) const {
    return inputs_.isObject() && inputs_.hasKey(key);
}

// NodeParameters implementation
void NodeParameters::setParameter(const std::string& key, const types::NodeValue& value) {
    parameters_[key] = value;
}

types::NodeValue NodeParameters::getParameter(const std::string& key) const {
    auto it = parameters_.find(key);
    return (it != parameters_.end()) ? it->second : types::NodeValue();
}

bool NodeParameters::hasParameter(const std::string& key) const {
    return parameters_.find(key) != parameters_.end();
}

const std::map<std::string, types::NodeValue>& NodeParameters::getAllParameters() const {
    return parameters_;
}

void NodeParameters::setParameterType(const std::string& key, const std::string& type) {
    parameterTypes_[key] = type;
}

std::string NodeParameters::getParameterType(const std::string& key) const {
    auto it = parameterTypes_.find(key);
    return (it != parameterTypes_.end()) ? it->second : "";
}

void NodeParameters::setParameterRequired(const std::string& key, bool required) {
    requiredParameters_[key] = required;
}

bool NodeParameters::isParameterRequired(const std::string& key) const {
    auto it = requiredParameters_.find(key);
    return (it != requiredParameters_.end()) ? it->second : false;
}

std::vector<std::string> NodeParameters::validate() const {
    std::vector<std::string> errors;

    // Check required parameters
    for (const auto& required : requiredParameters_) {
        if (required.second && !hasParameter(required.first)) {
            errors.push_back("Required parameter '" + required.first + "' is missing");
        }
    }

    // Check parameter types
    for (const auto& param : parameters_) {
        const std::string& key = param.first;
        const types::NodeValue& value = param.second;
        
        auto typeIt = parameterTypes_.find(key);
        if (typeIt != parameterTypes_.end()) {
            const std::string& expectedType = typeIt->second;
            
            // Basic type checking
            if (expectedType == "string" && !value.isString()) {
                errors.push_back("Parameter '" + key + "' must be a string");
            } else if (expectedType == "integer" && !value.isInteger()) {
                errors.push_back("Parameter '" + key + "' must be an integer");
            } else if (expectedType == "double" && !value.isDouble()) {
                errors.push_back("Parameter '" + key + "' must be a double");
            } else if (expectedType == "boolean" && !value.isBoolean()) {
                errors.push_back("Parameter '" + key + "' must be a boolean");
            } else if (expectedType == "array" && !value.isArray()) {
                errors.push_back("Parameter '" + key + "' must be an array");
            } else if (expectedType == "object" && !value.isObject()) {
                errors.push_back("Parameter '" + key + "' must be an object");
            }
        }
    }

    return errors;
}

bool NodeParameters::isValid() const {
    return validate().empty();
}

// ValidationResult implementation
ValidationResult::ValidationResult(bool valid) : valid_(valid) {}

bool ValidationResult::isValid() const {
    return valid_ && errors_.empty();
}

void ValidationResult::setValid(bool valid) {
    valid_ = valid;
}

void ValidationResult::addError(const std::string& error) {
    errors_.push_back(error);
    valid_ = false;
}

const std::vector<std::string>& ValidationResult::getErrors() const {
    return errors_;
}

bool ValidationResult::hasErrors() const {
    return !errors_.empty();
}

void ValidationResult::addWarning(const std::string& warning) {
    warnings_.push_back(warning);
}

const std::vector<std::string>& ValidationResult::getWarnings() const {
    return warnings_;
}

bool ValidationResult::hasWarnings() const {
    return !warnings_.empty();
}

void ValidationResult::merge(const ValidationResult& other) {
    if (!other.isValid()) {
        valid_ = false;
    }
    
    errors_.insert(errors_.end(), other.errors_.begin(), other.errors_.end());
    warnings_.insert(warnings_.end(), other.warnings_.begin(), other.warnings_.end());
}

std::string ValidationResult::toString() const {
    std::ostringstream oss;
    oss << "ValidationResult{valid=" << (isValid() ? "true" : "false");
    
    if (!errors_.empty()) {
        oss << ", errors=[";
        for (size_t i = 0; i < errors_.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << "\"" << errors_[i] << "\"";
        }
        oss << "]";
    }
    
    if (!warnings_.empty()) {
        oss << ", warnings=[";
        for (size_t i = 0; i < warnings_.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << "\"" << warnings_[i] << "\"";
        }
        oss << "]";
    }
    
    oss << "}";
    return oss.str();
}

} // namespace interfaces
} // namespace foundation
} // namespace akao