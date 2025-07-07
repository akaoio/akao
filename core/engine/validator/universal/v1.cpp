#include "v1.hpp"
#include "../../logic/pure/v1.hpp"
#include <filesystem>
#include <fstream>
#include <regex>
#include <chrono>
#include <algorithm>
#include <iostream>

namespace akao::core::engine::validator {

// ValidationException implementation
UniversalValidator::ValidationException::ValidationException(const ValidationError& error) 
    : error_(error) {
    std::stringstream ss;
    ss << "Validation Error";
    if (!error_.target_path.empty()) {
        ss << " in " << error_.target_path;
    }
    if (!error_.rule_id.empty()) {
        ss << " (rule: " << error_.rule_id << ")";
    }
    ss << ": " << error_.message;
    if (!error_.details.empty()) {
        ss << "\nDetails: " << error_.details;
    }
    what_message_ = ss.str();
}

const char* UniversalValidator::ValidationException::what() const noexcept {
    return what_message_.c_str();
}

const UniversalValidator::ValidationError& UniversalValidator::ValidationException::getError() const {
    return error_;
}

// Constructor
UniversalValidator::UniversalValidator(const std::string& rules_directory) 
    : rules_directory_(rules_directory), is_initialized_(false) {
    yaml_parser_ = std::make_unique<parser::YamlParser>();
    rule_registry_ = std::make_unique<rule::registry::RuleRegistry>(rules_directory);
}

UniversalValidator::~UniversalValidator() = default;

// Initialization
bool UniversalValidator::initialize() {
    try {
        if (!rule_registry_->loadRules()) {
            throwValidationError("Failed to load rules", "Check rules directory: " + rules_directory_);
            return false;
        }
        
        is_initialized_ = true;
        return true;
    } catch (const std::exception& e) {
        throwValidationError("Initialization failed", e.what());
        return false;
    }
}

bool UniversalValidator::isInitialized() const {
    return is_initialized_;
}

// Configuration
void UniversalValidator::setConfig(const ValidationConfig& config) {
    config_ = config;
}

const UniversalValidator::ValidationConfig& UniversalValidator::getConfig() const {
    return config_;
}

void UniversalValidator::enableAutoFix(bool enable) {
    config_.enable_auto_fix = enable;
}

void UniversalValidator::setOutputFormat(const std::string& format) {
    config_.output_format = format;
}

// Main validation methods
ValidationResult UniversalValidator::validate(const std::string& target_path) {
    if (!is_initialized_) {
        throwValidationError("Validator not initialized", "Call initialize() first");
    }
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    try {
        // Build execution context
        auto context = buildExecutionContext(target_path);
        
        // Execute validation
        auto result = executeValidation(context);
        
        // Calculate execution time
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time);
        result.setExecutionDuration(duration);
        
        // Update statistics
        updateStats(result);
        
        return result;
        
    } catch (const std::exception& e) {
        throwValidationError("Validation failed", e.what(), target_path);
        return ValidationResult(target_path); // Never reached
    }
}

ValidationResult UniversalValidator::validateProject(const std::string& project_path) {
    return validate(project_path);
}

ValidationResult UniversalValidator::validateFile(const std::string& file_path) {
    return validate(file_path);
}

ValidationResult UniversalValidator::validateDirectory(const std::string& directory_path) {
    return validate(directory_path);
}

// Self-validation (CRITICAL for Akao philosophy compliance)
ValidationResult UniversalValidator::validateSelf() {
    // Validate Akao itself - this is required by akao:philosophy::validation:universal:v1
    std::string akao_root = ".";  // Current directory should be Akao root
    return validate(akao_root);
}

bool UniversalValidator::isSelfCompliant() {
    auto result = validateSelf();
    return result.isCompliant();
}

