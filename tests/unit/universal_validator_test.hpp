#pragma once

#include "../../core/engine/validator/universal/v1.hpp"
#include <memory>

namespace akao::tests::unit {

/**
 * @brief Unit tests for UniversalValidator
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
