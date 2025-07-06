/*
 * Akao Self-Validation Test
 * Tests that Akao validates itself according to its own rules and philosophies
 * 
 * Philosophy compliance:
 * - akao:philosophy:validation:universal:v1 - Universal validation capability  
 * - akao:philosophy:rule:governance:v1 - Rules govern rules
 * - akao:philosophy:testing:coverage:v1 - Comprehensive testing
 */

#pragma once

#include "../../core/engine/validator/universal_validator.hpp"
#include <string>
#include <vector>

namespace akao::tests::compliance {

/**
 * @brief Self-validation test suite for Akao framework
 * 
 * Ensures that Akao itself complies with all its own rules and philosophies.
 * This is critical for the "rules govern rules" philosophy.
 */
class SelfValidationTest {
public:
    struct TestResult {
        bool passed = false;
        std::string test_name;
        std::string error_message;
        size_t violations_found = 0;
        size_t expected_violations = 0;
    };

    struct SuiteResult {
        std::vector<TestResult> test_results;
        size_t tests_passed = 0;
        size_t tests_failed = 0;
        bool overall_success = false;
    };

    SelfValidationTest();
    ~SelfValidationTest() = default;

    // Main test execution
    SuiteResult runAllTests();
    
    // Individual test methods
    TestResult testStructureCompliance();
    TestResult testInterfaceConsistency();
    TestResult testPhilosophyAdherence();
    TestResult testRuleGovernance();
    TestResult testUniversalValidation();
    
private:
    std::unique_ptr<akao::core::engine::validator::UniversalValidator> validator_;
    std::string akao_root_path_;
    
    // Helper methods
    bool validateAkaoItself();
    size_t countExpectedViolations();
    bool isAcceptableViolation(const std::string& rule_id, const std::string& file_path);
};

} // namespace akao::tests::compliance