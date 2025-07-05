#pragma once

#include "validation_result.hpp"
#include "../parser/yaml_parser.hpp"
#include "../../rule/registry/rule_registry.hpp"
#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <map>

namespace akao::core::engine::validator {

/**
 * @brief Universal Validator for Akao framework
 * 
 * THE CORE OF AKAO - Validates ANY project type
 * "Language of Languages - Framework of Frameworks"
 * 
 * Philosophy Compliance:
 * - akao:philosophy::structure:isolation:v1 (one class per file)
 * - akao:philosophy::validation:universal:v1 (universal validation)
 * - akao:philosophy::language:isolation:v1 (pure C++ core)
 * - akao:philosophy::rule:governance:v1 (rules govern rules)
 * - akao:philosophy::measurement:traceability:v1 (complete traceability)
 */
class UniversalValidator {
private:
    // Core components
    std::unique_ptr<rule::registry::RuleRegistry> rule_registry_;
    std::unique_ptr<parser::YamlParser> yaml_parser_;
    
    // Validation configuration
    struct ValidationConfig {
        bool enable_auto_fix = false;
        bool stop_on_first_error = false;
        bool include_skipped_rules = true;
        std::vector<std::string> enabled_categories;
        std::vector<std::string> enabled_severities;
        std::vector<std::string> excluded_rules;
        std::string output_format = "table";  // table, yaml, json
        int max_violations_per_rule = 100;
        bool parallel_execution = false;
    };
    
    ValidationConfig config_;
    
    // Project type detection
    enum class ProjectType {
        UNKNOWN,
        CPP_PROJECT,
        RUST_PROJECT,
        PYTHON_PROJECT,
        JAVASCRIPT_PROJECT,
        GO_PROJECT,
        MIXED_PROJECT,
        AKAO_PROJECT
    };
    
    // Rule execution context
    struct RuleExecutionContext {
        std::string target_path;
        ProjectType project_type;
        std::map<std::string, std::string> project_metadata;
        std::vector<std::string> discovered_files;
        std::map<std::string, std::string> file_types;
    };

public:
    // Constructor
    explicit UniversalValidator(const std::string& rules_directory = "rules");
    
    // Destructor
    ~UniversalValidator() = default;

    // Initialization
    bool initialize();
    bool isInitialized() const;
    void setRulesDirectory(const std::string& rules_directory);

    // Configuration
    void setConfig(const ValidationConfig& config);
    const ValidationConfig& getConfig() const;
    void enableAutoFix(bool enable);
    void setOutputFormat(const std::string& format);
    void enableCategory(const std::string& category);
    void disableCategory(const std::string& category);
    void excludeRule(const std::string& rule_id);

    // Main validation methods
    ValidationResult validate(const std::string& target_path);
    ValidationResult validateFile(const std::string& file_path);
    ValidationResult validateDirectory(const std::string& directory_path);
    ValidationResult validateProject(const std::string& project_path);
    
    // Self-validation (required by philosophy)
    ValidationResult validateSelf();
    bool isSelfCompliant();

    // Batch validation
    std::vector<ValidationResult> validateMultiple(const std::vector<std::string>& target_paths);
    ValidationResult validateAndMerge(const std::vector<std::string>& target_paths);

    // Rule-specific validation
    ValidationResult validateWithRule(const std::string& target_path, const std::string& rule_id);
    ValidationResult validateWithCategory(const std::string& target_path, const std::string& category);
    ValidationResult validateWithPhilosophy(const std::string& target_path, const std::string& philosophy_id);

    // Project type detection and analysis
    ProjectType detectProjectType(const std::string& path);
    std::string getProjectTypeString(ProjectType type) const;
    std::map<std::string, std::string> analyzeProject(const std::string& path);
    std::vector<std::string> discoverFiles(const std::string& path);

    // Rule management
    size_t getLoadedRulesCount() const;
    std::vector<std::string> getAvailableCategories() const;
    std::vector<std::string> getAvailableRules() const;
    bool reloadRules();

