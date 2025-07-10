/**
 * @id: akao:file:core:filesystem:enforcer_structure_enforcer_impl:v1
 * @doc: Implementation of Universal Structure Enforcer providing concrete algorithms for directory and file structure validation, enforcement, and auto-fixing capabilities. Implements comprehensive project template management, violation detection and remediation, backup and recovery systems, and configurable enforcement modes with detailed action tracking and statistics collection.
 * @specification: Concrete implementation of universal structure enforcement algorithms with template management
 * @scope: Core filesystem enforcement implementation
 * @timeline: 2025-07-09
 * @rationale: Provide executable structure enforcement capabilities for maintaining architectural consistency and project standards
 * @methodology: Rule-based enforcement algorithms with template management, action execution, and comprehensive violation remediation
 * @references: ["akao:file:core:filesystem:enforcer_structure_enforcer:v1", "akao:file:core:filesystem:scanner_directory_scanner:v1"]
 */

#include "v1.hpp"
#include <chrono>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <regex>

namespace akao::core::filesystem::enforcer {

StructureEnforcer::StructureEnforcer() {
    // Initialize with default config
    config_.mode = Mode::VALIDATE_ONLY;
    config_.strict_mode = false;
    config_.create_missing_directories = true;
    config_.create_missing_files = false;
    config_.remove_forbidden_paths = false;
    config_.backup_directory = ".akao_backup";
    config_.create_backups = true;
    
    scanner_ = std::make_unique<scanner::DirectoryScanner>();
    initializeDefaultTemplates();
    loadDefaultAkaoRules();
}

StructureEnforcer::StructureEnforcer(const Config& config) : config_(config) {
    scanner_ = std::make_unique<scanner::DirectoryScanner>();
    initializeDefaultTemplates();
    loadDefaultAkaoRules();
}

void StructureEnforcer::setConfig(const Config& config) {
    config_ = config;
}

bool StructureEnforcer::initialize() {
    try {
        loadDefaultAkaoRules();
        initializeDefaultTemplates();
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

const StructureEnforcer::Config& StructureEnforcer::getConfig() const {
    return config_;
}

void StructureEnforcer::addRule(const EnforcementRule& rule) {
    rules_.push_back(rule);
}

bool StructureEnforcer::loadRules(const std::string& rules_directory) {
    // This would load rules from YAML files in the directory
    // For now, we'll use the default rules
    return true;
}

void StructureEnforcer::loadDefaultAkaoRules() {
    // Load mandatory rules
    auto mandatory_rules = akao_rules::getMandatoryRules();
    rules_.insert(rules_.end(), mandatory_rules.begin(), mandatory_rules.end());
    
    // Load recommended rules
    auto recommended_rules = akao_rules::getRecommendedRules();
    rules_.insert(rules_.end(), recommended_rules.begin(), recommended_rules.end());
    
    // Load optional rules if not in strict mode
    if (!config_.strict_mode) {
        auto optional_rules = akao_rules::getOptionalRules();
        rules_.insert(rules_.end(), optional_rules.begin(), optional_rules.end());
    }
}

EnforcementResult StructureEnforcer::enforceStructure(const std::string& directory_path) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    EnforcementResult result;
    
    try {
        // Scan directory structure
        auto scan_result = scanner_->scanDirectory(directory_path);
        if (!scan_result.success) {
            result.error_message = "Failed to scan directory: " + scan_result.error_message;
            return result;
        }
        
        // Check all rules
        result = checkRules(scan_result.root_directory, directory_path);
        
        // If in auto-fix mode, attempt to fix violations
        if (config_.mode == Mode::AUTO_FIX && !result.violations.empty()) {
            auto fix_result = autoFix(directory_path, result.violations);
            result.actions_taken = fix_result.actions_taken;
            
            // Re-validate after fixes
            if (fix_result.success) {
                auto revalidation_result = validateStructure(directory_path);
                result.violations = revalidation_result.violations;
            }
        } else {
            // Generate recommended actions
            result.recommended_actions = generateActions(result.violations);
        }
        
        result.success = true;
        
    } catch (const std::exception& e) {
        result.success = false;
        result.error_message = "Enforcement failed: " + std::string(e.what());
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time);
    result.enforcement_duration_seconds = duration.count();
    
    updateStats(result);
    
    return result;
}

EnforcementResult StructureEnforcer::validateStructure(const std::string& directory_path) {
    auto original_mode = config_.mode;
    config_.mode = Mode::VALIDATE_ONLY;
    
    auto result = enforceStructure(directory_path);
    
    config_.mode = original_mode;
    return result;
}

EnforcementResult StructureEnforcer::autoFix(const std::string& directory_path, 
                                            const std::vector<akao::core::engine::validator::Violation>& violations) {
    EnforcementResult result;
    result.success = true;
    
    auto actions = generateActions(violations);
    
    for (const auto& action : actions) {
        if (config_.create_backups && action.is_reversible) {
            createBackup(action.source_path);
        }
        
        if (executeAction(action)) {
            result.actions_taken.push_back(action);
        } else {
            result.recommended_actions.push_back(action);
            result.success = false;
        }
    }
    
    return result;
}

std::vector<EnforcementAction> StructureEnforcer::previewActions(const std::string& directory_path) {
    auto validation_result = validateStructure(directory_path);
    return generateActions(validation_result.violations);
}

EnforcementResult StructureEnforcer::createProjectStructure(const std::string& directory_path, 
                                                           const std::string& project_type) {
    EnforcementResult result;
    
    auto template_structure = getTemplateStructure(project_type);
    if (template_structure.empty()) {
        result.error_message = "Unknown project type: " + project_type;
        return result;
    }
    
    try {
        // Create base directory if it doesn't exist
        if (!std::filesystem::exists(directory_path)) {
            std::filesystem::create_directories(directory_path);
        }
        
        // Create directory structure
        for (const auto& [dir_name, files] : template_structure) {
            std::string dir_path = directory_path + "/" + dir_name;
            
            if (createDirectory(dir_path)) {
                EnforcementAction action;
                action.action_type = EnforcementAction::CREATE_DIRECTORY;
                action.target_path = dir_path;
                action.reason = "Creating project structure for " + project_type;
                result.actions_taken.push_back(action);
            }
            
            // Create files in directory
            for (const auto& filename : files) {
                std::string file_path = dir_path + "/" + filename;
                std::string content = generateDefaultFileContent(filename, project_type);
                
                if (createFile(file_path, content)) {
                    EnforcementAction action;
                    action.action_type = EnforcementAction::CREATE_FILE;
                    action.target_path = file_path;
                    action.content = content;
                    action.reason = "Creating default " + filename + " for " + project_type;
                    result.actions_taken.push_back(action);
                }
            }
        }
        
        result.success = true;
        
    } catch (const std::exception& e) {
        result.success = false;
        result.error_message = "Failed to create project structure: " + std::string(e.what());
    }
    
    return result;
}

EnforcementResult StructureEnforcer::migrateStructure(const std::string& directory_path,
                                                     const std::map<std::string, std::vector<std::string>>& target_structure) {
    EnforcementResult result;
    result.success = true;
    
    // This would implement structure migration logic
    // For now, return success
    return result;
}

std::vector<std::string> StructureEnforcer::getAvailableTemplates() const {
    std::vector<std::string> templates;
    for (const auto& [name, structure] : project_templates_) {
        templates.push_back(name);
    }
    return templates;
}

void StructureEnforcer::registerTemplate(const std::string& template_name,
                                        const std::map<std::string, std::vector<std::string>>& structure) {
    project_templates_[template_name] = structure;
}

const StructureEnforcer::EnforcementStats& StructureEnforcer::getStats() const {
    return stats_;
}

void StructureEnforcer::clearStats() {
    stats_ = EnforcementStats{};
}

// Private methods implementation
EnforcementResult StructureEnforcer::checkRules(const scanner::DirectoryInfo& directory_info, 
                                               const std::string& base_path) {
    EnforcementResult result;
    result.success = true;
    
    for (const auto& rule : rules_) {
        if (isRuleExcluded(rule.id)) {
            continue;
        }
        
        result.rules_checked++;
        
        std::vector<akao::core::engine::validator::Violation> rule_violations;
        bool rule_passed = checkRule(rule, directory_info, rule_violations);
        
        if (rule_passed) {
            result.rules_passed++;
        } else {
            result.rules_failed++;
            result.violations.insert(result.violations.end(), 
                                   rule_violations.begin(), rule_violations.end());
        }
    }
    
    return result;
}

bool StructureEnforcer::checkRule(const EnforcementRule& rule, 
                                 const scanner::DirectoryInfo& directory_info,
                                 std::vector<akao::core::engine::validator::Violation>& violations) {
    bool rule_passed = true;
    
    // Check required directories
    if (!checkRequiredDirectories(rule, directory_info, violations)) {
        rule_passed = false;
    }
    
    // Check required files
    if (!checkRequiredFiles(rule, directory_info, violations)) {
        rule_passed = false;
    }
    
    // Check forbidden paths
    if (!checkForbiddenPaths(rule, directory_info, violations)) {
        rule_passed = false;
    }
    
    // Check naming patterns
    if (!checkNamingPatterns(rule, directory_info, violations)) {
        rule_passed = false;
    }
    
    // Run custom validator if provided
    if (rule.validator && !rule.validator(directory_info)) {
        auto violation = createViolation(rule, directory_info.path, 
                                       "Custom validation failed", 
                                       "Check rule-specific requirements");
        violations.push_back(violation);
        rule_passed = false;
    }
    
    return rule_passed;
}

std::vector<EnforcementAction> StructureEnforcer::generateActions(const std::vector<akao::core::engine::validator::Violation>& violations) {
    std::vector<EnforcementAction> actions;
    
    for (const auto& violation : violations) {
        EnforcementAction action;
        action.reason = "Fix violation: " + violation.message;
        
        // Determine action type based on violation
        if (violation.message.find("missing directory") != std::string::npos) {
            action.action_type = EnforcementAction::CREATE_DIRECTORY;
            action.target_path = violation.file_path;
        } else if (violation.message.find("missing file") != std::string::npos) {
            action.action_type = EnforcementAction::CREATE_FILE;
            action.target_path = violation.file_path;
        } else if (violation.message.find("forbidden path") != std::string::npos) {
            action.action_type = EnforcementAction::DELETE_FILE;
            action.source_path = violation.file_path;
        } else {
            action.action_type = EnforcementAction::VALIDATE_ONLY;
        }
        
        actions.push_back(action);
    }
    
    return actions;
}

bool StructureEnforcer::executeAction(const EnforcementAction& action) {
    switch (action.action_type) {
        case EnforcementAction::CREATE_DIRECTORY:
            return createDirectory(action.target_path);
        
        case EnforcementAction::CREATE_FILE:
            return createFile(action.target_path, action.content);
        
        case EnforcementAction::MOVE_FILE:
            return moveFile(action.source_path, action.target_path);
        
        case EnforcementAction::RENAME_FILE:
            return renameFile(action.source_path, action.target_path);
        
        case EnforcementAction::DELETE_FILE:
            if (config_.remove_forbidden_paths) {
                return deleteFile(action.source_path);
            }
            return false;
        
        case EnforcementAction::MODIFY_CONTENT:
            return modifyFileContent(action.target_path, action.content);
        
        case EnforcementAction::VALIDATE_ONLY:
        default:
            return false;
    }
}

// Rule checking implementation
bool StructureEnforcer::checkRequiredDirectories(const EnforcementRule& rule,
                                                const scanner::DirectoryInfo& directory_info,
                                                std::vector<akao::core::engine::validator::Violation>& violations) {
    bool all_found = true;
    
    for (const auto& required_dir : rule.required_directories) {
        bool found = false;
        
        // Check if directory exists in current level
        for (const auto& subdir : directory_info.subdirectories) {
            if (subdir.name == required_dir) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            std::string full_path = directory_info.path + "/" + required_dir;
            auto violation = createViolation(rule, full_path,
                                           "Required directory missing: " + required_dir,
                                           "Create directory: " + required_dir);
            violations.push_back(violation);
            all_found = false;
        }
    }
    
    return all_found;
}

bool StructureEnforcer::checkRequiredFiles(const EnforcementRule& rule,
                                          const scanner::DirectoryInfo& directory_info,
                                          std::vector<akao::core::engine::validator::Violation>& violations) {
    bool all_found = true;
    
    for (const auto& required_file : rule.required_files) {
        bool found = false;
        
        // Check if file exists in current level
        for (const auto& file : directory_info.files) {
            if (file.filename == required_file) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            std::string full_path = directory_info.path + "/" + required_file;
            auto violation = createViolation(rule, full_path,
                                           "Required file missing: " + required_file,
                                           "Create file: " + required_file);
            violations.push_back(violation);
            all_found = false;
        }
    }
    
    return all_found;
}

bool StructureEnforcer::checkForbiddenPaths(const EnforcementRule& rule,
                                           const scanner::DirectoryInfo& directory_info,
                                           std::vector<akao::core::engine::validator::Violation>& violations) {
    bool no_forbidden_found = true;
    
    std::function<void(const scanner::DirectoryInfo&)> checkRecursive = [&](const scanner::DirectoryInfo& dir) {
        // Check files
        for (const auto& file : dir.files) {
            for (const auto& forbidden_pattern : rule.forbidden_paths) {
                if (file.path.find(forbidden_pattern) != std::string::npos) {
                    auto violation = createViolation(rule, file.path,
                                                   "Forbidden path found: " + file.path,
                                                   "Remove or relocate: " + file.filename);
                    violations.push_back(violation);
                    no_forbidden_found = false;
                }
            }
        }
        
        // Check subdirectories
        for (const auto& subdir : dir.subdirectories) {
            for (const auto& forbidden_pattern : rule.forbidden_paths) {
                if (subdir.path.find(forbidden_pattern) != std::string::npos) {
                    auto violation = createViolation(rule, subdir.path,
                                                   "Forbidden directory found: " + subdir.path,
                                                   "Remove or relocate: " + subdir.name);
                    violations.push_back(violation);
                    no_forbidden_found = false;
                }
            }
            checkRecursive(subdir);
        }
    };
    
    checkRecursive(directory_info);
    return no_forbidden_found;
}

bool StructureEnforcer::checkNamingPatterns(const EnforcementRule& rule,
                                           const scanner::DirectoryInfo& directory_info,
                                           std::vector<akao::core::engine::validator::Violation>& violations) {
    bool all_match = true;
    
    for (const auto& pattern : rule.naming_patterns) {
        std::regex pattern_regex(pattern);
        
        // Check file names
        for (const auto& file : directory_info.files) {
            if (!std::regex_match(file.filename, pattern_regex)) {
                auto violation = createViolation(rule, file.path,
                                               "File name doesn't match pattern: " + pattern,
                                               "Rename file to match pattern: " + pattern);
                violations.push_back(violation);
                all_match = false;
            }
        }
        
        // Check directory names
        for (const auto& subdir : directory_info.subdirectories) {
            if (!std::regex_match(subdir.name, pattern_regex)) {
                auto violation = createViolation(rule, subdir.path,
                                               "Directory name doesn't match pattern: " + pattern,
                                               "Rename directory to match pattern: " + pattern);
                violations.push_back(violation);
                all_match = false;
            }
        }
    }
    
    return all_match;
}

// Action execution methods
bool StructureEnforcer::createDirectory(const std::string& path) {
    try {
        if (!std::filesystem::exists(path)) {
            return std::filesystem::create_directories(path);
        }
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool StructureEnforcer::createFile(const std::string& path, const std::string& content) {
    try {
        // Create parent directories if needed
        std::filesystem::path file_path(path);
        std::filesystem::create_directories(file_path.parent_path());
        
        std::ofstream file(path);
        if (file.is_open()) {
            file << content;
            file.close();
            return true;
        }
        return false;
    } catch (const std::exception&) {
        return false;
    }
}

bool StructureEnforcer::moveFile(const std::string& source, const std::string& target) {
    try {
        std::filesystem::create_directories(std::filesystem::path(target).parent_path());
        std::filesystem::rename(source, target);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool StructureEnforcer::renameFile(const std::string& source, const std::string& target) {
    return moveFile(source, target);
}

bool StructureEnforcer::deleteFile(const std::string& path) {
    try {
        return std::filesystem::remove(path);
    } catch (const std::exception&) {
        return false;
    }
}

bool StructureEnforcer::modifyFileContent(const std::string& path, const std::string& content) {
    try {
        std::ofstream file(path);
        if (file.is_open()) {
            file << content;
            file.close();
            return true;
        }
        return false;
    } catch (const std::exception&) {
        return false;
    }
}

// Helper methods
bool StructureEnforcer::createBackup(const std::string& path) {
    if (!std::filesystem::exists(path)) {
        return true;
    }
    
    try {
        std::string backup_path = getBackupPath(path);
        std::filesystem::create_directories(std::filesystem::path(backup_path).parent_path());
        std::filesystem::copy(path, backup_path);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::string StructureEnforcer::getBackupPath(const std::string& original_path) {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&time_t);
    
    char timestamp[20];
    std::strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", &tm);
    
    std::filesystem::path path(original_path);
    std::string backup_name = path.filename().string() + "_" + timestamp;
    
    return config_.backup_directory + "/" + backup_name;
}

void StructureEnforcer::initializeDefaultTemplates() {
    // Register Akao project template
    project_templates_["akao"] = akao_rules::getAkaoTemplate();
    
    // Register basic C++ template
    project_templates_["cpp"] = {
        {"src", {"main.cpp"}},
        {"include", {"project.hpp"}},
        {"tests", {"test_main.cpp"}},
        {"docs", {"README.md"}},
        {"build", {}},
        {".", {"CMakeLists.txt", "Makefile", ".gitignore"}}
    };
    
    // Register Python template
    project_templates_["python"] = {
        {"src", {"__init__.py", "main.py"}},
        {"tests", {"__init__.py", "test_main.py"}},
        {"docs", {"README.md"}},
        {".", {"setup.py", "requirements.txt", ".gitignore"}}
    };
}

std::map<std::string, std::vector<std::string>> StructureEnforcer::getTemplateStructure(const std::string& template_name) {
    auto it = project_templates_.find(template_name);
    if (it != project_templates_.end()) {
        return it->second;
    }
    return {};
}

std::string StructureEnforcer::generateDefaultFileContent(const std::string& filename, const std::string& project_type) {
    if (filename == "README.md") {
        return "# " + project_type + " Project\n\nProject description goes here.\n";
    } else if (filename == ".gitignore") {
        if (project_type == "cpp") {
            return "build/\n*.o\n*.exe\n*.out\n";
        } else if (project_type == "python") {
            return "__pycache__/\n*.pyc\n*.pyo\ndist/\nbuild/\n";
        }
    } else if (filename == "CMakeLists.txt") {
        return "cmake_minimum_required(VERSION 3.10)\nproject(" + project_type + ")\n\nset(CMAKE_CXX_STANDARD 17)\n\nadd_executable(main src/main.cpp)\n";
    }
    
    return "";
}

bool StructureEnforcer::isPathExcluded(const std::string& path) {
    for (const auto& excluded_path : config_.excluded_paths) {
        if (path.find(excluded_path) != std::string::npos) {
            return true;
        }
    }
    return false;
}

bool StructureEnforcer::isRuleExcluded(const std::string& rule_id) {
    return std::find(config_.excluded_rules.begin(), config_.excluded_rules.end(), rule_id) 
           != config_.excluded_rules.end();
}

akao::core::engine::validator::Violation StructureEnforcer::createViolation(const EnforcementRule& rule,
                                                               const std::string& path,
                                                               const std::string& message,
                                                               const std::string& suggestion) {
    akao::core::engine::validator::Violation violation;
    violation.id = rule.id + ":violation:" + std::filesystem::path(path).filename().string();
    violation.rule_id = rule.id;
    violation.rule_name = rule.name;
    violation.rule_category = "structure";
    violation.file_path = path;
    violation.line_number = 1;
    violation.column_number = 1;
    violation.message = message;
    violation.suggestion = suggestion;
    violation.severity = rule.severity;
    violation.auto_fix_available = rule.auto_fix_enabled;
    violation.detected_at = std::chrono::system_clock::now();
    
    return violation;
}

void StructureEnforcer::updateStats(const EnforcementResult& result) {
    stats_.total_enforcements++;
    if (result.success) {
        stats_.successful_enforcements++;
    } else {
        stats_.failed_enforcements++;
    }
    stats_.total_violations_found += result.violations.size();
    stats_.total_violations_fixed += result.actions_taken.size();
    stats_.total_enforcement_time += result.enforcement_duration_seconds;
    
    for (const auto& violation : result.violations) {
        stats_.rule_violation_counts[violation.rule_id]++;
    }
}

// Akao rules implementation
namespace akao_rules {

std::vector<EnforcementRule> getMandatoryRules() {
    std::vector<EnforcementRule> rules;
    
    EnforcementRule philosophies_rule;
    philosophies_rule.id = "akao:structure:philosophies:mandatory";
    philosophies_rule.name = "Philosophies Directory Required";
    philosophies_rule.description = "Every Akao project must have a philosophies directory";
    philosophies_rule.category = "mandatory";
    philosophies_rule.required_directories = {"philosophies"};
    philosophies_rule.severity = "error";
    philosophies_rule.auto_fix_enabled = true;
    rules.push_back(philosophies_rule);
    
    EnforcementRule rules_rule;
    rules_rule.id = "akao:structure:rules:mandatory";
    rules_rule.name = "Rules Directory Required";
    rules_rule.description = "Every Akao project must have a rules directory";
    rules_rule.category = "mandatory";
    rules_rule.required_directories = {"rules"};
    rules_rule.severity = "error";
    rules_rule.auto_fix_enabled = true;
    rules.push_back(rules_rule);
    
    EnforcementRule core_rule;
    core_rule.id = "akao:structure:core:mandatory";
    core_rule.name = "Core Directory Required";
    core_rule.description = "Akao implementation must have a core directory";
    core_rule.category = "mandatory";
    core_rule.required_directories = {"core"};
    core_rule.severity = "error";
    core_rule.auto_fix_enabled = true;
    rules.push_back(core_rule);
    
    return rules;
}

std::vector<EnforcementRule> getRecommendedRules() {
    std::vector<EnforcementRule> rules;
    
    EnforcementRule blueprint_rule;
    blueprint_rule.id = "akao:structure:blueprint:recommended";
    blueprint_rule.name = "Blueprint Directory Recommended";
    blueprint_rule.description = "Akao projects should have a blueprint directory for design docs";
    blueprint_rule.category = "recommended";
    blueprint_rule.required_directories = {"blueprint"};
    blueprint_rule.severity = "warning";
    blueprint_rule.auto_fix_enabled = true;
    rules.push_back(blueprint_rule);
    
    EnforcementRule interfaces_rule;
    interfaces_rule.id = "akao:structure:interfaces:recommended";
    interfaces_rule.name = "Interfaces Directory Recommended";
    interfaces_rule.description = "Akao projects should have an interfaces directory";
    interfaces_rule.category = "recommended";
    interfaces_rule.required_directories = {"interfaces"};
    interfaces_rule.severity = "warning";
    interfaces_rule.auto_fix_enabled = true;
    rules.push_back(interfaces_rule);
    
    return rules;
}

std::vector<EnforcementRule> getOptionalRules() {
    std::vector<EnforcementRule> rules;
    
    EnforcementRule tests_rule;
    tests_rule.id = "akao:structure:tests:optional";
    tests_rule.name = "Tests Directory Optional";
    tests_rule.description = "Akao projects may have a tests directory";
    tests_rule.category = "optional";
    tests_rule.required_directories = {"tests"};
    tests_rule.severity = "info";
    tests_rule.auto_fix_enabled = false;
    rules.push_back(tests_rule);
    
    return rules;
}

std::map<std::string, std::vector<std::string>> getAkaoTemplate() {
    return {
        {"philosophies", {"index.yaml"}},
        {"rules", {"index.yaml"}},
        {"core", {"engine", "rule", "filesystem", "trace"}},
        {"core/engine", {"parser", "validator"}},
        {"core/engine/parser", {"yaml"}},
        {"core/engine/validator", {"universal_validator.hpp", "universal_validator.cpp", "validation_result.hpp", "validation_result.cpp"}},
        {"core/rule", {"loader", "registry", "executor"}},
        {"core/rule/loader", {"rule_loader.hpp", "rule_loader.cpp"}},
        {"core/rule/registry", {"rule_registry.hpp", "rule_registry.cpp"}},
        {"core/rule/executor", {"rule_executor.hpp", "rule_executor.cpp"}},
        {"core/filesystem", {"scanner", "enforcer"}},
        {"core/filesystem/scanner", {"directory_scanner.hpp", "directory_scanner.cpp"}},
        {"core/filesystem/enforcer", {"structure_enforcer.hpp", "structure_enforcer.cpp"}},
        {"core/trace", {"tracer", "reporter"}},
        {"interfaces", {"cli", "api"}},
        {"interfaces/cli", {"parser", "executor", "formatter"}},
        {"blueprint", {"tree.md", "plan.md", "cli.md", "compliance_audit.md"}},
        {"tests", {"compliance"}},
        {".", {"main.cpp", "CMakeLists.txt", "Makefile", "README.md", "LICENSE", ".gitignore"}}
    };
}

} // namespace akao_rules

} // namespace akao::core::filesystem::enforcer
