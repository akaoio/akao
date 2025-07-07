#pragma once

#include "../../engine/parser/yaml/node/v1.hpp"
#include <string>
#include <vector>
#include <memory>
#include <sstream>

// Forward declaration to avoid circular dependency
namespace akao::logic {
    class PureLogicEngine;
    class Context;
}

namespace akao::core::philosophy::engine {

/**
 * @brief Philosophy validation result with formal proof details
 */
struct PhilosophyValidationResult {
    std::string philosophy_file;
    bool is_valid = false;
    std::string formal_proof;
    std::string error_message;
    std::vector<std::string> failed_theorems;
};

/**
 * @brief Philosophy Engine with Formal Logical Proof Capabilities
 * 
 * Implements Phase 3: Philosophy System Reconstruction with formal logical proofs
 * 
 * Features:
 * - Formal logical proof generation for each philosophy
 * - Step-by-step theorem validation
 * - Axiom-based reasoning system
 * - Cross-philosophy consistency checking
 * - Mathematical rigor in philosophy validation
 * 
 * Philosophy Compliance:
 * - akao:philosophy::structure:isolation:v1 (one class per file)
 * - akao:philosophy::validation:universal:v1 (comprehensive validation)
 * - akao:philosophy::security:explicit_behavior:v1 (clear validation semantics)
 */
class PhilosophyEngine {
private:
    std::unique_ptr<akao::logic::PureLogicEngine> logic_engine_;
    
    // Formal proof generation methods
    std::string generateFormalProof(const akao::core::engine::parser::YamlNode& logic_node);
    void generateProofSteps(const akao::core::engine::parser::YamlNode& node, std::stringstream& proof, int step);
    void loadAxiomsIntoContext(const akao::core::engine::parser::YamlNode& axioms, akao::logic::Context& context);
    
    // Philosophy discovery
    std::vector<std::string> findPhilosophyFiles(const std::string& directory);

public:
    PhilosophyEngine();
    ~PhilosophyEngine();
    
    // Core philosophy validation with formal proofs
    bool validatePhilosophy(const std::string& philosophy_file);
    PhilosophyValidationResult validateWithProof(const std::string& philosophy_file);
    
    // Batch validation
    std::vector<PhilosophyValidationResult> validateAllPhilosophies(const std::string& philosophies_dir);
    
    // Consistency proving
    bool provePhilosophyConsistency(const std::vector<std::string>& philosophy_files);
};

} // namespace akao::core::philosophy::engine