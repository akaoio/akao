#include "v1.hpp"
#include "../parser/v1.hpp"
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>

namespace akao::core::engine::parser {

// Constructor
YamlValidator::YamlValidator(ValidationLevel level) : level_(level) {}

// Main validation methods
YamlValidator::ValidationResult YamlValidator::validate(const std::shared_ptr<YamlNode>& root) const {
    ValidationResult result;
    result.is_valid = true;
    
    if (!root) {
        addError(result, "Root node is null", "/", "error");
        return result;
    }
    
    return performValidation(root);
}

YamlValidator::ValidationResult YamlValidator::validateFile(const std::string& file_path) const {
    ValidationResult result;
    result.is_valid = true;
    
    try {
        YamlParser parser;
        auto root = parser.parseFile(file_path);
        return validate(root);
    } catch (const std::exception& e) {
        addError(result, "Failed to parse file: " + std::string(e.what()), file_path, "error");
        return result;
    }
}

YamlValidator::ValidationResult YamlValidator::validateString(const std::string& yaml_content) const {
    ValidationResult result;
    result.is_valid = true;
    
    try {
        YamlParser parser;
        auto root = parser.parse(yaml_content);
        return validate(root);
    } catch (const std::exception& e) {
        addError(result, "Failed to parse YAML content: " + std::string(e.what()), "/", "error");
        return result;
    }
}

// Schema management
void YamlValidator::addSchemaRule(const SchemaRule& rule) {
    schema_rules_.push_back(rule);
}

void YamlValidator::clearSchema() {
    schema_rules_.clear();
}

// AKAO-specific validation
YamlValidator::ValidationResult YamlValidator::validateAkaoPhilosophy(const std::shared_ptr<YamlNode>& root) const {
    ValidationResult result = validate(root);
    
    // Check required sections for AKAO philosophy
    if (!root->hasKey("metadata")) {
        addError(result, "Missing required 'metadata' section", "/", "error");
    } else {
        validateMetadataSection((*root)["metadata"], result);
    }
    
    if (!root->hasKey("description")) {
        addError(result, "Missing required 'description' field", "/", "error");
    }
    
    if (!root->hasKey("formal_proof")) {
        addError(result, "Missing required 'formal_proof' field", "/", "error");
    }
    
    if (!root->hasKey("implementation")) {
        addError(result, "Missing required 'implementation' section", "/", "error");
    }
    
    return result;
}

YamlValidator::ValidationResult YamlValidator::validateAkaoRule(const std::shared_ptr<YamlNode>& root) const {
    ValidationResult result = validate(root);
    
    // Check required sections for AKAO rule
    if (!root->hasKey("metadata")) {
        addError(result, "Missing required 'metadata' section", "/", "error");
    }
    
    if (!root->hasKey("philosophy_links")) {
        addError(result, "Missing required 'philosophy_links' section", "/", "error");
    } else {
        validatePhilosophyLinks((*root)["philosophy_links"], result);
    }
    
    if (!root->hasKey("validation")) {
        addError(result, "Missing required 'validation' section", "/", "error");
    }
    
    return result;
}

YamlValidator::ValidationResult YamlValidator::validateAkaoRuleset(const std::shared_ptr<YamlNode>& root) const {
    ValidationResult result = validate(root);
    
    // Check required sections for AKAO ruleset
    if (!root->hasKey("metadata")) {
        addError(result, "Missing required 'metadata' section", "/", "error");
    }
    
    if (!root->hasKey("philosophy_links")) {
        addError(result, "Missing required 'philosophy_links' section", "/", "error");
    }
    
    if (!root->hasKey("rule_collection")) {
        addError(result, "Missing required 'rule_collection' section", "/", "error");
    } else {
        validateRuleCollection((*root)["rule_collection"], result);
    }
    
    return result;
}

// Utility methods
void YamlValidator::setValidationLevel(ValidationLevel level) {
    level_ = level;
}

YamlValidator::ValidationLevel YamlValidator::getValidationLevel() const {
    return level_;
}

// Private implementation methods
YamlValidator::ValidationResult YamlValidator::performValidation(const std::shared_ptr<YamlNode>& root) const {
    ValidationResult result;
    result.is_valid = true;
    
    // Perform basic validation
    validateNode(root, "/", result);
    
    // Apply schema rules if any
    if (!schema_rules_.empty()) {
        applySchemaRules(root, result);
    }
    
    // Generate summary
    if (result.errors.empty() && result.warnings.empty()) {
        result.summary = "✅ YAML validation passed with no issues";
    } else {
        std::stringstream ss;
        ss << "Validation completed with " << result.errors.size() << " errors and " 
           << result.warnings.size() << " warnings";
        result.summary = ss.str();
        
        if (!result.errors.empty()) {
            result.is_valid = false;
        }
    }
    
    return result;
}

void YamlValidator::validateNode(const std::shared_ptr<YamlNode>& node, const std::string& path, ValidationResult& result) const {
    if (!node) {
        addError(result, "Node is null", path, "error");
        return;
    }
    
    // Validate based on node type
    if (node->isMapping()) {
        validateMapping(node, path, result);
    } else if (node->isSequence()) {
        validateSequence(node, path, result);
    }
    // Scalar values are generally valid as-is
}

void YamlValidator::validateMapping(const std::shared_ptr<YamlNode>& node, const std::string& path, ValidationResult& result) const {
    if (!node->isMapping()) {
        addError(result, "Expected mapping", path, "error");
        return;
    }
    
    const auto& mapping = node->asMapping();
    for (const auto& [key, value] : mapping) {
        std::string child_path = generatePath(path, key);
        validateNode(value, child_path, result);
    }
}

void YamlValidator::validateSequence(const std::shared_ptr<YamlNode>& node, const std::string& path, ValidationResult& result) const {
    if (!node->isSequence()) {
        addError(result, "Expected sequence", path, "error");
        return;
    }
    
    const auto& sequence = node->asSequence();
    for (size_t i = 0; i < sequence.size(); ++i) {
        std::string child_path = path + "[" + std::to_string(i) + "]";
        validateNode(sequence[i], child_path, result);
    }
}

void YamlValidator::validateMetadataSection(const std::shared_ptr<YamlNode>& metadata, ValidationResult& result) const {
    if (!metadata->hasKey("id")) {
        addError(result, "Missing required 'id' field in metadata", "/metadata", "error");
    }
    
    if (!metadata->hasKey("name")) {
        addError(result, "Missing required 'name' field in metadata", "/metadata", "error");
    }
    
    if (!metadata->hasKey("version")) {
        addError(result, "Missing required 'version' field in metadata", "/metadata", "error");
    }
}

void YamlValidator::validatePhilosophyLinks(const std::shared_ptr<YamlNode>& links, ValidationResult& result) const {
    if (!links->isSequence()) {
        addError(result, "Philosophy links must be a sequence", "/philosophy_links", "error");
        return;
    }
    
    const auto& sequence = links->asSequence();
    if (sequence.empty()) {
        addError(result, "Philosophy links cannot be empty", "/philosophy_links", "warning");
    }
    
    // Validate each link format
    std::regex link_pattern(R"(^akao:philosophy::[a-zA-Z_][a-zA-Z0-9_]*:[a-zA-Z_][a-zA-Z0-9_]*:v\d+$)");
    for (size_t i = 0; i < sequence.size(); ++i) {
        if (sequence[i]->isString()) {
            const std::string& link = sequence[i]->asString();
            if (!std::regex_match(link, link_pattern)) {
                addError(result, "Invalid philosophy link format: " + link, 
                        "/philosophy_links[" + std::to_string(i) + "]", "error");
            }
        } else {
            addError(result, "Philosophy link must be a string", 
                    "/philosophy_links[" + std::to_string(i) + "]", "error");
        }
    }
}

void YamlValidator::validateRuleCollection(const std::shared_ptr<YamlNode>& rules, ValidationResult& result) const {
    if (!rules->isSequence()) {
        addError(result, "Rule collection must be a sequence", "/rule_collection", "error");
        return;
    }
    
    const auto& sequence = rules->asSequence();
    if (sequence.empty()) {
        addError(result, "Rule collection cannot be empty", "/rule_collection", "warning");
    }
    
    // Validate each rule ID format
    std::regex rule_pattern(R"(^akao:rule::[a-zA-Z_][a-zA-Z0-9_]*:[a-zA-Z_][a-zA-Z0-9_]*:v\d+$)");
    for (size_t i = 0; i < sequence.size(); ++i) {
        if (sequence[i]->isString()) {
            const std::string& rule_id = sequence[i]->asString();
            if (!std::regex_match(rule_id, rule_pattern)) {
                addError(result, "Invalid rule ID format: " + rule_id, 
                        "/rule_collection[" + std::to_string(i) + "]", "error");
            }
        } else {
            addError(result, "Rule ID must be a string", 
                    "/rule_collection[" + std::to_string(i) + "]", "error");
        }
    }
}

void YamlValidator::applySchemaRules(const std::shared_ptr<YamlNode>& root, ValidationResult& result) const {
    for (const auto& rule : schema_rules_) {
        validateSchemaRule(rule, root, result);
    }
}

void YamlValidator::validateSchemaRule(const SchemaRule& rule, const std::shared_ptr<YamlNode>& root, ValidationResult& result) const {
    auto node = getNodeAtPath(root, rule.path);
    
    if (!node) {
        if (rule.is_required) {
            addError(result, "Required field '" + rule.path + "' is missing", rule.path, "error");
        }
        return;
    }
    
    // Validate type
    if (!rule.required_type.empty()) {
        bool type_matches = false;
        if (rule.required_type == "string" && node->isString()) type_matches = true;
        else if (rule.required_type == "integer" && node->isInteger()) type_matches = true;
        else if (rule.required_type == "float" && node->isFloat()) type_matches = true;
        else if (rule.required_type == "boolean" && node->isBoolean()) type_matches = true;
        else if (rule.required_type == "null" && node->isNull()) type_matches = true;
        else if (rule.required_type == "sequence" && node->isSequence()) type_matches = true;
        else if (rule.required_type == "mapping" && node->isMapping()) type_matches = true;
        
        if (!type_matches) {
            addError(result, "Field '" + rule.path + "' should be of type '" + rule.required_type + "'", 
                    rule.path, "error");
        }
    }
    
    // Validate allowed values
    if (!rule.allowed_values.empty() && node->isString()) {
        const std::string& value = node->asString();
        auto it = std::find(rule.allowed_values.begin(), rule.allowed_values.end(), value);
        if (it == rule.allowed_values.end()) {
            addError(result, "Field '" + rule.path + "' has invalid value '" + value + "'", 
                    rule.path, "error");
        }
    }
}

std::shared_ptr<YamlNode> YamlValidator::getNodeAtPath(const std::shared_ptr<YamlNode>& root, const std::string& path) const {
    if (path == "/" || path.empty()) {
        return root;
    }
    
    std::vector<std::string> parts;
    std::stringstream ss(path);
    std::string part;
    
    // Split path by '/'
    while (std::getline(ss, part, '/')) {
        if (!part.empty()) {
            parts.push_back(part);
        }
    }
    
    auto current = root;
    for (const auto& part : parts) {
        if (!current || !current->isMapping()) {
            return nullptr;
        }
        
        if (!current->hasKey(part)) {
            return nullptr;
        }
        
        current = (*current)[part];
    }
    
    return current;
}

std::string YamlValidator::generatePath(const std::string& base_path, const std::string& key) const {
    if (base_path == "/") {
        return "/" + key;
    }
    return base_path + "/" + key;
}

void YamlValidator::addError(ValidationResult& result, const std::string& message, const std::string& path, 
                           const std::string& severity) const {
    ValidationError error;
    error.message = message;
    error.path = path;
    error.line = 0;
    error.column = 0;
    error.severity = severity;
    
    if (severity == "error") {
        result.errors.push_back(error);
    } else if (severity == "warning") {
        result.warnings.push_back(error);
    }
}

} // namespace akao::core::engine::parser