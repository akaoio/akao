/**
 * @id: akao:file:core:engine:validator:universal:v1_hpp
 * @doc: Universal validator header defining comprehensive validation framework interface for any codebase including Akao with philosophy-rule separation, Pure Logic engine integration, and archaeological preservation capabilities
 * @specification: Header file for UniversalValidator class with complete interface definition, configuration structures, and validation framework declarations
 * @scope: Core engine universal validation header for system-wide compliance and quality assurance interface
 * @timeline: 2025-01-08
 * @rationale: Provide universal validation interface that operates on any codebase while maintaining philosophy-rule separation and archaeological preservation
 * @methodology: Universal validation architecture interface with Pure Logic engine integration, project type detection, and comprehensive rule execution framework
 * @references: ["akao:file:core:engine:validator:universal:v1", "akao:class:core:engine:universal_validator:v1", "akao:philosophy:validation:universal:v1"]
 */

#pragma once

#include "../validation_result/v1.hpp"
#include "../../parser/yaml/parser/v1.hpp"
#include "../../../rule/registry/rule-registry/v1.hpp"
#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <map>
#include <regex>

// Forward declarations for Pure Logic Engine
namespace akao::logic {
    class PureLogicEngine;
    class Context;
    class Value;
}

namespace akao::core::engine::validator {

/**
 * @id: akao:class:core:engine:universal_validator:v1
 * @doc: Universal Validator class for Akao framework providing comprehensive validation capability for any project type with philosophy-rule separation, Pure Logic engine integration, and archaeological preservation
 * @specification: Core universal validation class with project type detection, rule execution, Pure Logic integration, and self-validation capabilities
 * @scope: Core engine universal validation class for system-wide compliance and quality assurance
 * @timeline: 2025-01-08
 * @rationale: Provide universal validation capability that operates on any codebase while maintaining philosophy-rule separation and archaeological preservation
 * @methodology: Universal validation architecture with Pure Logic engine integration, project type detection, and comprehensive rule execution framework
 * @references: ["akao:file:core:engine:validator:universal:v1", "akao:philosophy:validation:universal:v1", "akao:philosophy:structure:isolation:v1"]
 */
class UniversalValidator {
private:
    // Core components
    std::unique_ptr<rule::registry::RuleRegistry> rule_registry_;
    std::unique_ptr<parser::YamlParser> yaml_parser_;
    
    // Pure Logic Engine for mathematical formal proofs
    mutable std::unique_ptr<akao::logic::PureLogicEngine> pure_logic_engine_;
    
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
    ~UniversalValidator();

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
    
    // Pure Logic Engine execution with mathematical formal proofs
    std::vector<std::string> findPureLogicViolations(const std::string& logic_expression, const RuleExecutionContext& context);
    std::string convertDatalogToPureLogic(const std::string& datalog_rule);
    
    // Datalog/Prolog logic execution (deprecated - use Pure Logic Engine)
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
