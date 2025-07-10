/**
 * @id akao.evolve.node_parameter.v1
 * @doc Rich parameter system for node configuration inspired by N8N but built for Akao's philosophy-driven architecture. Provides comprehensive parameter types including Akao Pure Logic expressions for advanced validation and processing.
 * @specification Parameter definition system supporting primitive types, collections, code blocks, and Akao-specific validation rules with runtime type checking and validation execution.
 * @scope artifacts/evolve/phase1/step1-2-2
 * @timeline 2025-07-10
 * @rationale Enable flexible, type-safe node configuration while maintaining Akao's philosophical foundations and .a format integration for parameter validation.
 * @methodology Object-oriented parameter definition with enum-based type system, variant storage, and integrated Akao Pure Logic validation engine.
 * @references akao.evolve.nodevalue.v1, akao.engine.logic.runtime.v1
 */

#ifndef AKAO_EVOLVE_NODE_PARAMETER_V1_HPP
#define AKAO_EVOLVE_NODE_PARAMETER_V1_HPP

#include <string>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <stdexcept>

namespace akao {
namespace evolve {

/**
 * @brief Enumeration of supported parameter types
 */
enum class ParameterType {
    STRING,                // Simple string input
    NUMBER,               // Numeric input (int64_t or double)
    BOOLEAN,              // Boolean true/false
    OPTIONS,              // Single selection from predefined options
    COLLECTION,           // Array/list of values
    AKAO_EXPRESSION,      // .a format logical expression
    MULTI_LANGUAGE_CODE,  // Code in various programming languages
    FILE_PATH             // File system path with validation
};

/**
 * @brief Single option for OPTIONS parameter type
 */
struct ParameterOption {
    std::string value;
    std::string display_name;
    std::string description;
    
    ParameterOption() = default;
    ParameterOption(const std::string& val, const std::string& name, const std::string& desc = "")
        : value(val), display_name(name), description(desc) {}
};

/**
 * @brief Universal parameter value storage
 */
class ParameterValue {
public:
    using ValueVariant = std::variant<
        std::string,
        int64_t,
        double,
        bool,
        std::vector<std::string>
    >;
    
private:
    ParameterType type_;
    ValueVariant value_;
    
public:
    ParameterValue() : type_(ParameterType::STRING), value_(std::string{}) {}
    
    explicit ParameterValue(const char* val) 
        : type_(ParameterType::STRING), value_(std::string(val)) {}
    
    explicit ParameterValue(const std::string& val) 
        : type_(ParameterType::STRING), value_(val) {}
    
    explicit ParameterValue(int64_t val) 
        : type_(ParameterType::NUMBER), value_(val) {}
    
    explicit ParameterValue(double val) 
        : type_(ParameterType::NUMBER), value_(val) {}
    
    explicit ParameterValue(bool val) 
        : type_(ParameterType::BOOLEAN), value_(val) {}
    
    explicit ParameterValue(const std::vector<std::string>& val) 
        : type_(ParameterType::COLLECTION), value_(val) {}
    
    ParameterValue(ParameterType type, const ValueVariant& value) 
        : type_(type), value_(value) {}
    
    ParameterType getType() const { return type_; }
    const ValueVariant& getVariant() const { return value_; }
    
    template<typename T>
    T as() const {
        try {
            return std::get<T>(value_);
        } catch (const std::bad_variant_access&) {
            throw std::runtime_error("Type mismatch: attempted to access variant with wrong type");
        }
    }
    
    bool isString() const { return type_ == ParameterType::STRING; }
    bool isNumber() const { return type_ == ParameterType::NUMBER; }
    bool isBoolean() const { return type_ == ParameterType::BOOLEAN; }
    bool isCollection() const { return type_ == ParameterType::COLLECTION; }
    
    std::string toString() const;
};

/**
 * @brief Validation result for parameter validation
 */
struct ValidationResult {
    bool valid;
    std::string error_message;
    
    ValidationResult() : valid(true) {}
    ValidationResult(bool v, const std::string& msg = "") : valid(v), error_message(msg) {}
    
