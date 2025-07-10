/**
 * @id: akao:file:core:engine:validator:v1
 * @doc: Unified validation system combining universal validation, compliance checking, rule-driven validation, and metadata standards validation into a single cohesive framework with .akao/rules lazy loading support.
 * @specification: Consolidated validation system with .akao/rules workflow, lazy loading, and rule-driven architecture
 * @scope: Core engine unified validation system
 * @timeline: 2025-07-09
 * @rationale: Eliminate architectural duplication by combining all validation concerns into a single unified system with zero hardcoded logic
 * @methodology: Unified validation architecture with rule-driven execution, lazy loading, and .akao/rules workflow
 * @references: ["akao:class:core:engine:validator:unified_validator:v1"]
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <regex>
#include <optional>
#include <chrono>
#include <filesystem>
#include "../logic/runtime/v1.hpp"
#include "../runtime/lazy_loader/v1.hpp"

namespace akao::core::engine::validator {

/**
 * @brief Settings configuration for validation framework
 */
struct ValidationSettings {
    // File Discovery Settings
    bool respect_gitignore = true;
    bool include_hidden_files = false;
    bool follow_symlinks = false;
    
    // Validation Behavior
    bool fail_fast = false;
    int max_violations_per_rule = 0;
    
    // Performance Settings
    bool parallel_execution = true;
    int max_concurrent_files = 0;
};

/**
 * @brief Comprehensive validation violation record (merged from all validation systems)
 */
struct Violation {
    std::string id;
    std::string rule_id;
    std::string rule_name;
    std::string philosophy_id;
    std::string description;
    std::string message;
    std::string file_path;
    int line_number = 0;
    int column_number = 0;
    std::string severity = "error";
    std::string rule_category;
    std::string suggestion;
    bool auto_fix_available = false;
    std::chrono::system_clock::time_point detected_at = std::chrono::system_clock::now();
    
    // Merged from trace/tracer/
    std::string trace_id;
    std::string violation_id;
    std::string project_path;
    std::vector<std::string> call_stack;
    std::vector<std::string> rule_chain;
    std::map<std::string, std::string> context_variables;
    std::chrono::system_clock::time_point traced_at;
    std::string violation_category;
    std::string root_cause;
    std::vector<std::string> related_violations;
    std::vector<std::string> fix_commands;
    std::map<std::string, std::string> metadata;
    
    // Merged from philosophy/engine/
    std::string formal_proof;
    std::vector<std::string> failed_theorems;
};

/**
 * @brief Validation result
 */
struct ValidationResult {
    bool is_valid = true;
    std::vector<Violation> violations;
    std::string summary;
    std::string target_path;
    std::string validation_type;
    size_t total_rules_executed = 0;
    size_t total_files_analyzed = 0;
    std::chrono::duration<double> execution_duration{0};
    
    // Constructors
    ValidationResult() = default;
    ValidationResult(const std::string& path) : target_path(path) {}
    ValidationResult(const std::string& path, const std::string& type) 
        : target_path(path), validation_type(type) {}
    
    void addViolation(const Violation& violation) {
        violations.push_back(violation);
        is_valid = false;
    }
    
    bool hasViolations() const { return !violations.empty(); }
    size_t getViolationCount() const { return violations.size(); }
    bool isCompliant() const { return is_valid; }
    
    void addViolations(const std::vector<Violation>& new_violations) {
        for (const auto& violation : new_violations) {
            addViolation(violation);
        }
    }
    
    void setExecutionDuration(const std::chrono::duration<double>& duration) {
        execution_duration = duration;
    }
    
    std::chrono::duration<double> getExecutionDuration() const {
        return execution_duration;
    }
};

/**
 * @brief Rule configuration for .akao/rules system
 */
struct RuleConfig {
    std::string rule_id;
    std::string name;
    std::string description;
    std::string category;
    std::string format = "yaml"; // yaml or a
    bool enabled = true;
    std::string severity = "error";
    std::vector<std::string> applies_to;
    std::vector<std::string> phases;  // New: validation phases this rule participates in
    std::map<std::string, std::string> parameters;
    std::string file_path;
    std::chrono::system_clock::time_point last_modified;
};

/**
 * @id: akao:class:core:engine:validator:unified_validator:v1
 * @doc: Unified validator combining universal validation, compliance checking, and metadata standards validation
 */
class UnifiedValidator {
public:
    // Configuration for different validation modes
    struct ValidationConfig {
        bool enable_universal_validation;
        bool enable_compliance_checking;
        bool enable_metadata_validation;
        bool enable_namespace_validation;
        std::string rules_directory;
        std::vector<std::string> file_patterns;
        bool enable_auto_fix;
        bool parallel_execution;
        
        // Constructor with defaults
        ValidationConfig() :
            enable_universal_validation(true),
            enable_compliance_checking(true),
            enable_metadata_validation(true),
            enable_namespace_validation(true),
            rules_directory("rules"),
            file_patterns({"*.cpp", "*.hpp", "*.yaml"}),
            enable_auto_fix(false),
            parallel_execution(false)
        {}
    };

