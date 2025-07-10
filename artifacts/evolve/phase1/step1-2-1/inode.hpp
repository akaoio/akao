/**
 * @id: akao:file:core:node:interface:inode:v1
 * @doc: Standard node interface definition for Akao node-based workflow system providing unified contract for all node implementations including execution, validation, metadata, and configuration management. Enables consistent node behavior across different implementations and languages.
 * @specification: Universal node interface with execution, validation, and metadata capabilities
 * @scope: Core node interface system
 * @timeline: 2025-07-10
 * @rationale: Provide standardized interface for all node implementations to ensure consistency
 * @methodology: Abstract interface with comprehensive contract definition and type safety
 * @references: ["akao:class:core:node:interface:inode:v1", "akao:class:core:node:interface:nodedefinition:v1"]
 */

#pragma once

#include "../step1-1-1/nodevalue.hpp"
#include "../step1-1-2/nodecontext.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace akao::node {

// Forward declarations
class NodeDefinition;
class ValidationResult;
class NodeParameter;

/**
 * @id: akao:class:core:node:interface:validationresult:v1
 * @doc: Validation result container for node parameter and configuration validation providing detailed error reporting, severity classification, and comprehensive validation status tracking with context information for debugging invalid configurations.
 * @specification: Comprehensive validation result with error details and severity tracking
 * @scope: Core node validation system
 * @timeline: 2025-07-10
 * @rationale: Provide detailed validation feedback for node configuration and parameters
 * @methodology: Result container with error classification and context preservation
 * @references: ["akao:class:core:node:interface:inode:v1"]
 */
class ValidationResult {
public:
    enum class Severity {
        INFO,
        WARNING,
        ERROR,
        FATAL
    };
    
    struct ValidationError {
        std::string parameter_name;
        std::string error_message;
        std::string error_code;
        Severity severity;
        std::string context;
        
        ValidationError(const std::string& param, const std::string& message, 
                       Severity sev = Severity::ERROR, const std::string& code = "")
            : parameter_name(param), error_message(message), error_code(code), severity(sev) {}
    };
    
    ValidationResult() : is_valid_(true) {}
    
    bool isValid() const { return is_valid_; }
    bool hasErrors() const { return !errors_.empty(); }
    bool hasWarnings() const;
    
    void addError(const ValidationError& error);
    void addError(const std::string& param_name, const std::string& message, 
                  Severity severity = Severity::ERROR, const std::string& code = "");
    
    const std::vector<ValidationError>& getErrors() const { return errors_; }
    std::vector<ValidationError> getErrorsBySeverity(Severity severity) const;
    
    size_t getErrorCount() const { return errors_.size(); }
    size_t getWarningCount() const;
    size_t getFatalErrorCount() const;
    
    std::string toString() const;
    
private:
    bool is_valid_;
    std::vector<ValidationError> errors_;
};

/**
 * @id: akao:class:core:node:interface:inode:v1
 * @doc: Standard interface for all node implementations in Akao workflow system defining execution contract, validation requirements, metadata provision, and configuration management. Ensures consistent behavior across C++, Python, JavaScript, and other language implementations.
 * @specification: Universal node interface with execution, validation, and metadata methods
 * @scope: Core node interface definition
 * @timeline: 2025-07-10
 * @rationale: Provide unified contract for all node implementations regardless of language
 * @methodology: Abstract interface with pure virtual methods and comprehensive contract
 * @references: ["akao:class:core:node:execution:nodecontext:v1", "akao:class:core:node:execution:executionresult:v1"]
 */
class INode {
public:
    virtual ~INode() = default;
    
    // Core identification
    virtual std::string getNodeId() const = 0;
    virtual std::string getNodeType() const = 0;
    virtual std::string getVersion() const = 0;
    
    // Node metadata and definition
    virtual NodeDefinition getDefinition() const = 0;
    virtual std::string getDisplayName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::vector<std::string> getCategories() const = 0;
    
    // Core execution method
    virtual ExecutionResult execute(const NodeContext& context) = 0;
    
    // Validation methods
    virtual ValidationResult validate(const std::map<std::string, NodeValue>& parameters) = 0;
    virtual ValidationResult validateContext(const NodeContext& context) = 0;
    virtual bool canExecute(const NodeContext& context) = 0;
    
    // Node lifecycle
    virtual bool initialize() { return true; }
    virtual void shutdown() {}
    virtual bool isInitialized() const { return true; }
    
