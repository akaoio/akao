#pragma once

#include "../../engine/parser/yaml_parser.hpp"
#include "../../engine/parser/yaml_node.hpp"
#include <string>
#include <vector>
#include <memory>
#include <filesystem>

namespace akao::core::rule::loader {

/**
 * @brief Rule structure representing a loaded Akao rule
 * 
 * Philosophy Compliance:
 * - akao:philosophy::structure:isolation:v1 (one class per file)
 * - akao:philosophy::rule:governance:v1 (rules govern rules)
 * - akao:philosophy::measurement:traceability:v1 (full traceability)
 */
struct Rule {
    // Metadata
    std::string id;
    std::string name;
    std::string version;
    std::string category;
    std::string description;
    std::string file_path;
    
    // Philosophy links
    std::vector<std::string> philosophies;
    
    // Rule definition
    std::string scope;
    std::string target;
    std::vector<std::string> conditions;
    std::vector<std::string> exceptions;
    std::vector<std::string> datalog_rules;
    
    // Logic queries
    std::string check_compliance_query;
    std::string find_violations_query;
    std::string count_violations_query;
    
    // Implementation details
    std::string check_method;
    bool auto_fix;
    std::string severity;
    
    // Validation test cases
    struct TestCase {
        std::string name;
        std::string description;
        std::string expected;
    };
    std::vector<TestCase> test_cases;
    
    // Audit information
    std::vector<std::string> compliance_checks;
    std::string violation_severity;
    bool auto_fix_available;
};

/**
 * @brief Rule Loader for Akao framework
 * 
 * Loads all 32 rules from rules/ directory
 * Uses built-in YAML parser (no external dependencies)
 * 
 * Philosophy Compliance:
 * - akao:philosophy::structure:isolation:v1 (one class per file)
 * - akao:philosophy::language:isolation:v1 (pure C++ only)
 * - akao:philosophy::rule:governance:v1 (loads and validates rules)
 */
class RuleLoader {
private:
    engine::parser::YamlParser yaml_parser_;
    std::string rules_directory_;
    std::vector<std::shared_ptr<Rule>> loaded_rules_;

public:
    // Constructor
    explicit RuleLoader(const std::string& rules_directory = "rules");

    // Main loading methods
    bool loadAllRules();
    bool loadRule(const std::string& rule_file_path);
    bool loadRuleFromYaml(const std::string& yaml_content, const std::string& source_file = "");

    // Rule access
    const std::vector<std::shared_ptr<Rule>>& getRules() const;
    std::shared_ptr<Rule> getRuleById(const std::string& rule_id) const;
    std::vector<std::shared_ptr<Rule>> getRulesByCategory(const std::string& category) const;
    std::vector<std::shared_ptr<Rule>> getRulesByPhilosophy(const std::string& philosophy_id) const;

    // Rule validation
    bool validateRule(const Rule& rule) const;
    std::vector<std::string> validateAllRules() const;

    // Statistics
    size_t getTotalRulesCount() const;
    std::vector<std::string> getCategories() const;
    std::vector<std::string> getPhilosophies() const;

    // Rule discovery
    std::vector<std::string> discoverRuleFiles() const;
    bool isValidRuleFile(const std::string& file_path) const;

    // Error handling
    struct LoadError {
        std::string file_path;
        std::string message;
        std::string details;
    };

    class LoadException : public std::exception {
    private:
        LoadError error_;
        std::string what_message_;

    public:
        explicit LoadException(const LoadError& error);
        const char* what() const noexcept override;
        const LoadError& getError() const;
    };

private:
    // YAML parsing helpers
    std::shared_ptr<Rule> parseRuleFromYaml(std::shared_ptr<engine::parser::YamlNode> yaml_root, 
                                          const std::string& source_file);
    void parseMetadata(Rule& rule, std::shared_ptr<engine::parser::YamlNode> metadata_node);
    void parsePhilosophies(Rule& rule, std::shared_ptr<engine::parser::YamlNode> philosophies_node);
    void parseRuleDefinition(Rule& rule, std::shared_ptr<engine::parser::YamlNode> rule_def_node);
    void parseLogicQueries(Rule& rule, std::shared_ptr<engine::parser::YamlNode> logic_node);
    void parseImplementation(Rule& rule, std::shared_ptr<engine::parser::YamlNode> impl_node);
    void parseValidation(Rule& rule, std::shared_ptr<engine::parser::YamlNode> validation_node);
    void parseAudit(Rule& rule, std::shared_ptr<engine::parser::YamlNode> audit_node);

    // Validation helpers
    bool isValidRuleId(const std::string& rule_id) const;
    bool hasRequiredFields(const Rule& rule) const;
    bool hasValidPhilosophyReferences(const Rule& rule) const;

    // Error reporting
    void throwLoadError(const std::string& file_path, const std::string& message, 
                       const std::string& details = "");
};

} // namespace akao::core::rule::loader
