/**
 * @id akao.evolve.node_parameter.v1
 * @doc Implementation of rich parameter system with comprehensive validation logic including type checking, constraint validation, and Akao Pure Logic expression evaluation.
 * @specification Complete implementation of parameter validation methods, factory functions, and constraint checking with robust error handling and detailed validation messages.
 * @scope artifacts/evolve/phase1/step1-2-2
 * @timeline 2025-07-10
 * @rationale Provide complete parameter validation system that maintains type safety while supporting Akao's philosophy-driven validation approach.
 * @methodology Template-based type conversion, constraint validation, and mock Akao logic validation for comprehensive parameter processing.
 * @references akao.evolve.node_parameter.v1
 */

#include "node_parameter.hpp"
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>

namespace akao {
namespace evolve {

// ParameterValue implementation

std::string ParameterValue::toString() const {
    try {
        switch (type_) {
            case ParameterType::STRING:
            case ParameterType::AKAO_EXPRESSION:
            case ParameterType::MULTI_LANGUAGE_CODE:
            case ParameterType::FILE_PATH:
            case ParameterType::OPTIONS:
                return std::get<std::string>(value_);
            
            case ParameterType::NUMBER:
                if (std::holds_alternative<int64_t>(value_)) {
                    return std::to_string(std::get<int64_t>(value_));
                } else if (std::holds_alternative<double>(value_)) {
                    return std::to_string(std::get<double>(value_));
                } else {
                    return "0";
                }
            
            case ParameterType::BOOLEAN:
                return std::get<bool>(value_) ? "true" : "false";
            
            case ParameterType::COLLECTION: {
                const auto& vec = std::get<std::vector<std::string>>(value_);
                std::ostringstream oss;
                oss << "[";
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << vec[i] << "\"";
                }
                oss << "]";
                return oss.str();
            }
        }
    } catch (const std::bad_variant_access&) {
        return "ERROR: variant access failed for type " + std::to_string(static_cast<int>(type_));
    }
    return "";
}

// NodeParameter implementation

ValidationResult NodeParameter::validate(const ParameterValue& value) const {
    // Check if required parameter has a value
    auto required_result = validateRequired(value);
    if (!required_result.valid) {
        return required_result;
    }
    
    // Check type compatibility
    auto type_result = validateType(value);
    if (!type_result.valid) {
        return type_result;
    }
    
    // Check constraints (length, range, etc.)
    auto constraint_result = validateConstraints(value);
    if (!constraint_result.valid) {
        return constraint_result;
    }
    
    // Check Akao logic validation if defined
    if (!akao_validation_rule.empty()) {
        auto akao_result = validateWithAkaoLogic(value);
        if (!akao_result.valid) {
            return akao_result;
        }
    }
    
    return ValidationResult::success();
}

ValidationResult NodeParameter::validateWithAkaoLogic(const ParameterValue& value) const {
    if (akao_validation_rule.empty()) {
        return ValidationResult::success();
    }
    
    // Mock Akao logic validation for now
    // In a real implementation, this would integrate with the Akao Pure Logic Engine
    
    // Simple validation rules for demonstration
    if (akao_validation_rule == "string.length($value) > 0") {
        if (value.isString() && value.as<std::string>().empty()) {
            return ValidationResult::failure("Akao validation failed: string must not be empty");
        }
    } else if (akao_validation_rule == "math.is_positive($value)") {
        if (value.isNumber()) {
            try {
                auto int_val = value.as<int64_t>();
                if (int_val <= 0) {
                    return ValidationResult::failure("Akao validation failed: number must be positive");
                }
            } catch (...) {
                try {
                    auto double_val = value.as<double>();
                    if (double_val <= 0.0) {
                        return ValidationResult::failure("Akao validation failed: number must be positive");
                    }
                } catch (...) {
                    return ValidationResult::failure("Akao validation failed: unable to parse number");
                }
            }
        }
    } else if (akao_validation_rule == "filesystem.exists($value)") {
        if (value.isString()) {
            std::string path = value.as<std::string>();
            if (!std::filesystem::exists(path)) {
                return ValidationResult::failure("Akao validation failed: file path does not exist");
            }
        }
    }
    
    return ValidationResult::success();
}

ValidationResult NodeParameter::validateType(const ParameterValue& value) const {
    switch (type) {
        case ParameterType::STRING:
        case ParameterType::AKAO_EXPRESSION:
        case ParameterType::MULTI_LANGUAGE_CODE:
        case ParameterType::FILE_PATH:
            if (!value.isString()) {
                return ValidationResult::failure("Expected string value for parameter '" + name + "'");
            }
            break;
        
        case ParameterType::NUMBER:
            if (!value.isNumber()) {
                return ValidationResult::failure("Expected numeric value for parameter '" + name + "'");
            }
            break;
        
        case ParameterType::BOOLEAN:
            if (!value.isBoolean()) {
                return ValidationResult::failure("Expected boolean value for parameter '" + name + "'");
            }
            break;
        
        case ParameterType::COLLECTION:
            if (!value.isCollection()) {
                return ValidationResult::failure("Expected collection value for parameter '" + name + "'");
            }
            break;
        
        case ParameterType::OPTIONS:
            if (!value.isString()) {
                return ValidationResult::failure("Expected string value for options parameter '" + name + "'");
            } else {
                // Validate that the value is one of the allowed options
                std::string val = value.as<std::string>();
                bool found = false;
                for (const auto& option : options) {
                    if (option.value == val) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    return ValidationResult::failure("Value '" + val + "' is not a valid option for parameter '" + name + "'");
                }
            }
            break;
    }
    
    return ValidationResult::success();
}

ValidationResult NodeParameter::validateConstraints(const ParameterValue& value) const {
    // Length constraints for string-like types
    if ((type == ParameterType::STRING || type == ParameterType::AKAO_EXPRESSION || 
         type == ParameterType::MULTI_LANGUAGE_CODE || type == ParameterType::FILE_PATH) && 
         value.isString()) {
        
        std::string str_val = value.as<std::string>();
        int64_t length = static_cast<int64_t>(str_val.length());
        
        if (min_length.has_value() && length < min_length.value()) {
            return ValidationResult::failure("Parameter '" + name + "' must be at least " + 
                                           std::to_string(min_length.value()) + " characters long");
        }
        
        if (max_length.has_value() && length > max_length.value()) {
            return ValidationResult::failure("Parameter '" + name + "' must be at most " + 
                                           std::to_string(max_length.value()) + " characters long");
        }
    }
    
    // Value constraints for numeric types
    if (type == ParameterType::NUMBER && value.isNumber()) {
        double num_val;
        try {
            auto int_val = value.as<int64_t>();
            num_val = static_cast<double>(int_val);
        } catch (...) {
            try {
                num_val = value.as<double>();
            } catch (...) {
                return ValidationResult::failure("Parameter '" + name + "' has invalid numeric value");
            }
        }
        
        if (min_value.has_value() && num_val < min_value.value()) {
            return ValidationResult::failure("Parameter '" + name + "' must be at least " + 
                                           std::to_string(min_value.value()));
        }
        
        if (max_value.has_value() && num_val > max_value.value()) {
            return ValidationResult::failure("Parameter '" + name + "' must be at most " + 
                                           std::to_string(max_value.value()));
        }
    }
    
    // File extension validation for FILE_PATH type
    if (type == ParameterType::FILE_PATH && value.isString() && !allowed_extensions.empty()) {
        std::string path = value.as<std::string>();
        std::filesystem::path fs_path(path);
        std::string extension = fs_path.extension().string();
        
        bool valid_extension = false;
        for (const auto& ext : allowed_extensions) {
            if (extension == ext || (ext.front() != '.' && extension == "." + ext)) {
                valid_extension = true;
                break;
            }
        }
        
        if (!valid_extension) {
            std::ostringstream allowed_exts;
            for (size_t i = 0; i < allowed_extensions.size(); ++i) {
                if (i > 0) allowed_exts << ", ";
                allowed_exts << allowed_extensions[i];
            }
            return ValidationResult::failure("File extension '" + extension + "' is not allowed. " +
                                           "Allowed extensions: " + allowed_exts.str());
        }
    }
    
    return ValidationResult::success();
}

ValidationResult NodeParameter::validateRequired(const ParameterValue& value) const {
    if (!required) {
        return ValidationResult::success();
    }
    
    // Check if the value is "empty" based on its type
    switch (type) {
        case ParameterType::STRING:
        case ParameterType::AKAO_EXPRESSION:
        case ParameterType::MULTI_LANGUAGE_CODE:
        case ParameterType::FILE_PATH:
        case ParameterType::OPTIONS:
            if (value.isString() && value.as<std::string>().empty()) {
                return ValidationResult::failure("Required parameter '" + name + "' cannot be empty");
            }
            break;
        
        case ParameterType::COLLECTION:
            if (value.isCollection() && value.as<std::vector<std::string>>().empty()) {
                return ValidationResult::failure("Required parameter '" + name + "' cannot be empty");
            }
            break;
        
        case ParameterType::NUMBER:
        case ParameterType::BOOLEAN:
            // Numbers and booleans always have a value
            break;
    }
    
    return ValidationResult::success();
}

// Factory methods

NodeParameter NodeParameter::createString(const std::string& name, 
                                        const std::string& display,
                                        bool required,
                                        const std::string& default_val) {
    NodeParameter param(name, display, ParameterType::STRING, required);
    param.default_value = ParameterValue(default_val);
    return param;
}

NodeParameter NodeParameter::createNumber(const std::string& name,
                                        const std::string& display, 
                                        bool required,
                                        double default_val) {
    NodeParameter param(name, display, ParameterType::NUMBER, required);
    param.default_value = ParameterValue(default_val);
    return param;
}

NodeParameter NodeParameter::createBoolean(const std::string& name,
                                         const std::string& display,
                                         bool required,
                                         bool default_val) {
    NodeParameter param(name, display, ParameterType::BOOLEAN, required);
    param.default_value = ParameterValue(default_val);
    return param;
}

NodeParameter NodeParameter::createOptions(const std::string& name,
                                         const std::string& display,
                                         const std::vector<ParameterOption>& opts,
                                         bool required,
                                         const std::string& default_val) {
    NodeParameter param(name, display, ParameterType::OPTIONS, required);
    param.options = opts;
    param.default_value = ParameterValue(default_val);
    return param;
}

NodeParameter NodeParameter::createAkaoExpression(const std::string& name,
                                                const std::string& display,
                                                bool required,
                                                const std::string& default_expr) {
    NodeParameter param(name, display, ParameterType::AKAO_EXPRESSION, required);
    param.default_value = ParameterValue(default_expr);
    return param;
}

NodeParameter NodeParameter::createFilePath(const std::string& name,
                                          const std::string& display,
                                          const std::vector<std::string>& extensions,
                                          bool required,
                                          const std::string& default_path) {
    NodeParameter param(name, display, ParameterType::FILE_PATH, required);
    param.allowed_extensions = extensions;
    param.default_value = ParameterValue(default_path);
    return param;
}

// Constraint setter methods

NodeParameter& NodeParameter::setLengthConstraints(int64_t min_len, int64_t max_len) {
    min_length = min_len;
    max_length = max_len;
    return *this;
}

NodeParameter& NodeParameter::setValueConstraints(double min_val, double max_val) {
    min_value = min_val;
    max_value = max_val;
    return *this;
}

NodeParameter& NodeParameter::setAkaoValidation(const std::string& rule) {
    akao_validation_rule = rule;
    return *this;
}

NodeParameter& NodeParameter::setDescription(const std::string& desc) {
    description = desc;
    return *this;
}

// ParameterSet implementation

void ParameterSet::addParameter(const NodeParameter& param) {
    parameters_.push_back(param);
}

void ParameterSet::addParameter(NodeParameter&& param) {
    parameters_.push_back(std::move(param));
}

std::optional<NodeParameter> ParameterSet::getParameter(const std::string& name) const {
    for (const auto& param : parameters_) {
        if (param.name == name) {
            return param;
        }
    }
    return std::nullopt;
}

ValidationResult ParameterSet::validateAll(const std::map<std::string, ParameterValue>& values) const {
    // Validate all provided values
    for (const auto& [name, value] : values) {
        auto param = getParameter(name);
        if (!param.has_value()) {
            return ValidationResult::failure("Unknown parameter: " + name);
        }
        
        auto result = param->validate(value);
        if (!result.valid) {
            return result;
        }
    }
    
    // Check that all required parameters are provided
    for (const auto& param : parameters_) {
        if (param.required && values.find(param.name) == values.end()) {
            return ValidationResult::failure("Required parameter '" + param.name + "' is missing");
        }
    }
    
    return ValidationResult::success();
}

std::vector<std::string> ParameterSet::getRequiredParameters() const {
    std::vector<std::string> required;
    for (const auto& param : parameters_) {
        if (param.required) {
            required.push_back(param.name);
        }
    }
    return required;
}

std::vector<std::string> ParameterSet::getAllParameterNames() const {
    std::vector<std::string> names;
    for (const auto& param : parameters_) {
        names.push_back(param.name);
    }
    return names;
}

} // namespace evolve
} // namespace akao