    // Configuration and parameters
    virtual std::map<std::string, NodeValue> getDefaultParameters() const { return {}; }
    virtual bool supportsStreaming() const { return false; }
    virtual bool supportsBatchProcessing() const { return true; }
    virtual size_t getMaxBatchSize() const { return 1000; }
    
    // Performance and resource information
    virtual size_t getEstimatedMemoryUsage(const NodeContext& context) const;
    virtual std::chrono::milliseconds getEstimatedExecutionTime(const NodeContext& context) const;
    
    // Introspection and debugging
    virtual std::string getImplementationLanguage() const { return "cpp"; }
    virtual std::string getAuthor() const { return "Akao Framework"; }
    virtual std::string getLicense() const { return "MIT"; }
    
    // Clone and factory support
    virtual std::unique_ptr<INode> clone() const = 0;
};

/**
 * @id: akao:class:core:node:interface:nodecapabilities:v1
 * @doc: Node capabilities descriptor defining what features and operations a node supports including streaming, batch processing, error recovery, parallel execution, and resource requirements for optimal workflow orchestration and resource allocation.
 * @specification: Comprehensive node capability description and resource requirements
 * @scope: Core node capability system
 * @timeline: 2025-07-10
 * @rationale: Enable workflow engine to optimize execution based on node capabilities
 * @methodology: Capability flags and resource descriptors for informed scheduling
 * @references: ["akao:class:core:node:interface:nodedefinition:v1"]
 */
struct NodeCapabilities {
    // Processing capabilities
    bool supports_streaming = false;
    bool supports_batch_processing = true;
    bool supports_parallel_execution = false;
    bool supports_incremental_processing = false;
    
    // Error handling capabilities
    bool supports_retry = true;
    bool supports_graceful_degradation = false;
    bool supports_error_recovery = false;
    
    // Data handling capabilities
    bool supports_binary_data = false;
    bool supports_large_datasets = false;
    bool supports_real_time_processing = false;
    
    // Integration capabilities
    bool supports_akao_logic = true;
    bool supports_external_apis = false;
    bool supports_file_system = false;
    bool supports_network_access = false;
    
    // Resource requirements
    size_t max_memory_mb = 256;
    size_t max_execution_time_seconds = 300;
    size_t recommended_batch_size = 100;
    
    // Security requirements
    bool requires_elevated_permissions = false;
    bool requires_network_access = false;
    bool requires_file_system_access = false;
    
    std::string toString() const;
};

/**
 * @id: akao:class:core:node:interface:nodeexample:v1
 * @doc: Example usage container for node documentation providing sample input data, expected output, configuration examples, and common use cases to help users understand node behavior and proper configuration patterns.
 * @specification: Node usage examples with input/output samples and configurations
 * @scope: Core node documentation system
 * @timeline: 2025-07-10
 * @rationale: Provide comprehensive usage examples for node documentation and testing
 * @methodology: Example container with realistic data samples and configurations
 * @references: ["akao:class:core:node:interface:nodedefinition:v1"]
 */
struct NodeExample {
    std::string title;
    std::string description;
    std::map<std::string, NodeValue> input_parameters;
    std::vector<NodeItem> sample_input_data;
    std::vector<NodeItem> expected_output_data;
    std::string use_case_description;
    
    NodeExample(const std::string& title_, const std::string& description_)
        : title(title_), description(description_) {}
};

/**
 * @id: akao:class:core:node:interface:nodedefinition:v1
 * @doc: Complete node definition containing all metadata, parameters, capabilities, examples, and configuration information required for node registration, documentation generation, UI rendering, and workflow execution. Serves as comprehensive node specification.
 * @specification: Complete node specification with metadata, parameters, and capabilities
 * @scope: Core node definition system
 * @timeline: 2025-07-10
 * @rationale: Provide comprehensive node specification for registration and documentation
 * @methodology: Complete definition container with all node aspects and metadata
 * @references: ["akao:class:core:node:interface:inode:v1", "akao:class:core:node:interface:nodeparameter:v1"]
 */
class NodeDefinition {
public:
    NodeDefinition(const std::string& node_id, const std::string& node_type, const std::string& version);
    
    // Basic identification
    std::string getNodeId() const { return node_id_; }
    std::string getNodeType() const { return node_type_; }
    std::string getVersion() const { return version_; }
    
    // Display information
    std::string getDisplayName() const { return display_name_; }
    void setDisplayName(const std::string& name) { display_name_ = name; }
    
