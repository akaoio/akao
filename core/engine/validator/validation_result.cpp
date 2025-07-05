#include "validation_result.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <iostream>

namespace akao::core::engine::validator {

// Violation helper methods
std::string Violation::toString() const {
    std::stringstream ss;
    ss << "[" << severity << "] " << rule_name << " in " << file_path << ":" << line_number;
    ss << " - " << message;
    if (auto_fix_available) {
        ss << " (auto-fixable)";
    }
    return ss.str();
}

std::string Violation::toYaml() const {
    std::stringstream ss;
    ss << "- id: " << id << "\n";
    ss << "  rule_id: " << rule_id << "\n";
    ss << "  rule_name: " << rule_name << "\n";
    ss << "  category: " << rule_category << "\n";
    ss << "  philosophy: " << philosophy_id << "\n";
    ss << "  file: " << file_path << "\n";
    ss << "  line: " << line_number << "\n";
    ss << "  column: " << column_number << "\n";
    ss << "  message: " << message << "\n";
    ss << "  suggestion: " << suggestion << "\n";
    ss << "  severity: " << severity << "\n";
    ss << "  auto_fix_available: " << (auto_fix_available ? "true" : "false") << "\n";
    return ss.str();
}

std::string Violation::toJson() const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"id\": \"" << id << "\",\n";
    ss << "  \"rule_id\": \"" << rule_id << "\",\n";
    ss << "  \"rule_name\": \"" << rule_name << "\",\n";
    ss << "  \"category\": \"" << rule_category << "\",\n";
    ss << "  \"philosophy\": \"" << philosophy_id << "\",\n";
    ss << "  \"file\": \"" << file_path << "\",\n";
    ss << "  \"line\": " << line_number << ",\n";
    ss << "  \"column\": " << column_number << ",\n";
    ss << "  \"message\": \"" << message << "\",\n";
    ss << "  \"suggestion\": \"" << suggestion << "\",\n";
    ss << "  \"severity\": \"" << severity << "\",\n";
    ss << "  \"auto_fix_available\": " << (auto_fix_available ? "true" : "false") << "\n";
    ss << "}";
    return ss.str();
}

// ValidationResult implementation
ValidationResult::ValidationResult(const std::string& target_path, const std::string& target_type)
    : target_path_(target_path), target_type_(target_type), 
      validation_time_(std::chrono::system_clock::now()),
      execution_duration_(0), total_rules_executed_(0), rules_passed_(0), 
      rules_failed_(0), rules_skipped_(0), files_processed_(0), execution_time_(0.0),
      overall_compliance_score_(0.0) {
}

// Basic information
const std::string& ValidationResult::getTargetPath() const {
    return target_path_;
}

const std::string& ValidationResult::getTargetType() const {
    return target_type_;
}

std::chrono::system_clock::time_point ValidationResult::getValidationTime() const {
    return validation_time_;
}

std::chrono::duration<double> ValidationResult::getExecutionDuration() const {
    return execution_duration_;
}

void ValidationResult::setExecutionDuration(std::chrono::duration<double> duration) {
    execution_duration_ = duration;
}

// Rule execution tracking
void ValidationResult::setTotalRulesExecuted(size_t total) {
    total_rules_executed_ = total;
}

void ValidationResult::setRulesPassed(size_t passed) {
    rules_passed_ = passed;
}

void ValidationResult::setRulesFailed(size_t failed) {
    rules_failed_ = failed;
}

void ValidationResult::setRulesSkipped(size_t skipped) {
    rules_skipped_ = skipped;
}

size_t ValidationResult::getTotalRulesExecuted() const {
    return total_rules_executed_;
}

size_t ValidationResult::getRulesPassed() const {
    return rules_passed_;
}

size_t ValidationResult::getRulesFailed() const {
    return rules_failed_;
}

size_t ValidationResult::getRulesSkipped() const {
    return rules_skipped_;
}

// Violation management
void ValidationResult::addViolation(const Violation& violation) {
    violations_.push_back(violation);
    updateViolationIndexes(violation);
}

void ValidationResult::addViolations(const std::vector<Violation>& violations) {
    for (const auto& violation : violations) {
        addViolation(violation);
    }
}

void ValidationResult::removeViolation(const std::string& violation_id) {
    auto it = std::find_if(violations_.begin(), violations_.end(),
        [&violation_id](const Violation& v) { return v.id == violation_id; });
    
    if (it != violations_.end()) {
        removeFromIndexes(violation_id);
        violations_.erase(it);
    }
}

void ValidationResult::clearViolations() {
    violations_.clear();
    violations_by_category_.clear();
    violations_by_severity_.clear();
    violations_by_file_.clear();
}