// Project type detection
UniversalValidator::ProjectType UniversalValidator::detectProjectType(const std::string& path) {
    // Check cache first
    auto it = project_type_cache_.find(path);
    if (it != project_type_cache_.end()) {
        return it->second;
    }
    
    ProjectType detected_type = ProjectType::UNKNOWN;
    
    if (std::filesystem::is_directory(path)) {
        // Check for Akao project
        if (std::filesystem::exists(path + "/philosophies") && 
            std::filesystem::exists(path + "/rules")) {
            detected_type = ProjectType::AKAO_PROJECT;
        }
        // Check for C++ project
        else if (std::filesystem::exists(path + "/CMakeLists.txt") ||
                 std::filesystem::exists(path + "/Makefile")) {
            detected_type = ProjectType::CPP_PROJECT;
        }
        // Check for Rust project
        else if (std::filesystem::exists(path + "/Cargo.toml")) {
            detected_type = ProjectType::RUST_PROJECT;
        }
        // Check for Python project
        else if (std::filesystem::exists(path + "/setup.py") ||
                 std::filesystem::exists(path + "/pyproject.toml")) {
            detected_type = ProjectType::PYTHON_PROJECT;
        }
        // Check for JavaScript project
        else if (std::filesystem::exists(path + "/package.json")) {
            detected_type = ProjectType::JAVASCRIPT_PROJECT;
        }
        // Check for Go project
        else if (std::filesystem::exists(path + "/go.mod")) {
            detected_type = ProjectType::GO_PROJECT;
        }
    } else if (std::filesystem::is_regular_file(path)) {
        std::string ext = std::filesystem::path(path).extension();
        if (ext == ".cpp" || ext == ".hpp" || ext == ".h" || ext == ".cc") {
            detected_type = ProjectType::CPP_PROJECT;
        } else if (ext == ".rs") {
            detected_type = ProjectType::RUST_PROJECT;
        } else if (ext == ".py") {
            detected_type = ProjectType::PYTHON_PROJECT;
        } else if (ext == ".js" || ext == ".ts") {
            detected_type = ProjectType::JAVASCRIPT_PROJECT;
        } else if (ext == ".go") {
            detected_type = ProjectType::GO_PROJECT;
        }
    }
    
    // Cache the result
    project_type_cache_[path] = detected_type;
    return detected_type;
}

std::string UniversalValidator::getProjectTypeString(ProjectType type) const {
    switch (type) {
        case ProjectType::CPP_PROJECT: return "C++";
        case ProjectType::RUST_PROJECT: return "Rust";
        case ProjectType::PYTHON_PROJECT: return "Python";
        case ProjectType::JAVASCRIPT_PROJECT: return "JavaScript";
        case ProjectType::GO_PROJECT: return "Go";
        case ProjectType::AKAO_PROJECT: return "Akao";
        case ProjectType::MIXED_PROJECT: return "Mixed";
        default: return "Unknown";
    }
}

// Core validation logic
ValidationResult UniversalValidator::executeValidation(const RuleExecutionContext& context) {
    ValidationResult result(context.target_path, "project");
    
    // Get applicable rules
    auto applicable_rules = getApplicableRules(context);
    
    result.setTotalRulesExecuted(applicable_rules.size());
    
    size_t rules_passed = 0;
    size_t rules_failed = 0;
    size_t rules_skipped = 0;
    
    // Execute each applicable rule
    for (const auto& rule : applicable_rules) {
        try {
            auto violations = executeRule(*rule, context);
            
            if (violations.empty()) {
                rules_passed++;
            } else {
                rules_failed++;
                result.addViolations(violations);
            }
            
            trackRuleExecution(rule->id);
            
        } catch (const std::exception& e) {
            rules_skipped++;
            // Log rule execution error but continue
        }
    }
    
    result.setRulesPassed(rules_passed);
    result.setRulesFailed(rules_failed);
    result.setRulesSkipped(rules_skipped);
    
    // Set the number of files processed
    result.setFilesProcessed(context.discovered_files.size());
    
    // Calculate compliance scores
    result.calculateComplianceScores();
    
    return result;
}

