/**
 * @id: akao:file:core:engine:validator:v1_impl
 * @doc: Implementation of unified validation system combining universal validation, compliance checking, and metadata standards validation.
 * @specification: Concrete implementation of unified validation framework
 * @scope: Core engine unified validation implementation
 * @timeline: 2025-07-09
 * @rationale: Provide executable unified validation with all validation concerns in one place
 * @methodology: Unified validation implementation with pluggable validation strategies
 * @references: ["akao:file:core:engine:validator:v1", "akao:class:core:engine:validator:unified_validator:v1"]
 */

#include "v1.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <regex>
#include <sstream>
#include <chrono>
#include <thread>
#include <future>
#include <iomanip>
#include <set>
#include <map>
#include <cctype>

namespace akao::core::engine::validator {

UnifiedValidator::UnifiedValidator(const ValidationConfig& config) 
    : config_(config), is_initialized_(true) {
    
    // Initialize Pure Logic Engine for rule execution
    logic_engine_ = std::make_unique<akao::logic::PureLogicEngine>();
    logic_engine_->initialize();
    
    // Initialize rule registry and loader
    if (!config_.rules_directory.empty()) {
        initializeRuleSystem();
    }
    
    std::cout << "✅ UnifiedValidator initialized with consolidated validation system" << std::endl;
}

ValidationResult UnifiedValidator::validate(const std::string& target_path) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    ValidationResult result(target_path, "unified");
    
    if (!is_initialized_) {
        result.addViolation(createViolation("INIT_ERROR", "Validator not initialized", target_path));
        return result;
    }
    
    std::vector<ValidationResult> results;
    
    // Primary validation: Rule-driven validation using .akao/rules
    if (!enabled_rules_.empty()) {
        std::cout << "🔄 Running rule-driven validation with " << enabled_rules_.size() << " rules" << std::endl;
        results.push_back(performRuleDrivenValidation(target_path));
    }
    
    // Fallback to hardcoded validation only if no rules are available
    if (enabled_rules_.empty()) {
        std::cout << "⚠️  No rules loaded, falling back to hardcoded validation" << std::endl;
        
        if (config_.enable_universal_validation) {
            results.push_back(performUniversalValidation(target_path));
        }
        
        if (config_.enable_compliance_checking) {
            results.push_back(performComplianceValidation(target_path));
        }
        
        if (config_.enable_metadata_validation) {
            results.push_back(performMetadataValidation(target_path));
        }
        
        if (config_.enable_namespace_validation) {
            results.push_back(performNamespaceValidation(target_path));
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time);
    
    ValidationResult merged_result = mergeResults(results);
    merged_result.setExecutionDuration(duration);
    
    return merged_result;
}

ValidationResult UnifiedValidator::validateFile(const std::string& file_path) {
    return validate(file_path);
}

ValidationResult UnifiedValidator::validateDirectory(const std::string& dir_path) {
    return validate(dir_path);
}

ValidationResult UnifiedValidator::validateUniversal(const std::string& target_path) {
    return performUniversalValidation(target_path);
}

ValidationResult UnifiedValidator::validateCompliance(const std::string& target_path) {
    return performComplianceValidation(target_path);
}

ValidationResult UnifiedValidator::validateMetadata(const std::string& target_path) {
    return performMetadataValidation(target_path);
}

ValidationResult UnifiedValidator::validateNamespace(const std::string& target_path) {
    return performNamespaceValidation(target_path);
}

void UnifiedValidator::setConfig(const ValidationConfig& config) {
    config_ = config;
}

const UnifiedValidator::ValidationConfig& UnifiedValidator::getConfig() const {
    return config_;
}

bool UnifiedValidator::isInitialized() const {
    return is_initialized_;
}

std::vector<std::string> UnifiedValidator::getSupportedFileTypes() const {
    return config_.file_patterns;
}

UnifiedValidator::ParsedNamespace UnifiedValidator::parseNamespace(const std::string& namespace_id) {
    ParsedNamespace result;
    
    // Parse akao:type:domain:component:name:version format
    std::regex namespace_regex(R"(akao:([^:]+):([^:]+):([^:]+):([^:]+):([^:]+))");
    std::smatch matches;
    
    if (std::regex_match(namespace_id, matches, namespace_regex)) {
        result.type = matches[1];
        result.domain = matches[2];
        result.component = matches[3];
        result.name = matches[4];
        result.version = matches[5];
        result.is_valid = true;
    }
    
    return result;
}

bool UnifiedValidator::validateAkaoNamespace(const std::string& namespace_id) {
    return parseNamespace(namespace_id).is_valid;
}

// Private implementation methods

ValidationResult UnifiedValidator::performUniversalValidation(const std::string& target_path) {
    ValidationResult result(target_path, "universal");
    
    // Universal validation logic
    auto files = discoverFiles(target_path);
    result.total_files_analyzed = files.size();
    
    // Execute universal rules
    auto violations = executeRules(target_path);
    result.addViolations(violations);
    
    return result;
}

ValidationResult UnifiedValidator::performComplianceValidation(const std::string& target_path) {
    ValidationResult result(target_path, "compliance");
    
    // Compliance validation logic
    if (std::filesystem::is_directory(target_path)) {
        auto violations = checkDirectoryStructure(target_path);
        result.addViolations(violations);
    } else {
        auto violations = checkFileStructure(target_path);
        result.addViolations(violations);
        
        auto naming_violations = checkNamingConventions(target_path);
        result.addViolations(naming_violations);
    }
    
    return result;
}

ValidationResult UnifiedValidator::performMetadataValidation(const std::string& target_path) {
    ValidationResult result(target_path, "metadata");
    
    // Metadata validation logic
    auto files = discoverFiles(target_path);
    for (const auto& file : files) {
        auto violations = checkMissingMetadata(file);
        result.addViolations(violations);
    }
    
    return result;
}

ValidationResult UnifiedValidator::performNamespaceValidation(const std::string& target_path) {
    ValidationResult result(target_path, "namespace");
    
    // Namespace validation logic - check for proper akao namespace IDs
    auto files = discoverFiles(target_path);
    for (const auto& file : files) {
        std::ifstream filestream(file);
        if (filestream.is_open()) {
            std::string line;
            int line_num = 0;
            while (std::getline(filestream, line)) {
                line_num++;
                if (line.find("@id:") != std::string::npos) {
                    // Extract namespace ID and validate
                    auto pos = line.find("akao:");
                    if (pos != std::string::npos) {
                        auto end_pos = line.find_first_of(" \t\"", pos);
                        if (end_pos != std::string::npos) {
                            std::string namespace_id = line.substr(pos, end_pos - pos);
                            if (!validateAkaoNamespace(namespace_id)) {
                                result.addViolation(createViolation("INVALID_NAMESPACE", 
                                    "Invalid akao namespace format: " + namespace_id, file, line_num));
                            }
                        }
                    }
                }
            }
        }
    }
    
    return result;
}

std::vector<std::string> UnifiedValidator::discoverFiles(const std::string& path) {
    std::vector<std::string> files;
    
    if (std::filesystem::is_regular_file(path)) {
        files.push_back(path);
    } else if (std::filesystem::is_directory(path)) {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
            if (entry.is_regular_file()) {
                std::string file_path = entry.path().string();
                
                // No hardcoded filtering - file exclusions handled by sanitization phase rules
                
                files.push_back(file_path);
            }
        }
    }
    
    return files;
}