// Violation access
const std::vector<Violation>& ValidationResult::getAllViolations() const {
    return violations_;
}

// These methods are implemented later in the file with correct signatures
// Removing duplicate implementations

std::vector<Violation> ValidationResult::getViolationsByRule(const std::string& rule_id) const {
    std::vector<Violation> result;
    for (const auto& violation : violations_) {
        if (violation.rule_id == rule_id) {
            result.push_back(violation);
        }
    }
    return result;
}

Violation* ValidationResult::getViolationById(const std::string& violation_id) {
    auto it = std::find_if(violations_.begin(), violations_.end(),
        [&violation_id](const Violation& v) { return v.id == violation_id; });
    return (it != violations_.end()) ? &(*it) : nullptr;
}

const Violation* ValidationResult::getViolationById(const std::string& violation_id) const {
    auto it = std::find_if(violations_.begin(), violations_.end(),
        [&violation_id](const Violation& v) { return v.id == violation_id; });
    return (it != violations_.end()) ? &(*it) : nullptr;
}

// Violation statistics
size_t ValidationResult::getTotalViolationsCount() const {
    return violations_.size();
}

size_t ValidationResult::getViolationsCountByCategory(const std::string& category) const {
    auto violations = getViolationsByCategory(category);
    return violations.size();
}

size_t ValidationResult::getViolationsCountBySeverity(const std::string& severity) const {
    auto violations = getViolationsBySeverity(severity);
    return violations.size();
}

size_t ValidationResult::getViolationsCountByFile(const std::string& file_path) const {
    auto violations = getViolationsByFile(file_path);
    return violations.size();
}

std::vector<std::string> ValidationResult::getAffectedFiles() const {
    std::vector<std::string> files;
    for (const auto& pair : violations_by_file_) {
        files.push_back(pair.first);
    }
    std::sort(files.begin(), files.end());
    return files;
}

std::vector<std::string> ValidationResult::getViolatedCategories() const {
    std::vector<std::string> categories;
    for (const auto& pair : violations_by_category_) {
        categories.push_back(pair.first);
    }
    std::sort(categories.begin(), categories.end());
    return categories;
}

std::vector<std::string> ValidationResult::getViolatedSeverities() const {
    std::vector<std::string> severities;
    for (const auto& pair : violations_by_severity_) {
        severities.push_back(pair.first);
    }
    std::sort(severities.begin(), severities.end());
    return severities;
}

// Compliance scoring
double ValidationResult::getOverallComplianceScore() const {
    return overall_compliance_score_;
}

double ValidationResult::getCategoryComplianceScore(const std::string& category) const {
    auto it = category_compliance_scores_.find(category);
    return (it != category_compliance_scores_.end()) ? it->second : 0.0;
}

double ValidationResult::getPhilosophyComplianceScore(const std::string& philosophy_id) const {
    auto it = philosophy_compliance_scores_.find(philosophy_id);
    return (it != philosophy_compliance_scores_.end()) ? it->second : 0.0;
}

void ValidationResult::calculateComplianceScores() {
    if (total_rules_executed_ == 0) {
        overall_compliance_score_ = 100.0;
        return;
    }
    
    // Overall compliance = (rules_passed / total_rules) * 100
    overall_compliance_score_ = (static_cast<double>(rules_passed_) / total_rules_executed_) * 100.0;
}

void ValidationResult::setComplianceScore(double score) {
    overall_compliance_score_ = score;
}

void ValidationResult::setCategoryComplianceScore(const std::string& category, double score) {
    category_compliance_scores_[category] = score;
}

void ValidationResult::setPhilosophyComplianceScore(const std::string& philosophy_id, double score) {
    philosophy_compliance_scores_[philosophy_id] = score;
}

// Success/failure determination
bool ValidationResult::isValid() const {
    return violations_.empty();
}

bool ValidationResult::isCompliant() const {
    return overall_compliance_score_ >= 95.0;  // 95% compliance threshold
}

bool ValidationResult::hasViolations() const {
    return !violations_.empty();
}

bool ValidationResult::hasCriticalViolations() const {
    return getViolationsCountBySeverity("CRITICAL") > 0;
}

bool ValidationResult::hasAutoFixableViolations() const {
    for (const auto& violation : violations_) {
        if (violation.auto_fix_available) {
            return true;
        }
    }
    return false;
}

