/**
 * @id akao.evolve.yaml_parser_node.v1
 * @doc YamlParserNode converts the existing YamlParser into the node-based architecture, providing comprehensive YAML parsing capabilities through the INode interface with support for all YAML 1.2 features including scalars, sequences, mappings, and advanced features.
 * @specification Node wrapper for YamlParser enabling YAML parsing workflows in the node-based architecture while maintaining all existing parsing functionality including multi-document support, error handling, and anchor/alias processing.
 * @scope artifacts/evolve/phase2/step2-1-2
 * @timeline 2025-07-10
 * @rationale Enable YAML parsing functionality in node-based workflows while preserving all existing parser capabilities and maintaining compatibility with current YAML processing requirements.
 * @methodology Adapter pattern wrapping YamlParser with INode interface, input/output transformation, and comprehensive error handling while maintaining direct access to all parser features.
 * @references akao.evolve.inode.v1, akao.core.engine.parser.yaml.v1
 */

#ifndef AKAO_EVOLVE_YAML_PARSER_NODE_V1_HPP
#define AKAO_EVOLVE_YAML_PARSER_NODE_V1_HPP

#include "../../phase1/step1-2-1/inode.hpp"
#include "../../phase1/step1-1-1/nodevalue.hpp"
#include "../../phase1/step1-1-2/nodecontext.hpp"
#include "../../phase1/step1-2-2/node_parameter.hpp"
#include "../../phase1/step1-2-1/node_definition.hpp"
#include "../../../../core/engine/parser/yaml/parser/v1.hpp"
#include "../../../../core/engine/parser/yaml/node/v1.hpp"
#include <memory>

namespace akao {
namespace evolve {
namespace component {

// Import node framework types
using namespace akao::node;
using ValidationResult = akao::node::ValidationResult;

/**
 * @brief YAML parsing mode enumeration for YamlParserNode
 */
enum class YamlParsingMode {
    SINGLE_DOCUMENT,    // Parse single YAML document
    MULTI_DOCUMENT,     // Parse multi-document YAML
    FILE_INPUT,         // Parse from file path
    STRING_INPUT,       // Parse from string content
    VALIDATION_ONLY,    // Validate YAML without full parsing
    STRUCTURED_OUTPUT,  // Convert to structured NodeValue
    RAW_NODE_OUTPUT     // Return raw YamlNode objects
};

/**
 * @brief Convert YamlParsingMode to string for parameter handling
 */
std::string yamlParsingModeToString(YamlParsingMode mode);

/**
 * @brief Convert string to YamlParsingMode for parameter parsing
 */
std::optional<YamlParsingMode> stringToYamlParsingMode(const std::string& mode_str);

/**
 * @brief YAML output format enumeration
 */
enum class YamlOutputFormat {
    STRUCTURED,         // Convert to NodeValue structures
    JSON_LIKE,          // JSON-like representation
    FLAT_PROPERTIES,    // Flatten to key-value properties
    NODE_ITEMS,         // Individual NodeItems for each value
    SUMMARY_ONLY,       // Summary of parsing results
    HIERARCHICAL        // Preserve YAML hierarchy
};

/**
 * @brief Convert YamlOutputFormat to string
 */
std::string yamlOutputFormatToString(YamlOutputFormat format);

/**
 * @brief Convert string to YamlOutputFormat
 */
std::optional<YamlOutputFormat> stringToYamlOutputFormat(const std::string& format_str);

/**
 * @brief YamlParserNode - Node wrapper for YamlParser
 */
class YamlParserNode : public INode {
private:
    std::unique_ptr<akao::core::engine::parser::YamlParser> parser_;
    mutable akao::core::engine::parser::YamlParser::ParseOptions cached_options_;
    
public:
    YamlParserNode();
    ~YamlParserNode() override = default;
    