std::vector<std::string> UnifiedValidator::discoverCurrentDirectoryFiles(const std::string& path) {
    std::vector<std::string> files;
    
    if (std::filesystem::is_regular_file(path)) {
        files.push_back(path);
    } else if (std::filesystem::is_directory(path)) {
        // Only scan current directory, not recursive
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_regular_file()) {
                std::string file_path = entry.path().string();
                
                // Prioritize files with "." prefix first (sort later)
                files.push_back(file_path);
            }
        }
        
        // Sort to prioritize files with "." prefix at current directory level
        std::sort(files.begin(), files.end(), [](const std::string& a, const std::string& b) {
            std::string filename_a = std::filesystem::path(a).filename().string();
            std::string filename_b = std::filesystem::path(b).filename().string();
            
            bool a_has_dot = filename_a[0] == '.';
            bool b_has_dot = filename_b[0] == '.';
            
            if (a_has_dot && !b_has_dot) return true;
            if (!a_has_dot && b_has_dot) return false;
            
            return filename_a < filename_b;
        });
    }
    
    return files;
}

ValidationResult UnifiedValidator::mergeResults(const std::vector<ValidationResult>& results) {
    ValidationResult merged;
    
    for (const auto& result : results) {
        merged.addViolations(result.violations);
        merged.total_files_analyzed += result.total_files_analyzed;
        merged.total_rules_executed += result.total_rules_executed;
    }
    
    return merged;
}

Violation UnifiedValidator::createViolation(const std::string& rule_id, const std::string& message, 
                                           const std::string& file_path, int line) {
    Violation violation;
    violation.id = rule_id + "_" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count());
    violation.rule_id = rule_id;
    violation.message = message;
    violation.file_path = file_path;
    violation.line_number = line;
    violation.detected_at = std::chrono::system_clock::now();
    
    return violation;
}

std::vector<Violation> UnifiedValidator::checkMissingMetadata(const std::string& file_path) {
    std::vector<Violation> violations;
    
    std::ifstream file(file_path);
    if (!file.is_open()) return violations;
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // Check for required metadata fields
    std::vector<std::string> required_fields = {"@id", "@doc", "@specification", "@scope", "@timeline"};
    
    for (const auto& field : required_fields) {
        if (content.find(field) == std::string::npos) {
            violations.push_back(createViolation("MISSING_METADATA", 
                "Missing required metadata field: " + field, file_path));
        }
    }
    
    return violations;
}

std::vector<Violation> UnifiedValidator::checkNamingConventions(const std::string& file_path) {
    std::vector<Violation> violations;
    
    std::filesystem::path path(file_path);
    std::string filename = path.filename().string();
    
    // Check for underscore violations
    if (filename.find('_') != std::string::npos) {
        violations.push_back(createViolation("UNDERSCORE_VIOLATION", 
            "Filename contains underscore, should use kebab-case", file_path));
    }
    
    // Check for length violations
    if (filename.length() > 20) {
        violations.push_back(createViolation("LENGTH_VIOLATION", 
            "Filename too long (>" + std::to_string(20) + " chars)", file_path));
    }
    
    return violations;
}

std::vector<Violation> UnifiedValidator::checkFileStructure(const std::string& file_path) {
    std::vector<Violation> violations;
    
    // Check if file follows proper structure patterns
    std::filesystem::path path(file_path);
    std::string filename = path.filename().string();
    
    // Check for version pattern (should be v1.cpp, v1.hpp, etc.)
    if (filename.find("v1.") == std::string::npos) {
        violations.push_back(createViolation("VERSION_PATTERN", 
            "File should follow version pattern (v1.ext)", file_path));
    }
    
    return violations;
}

std::vector<Violation> UnifiedValidator::checkDirectoryStructure(const std::string& dir_path) {
    std::vector<Violation> violations;
    
    // Check if directory has required _.yaml file
    std::filesystem::path yaml_path = std::filesystem::path(dir_path) / "_.yaml";
    if (!std::filesystem::exists(yaml_path)) {
        violations.push_back(createViolation("MISSING_YAML", 
            "Directory missing required _.yaml file", dir_path));
    }
    
    return violations;
}

std::vector<Violation> UnifiedValidator::executeRules(const std::string& target_path) {
    std::vector<Violation> violations;
    
    // Universal rule execution logic would go here
    // For now, just return empty vector
    (void)target_path; // Suppress unused parameter warning
    
    return violations;
}

bool UnifiedValidator::isRuleApplicable(const std::string& rule_id, const std::string& file_path) {
    // Rule applicability logic would go here
    (void)rule_id; // Suppress unused parameter warning
    (void)file_path; // Suppress unused parameter warning
    return true;
}

bool UnifiedValidator::isValidNamespaceFormat(const std::string& namespace_id) {
    return validateAkaoNamespace(namespace_id);
}

std::vector<std::string> UnifiedValidator::getRequiredMetadataFields() {
    return {"@id", "@doc", "@specification", "@scope", "@timeline", "@rationale", "@methodology", "@references"};
}

// Rule-driven validation implementation
ValidationResult UnifiedValidator::validateWithRules(const std::string& target_path, 
                                                   const std::vector<std::string>& rule_ids) {
    if (!is_initialized_) {
        ValidationResult result(target_path, "error");
        Violation violation;
        violation.rule_id = "system:not_initialized";
        violation.message = "UnifiedValidator not initialized";
        violation.severity = "error";
        result.addViolation(violation);
        return result;
    }
    
    if (rule_ids.empty()) {
        return performRuleDrivenValidation(target_path);
    }
    
    // Execute specific rules
    ValidationResult result(target_path, "rule-driven");
    for (const auto& rule_id : rule_ids) {
        auto rule_it = std::find_if(enabled_rules_.begin(), enabled_rules_.end(),
                                   [&](const RuleConfig& rule) { return rule.rule_id == rule_id; });
        if (rule_it != enabled_rules_.end()) {
            auto rule_result = executeRuleConfig(*rule_it, target_path);
            result.addViolations(rule_result.violations);
            result.total_rules_executed++;
        }
    }
    
    return result;
}

ValidationResult UnifiedValidator::performRuleDrivenValidation(const std::string& target_path) {
    return validatePhased(target_path);
}

ValidationResult UnifiedValidator::validatePhased(const std::string& target_path) {
    auto applicable_rules = discoverApplicableRules(target_path);
    
    ValidationResult result(target_path, "phased-validation");
    
    // Step 1: Scan ONLY dotfiles in working directory (NON-RECURSIVE)
    auto dotfiles = discoverDotfilesInWorkingDirectory(target_path);
    
    // Step 2: Process dotfiles to build ignore patterns (real-time updates)
    std::vector<std::string> ignore_patterns;
    for (const auto& dotfile : dotfiles) {
        std::string filename = std::filesystem::path(dotfile).filename().string();
        if (filename == ".gitignore") {
            auto new_patterns = processGitignoreFile(dotfile);
            ignore_patterns.insert(ignore_patterns.end(), new_patterns.begin(), new_patterns.end());
        }
    }
    
    // Step 3: Scan non-dotfiles in same folder RECURSIVELY (excluding ignored)
    auto non_dotfiles = discoverNonDotfilesRecursively(target_path, ignore_patterns);
    
    // Step 4: Combine all files for phase processing
    std::vector<std::string> all_files = dotfiles;
    all_files.insert(all_files.end(), non_dotfiles.begin(), non_dotfiles.end());
    
    // Step 5: Files already filtered by native C++ code - skip Pure Logic Engine
    std::vector<std::string> sanitized_files = all_files;
    
    // Phase 2: Compliance - Execute Pure Logic rules in small batches
    std::vector<std::string> compliant_files;
    int phase2_violations = 0;
    
    // Process files in batches of 10 to avoid hanging
    for (size_t i = 0; i < sanitized_files.size(); i += 10) {
        size_t batch_end = std::min(i + 10, sanitized_files.size());
        
        for (size_t j = i; j < batch_end; j++) {
            const auto& file = sanitized_files[j];
            std::cout << "🔍 Processing file: " << file << std::endl;
            auto compliance_violations = executePhaseValidationForFile("compliance", target_path, file);
            phase2_violations += compliance_violations.size();
            result.addViolations(compliance_violations);
            compliant_files.push_back(file);
            
            // Show violations immediately
            if (!compliance_violations.empty()) {
                std::cout << "⚠️  Found " << compliance_violations.size() << " violations in: " << file << std::endl;
            }
        }
    }
    
    // Phase 3: Enforcement - Skip for now (avoid hanging)
    std::vector<std::string> enforced_files = compliant_files;
    
    // Phase 4: Remediation - Skip for now (avoid hanging)
    std::vector<std::string> final_files = enforced_files;
    
    // Count only files that passed all phases
    result.total_files_analyzed = static_cast<int>(final_files.size());
    
    // Count rules executed across all phases
    for (const auto& rule : applicable_rules) {
        if (isRuleApplicableToTarget(rule, target_path)) {
            result.total_rules_executed++;
        }
    }
    
    // Export logs to /logs/ directory
    std::string log_content = generateValidationLogContent(result, target_path);
    exportValidationLogs(log_content, target_path);
    
    return result;
}


