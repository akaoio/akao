/**
 * @id: akao:class:nodes:builtin:logic:v1
 * @doc: .a format executor node using legacy Pure Logic Engine with comprehensive logical expression evaluation. Adapts the proven zero-domain logic engine for node-based architecture while preserving all 47+ built-in functions, variable scoping, and advanced .a format capabilities.
 * @specification: .a format executor node using legacy Pure Logic Engine
 * @scope: Builtin logic execution node
 * @timeline: 2025-07-10
 * @rationale: Leverage existing high-quality Pure Logic Engine for node workflows
 * @methodology: Adapter pattern converting legacy logic engine results to NodeValue format
 * @references: ["akao:interface:core:foundation:interfaces:inode:v1", "akao:file:core:engine:logic_pure:v1"]
 */

#pragma once

#include "core/foundation/interfaces/inode/v1.hpp"
#include "legacy/core/engine/logic/runtime/v1.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace akao {
namespace nodes {
namespace builtin {

/**
 * .a format executor node using legacy Pure Logic Engine
 */
class AkaoLogicExecutorNode : public foundation::interfaces::INode {
private:
    std::string nodeId_;
    std::string nodeType_;
    std::string version_;
    std::string description_;
    
    // Conversion utilities
    akao::logic::Value nodeValueToLogicValue(const foundation::types::NodeValue& value) const;
    foundation::types::NodeValue logicValueToNodeValue(const akao::logic::Value& value) const;
    void populateContextFromParameters(akao::logic::Context& context, const foundation::interfaces::NodeParameters& params) const;
    
public:
    AkaoLogicExecutorNode();
    ~AkaoLogicExecutorNode() = default;
    
    // INode interface implementation
    std::string getNodeId() const override;
    std::string getNodeType() const override;
    std::string getVersion() const override;
    std::string getDescription() const override;
    
    foundation::interfaces::ValidationResult validate(const foundation::interfaces::NodeParameters& params) const override;
    foundation::types::ExecutionResult execute(const foundation::interfaces::NodeContext& context,
                                             const foundation::interfaces::NodeParameters& params) override;
    foundation::types::NodeValue getParameterSchema() const override;
    
    // Static utility methods for direct .a format execution
    static bool executeExpression(const std::string& expression);
    static bool executeExpressionWithContext(const std::string& expression, const std::map<std::string, foundation::types::NodeValue>& variables);
    static foundation::types::NodeValue evaluateExpression(const std::string& expression);
    static std::vector<std::string> validateExpression(const std::string& expression);
    static bool isValidExpression(const std::string& expression);
    static bool isAkaoFormat(const std::string& code);
};

} // namespace builtin
} // namespace nodes
} // namespace akao