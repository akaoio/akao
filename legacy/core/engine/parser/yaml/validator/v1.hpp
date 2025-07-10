/**
 * @id: akao:file:core:engine:parser_yaml_validator:v1
 * @doc: Production-ready YAML schema validator providing comprehensive validation capabilities for YAML documents with schema support and AKAO-specific validation rules. Features basic type checking, standard YAML validation, strict schema compliance, and specialized AKAO philosophy/rule validation. Supports validation levels from basic to AKAO-specific, schema rule management, error reporting with severity levels, and libyaml-inspired validation patterns for reliable YAML document verification in the Akao framework.
 * @specification: Header for YAML schema validator with comprehensive validation levels and AKAO-specific rule support
 * @scope: Core engine parser YAML validation framework with schema compliance and error reporting
 * @timeline: 2025-07-09
 * @rationale: Provide comprehensive YAML validation for philosophy and rule documents with schema compliance verification
 * @methodology: Multi-level validation system with schema rules, AKAO-specific validators, and detailed error reporting
 * @references: ["akao:file:core:engine:parser_yaml_node:v1", "akao:philosophy:validation:universal:v1", "akao:philosophy:structure:isolation:v1"]
 */

#pragma once

#include "../node/v1.hpp"
#include <string>
#include <vector>
#include <memory>

namespace akao::core::engine::parser {

/**
 * @brief YAML Schema Validator for Akao framework
 * 
 * Production-ready YAML validation system with schema support
 * Inspired by libyaml validation patterns and best practices
 * 
 * Philosophy Compliance:
 * - akao:philosophy:structure:isolation:v1 (one class per file)
 * - akao:philosophy:language:isolation:v1 (pure C++ only)
 * - akao:philosophy:validation:universal:v1 (comprehensive validation)
 */
class YamlValidator {
public:
    enum class ValidationLevel {
        BASIC,      // Basic type checking only
        STANDARD,   // Standard YAML validation
        STRICT,     // Strict schema compliance
        AKAO_SPEC   // AKAO-specific validation rules
    };

    struct ValidationError {
        std::string message;
        std::string path;
        size_t line;
        size_t column;
        std::string severity; // "error", "warning", "info"
    };

    struct ValidationResult {
        bool is_valid;
        std::vector<ValidationError> errors;
        std::vector<ValidationError> warnings;
        std::string summary;
    };

    struct SchemaRule {
        std::string path;
        std::string required_type;
        bool is_required;
        std::vector<std::string> allowed_values;
        bool allow_empty;
    };

private:
    ValidationLevel level_;
    std::vector<SchemaRule> schema_rules_;
    
public:
    // Constructor
    explicit YamlValidator(ValidationLevel level = ValidationLevel::STANDARD);
    
    // Main validation methods
    ValidationResult validate(const std::shared_ptr<YamlNode>& root) const;
    ValidationResult validateFile(const std::string& file_path) const;
    ValidationResult validateString(const std::string& yaml_content) const;
    
    // Schema management
    void addSchemaRule(const SchemaRule& rule);
    void loadSchema(const std::string& schema_file);
    void clearSchema();
    
    // AKAO-specific validation
    ValidationResult validateAkaoPhilosophy(const std::shared_ptr<YamlNode>& root) const;
    ValidationResult validateAkaoRule(const std::shared_ptr<YamlNode>& root) const;
    ValidationResult validateAkaoRuleset(const std::shared_ptr<YamlNode>& root) const;
    
    // Utility methods
    void setValidationLevel(ValidationLevel level);
    ValidationLevel getValidationLevel() const;
    
private:
    // Core validation logic
    ValidationResult performValidation(const std::shared_ptr<YamlNode>& root) const;
    void validateNode(const std::shared_ptr<YamlNode>& node, const std::string& path, ValidationResult& result) const;
    void validateMapping(const std::shared_ptr<YamlNode>& node, const std::string& path, ValidationResult& result) const;
    void validateSequence(const std::shared_ptr<YamlNode>& node, const std::string& path, ValidationResult& result) const;
    
    // Schema validation
    void applySchemaRules(const std::shared_ptr<YamlNode>& root, ValidationResult& result) const;
    void validateSchemaRule(const SchemaRule& rule, const std::shared_ptr<YamlNode>& root, ValidationResult& result) const;
    
    // AKAO-specific validators
    void validateMetadataSection(const std::shared_ptr<YamlNode>& metadata, ValidationResult& result) const;
    void validatePhilosophyLinks(const std::shared_ptr<YamlNode>& links, ValidationResult& result) const;
    void validateRuleCollection(const std::shared_ptr<YamlNode>& rules, ValidationResult& result) const;
    
    // Utility methods
    std::shared_ptr<YamlNode> getNodeAtPath(const std::shared_ptr<YamlNode>& root, const std::string& path) const;
    std::string generatePath(const std::string& base_path, const std::string& key) const;
    void addError(ValidationResult& result, const std::string& message, const std::string& path, 
                  const std::string& severity = "error") const;
};

} // namespace akao::core::engine::parser
