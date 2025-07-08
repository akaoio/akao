#pragma once

#include "../self_validation/v1.hpp"
#include "../self_reflection/v1.hpp"
#include "../../rule/registry/rule-registry/v1.hpp"
#include "../../philosophy/engine/v1.hpp"
#include <string>
#include <vector>
#include <map>
#include <chrono>

namespace akao {
namespace core {
namespace engine {
namespace phase6 {

/**
 * @brief Phase 6 Complete System Integration Results
 */
struct Phase6IntegrationResult {
    bool success = true;
    std::string summary;
    double overall_score = 0.0; // 0-100 scale
    
    // Component integration status
    std::map<std::string, bool> component_status;
    std::map<std::string, std::string> component_details;
    
    // Performance metrics
    double end_to_end_latency_ms = 0.0;
    size_t total_components_tested = 0;
    size_t components_passing = 0;
    
    // Quality metrics
    double architectural_compliance_score = 0.0;
    double philosophical_alignment_score = 0.0;
    double self_validation_score = 0.0;
    double production_readiness_score = 0.0;
    
    // Issues and recommendations
    std::vector<std::string> critical_issues;
    std::vector<std::string> warnings;
    std::vector<std::string> recommendations;
    
    // Timing information
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point end_time;
};

/**
 * @brief Phase 6: Complete System Integration and Final Validation
 * 
 * This orchestrates the final phase of the Pure Logic Engine migration,
 * bringing together all previous phases into a cohesive, production-ready system.
 * 
 * Phase 6 Requirements:
 * - Complete System Integration
 * - Final Quality Assurance  
 * - Production Deployment Preparation
 * 
 * Philosophy compliance:
 * - akao:philosophy:integration:holistic:v1 - Complete system integration
 * - akao:philosophy:validation:comprehensive:v1 - Thorough validation
 * - akao:philosophy:deployment:production:v1 - Production readiness
 */
class Phase6SystemIntegrator {
private:
    std::string system_root_path_;
    bool verbose_mode_;
    
    // Core engine components
    std::unique_ptr<akao::core::engine::self_validation::SelfValidationOrchestrator> self_validator_;
    std::unique_ptr<akao::core::engine::self_reflection::SelfReflectionEngine> self_reflector_;
    std::unique_ptr<akao::core::philosophy::engine::PhilosophyEngine> philosophy_engine_;
    std::unique_ptr<akao::core::rule::registry::RuleRegistry> rule_registry_;
    
public:
    Phase6SystemIntegrator(const std::string& system_root_path, bool verbose = false);
    ~Phase6SystemIntegrator() = default;

    /**
     * @brief Execute Phase 6: Complete System Integration
     * 
     * This is the main Phase 6 entry point implementing all requirements:
     * 1. Complete System Integration
     * 2. Final Quality Assurance  
     * 3. Production Deployment Preparation
     */
    Phase6IntegrationResult executePhase6Integration();

    /**
     * @brief 1. Complete System Integration
     * 
     * Integrates all phases into cohesive system architecture:
     * - Validates end-to-end functionality from CLI to Pure Logic Engine
     * - Confirms philosophy-rule-ruleset relationships function correctly
     * - Establishes complete self-validation workflow
     */
    bool executeCompleteSystemIntegration(Phase6IntegrationResult& result);

    /**
     * @brief 2. Final Quality Assurance
     * 
     * Executes comprehensive system validation using AKAO principles:
     * - Confirms mathematical formal proof validation across all components
     * - Validates self-reflection and contradiction detection capabilities
     * - Generates final integration report with proof validation
     */
    bool executeFinalQualityAssurance(Phase6IntegrationResult& result);

    /**
     * @brief 3. Production Deployment Preparation
     * 
     * Confirms zero external dependencies compliance:
     * - Validates build system integrity across all platforms
     * - Establishes deployment procedures with rollback capabilities
     * - Generates production readiness certification
     */
    bool executeProductionDeploymentPreparation(Phase6IntegrationResult& result);

    /**
     * @brief Generate Final Phase 6 Certification Report
     */
    std::string generateFinalCertificationReport(const Phase6IntegrationResult& result);

private:
    // Component Integration Testing
    bool testCliToLogicEngineIntegration();
    bool testPhilosophyRulesetIntegration();
    bool testSelfValidationWorkflow();
    bool testYamlLibraryIntegration();
    bool testBuildSystemIntegration();
    
    // Quality Assurance Testing
    bool validateMathematicalProofSystem();
    bool validateContradictionDetection();
    bool validateSelfReflectionCapabilities();
    bool executeComprehensiveSystemValidation();
    
    // Production Readiness Testing
    bool validateZeroExternalDependencies();
    bool validateBuildSystemIntegrity();
    bool validateDeploymentProcedures();
    bool generateProductionCertification(Phase6IntegrationResult& result);
    
    // Metrics and Scoring
    double calculateOverallScore(const Phase6IntegrationResult& result);
    double calculateArchitecturalCompliance();
    double calculatePhilosophicalAlignment();
    double calculateProductionReadiness();
    
    // Utilities
    void logVerbose(const std::string& message);
    void recordTiming(Phase6IntegrationResult& result);
    std::vector<std::string> generateRecommendations(const Phase6IntegrationResult& result);
};

} // namespace phase6
} // namespace engine
} // namespace core
} // namespace akao