// Export and serialization
std::string ValidationResult::toYaml() const {
    std::stringstream ss;
    ss << "---\n";
    ss << "akao_validation:\n";
    ss << "  version: 1.0.0\n";
    ss << "  timestamp: '" << formatTimestamp(validation_time_) << "'\n";
    ss << "  target: " << target_path_ << "\n";
    ss << "  target_type: " << target_type_ << "\n";
    ss << "  execution_duration: " << formatDuration(execution_duration_) << "\n";
    ss << "  summary:\n";
    ss << "    total_rules: " << total_rules_executed_ << "\n";
    ss << "    passed: " << rules_passed_ << "\n";
    ss << "    failed: " << rules_failed_ << "\n";
    ss << "    skipped: " << rules_skipped_ << "\n";
    ss << "    total_violations: " << getTotalViolationsCount() << "\n";
    ss << "    compliance_score: " << std::fixed << std::setprecision(2) << overall_compliance_score_ << "\n";
    
    if (!violations_.empty()) {
        ss << "  violations:\n";
        for (const auto& violation : violations_) {
            std::string violation_yaml = violation.toYaml();
            // Indent each line by 2 spaces
            std::istringstream iss(violation_yaml);
            std::string line;
            while (std::getline(iss, line)) {
                ss << "  " << line << "\n";
            }
        }
    }
    
    return ss.str();
}

std::string ValidationResult::toTable() const {
    std::stringstream ss;
    
    ss << "✅ AKAO VALIDATION RESULTS\n";
    ss << "📊 Project: " << target_path_ << "\n";
    ss << "🕒 Timestamp: " << formatTimestamp(validation_time_) << "\n";
    ss << "⏱️ Duration: " << formatDuration(execution_duration_) << "\n";
    ss << "🏛️ Rules: " << total_rules_executed_ << " checked, " 
       << rules_passed_ << " passed, " << rules_failed_ << " failed\n";
    ss << "📈 Compliance: " << std::fixed << std::setprecision(1) 
       << overall_compliance_score_ << "%\n\n";
    
    if (violations_.empty()) {
        ss << "🎉 NO VIOLATIONS FOUND - PERFECT COMPLIANCE!\n";
    } else {
        ss << "❌ VIOLATIONS:\n";
        for (const auto& violation : violations_) {
            ss << violation.id << "\n";
            ss << "  📁 File: " << violation.file_path << ":" << violation.line_number << "\n";
            ss << "  📋 Rule: " << violation.rule_name << " (" << violation.rule_category << ")\n";
            ss << "  🎯 Issue: " << violation.message << "\n";
            ss << "  💡 Suggestion: " << violation.suggestion << "\n";
            ss << "  📚 Philosophy: " << violation.philosophy_id << "\n";
            if (violation.auto_fix_available) {
                ss << "  🔧 Auto-fix: Available\n";
            }
            ss << "\n";
        }
    }
    
    return ss.str();
}

std::string ValidationResult::toSummary() const {
    std::stringstream ss;
    ss << "Akao Validation: " << rules_passed_ << "/" << total_rules_executed_ 
       << " rules passed (" << std::fixed << std::setprecision(1) 
       << overall_compliance_score_ << "% compliance)";
    
    if (hasViolations()) {
        ss << ", " << getTotalViolationsCount() << " violations found";
    }
    
    return ss.str();
}

