/**
 * @id akao.evolve.mock_validator.v1
 * @doc Mock validator implementation demonstrating the ValidatorNode conversion pattern without requiring full core dependencies, enabling testing of the node-based architecture concepts.
 * @specification Simplified validator mock providing the same interface as UnifiedValidator but with basic implementation for testing ValidatorNode functionality and conversion patterns.
 * @scope artifacts/evolve/phase2/step2-1-1
 * @timeline 2025-07-10
 * @rationale Enable ValidatorNode testing and demonstration without requiring complex core dependencies that may not be fully available during development.
 * @methodology Mock implementation maintaining interface compatibility while providing simplified validation logic for testing purposes.
 * @references akao.evolve.validator_node.v1
 */

#ifndef AKAO_EVOLVE_MOCK_VALIDATOR_V1_HPP
#define AKAO_EVOLVE_MOCK_VALIDATOR_V1_HPP

#include <string>
#include <vector>
#include <chrono>
#include <map>

namespace akao {
namespace core {
namespace engine {
namespace validator {

/**
 * @brief Mock violation for testing
 */
struct Violation {
    std::string id;
    std::string rule_id;
    std::string rule_name;
    std::string philosophy_id;
    std::string description;
    std::string message;
    std::string file_path;
    int line_number = 0;
    int column_number = 0;
    std::string severity = "error";
    std::string rule_category;
    std::string suggestion;
    bool auto_fix_available = false;
    std::chrono::system_clock::time_point detected_at = std::chrono::system_clock::now();
    
    // Trace fields
    std::string trace_id;
    std::string violation_id;
    std::string project_path;
    std::vector<std::string> call_stack;
    std::vector<std::string> rule_chain;
    std::map<std::string, std::string> context_variables;
    std::chrono::system_clock::time_point traced_at;
    std::string violation_category;
    std::string root_cause;
    std::vector<std::string> related_violations;
    std::vector<std::string> fix_commands;
    std::map<std::string, std::string> metadata;
    
    // Philosophy fields
    std::string formal_proof;
    std::vector<std::string> failed_theorems;
};

/**
 * @brief Mock validation result
 */
struct ValidationResult {
    bool is_valid = true;
    std::vector<Violation> violations;
    std::string summary;
    std::string target_path;
    std::string validation_type;
    size_t total_rules_executed = 0;
    size_t total_files_analyzed = 0;
    std::chrono::duration<double> execution_duration{0};
    
    void addViolation(const Violation& violation) {
        violations.push_back(violation);
        is_valid = false;
    }
};

/**
 * @brief Mock validation configuration
 */
struct ValidationConfig {
    bool enable_universal_validation;
    bool enable_compliance_checking;
    bool enable_metadata_validation;
    bool enable_namespace_validation;
    std::string rules_directory;
    std::vector<std::string> file_patterns;
    bool enable_auto_fix;
    bool parallel_execution;
    
    ValidationConfig() 
        : enable_universal_validation(true)
        , enable_compliance_checking(true)
        , enable_metadata_validation(true)
        , enable_namespace_validation(true)
        , rules_directory(".akao/rules")
        , file_patterns({"*.cpp", "*.hpp", "*.yaml"})
        , enable_auto_fix(false)
        , parallel_execution(false)
    {}
};

/**
 * @brief Mock unified validator
 */
class UnifiedValidator {
public:

    explicit UnifiedValidator(const ValidationConfig& config = ValidationConfig()) 
        : config_(config), is_initialized_(true) {}
    
    ~UnifiedValidator() = default;

    // Mock validation methods
    ValidationResult validate(const std::string& target_path) {
        return createMockResult(target_path, "universal");
    }
    
    ValidationResult validateFile(const std::string& file_path) {
        return createMockResult(file_path, "file");
    }
    
    ValidationResult validateDirectory(const std::string& dir_path) {
        return createMockResult(dir_path, "directory");
    }
    
    ValidationResult validateUniversal(const std::string& target_path) {
        return createMockResult(target_path, "universal");
    }
    
    ValidationResult validateCompliance(const std::string& target_path) {
        return createMockResult(target_path, "compliance");
    }
    
    ValidationResult validateMetadata(const std::string& target_path) {
        return createMockResult(target_path, "metadata");
    }
    
    ValidationResult validateNamespace(const std::string& target_path) {
        return createMockResult(target_path, "namespace");
    }
    
    ValidationResult validatePhilosophy(const std::string& target_path) {
        return createMockResult(target_path, "philosophy");
    }
    
    ValidationResult validatePhased(const std::string& target_path) {
        return createMockResult(target_path, "phased");
    }
    
    ValidationResult validateWithRules(const std::string& target_path, const std::vector<std::string>& rule_ids) {
        auto result = createMockResult(target_path, "rule_driven");
        result.summary += " (with " + std::to_string(rule_ids.size()) + " specific rules)";
        return result;
    }
    
    bool isInitialized() const { return is_initialized_; }

private:
    ValidationConfig config_;
    bool is_initialized_ = false;
    
    ValidationResult createMockResult(const std::string& target_path, const std::string& type) {
        ValidationResult result;
        result.target_path = target_path;
        result.validation_type = type;
        result.total_rules_executed = 5;
        result.total_files_analyzed = 10;
        result.execution_duration = std::chrono::duration<double>(0.1);
        
        // Create some mock violations for testing
        if (target_path.find("invalid") != std::string::npos) {
            Violation v1;
            v1.id = "mock_violation_1";
            v1.rule_id = "mock_rule_1";
            v1.rule_name = "Mock Rule 1";
            v1.message = "Mock validation violation in " + target_path;
            v1.file_path = target_path;
            v1.line_number = 42;
            v1.severity = "error";
            result.addViolation(v1);
            
            Violation v2;
            v2.id = "mock_violation_2";
            v2.rule_id = "mock_rule_2";
            v2.rule_name = "Mock Rule 2";
            v2.message = "Another mock violation";
            v2.file_path = target_path;
            v2.line_number = 100;
            v2.severity = "warning";
            result.addViolation(v2);
        }
        
        result.summary = "Mock " + type + " validation of " + target_path + 
                        " found " + std::to_string(result.violations.size()) + " violations";
        
        return result;
    }
};

} // namespace validator
} // namespace engine
} // namespace core
} // namespace akao

#endif // AKAO_EVOLVE_MOCK_VALIDATOR_V1_HPP