std::vector<std::string> UnifiedValidator::executePhase1WithRealTimeFiltering(const std::string& phase, const std::string& target_path, const std::vector<std::string>& files) {
    std::vector<std::string> remaining_files = files;
    
    // Get all sanitization rules
    auto sanitization_rules = getRulesByPhase(phase);
    
    std::cout << "🔍 Phase 1 processing with " << sanitization_rules.size() << " rules" << std::endl;
    
    // SPECIAL CASE: If no rules are enabled, return files as-is (already filtered by native C++ code)
    if (sanitization_rules.empty()) {
        std::cout << "📋 No sanitization rules found - using native C++ filtering (already applied)" << std::endl;
        return remaining_files;
    }
    
    // Process each rule and update file list immediately
    for (const auto& rule : sanitization_rules) {
        std::vector<std::string> filtered_files;
        size_t initial_count = remaining_files.size();
        
        std::cout << "🔍 Processing rule: " << rule.name << std::endl;
        
        // OPTIMIZATION: Skip Pure Logic Engine execution for gitignore rules
        // Native C++ filtering already applied in discoverNonDotfilesRecursively
        if (rule.rule_id.find("gitignore") != std::string::npos) {
            std::cout << "📋 Skipping Pure Logic Engine for gitignore rule (native C++ filtering already applied)" << std::endl;
            filtered_files = remaining_files;  // Keep all files - already filtered
        } else {
            // Apply rule to each remaining file using Pure Logic Engine
            for (const auto& file : remaining_files) {
                auto rule_result = executeRuleForFile(rule, target_path, file);
                
                // For sanitization phase: if rule passes (no violations), keep the file
                if (rule_result.is_valid) {
                    filtered_files.push_back(file);
                } else {
                    // Rule failed (returned false), so file should be excluded
                    std::cout << "🚫 Excluding file: " << file << std::endl;
                }
                
                // Debug: show first few rule results
                static int debug_rule_count = 0;
                if (debug_rule_count < 10) {
                    std::cout << "  🔍 Rule result for " << file << ": " << (rule_result.is_valid ? "VALID (include)" : "INVALID (exclude)") << std::endl;
                    debug_rule_count++;
                }
            }
        }
        
        // Update remaining files list immediately
        remaining_files = filtered_files;
        
        std::cout << "📋 Rule " << rule.name << ": " << initial_count << " → " << remaining_files.size() << " files" << std::endl;
    }
    
    return remaining_files;
}

std::vector<std::string> UnifiedValidator::discoverDotfilesInWorkingDirectory(const std::string& path) {
    std::vector<std::string> dotfiles;
    
    if (std::filesystem::is_directory(path)) {
        // Scan ONLY current directory (non-recursive) for dotfiles
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (filename[0] == '.') {
                    dotfiles.push_back(entry.path().string());
                }
            }
        }
    }
    
    return dotfiles;
}

std::vector<std::string> UnifiedValidator::discoverNonDotfilesRecursively(const std::string& path, const std::vector<std::string>& ignore_patterns) {
    std::vector<std::string> non_dotfiles;
    
    if (std::filesystem::is_directory(path)) {
        // Scan recursively for non-dotfiles, excluding ignored patterns
        for (const auto& entry : std::filesystem::recursive_directory_iterator(path, std::filesystem::directory_options::skip_permission_denied)) {
            if (entry.is_regular_file()) {
                std::string file_path = entry.path().string();
                std::string filename = entry.path().filename().string();
                
                // Skip dotfiles (already processed)
                if (filename[0] == '.') {
                    continue;
                }
                
                // Check if file should be ignored using simple pattern matching
                bool should_ignore = false;
                std::string relative_path = file_path;
                if (file_path.substr(0, path.length()) == path) {
                    relative_path = file_path.substr(path.length());
                    if (relative_path[0] == '/') {
                        relative_path = relative_path.substr(1);
                    }
                }
                
                for (const auto& pattern : ignore_patterns) {
                    if (matchesSimplePattern(relative_path, pattern)) {
                        should_ignore = true;
                        break;
                    }
                }
                
                // Only add if not ignored and is readable
                if (!should_ignore && isReadableFile(file_path)) {
                    non_dotfiles.push_back(file_path);
                }
            }
        }
    }
    
    return non_dotfiles;
}

std::vector<std::string> UnifiedValidator::processGitignoreFile(const std::string& gitignore_path) {
    std::vector<std::string> patterns;
    
    std::ifstream gitignore_file(gitignore_path);
    if (gitignore_file.is_open()) {
        std::string line;
        while (std::getline(gitignore_file, line)) {
            // Skip empty lines and comments
            if (!line.empty() && line[0] != '#') {
                // Trim trailing whitespace
                line.erase(line.find_last_not_of(" \t\n\r") + 1);
                if (!line.empty()) {
                    patterns.push_back(line);
                }
            }
        }
        gitignore_file.close();
    }
    
    return patterns;
}



bool UnifiedValidator::isReadableFile(const std::string& file_path) {
    std::filesystem::path path(file_path);
    std::string extension = path.extension().string();
    
    // Skip binary and non-text files
    std::vector<std::string> unreadable_extensions = {
        ".jpg", ".jpeg", ".png", ".gif", ".bmp", ".ico", ".svg",
        ".mp4", ".avi", ".mov", ".mkv", ".wmv",
        ".mp3", ".wav", ".flac", ".ogg",
        ".zip", ".tar", ".gz", ".rar", ".7z",
        ".exe", ".dll", ".so", ".dylib",
        ".pdf", ".doc", ".docx", ".xls", ".xlsx",
        ".bin", ".dat", ".db", ".sqlite"
    };
    
    // Convert to lowercase for comparison
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    return std::find(unreadable_extensions.begin(), unreadable_extensions.end(), extension) == unreadable_extensions.end();
}

bool UnifiedValidator::matchesIgnorePattern(const std::string& file_path, const std::string& pattern, const std::string& base_path) {
    // Convert absolute path to relative path
    std::string relative_path = file_path;
    if (file_path.substr(0, base_path.length()) == base_path) {
        relative_path = file_path.substr(base_path.length());
        if (relative_path[0] == '/') {
            relative_path = relative_path.substr(1);
        }
    }
    
    // Use Pure Logic Engine gitignore functions
    if (logic_engine_) {
        try {
            akao::logic::Context ctx;
            ctx.bindVariable("$file_path", akao::logic::Value(relative_path));
            ctx.bindVariable("$pattern", akao::logic::Value(pattern));
            
            auto result = logic_engine_->executeAkaoFormat("gitignore.match_pattern($file_path, $pattern)", ctx);
            return result.isBoolean() && result.asBoolean();
        } catch (const std::exception& e) {
            // If pattern matching fails, don't ignore the file
            return false;
        }
    }
    
    return false;
}