bool ValidationResult::exportToFile(const std::string& file_path, const std::string& format) const {
    try {
        std::ofstream file(file_path);
        if (!file.is_open()) {
            return false;
        }
        
        if (format == "yaml") {
            file << toYaml();
        } else if (format == "json") {
            file << toJson();
        } else if (format == "table") {
            file << toTable();
        } else {
            return false;
        }
        
        file.close();
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::string ValidationResult::toJson() const {
    std::stringstream ss;
    
    ss << "{\n";
    ss << "  \"target_path\": \"" << target_path_ << "\",\n";
    ss << "  \"target_type\": \"" << target_type_ << "\",\n";
    ss << "  \"validation_time\": \"" << formatTimestamp(validation_time_) << "\",\n";
    ss << "  \"execution_duration\": " << execution_duration_.count() << ",\n";
    ss << "  \"files_processed\": " << files_processed_ << ",\n";
    ss << "  \"total_rules_executed\": " << total_rules_executed_ << ",\n";
    ss << "  \"rules_passed\": " << rules_passed_ << ",\n";
    ss << "  \"rules_failed\": " << rules_failed_ << ",\n";
    ss << "  \"rules_skipped\": " << rules_skipped_ << ",\n";
    ss << "  \"total_violations\": " << violations_.size() << ",\n";
    ss << "  \"overall_compliance_score\": " << overall_compliance_score_ << ",\n";
    ss << "  \"violations\": [\n";
    
    for (size_t i = 0; i < violations_.size(); ++i) {
        ss << "    " << violations_[i].toJson();
        if (i < violations_.size() - 1) ss << ",";
        ss << "\n";
    }
    
    ss << "  ]\n";
    ss << "}";
    
    return ss.str();
}

// Debug and diagnostics
std::string ValidationResult::getDebugInfo() const {
    std::stringstream ss;
    ss << "ValidationResult Debug Info:\n";
    ss << "  Target: " << target_path_ << " (" << target_type_ << ")\n";
    ss << "  Rules: " << total_rules_executed_ << " total\n";
    ss << "  Violations: " << violations_.size() << " total\n";
    ss << "  Categories with violations: " << violations_by_category_.size() << "\n";
    ss << "  Files with violations: " << violations_by_file_.size() << "\n";
    ss << "  Compliance score: " << overall_compliance_score_ << "%\n";
    return ss.str();
}

void ValidationResult::printSummary() const {
    std::cout << toSummary() << std::endl;
}

void ValidationResult::printDetailedReport() const {
    std::cout << toTable() << std::endl;
}

// Private methods
void ValidationResult::buildViolationIndexes() {
    violations_by_category_.clear();
    violations_by_severity_.clear();
    violations_by_file_.clear();
    
    for (const auto& violation : violations_) {
        updateViolationIndexes(violation);
    }
}

void ValidationResult::updateViolationIndexes(const Violation& violation) {
    violations_by_category_[violation.rule_category].push_back(violation);
    violations_by_severity_[violation.severity].push_back(violation);
    violations_by_file_[violation.file_path].push_back(violation);
}

void ValidationResult::removeFromIndexes(const std::string& violation_id) {
    // Remove from category index
    for (auto& pair : violations_by_category_) {
        auto& violations = pair.second;
        violations.erase(std::remove_if(violations.begin(), violations.end(),
            [&violation_id](const Violation& v) { return v.id == violation_id; }), violations.end());
    }
    
    // Remove from severity index
    for (auto& pair : violations_by_severity_) {
        auto& violations = pair.second;
        violations.erase(std::remove_if(violations.begin(), violations.end(),
            [&violation_id](const Violation& v) { return v.id == violation_id; }), violations.end());
    }
    
    // Remove from file index
    for (auto& pair : violations_by_file_) {
        auto& violations = pair.second;
        violations.erase(std::remove_if(violations.begin(), violations.end(),
            [&violation_id](const Violation& v) { return v.id == violation_id; }), violations.end());
    }
}

std::string ValidationResult::formatDuration(std::chrono::duration<double> duration) const {
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    return std::to_string(ms.count()) + "ms";
}

std::string ValidationResult::formatTimestamp(std::chrono::system_clock::time_point timestamp) const {
    auto time_t = std::chrono::system_clock::to_time_t(timestamp);
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&time_t), "%Y-%m-%dT%H:%M:%SZ");
    return ss.str();
}

int ValidationResult::severityToNumber(const std::string& severity) const {
    if (severity == "CRITICAL") return 4;
    if (severity == "HIGH") return 3;
    if (severity == "MEDIUM") return 2;
    if (severity == "LOW") return 1;
    return 0;
}

// Violation access methods
const std::vector<Violation>& ValidationResult::getViolations() const {
    return violations_;
}

std::vector<Violation>& ValidationResult::getViolations() {
    return violations_;
}

const std::vector<Violation>& ValidationResult::getViolationsByCategory(const std::string& category) const {
    static std::vector<Violation> empty_violations;
    auto it = violations_by_category_.find(category);
    return (it != violations_by_category_.end()) ? it->second : empty_violations;
}

const std::vector<Violation>& ValidationResult::getViolationsBySeverity(const std::string& severity) const {
    static std::vector<Violation> empty_violations;
    auto it = violations_by_severity_.find(severity);
    return (it != violations_by_severity_.end()) ? it->second : empty_violations;
}

const std::vector<Violation>& ValidationResult::getViolationsByFile(const std::string& file_path) const {
    static std::vector<Violation> empty_violations;
    auto it = violations_by_file_.find(file_path);
    return (it != violations_by_file_.end()) ? it->second : empty_violations;
}

// File processing stats
size_t ValidationResult::getFilesProcessed() const {
    return files_processed_;
}

void ValidationResult::setFilesProcessed(size_t count) {
    files_processed_ = count;
}

// Execution time compatibility
double ValidationResult::getExecutionTime() const {
    return execution_time_;
}

void ValidationResult::setExecutionTime(double time) {
    execution_time_ = time;
}

} // namespace akao::core::engine::validator
