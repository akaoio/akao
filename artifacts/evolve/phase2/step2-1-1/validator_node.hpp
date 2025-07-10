/**
 * @id akao.evolve.validator_node.v1
 * @doc ValidatorNode converts the existing UnifiedValidator into the node-based architecture, providing the same validation capabilities through the INode interface with full integration to the Pure Logic Engine and .akao/rules system.
 * @specification Node wrapper for UnifiedValidator enabling validation workflows in the node-based architecture while maintaining all existing validation functionality including rule execution and violation reporting.
 * @scope artifacts/evolve/phase2/step2-1-1
 * @timeline 2025-07-10
 * @rationale Enable validator functionality in node-based workflows while preserving all existing validation logic and maintaining compatibility with current .akao rule system.
 * @methodology Adapter pattern wrapping UnifiedValidator with INode interface, parameter conversion, and result transformation while maintaining direct access to all validator capabilities.
 * @references akao.evolve.inode.v1, akao.core.engine.validator.v1
 */

#ifndef AKAO_EVOLVE_VALIDATOR_NODE_V1_HPP
#define AKAO_EVOLVE_VALIDATOR_NODE_V1_HPP

#include "../../phase1/step1-3-2/inode.hpp"
#include "../../phase1/step1-3-1/node_definition.hpp"
#include "../../phase1/step1-3-1/node_registry.hpp"
#include "../../phase1/step1-2-2/node_parameter.hpp"
#ifdef AKAO_USE_REAL_VALIDATOR
#include "core/engine/validator/v1.hpp"
#else
#include "mock_validator.hpp"
#endif
#include <memory>
#include <filesystem>

namespace akao {
namespace evolve {
namespace component {

/**
 * @brief Validation mode enumeration for ValidatorNode
 */
enum class ValidationMode {
    UNIVERSAL,      // Universal validation using all rules
    COMPLIANCE,     // Compliance checking only
    METADATA,       // Metadata validation only  
    NAMESPACE,      // Namespace validation only
    PHILOSOPHY,     // Philosophy validation only
    PHASED,         // Phase-aware validation pipeline
    RULE_DRIVEN,    // Specific rule-driven validation
    SINGLE_FILE,    // Single file validation
    DIRECTORY       // Directory validation
};

/**
 * @brief Convert ValidationMode to string for parameter handling
 */
std::string validationModeToString(ValidationMode mode);

/**
 * @brief Convert string to ValidationMode for parameter parsing
 */
std::optional<ValidationMode> stringToValidationMode(const std::string& mode_str);

/**
 * @brief ValidatorNode - Node wrapper for UnifiedValidator
 */
class ValidatorNode : public INode {
private:
    std::unique_ptr<akao::core::engine::validator::UnifiedValidator> validator_;
    mutable akao::core::engine::validator::ValidationConfig cached_config_;
    
public:
    ValidatorNode();
    ~ValidatorNode() override = default;
    
    // INode interface implementation
    std::string getNodeId() const override;
    std::string getNodeType() const override;
    NodeDefinition getDefinition() const override;
    ExecutionResult execute(const NodeContext& context) override;
    ValidationResult validate(const std::map<std::string, ParameterValue>& parameters) override;
    
    // ValidatorNode specific methods
    
    /**
     * @brief Execute validation with specified mode
     */
    ExecutionResult executeValidation(const NodeContext& context, ValidationMode mode);
    
    /**
     * @brief Convert validator violations to node items
     */
    std::vector<NodeItem> convertViolationsToItems(
        const std::vector<akao::core::engine::validator::Violation>& violations) const;
    
    /**
     * @brief Convert validator result to execution result
     */
    ExecutionResult convertValidationResult(
        const akao::core::engine::validator::ValidationResult& validator_result) const;
    
    /**
     * @brief Create validation config from node parameters
     */
    akao::core::engine::validator::ValidationConfig 
    createConfigFromParameters(const NodeContext& context) const;
    
    /**
     * @brief Resolve target path from context (parameter or input items)
     */
    std::string resolveTargetPath(const NodeContext& context) const;
    
    /**
     * @brief Get enabled rule IDs from parameters
     */
    std::vector<std::string> getRuleIdsFromParameters(const NodeContext& context) const;
    
    /**
     * @brief Validate target path exists and is accessible
     */
    bool validateTargetPath(const std::string& target_path, ExecutionResult& result) const;
    
    /**
     * @brief Create violation summary for metadata
     */
    ParameterValue createViolationSummary(
        const std::vector<akao::core::engine::validator::Violation>& violations) const;
    
    /**
     * @brief Convert single violation to NodeItem
     */
    NodeItem convertViolationToItem(
        const akao::core::engine::validator::Violation& violation) const;
    
    /**
     * @brief Get validation statistics as metadata
     */
    std::map<std::string, ParameterValue> getValidationStatistics(
        const akao::core::engine::validator::ValidationResult& validator_result) const;
    
    /**
     * @brief Initialize validator with configuration
     */
    void initializeValidator(const akao::core::engine::validator::ValidationConfig& config);
    
    /**
     * @brief Check if validator is properly initialized
     */
    bool isValidatorReady() const;
    
    /**
     * @brief Get underlying validator for advanced operations
     */
    const akao::core::engine::validator::UnifiedValidator* getValidator() const;
};

/**
 * @brief Register ValidatorNode with registry
 */
void registerValidatorNode(NodeRegistry& registry);

/**
 * @brief Factory function for ValidatorNode creation
 */
std::unique_ptr<INode> createValidatorNode();

} // namespace component
} // namespace evolve
} // namespace akao

#endif // AKAO_EVOLVE_VALIDATOR_NODE_V1_HPP