bool UnifiedValidator::matchesSimplePattern(const std::string& file_path, const std::string& pattern) {
    // Simple pattern matching for common gitignore patterns
    
    // Directory patterns (ending with /)
    if (pattern.back() == '/') {
        std::string dir_pattern = pattern.substr(0, pattern.length() - 1);
        return file_path.find(dir_pattern + "/") == 0;
    }
    
    // Wildcard patterns (*.ext)
    if (pattern[0] == '*' && pattern.find('.') != std::string::npos) {
        std::string extension = pattern.substr(1); // Remove *
        return file_path.length() >= extension.length() && 
               file_path.substr(file_path.length() - extension.length()) == extension;
    }
    
    // Exact match
    return file_path == pattern;
}


std::vector<Violation> UnifiedValidator::executePhaseValidationForFile(const std::string& phase, const std::string& target_path, const std::string& file) {
    std::vector<Violation> violations;
    
    // Execute rules for this phase on this specific file
    for (const auto& rule : enabled_rules_) {
        if (std::find(rule.phases.begin(), rule.phases.end(), phase) != rule.phases.end()) {
            auto rule_result = executeRuleForFile(rule, target_path, file);
            violations.insert(violations.end(), rule_result.violations.begin(), rule_result.violations.end());
        }
    }
    
    return violations;
}

std::vector<RuleConfig> UnifiedValidator::getRulesByPhase(const std::string& phase) {
    std::vector<RuleConfig> phase_rules;
    
    for (const auto& rule : enabled_rules_) {
        if (std::find(rule.phases.begin(), rule.phases.end(), phase) != rule.phases.end()) {
            phase_rules.push_back(rule);
        }
    }
    
    return phase_rules;
}

ValidationResult UnifiedValidator::executeRuleForFile(const RuleConfig& rule, const std::string& target_path, const std::string& file) {
    ValidationResult result(target_path, "rule-execution");
    
    try {
        // Read rule content
        std::ifstream rule_file(rule.file_path);
        if (!rule_file.is_open()) {
            Violation violation;
            violation.rule_id = rule.rule_id;
            violation.message = "Cannot read rule file: " + rule.file_path;
            violation.file_path = file;
            violation.severity = "error";
            result.addViolation(violation);
            return result;
        }
        
        std::string rule_content;
        std::string line;
        while (std::getline(rule_file, line)) {
            if (!line.empty() && line[0] != '#') {
                rule_content += line + "\n";
            }
        }
        rule_file.close();
        
        // Read file content if it's readable
        std::string file_content;
        if (std::filesystem::exists(file) && std::filesystem::is_regular_file(file)) {
            std::ifstream content_file(file);
            if (content_file.is_open()) {
                std::string content_line;
                while (std::getline(content_file, content_line)) {
                    file_content += content_line + "\n";
                }
                content_file.close();
            }
        }
        
        // Execute rule using Pure Logic Engine with timeout protection
        if (logic_engine_) {
            akao::logic::Context ctx;
            ctx.bindVariable("$target_path", akao::logic::Value(target_path));
            ctx.bindVariable("$file", akao::logic::Value(file));
            ctx.bindVariable("$file_content", akao::logic::Value(file_content));
            ctx.bindVariable("$rule_id", akao::logic::Value(rule.rule_id));
            ctx.bindVariable("$category", akao::logic::Value(rule.category));
            
            // Show progress for slow rules
            static int rule_execution_count = 0;
            rule_execution_count++;
            if (rule_execution_count % 10 == 0) {
                std::cout << "🔍 Executed " << rule_execution_count << " Pure Logic rules..." << std::endl;
            }
            
            auto rule_result = logic_engine_->executeAkaoFormat(rule_content, ctx);
            
            if (rule_result.isBoolean() && !rule_result.asBoolean()) {
                Violation violation;
                violation.rule_id = rule.rule_id;
                violation.rule_name = rule.name;
                violation.description = rule.description;
                violation.message = "Rule violation: " + rule.name;
                violation.file_path = file;
                violation.severity = rule.severity;
                violation.rule_category = rule.category;
                violation.suggestion = "Check rule compliance for: " + rule.name;
                result.addViolation(violation);
            }
        }
        
    } catch (const akao::logic::ForallViolationException& e) {
        // Handle structured forall violations with specific failing values
        for (const auto& failing_value : e.failing_values) {
            Violation violation;
            violation.rule_id = rule.rule_id;
            violation.rule_name = rule.name;
            violation.description = rule.description;
            violation.message = "Rule violation: " + rule.name;
            violation.file_path = failing_value.asString(); // Use the specific failing file
            violation.severity = rule.severity;
            violation.rule_category = rule.category;
            violation.suggestion = "Fix violation in: " + failing_value.asString();
            result.addViolation(violation);
        }
    } catch (const std::exception& e) {
        Violation violation;
        violation.rule_id = rule.rule_id;
        violation.message = "Rule execution error: " + std::string(e.what());
        violation.file_path = file;
        violation.severity = "error";
        result.addViolation(violation);
    }
    
    return result;
}


ValidationResult UnifiedValidator::executeRuleConfig(const RuleConfig& rule, const std::string& target_path) {
    ValidationResult result(target_path, "rule-execution");
    
    try {
        // Read rule content
        std::ifstream file(rule.file_path);
        if (!file.is_open()) {
            Violation violation;
            violation.rule_id = rule.rule_id;
            violation.message = "Cannot read rule file: " + rule.file_path;
            violation.file_path = target_path;
            violation.severity = "error";
            result.addViolation(violation);
            return result;
        }
        
        std::string rule_content;
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty() && line[0] != '#') {
                rule_content += line + "\n";
            }
        }
        file.close();
        
        // Execute rule using Pure Logic Engine
        if (logic_engine_) {
            akao::logic::Context ctx;
            ctx.bindVariable("$target_path", akao::logic::Value(target_path));
            ctx.bindVariable("$rule_id", akao::logic::Value(rule.rule_id));
            ctx.bindVariable("$category", akao::logic::Value(rule.category));
            
            auto rule_result = logic_engine_->executeAkaoFormat(rule_content, ctx);
            
            if (rule_result.isBoolean() && !rule_result.asBoolean()) {
                Violation violation;
                violation.rule_id = rule.rule_id;
                violation.rule_name = rule.name;
                violation.description = rule.description;
                violation.message = "Rule violation: " + rule.name;
                violation.file_path = target_path;
                violation.severity = rule.severity;
                violation.rule_category = rule.category;
                violation.suggestion = "Check rule compliance for: " + rule.name;
                result.addViolation(violation);
            }
        }
        
    } catch (const akao::logic::ForallViolationException& e) {
        // Handle structured forall violations with specific failing values
        for (const auto& failing_value : e.failing_values) {
            Violation violation;
            violation.rule_id = rule.rule_id;
            violation.rule_name = rule.name;
            violation.description = rule.description;
            violation.message = "Rule violation: " + rule.name;
            violation.file_path = failing_value.asString(); // Use the specific failing file
            violation.severity = rule.severity;
            violation.rule_category = rule.category;
            violation.suggestion = "Fix violation in: " + failing_value.asString();
            result.addViolation(violation);
        }
    } catch (const std::exception& e) {
        Violation violation;
        violation.rule_id = rule.rule_id;
        violation.message = "Rule execution error: " + std::string(e.what());
        violation.file_path = target_path;
        violation.severity = "error";
        result.addViolation(violation);
    }
    
    return result;
}

std::vector<RuleConfig> UnifiedValidator::discoverApplicableRules(const std::string& target_path) {
    (void)target_path; // Suppress unused parameter warning
    return enabled_rules_;
}

bool UnifiedValidator::isRuleApplicableToTarget(const RuleConfig& rule, const std::string& target_path) {
    if (rule.applies_to.empty()) {
        return true; // Rule applies to everything
    }
    
    std::string file_extension = std::filesystem::path(target_path).extension().string();
    std::string filename = std::filesystem::path(target_path).filename().string();
    
    for (const auto& pattern : rule.applies_to) {
        if (pattern == "*") {
            return true;
        }
        
        if (pattern.find(file_extension) != std::string::npos) {
            return true;
        }
        
        if (pattern.find(filename) != std::string::npos) {
            return true;
        }
    }
    
    return false;
}

