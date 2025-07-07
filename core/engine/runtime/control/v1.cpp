#include "v1.hpp"
#include <iostream>
#include <algorithm>
#include <vector>

namespace akao::core::engine::runtime {

DynamicController::DynamicController() {
    // Initialize with default enabled components
    loadDependencyMappings();
    
    // Enable core components by default
    enablePhilosophy("akao:philosophy::structure:enforcement:v1");
    enablePhilosophy("akao:philosophy::structure:isolation:v1");
    enableRuleset("akao:ruleset:structure:cpp_standards:v1");
}

DynamicController::~DynamicController() = default;

// Philosophy Management
bool DynamicController::enablePhilosophy(const std::string& philosophy_id) {
    std::lock_guard<std::mutex> lock(control_mutex_);
    enabled_philosophies_.insert(philosophy_id);
    return true;
}

bool DynamicController::disablePhilosophy(const std::string& philosophy_id) {
    std::lock_guard<std::mutex> lock(control_mutex_);
    
    // Check if any rules or rulesets depend on this philosophy
    if (!canDisablePhilosophy(philosophy_id)) {
        std::cerr << "Cannot disable philosophy " << philosophy_id 
                  << ": dependent rules/rulesets must be disabled first" << std::endl;
        return false;
    }
    
    enabled_philosophies_.erase(philosophy_id);
    cascadeDisable(philosophy_id);
    return true;
}

bool DynamicController::isPhilosophyEnabled(const std::string& philosophy_id) const {
    std::lock_guard<std::mutex> lock(control_mutex_);
    return enabled_philosophies_.count(philosophy_id) > 0;
}

// Rule Management
bool DynamicController::enableRule(const std::string& rule_id) {
    std::lock_guard<std::mutex> lock(control_mutex_);
    
    // Validate philosophy dependencies
    if (!validateDependenciesInternal(rule_id, philosophy_dependencies_[rule_id])) {
        std::cerr << "Cannot enable rule " << rule_id 
                  << ": required philosophies not enabled" << std::endl;
        return false;
    }
    
    enabled_rules_.insert(rule_id);
    return true;
}

bool DynamicController::disableRule(const std::string& rule_id) {
    std::lock_guard<std::mutex> lock(control_mutex_);
    enabled_rules_.erase(rule_id);
    return true;
}

bool DynamicController::isRuleEnabled(const std::string& rule_id) const {
    std::lock_guard<std::mutex> lock(control_mutex_);
    return enabled_rules_.count(rule_id) > 0;
}

// Ruleset Management
bool DynamicController::enableRuleset(const std::string& ruleset_id) {
    std::lock_guard<std::mutex> lock(control_mutex_);
    
    // Validate philosophy dependencies for the ruleset
    if (!validateDependenciesInternal(ruleset_id, philosophy_dependencies_[ruleset_id])) {
        std::cerr << "Cannot enable ruleset " << ruleset_id 
                  << ": required philosophies not enabled" << std::endl;
        return false;
    }
    
    enabled_rulesets_.insert(ruleset_id);
    return true;
}

bool DynamicController::disableRuleset(const std::string& ruleset_id) {
    std::lock_guard<std::mutex> lock(control_mutex_);
    enabled_rulesets_.erase(ruleset_id);
    
    // Also disable all rules in this ruleset
    for (const auto& rule_id : ruleset_rules_[ruleset_id]) {
        enabled_rules_.erase(rule_id);
    }
    
    return true;
}

bool DynamicController::isRulesetEnabled(const std::string& ruleset_id) const {
    std::lock_guard<std::mutex> lock(control_mutex_);
    return enabled_rulesets_.count(ruleset_id) > 0;
}

// Bulk Operations
bool DynamicController::enableAllRulesInRuleset(const std::string& ruleset_id) {
    if (!isRulesetEnabled(ruleset_id)) {
        std::cerr << "Ruleset " << ruleset_id << " is not enabled" << std::endl;
        return false;
    }
    
    std::lock_guard<std::mutex> lock(control_mutex_);
    bool all_enabled = true;
    
    for (const auto& rule_id : ruleset_rules_[ruleset_id]) {
        // Check dependencies for each rule
        if (validateDependenciesInternal(rule_id, philosophy_dependencies_[rule_id])) {
            enabled_rules_.insert(rule_id);
        } else {
            all_enabled = false;
            std::cerr << "Failed to enable rule " << rule_id 
                      << " (missing philosophy dependencies)" << std::endl;
        }
    }
    
    return all_enabled;
}

bool DynamicController::disableAllRulesInRuleset(const std::string& ruleset_id) {
    std::lock_guard<std::mutex> lock(control_mutex_);
    
    for (const auto& rule_id : ruleset_rules_[ruleset_id]) {
        enabled_rules_.erase(rule_id);
    }
    
    return true;
}

bool DynamicController::validateRulesetConsistency(const std::string& ruleset_id) {
    std::lock_guard<std::mutex> lock(control_mutex_);
    
    // Check that ruleset and all its dependencies are properly configured
    if (!validateDependenciesInternal(ruleset_id, philosophy_dependencies_[ruleset_id])) {
        return false;
    }
    
    // Check that all rules in the ruleset have their dependencies met
    for (const auto& rule_id : ruleset_rules_[ruleset_id]) {
        if (!validateDependenciesInternal(rule_id, philosophy_dependencies_[rule_id])) {
            return false;
        }
    }
    
    return true;
}

// Dependency Validation
bool DynamicController::validatePhilosophyDependencies(const std::string& rule_or_ruleset_id) {
    std::lock_guard<std::mutex> lock(control_mutex_);
    return validateDependenciesInternal(rule_or_ruleset_id, philosophy_dependencies_[rule_or_ruleset_id]);
}

bool DynamicController::canDisablePhilosophy(const std::string& philosophy_id) const {
    // Check if any enabled rules or rulesets depend on this philosophy
    for (const auto& rule_id : enabled_rules_) {
        auto it = philosophy_dependencies_.find(rule_id);
        if (it != philosophy_dependencies_.end() && it->second.count(philosophy_id) > 0) {
            return false;
        }
    }
    
    for (const auto& ruleset_id : enabled_rulesets_) {
        auto it = philosophy_dependencies_.find(ruleset_id);
        if (it != philosophy_dependencies_.end() && it->second.count(philosophy_id) > 0) {
            return false;
        }
    }
    
    return true;
}

// Hot-reload Capabilities
bool DynamicController::hotReloadComponent(const std::string& component_id) {
    std::lock_guard<std::mutex> lock(control_mutex_);
    
    // Simulate hot-reload by reloading dependency mappings
    // In a real implementation, this would reload the component definition from disk
    loadDependencyMappings();
    
    std::cout << "Hot-reloaded component: " << component_id << std::endl;
    return true;
}

bool DynamicController::hotReloadRuleset(const std::string& ruleset_id) {
    std::lock_guard<std::mutex> lock(control_mutex_);
    
    // Reload the entire ruleset
    loadDependencyMappings();
    
    std::cout << "Hot-reloaded ruleset: " << ruleset_id << std::endl;
    return true;
}

// User Control Interface
void DynamicController::listActiveComponents() const {
    std::lock_guard<std::mutex> lock(control_mutex_);
    
    std::cout << "=== Active Components ===" << std::endl;
    
    std::cout << "Philosophies (" << enabled_philosophies_.size() << "):" << std::endl;
    for (const auto& id : enabled_philosophies_) {
        std::cout << "  ✓ " << id << std::endl;
    }
    
    std::cout << "Rules (" << enabled_rules_.size() << "):" << std::endl;
    for (const auto& id : enabled_rules_) {
        std::cout << "  ✓ " << id << std::endl;
    }
    
    std::cout << "Rulesets (" << enabled_rulesets_.size() << "):" << std::endl;
    for (const auto& id : enabled_rulesets_) {
        std::cout << "  ✓ " << id << std::endl;
    }
}

void DynamicController::showComponentStatus(const std::string& component_id) const {
    std::lock_guard<std::mutex> lock(control_mutex_);
    
    std::cout << "Component: " << component_id << std::endl;
    
    if (enabled_philosophies_.count(component_id) > 0) {
        std::cout << "  Type: Philosophy (✓ enabled)" << std::endl;
    } else if (enabled_rules_.count(component_id) > 0) {
        std::cout << "  Type: Rule (✓ enabled)" << std::endl;
    } else if (enabled_rulesets_.count(component_id) > 0) {
        std::cout << "  Type: Ruleset (✓ enabled)" << std::endl;
    } else {
        std::cout << "  Status: ✗ disabled or not found" << std::endl;
    }
    
    // Show dependencies
    auto it = philosophy_dependencies_.find(component_id);
    if (it != philosophy_dependencies_.end() && !it->second.empty()) {
        std::cout << "  Philosophy Dependencies:" << std::endl;
        for (const auto& dep : it->second) {
            bool enabled = enabled_philosophies_.count(dep) > 0;
            std::cout << "    " << (enabled ? "✓" : "✗") << " " << dep << std::endl;
        }
    }
}

// Private Methods
void DynamicController::loadDependencyMappings() {
    // Initialize with known dependencies from the ruleset we created
    philosophy_dependencies_["akao:ruleset:structure:cpp_standards:v1"] = {
        "akao:philosophy::structure:enforcement:v1",
        "akao:philosophy::structure:isolation:v1"
    };
    
    ruleset_rules_["akao:ruleset:structure:cpp_standards:v1"] = {
        "akao:rule::structure:class_separation:v1",
        "akao:rule::structure:file_organization:v1",
        "akao:rule::structure:folder_validation:v1",
        "akao:rule::structure:layout_enforcement:v1"
    };
    
    // Add rule dependencies (from existing rule definitions)
    philosophy_dependencies_["akao:rule::structure:class_separation:v1"] = {
        "akao:philosophy::structure:isolation:v1"
    };
}

bool DynamicController::validateDependenciesInternal(const std::string& component_id, 
                                                     const std::unordered_set<std::string>& required_philosophies) {
    for (const auto& philosophy_id : required_philosophies) {
        if (enabled_philosophies_.count(philosophy_id) == 0) {
            return false;
        }
    }
    return true;
}

void DynamicController::cascadeDisable(const std::string& philosophy_id) {
    // Disable any rules/rulesets that depend on this philosophy
    auto rules_to_disable = getDependentRules(philosophy_id);
    auto rulesets_to_disable = getDependentRulesets(philosophy_id);
    
    for (const auto& rule_id : rules_to_disable) {
        enabled_rules_.erase(rule_id);
    }
    
    for (const auto& ruleset_id : rulesets_to_disable) {
        enabled_rulesets_.erase(ruleset_id);
    }
}

std::vector<std::string> DynamicController::getDependentRules(const std::string& philosophy_id) const {
    std::vector<std::string> dependents;
    
    for (const auto& [rule_id, philosophies] : philosophy_dependencies_) {
        if (philosophies.count(philosophy_id) > 0 && enabled_rules_.count(rule_id) > 0) {
            dependents.push_back(rule_id);
        }
    }
    
    return dependents;
}

std::vector<std::string> DynamicController::getDependentRulesets(const std::string& philosophy_id) const {
    std::vector<std::string> dependents;
    
    for (const auto& [ruleset_id, philosophies] : philosophy_dependencies_) {
        if (philosophies.count(philosophy_id) > 0 && enabled_rulesets_.count(ruleset_id) > 0) {
            dependents.push_back(ruleset_id);
        }
    }
    
    return dependents;
}

} // namespace akao::core::engine::runtime