    explicit UnifiedValidator(const ValidationConfig& config = ValidationConfig());
    ~UnifiedValidator() = default;

    // Main validation interface
    ValidationResult validate(const std::string& target_path);
    ValidationResult validateFile(const std::string& file_path);
    ValidationResult validateDirectory(const std::string& dir_path);
    
    // Phase-aware validation pipeline
    ValidationResult validatePhased(const std::string& target_path);
    
    
    std::vector<Violation> executePhaseValidationForFile(const std::string& phase, const std::string& target_path, const std::string& file);
    std::vector<std::string> executePhase1WithRealTimeFiltering(const std::string& phase, const std::string& target_path, const std::vector<std::string>& files);
    bool isReadableFile(const std::string& file_path);
    bool matchesIgnorePattern(const std::string& file_path, const std::string& pattern, const std::string& base_path);
    bool matchesSimplePattern(const std::string& file_path, const std::string& pattern);

    // Specific validation modes
    ValidationResult validateUniversal(const std::string& target_path);
    ValidationResult validateCompliance(const std::string& target_path);
    ValidationResult validateMetadata(const std::string& target_path);
    ValidationResult validateNamespace(const std::string& target_path);
    
    // Consolidated philosophy validation (from core/philosophy/)
    ValidationResult validatePhilosophy(const std::string& philosophy_file);
    std::string generateFormalProof(const std::string& philosophy_file);
    
    // Consolidated rule execution (from core/rule/)
    std::vector<Violation> executeRules(const std::string& target_path);
    ValidationResult executeRule(const std::string& rule_id, const std::string& target_path);
    
    // Consolidated violation tracing (from core/trace/)
    std::string traceViolation(const Violation& violation);
    ValidationResult traceViolations(const std::string& target_path);
    std::string generateTraceReport(const std::vector<Violation>& violations);
    
    // Consolidated logging (from core/trace/logger/)
    void logViolation(const std::string& rule_id, const std::string& file_path, const std::string& details);
    void logValidation(const std::string& target_path, bool success, int violation_count);
    bool exportLogs(const std::string& output_path, const std::string& format = "text");
    
    // Consolidated reporting (from core/trace/reporter/)
    std::string generateReport(const std::vector<Violation>& violations, const std::string& format = "markdown");
    bool exportReport(const std::string& report_content, const std::string& output_path);

    // Rule-driven validation (zero hardcoded logic)
    ValidationResult validateWithRules(const std::string& target_path, 
                                     const std::vector<std::string>& rule_ids = {});
    std::vector<RuleConfig> getAvailableRules() const;
    std::vector<RuleConfig> getEnabledRules() const;
    std::vector<RuleConfig> getRulesByCategory(const std::string& category) const;
    bool enableRule(const std::string& rule_id);
    bool disableRule(const std::string& rule_id);
    bool isRuleEnabled(const std::string& rule_id) const;
    
    // .akao/rules management
    bool initializeRulesDirectory();
    bool loadRulesFromDirectory(const std::string& rules_dir = ".akao/rules");
    
    // Statistics
    struct Statistics {
        size_t total_validations = 0;
        size_t successful_validations = 0;
        size_t failed_validations = 0;
        size_t rules_executed = 0;
        size_t violations_found = 0;
        std::chrono::duration<double> total_execution_time{0};
        std::map<std::string, size_t> category_validation_count;
    };
    
    const Statistics& getStatistics() const;
    void resetStatistics();
    
    // Configuration
    void setConfig(const ValidationConfig& config);
    const ValidationConfig& getConfig() const;

    // Utility methods
    bool isInitialized() const;
    std::vector<std::string> getSupportedFileTypes() const;
    
    // Namespace parsing
    struct ParsedNamespace {
        std::string type;
        std::string domain;
        std::string component;
        std::string name;
        std::string version;
        bool is_valid = false;
    };
    
    ParsedNamespace parseNamespace(const std::string& namespace_id);
    bool validateAkaoNamespace(const std::string& namespace_id);
    
private:
    ValidationConfig config_;
    bool is_initialized_ = false;
    
    // Rule management
    std::vector<RuleConfig> available_rules_;
    std::vector<RuleConfig> enabled_rules_;
    std::unique_ptr<akao::logic::PureLogicEngine> logic_engine_;
    std::unique_ptr<akao::core::engine::runtime::LazyLoader> lazy_loader_;
    
    // Statistics
    Statistics statistics_;
    
    // Consolidated trace functionality
    std::vector<std::string> log_entries_;
    std::map<std::string, std::string> trace_context_;
    size_t trace_counter_ = 0;

    // Internal validation methods
    ValidationResult performUniversalValidation(const std::string& target_path);
    ValidationResult performComplianceValidation(const std::string& target_path);
    ValidationResult performMetadataValidation(const std::string& target_path);
    ValidationResult performNamespaceValidation(const std::string& target_path);

