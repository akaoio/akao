#pragma once

#include "core/engine/validator/universal_validator.hpp"
#include <string>
#include <vector>
#include <map>

namespace akao::tests::compliance {

/**
 * @brief Philosophy compliance test result
 */
struct PhilosophyComplianceResult {
    std::string philosophy_id;
    std::string philosophy_name;
    bool compliant = false;
    std::vector<std::string> violations;
    std::vector<std::string> evidence;
    double compliance_score = 0.0;
};

/**
 * @brief Comprehensive philosophy compliance testing
 * 
 * Tests each Akao philosophy individually to ensure the framework
 * truly embodies its own philosophical principles.
 * 
 * Philosophy compliance:
 * - akao:philosophy:rule:governance:v1 - Rules govern rules principle
 * - akao:philosophy:testing:coverage:v1 - Comprehensive test coverage
 * - akao:philosophy:validation:universal:v1 - Universal validation approach
 */
class PhilosophyComplianceTest {
public:
    /**
     * @brief Constructor
     */
    PhilosophyComplianceTest();

    /**
     * @brief Initialize test suite
     */
    bool initialize();

    /**
     * @brief Test all philosophies
     */
    std::vector<PhilosophyComplianceResult> testAllPhilosophies();

    /**
     * @brief Test specific philosophy
     */
    PhilosophyComplianceResult testPhilosophy(const std::string& philosophy_id);

    /**
     * @brief Test automation philosophy
     */
    PhilosophyComplianceResult testAutomationUpdatesV1();

    /**
     * @brief Test build duality philosophy
     */
    PhilosophyComplianceResult testBuildDualityV1();

    /**
     * @brief Test documentation generation philosophy
     */
    PhilosophyComplianceResult testDocumentationGenerationV1();

    /**
     * @brief Test interface consistency philosophy
     */
    PhilosophyComplianceResult testInterfaceConsistencyV1();

    /**
     * @brief Test language isolation philosophy
     */
    PhilosophyComplianceResult testLanguageIsolationV1();

    /**
     * @brief Test measurement observability philosophy
     */
    PhilosophyComplianceResult testMeasurementObservabilityV1();

    /**
     * @brief Test measurement traceability philosophy
     */
    PhilosophyComplianceResult testMeasurementTraceabilityV1();

    /**
     * @brief Test rule governance philosophy
     */
    PhilosophyComplianceResult testRuleGovernanceV1();

    /**
     * @brief Test security explicit behavior philosophy
     */
    PhilosophyComplianceResult testSecurityExplicitBehaviorV1();

    /**
     * @brief Test structure enforcement philosophy
     */
    PhilosophyComplianceResult testStructureEnforcementV1();

    /**
     * @brief Test structure isolation philosophy
     */
    PhilosophyComplianceResult testStructureIsolationV1();

    /**
     * @brief Test testing coverage philosophy
     */
    PhilosophyComplianceResult testTestingCoverageV1();

    /**
     * @brief Test validation universal philosophy
     */
    PhilosophyComplianceResult testValidationUniversalV1();

    /**
     * @brief Test visualization graph explainability philosophy
     */
    PhilosophyComplianceResult testVisualizationGraphExplainabilityV1();

    /**
     * @brief Generate philosophy compliance report
     */
    std::string generateComplianceReport(const std::vector<PhilosophyComplianceResult>& results);

    /**
     * @brief Get overall compliance score
     */
    double getOverallComplianceScore(const std::vector<PhilosophyComplianceResult>& results);

    /**
     * @brief Check if all philosophies are compliant
     */
    bool allPhilosophiesCompliant(const std::vector<PhilosophyComplianceResult>& results);

private:
    std::unique_ptr<core::engine::validator::UniversalValidator> validator_;
    std::string akao_source_path_;

    // Helper methods
    PhilosophyComplianceResult createPhilosophyResult(const std::string& id, const std::string& name);
    void addViolation(PhilosophyComplianceResult& result, const std::string& violation);
    void addEvidence(PhilosophyComplianceResult& result, const std::string& evidence);
    void calculateComplianceScore(PhilosophyComplianceResult& result);

    // Validation helpers
    bool checkFileExists(const std::string& path);
    bool checkDirectoryExists(const std::string& path);
    bool checkFileContains(const std::string& path, const std::string& content);
    bool checkMultipleBuildSystems();
    bool checkOnlyStandardLibrary();
    bool checkConsistentInterfaces();
    bool checkComprehensiveDocumentation();
    bool checkRuleGovernance();
    bool checkMeasurementCapabilities();
    bool checkSecurityPrinciples();
    bool checkStructuralCompliance();
    bool checkTestCoverage();
    bool checkVisualizationCapabilities();
    bool checkTraceabilityFeatures();
    bool checkAutomationCapabilities();
    bool checkModularIsolation();

    // File analysis helpers
    std::vector<std::string> findFilesWithExtension(const std::string& extension);
    std::vector<std::string> analyzeIncludeDependencies();
    std::vector<std::string> analyzeBuildConfigurations();
    std::vector<std::string> analyzeInterfaceConsistency();
    std::vector<std::string> analyzeDocumentationCoverage();
    std::vector<std::string> analyzeTestCoverage();
    std::vector<std::string> analyzeCodeStructure();

    // Specific compliance checkers
    bool validateAutomationImplementation();
    bool validateBuildDualityImplementation();
    bool validateDocumentationGenerationImplementation();
    bool validateInterfaceConsistencyImplementation();
    bool validateLanguageIsolationImplementation();
    bool validateMeasurementObservabilityImplementation();
    bool validateMeasurementTraceabilityImplementation();
    bool validateRuleGovernanceImplementation();
    bool validateSecurityExplicitBehaviorImplementation();
    bool validateStructureEnforcementImplementation();
    bool validateStructureIsolationImplementation();
    bool validateTestingCoverageImplementation();
    bool validateValidationUniversalImplementation();
    bool validateVisualizationGraphExplainabilityImplementation();
};

/**
 * @brief Philosophy compliance utilities
 */
namespace philosophy_utils {

/**
 * @brief Format philosophy compliance result
 */
std::string formatPhilosophyResult(const PhilosophyComplianceResult& result);

/**
 * @brief Calculate weighted compliance score
 */
double calculateWeightedScore(const std::vector<PhilosophyComplianceResult>& results);

/**
 * @brief Get critical philosophy violations
 */
std::vector<std::string> getCriticalViolations(const std::vector<PhilosophyComplianceResult>& results);

/**
 * @brief Generate philosophy summary
 */
std::string generatePhilosophySummary(const std::vector<PhilosophyComplianceResult>& results);

/**
 * @brief Check philosophy dependencies
 */
std::map<std::string, std::vector<std::string>> getPhilosophyDependencies();

/**
 * @brief Validate philosophy interdependencies
 */
bool validatePhilosophyInterdependencies(const std::vector<PhilosophyComplianceResult>& results);

} // namespace philosophy_utils

} // namespace akao::tests::compliance
