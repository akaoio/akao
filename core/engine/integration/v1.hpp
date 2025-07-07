#pragma once

#include "../logic/pure/v1.hpp"
#include "../../philosophy/engine/v1.hpp"
#include "../../rule/registry/rule_registry.hpp"
#include <string>
#include <vector>
#include <memory>

namespace akao::core::engine::integration {

/**
 * @brief Pure Logic Engine Integration Coordinator
 * 
 * Implements Phase 4: Pure Logic Engine Integration across all components
 * 
 * Responsibilities:
 * - Coordinate pure logic engine usage across all system components
 * - Ensure consistent logic evaluation throughout the system
 * - Manage cross-component logic integration
 * - Validate end-to-end logic flow
 * 
 * Integration Points:
 * - Philosophy validation system
 * - Rule execution engine
 * - Universal validator
 * - Runtime control systems
 */
class LogicEngineIntegrator {
private:
    std::unique_ptr<akao::logic::PureLogicEngine> master_logic_engine_;
    std::unique_ptr<akao::core::philosophy::engine::PhilosophyEngine> philosophy_engine_;
    
    // Integration validation
    bool validatePhilosophyIntegration();
    bool validateRuleIntegration();
    bool validateValidatorIntegration();
    bool validateRuntimeIntegration();

public:
    LogicEngineIntegrator();
    ~LogicEngineIntegrator();
    
    // Phase 4 Implementation
    bool integrateAllComponents();
    bool validateCrossComponentIntegration();
    
    // Component-specific integration
    bool integratePhilosophySystem(const std::string& philosophies_dir);
    bool integrateRuleSystem(const std::string& rules_dir);
    bool integrateValidationSystem();
    bool integrateRuntimeControl();
    
    // End-to-end validation
    bool validateEndToEndLogicFlow();
    
    // Access to integrated engines
    akao::logic::PureLogicEngine& getMasterLogicEngine();
    akao::core::philosophy::engine::PhilosophyEngine& getPhilosophyEngine();
};

} // namespace akao::core::engine::integration