    static ValidationResult success() { return ValidationResult(true); }
    static ValidationResult failure(const std::string& msg) { return ValidationResult(false, msg); }
};

/**
 * @brief Rich parameter definition for node configuration
 */
class NodeParameter {
public:
    std::string name;
    std::string display_name;
    std::string description;
    ParameterType type;
    ParameterValue default_value;
    bool required;
    std::vector<ParameterOption> options;
    std::string akao_validation_rule;
    
    // Additional type-specific constraints
    std::optional<int64_t> min_length;
    std::optional<int64_t> max_length;
    std::optional<double> min_value;
    std::optional<double> max_value;
    std::vector<std::string> allowed_extensions; // For FILE_PATH type
    
public:
    NodeParameter() : type(ParameterType::STRING), required(false) {}
    
    NodeParameter(const std::string& name_, 
                  const std::string& display_,
                  ParameterType type_,
                  bool required_ = false)
        : name(name_), display_name(display_), type(type_), required(required_) {}
    
    /**
     * @brief Validate a parameter value against this parameter definition
     */
    ValidationResult validate(const ParameterValue& value) const;
    
    /**
     * @brief Validate using Akao Pure Logic expression if defined
     */
    ValidationResult validateWithAkaoLogic(const ParameterValue& value) const;
    
    /**
     * @brief Check if value meets basic type requirements
     */
    ValidationResult validateType(const ParameterValue& value) const;
    
    /**
     * @brief Check if value meets constraint requirements (length, range, etc.)
     */
    ValidationResult validateConstraints(const ParameterValue& value) const;
    
    /**
     * @brief Check if required parameter has a value
     */
    ValidationResult validateRequired(const ParameterValue& value) const;
    
    /**
     * @brief Factory methods for common parameter types
     */
    static NodeParameter createString(const std::string& name, 
                                    const std::string& display,
                                    bool required = false,
                                    const std::string& default_val = "");
    
    static NodeParameter createNumber(const std::string& name,
                                    const std::string& display, 
                                    bool required = false,
                                    double default_val = 0.0);
    
    static NodeParameter createBoolean(const std::string& name,
                                     const std::string& display,
                                     bool required = false,
                                     bool default_val = false);
    
    static NodeParameter createOptions(const std::string& name,
                                     const std::string& display,
                                     const std::vector<ParameterOption>& options,
                                     bool required = false,
                                     const std::string& default_val = "");
    
    static NodeParameter createAkaoExpression(const std::string& name,
                                            const std::string& display,
                                            bool required = false,
                                            const std::string& default_expr = "");
    
    static NodeParameter createFilePath(const std::string& name,
                                      const std::string& display,
                                      const std::vector<std::string>& extensions = {},
                                      bool required = false,
                                      const std::string& default_path = "");
    
    /**
     * @brief Set validation constraints
     */
    NodeParameter& setLengthConstraints(int64_t min_len, int64_t max_len);
    NodeParameter& setValueConstraints(double min_val, double max_val);
    NodeParameter& setAkaoValidation(const std::string& rule);
    NodeParameter& setDescription(const std::string& desc);
};

/**
 * @brief Collection of parameters for a node
 */
class ParameterSet {
private:
    std::vector<NodeParameter> parameters_;
    
public:
    void addParameter(const NodeParameter& param);
    void addParameter(NodeParameter&& param);
    
    const std::vector<NodeParameter>& getParameters() const { return parameters_; }
    
    std::optional<NodeParameter> getParameter(const std::string& name) const;
    
    ValidationResult validateAll(const std::map<std::string, ParameterValue>& values) const;
    
    std::vector<std::string> getRequiredParameters() const;
    std::vector<std::string> getAllParameterNames() const;
    
    size_t size() const { return parameters_.size(); }
    bool empty() const { return parameters_.empty(); }
};

} // namespace evolve
} // namespace akao

#endif // AKAO_EVOLVE_NODE_PARAMETER_V1_HPP