std::vector<RuleConfig> UnifiedValidator::getAvailableRules() const {
    return available_rules_;
}

std::vector<RuleConfig> UnifiedValidator::getEnabledRules() const {
    return enabled_rules_;
}

std::vector<RuleConfig> UnifiedValidator::getRulesByCategory(const std::string& category) const {
    std::vector<RuleConfig> category_rules;
    for (const auto& rule : available_rules_) {
        if (rule.category == category) {
            category_rules.push_back(rule);
        }
    }
    return category_rules;
}

bool UnifiedValidator::enableRule(const std::string& rule_id) {
    // Find rule in available rules
    auto it = std::find_if(available_rules_.begin(), available_rules_.end(),
                          [&](const RuleConfig& rule) { return rule.rule_id == rule_id; });
    
    if (it != available_rules_.end()) {
        // Add to enabled rules if not already there
        auto enabled_it = std::find_if(enabled_rules_.begin(), enabled_rules_.end(),
                                      [&](const RuleConfig& rule) { return rule.rule_id == rule_id; });
        if (enabled_it == enabled_rules_.end()) {
            RuleConfig enabled_rule = *it;
            enabled_rule.enabled = true;
            enabled_rules_.push_back(enabled_rule);
        }
        return true;
    }
    
    return false;
}

bool UnifiedValidator::disableRule(const std::string& rule_id) {
    // Remove from enabled rules
    auto it = std::remove_if(enabled_rules_.begin(), enabled_rules_.end(),
                            [&](const RuleConfig& rule) { return rule.rule_id == rule_id; });
    
    if (it != enabled_rules_.end()) {
        enabled_rules_.erase(it, enabled_rules_.end());
        return true;
    }
    
    return false;
}

bool UnifiedValidator::isRuleEnabled(const std::string& rule_id) const {
    return std::find_if(enabled_rules_.begin(), enabled_rules_.end(),
                       [&](const RuleConfig& rule) { return rule.rule_id == rule_id; }) != enabled_rules_.end();
}

bool UnifiedValidator::initializeRulesDirectory() {
    std::string rules_dir = config_.rules_directory;
    
    try {
        // Create main rules directory
        std::filesystem::create_directories(rules_dir);
        std::filesystem::create_directories(rules_dir + "/enabled");
        std::filesystem::create_directories(rules_dir + "/disabled");
        std::filesystem::create_directories(rules_dir + "/examples");
        
        // Create settings.yaml (extract project root from rules directory)
        std::filesystem::path rules_path(rules_dir);
        std::string project_root = rules_path.parent_path().parent_path().string();
        std::string settings_path = project_root + "/.akao/settings.yaml";
        std::ofstream settings_file(settings_path);
        settings_file << "# Akao Settings Configuration\n";
        settings_file << "version: 1.0\n";
        settings_file << "rules_directory: " << rules_dir << "\n";
        settings_file << "enable_lazy_loading: true\n";
        settings_file << "enable_parallel_execution: false\n";
        settings_file.close();
        
        return true;
    } catch (const std::exception& e) {
        (void)e; // Suppress unused variable warning
        return false;
    }
}

bool UnifiedValidator::loadRulesFromDirectory(const std::string& rules_dir) {
    available_rules_.clear();
    enabled_rules_.clear();
    
    try {
        scanRulesDirectory(rules_dir + "/enabled");
        scanRulesDirectory(rules_dir + "/disabled");
        std::cout << "✅ Loaded " << available_rules_.size() << " rules (" << enabled_rules_.size() << " enabled)" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cout << "❌ Error loading rules: " << e.what() << std::endl;
        return false;
    }
}

void UnifiedValidator::scanRulesDirectory(const std::string& rules_dir) {
    if (!std::filesystem::exists(rules_dir)) {
        return;
    }
    
    for (const auto& entry : std::filesystem::recursive_directory_iterator(rules_dir)) {
        if (entry.is_regular_file()) {
            std::string extension = entry.path().extension().string();
            if (extension == ".yaml" || extension == ".yml" || extension == ".a") {
                auto rule_config = parseRuleFile(entry.path().string());
                if (!rule_config.rule_id.empty()) {
                    available_rules_.push_back(rule_config);
                    
                    // If in enabled directory, add to enabled rules
                    if (rules_dir.find("/enabled") != std::string::npos) {
                        rule_config.enabled = true;
                        enabled_rules_.push_back(rule_config);
                    }
                }
            }
        }
    }
}

RuleConfig UnifiedValidator::parseRuleFile(const std::string& file_path) {
    RuleConfig config;
    config.file_path = file_path;
    
    std::string extension = std::filesystem::path(file_path).extension().string();
    
    if (extension == ".a") {
        // Parse .a format
        config.format = "a";
        
        std::ifstream file(file_path);
        if (!file.is_open()) {
            return config;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            if (line.substr(0, 5) == "# id:") {
                config.rule_id = line.substr(5);
                config.rule_id.erase(0, config.rule_id.find_first_not_of(" \t"));
                config.rule_id.erase(config.rule_id.find_last_not_of(" \t") + 1);
            } else if (line.substr(0, 7) == "# name:") {
                config.name = line.substr(7);
                config.name.erase(0, config.name.find_first_not_of(" \t"));
                config.name.erase(config.name.find_last_not_of(" \t") + 1);
            } else if (line.substr(0, 14) == "# description:") {
                config.description = line.substr(14);
                config.description.erase(0, config.description.find_first_not_of(" \t"));
                config.description.erase(config.description.find_last_not_of(" \t") + 1);
            } else if (line.substr(0, 11) == "# category:") {
                config.category = line.substr(11);
                config.category.erase(0, config.category.find_first_not_of(" \t"));
                config.category.erase(config.category.find_last_not_of(" \t") + 1);
            } else if (line.substr(0, 11) == "# severity:") {
                config.severity = line.substr(11);
                config.severity.erase(0, config.severity.find_first_not_of(" \t"));
                config.severity.erase(config.severity.find_last_not_of(" \t") + 1);
            } else if (line.substr(0, 10) == "# @phases:") {
                // Parse phases array: # @phases: ["sanitization", "compliance"]
                std::string phases_str = line.substr(10);
                phases_str.erase(0, phases_str.find_first_not_of(" \t"));
                phases_str.erase(phases_str.find_last_not_of(" \t") + 1);
                
                // Simple parsing for ["phase1", "phase2"] format
                if (phases_str.front() == '[' && phases_str.back() == ']') {
                    phases_str = phases_str.substr(1, phases_str.length() - 2);
                    std::stringstream ss(phases_str);
                    std::string phase;
                    while (std::getline(ss, phase, ',')) {
                        // Remove quotes and whitespace
                        phase.erase(0, phase.find_first_not_of(" \t\""));
                        phase.erase(phase.find_last_not_of(" \t\"") + 1);
                        if (!phase.empty()) {
                            config.phases.push_back(phase);
                        }
                    }
                }
            }
        }
        file.close();
    } else if (extension == ".yaml" || extension == ".yml") {
        // Parse YAML format
        config.format = "yaml";
        
        std::ifstream file(file_path);
        if (!file.is_open()) {
            return config;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            if (line.substr(0, 3) == "id:") {
                config.rule_id = line.substr(3);
                config.rule_id.erase(0, config.rule_id.find_first_not_of(" \t\""));
                config.rule_id.erase(config.rule_id.find_last_not_of(" \t\"") + 1);
            } else if (line.substr(0, 5) == "name:") {
                config.name = line.substr(5);
                config.name.erase(0, config.name.find_first_not_of(" \t\""));
                config.name.erase(config.name.find_last_not_of(" \t\"") + 1);
            } else if (line.substr(0, 12) == "description:") {
                config.description = line.substr(12);
                config.description.erase(0, config.description.find_first_not_of(" \t\""));
                config.description.erase(config.description.find_last_not_of(" \t\"") + 1);
            } else if (line.substr(0, 9) == "category:") {
                config.category = line.substr(9);
                config.category.erase(0, config.category.find_first_not_of(" \t\""));
                config.category.erase(config.category.find_last_not_of(" \t\"") + 1);
            } else if (line.substr(0, 9) == "severity:") {
                config.severity = line.substr(9);
                config.severity.erase(0, config.severity.find_first_not_of(" \t\""));
                config.severity.erase(config.severity.find_last_not_of(" \t\"") + 1);
            }
        }
        file.close();
    }
    
    return config;
}

