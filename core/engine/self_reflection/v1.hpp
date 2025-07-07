#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "../validator/universal/v1.hpp"

namespace akao::core::engine::self_reflection {

/**
 * @brief Results of self-reflection analysis
 */
struct SelfReflectionResult {
    bool architectural_compliance = true;
    bool philosophical_consistency = true;
    bool mathematical_soundness = true;
    
    std::vector<std::string> compliance_violations;
    std::vector<std::string> consistency_issues;
    std::vector<std::string> mathematical_inconsistencies;
    std::vector<std::string> improvement_recommendations;
    
    size_t files_analyzed = 0;
    size_t functions_analyzed = 0;
    size_t philosophical_principles_verified = 0;
    
    // Code quality metrics
    double code_duplication_percentage = 0.0;
    size_t redundant_implementations = 0;
    size_t architectural_drift_instances = 0;
    
    std::string detailed_report;
};

/**
 * @brief Code quality metrics for self-assessment
 */
struct CodeQualityMetrics {
    std::map<std::string, std::vector<std::string>> duplicate_code_blocks;
    std::map<std::string, std::vector<std::string>> redundant_functions;
    std::vector<std::string> architectural_violations;
    std::vector<std::string> naming_inconsistencies;
    double overall_quality_score = 0.0;
};

/**
 * @brief Self-Reflection Engine for AKAO system introspection
 * 
 * Implements AKAO's fundamental requirement for systems to apply
 * their own philosophies and rules to themselves.
 * 
 * Philosophy compliance:
 * - akao:philosophy:rule:governance:v1 - Rules govern rules
 * - akao:philosophy:validation:universal:v1 - Universal validation
 * - akao:philosophy:structure:enforcement:v1 - Structure enforcement
 */
class SelfReflectionEngine {
public:
    SelfReflectionEngine();
    ~SelfReflectionEngine() = default;

    /**
     * @brief Perform comprehensive self-reflection analysis
     */
    SelfReflectionResult performSelfReflection();

    /**
     * @brief Analyze architectural compliance against AKAO principles
     */
    bool analyzeArchitecturalCompliance();

    /**
     * @brief Validate philosophical consistency across all components
     */
    bool validatePhilosophicalConsistency();

    /**
     * @brief Verify mathematical soundness of the system
     */
    bool verifyMathematicalSoundness();

    /**
     * @brief Perform code quality self-assessment
     */
    CodeQualityMetrics performCodeQualityAssessment();

    /**
     * @brief Detect logical contradictions in the system
     */
    std::vector<std::string> detectContradictions();

    /**
     * @brief Generate improvement recommendations
     */
    std::vector<std::string> generateImprovementRecommendations();

    /**
     * @brief Generate comprehensive self-reflection report
     */
    std::string generateSelfReflectionReport(const SelfReflectionResult& result);

private:
    std::unique_ptr<validator::UniversalValidator> validator_;
    std::string akao_root_path_;
    
    // Internal analysis methods
    std::vector<std::string> analyzeSourceCodeStructure();
    std::vector<std::string> validateNamingConventions();
    std::vector<std::string> checkPhilosophyImplementationAlignment();
    std::map<std::string, std::vector<std::string>> detectCodeDuplication();
    std::vector<std::string> identifyRedundantImplementations();
    std::vector<std::string> detectArchitecturalDrift();
    bool verifyMathematicalProofConsistency();
    std::vector<std::string> validateSelfReferenceHandling();
    
    // Utility methods
    std::vector<std::string> getAllSourceFiles();
    std::string calculateFileHash(const std::string& filepath);
    size_t countFunctions(const std::string& filepath);
    std::vector<std::string> extractFunctionSignatures(const std::string& filepath);
    bool isPhilosophyCompliantFile(const std::string& filepath);
};

} // namespace akao::core::engine::self_reflection