    // Validation statistics
    struct ValidationStats {
        size_t total_validations = 0;
        size_t successful_validations = 0;
        size_t failed_validations = 0;
        std::chrono::duration<double> total_execution_time{0};
        std::map<std::string, size_t> rule_execution_counts;
        std::map<std::string, size_t> category_violation_counts;
    };
    
    const ValidationStats& getStats() const;
    void resetStats();

    // Export and reporting
    std::string generateReport(const ValidationResult& result) const;
    std::string generateSummaryReport(const std::vector<ValidationResult>& results) const;
    bool exportReport(const ValidationResult& result, const std::string& file_path) const;

    // Error handling
    struct ValidationError {
        std::string message;
        std::string details;
        std::string target_path;
        std::string rule_id;
    };

    class ValidationException : public std::exception {
    private:
        ValidationError error_;
        std::string what_message_;

    public:
        explicit ValidationException(const ValidationError& error);
        const char* what() const noexcept override;
        const ValidationError& getError() const;
    };

private:
    // Core validation logic
    ValidationResult executeValidation(const RuleExecutionContext& context);
    std::vector<Violation> executeRule(const rule::loader::Rule& rule, const RuleExecutionContext& context);
    
    // Rule execution engines
    std::vector<Violation> executeStructureRule(const rule::loader::Rule& rule, const RuleExecutionContext& context);
    std::vector<Violation> executeInterfaceRule(const rule::loader::Rule& rule, const RuleExecutionContext& context);
    std::vector<Violation> executeLanguageRule(const rule::loader::Rule& rule, const RuleExecutionContext& context);
    std::vector<Violation> executeSecurityRule(const rule::loader::Rule& rule, const RuleExecutionContext& context);
    std::vector<Violation> executeTestingRule(const rule::loader::Rule& rule, const RuleExecutionContext& context);
    
    // Datalog/Prolog logic execution
    bool executeDatalogQuery(const std::string& query, const RuleExecutionContext& context);
    std::vector<std::string> findDatalogViolations(const std::string& query, const RuleExecutionContext& context);
    
    // Project analysis helpers
    RuleExecutionContext buildExecutionContext(const std::string& target_path);
    std::map<std::string, std::string> extractProjectMetadata(const std::string& path);
    std::string detectFileType(const std::string& file_path);
    bool isSourceFile(const std::string& file_path);
    bool isBuildFile(const std::string& file_path);
    bool isConfigFile(const std::string& file_path);
    
    // Violation generation
    Violation createViolation(const rule::loader::Rule& rule, const std::string& file_path, 
                            size_t line_number, const std::string& message, 
                            const std::string& suggestion = "");
    std::string generateViolationId(const rule::loader::Rule& rule, const std::string& file_path, 
                                   size_t line_number);
    
    // File analysis
    std::vector<std::string> readFileLines(const std::string& file_path);
    std::string getCodeContext(const std::string& file_path, size_t line_number, size_t context_lines = 3);
    
    // Rule filtering
    std::vector<std::shared_ptr<rule::loader::Rule>> getApplicableRules(const RuleExecutionContext& context);
    bool isRuleApplicable(const rule::loader::Rule& rule, const RuleExecutionContext& context);
    bool shouldSkipRule(const rule::loader::Rule& rule);
    
    // Performance optimization
    void optimizeRuleExecution(std::vector<std::shared_ptr<rule::loader::Rule>>& rules);
    void cacheRuleResults(const std::string& rule_id, const std::string& target_path, 
                         const std::vector<Violation>& violations);
    std::vector<Violation> getCachedRuleResults(const std::string& rule_id, const std::string& target_path);
    
    // Statistics tracking
    ValidationStats stats_;
    void updateStats(const ValidationResult& result);
    void trackRuleExecution(const std::string& rule_id);
    
    // Error handling
    void throwValidationError(const std::string& message, const std::string& details = "", 
                             const std::string& target_path = "", const std::string& rule_id = "");
    
    // Initialization state
    bool is_initialized_;
    std::string rules_directory_;
    
    // Caching
    std::map<std::string, std::vector<Violation>> rule_result_cache_;
    std::map<std::string, ProjectType> project_type_cache_;
    std::map<std::string, std::map<std::string, std::string>> project_metadata_cache_;
};

} // namespace akao::core::engine::validator