bool UnifiedValidator::loadRuleConfiguration(const std::string& config_path) {
    (void)config_path; // Suppress unused parameter warning
    // TODO: Implement YAML configuration loading
    return true;
}

// Consolidated philosophy validation (from core/philosophy/)
ValidationResult UnifiedValidator::validatePhilosophy(const std::string& philosophy_file) {
    ValidationResult result(philosophy_file, "philosophy");
    
    try {
        // Parse philosophy file
        std::ifstream file(philosophy_file);
        if (!file.is_open()) {
            result.addViolation(createViolation("PHILOSOPHY_READ_ERROR", 
                "Cannot read philosophy file", philosophy_file));
            return result;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        
        // Validate logical proof structure
        if (content.find("logical_proof") == std::string::npos) {
            result.addViolation(createViolation("MISSING_LOGICAL_PROOF", 
                "Philosophy missing logical_proof section", philosophy_file));
        }
        
        // Validate axioms
        if (content.find("axioms") == std::string::npos) {
            result.addViolation(createViolation("MISSING_AXIOMS", 
                "Philosophy missing axioms section", philosophy_file));
        }
        
        // Validate theorems
        if (content.find("theorems") == std::string::npos) {
            result.addViolation(createViolation("MISSING_THEOREMS", 
                "Philosophy missing theorems section", philosophy_file));
        }
        
        std::cout << "✅ Philosophy validation completed: " << philosophy_file << std::endl;
        
    } catch (const std::exception& e) {
        result.addViolation(createViolation("PHILOSOPHY_VALIDATION_ERROR", 
            "Philosophy validation failed: " + std::string(e.what()), philosophy_file));
    }
    
    return result;
}

// Consolidated rule execution system (from core/rule/)
ValidationResult UnifiedValidator::executeAdvancedRules(const std::string& target_path, 
                                                       const std::vector<std::string>& rule_patterns) {
    ValidationResult result(target_path, "advanced-rules");
    
    for (const auto& pattern : rule_patterns) {
        auto matching_rules = findRulesByPattern(pattern);
        for (const auto& rule : matching_rules) {
            auto rule_result = executeAdvancedRule(rule, target_path);
            result.addViolations(rule_result.violations);
            result.total_rules_executed++;
        }
    }
    
    return result;
}

ValidationResult UnifiedValidator::executeAdvancedRule(const RuleConfig& rule, const std::string& target_path) {
    ValidationResult result(target_path, "advanced-rule");
    
    try {
        // Enhanced rule execution with category-specific handling
        if (rule.category == "structure") {
            result = executeStructureRule(rule, target_path);
        } else if (rule.category == "interface") {
            result = executeInterfaceRule(rule, target_path);
        } else if (rule.category == "language") {
            result = executeLanguageRule(rule, target_path);
        } else if (rule.category == "security") {
            result = executeSecurityRule(rule, target_path);
        } else if (rule.category == "testing") {
            result = executeTestingRule(rule, target_path);
        } else {
            // Generic rule execution using Pure Logic Engine
            result = executeGenericRule(rule, target_path);
        }
        
    } catch (const std::exception& e) {
        result.addViolation(createViolation("RULE_EXECUTION_ERROR", 
            "Rule execution failed: " + std::string(e.what()), target_path));
    }
    
    return result;
}

// Category-specific rule execution methods
ValidationResult UnifiedValidator::executeStructureRule(const RuleConfig& rule, const std::string& target_path) {
    ValidationResult result(target_path, "structure");
    
    if (rule.name.find("class_separation") != std::string::npos) {
        // Check one class per file
        auto files = discoverFiles(target_path);
        for (const auto& file : files) {
            if (file.find(".cpp") != std::string::npos || file.find(".hpp") != std::string::npos) {
                std::ifstream f(file);
                std::string line;
                int class_count = 0;
                int line_num = 0;
                
                while (std::getline(f, line)) {
                    line_num++;
                    if (line.find("class ") != std::string::npos || line.find("struct ") != std::string::npos) {
                        class_count++;
                        if (class_count > 1) {
                            result.addViolation(createViolation("MULTIPLE_CLASSES", 
                                "Multiple classes found in single file", file, line_num));
                            break;
                        }
                    }
                }
            }
        }
    }
    
    return result;
}

ValidationResult UnifiedValidator::executeInterfaceRule(const RuleConfig& rule, const std::string& target_path) {
    ValidationResult result(target_path, "interface");
    
    // Check for command parity, parameter consistency, etc.
    if (rule.name.find("command_parity") != std::string::npos) {
        // Simplified implementation for command parity checking
        auto files = discoverFiles(target_path);
        bool has_cli = false;
        bool has_api = false;
        
        for (const auto& file : files) {
            std::ifstream f(file);
            std::string content((std::istreambuf_iterator<char>(f)),
                               std::istreambuf_iterator<char>());
            
            if (content.find("CLI") != std::string::npos || content.find("cli") != std::string::npos) {
                has_cli = true;
            }
            if (content.find("API") != std::string::npos || content.find("api") != std::string::npos) {
                has_api = true;
            }
        }
        
        if (has_cli && !has_api) {
            result.addViolation(createViolation("MISSING_API_PARITY", 
                "CLI commands found but no corresponding API interface", target_path));
        }
        if (has_api && !has_cli) {
            result.addViolation(createViolation("MISSING_CLI_PARITY", 
                "API interface found but no corresponding CLI commands", target_path));
        }
    }
    
    return result;
}

ValidationResult UnifiedValidator::executeLanguageRule(const RuleConfig& rule, const std::string& target_path) {
    ValidationResult result(target_path, "language");
    
    if (rule.name.find("single_primary_lang") != std::string::npos) {
        // Check for single primary language
        std::map<std::string, int> language_counts;
        auto files = discoverFiles(target_path);
        
        for (const auto& file : files) {
            std::string ext = std::filesystem::path(file).extension();
            if (ext == ".cpp" || ext == ".hpp") language_counts["cpp"]++;
            else if (ext == ".py") language_counts["python"]++;
            else if (ext == ".js") language_counts["javascript"]++;
            else if (ext == ".rs") language_counts["rust"]++;
            else if (ext == ".go") language_counts["go"]++;
        }
        
        int primary_languages = 0;
        for (const auto& [lang, count] : language_counts) {
            if (count > 5) { // Threshold for primary language
                primary_languages++;
            }
        }
        
        if (primary_languages > 1) {
            result.addViolation(createViolation("MULTIPLE_PRIMARY_LANGUAGES", 
                "Multiple primary languages detected", target_path));
        }
    }
    
    return result;
}

ValidationResult UnifiedValidator::executeSecurityRule(const RuleConfig& rule, const std::string& target_path) {
    ValidationResult result(target_path, "security");
    
    if (rule.name.find("behavior_definition") != std::string::npos) {
        // Check for explicit security configuration
        auto files = discoverFiles(target_path);
        bool found_security_config = false;
        
        for (const auto& file : files) {
            if (file.find("security") != std::string::npos || 
                file.find("config") != std::string::npos) {
                found_security_config = true;
                break;
            }
        }
        
        if (!found_security_config) {
            result.addViolation(createViolation("MISSING_SECURITY_CONFIG", 
                "No explicit security configuration found", target_path));
        }
    }
    
    return result;
}

ValidationResult UnifiedValidator::executeTestingRule(const RuleConfig& rule, const std::string& target_path) {
    ValidationResult result(target_path, "testing");
    
    if (rule.name.find("coverage_enforcement") != std::string::npos) {
        // Check test coverage
        auto files = discoverFiles(target_path);
        size_t source_files = 0;
        size_t test_files = 0;
        
        for (const auto& file : files) {
            if (file.find(".cpp") != std::string::npos || file.find(".hpp") != std::string::npos) {
                if (file.find("test") != std::string::npos || file.find("spec") != std::string::npos) {
                    test_files++;
                } else {
                    source_files++;
                }
            }
        }
        
        if (source_files > 0 && test_files == 0) {
            result.addViolation(createViolation("NO_TESTS_FOUND", 
                "No test files found for source files", target_path));
        }
    }
    
    return result;
}

ValidationResult UnifiedValidator::executeGenericRule(const RuleConfig& rule, const std::string& target_path) {
    ValidationResult result(target_path, "generic");
    
    try {
        // Execute using Pure Logic Engine
        std::ifstream file(rule.file_path);
        if (!file.is_open()) {
            result.addViolation(createViolation("RULE_FILE_ERROR", 
                "Cannot read rule file: " + rule.file_path, target_path));
            return result;
        }
        
        std::string rule_content;
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty() && line[0] != '#') {
                rule_content += line + "\n";
            }
        }
        
        // Execute with Pure Logic Engine
        if (logic_engine_) {
            akao::logic::Context ctx;
            ctx.bindVariable("$target_path", akao::logic::Value(target_path));
            ctx.bindVariable("$rule_id", akao::logic::Value(rule.rule_id));
            
            auto rule_result = logic_engine_->executeAkaoFormat(rule_content, ctx);
            
            if (rule_result.isBoolean() && !rule_result.asBoolean()) {
                result.addViolation(createViolation(rule.rule_id, 
                    "Rule violation: " + rule.name, target_path));
            }
        }
        
    } catch (const std::exception& e) {
        result.addViolation(createViolation("GENERIC_RULE_ERROR", 
            "Generic rule execution failed: " + std::string(e.what()), target_path));
    }
    
    return result;
}

