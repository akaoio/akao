#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <mutex>
#include <vector>

namespace akao::core::engine::runtime {

/**
 * @brief Dynamic Management System for Runtime Control
 * 
 * Provides runtime enable/disable capabilities for philosophies, rules, and rulesets
 * with lazy loading and user control interface. Implements the dynamic management
 * requirements from Phase 1 architecture preparation.
 */
class DynamicController {
public:
    DynamicController();
    ~DynamicController();

    // Component State Management
    bool enablePhilosophy(const std::string& philosophy_id);
    bool disablePhilosophy(const std::string& philosophy_id);
    bool isPhilosophyEnabled(const std::string& philosophy_id) const;
    
    bool enableRule(const std::string& rule_id);
    bool disableRule(const std::string& rule_id);
    bool isRuleEnabled(const std::string& rule_id) const;
    
    bool enableRuleset(const std::string& ruleset_id);
    bool disableRuleset(const std::string& ruleset_id);
    bool isRulesetEnabled(const std::string& ruleset_id) const;

    // Bulk Operations for Rulesets
    bool enableAllRulesInRuleset(const std::string& ruleset_id);
    bool disableAllRulesInRuleset(const std::string& ruleset_id);
    bool validateRulesetConsistency(const std::string& ruleset_id);

    // Dependency Validation
    bool validatePhilosophyDependencies(const std::string& rule_or_ruleset_id);
    bool canDisablePhilosophy(const std::string& philosophy_id) const;
    std::vector<std::string> getDependentRules(const std::string& philosophy_id) const;
    std::vector<std::string> getDependentRulesets(const std::string& philosophy_id) const;

    // Hot-reload Capabilities
    bool hotReloadComponent(const std::string& component_id);
    bool hotReloadRuleset(const std::string& ruleset_id);
    
    // User Control Interface
    void listActiveComponents() const;
    void showComponentStatus(const std::string& component_id) const;
    void showDependencyTree(const std::string& component_id) const;

private:
    mutable std::mutex control_mutex_;
    
    // Component state tracking
    std::unordered_set<std::string> enabled_philosophies_;
    std::unordered_set<std::string> enabled_rules_;
    std::unordered_set<std::string> enabled_rulesets_;
    
    // Dependency mapping  
    std::unordered_map<std::string, std::unordered_set<std::string>> philosophy_dependencies_; // rule/ruleset -> philosophies
    std::unordered_map<std::string, std::unordered_set<std::string>> ruleset_rules_; // ruleset -> rules
    
    // Internal helpers
    void loadDependencyMappings();
    bool validateDependenciesInternal(const std::string& component_id, const std::unordered_set<std::string>& required_philosophies);
    void cascadeDisable(const std::string& philosophy_id);
};

} // namespace akao::core::engine::runtime