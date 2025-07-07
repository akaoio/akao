#pragma once

#include "../self_reflection/v1.hpp"
#include "../self_proving/v1.hpp"
#include <string>
#include <vector>
#include <memory>

namespace akao::core::engine::self_validation {

/**
 * @brief Comprehensive self-validation results
 */
struct SelfValidationResult {
    bool overall_success = true;
    
    // Component results
    self_reflection::SelfReflectionResult reflection_result;
    self_proving::ProofVerificationResult proof_result;
    
    // Consolidated metrics
    size_t total_issues_found = 0;
    size_t critical_issues = 0;
    size_t warnings = 0;
    
    // Compliance scores (0.0 to 1.0)
    double architectural_compliance_score = 1.0;
    double philosophical_consistency_score = 1.0;
    double mathematical_soundness_score = 1.0;
    double overall_quality_score = 1.0;
    
    std::string executive_summary;
    std::string detailed_report;
    std::vector<std::string> action_items;
};

/**
 * @brief Self-Validation Orchestrator - Phase 3 Implementation
 * 
 * Implements AKAO's fundamental requirement for systems to apply
 * their own philosophies and rules to themselves. This orchestrator
 * coordinates all self-validation activities.
 * 
 * Philosophy compliance:
 * - akao:philosophy:rule:governance:v1 - Rules govern rules
 * - akao:philosophy:validation:universal:v1 - Universal validation
 * - akao:philosophy:structure:enforcement:v1 - Structure enforcement
 * - akao:philosophy:measurement:traceability:v1 - Full traceability
 */
class SelfValidationOrchestrator {
public:
    SelfValidationOrchestrator();
    ~SelfValidationOrchestrator() = default;

    /**
     * @brief Execute complete Phase 3 self-validation
     */
    SelfValidationResult executePhase3SelfValidation();

    /**
     * @brief Validate that AKAO applies its own philosophies to itself
     */
    bool validateSelfGovernance();

    /**
     * @brief Detect contradictions across all system components
     */
    std::vector<std::string> detectSystemContradictions();

    /**
     * @brief Generate executive summary for stakeholders
     */
    std::string generateExecutiveSummary(const SelfValidationResult& result);

    /**
     * @brief Generate detailed technical report
     */
    std::string generateDetailedReport(const SelfValidationResult& result);

    /**
     * @brief Generate actionable improvement recommendations
     */
    std::vector<std::string> generateActionItems(const SelfValidationResult& result);

    /**
     * @brief Verify that validation criteria are met
     */
    bool verifyValidationCriteria(const SelfValidationResult& result);

private:
    std::unique_ptr<self_reflection::SelfReflectionEngine> reflection_engine_;
    std::unique_ptr<self_proving::SelfProvingEngine> proving_engine_;
    
    // Internal validation methods
    double calculateComplianceScore(const self_reflection::SelfReflectionResult& result);
    double calculateConsistencyScore(const self_reflection::SelfReflectionResult& result);
    double calculateSoundnessScore(const self_proving::ProofVerificationResult& result);
    double calculateOverallScore(const SelfValidationResult& result);
    
    // Reporting helpers
    std::string formatScore(double score);
    std::string formatIssueCount(size_t count, const std::string& type);
    std::string generateScorecard(const SelfValidationResult& result);
};

} // namespace akao::core::engine::self_validation