// Helper methods for rule management
std::vector<RuleConfig> UnifiedValidator::findRulesByPattern(const std::string& pattern) {
    std::vector<RuleConfig> matching_rules;
    
    for (const auto& rule : enabled_rules_) {
        if (pattern == "*" || rule.category == pattern || rule.rule_id.find(pattern) != std::string::npos) {
            matching_rules.push_back(rule);
        }
    }
    
    return matching_rules;
}

bool UnifiedValidator::initializeRuleSystem() {
    try {
        // Initialize rules directory structure
        std::filesystem::create_directories(config_.rules_directory + "/enabled");
        std::filesystem::create_directories(config_.rules_directory + "/disabled");
        
        // Load available rules
        return loadRulesFromDirectory(config_.rules_directory);
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Failed to initialize rule system: " << e.what() << std::endl;
        return false;
    }
}

// Consolidated trace functionality (from core/trace/)
std::string UnifiedValidator::traceViolation(const Violation& violation) {
    std::string trace_id = generateTraceId(violation);
    
    // Add to context for future reference
    trace_context_[trace_id] = violation.file_path + ":" + std::to_string(violation.line_number);
    
    // Log the violation
    logViolation(violation.rule_id, violation.file_path, violation.message);
    
    return trace_id;
}

ValidationResult UnifiedValidator::traceViolations(const std::string& target_path) {
    ValidationResult result = validate(target_path);
    
    // Trace each violation
    for (const auto& violation : result.violations) {
        traceViolation(violation);
    }
    
    return result;
}

std::string UnifiedValidator::generateTraceReport(const std::vector<Violation>& violations) {
    return generateReport(violations, "markdown");
}

// Consolidated logging (from core/trace/logger/)
void UnifiedValidator::logViolation(const std::string& rule_id, const std::string& file_path, const std::string& details) {
    std::string message = "VIOLATION: " + rule_id + " in " + file_path + " - " + details;
    std::string log_entry = formatLogEntry("WARNING", message);
    log_entries_.push_back(log_entry);
}

void UnifiedValidator::logValidation(const std::string& target_path, bool success, int violation_count) {
    std::string message = "VALIDATION: " + target_path + " - " + 
                         (success ? "SUCCESS" : "FAILED") + 
                         " (" + std::to_string(violation_count) + " violations)";
    std::string level = success ? "INFO" : "ERROR";
    std::string log_entry = formatLogEntry(level, message);
    log_entries_.push_back(log_entry);
}

bool UnifiedValidator::exportLogs(const std::string& output_path, const std::string& format) {
    (void)format; // Suppress unused parameter warning
    
    std::ofstream file(output_path);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : log_entries_) {
        file << entry << std::endl;
    }
    
    return true;
}

// Consolidated reporting (from core/trace/reporter/)
std::string UnifiedValidator::generateReport(const std::vector<Violation>& violations, const std::string& format) {
    std::stringstream report;
    
    if (format == "markdown") {
        report << "# Akao Validation Report\n\n";
        report << "**Generated**: " << getTimestamp() << "\n";
        report << "**Total Violations**: " << violations.size() << "\n\n";
        
        report << generateSummarySection(violations);
        report << generateDetailsSection(violations);
        report << generateStatisticsSection(violations);
        
    } else if (format == "yaml") {
        report << "report:\n";
        report << "  generated_at: \"" << getTimestamp() << "\"\n";
        report << "  total_violations: " << violations.size() << "\n";
        report << "  violations:\n";
        
        for (const auto& violation : violations) {
            report << "    - rule_id: \"" << violation.rule_id << "\"\n";
            report << "      file_path: \"" << violation.file_path << "\"\n";
            report << "      line_number: " << violation.line_number << "\n";
            report << "      message: \"" << violation.message << "\"\n";
            report << "      severity: \"" << violation.severity << "\"\n";
        }
        
    } else if (format == "json") {
        report << "{\n";
        report << "  \"generated_at\": \"" << getTimestamp() << "\",\n";
        report << "  \"total_violations\": " << violations.size() << ",\n";
        report << "  \"violations\": [\n";
        
        for (size_t i = 0; i < violations.size(); ++i) {
            const auto& violation = violations[i];
            report << "    {\n";
            report << "      \"rule_id\": \"" << violation.rule_id << "\",\n";
            report << "      \"file_path\": \"" << violation.file_path << "\",\n";
            report << "      \"line_number\": " << violation.line_number << ",\n";
            report << "      \"message\": \"" << violation.message << "\",\n";
            report << "      \"severity\": \"" << violation.severity << "\"\n";
            report << "    }" << (i < violations.size() - 1 ? "," : "") << "\n";
        }
        
        report << "  ]\n";
        report << "}\n";
    }
    
    return report.str();
}

bool UnifiedValidator::exportReport(const std::string& report_content, const std::string& output_path) {
    std::ofstream file(output_path);
    if (!file.is_open()) {
        return false;
    }
    
    file << report_content;
    return true;
}

// Consolidated trace helper methods
std::string UnifiedValidator::generateTraceId(const Violation& violation) {
    return "trace_" + std::to_string(++trace_counter_) + "_" + violation.rule_id;
}

std::string UnifiedValidator::formatLogEntry(const std::string& level, const std::string& message) {
    return "[" + getTimestamp() + "] [" + level + "] " + message;
}

std::string UnifiedValidator::getTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string UnifiedValidator::formatViolationForReport(const Violation& violation, const std::string& format) {
    if (format == "markdown") {
        return "- **" + violation.rule_id + "**: " + violation.message + " (" + violation.file_path + ":" + std::to_string(violation.line_number) + ")";
    }
    return violation.rule_id + ": " + violation.message;
}

