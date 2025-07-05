#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>

namespace akao::core::engine::validator {

/**
 * @brief Violation information for Akao validation
 * 
 * Complete traceability as required by philosophy
 * akao:philosophy::measurement:traceability:v1
 */
struct Violation {
    // Unique violation identifier
    std::string id;  // Format: akao:rule::category:rule_name:v1:violation:file.cpp:42
    
    // Rule information
    std::string rule_id;
    std::string rule_name;
    std::string rule_category;
    std::string philosophy_id;
    
    // Location information
    std::string file_path;
    size_t line_number;
    size_t column_number;
    
    // Violation details
    std::string message;
    std::string description;
    std::string suggestion;
    std::string severity;  // LOW, MEDIUM, HIGH, CRITICAL
    
    // Fix information
    bool auto_fix_available;
    std::string fix_description;
    std::string fix_command;
    
    // Context information
    std::string code_context;  // Surrounding code lines
    std::vector<std::string> stack_trace;
    std::map<std::string, std::string> metadata;
    
    // Timestamps
    std::chrono::system_clock::time_point detected_at;
    
    // Helper methods
    std::string toString() const;
    std::string toYaml() const;
    std::string toJson() const;
};

/**
 * @brief Validation Result for Akao framework
 * 
 * Comprehensive validation results with complete traceability
 * 
 * Philosophy Compliance:
 * - akao:philosophy::structure:isolation:v1 (one class per file)
 * - akao:philosophy::measurement:traceability:v1 (complete traceability)
 * - akao:philosophy::measurement:observability:v1 (observable results)
 * - akao:philosophy::validation:universal:v1 (universal validation)
 */
class ValidationResult {
private:
    // Basic information
    std::string target_path_;
    std::string target_type_;  // project, file, directory
    std::chrono::system_clock::time_point validation_time_;
    std::chrono::duration<double> execution_duration_;
    
    // Rule execution tracking
    size_t total_rules_executed_;
    size_t rules_passed_;
    size_t rules_failed_;
    size_t rules_skipped_;
    
    // Violations
    std::vector<Violation> violations_;
    std::map<std::string, std::vector<Violation>> violations_by_category_;
    std::map<std::string, std::vector<Violation>> violations_by_severity_;
    std::map<std::string, std::vector<Violation>> violations_by_file_;
    
    // Statistics
    std::map<std::string, size_t> rule_execution_stats_;
    std::map<std::string, std::chrono::duration<double>> rule_timing_stats_;
    size_t files_processed_;
    
    // Execution metadata
    double execution_time_;  // For compatibility with CLI expectations
    
    // Compliance metrics
    double overall_compliance_score_;
    std::map<std::string, double> category_compliance_scores_;
    std::map<std::string, double> philosophy_compliance_scores_;

public:
    // Constructor
    explicit ValidationResult(const std::string& target_path, const std::string& target_type = "project");
    
    // Destructor
    ~ValidationResult() = default;

    // Basic information
    const std::string& getTargetPath() const;
    const std::string& getTargetType() const;
    std::chrono::system_clock::time_point getValidationTime() const;
    std::chrono::duration<double> getExecutionDuration() const;
    
    void setExecutionDuration(std::chrono::duration<double> duration);

    // Rule execution tracking
    void setTotalRulesExecuted(size_t total);
    void setRulesPassed(size_t passed);
    void setRulesFailed(size_t failed);
    void setRulesSkipped(size_t skipped);
    
    size_t getTotalRulesExecuted() const;
    size_t getRulesPassed() const;
    size_t getRulesFailed() const;
    size_t getRulesSkipped() const;

    // Violation management
    void addViolation(const Violation& violation);
    void addViolations(const std::vector<Violation>& violations);
    void removeViolation(const std::string& violation_id);
    void clearViolations();

    // Violation access
    const std::vector<Violation>& getAllViolations() const;
    const std::vector<Violation>& getViolations() const;
    std::vector<Violation>& getViolations();
    const std::vector<Violation>& getViolationsByCategory(const std::string& category) const;
    const std::vector<Violation>& getViolationsBySeverity(const std::string& severity) const;
    const std::vector<Violation>& getViolationsByFile(const std::string& file_path) const;
    std::vector<Violation> getViolationsByRule(const std::string& rule_id) const;
    
    Violation* getViolationById(const std::string& violation_id);
    const Violation* getViolationById(const std::string& violation_id) const;

    // Violation statistics
    size_t getTotalViolationsCount() const;
    size_t getViolationsCountByCategory(const std::string& category) const;
    size_t getViolationsCountBySeverity(const std::string& severity) const;
    size_t getViolationsCountByFile(const std::string& file_path) const;
    
    std::vector<std::string> getAffectedFiles() const;
    std::vector<std::string> getViolatedCategories() const;
    std::vector<std::string> getViolatedSeverities() const;

    // Compliance scoring
    double getOverallComplianceScore() const;
    double getCategoryComplianceScore(const std::string& category) const;
    double getPhilosophyComplianceScore(const std::string& philosophy_id) const;
    
    void calculateComplianceScores();
    void setComplianceScore(double score);
    void setCategoryComplianceScore(const std::string& category, double score);
    void setPhilosophyComplianceScore(const std::string& philosophy_id, double score);

    // Success/failure determination
    bool isValid() const;  // No violations
    bool isCompliant() const;  // Compliance score above threshold
    bool hasViolations() const;
    bool hasCriticalViolations() const;
    bool hasAutoFixableViolations() const;

    // Rule timing and statistics
    void setRuleExecutionStat(const std::string& rule_id, size_t execution_count);
    void setRuleTimingStat(const std::string& rule_id, std::chrono::duration<double> execution_time);
    
    size_t getRuleExecutionStat(const std::string& rule_id) const;
    std::chrono::duration<double> getRuleTimingStat(const std::string& rule_id) const;
    
    const std::map<std::string, size_t>& getAllRuleExecutionStats() const;
    const std::map<std::string, std::chrono::duration<double>>& getAllRuleTimingStats() const;

    // Export and serialization
    std::string toYaml() const;
    std::string toJson() const;
    std::string toTable() const;  // Human-readable table format
    std::string toSummary() const;  // Brief summary
    
    bool exportToFile(const std::string& file_path, const std::string& format = "yaml") const;

    // Filtering and sorting
    ValidationResult filterByCategory(const std::string& category) const;
    ValidationResult filterBySeverity(const std::string& severity) const;
    ValidationResult filterByFile(const std::string& file_path) const;
    
    void sortViolationsByFile();
    void sortViolationsBySeverity();
    void sortViolationsByCategory();

    // Merging results (for parallel validation)
    void mergeWith(const ValidationResult& other);
    static ValidationResult merge(const std::vector<ValidationResult>& results);

    // Debug and diagnostics
    std::string getDebugInfo() const;
    void printSummary() const;
    void printDetailedReport() const;

    // File processing stats
    size_t getFilesProcessed() const;
    void setFilesProcessed(size_t count);
    
    // Execution time compatibility
    double getExecutionTime() const;
    void setExecutionTime(double time);

private:
    // Index building and maintenance
    void buildViolationIndexes();
    void updateViolationIndexes(const Violation& violation);
    void removeFromIndexes(const std::string& violation_id);
    
    // Utility methods
    std::string formatDuration(std::chrono::duration<double> duration) const;
    std::string formatTimestamp(std::chrono::system_clock::time_point timestamp) const;
    std::string severityToString(const std::string& severity) const;
    int severityToNumber(const std::string& severity) const;
};

} // namespace akao::core::engine::validator
