#pragma once

#include "../../loader/rule-loader/v1.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <set>
#include <any>
#include <functional>

namespace akao::core::rule::registry {

/**
 * @brief Rule Registry for Akao framework
 * 
 * Manages all loaded rules with unique IDs
 * Provides fast lookup and categorization
 * 
 * Philosophy Compliance:
 * - akao:philosophy::structure:isolation:v1 (one class per file)
 * - akao:philosophy::rule:governance:v1 (rule management)
 * - akao:philosophy::measurement:traceability:v1 (rule traceability)
 */
class RuleRegistry {
private:
    // Core storage
    std::map<std::string, std::shared_ptr<loader::Rule>> rules_by_id_;
    std::map<std::string, std::vector<std::shared_ptr<loader::Rule>>> rules_by_category_;
    std::map<std::string, std::vector<std::shared_ptr<loader::Rule>>> rules_by_philosophy_;
    
    // Rule loader
    std::unique_ptr<loader::RuleLoader> rule_loader_;
    
    // Registry state
    bool is_loaded_;
    std::string rules_directory_;
    size_t last_load_timestamp_;

public:
    // Constructor
    explicit RuleRegistry(const std::string& rules_directory = "rules");
    
    // Destructor
    ~RuleRegistry() = default;

    // Registry management
    bool loadRules();
    bool reloadRules();
    void clearRegistry();
    bool isLoaded() const;

    // Rule registration (for programmatic rule addition)
    bool registerRule(std::shared_ptr<loader::Rule> rule);
    bool unregisterRule(const std::string& rule_id);

    // Rule lookup
    std::shared_ptr<loader::Rule> getRuleById(const std::string& rule_id) const;
    std::vector<std::shared_ptr<loader::Rule>> getAllRules() const;
    std::vector<std::shared_ptr<loader::Rule>> getRulesByCategory(const std::string& category) const;
    std::vector<std::shared_ptr<loader::Rule>> getRulesByPhilosophy(const std::string& philosophy_id) const;
    std::vector<std::shared_ptr<loader::Rule>> getRulesByScope(const std::string& scope) const;
    std::vector<std::shared_ptr<loader::Rule>> getRulesBySeverity(const std::string& severity) const;

    // Rule queries
    bool hasRule(const std::string& rule_id) const;
    bool hasCategory(const std::string& category) const;
    bool hasPhilosophy(const std::string& philosophy_id) const;

    // Rule filtering
    std::vector<std::shared_ptr<loader::Rule>> filterRules(
        const std::function<bool(const loader::Rule&)>& predicate) const;
    std::vector<std::shared_ptr<loader::Rule>> getRulesWithAutoFix() const;
    std::vector<std::shared_ptr<loader::Rule>> getActiveRules() const;

    // Statistics and information
    size_t getTotalRulesCount() const;
    size_t getRulesCountByCategory(const std::string& category) const;
    size_t getRulesCountByPhilosophy(const std::string& philosophy_id) const;
    
    std::vector<std::string> getAllCategories() const;
    std::vector<std::string> getAllPhilosophies() const;
    std::vector<std::string> getAllScopes() const;
    std::vector<std::string> getAllSeverities() const;

    // Rule validation
    bool validateRegistry() const;
    std::vector<std::string> getValidationErrors() const;
    bool checkRuleConsistency() const;

    // Rule dependency management
    std::vector<std::string> getRuleDependencies(const std::string& rule_id) const;
    std::vector<std::string> getRuleDependents(const std::string& rule_id) const;
    bool hasCircularDependencies() const;

    // Export and serialization
    std::string exportToYaml() const;
    std::string exportToJson() const;
    bool exportToFile(const std::string& file_path, const std::string& format = "yaml") const;

    // Rule execution order
    std::vector<std::shared_ptr<loader::Rule>> getRulesInExecutionOrder() const;
    std::vector<std::shared_ptr<loader::Rule>> getRulesForTarget(const std::string& target_path) const;

    // Event handling for rule changes
    using RuleChangeCallback = std::function<void(const std::string& rule_id, const std::string& change_type)>;
    void setRuleChangeCallback(RuleChangeCallback callback);

    // Debug and diagnostics
    std::string getRegistryStatus() const;
    void printRegistryStats() const;
    std::map<std::string, std::any> getRegistryMetrics() const;

private:
    // Internal management
    void buildIndexes();
    void updateCategoryIndex(std::shared_ptr<loader::Rule> rule);
    void updatePhilosophyIndex(std::shared_ptr<loader::Rule> rule);
    void removeCategoryIndex(const std::string& rule_id);
    void removePhilosophyIndex(const std::string& rule_id);

    // Validation helpers
    bool isValidRuleForRegistration(const loader::Rule& rule) const;
    bool checkRuleIdUniqueness(const std::string& rule_id) const;
    bool validateRuleReferences(const loader::Rule& rule) const;

    // Utility methods
    std::vector<std::string> extractUniqueValues(
        const std::function<std::string(const loader::Rule&)>& extractor) const;
    
    // Event handling
    RuleChangeCallback rule_change_callback_;
    void notifyRuleChange(const std::string& rule_id, const std::string& change_type);
};

} // namespace akao::core::rule::registry
