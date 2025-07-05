#pragma once

#include "core/engine/validator/universal_validator.hpp"
#include "core/rule/registry/rule_registry.hpp"
#include <string>
#include <vector>
#include <map>

namespace akao::tests::compliance {

/**
 * @brief Self-validation test result
 */
struct SelfValidationResult {
    bool passed = false;
    std::string test_name;
    std::string description;
    std::vector<std::string> violations;
    std::vector<std::string> warnings;
    double execution_time = 0.0;
};

/**
 * @brief Self-validation test suite for Akao
 * 
 * Implements comprehensive validation of Akao against its own
 * philosophies and rules, ensuring "rules govern rules" principle.
 * 
 * Philosophy compliance:
 * - akao:philosophy:rule:governance:v1 - Rules govern the rule system itself
 * - akao:philosophy:testing:coverage:v1 - Comprehensive test coverage
 * - akao:philosophy:validation:universal:v1 - Universal validation approach
 * - akao:philosophy:measurement:observability:v1 - Observable validation metrics
 */
class SelfValidationTest {
public:
    /**
     * @brief Constructor
     */
    SelfValidationTest();

    /**
     * @brief Initialize test suite
     */
    bool initialize();

    /**
     * @brief Run all self-validation tests
     */
    std::vector<SelfValidationResult> runAllTests();

    /**
     * @brief Run specific test by name
     */
    SelfValidationResult runTest(const std::string& test_name);

    /**
     * @brief Test core architecture compliance
     */
    SelfValidationResult testCoreArchitecture();

    /**
     * @brief Test rule system compliance
     */
    SelfValidationResult testRuleSystem();

    /**
     * @brief Test interface consistency
     */
    SelfValidationResult testInterfaceConsistency();

    /**
     * @brief Test philosophy adherence
     */
    SelfValidationResult testPhilosophyAdherence();

    /**
     * @brief Test build system compliance
     */
    SelfValidationResult testBuildSystem();

    /**
     * @brief Test documentation compliance
     */
    SelfValidationResult testDocumentation();

    /**
     * @brief Test language isolation
     */
    SelfValidationResult testLanguageIsolation();

    /**
     * @brief Test structure enforcement
     */
    SelfValidationResult testStructureEnforcement();

    /**
     * @brief Test measurement and observability
     */
    SelfValidationResult testMeasurementObservability();

    /**
     * @brief Test security compliance
     */
    SelfValidationResult testSecurityCompliance();

    /**
     * @brief Generate comprehensive self-validation report
     */
    std::string generateReport(const std::vector<SelfValidationResult>& results);

    /**
     * @brief Get test names
     */
    std::vector<std::string> getTestNames() const;

    /**
     * @brief Check if all tests passed
     */
    bool allTestsPassed(const std::vector<SelfValidationResult>& results) const;

private:
    std::unique_ptr<core::engine::validator::UniversalValidator> validator_;
    std::unique_ptr<core::rule::registry::RuleRegistry> rule_registry_;
    std::string akao_source_path_;

    // Test helper methods
    SelfValidationResult createTestResult(const std::string& name, const std::string& description);
    void addViolation(SelfValidationResult& result, const std::string& violation);
    void addWarning(SelfValidationResult& result, const std::string& warning);
    void setTestPassed(SelfValidationResult& result, bool passed);

    // Specific validation helpers
    bool validateDirectoryStructure();
    bool validateFileNamingConventions();
    bool validateHeaderIncludes();
    bool validateNamespaceUsage();
    bool validatePhilosophyCompliance();
    bool validateRuleDefinitions();
    bool validateCMakeConfiguration();
    bool validateMakefileConfiguration();
    bool validateDocumentationCoverage();
    bool validateCodeComments();
    bool validateErrorHandling();
    bool validateMemoryManagement();
    bool validatePerformanceRequirements();

    // File system helpers
    std::vector<std::string> findSourceFiles();
    std::vector<std::string> findHeaderFiles();
    std::vector<std::string> findYamlFiles();
    bool fileExists(const std::string& path);
    std::string readFile(const std::string& path);
    std::vector<std::string> getFileLines(const std::string& path);

    // Validation rule checkers
    bool checkIncludeGuards(const std::string& header_file);
    bool checkNamespaceStructure(const std::string& source_file);
    bool checkErrorHandlingPattern(const std::string& source_file);
    bool checkMemoryManagementPattern(const std::string& source_file);
    bool checkDocumentationPattern(const std::string& source_file);
    bool checkPerformancePattern(const std::string& source_file);

    // Philosophy compliance checkers
    bool checkRuleGovernancePhilosophy();
    bool checkInterfaceConsistencyPhilosophy();
    bool checkAutomationPhilosophy();
    bool checkBuildDualityPhilosophy();
    bool checkDocumentationGenerationPhilosophy();
    bool checkLanguageIsolationPhilosophy();
    bool checkMeasurementObservabilityPhilosophy();
    bool checkSecurityExplicitBehaviorPhilosophy();
    bool checkStructureEnforcementPhilosophy();
    bool checkTestingCoveragePhilosophy();
    bool checkValidationUniversalPhilosophy();
    bool checkVisualizationExplainabilityPhilosophy();

    // Rule compliance checkers
    std::map<std::string, bool> checkAllRuleCompliance();
    bool checkAutomationRules();
    bool checkBuildRules();
    bool checkDocumentationRules();
    bool checkInterfaceRules();
    bool checkLanguageRules();
    bool checkMeasurementRules();
    bool checkRuleMetaRules();
    bool checkSecurityRules();
    bool checkStructureRules();
    bool checkTestingRules();
    bool checkValidationRules();
    bool checkVisualizationRules();
};

/**
 * @brief Test utilities
 */
namespace utils {

/**
 * @brief Format test results for console output
 */
std::string formatTestResults(const std::vector<SelfValidationResult>& results);

/**
 * @brief Format single test result
 */
std::string formatTestResult(const SelfValidationResult& result);

/**
 * @brief Calculate overall compliance score
 */
double calculateComplianceScore(const std::vector<SelfValidationResult>& results);

/**
 * @brief Generate test summary
 */
std::string generateTestSummary(const std::vector<SelfValidationResult>& results);

/**
 * @brief Check if critical tests passed
 */
bool criticalTestsPassed(const std::vector<SelfValidationResult>& results);

/**
 * @brief Get failed test names
 */
std::vector<std::string> getFailedTestNames(const std::vector<SelfValidationResult>& results);

/**
 * @brief Get test execution statistics
 */
struct TestStats {
    size_t total_tests;
    size_t passed_tests;
    size_t failed_tests;
    double total_execution_time;
    size_t total_violations;
    size_t total_warnings;
};

TestStats calculateTestStats(const std::vector<SelfValidationResult>& results);

} // namespace utils

} // namespace akao::tests::compliance
