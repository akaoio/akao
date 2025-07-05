#include "rule_loader.hpp"
#include <sstream>
#include <algorithm>
#include <regex>
#include <iostream>
#include <fstream>

namespace akao::core::rule::loader {

// LoadException implementation
RuleLoader::LoadException::LoadException(const LoadError& error) 
    : error_(error) {
    std::stringstream ss;
    ss << "Rule Load Error in " << error_.file_path << ": " << error_.message;
    if (!error_.details.empty()) {
        ss << "\nDetails: " << error_.details;
    }
    what_message_ = ss.str();
}

const char* RuleLoader::LoadException::what() const noexcept {
    return what_message_.c_str();
}

const RuleLoader::LoadError& RuleLoader::LoadException::getError() const {
    return error_;
}

// Constructor
RuleLoader::RuleLoader(const std::string& rules_directory) 
    : rules_directory_(rules_directory) {
    // Ensure rules directory exists
    if (!std::filesystem::exists(rules_directory_)) {
        throwLoadError(rules_directory_, "Rules directory does not exist");
    }
}

// Main loading methods
bool RuleLoader::loadAllRules() {
    loaded_rules_.clear();
    
    try {
        auto rule_files = discoverRuleFiles();
        
        for (const auto& file_path : rule_files) {
            if (!loadRule(file_path)) {
                // Log error but continue loading other rules
                continue;
            }
        }
        
        // Validate all loaded rules
        auto validation_errors = validateAllRules();
        if (!validation_errors.empty()) {
            std::string combined_errors;
            for (const auto& error : validation_errors) {
                combined_errors += error + "; ";
            }
            throwLoadError("validation", "Rule validation failed", combined_errors);
        }
        
        return true;
    } catch (const std::exception& e) {
        throwLoadError("loadAllRules", "Failed to load rules", e.what());
        return false;
    }
}

bool RuleLoader::loadRule(const std::string& rule_file_path) {
    try {
        if (!isValidRuleFile(rule_file_path)) {
            throwLoadError(rule_file_path, "Not a valid rule file");
        }
        
        // BUGFIX: Preprocess YAML to remove document markers that cause parsing issues
        std::ifstream file(rule_file_path);
        if (!file.is_open()) {
            throwLoadError(rule_file_path, "Cannot open rule file");
        }
        
        std::string yaml_content;
        std::string line;
        bool skip_first_doc_marker = true;
        while (std::getline(file, line)) {
            // Skip the first document marker but keep others
            if (line == "---" && skip_first_doc_marker) {
                std::cerr << "DEBUG: Skipping document marker in " << rule_file_path << std::endl;
                skip_first_doc_marker = false;
                continue;
            }
            yaml_content += line + "\n";
        }
        file.close();
        
        std::cerr << "DEBUG: Preprocessed YAML content length: " << yaml_content.length() << std::endl;
        std::cerr << "DEBUG: First 100 chars: " << yaml_content.substr(0, 100) << std::endl;
        
        auto yaml_root = yaml_parser_.parse(yaml_content);
        if (!yaml_root) {
            throwLoadError(rule_file_path, "Failed to parse YAML");
        }
        
        auto rule = parseRuleFromYaml(yaml_root, rule_file_path);
        if (!rule) {
            throwLoadError(rule_file_path, "Failed to parse rule structure");
        }
        
        if (!validateRule(*rule)) {
            throwLoadError(rule_file_path, "Rule validation failed");
        }
        
        loaded_rules_.push_back(rule);
        return true;
        
    } catch (const std::exception& e) {
        throwLoadError(rule_file_path, "Exception during rule loading", e.what());
        return false;
    }
}

bool RuleLoader::loadRuleFromYaml(const std::string& yaml_content, const std::string& source_file) {
    try {
        auto yaml_root = yaml_parser_.parse(yaml_content);
        if (!yaml_root) {
            throwLoadError(source_file, "Failed to parse YAML content");
        }
        
        auto rule = parseRuleFromYaml(yaml_root, source_file);
        if (!rule) {
            throwLoadError(source_file, "Failed to parse rule structure");
        }
        
        if (!validateRule(*rule)) {
            throwLoadError(source_file, "Rule validation failed");
        }
        
        loaded_rules_.push_back(rule);
        return true;
        
    } catch (const std::exception& e) {
        throwLoadError(source_file, "Exception during YAML rule loading", e.what());
        return false;
    }
}

// Rule access methods
const std::vector<std::shared_ptr<Rule>>& RuleLoader::getRules() const {
    return loaded_rules_;
}

std::shared_ptr<Rule> RuleLoader::getRuleById(const std::string& rule_id) const {
    for (const auto& rule : loaded_rules_) {
        if (rule->id == rule_id) {
            return rule;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<Rule>> RuleLoader::getRulesByCategory(const std::string& category) const {
    std::vector<std::shared_ptr<Rule>> result;
    for (const auto& rule : loaded_rules_) {
        if (rule->category == category) {
            result.push_back(rule);
        }
    }
    return result;
}

std::vector<std::shared_ptr<Rule>> RuleLoader::getRulesByPhilosophy(const std::string& philosophy_id) const {
    std::vector<std::shared_ptr<Rule>> result;
    for (const auto& rule : loaded_rules_) {
        auto it = std::find(rule->philosophies.begin(), rule->philosophies.end(), philosophy_id);
        if (it != rule->philosophies.end()) {
            result.push_back(rule);
        }
    }
    return result;
}

// Rule validation
bool RuleLoader::validateRule(const Rule& rule) const {
    bool valid_id = isValidRuleId(rule.id);
    bool has_fields = hasRequiredFields(rule);
    bool valid_philosophies = hasValidPhilosophyReferences(rule);
    
    // Debug output to identify which validation is failing
    if (!valid_id) {
        std::cerr << "DEBUG: Rule ID validation failed for: '" << rule.id << "'" << std::endl;
    }
    if (!has_fields) {
        std::cerr << "DEBUG: Required fields validation failed for rule: '" << rule.id << "'" << std::endl;
        std::cerr << "  - id: '" << rule.id << "' (empty: " << rule.id.empty() << ")" << std::endl;
        std::cerr << "  - name: '" << rule.name << "' (empty: " << rule.name.empty() << ")" << std::endl;
        std::cerr << "  - category: '" << rule.category << "' (empty: " << rule.category.empty() << ")" << std::endl;
        std::cerr << "  - scope: '" << rule.scope << "' (empty: " << rule.scope.empty() << ")" << std::endl;
        std::cerr << "  - target: '" << rule.target << "' (empty: " << rule.target.empty() << ")" << std::endl;
    }
    if (!valid_philosophies) {
        std::cerr << "DEBUG: Philosophy validation failed for rule: '" << rule.id << "'" << std::endl;
        for (const auto& phil : rule.philosophies) {
            std::cerr << "  - philosophy: '" << phil << "'" << std::endl;
        }
    }
    
    return valid_id && has_fields && valid_philosophies;
}

std::vector<std::string> RuleLoader::validateAllRules() const {
    std::vector<std::string> errors;
    
    for (const auto& rule : loaded_rules_) {
        if (!validateRule(*rule)) {
            errors.push_back("Rule validation failed: " + rule->id);
        }
    }
    
    return errors;
}

// Statistics
size_t RuleLoader::getTotalRulesCount() const {
    return loaded_rules_.size();
}

std::vector<std::string> RuleLoader::getCategories() const {
    std::vector<std::string> categories;
    for (const auto& rule : loaded_rules_) {
        if (std::find(categories.begin(), categories.end(), rule->category) == categories.end()) {
            categories.push_back(rule->category);
        }
    }
    std::sort(categories.begin(), categories.end());
    return categories;
}

std::vector<std::string> RuleLoader::getPhilosophies() const {
    std::vector<std::string> philosophies;
    for (const auto& rule : loaded_rules_) {
        for (const auto& philosophy : rule->philosophies) {
            if (std::find(philosophies.begin(), philosophies.end(), philosophy) == philosophies.end()) {
                philosophies.push_back(philosophy);
            }
        }
    }
    std::sort(philosophies.begin(), philosophies.end());
    return philosophies;
}

// Rule discovery
std::vector<std::string> RuleLoader::discoverRuleFiles() const {
    std::vector<std::string> rule_files;
    
    for (const auto& entry : std::filesystem::recursive_directory_iterator(rules_directory_)) {
        if (entry.is_regular_file() && entry.path().extension() == ".yaml") {
            rule_files.push_back(entry.path().string());
        }
    }
    
    std::sort(rule_files.begin(), rule_files.end());
    return rule_files;
}

bool RuleLoader::isValidRuleFile(const std::string& file_path) const {
    return std::filesystem::exists(file_path) && 
           std::filesystem::is_regular_file(file_path) &&
           (file_path.length() >= 5 && file_path.substr(file_path.length() - 5) == ".yaml");
}

// YAML parsing helpers
std::shared_ptr<Rule> RuleLoader::parseRuleFromYaml(std::shared_ptr<engine::parser::YamlNode> yaml_root, 
                                                   const std::string& source_file) {
    std::cerr << "DEBUG: parseRuleFromYaml called for file: " << source_file << std::endl;
    
    if (!yaml_root) {
        std::cerr << "DEBUG: yaml_root is null!" << std::endl;
        return nullptr;
    }
    
    std::cerr << "DEBUG: yaml_root type: " << static_cast<int>(yaml_root->getType()) << std::endl;
    std::cerr << "DEBUG: yaml_root isMapping: " << yaml_root->isMapping() << std::endl;
    
    // BUGFIX: Handle case where YAML parser returns a sequence instead of mapping due to document marker
    std::shared_ptr<engine::parser::YamlNode> actual_root = yaml_root;
    if (yaml_root->isSequence() && yaml_root->size() > 0) {
        std::cerr << "DEBUG: Root is sequence with " << yaml_root->size() << " elements, taking first element" << std::endl;
        actual_root = yaml_root->operator[](0);
        if (actual_root) {
            std::cerr << "DEBUG: First element type: " << static_cast<int>(actual_root->getType()) << std::endl;
            std::cerr << "DEBUG: First element isMapping: " << actual_root->isMapping() << std::endl;
            if (actual_root->isString()) {
                std::cerr << "DEBUG: First element string value: '" << actual_root->asString() << "'" << std::endl;
            }
        }
    }
    
    if (actual_root && actual_root->isMapping()) {
        auto keys = actual_root->getKeys();
        std::cerr << "DEBUG: actual_root has " << keys.size() << " keys: ";
        for (const auto& key : keys) {
            std::cerr << "'" << key << "' ";
        }
        std::cerr << std::endl;
    }
    
    auto rule = std::make_shared<Rule>();
    rule->file_path = source_file;
    
    std::cerr << "DEBUG: Checking for metadata section..." << std::endl;
    // Parse metadata
    if (auto metadata = actual_root->operator[]("metadata")) {
        std::cerr << "DEBUG: Found metadata section, parsing..." << std::endl;
        parseMetadata(*rule, metadata);
    } else {
        std::cerr << "DEBUG: No metadata section found!" << std::endl;
    }
    
    std::cerr << "DEBUG: After parsing metadata - rule.id: '" << rule->id << "'" << std::endl;
    
    // Parse description
    if (auto desc = actual_root->operator[]("description")) {
        rule->description = desc->asString();
    }
    
    // Parse philosophies
    if (auto philosophies = actual_root->operator[]("philosophies")) {
        parsePhilosophies(*rule, philosophies);
    }
    
    // Parse rule definition
    if (auto rule_def = actual_root->operator[]("rule_definition")) {
        parseRuleDefinition(*rule, rule_def);
    }
    
    // Parse implementation
    if (auto impl = actual_root->operator[]("implementation")) {
        parseImplementation(*rule, impl);
    }
    
    // Parse validation
    if (auto validation = actual_root->operator[]("validation")) {
        parseValidation(*rule, validation);
    }
    
    // Parse audit
    if (auto audit = actual_root->operator[]("audit")) {
        parseAudit(*rule, audit);
    }
    
    std::cerr << "DEBUG: Final rule - id: '" << rule->id << "', name: '" << rule->name << "', scope: '" << rule->scope << "', target: '" << rule->target << "'" << std::endl;
    
    return rule;
}

void RuleLoader::parseMetadata(Rule& rule, std::shared_ptr<engine::parser::YamlNode> metadata_node) {
    if (auto id = metadata_node->operator[]("id")) {
        rule.id = id->asString();
    }
    if (auto name = metadata_node->operator[]("name")) {
        rule.name = name->asString();
    }
    if (auto version = metadata_node->operator[]("version")) {
        rule.version = version->asString();
    }
    if (auto category = metadata_node->operator[]("category")) {
        rule.category = category->asString();
    }
}

void RuleLoader::parsePhilosophies(Rule& rule, std::shared_ptr<engine::parser::YamlNode> philosophies_node) {
    if (philosophies_node->isSequence()) {
        const auto& sequence = philosophies_node->asSequence();
        for (const auto& item : sequence) {
            rule.philosophies.push_back(item->asString());
        }
    }
}

void RuleLoader::parseRuleDefinition(Rule& rule, std::shared_ptr<engine::parser::YamlNode> rule_def_node) {
    if (auto scope = rule_def_node->operator[]("scope")) {
        rule.scope = scope->asString();
    }
    if (auto target = rule_def_node->operator[]("target")) {
        rule.target = target->asString();
    }
    
    if (auto conditions = rule_def_node->operator[]("conditions")) {
        if (conditions->isSequence()) {
            const auto& sequence = conditions->asSequence();
            for (const auto& item : sequence) {
                rule.conditions.push_back(item->asString());
            }
        }
    }
    
    if (auto exceptions = rule_def_node->operator[]("exceptions")) {
        if (exceptions->isSequence()) {
            const auto& sequence = exceptions->asSequence();
            for (const auto& item : sequence) {
                rule.exceptions.push_back(item->asString());
            }
        }
    }
    
    if (auto datalog_rules = rule_def_node->operator[]("datalog_rules")) {
        if (datalog_rules->isSequence()) {
            const auto& sequence = datalog_rules->asSequence();
            for (const auto& item : sequence) {
                rule.datalog_rules.push_back(item->asString());
            }
        }
    }
    
    if (auto logic_queries = rule_def_node->operator[]("logic_queries")) {
        parseLogicQueries(rule, logic_queries);
    }
}

void RuleLoader::parseLogicQueries(Rule& rule, std::shared_ptr<engine::parser::YamlNode> logic_node) {
    if (auto check_compliance = logic_node->operator[]("check_compliance")) {
        rule.check_compliance_query = check_compliance->asString();
    }
    if (auto find_violations = logic_node->operator[]("find_violations")) {
        rule.find_violations_query = find_violations->asString();
    }
    if (auto count_violations = logic_node->operator[]("count_violations")) {
        rule.count_violations_query = count_violations->asString();
    }
}

void RuleLoader::parseImplementation(Rule& rule, std::shared_ptr<engine::parser::YamlNode> impl_node) {
    if (auto check_method = impl_node->operator[]("check_method")) {
        rule.check_method = check_method->asString();
    }
    if (auto auto_fix = impl_node->operator[]("auto_fix")) {
        rule.auto_fix = auto_fix->asBoolean();
    }
    if (auto severity = impl_node->operator[]("severity")) {
        rule.severity = severity->asString();
    }
}

void RuleLoader::parseValidation(Rule& rule, std::shared_ptr<engine::parser::YamlNode> validation_node) {
    if (auto test_cases = validation_node->operator[]("test_cases")) {
        if (test_cases->isSequence()) {
            const auto& sequence = test_cases->asSequence();
            for (const auto& test_case_node : sequence) {
                Rule::TestCase test_case;
                if (auto name = test_case_node->operator[]("name")) {
                    test_case.name = name->asString();
                }
                if (auto description = test_case_node->operator[]("description")) {
                    test_case.description = description->asString();
                }
                if (auto expected = test_case_node->operator[]("expected")) {
                    test_case.expected = expected->asString();
                }
                rule.test_cases.push_back(test_case);
            }
        }
    }
}

void RuleLoader::parseAudit(Rule& rule, std::shared_ptr<engine::parser::YamlNode> audit_node) {
    if (auto compliance_checks = audit_node->operator[]("compliance_checks")) {
        if (compliance_checks->isSequence()) {
            const auto& sequence = compliance_checks->asSequence();
            for (const auto& item : sequence) {
                rule.compliance_checks.push_back(item->asString());
            }
        }
    }
    if (auto violation_severity = audit_node->operator[]("violation_severity")) {
        rule.violation_severity = violation_severity->asString();
    }
    if (auto auto_fix_available = audit_node->operator[]("auto_fix_available")) {
        rule.auto_fix_available = auto_fix_available->asBoolean();
    }
}

// Validation helpers
bool RuleLoader::isValidRuleId(const std::string& rule_id) const {
    // Rule ID format: akao:rule::category:rule_name:version
    std::regex rule_id_pattern(R"(akao:rule::[a-zA-Z_]+:[a-zA-Z_]+:v\d+)");
    return std::regex_match(rule_id, rule_id_pattern);
}

bool RuleLoader::hasRequiredFields(const Rule& rule) const {
    return !rule.id.empty() && 
           !rule.name.empty() && 
           !rule.category.empty() && 
           !rule.scope.empty() && 
           !rule.target.empty();
}

bool RuleLoader::hasValidPhilosophyReferences(const Rule& rule) const {
    // For now, just check that philosophy IDs follow the correct format
    for (const auto& philosophy_id : rule.philosophies) {
        std::regex philosophy_pattern(R"(akao:philosophy::[a-zA-Z_]+:[a-zA-Z_]+:v\d+)");
        if (!std::regex_match(philosophy_id, philosophy_pattern)) {
            return false;
        }
    }
    return true;
}

// Error reporting
void RuleLoader::throwLoadError(const std::string& file_path, const std::string& message, 
                               const std::string& details) {
    LoadError error;
    error.file_path = file_path;
    error.message = message;
    error.details = details;
    throw LoadException(error);
}

} // namespace akao::core::rule::loader