    // INode interface implementation
    std::string getNodeId() const override;
    std::string getNodeType() const override;
    std::string getVersion() const override;
    akao::node::NodeDefinition getDefinition() const override;
    std::string getDisplayName() const override;
    std::string getDescription() const override;
    std::vector<std::string> getCategories() const override;
    ExecutionResult execute(const NodeContext& context) override;
    ValidationResult validate(const std::map<std::string, NodeValue>& parameters) override;
    ValidationResult validateContext(const NodeContext& context) override;
    bool canExecute(const NodeContext& context) override;
    std::unique_ptr<INode> clone() const override;
    
    // YamlParserNode specific methods
    
    /**
     * @brief Execute YAML parsing with specified mode
     */
    ExecutionResult executeYamlParsing(const NodeContext& context, YamlParsingMode mode);
    
    /**
     * @brief Convert YamlNode to NodeValue
     */
    NodeValue convertYamlNodeToNodeValue(
        const std::shared_ptr<akao::core::engine::parser::YamlNode>& yaml_node) const;
    
    /**
     * @brief Convert YamlNode to NodeItem
     */
    NodeItem convertYamlNodeToNodeItem(
        const std::shared_ptr<akao::core::engine::parser::YamlNode>& yaml_node,
        const std::string& key_path = "") const;
    
    /**
     * @brief Convert parsing result to execution result
     */
    ExecutionResult convertParseResult(
        const std::shared_ptr<akao::core::engine::parser::YamlNode>& result,
        YamlOutputFormat format) const;
    
    /**
     * @brief Convert multi-document parsing result to execution result
     */
    ExecutionResult convertMultiDocumentResult(
        const std::vector<std::shared_ptr<akao::core::engine::parser::YamlNode>>& results,
        YamlOutputFormat format) const;
    
    /**
     * @brief Create parse options from node parameters
     */
    akao::core::engine::parser::YamlParser::ParseOptions 
    createOptionsFromParameters(const NodeContext& context) const;
    
    /**
     * @brief Resolve input content from context (parameter or input items)
     */
    std::string resolveInputContent(const NodeContext& context) const;
    
    /**
     * @brief Resolve input file path from context
     */
    std::string resolveInputFilePath(const NodeContext& context) const;
    
    /**
     * @brief Get parsing mode from parameters
     */
    YamlParsingMode getParsingModeFromParameters(const NodeContext& context) const;
    
    /**
     * @brief Get output format from parameters
     */
    YamlOutputFormat getOutputFormatFromParameters(const NodeContext& context) const;
    
    /**
     * @brief Validate YAML content without full parsing
     */
    ExecutionResult validateYamlContent(const std::string& content) const;
    
    /**
     * @brief Create parsing summary for metadata
     */
    NodeValue createParsingSummary(
        const std::shared_ptr<akao::core::engine::parser::YamlNode>& result) const;
    
    /**
     * @brief Convert YamlNode to flat properties
     */
    std::vector<NodeItem> convertToFlatProperties(
        const std::shared_ptr<akao::core::engine::parser::YamlNode>& yaml_node,
        const std::string& prefix = "") const;
    
    /**
     * @brief Get parsing statistics as metadata
     */
    std::map<std::string, NodeValue> getParsingStatistics(
        const std::shared_ptr<akao::core::engine::parser::YamlNode>& result) const;
    
    /**
     * @brief Convert parse error to execution error
     */
    ExecutionError convertParseError(
        const akao::core::engine::parser::YamlParser::ParseError& parse_error) const;
    
    /**
     * @brief Check if parser is properly initialized
     */
    bool isParserReady() const;
    
    /**
     * @brief Get underlying parser for advanced operations
     */
    const akao::core::engine::parser::YamlParser* getParser() const;
};

/**
 * @brief Register YamlParserNode with registry
 */
// Note: NodeRegistry not available, registration done manually

/**
 * @brief Factory function for YamlParserNode creation
 */
std::unique_ptr<INode> createYamlParserNode();

} // namespace component
} // namespace evolve
} // namespace akao

#endif // AKAO_EVOLVE_YAML_PARSER_NODE_V1_HPP