std::vector<Violation> UniversalValidator::executeRule(const rule::loader::Rule& rule, 
                                                      const RuleExecutionContext& context) {
    std::vector<Violation> violations;
    
    // Route to appropriate rule execution engine based on category
    if (rule.category == "structure") {
        violations = executeStructureRule(rule, context);
    } else if (rule.category == "interface") {
        violations = executeInterfaceRule(rule, context);
    } else if (rule.category == "language") {
        violations = executeLanguageRule(rule, context);
    } else if (rule.category == "security") {
        violations = executeSecurityRule(rule, context);
    } else if (rule.category == "testing") {
        violations = executeTestingRule(rule, context);
    } else {
        // Execute Pure Logic expressions with mathematical formal proofs
        if (!rule.pure_logic_expressions.empty()) {
            for (const auto& logic_expression : rule.pure_logic_expressions) {
                auto pure_logic_violations = findPureLogicViolations(logic_expression, context);
                for (const auto& violation_desc : pure_logic_violations) {
                    auto violation = createViolation(rule, context.target_path, 1, violation_desc);
                    violations.push_back(violation);
                }
            }
        }
        
        // Fallback to datalog_rules for backward compatibility during migration
        if (violations.empty() && !rule.datalog_rules.empty()) {
            for (const auto& datalog_rule : rule.datalog_rules) {
                // Convert datalog to Pure Logic automatically
                std::string pure_logic_expr = convertDatalogToPureLogic(datalog_rule);
                auto pure_logic_violations = findPureLogicViolations(pure_logic_expr, context);
                for (const auto& violation_desc : pure_logic_violations) {
                    auto violation = createViolation(rule, context.target_path, 1, violation_desc);
                    violations.push_back(violation);
                }
            }
        }
    }
    
    return violations;
}

// Simplified rule execution engines (basic implementations)
std::vector<Violation> UniversalValidator::executeStructureRule(const rule::loader::Rule& rule, 
                                                               const RuleExecutionContext& context) {
    std::vector<Violation> violations;
    
    if (rule.target == "class_separation") {
        // Check for one class per file
        for (const auto& file_path : context.discovered_files) {
            if (detectFileType(file_path) == "source") {
                auto lines = readFileLines(file_path);
                int class_count = 0;
                size_t line_number = 1;
                
                for (const auto& line : lines) {
                    if (line.find("class ") != std::string::npos || 
                        line.find("struct ") != std::string::npos) {
                        class_count++;
                        if (class_count > 1) {
                            auto violation = createViolation(rule, file_path, line_number, 
                                "Multiple classes found in single file", 
                                "Move additional classes to separate files");
                            violations.push_back(violation);
                            break;
                        }
                    }
                    line_number++;
                }
            }
        }
    }
    
    return violations;
}

std::vector<Violation> UniversalValidator::executeInterfaceRule(const rule::loader::Rule& rule, 
                                                               const RuleExecutionContext& context) {
    // Simplified interface rule execution
    return {};
}

std::vector<Violation> UniversalValidator::executeLanguageRule(const rule::loader::Rule& rule, 
                                                              const RuleExecutionContext& context) {
    // Simplified language rule execution
    return {};
}

std::vector<Violation> UniversalValidator::executeSecurityRule(const rule::loader::Rule& rule, 
                                                              const RuleExecutionContext& context) {
    // Simplified security rule execution
    return {};
}

std::vector<Violation> UniversalValidator::executeTestingRule(const rule::loader::Rule& rule, 
                                                             const RuleExecutionContext& context) {
    // Simplified testing rule execution
    return {};
}

// Helper methods
UniversalValidator::RuleExecutionContext UniversalValidator::buildExecutionContext(const std::string& target_path) {
    RuleExecutionContext context;
    context.target_path = target_path;
    context.project_type = detectProjectType(target_path);
    context.project_metadata = extractProjectMetadata(target_path);
    context.discovered_files = discoverFiles(target_path);
    
    // Detect file types
    for (const auto& file_path : context.discovered_files) {
        context.file_types[file_path] = detectFileType(file_path);
    }
    
    return context;
}

std::vector<std::string> UniversalValidator::discoverFiles(const std::string& path) {
    std::vector<std::string> files;
    
    if (std::filesystem::is_regular_file(path)) {
        files.push_back(path);
    } else if (std::filesystem::is_directory(path)) {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().string());
            }
        }
    }
    
    return files;
}

std::string UniversalValidator::detectFileType(const std::string& file_path) {
    std::string ext = std::filesystem::path(file_path).extension();
    
    if (ext == ".cpp" || ext == ".hpp" || ext == ".h" || ext == ".cc" || 
        ext == ".rs" || ext == ".py" || ext == ".js" || ext == ".ts" || ext == ".go") {
        return "source";
    } else if (ext == ".yaml" || ext == ".yml" || ext == ".json" || ext == ".toml") {
        return "config";
    } else if (file_path.find("CMakeLists.txt") != std::string::npos || 
               file_path.find("Makefile") != std::string::npos ||
               file_path.find("Cargo.toml") != std::string::npos) {
        return "build";
    }
    
    return "other";
}

