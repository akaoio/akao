/**
 * @id: akao:file:tests:unit:universal_validator_test_hpp:v1
 * @doc: Universal validator test header defining comprehensive test interface for core validation engine functionality with deterministic test cases and archaeological test data preservation
 * @specification: Header file for UniversalValidatorTest class with test result structure and test method declarations
 * @scope: Unit testing header for core engine universal validation component
 * @timeline: 2025-01-08
 * @rationale: Provide structured test interface for universal validator functionality with deterministic inputs and reproducible results
 * @methodology: Test class definition with result structure and comprehensive test method declarations
 * @references: ["akao:file:tests:unit:universal_validator_test:v1", "akao:class:core:engine:universal_validator:v1", "akao:directory:tests:unit:v1"]
 */

#pragma once

#include "../../core/engine/validator/universal/v1.hpp"
#include <memory>

namespace akao::tests::unit {

/**
 * @id: akao:class:tests:unit:universal_validator_test:v1
 * @doc: Unit test class for UniversalValidator providing comprehensive testing interface with deterministic test cases and archaeological test data preservation
 * @specification: Test class with initialization, validation, rule loading, and self-compliance testing methods
 * @scope: Unit testing class for core engine universal validation component
 * @timeline: 2025-01-08
 * @rationale: Provide structured testing capability for universal validator with deterministic inputs and reproducible results
 * @methodology: Class-based testing with result structure and comprehensive test method implementation
 * @references: ["akao:class:core:engine:universal_validator:v1", "akao:directory:tests:unit:v1"]
 */
class UniversalValidatorTest {
public:
    struct TestResult {
        std::string test_name;
        bool passed = false;
        std::string error_message;
    };

    UniversalValidatorTest();
    ~UniversalValidatorTest() = default;

    /**
     * @brief Run all unit tests for UniversalValidator
     */
    std::vector<TestResult> runAllTests();

    /**
     * @brief Test validator initialization
     */
    TestResult testInitialization();

    /**
     * @brief Test basic validation
     */
    TestResult testBasicValidation();

    /**
     * @brief Test rule loading
     */
    TestResult testRuleLoading();

    /**
     * @brief Test self-compliance
     */
    TestResult testSelfCompliance();

private:
    std::unique_ptr<core::engine::validator::UniversalValidator> validator_;
};

} // namespace akao::tests::unit
