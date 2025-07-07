#include "core/engine/validator/universal/v1.hpp"
#include <iostream>
#include <cassert>

/**
 * @brief Temporary unit test to analyze structure violations
 * 
 * This test examines the 56 structure violations to understand their root cause
 * and categorize them for targeted fixes.
 */

int main() {
    std::cout << "🔍 Structure Violation Analysis Test" << std::endl;
    
    // Initialize validator
    akao::core::engine::validator::UniversalValidator validator("rules");
    
    // Test input: Current project directory
    std::string test_input = ".";
    
    // Initialize validator
    bool initialized = validator.initialize();
    assert(initialized && "Validator must initialize successfully");
    std::cout << "✅ Validator initialized" << std::endl;
    
    // Execute validation
    auto result = validator.validate(test_input);
    
    // Expected: 56 violations detected
    size_t expected_violations = 56;
    size_t actual_violations = result.getViolations().size();
    
    std::cout << "📊 Violation Analysis:" << std::endl;
    std::cout << "  Expected violations: " << expected_violations << std::endl;
    std::cout << "  Actual violations: " << actual_violations << std::endl;
    
    // Analyze violation categories
    std::map<std::string, int> violation_categories;
    std::map<std::string, int> violation_rules;
    std::map<std::string, int> violation_files;
    
    for (const auto& violation : result.getViolations()) {
        violation_categories[violation.rule_category]++;
        violation_rules[violation.rule_id]++;
        violation_files[violation.file_path]++;
        
        // Print detailed violation info
        std::cout << "🚨 Violation: " << violation.rule_id << std::endl;
        std::cout << "   File: " << violation.file_path << ":" << violation.line_number << std::endl;
        std::cout << "   Message: " << violation.message << std::endl;
        std::cout << "   Category: " << violation.rule_category << std::endl;
        std::cout << "   Severity: " << violation.severity << std::endl;
        std::cout << std::endl;
    }
    
    // Category summary
    std::cout << "📂 Violation Categories:" << std::endl;
    for (const auto& cat : violation_categories) {
        std::cout << "  " << cat.first << ": " << cat.second << " violations" << std::endl;
    }
    
    // Rule summary
    std::cout << "📋 Top Violation Rules:" << std::endl;
    for (const auto& rule : violation_rules) {
        std::cout << "  " << rule.first << ": " << rule.second << " violations" << std::endl;
    }
    
    // File summary
    std::cout << "📄 Top Violating Files:" << std::endl;
    int file_count = 0;
    for (const auto& file : violation_files) {
        if (file_count++ < 10) {  // Show top 10
            std::cout << "  " << file.first << ": " << file.second << " violations" << std::endl;
        }
    }
    
    // Assertion: Verify we can detect the violations (fraud prevention)
    assert(actual_violations > 0 && "Must detect structure violations");
    assert(actual_violations <= 100 && "Violation count must be reasonable");
    
    // Test passes if we can analyze the violations
    bool analysis_successful = !result.getViolations().empty();
    
    if (analysis_successful) {
        std::cout << "✅ Structure violation analysis completed successfully" << std::endl;
        return 0;
    } else {
        std::cout << "❌ Structure violation analysis failed" << std::endl;
        return 1;
    }
}