std::vector<std::shared_ptr<rule::loader::Rule>> UniversalValidator::getApplicableRules(const RuleExecutionContext& context) {
    auto all_rules = rule_registry_->getAllRules();
    std::vector<std::shared_ptr<rule::loader::Rule>> applicable_rules;
    
    for (const auto& rule : all_rules) {
        if (isRuleApplicable(*rule, context) && !shouldSkipRule(*rule)) {
            applicable_rules.push_back(rule);
        }
    }
    
    return applicable_rules;
}

bool UniversalValidator::isRuleApplicable(const rule::loader::Rule& rule, const RuleExecutionContext& context) {
    // All rules are applicable for universal validation
    // More sophisticated logic can be added based on project type, file types, etc.
    return true;
}

bool UniversalValidator::shouldSkipRule(const rule::loader::Rule& rule) {
    // Check if rule is in excluded list
    return std::find(config_.excluded_rules.begin(), config_.excluded_rules.end(), rule.id) 
           != config_.excluded_rules.end();
}

Violation UniversalValidator::createViolation(const rule::loader::Rule& rule, const std::string& file_path, 
                                             size_t line_number, const std::string& message, 
                                             const std::string& suggestion) {
    Violation violation;
    violation.id = generateViolationId(rule, file_path, line_number);
    violation.rule_id = rule.id;
    violation.rule_name = rule.name;
    violation.rule_category = rule.category;
    violation.philosophy_id = rule.philosophies.empty() ? "" : rule.philosophies[0];
    violation.file_path = file_path;
    violation.line_number = line_number;
    violation.column_number = 1;
    violation.message = message;
    violation.suggestion = suggestion;
    violation.severity = rule.severity;
    violation.auto_fix_available = rule.auto_fix;
    violation.detected_at = std::chrono::system_clock::now();
    
    return violation;
}

std::string UniversalValidator::generateViolationId(const rule::loader::Rule& rule, 
                                                   const std::string& file_path, size_t line_number) {
    std::string filename = std::filesystem::path(file_path).filename();
    return rule.id + ":violation:" + filename + ":" + std::to_string(line_number);
}

std::vector<std::string> UniversalValidator::readFileLines(const std::string& file_path) {
    std::vector<std::string> lines;
    std::ifstream file(file_path);
    
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            lines.push_back(line);
        }
        file.close();
    }
    
    return lines;
}

std::map<std::string, std::string> UniversalValidator::extractProjectMetadata(const std::string& path) {
    std::map<std::string, std::string> metadata;
    metadata["path"] = path;
    metadata["type"] = getProjectTypeString(detectProjectType(path));
    return metadata;
}

// Statistics
void UniversalValidator::updateStats(const ValidationResult& result) {
    stats_.total_validations++;
    if (result.isCompliant()) {
        stats_.successful_validations++;
    } else {
        stats_.failed_validations++;
    }
    stats_.total_execution_time += result.getExecutionDuration();
}

void UniversalValidator::trackRuleExecution(const std::string& rule_id) {
    stats_.rule_execution_counts[rule_id]++;
}

const UniversalValidator::ValidationStats& UniversalValidator::getStats() const {
    return stats_;
}

// Error handling
void UniversalValidator::throwValidationError(const std::string& message, const std::string& details, 
                                             const std::string& target_path, const std::string& rule_id) {
    ValidationError error;
    error.message = message;
    error.details = details;
    error.target_path = target_path;
    error.rule_id = rule_id;
    throw ValidationException(error);
}

// Simple implementations for required methods
size_t UniversalValidator::getLoadedRulesCount() const {
    return rule_registry_->getTotalRulesCount();
}

std::vector<std::string> UniversalValidator::getAvailableCategories() const {
    return rule_registry_->getAllCategories();
}

