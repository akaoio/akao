/**
 * @id: akao:file:core:node:interface:nodedefinition_ext:v1
 * @doc: Extended node definition system providing comprehensive parameter definitions, UI metadata, validation rules, and documentation support for node-based workflow system. Includes parameter types, constraints, conditional display, and example management.
 * @specification: Extended node definition with comprehensive parameter and UI support
 * @scope: Core node definition extension system
 * @timeline: 2025-07-10
 * @rationale: Provide complete node definition system with rich parameter and UI support
 * @methodology: Extended definition classes with comprehensive metadata and validation
 * @references: ["akao:class:core:node:interface:nodeparameter:v1", "akao:class:core:node:interface:parametergroup:v1"]
 */

#pragma once

#include "../step1-1-1/nodevalue.hpp"
#include <string>
#include <vector>
#include <map>
#include <optional>
#include <functional>

namespace akao::node {

// Forward declarations
class ValidationResult;

/**
 * @id: akao:class:core:node:interface:parametertype:v1
 * @doc: Parameter type enumeration for node configuration defining all supported input types including primitives, collections, files, code blocks, and Akao-specific types like .a format expressions with comprehensive type validation and conversion support.
 * @specification: Complete parameter type enumeration with Akao-specific extensions
 * @scope: Core node parameter type system
 * @timeline: 2025-07-10
 * @rationale: Provide comprehensive type system for node parameter configuration
 * @methodology: Enumeration-based type classification with validation and conversion support
 * @references: ["akao:class:core:node:interface:nodeparameter:v1"]
 */
enum class ParameterType {
    // Basic types
    STRING,
    INTEGER,
    FLOAT,
    BOOLEAN,
    
    // Collection types
    ARRAY,
    OBJECT,
    
    // Advanced types
    FILE_PATH,
    DIRECTORY_PATH,
    URL,
    EMAIL,
    PASSWORD,
    
    // UI types
    OPTIONS,              // Dropdown selection
    MULTI_SELECT,         // Multiple selection
    RESOURCE_LOCATOR,     // Reference to other resources
    DATE_TIME,            // Date/time picker
    COLOR,                // Color picker
    
    // Code types
    CODE,                 // Generic code editor
    MULTI_LANGUAGE_CODE,  // Code with language selection
    JSON,                 // JSON editor
    YAML,                 // YAML editor
    XML,                  // XML editor
    
    // Akao-specific types
    AKAO_EXPRESSION,      // .a format expression editor
    AKAO_RULE,            // .a format rule definition
    NODE_REFERENCE,       // Reference to another node
    WORKFLOW_REFERENCE,   // Reference to another workflow
    
    // Binary types
    BINARY_DATA,          // Binary data input
    FILE_UPLOAD,          // File upload widget
    
    // Special types
    HIDDEN,               // Hidden parameter
    COMPUTED,             // Computed from other parameters
    DYNAMIC               // Dynamic type based on context
};

/**
 * @id: akao:class:core:node:interface:parameteroption:v1
 * @doc: Parameter option definition for dropdown and selection parameters providing display name, value, description, and conditional availability to create rich selection interfaces with context-sensitive options and proper internationalization support.
 * @specification: Parameter option with display metadata and conditional availability
 * @scope: Core node parameter option system
 * @timeline: 2025-07-10
 * @rationale: Provide rich option definitions for selection parameters
 * @methodology: Option container with display metadata and conditional logic
 * @references: ["akao:class:core:node:interface:nodeparameter:v1"]
 */
class ParameterOption {
public:
    ParameterOption(const std::string& value, const std::string& display_name);
    ParameterOption(const std::string& value, const std::string& display_name, const std::string& description);
    
    const std::string& getValue() const { return value_; }
    const std::string& getDisplayName() const { return display_name_; }
    const std::string& getDescription() const { return description_; }
    
    void setDescription(const std::string& desc) { description_ = desc; }
    
    // Icon and visual metadata
    const std::string& getIcon() const { return icon_; }
    void setIcon(const std::string& icon) { icon_ = icon; }
    
    // Conditional availability
    const std::string& getCondition() const { return condition_; }
    void setCondition(const std::string& condition) { condition_ = condition; }
    
