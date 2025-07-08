/**
 * @id: akao:file:core:rule:registry:rule_registry
 * @doc: Rule registry implementation providing comprehensive rule management system with loading, indexing, filtering, and validation capabilities for systematic governance and archaeological preservation
 * @specification: Core rule registry implementation with rule loading, category indexing, philosophy mapping, and validation framework
 * @scope: Core rule registry implementation for systematic rule management and governance
 * @timeline: 2025-01-08
 * @rationale: Provide centralized rule management capability with systematic indexing and validation for archaeological preservation
 * @methodology: Registry-based rule management with multi-dimensional indexing, validation, and event notification system
 * @references: ["akao:file:core:rule:registry:rule_registry_hpp", "akao:class:core:rule:registry:rule_registry", "akao:philosophy:rule:governance:v1"]
 */

#include "v1.hpp"
#include <algorithm>
#include <functional>
#include <sstream>
#include <chrono>
#include <iostream>

namespace akao::core::rule::registry {

// Constructor
RuleRegistry::RuleRegistry(const std::string& rules_directory) 
    : rules_directory_(rules_directory), is_loaded_(false), last_load_timestamp_(0) {
    rule_loader_ = std::make_unique<loader::RuleLoader>(rules_directory);
}

// Registry management
bool RuleRegistry::loadRules() {
    try {
        clearRegistry();
        
        if (!rule_loader_->loadAllRules()) {
            return false;
        }
        
        // Get loaded rules from loader
        const auto& loaded_rules = rule_loader_->getRules();
        
        // Register all loaded rules
        for (const auto& rule : loaded_rules) {
            if (!registerRule(rule)) {
                return false;
            }
        }
        
        buildIndexes();
        is_loaded_ = true;
        last_load_timestamp_ = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        
        notifyRuleChange("*", "registry_loaded");
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to load rules: " << e.what() << std::endl;
        return false;
    }
}

bool RuleRegistry::reloadRules() {
    clearRegistry();
    return loadRules();
}

void RuleRegistry::clearRegistry() {
    rules_by_id_.clear();
    rules_by_category_.clear();
    rules_by_philosophy_.clear();
    is_loaded_ = false;
    
    notifyRuleChange("*", "registry_cleared");
}

bool RuleRegistry::isLoaded() const {
    return is_loaded_;
}

// Rule registration
bool RuleRegistry::registerRule(std::shared_ptr<loader::Rule> rule) {
    if (!rule || !isValidRuleForRegistration(*rule)) {
        return false;
    }
    
    if (!checkRuleIdUniqueness(rule->id)) {
        return false;
    }
    
    // Register in main index
    rules_by_id_[rule->id] = rule;
    
    // Update category index
    updateCategoryIndex(rule);
    
    // Update philosophy index
    updatePhilosophyIndex(rule);
    
    notifyRuleChange(rule->id, "rule_registered");
    return true;
}

bool RuleRegistry::unregisterRule(const std::string& rule_id) {
    auto it = rules_by_id_.find(rule_id);
    if (it == rules_by_id_.end()) {
        return false;
    }
    
    // Remove from indexes
    removeCategoryIndex(rule_id);
    removePhilosophyIndex(rule_id);
    
    // Remove from main index
    rules_by_id_.erase(it);
    
    notifyRuleChange(rule_id, "rule_unregistered");
    return true;
}

// Rule lookup
std::shared_ptr<loader::Rule> RuleRegistry::getRuleById(const std::string& rule_id) const {
    auto it = rules_by_id_.find(rule_id);
    return (it != rules_by_id_.end()) ? it->second : nullptr;
}

std::vector<std::shared_ptr<loader::Rule>> RuleRegistry::getAllRules() const {
    std::vector<std::shared_ptr<loader::Rule>> all_rules;
    for (const auto& pair : rules_by_id_) {
        all_rules.push_back(pair.second);
    }
    return all_rules;
}

std::vector<std::shared_ptr<loader::Rule>> RuleRegistry::getRulesByCategory(const std::string& category) const {
    auto it = rules_by_category_.find(category);
    return (it != rules_by_category_.end()) ? it->second : std::vector<std::shared_ptr<loader::Rule>>{};
}

std::vector<std::shared_ptr<loader::Rule>> RuleRegistry::getRulesByPhilosophy(const std::string& philosophy_id) const {
    auto it = rules_by_philosophy_.find(philosophy_id);
    return (it != rules_by_philosophy_.end()) ? it->second : std::vector<std::shared_ptr<loader::Rule>>{};
}

std::vector<std::shared_ptr<loader::Rule>> RuleRegistry::getRulesByScope(const std::string& scope) const {
    return filterRules([&scope](const loader::Rule& rule) {
        return rule.scope == scope;
    });
}

std::vector<std::shared_ptr<loader::Rule>> RuleRegistry::getRulesBySeverity(const std::string& severity) const {
    return filterRules([&severity](const loader::Rule& rule) {
        return rule.severity == severity;
    });
}

// Rule queries
bool RuleRegistry::hasRule(const std::string& rule_id) const {
    return rules_by_id_.find(rule_id) != rules_by_id_.end();
}

bool RuleRegistry::hasCategory(const std::string& category) const {
    return rules_by_category_.find(category) != rules_by_category_.end();
}

bool RuleRegistry::hasPhilosophy(const std::string& philosophy_id) const {
    return rules_by_philosophy_.find(philosophy_id) != rules_by_philosophy_.end();
}

// Rule filtering
std::vector<std::shared_ptr<loader::Rule>> RuleRegistry::filterRules(
    const std::function<bool(const loader::Rule&)>& predicate) const {
    std::vector<std::shared_ptr<loader::Rule>> filtered_rules;
    
    for (const auto& pair : rules_by_id_) {
        if (predicate(*pair.second)) {
            filtered_rules.push_back(pair.second);
        }
    }
    
    return filtered_rules;
}

std::vector<std::shared_ptr<loader::Rule>> RuleRegistry::getRulesWithAutoFix() const {
    return filterRules([](const loader::Rule& rule) {
        return rule.auto_fix;
    });
}

std::vector<std::shared_ptr<loader::Rule>> RuleRegistry::getActiveRules() const {
    // For now, all loaded rules are considered active
    return getAllRules();
}

// Statistics and information
size_t RuleRegistry::getTotalRulesCount() const {
    return rules_by_id_.size();
}

size_t RuleRegistry::getRulesCountByCategory(const std::string& category) const {
    auto rules = getRulesByCategory(category);
    return rules.size();
}

size_t RuleRegistry::getRulesCountByPhilosophy(const std::string& philosophy_id) const {
    auto rules = getRulesByPhilosophy(philosophy_id);
    return rules.size();
}

std::vector<std::string> RuleRegistry::getAllCategories() const {
    std::vector<std::string> categories;
    for (const auto& pair : rules_by_category_) {
        categories.push_back(pair.first);
    }
    std::sort(categories.begin(), categories.end());
    return categories;
}

std::vector<std::string> RuleRegistry::getAllPhilosophies() const {
    std::vector<std::string> philosophies;
    for (const auto& pair : rules_by_philosophy_) {
        philosophies.push_back(pair.first);
    }
    std::sort(philosophies.begin(), philosophies.end());
    return philosophies;
}

std::vector<std::string> RuleRegistry::getAllScopes() const {
    return extractUniqueValues([](const loader::Rule& rule) {
        return rule.scope;
    });
}

std::vector<std::string> RuleRegistry::getAllSeverities() const {
    return extractUniqueValues([](const loader::Rule& rule) {
        return rule.severity;
    });
}

// Rule validation
bool RuleRegistry::validateRegistry() const {
    auto errors = getValidationErrors();
    return errors.empty();
}

std::vector<std::string> RuleRegistry::getValidationErrors() const {
    std::vector<std::string> errors;
    
    // Check for duplicate rule IDs (should not happen due to map structure)
    // Check for valid rule references
    for (const auto& pair : rules_by_id_) {
        if (!validateRuleReferences(*pair.second)) {
            errors.push_back("Invalid references in rule: " + pair.first);
        }
    }
    
    // Check for circular dependencies
    if (hasCircularDependencies()) {
        errors.push_back("Circular dependencies detected in rule registry");
    }
    
    return errors;
}

bool RuleRegistry::checkRuleConsistency() const {
    // Verify that category and philosophy indexes are consistent
    for (const auto& pair : rules_by_id_) {
        const auto& rule = pair.second;
        
        // Check category index
        auto category_rules = getRulesByCategory(rule->category);
        bool found_in_category = std::find(category_rules.begin(), category_rules.end(), rule) != category_rules.end();
        if (!found_in_category) {
            return false;
        }
        
        // Check philosophy indexes
        for (const auto& philosophy : rule->philosophies) {
            auto philosophy_rules = getRulesByPhilosophy(philosophy);
            bool found_in_philosophy = std::find(philosophy_rules.begin(), philosophy_rules.end(), rule) != philosophy_rules.end();
            if (!found_in_philosophy) {
                return false;
            }
        }
    }
    
    return true;
}

// Rule dependency management (simplified for now)
std::vector<std::string> RuleRegistry::getRuleDependencies(const std::string& rule_id) const {
    // For now, return empty as we don't have explicit dependencies
    return {};
}

std::vector<std::string> RuleRegistry::getRuleDependents(const std::string& rule_id) const {
    // For now, return empty as we don't have explicit dependencies
    return {};
}

bool RuleRegistry::hasCircularDependencies() const {
    // For now, return false as we don't have explicit dependencies
    return false;
}

// Export and serialization
std::string RuleRegistry::exportToYaml() const {
    std::stringstream ss;
    ss << "---\n";
    ss << "# Akao Rule Registry Export\n";
    ss << "rule_registry:\n";
    ss << "  total_rules: " << getTotalRulesCount() << "\n";
    ss << "  categories: " << getAllCategories().size() << "\n";
    ss << "  philosophies: " << getAllPhilosophies().size() << "\n";
    ss << "  timestamp: " << last_load_timestamp_ << "\n";
    ss << "\nrules:\n";
    
    for (const auto& pair : rules_by_id_) {
        const auto& rule = pair.second;
        ss << "- id: " << rule->id << "\n";
        ss << "  name: " << rule->name << "\n";
        ss << "  category: " << rule->category << "\n";
        ss << "  scope: " << rule->scope << "\n";
        ss << "  severity: " << rule->severity << "\n";
    }
    
    return ss.str();
}

// Rule execution order
std::vector<std::shared_ptr<loader::Rule>> RuleRegistry::getRulesInExecutionOrder() const {
    auto rules = getAllRules();
    
    // Sort by category, then by severity, then by ID
    std::sort(rules.begin(), rules.end(), [](const auto& a, const auto& b) {
        if (a->category != b->category) {
            return a->category < b->category;
        }
        if (a->severity != b->severity) {
            // High severity first
            return a->severity > b->severity;
        }
        return a->id < b->id;
    });
    
    return rules;
}

std::vector<std::shared_ptr<loader::Rule>> RuleRegistry::getRulesForTarget(const std::string& target_path) const {
    // For now, return all rules (will be refined based on target type detection)
    return getAllRules();
}

// Event handling
void RuleRegistry::setRuleChangeCallback(RuleChangeCallback callback) {
    rule_change_callback_ = callback;
}

// Debug and diagnostics
std::string RuleRegistry::getRegistryStatus() const {
    std::stringstream ss;
    ss << "Rule Registry Status:\n";
    ss << "  Loaded: " << (is_loaded_ ? "Yes" : "No") << "\n";
    ss << "  Total Rules: " << getTotalRulesCount() << "\n";
    ss << "  Categories: " << getAllCategories().size() << "\n";
    ss << "  Philosophies: " << getAllPhilosophies().size() << "\n";
    ss << "  Last Load: " << last_load_timestamp_ << "\n";
    ss << "  Consistent: " << (checkRuleConsistency() ? "Yes" : "No") << "\n";
    return ss.str();
}

void RuleRegistry::printRegistryStats() const {
    std::cout << getRegistryStatus() << std::endl;
}

// Private methods
void RuleRegistry::buildIndexes() {
    rules_by_category_.clear();
    rules_by_philosophy_.clear();
    
    for (const auto& pair : rules_by_id_) {
        updateCategoryIndex(pair.second);
        updatePhilosophyIndex(pair.second);
    }
}

void RuleRegistry::updateCategoryIndex(std::shared_ptr<loader::Rule> rule) {
    rules_by_category_[rule->category].push_back(rule);
}

void RuleRegistry::updatePhilosophyIndex(std::shared_ptr<loader::Rule> rule) {
    for (const auto& philosophy : rule->philosophies) {
        rules_by_philosophy_[philosophy].push_back(rule);
    }
}

void RuleRegistry::removeCategoryIndex(const std::string& rule_id) {
    auto rule = getRuleById(rule_id);
    if (!rule) return;
    
    auto& category_rules = rules_by_category_[rule->category];
    category_rules.erase(std::remove(category_rules.begin(), category_rules.end(), rule), category_rules.end());
    
    if (category_rules.empty()) {
        rules_by_category_.erase(rule->category);
    }
}

void RuleRegistry::removePhilosophyIndex(const std::string& rule_id) {
    auto rule = getRuleById(rule_id);
    if (!rule) return;
    
    for (const auto& philosophy : rule->philosophies) {
        auto& philosophy_rules = rules_by_philosophy_[philosophy];
        philosophy_rules.erase(std::remove(philosophy_rules.begin(), philosophy_rules.end(), rule), philosophy_rules.end());
        
        if (philosophy_rules.empty()) {
            rules_by_philosophy_.erase(philosophy);
        }
    }
}

bool RuleRegistry::isValidRuleForRegistration(const loader::Rule& rule) const {
    return !rule.id.empty() && !rule.name.empty() && !rule.category.empty();
}

bool RuleRegistry::checkRuleIdUniqueness(const std::string& rule_id) const {
    return rules_by_id_.find(rule_id) == rules_by_id_.end();
}

bool RuleRegistry::validateRuleReferences(const loader::Rule& rule) const {
    // For now, just return true (will be enhanced with actual reference validation)
    return true;
}

std::vector<std::string> RuleRegistry::extractUniqueValues(
    const std::function<std::string(const loader::Rule&)>& extractor) const {
    std::set<std::string> unique_values;
    
    for (const auto& pair : rules_by_id_) {
        unique_values.insert(extractor(*pair.second));
    }
    
    return std::vector<std::string>(unique_values.begin(), unique_values.end());
}

void RuleRegistry::notifyRuleChange(const std::string& rule_id, const std::string& change_type) {
    if (rule_change_callback_) {
        rule_change_callback_(rule_id, change_type);
    }
}

} // namespace akao::core::rule::registry