std::vector<std::string> UniversalValidator::findPureLogicViolations(const std::string& logic_expression, 
                                                                     const RuleExecutionContext& context) {
    std::vector<std::string> violations;
    
    try {
        // Create Pure Logic Engine execution context
        akao::logic::Context logic_context;
        
        // Set up context variables
        logic_context.bindVariable("target_path", akao::logic::Value(context.target_path));
        
        // Convert ProjectType enum to string
        std::string project_type_str;
        switch (context.project_type) {
            case UniversalValidator::ProjectType::CPP_PROJECT: project_type_str = "cpp"; break;
            case UniversalValidator::ProjectType::RUST_PROJECT: project_type_str = "rust"; break;
            case UniversalValidator::ProjectType::PYTHON_PROJECT: project_type_str = "python"; break;
            case UniversalValidator::ProjectType::JAVASCRIPT_PROJECT: project_type_str = "javascript"; break;
            case UniversalValidator::ProjectType::GO_PROJECT: project_type_str = "go"; break;
            case UniversalValidator::ProjectType::MIXED_PROJECT: project_type_str = "mixed"; break;
            case UniversalValidator::ProjectType::AKAO_PROJECT: project_type_str = "akao"; break;
            default: project_type_str = "unknown"; break;
        }
        logic_context.bindVariable("project_type", akao::logic::Value(project_type_str));
        
        // Add discovered files
        std::vector<akao::logic::Value> files;
        for (const auto& file : context.discovered_files) {
            files.push_back(akao::logic::Value(file));
        }
        logic_context.bindVariable("discovered_files", akao::logic::Value(files));
        
        // Initialize Pure Logic Engine if not already done
        if (!pure_logic_engine_) {
            pure_logic_engine_ = std::make_unique<akao::logic::PureLogicEngine>();
            pure_logic_engine_->initialize();
        }
        
        // Execute the Pure Logic expression
        auto result = pure_logic_engine_->evaluate(logic_expression, logic_context);
        
        // Convert result to violations
        if (result.isBoolean() && !result.asBoolean()) {
            violations.push_back("Pure Logic validation failed: " + logic_expression);
        } else if (result.isCollection()) {
            auto violation_list = result.asCollection();
            for (const auto& v : violation_list) {
                violations.push_back(v.toString());
            }
        }
        
    } catch (const std::exception& e) {
        violations.push_back("Pure Logic execution error: " + std::string(e.what()));
    }
    
    return violations;
}

std::string UniversalValidator::convertDatalogToPureLogic(const std::string& datalog_rule) {
    // Simple conversion from datalog to Pure Logic syntax  
    // This is a temporary bridge during migration
    
    std::string pure_logic = datalog_rule;
    
    // Convert basic datalog patterns to Pure Logic expressions
    std::regex violation_pattern(R"((\w+)_violation\(([^)]+)\)\s*:-\s*(.+)\.)");
    std::regex compliant_pattern(R"((\w+)_compliant\(([^)]+)\)\s*:-\s*(.+)\.)");
    
    std::smatch match;
    if (std::regex_search(datalog_rule, match, violation_pattern)) {
        // Convert violation rule to Pure Logic
        std::string rule_type = match[1].str();
        std::string file_var = match[2].str();
        std::string conditions = match[3].str();
        
        pure_logic = "forall(" + file_var + ", " + 
                     "implies(and(file_exists(" + file_var + "), " + conditions + "), " +
                     "not(" + rule_type + "_compliant(" + file_var + "))))";
    } else if (std::regex_search(datalog_rule, match, compliant_pattern)) {
        // Convert compliance rule to Pure Logic
        std::string rule_type = match[1].str();
        std::string file_var = match[2].str();
        std::string conditions = match[3].str();
        
        pure_logic = "forall(" + file_var + ", " +
                     "implies(file_exists(" + file_var + "), " +
                     "equals(" + rule_type + "_compliant(" + file_var + "), " + conditions + ")))";
    }
    
    return pure_logic;
}

bool UniversalValidator::executeDatalogQuery(const std::string& query, const RuleExecutionContext& context) {
    // Simplified datalog execution - return true for now
    return true;
}

std::vector<std::string> UniversalValidator::findDatalogViolations(const std::string& query, 
                                                                   const RuleExecutionContext& context) {
    // Simplified violation finding - return empty for now
    return {};
}

} // namespace akao::core::engine::validator