    // Grouping
    const std::string& getGroup() const { return group_; }
    void setGroup(const std::string& group) { group_ = group; }
    
    bool isEnabled() const { return enabled_; }
    void setEnabled(bool enabled) { enabled_ = enabled; }
    
    std::string toString() const;

private:
    std::string value_;
    std::string display_name_;
    std::string description_;
    std::string icon_;
    std::string condition_;
    std::string group_;
    bool enabled_ = true;
};

/**
 * @id: akao:class:core:node:interface:parametervalidation:v1
 * @doc: Parameter validation rules container defining constraints, patterns, ranges, and custom validation logic for node parameters with support for .a format validation expressions and comprehensive error reporting for invalid configurations.
 * @specification: Comprehensive parameter validation with constraints and custom rules
 * @scope: Core node parameter validation system
 * @timeline: 2025-07-10
 * @rationale: Provide robust validation for node parameter configurations
 * @methodology: Validation rule container with constraint definitions and custom logic
 * @references: ["akao:class:core:node:interface:nodeparameter:v1"]
 */
struct ParameterValidation {
    // Basic constraints
    bool required = false;
    
    // Numeric constraints
    std::optional<double> min_value;
    std::optional<double> max_value;
    
    // String constraints
    std::optional<size_t> min_length;
    std::optional<size_t> max_length;
    std::string pattern;  // Regex pattern
    
    // Array constraints
    std::optional<size_t> min_items;
    std::optional<size_t> max_items;
    
    // File constraints
    std::vector<std::string> allowed_extensions;
    std::optional<size_t> max_file_size_mb;
    
    // Custom validation
    std::string akao_validation_expression;  // .a format validation
    std::string custom_validation_message;
    
    // Dependency validation
    std::vector<std::string> depends_on;  // Other parameters this depends on
    std::string dependency_condition;     // Condition for dependency
    
    bool isEmpty() const;
    std::string toString() const;
};

/**
 * @id: akao:class:core:node:interface:nodeparameter:v1
 * @doc: Complete node parameter definition with type information, validation rules, UI metadata, conditional display logic, and default values providing comprehensive parameter specification for rich node configuration interfaces and automatic validation.
 * @specification: Complete node parameter with validation, UI metadata, and conditional logic
 * @scope: Core node parameter definition system
 * @timeline: 2025-07-10
 * @rationale: Provide comprehensive parameter definition for node configuration
 * @methodology: Parameter definition with complete metadata and validation support
 * @references: ["akao:class:core:node:interface:inode:v1", "akao:class:core:node:interface:parametertype:v1"]
 */
class NodeParameter {
public:
    NodeParameter(const std::string& name, ParameterType type);
    NodeParameter(const std::string& name, ParameterType type, const NodeValue& default_value);
    
    // Basic information
    const std::string& getName() const { return name_; }
    ParameterType getType() const { return type_; }
    
    // Display information
    const std::string& getDisplayName() const { return display_name_; }
    void setDisplayName(const std::string& name) { display_name_ = name; }
    
    const std::string& getDescription() const { return description_; }
    void setDescription(const std::string& desc) { description_ = desc; }
    
    const std::string& getPlaceholder() const { return placeholder_; }
    void setPlaceholder(const std::string& placeholder) { placeholder_ = placeholder; }
    
    // Default value
    const NodeValue& getDefaultValue() const { return default_value_; }
    void setDefaultValue(const NodeValue& value) { default_value_ = value; }
    
    // Options (for OPTIONS and MULTI_SELECT types)
    const std::vector<ParameterOption>& getOptions() const { return options_; }
    void addOption(const ParameterOption& option) { options_.push_back(option); }
    void setOptions(const std::vector<ParameterOption>& options) { options_ = options; }
    
    // Validation
    const ParameterValidation& getValidation() const { return validation_; }
    void setValidation(const ParameterValidation& validation) { validation_ = validation; }
    
    // UI behavior
    bool isHidden() const { return hidden_; }
    void setHidden(bool hidden) { hidden_ = hidden; }
    
    bool isReadOnly() const { return read_only_; }
    void setReadOnly(bool read_only) { read_only_ = read_only; }
    
    // Conditional display
    const std::string& getDisplayCondition() const { return display_condition_; }
    void setDisplayCondition(const std::string& condition) { display_condition_ = condition; }
    
