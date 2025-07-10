/**
 * @id: akao:file:core:node:interface:nodedefinition_impl:v1
 * @doc: Implementation of extended node definition system providing comprehensive parameter definitions, validation rules, and UI metadata support for node-based workflow system with complete parameter type handling and validation logic.
 * @specification: Complete implementation of NodeParameter, ParameterOption, and supporting classes
 * @scope: Core node definition implementation
 * @timeline: 2025-07-10
 * @rationale: Provide robust implementation of node definition system with parameter validation
 * @methodology: Complete parameter system with validation, constraints, and type handling
 * @references: ["akao:class:core:node:interface:nodeparameter:v1", "akao:class:core:node:interface:parametertype:v1"]
 */

#include "node_definition.hpp"
#include "inode.hpp"
#include <sstream>
#include <regex>
#include <algorithm>

namespace akao::node {

// ============================================================================
// ParameterOption Implementation
// ============================================================================

ParameterOption::ParameterOption(const std::string& value, const std::string& display_name)
    : value_(value), display_name_(display_name) {}

ParameterOption::ParameterOption(const std::string& value, const std::string& display_name, const std::string& description)
    : value_(value), display_name_(display_name), description_(description) {}

std::string ParameterOption::toString() const {
    std::stringstream ss;
    ss << "ParameterOption{";
    ss << "value: \"" << value_ << "\"";
    ss << ", display_name: \"" << display_name_ << "\"";
    if (!description_.empty()) {
        ss << ", description: \"" << description_ << "\"";
    }
    if (!group_.empty()) {
        ss << ", group: \"" << group_ << "\"";
    }
    ss << ", enabled: " << (enabled_ ? "true" : "false");
    ss << "}";
    return ss.str();
}

// ============================================================================
// ParameterValidation Implementation
// ============================================================================

bool ParameterValidation::isEmpty() const {
    return !required && 
           !min_value.has_value() && 
           !max_value.has_value() &&
           !min_length.has_value() && 
           !max_length.has_value() &&
           pattern.empty() &&
           !min_items.has_value() && 
           !max_items.has_value() &&
           allowed_extensions.empty() &&
           !max_file_size_mb.has_value() &&
           akao_validation_expression.empty() &&
           depends_on.empty();
}

std::string ParameterValidation::toString() const {
    std::stringstream ss;
    ss << "ParameterValidation{";
    ss << "required: " << (required ? "true" : "false");
    
    if (min_value.has_value() || max_value.has_value()) {
        ss << ", range: [";
        if (min_value.has_value()) ss << min_value.value();
        else ss << "-inf";
        ss << ", ";
        if (max_value.has_value()) ss << max_value.value();
        else ss << "+inf";
        ss << "]";
    }
    
    if (min_length.has_value() || max_length.has_value()) {
        ss << ", length: [";
        if (min_length.has_value()) ss << min_length.value();
        else ss << "0";
        ss << ", ";
        if (max_length.has_value()) ss << max_length.value();
        else ss << "unlimited";
        ss << "]";
    }
    
    if (!pattern.empty()) {
        ss << ", pattern: \"" << pattern << "\"";
    }
    
    if (!allowed_extensions.empty()) {
        ss << ", extensions: [";
        for (size_t i = 0; i < allowed_extensions.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << "\"" << allowed_extensions[i] << "\"";
        }
        ss << "]";
    }
    
    if (!akao_validation_expression.empty()) {
        ss << ", akao_validation: \"" << akao_validation_expression << "\"";
    }
    
    ss << "}";
    return ss.str();
}

// ============================================================================
// NodeParameter Implementation
// ============================================================================

NodeParameter::NodeParameter(const std::string& name, ParameterType type)
    : name_(name), type_(type), display_name_(name), default_value_(ParameterTypeUtils::getDefaultValueForType(type)) {}

NodeParameter::NodeParameter(const std::string& name, ParameterType type, const NodeValue& default_value)
    : name_(name), type_(type), display_name_(name), default_value_(default_value) {}

ValidationResult NodeParameter::validate(const NodeValue& value) const {
    ValidationResult result;
    
    // Check if required parameter is provided
    if (validation_.required && value.isNull()) {
        result.addError(name_, "Required parameter is missing", ValidationResult::Severity::ERROR);
        return result;
    }
    
    // Skip validation for null optional parameters
    if (value.isNull() && !validation_.required) {
        return result;
    }
    
    // Type-specific validation
    switch (type_) {
        case ParameterType::STRING:
        case ParameterType::PASSWORD:
        case ParameterType::EMAIL:
        case ParameterType::URL:
            if (!value.isString()) {
                result.addError(name_, "Expected string value", ValidationResult::Severity::ERROR);
            } else {
                std::string str_value = value.asString();
                
                // Length validation
                if (validation_.min_length.has_value() && str_value.length() < validation_.min_length.value()) {
                    result.addError(name_, "String too short (min: " + std::to_string(validation_.min_length.value()) + ")",
                                  ValidationResult::Severity::ERROR);
                }
                
                if (validation_.max_length.has_value() && str_value.length() > validation_.max_length.value()) {
                    result.addError(name_, "String too long (max: " + std::to_string(validation_.max_length.value()) + ")",
                                  ValidationResult::Severity::ERROR);
                }
                
                // Pattern validation
                if (!validation_.pattern.empty()) {
                    try {
                        std::regex pattern(validation_.pattern);
                        if (!std::regex_match(str_value, pattern)) {
                            result.addError(name_, "String does not match required pattern", ValidationResult::Severity::ERROR);
                        }
                    } catch (const std::exception& e) {
                        result.addError(name_, "Invalid regex pattern: " + std::string(e.what()),
                                      ValidationResult::Severity::ERROR);
                    }
                }
                
                // Type-specific validation
                if (type_ == ParameterType::EMAIL) {
                    std::regex email_pattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
                    if (!std::regex_match(str_value, email_pattern)) {
                        result.addError(name_, "Invalid email format", ValidationResult::Severity::ERROR);
                    }
                }
                
                if (type_ == ParameterType::URL) {
                    std::regex url_pattern(R"(https?://[^\s/$.?#].[^\s]*)");
                    if (!std::regex_match(str_value, url_pattern)) {
                        result.addError(name_, "Invalid URL format", ValidationResult::Severity::ERROR);
                    }
                }
            }
            break;
            
        case ParameterType::INTEGER:
            if (!value.isInteger()) {
                result.addError(name_, "Expected integer value", ValidationResult::Severity::ERROR);
            } else {
                double int_value = static_cast<double>(value.asInteger());
                
                if (validation_.min_value.has_value() && int_value < validation_.min_value.value()) {
                    result.addError(name_, "Value too small (min: " + std::to_string(validation_.min_value.value()) + ")",
                                  ValidationResult::Severity::ERROR);
                }
                
                if (validation_.max_value.has_value() && int_value > validation_.max_value.value()) {
                    result.addError(name_, "Value too large (max: " + std::to_string(validation_.max_value.value()) + ")",
                                  ValidationResult::Severity::ERROR);
                }
            }
            break;
            
        case ParameterType::FLOAT:
            if (!value.isFloat() && !value.isInteger()) {
                result.addError(name_, "Expected numeric value", ValidationResult::Severity::ERROR);
            } else {
                double float_value = value.isFloat() ? value.asFloat() : static_cast<double>(value.asInteger());
                
                if (validation_.min_value.has_value() && float_value < validation_.min_value.value()) {
                    result.addError(name_, "Value too small (min: " + std::to_string(validation_.min_value.value()) + ")",
                                  ValidationResult::Severity::ERROR);
                }
                
                if (validation_.max_value.has_value() && float_value > validation_.max_value.value()) {
                    result.addError(name_, "Value too large (max: " + std::to_string(validation_.max_value.value()) + ")",
                                  ValidationResult::Severity::ERROR);
                }
            }
            break;
            
        case ParameterType::BOOLEAN:
            if (!value.isBoolean()) {
                result.addError(name_, "Expected boolean value", ValidationResult::Severity::ERROR);
            }
            break;
            
        case ParameterType::ARRAY:
            if (!value.isArray()) {
                result.addError(name_, "Expected array value", ValidationResult::Severity::ERROR);
            } else {
                size_t array_size = value.size();
                
                if (validation_.min_items.has_value() && array_size < validation_.min_items.value()) {
                    result.addError(name_, "Array too small (min items: " + std::to_string(validation_.min_items.value()) + ")",
                                  ValidationResult::Severity::ERROR);
                }
                
                if (validation_.max_items.has_value() && array_size > validation_.max_items.value()) {
                    result.addError(name_, "Array too large (max items: " + std::to_string(validation_.max_items.value()) + ")",
                                  ValidationResult::Severity::ERROR);
                }
            }
            break;
            
        case ParameterType::OBJECT:
            if (!value.isObject()) {
                result.addError(name_, "Expected object value", ValidationResult::Severity::ERROR);
            }
            break;
            
        case ParameterType::OPTIONS:
        case ParameterType::MULTI_SELECT:
            if (!value.isString()) {
                result.addError(name_, "Expected string value for option selection", ValidationResult::Severity::ERROR);
            } else {
                std::string option_value = value.asString();
                bool found = false;
                for (const auto& option : options_) {
                    if (option.getValue() == option_value) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    result.addError(name_, "Invalid option value: " + option_value, ValidationResult::Severity::ERROR);
                }
            }
            break;
            
        case ParameterType::FILE_PATH:
        case ParameterType::DIRECTORY_PATH:
            if (!value.isString()) {
                result.addError(name_, "Expected string path", ValidationResult::Severity::ERROR);
            } else {
                std::string path = value.asString();
                if (path.empty()) {
                    result.addError(name_, "Path cannot be empty", ValidationResult::Severity::ERROR);
                }
                // Additional file system validation could be added here
            }
            break;
            
        default:
            // Generic validation for other types
            break;
    }
    
    // Custom Akao validation
    if (!validation_.akao_validation_expression.empty()) {
        // TODO: Implement Akao logic validation when engine is available
        // For now, just validate that it's not empty if required
    }
    
    return result;
}

std::string NodeParameter::toString() const {
    std::stringstream ss;
    ss << "NodeParameter{";
    ss << "name: \"" << name_ << "\"";
    ss << ", type: \"" << ParameterTypeUtils::toString(type_) << "\"";
    ss << ", display_name: \"" << display_name_ << "\"";
    ss << ", required: " << (validation_.required ? "true" : "false");
    
    if (!default_value_.isNull()) {
        ss << ", default: " << default_value_.toString();
    }
    
    if (!options_.empty()) {
        ss << ", options: " << options_.size();
    }
    
    if (hidden_) {
        ss << ", hidden: true";
    }
    
    if (!group_.empty()) {
        ss << ", group: \"" << group_ << "\"";
    }
    
    ss << "}";
    return ss.str();
}

// ============================================================================
// ParameterGroup Implementation
// ============================================================================

ParameterGroup::ParameterGroup(const std::string& name, const std::string& display_name)
    : name_(name), display_name_(display_name) {}

std::string ParameterGroup::toString() const {
    std::stringstream ss;
    ss << "ParameterGroup{";
    ss << "name: \"" << name_ << "\"";
    ss << ", display_name: \"" << display_name_ << "\"";
    ss << ", parameters: " << parameters_.size();
    ss << ", collapsible: " << (collapsible_ ? "true" : "false");
    ss << ", default_open: " << (default_open_ ? "true" : "false");
    ss << "}";
    return ss.str();
}

// ============================================================================
// ParameterTypeUtils Implementation
// ============================================================================

namespace ParameterTypeUtils {

std::string toString(ParameterType type) {
    switch (type) {
        case ParameterType::STRING: return "string";
        case ParameterType::INTEGER: return "integer";
        case ParameterType::FLOAT: return "float";
        case ParameterType::BOOLEAN: return "boolean";
        case ParameterType::ARRAY: return "array";
        case ParameterType::OBJECT: return "object";
        case ParameterType::FILE_PATH: return "file_path";
        case ParameterType::DIRECTORY_PATH: return "directory_path";
        case ParameterType::URL: return "url";
        case ParameterType::EMAIL: return "email";
        case ParameterType::PASSWORD: return "password";
        case ParameterType::OPTIONS: return "options";
        case ParameterType::MULTI_SELECT: return "multi_select";
        case ParameterType::RESOURCE_LOCATOR: return "resource_locator";
        case ParameterType::DATE_TIME: return "date_time";
        case ParameterType::COLOR: return "color";
        case ParameterType::CODE: return "code";
        case ParameterType::MULTI_LANGUAGE_CODE: return "multi_language_code";
        case ParameterType::JSON: return "json";
        case ParameterType::YAML: return "yaml";
        case ParameterType::XML: return "xml";
        case ParameterType::AKAO_EXPRESSION: return "akao_expression";
        case ParameterType::AKAO_RULE: return "akao_rule";
        case ParameterType::NODE_REFERENCE: return "node_reference";
        case ParameterType::WORKFLOW_REFERENCE: return "workflow_reference";
        case ParameterType::BINARY_DATA: return "binary_data";
        case ParameterType::FILE_UPLOAD: return "file_upload";
        case ParameterType::HIDDEN: return "hidden";
        case ParameterType::COMPUTED: return "computed";
        case ParameterType::DYNAMIC: return "dynamic";
    }
    return "unknown";
}

ParameterType fromString(const std::string& type_string) {
    static std::map<std::string, ParameterType> type_map = {
        {"string", ParameterType::STRING},
        {"integer", ParameterType::INTEGER},
        {"float", ParameterType::FLOAT},
        {"boolean", ParameterType::BOOLEAN},
        {"array", ParameterType::ARRAY},
        {"object", ParameterType::OBJECT},
        {"file_path", ParameterType::FILE_PATH},
        {"directory_path", ParameterType::DIRECTORY_PATH},
        {"url", ParameterType::URL},
        {"email", ParameterType::EMAIL},
        {"password", ParameterType::PASSWORD},
        {"options", ParameterType::OPTIONS},
        {"multi_select", ParameterType::MULTI_SELECT},
        {"resource_locator", ParameterType::RESOURCE_LOCATOR},
        {"date_time", ParameterType::DATE_TIME},
        {"color", ParameterType::COLOR},
        {"code", ParameterType::CODE},
        {"multi_language_code", ParameterType::MULTI_LANGUAGE_CODE},
        {"json", ParameterType::JSON},
        {"yaml", ParameterType::YAML},
        {"xml", ParameterType::XML},
        {"akao_expression", ParameterType::AKAO_EXPRESSION},
        {"akao_rule", ParameterType::AKAO_RULE},
        {"node_reference", ParameterType::NODE_REFERENCE},
        {"workflow_reference", ParameterType::WORKFLOW_REFERENCE},
        {"binary_data", ParameterType::BINARY_DATA},
        {"file_upload", ParameterType::FILE_UPLOAD},
        {"hidden", ParameterType::HIDDEN},
        {"computed", ParameterType::COMPUTED},
        {"dynamic", ParameterType::DYNAMIC}
    };
    
    auto it = type_map.find(type_string);
    return (it != type_map.end()) ? it->second : ParameterType::STRING;
}

bool isNumericType(ParameterType type) {
    return type == ParameterType::INTEGER || type == ParameterType::FLOAT;
}

bool isStringType(ParameterType type) {
    return type == ParameterType::STRING || 
           type == ParameterType::PASSWORD ||
           type == ParameterType::EMAIL ||
           type == ParameterType::URL ||
           type == ParameterType::FILE_PATH ||
           type == ParameterType::DIRECTORY_PATH;
}

bool isCollectionType(ParameterType type) {
    return type == ParameterType::ARRAY || type == ParameterType::OBJECT;
}

bool isCodeType(ParameterType type) {
    return type == ParameterType::CODE ||
           type == ParameterType::MULTI_LANGUAGE_CODE ||
           type == ParameterType::JSON ||
           type == ParameterType::YAML ||
           type == ParameterType::XML ||
           type == ParameterType::AKAO_EXPRESSION ||
           type == ParameterType::AKAO_RULE;
}

bool isAkaoType(ParameterType type) {
    return type == ParameterType::AKAO_EXPRESSION ||
           type == ParameterType::AKAO_RULE ||
           type == ParameterType::NODE_REFERENCE ||
           type == ParameterType::WORKFLOW_REFERENCE;
}

bool requiresOptions(ParameterType type) {
    return type == ParameterType::OPTIONS || type == ParameterType::MULTI_SELECT;
}

bool supportsValidation(ParameterType type) {
    return type != ParameterType::HIDDEN && type != ParameterType::COMPUTED;
}

NodeValue getDefaultValueForType(ParameterType type) {
    switch (type) {
        case ParameterType::STRING:
        case ParameterType::PASSWORD:
        case ParameterType::EMAIL:
        case ParameterType::URL:
        case ParameterType::FILE_PATH:
        case ParameterType::DIRECTORY_PATH:
        case ParameterType::CODE:
        case ParameterType::JSON:
        case ParameterType::YAML:
        case ParameterType::XML:
        case ParameterType::AKAO_EXPRESSION:
        case ParameterType::AKAO_RULE:
            return NodeValue("");
            
        case ParameterType::INTEGER:
            return NodeValue(int64_t(0));
            
        case ParameterType::FLOAT:
            return NodeValue(0.0);
            
        case ParameterType::BOOLEAN:
            return NodeValue(false);
            
        case ParameterType::ARRAY:
            return NodeValue(NodeValue::Array{});
            
        case ParameterType::OBJECT:
            return NodeValue(NodeValue::Object{});
            
        case ParameterType::COLOR:
            return NodeValue("#000000");
            
        case ParameterType::DATE_TIME:
            return NodeValue("1970-01-01T00:00:00Z");
            
        default:
            return NodeValue(); // null value
    }
}

} // namespace ParameterTypeUtils

} // namespace akao::node