    // Helper methods
    std::vector<std::string> discoverFiles(const std::string& path);
    std::vector<std::string> discoverCurrentDirectoryFiles(const std::string& path);
    std::vector<std::string> discoverDotfilesInWorkingDirectory(const std::string& path);
    std::vector<std::string> discoverNonDotfilesRecursively(const std::string& path, const std::vector<std::string>& ignore_patterns);
    std::vector<std::string> processGitignoreFile(const std::string& gitignore_path);
    ValidationResult mergeResults(const std::vector<ValidationResult>& results);
    Violation createViolation(const std::string& rule_id, const std::string& message, 
                             const std::string& file_path, int line = 0);
    
    // Metadata validation helpers
    bool isValidNamespaceFormat(const std::string& namespace_id);
    std::vector<std::string> getRequiredMetadataFields();
    std::vector<Violation> checkMissingMetadata(const std::string& file_path);
    
    // Compliance validation helpers
    std::vector<Violation> checkNamingConventions(const std::string& file_path);
    std::vector<Violation> checkFileStructure(const std::string& file_path);
    std::vector<Violation> checkDirectoryStructure(const std::string& dir_path);
    
    // Universal validation helpers
    std::vector<Violation> executeRulesInternal(const std::string& target_path);
    bool isRuleApplicable(const std::string& rule_id, const std::string& file_path);
    
    // Merged from philosophy/engine/
    std::vector<Violation> performPhilosophyValidation(const std::string& target_path);
    std::string generateFormalProofInternal(const std::string& philosophy_file);
    
    // Merged from rule/executor/
    std::vector<Violation> performRuleExecution(const std::string& target_path);
    std::vector<Violation> executeRuleInternal(const std::string& rule_id, const std::string& target_path);
    
    // Merged from trace/tracer/
    std::vector<Violation> performViolationTracing(const std::string& target_path);
    std::string generateTraceReportInternal(const std::vector<Violation>& violations);
    Violation createTraceViolation(const std::string& rule_id, const std::string& message, 
                                  const std::string& file_path, int line = 0);
    
    // Rule-driven validation internals
    ValidationResult performRuleDrivenValidation(const std::string& target_path);
    ValidationResult executeRuleConfig(const RuleConfig& rule, const std::string& target_path);
    std::vector<RuleConfig> discoverApplicableRules(const std::string& target_path);
    bool isRuleApplicableToTarget(const RuleConfig& rule, const std::string& target_path);
    RuleConfig parseRuleFile(const std::string& file_path);
    void scanRulesDirectory(const std::string& rules_dir);
    bool loadRuleConfiguration(const std::string& config_path);
    
    // Consolidated functionality from deprecated components
    ValidationResult executeAdvancedRules(const std::string& target_path, const std::vector<std::string>& rule_patterns);
    ValidationResult executeAdvancedRule(const RuleConfig& rule, const std::string& target_path);
    
    // Category-specific rule execution (consolidated from core/rule/)
    ValidationResult executeStructureRule(const RuleConfig& rule, const std::string& target_path);
    ValidationResult executeInterfaceRule(const RuleConfig& rule, const std::string& target_path);
    ValidationResult executeLanguageRule(const RuleConfig& rule, const std::string& target_path);
    ValidationResult executeSecurityRule(const RuleConfig& rule, const std::string& target_path);
    ValidationResult executeTestingRule(const RuleConfig& rule, const std::string& target_path);
    ValidationResult executeGenericRule(const RuleConfig& rule, const std::string& target_path);
    
    // Helper methods for consolidated functionality
    std::vector<RuleConfig> findRulesByPattern(const std::string& pattern);
    std::vector<RuleConfig> getRulesByPhase(const std::string& phase);
    ValidationResult executeRuleForFile(const RuleConfig& rule, const std::string& target_path, const std::string& file);
    bool initializeRuleSystem();
    
    // Log export functionality
    std::string generateValidationLogContent(const ValidationResult& result, const std::string& target_path);
    void exportValidationLogs(const std::string& log_content, const std::string& target_path);
    
    // Consolidated trace helpers
    std::string generateTraceId(const Violation& violation);
    std::string formatLogEntry(const std::string& level, const std::string& message);
    std::string getTimestamp() const;
    std::string formatViolationForReport(const Violation& violation, const std::string& format);
    std::string generateSummarySection(const std::vector<Violation>& violations);
    std::string generateDetailsSection(const std::vector<Violation>& violations);
    std::string generateStatisticsSection(const std::vector<Violation>& violations);
};

/**
 * @brief Factory for creating rule-driven validators
 */
class UnifiedValidatorFactory {
public:
    static std::unique_ptr<UnifiedValidator> create(const std::string& project_root = ".");
    static std::unique_ptr<UnifiedValidator> createWithConfig(
        const std::string& project_root, 
        const UnifiedValidator::ValidationConfig& config);
    static std::unique_ptr<UnifiedValidator> createForDevelopment(const std::string& project_root = ".");
    static std::unique_ptr<UnifiedValidator> createForCI(const std::string& project_root = ".");
    static std::unique_ptr<UnifiedValidator> createForProduction(const std::string& project_root = ".");
    
    static bool initializeRulesDirectory(const std::string& project_root = ".");
    static bool validateRulesDirectory(const std::string& project_root = ".");
};

} // namespace akao::core::engine::validator