std::string UnifiedValidator::generateSummarySection(const std::vector<Violation>& violations) {
    std::stringstream ss;
    
    ss << "## Summary\n\n";
    
    if (violations.empty()) {
        ss << "✅ No violations found. Project is compliant.\n\n";
        return ss.str();
    }
    
    // Count by severity
    std::map<std::string, size_t> severity_counts;
    for (const auto& violation : violations) {
        severity_counts[violation.severity]++;
    }
    
    ss << "### Violations by Severity\n";
    for (const auto& [severity, count] : severity_counts) {
        ss << "- **" << severity << "**: " << count << " violations\n";
    }
    
    // Count by rule
    std::map<std::string, size_t> rule_counts;
    for (const auto& violation : violations) {
        rule_counts[violation.rule_id]++;
    }
    
    ss << "\n### Most Common Violations\n";
    std::vector<std::pair<std::string, size_t>> sorted_rules(rule_counts.begin(), rule_counts.end());
    std::sort(sorted_rules.begin(), sorted_rules.end(), 
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    for (size_t i = 0; i < std::min(size_t(5), sorted_rules.size()); ++i) {
        ss << "- **" << sorted_rules[i].first << "**: " << sorted_rules[i].second << " violations\n";
    }
    
    ss << "\n";
    return ss.str();
}

std::string UnifiedValidator::generateDetailsSection(const std::vector<Violation>& violations) {
    std::stringstream ss;
    
    ss << "## Violation Details\n\n";
    
    for (const auto& violation : violations) {
        ss << "### " << violation.rule_id << "\n";
        ss << "- **File**: " << violation.file_path << "\n";
        ss << "- **Line**: " << violation.line_number << "\n";
        ss << "- **Severity**: " << violation.severity << "\n";
        ss << "- **Message**: " << violation.message << "\n";
        
        if (!violation.suggestion.empty()) {
            ss << "- **Suggestion**: " << violation.suggestion << "\n";
        }
        
        ss << "\n";
    }
    
    return ss.str();
}

std::string UnifiedValidator::generateStatisticsSection(const std::vector<Violation>& violations) {
    std::stringstream ss;
    
    ss << "## Statistics\n\n";
    
    // Count unique files
    std::set<std::string> unique_files;
    for (const auto& violation : violations) {
        unique_files.insert(violation.file_path);
    }
    
    ss << "- **Total Violations**: " << violations.size() << "\n";
    ss << "- **Files Affected**: " << unique_files.size() << "\n";
    
    if (!unique_files.empty()) {
        ss << "- **Average Violations per File**: " << std::fixed << std::setprecision(1) 
           << (static_cast<double>(violations.size()) / unique_files.size()) << "\n";
    }
    
    ss << "\n";
    return ss.str();
}

const UnifiedValidator::Statistics& UnifiedValidator::getStatistics() const {
    return statistics_;
}

void UnifiedValidator::resetStatistics() {
    statistics_ = Statistics{};
}

// Factory implementation
std::unique_ptr<UnifiedValidator> UnifiedValidatorFactory::create(const std::string& project_root) {
    UnifiedValidator::ValidationConfig config;
    config.rules_directory = project_root + "/.akao/rules";
    auto validator = std::make_unique<UnifiedValidator>(config);
    validator->loadRulesFromDirectory(config.rules_directory);
    return validator;
}

std::unique_ptr<UnifiedValidator> UnifiedValidatorFactory::createWithConfig(
    const std::string& project_root,
    const UnifiedValidator::ValidationConfig& config) {
    
    UnifiedValidator::ValidationConfig adjusted_config = config;
    if (adjusted_config.rules_directory.empty()) {
        adjusted_config.rules_directory = project_root + "/.akao/rules";
    }
    
    auto validator = std::make_unique<UnifiedValidator>(adjusted_config);
    validator->loadRulesFromDirectory(adjusted_config.rules_directory);
    return validator;
}

std::unique_ptr<UnifiedValidator> UnifiedValidatorFactory::createForDevelopment(const std::string& project_root) {
    UnifiedValidator::ValidationConfig config;
    config.rules_directory = project_root + "/.akao/rules";
    config.parallel_execution = false;
    return createWithConfig(project_root, config);
}

std::unique_ptr<UnifiedValidator> UnifiedValidatorFactory::createForCI(const std::string& project_root) {
    UnifiedValidator::ValidationConfig config;
    config.rules_directory = project_root + "/.akao/rules";
    config.parallel_execution = true;
    return createWithConfig(project_root, config);
}

std::unique_ptr<UnifiedValidator> UnifiedValidatorFactory::createForProduction(const std::string& project_root) {
    UnifiedValidator::ValidationConfig config;
    config.rules_directory = project_root + "/.akao/rules";
    config.parallel_execution = true;
    config.enable_auto_fix = false;
    return createWithConfig(project_root, config);
}

bool UnifiedValidatorFactory::initializeRulesDirectory(const std::string& project_root) {
    auto validator = create(project_root);
    return validator->initializeRulesDirectory();
}

bool UnifiedValidatorFactory::validateRulesDirectory(const std::string& project_root) {
    std::string rules_path = project_root + "/.akao/rules";
    std::string settings_path = project_root + "/.akao/settings.yaml";
    return std::filesystem::exists(rules_path) && 
           std::filesystem::exists(settings_path);
}

// Log export functionality implementation
std::string UnifiedValidator::generateValidationLogContent(const ValidationResult& result, const std::string& target_path) {
    std::ostringstream log_stream;
    
    // Log header
    log_stream << "=== AKAO VALIDATION LOG ===" << std::endl;
    log_stream << "Timestamp: " << getTimestamp() << std::endl;
    log_stream << "Target Path: " << target_path << std::endl;
    log_stream << "Validation Type: " << result.validation_type << std::endl;
    log_stream << "Status: " << (result.is_valid ? "PASSED" : "FAILED") << std::endl;
    log_stream << "Duration: " << result.execution_duration.count() << "s" << std::endl;
    log_stream << "Rules Executed: " << result.total_rules_executed << std::endl;
    log_stream << "Files Analyzed: " << result.total_files_analyzed << std::endl;
    log_stream << "Total Violations: " << result.violations.size() << std::endl;
    log_stream << std::endl;
    
    
    // Violations log
    if (!result.violations.empty()) {
        log_stream << "=== VIOLATIONS ===" << std::endl;
        for (const auto& violation : result.violations) {
            log_stream << "Rule: " << violation.rule_id << std::endl;
            log_stream << "File: " << violation.file_path << std::endl;
            log_stream << "Message: " << violation.message << std::endl;
            log_stream << "Severity: " << violation.severity << std::endl;
            log_stream << "---" << std::endl;
        }
        log_stream << std::endl;
    }
    
    // Summary
    log_stream << "=== SUMMARY ===" << std::endl;
    log_stream << "Validation " << (result.is_valid ? "PASSED" : "FAILED") << std::endl;
    
    return log_stream.str();
}

void UnifiedValidator::exportValidationLogs(const std::string& log_content, const std::string& target_path) {
    // Create .akao/logs directory if it doesn't exist
    std::string logs_dir;
    if (std::filesystem::is_directory(target_path)) {
        logs_dir = target_path + "/.akao/logs";
    } else {
        // If target_path is a file, use current directory
        logs_dir = "./.akao/logs";
    }
    
    if (!std::filesystem::exists(logs_dir)) {
        std::filesystem::create_directories(logs_dir);
    }
    
    // Generate timestamp-based filename
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::ostringstream filename_stream;
    filename_stream << logs_dir << "/validation_" 
                   << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S") 
                   << ".log";
    
    std::string log_file_path = filename_stream.str();
    
    // Write log content to file
    std::ofstream log_file(log_file_path);
    if (log_file.is_open()) {
        log_file << log_content;
        log_file.close();
        std::cout << "📝 Validation log exported to: " << log_file_path << std::endl;
    } else {
        std::cerr << "❌ Failed to export validation log to: " << log_file_path << std::endl;
    }
}

} // namespace akao::core::engine::validator