    // Grouping
    const std::string& getGroup() const { return group_; }
    void setGroup(const std::string& group) { group_ = group; }
    
    int getDisplayOrder() const { return display_order_; }
    void setDisplayOrder(int order) { display_order_ = order; }
    
    // Advanced features
    bool supportsVariables() const { return supports_variables_; }
    void setSupportsVariables(bool supports) { supports_variables_ = supports; }
    
    const std::string& getHelpUrl() const { return help_url_; }
    void setHelpUrl(const std::string& url) { help_url_ = url; }
    
    // Validation methods
    ValidationResult validate(const NodeValue& value) const;
    bool isRequired() const { return validation_.required; }
    
    // Serialization
    std::string toString() const;

private:
    // Basic information
    std::string name_;
    ParameterType type_;
    
    // Display information
    std::string display_name_;
    std::string description_;
    std::string placeholder_;
    
    // Value and options
    NodeValue default_value_;
    std::vector<ParameterOption> options_;
    
    // Validation
    ParameterValidation validation_;
    
    // UI behavior
    bool hidden_ = false;
    bool read_only_ = false;
    std::string display_condition_;
    
    // Grouping
    std::string group_;
    int display_order_ = 0;
    
    // Advanced features
    bool supports_variables_ = false;
    std::string help_url_;
};

/**
 * @id: akao:class:core:node:interface:parametergroup:v1
 * @doc: Parameter grouping container for organizing related parameters into logical sections with collapsible UI support, conditional display, and hierarchical organization to create intuitive node configuration interfaces with proper parameter organization.
 * @specification: Parameter group with UI organization and conditional display
 * @scope: Core node parameter grouping system
 * @timeline: 2025-07-10
 * @rationale: Provide logical organization of parameters for better user experience
 * @methodology: Group container with UI metadata and conditional display logic
 * @references: ["akao:class:core:node:interface:nodeparameter:v1", "akao:class:core:node:interface:nodedefinition:v1"]
 */
class ParameterGroup {
public:
    ParameterGroup(const std::string& name, const std::string& display_name);
    
    const std::string& getName() const { return name_; }
    const std::string& getDisplayName() const { return display_name_; }
    
    const std::string& getDescription() const { return description_; }
    void setDescription(const std::string& desc) { description_ = desc; }
    
    // Parameters in this group
    const std::vector<NodeParameter>& getParameters() const { return parameters_; }
    void addParameter(const NodeParameter& param) { parameters_.push_back(param); }
    void setParameters(const std::vector<NodeParameter>& params) { parameters_ = params; }
    
    // UI behavior
    bool isCollapsible() const { return collapsible_; }
    void setCollapsible(bool collapsible) { collapsible_ = collapsible; }
    
    bool isDefaultOpen() const { return default_open_; }
    void setDefaultOpen(bool open) { default_open_ = open; }
    
    // Conditional display
    const std::string& getDisplayCondition() const { return display_condition_; }
    void setDisplayCondition(const std::string& condition) { display_condition_ = condition; }
    
    // Ordering
    int getDisplayOrder() const { return display_order_; }
    void setDisplayOrder(int order) { display_order_ = order; }
    
    // Icon and styling
    const std::string& getIcon() const { return icon_; }
    void setIcon(const std::string& icon) { icon_ = icon; }
    
    std::string toString() const;

private:
    std::string name_;
    std::string display_name_;
    std::string description_;
    std::vector<NodeParameter> parameters_;
    
    // UI behavior
    bool collapsible_ = true;
    bool default_open_ = true;
    std::string display_condition_;
    
    // Ordering and styling
    int display_order_ = 0;
    std::string icon_;
};

// Utility functions for parameter type handling
namespace ParameterTypeUtils {
    std::string toString(ParameterType type);
    ParameterType fromString(const std::string& type_string);
    bool isNumericType(ParameterType type);
    bool isStringType(ParameterType type);
    bool isCollectionType(ParameterType type);
    bool isCodeType(ParameterType type);
    bool isAkaoType(ParameterType type);
    bool requiresOptions(ParameterType type);
    bool supportsValidation(ParameterType type);
    NodeValue getDefaultValueForType(ParameterType type);
}

} // namespace akao::node