    std::string getDescription() const { return description_; }
    void setDescription(const std::string& desc) { description_ = desc; }
    
    std::string getDetailedDescription() const { return detailed_description_; }
    void setDetailedDescription(const std::string& desc) { detailed_description_ = desc; }
    
    // Categorization
    const std::vector<std::string>& getCategories() const { return categories_; }
    void addCategory(const std::string& category) { categories_.push_back(category); }
    void setCategories(const std::vector<std::string>& categories) { categories_ = categories; }
    
    const std::vector<std::string>& getTags() const { return tags_; }
    void addTag(const std::string& tag) { tags_.push_back(tag); }
    void setTags(const std::vector<std::string>& tags) { tags_ = tags; }
    
    // Parameters
    const std::vector<NodeParameter>& getParameters() const { return parameters_; }
    void addParameter(const NodeParameter& param) { parameters_.push_back(param); }
    void setParameters(const std::vector<NodeParameter>& params) { parameters_ = params; }
    
    // Capabilities
    const NodeCapabilities& getCapabilities() const { return capabilities_; }
    void setCapabilities(const NodeCapabilities& caps) { capabilities_ = caps; }
    
    // Examples
    const std::vector<NodeExample>& getExamples() const { return examples_; }
    void addExample(const NodeExample& example) { examples_.push_back(example); }
    
    // Metadata
    std::string getAuthor() const { return author_; }
    void setAuthor(const std::string& author) { author_ = author; }
    
    std::string getLicense() const { return license_; }
    void setLicense(const std::string& license) { license_ = license; }
    
    std::string getDocumentationUrl() const { return documentation_url_; }
    void setDocumentationUrl(const std::string& url) { documentation_url_ = url; }
    
    std::string getRepositoryUrl() const { return repository_url_; }
    void setRepositoryUrl(const std::string& url) { repository_url_ = url; }
    
    // Visual metadata (for UI)
    std::string getIconUrl() const { return icon_url_; }
    void setIconUrl(const std::string& url) { icon_url_ = url; }
    
    std::string getColor() const { return color_; }
    void setColor(const std::string& color) { color_ = color; }
    
    // Validation
    ValidationResult validate() const;
    bool isValid() const { return validate().isValid(); }
    
    // Serialization
    std::string toJson() const;
    static NodeDefinition fromJson(const std::string& json);
    std::string toString() const;

private:
    // Core identification
    std::string node_id_;
    std::string node_type_;
    std::string version_;
    
    // Display information
    std::string display_name_;
    std::string description_;
    std::string detailed_description_;
    
    // Categorization
    std::vector<std::string> categories_;
    std::vector<std::string> tags_;
    
    // Configuration
    std::vector<NodeParameter> parameters_;
    NodeCapabilities capabilities_;
    std::vector<NodeExample> examples_;
    
    // Metadata
    std::string author_;
    std::string license_;
    std::string documentation_url_;
    std::string repository_url_;
    
    // Visual metadata
    std::string icon_url_;
    std::string color_;
};

/**
 * @id: akao:class:core:node:interface:noderegistry_helper:v1
 * @doc: Helper utilities for node registration and management providing factory functions, type checking, validation utilities, and node discovery mechanisms to support the main NodeRegistry implementation.
 * @specification: Helper utilities for node registration and type management
 * @scope: Core node registration utilities
 * @timeline: 2025-07-10
 * @rationale: Provide utility functions for node registration and management
 * @methodology: Static utility functions and helper classes for registration support
 * @references: ["akao:class:core:node:interface:inode:v1"]
 */
namespace NodeRegistryHelpers {
    // Type checking utilities
    bool isValidNodeId(const std::string& node_id);
    bool isValidNodeType(const std::string& node_type);
    bool isValidVersion(const std::string& version);
    
    // Node factory utilities
    using NodeFactory = std::function<std::unique_ptr<INode>()>;
    std::map<std::string, NodeFactory>& getNodeFactories();
    void registerNodeFactory(const std::string& node_type, NodeFactory factory);
    
    // Validation utilities
    ValidationResult validateNodeDefinition(const NodeDefinition& definition);
    ValidationResult validateNodeImplementation(const INode& node);
    
    // Discovery utilities
    std::vector<std::string> discoverAvailableNodeTypes();
    std::vector<NodeDefinition> loadNodeDefinitions(const std::string& directory);
}

} // namespace akao::node