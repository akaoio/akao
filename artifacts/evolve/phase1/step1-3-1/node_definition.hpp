/**
 * @id akao.evolve.node_definition.v1
 * @doc Node definition structure providing comprehensive metadata for node registration including parameters, display information, versioning, and capabilities. Essential for node discovery and validation in the registry system.
 * @specification Metadata container defining node identity, display properties, parameter specifications, version information, and execution capabilities with complete type safety and validation support.
 * @scope artifacts/evolve/phase1/step1-3-1
 * @timeline 2025-07-10
 * @rationale Enable comprehensive node metadata management for registry discovery, validation, and UI generation while maintaining Akao's philosophy-driven architecture.
 * @methodology Object-oriented metadata definition with parameter integration, versioning support, and capability flags for robust node management.
 * @references akao.evolve.node_parameter.v1, akao.evolve.node_registry.v1
 */

#ifndef AKAO_EVOLVE_NODE_DEFINITION_V1_HPP
#define AKAO_EVOLVE_NODE_DEFINITION_V1_HPP

#include <string>
#include <vector>
#include <map>
#include <optional>

// Forward declare ParameterSet from node_parameter.hpp
namespace akao {
namespace evolve {
    class ParameterSet;
    class NodeParameter;
}
}

namespace akao {
namespace evolve {

/**
 * @brief Node category enumeration for organization
 */
enum class NodeCategory {
    CORE,           // Core system nodes (built-in)
    FILESYSTEM,     // File system operations
    PARSER,         // Data parsing and transformation
    ANALYSIS,       // Code and data analysis
    VALIDATION,     // Validation and compliance
    CONTROL,        // Flow control and logic
    DATA,           // Data manipulation
    IO,             // Input/output operations
    AKAO,           // Akao-specific functionality
    COMMUNITY,      // Community-contributed nodes
    CUSTOM          // User-defined nodes
};

/**
 * @brief Node execution mode flags
 */
struct NodeCapabilities {
    bool supports_parallel_execution = true;
    bool requires_filesystem_access = false;
    bool modifies_filesystem = false;
    bool requires_network_access = false;
    bool is_deterministic = true;
    bool supports_streaming = false;
    bool requires_akao_engine = false;
    
    NodeCapabilities() = default;
    
    static NodeCapabilities safe() {
        NodeCapabilities caps;
        caps.supports_parallel_execution = true;
        caps.requires_filesystem_access = false;
        caps.modifies_filesystem = false;
        caps.requires_network_access = false;
        caps.is_deterministic = true;
        caps.supports_streaming = false;
        caps.requires_akao_engine = false;
        return caps;
    }
    
    static NodeCapabilities filesystem() {
        NodeCapabilities caps = safe();
        caps.requires_filesystem_access = true;
        return caps;
    }
    
    static NodeCapabilities akao() {
        NodeCapabilities caps = safe();
        caps.requires_akao_engine = true;
        return caps;
    }
};

/**
 * @brief Version information for node compatibility
 */
struct NodeVersion {
    int major = 1;
    int minor = 0;
    int patch = 0;
    std::string pre_release;
    
    NodeVersion() = default;
    NodeVersion(int maj, int min, int pat) : major(maj), minor(min), patch(pat) {}
    NodeVersion(int maj, int min, int pat, const std::string& pre) 
        : major(maj), minor(min), patch(pat), pre_release(pre) {}
    
    std::string toString() const {
        std::string version = std::to_string(major) + "." + 
                             std::to_string(minor) + "." + 
                             std::to_string(patch);
        if (!pre_release.empty()) {
            version += "-" + pre_release;
        }
        return version;
    }
    
    bool isCompatible(const NodeVersion& other) const {
        // Special case: (0,0,0) means no requirement, always compatible
        if (major == 0 && minor == 0 && patch == 0) return true;
        
        // This version is compatible with 'other' if they have the same major version
        // and other has equal or higher minor version
        return major == other.major && minor <= other.minor;
    }
    
    bool operator<(const NodeVersion& other) const {
        if (major != other.major) return major < other.major;
        if (minor != other.minor) return minor < other.minor;
        if (patch != other.patch) return patch < other.patch;
        return pre_release < other.pre_release;
    }
    
    bool operator==(const NodeVersion& other) const {
        return major == other.major && minor == other.minor && 
               patch == other.patch && pre_release == other.pre_release;
    }
};

/**
 * @brief Comprehensive node definition for registry management
 */
class NodeDefinition {
public:
    // Core identity
    std::string node_id;              // Unique identifier (e.g., "akao.validator.v1")
    std::string node_type;            // Type identifier for instantiation
    std::string display_name;         // Human-readable name
    std::string description;          // Detailed description
    
    // Organization and discovery
    NodeCategory category;            // Organizational category
    std::vector<std::string> tags;    // Search and filter tags
    std::string author;               // Node author/maintainer
    std::string license;              // License information
    
    // Version and compatibility
    NodeVersion version;              // Node version
    NodeVersion min_akao_version;     // Minimum required Akao version
    
    // Execution properties
    NodeCapabilities capabilities;    // Execution capabilities and requirements
    std::optional<int> max_parallel_instances; // Maximum parallel instances
    std::optional<int> timeout_seconds;        // Execution timeout
    
    // Parameters and configuration
    std::unique_ptr<ParameterSet> parameters; // Node parameters
    
    // Documentation and help
    std::string help_text;            // Extended help documentation
    std::string example_usage;        // Usage examples
    std::vector<std::string> related_nodes; // Related node suggestions
    
    // Validation and rules
    std::string akao_validation_rule; // .a format validation rule
    std::map<std::string, std::string> metadata; // Additional metadata
    
public:
    NodeDefinition() : category(NodeCategory::CUSTOM), parameters(nullptr) {}
    
    NodeDefinition(const std::string& id, const std::string& type, const std::string& name)
        : node_id(id), node_type(type), display_name(name), 
          category(NodeCategory::CUSTOM), parameters(nullptr) {}
    
    // Copy constructor
    NodeDefinition(const NodeDefinition& other);
    
    // Move constructor
    NodeDefinition(NodeDefinition&& other) noexcept;
    
    // Copy assignment
    NodeDefinition& operator=(const NodeDefinition& other);
    
    // Move assignment
    NodeDefinition& operator=(NodeDefinition&& other) noexcept;
    
    // Destructor
    ~NodeDefinition() = default;
    
    /**
     * @brief Set node parameters from ParameterSet
     */
    void setParameters(std::unique_ptr<ParameterSet> params);
    
    /**
     * @brief Get node parameters (const access)
     */
    const ParameterSet* getParameters() const;
    
    /**
     * @brief Check if node has parameters defined
     */
    bool hasParameters() const { return parameters != nullptr; }
    
    /**
     * @brief Add a tag for search and filtering
     */
    NodeDefinition& addTag(const std::string& tag);
    
    /**
     * @brief Set node category
     */
    NodeDefinition& setCategory(NodeCategory cat);
    
    /**
     * @brief Set node capabilities
     */
    NodeDefinition& setCapabilities(const NodeCapabilities& caps);
    
    /**
     * @brief Set version information
     */
    NodeDefinition& setVersion(const NodeVersion& ver);
    
    /**
     * @brief Set minimum Akao version requirement
     */
    NodeDefinition& setMinAkaoVersion(const NodeVersion& ver);
    
    /**
     * @brief Set author and license information
     */
    NodeDefinition& setAuthor(const std::string& author_name, const std::string& license_info = "");
    
    /**
     * @brief Set node description
     */
    NodeDefinition& setDescription(const std::string& desc);
    
    /**
     * @brief Set extended documentation
     */
    NodeDefinition& setDocumentation(const std::string& help, const std::string& example = "");
    
    /**
     * @brief Add related node suggestion
     */
    NodeDefinition& addRelatedNode(const std::string& node_id);
    
    /**
     * @brief Set Akao validation rule
     */
    NodeDefinition& setAkaoValidation(const std::string& rule);
    
    /**
     * @brief Add custom metadata
     */
    NodeDefinition& addMetadata(const std::string& key, const std::string& value);
    
    /**
     * @brief Get custom metadata value
     */
    std::optional<std::string> getMetadata(const std::string& key) const;
    
    /**
     * @brief Validate definition completeness
     */
    bool isValid() const;
    
    /**
     * @brief Get validation errors
     */
    std::vector<std::string> getValidationErrors() const;
    
    /**
     * @brief Check compatibility with Akao version
     */
    bool isCompatibleWith(const NodeVersion& akao_version) const;
    
    /**
     * @brief Generate summary string for display
     */
    std::string getSummary() const;
    
    /**
     * @brief Factory methods for common node types
     */
    static NodeDefinition createCoreNode(const std::string& id, const std::string& name);
    static NodeDefinition createFilesystemNode(const std::string& id, const std::string& name);
    static NodeDefinition createParserNode(const std::string& id, const std::string& name);
    static NodeDefinition createAkaoNode(const std::string& id, const std::string& name);
    static NodeDefinition createAnalysisNode(const std::string& id, const std::string& name);
    static NodeDefinition createValidationNode(const std::string& id, const std::string& name);
};

/**
 * @brief Convert category to string for display
 */
std::string categoryToString(NodeCategory category);

/**
 * @brief Convert string to category for parsing
 */
std::optional<NodeCategory> stringToCategory(const std::string& category_str);

} // namespace evolve
} // namespace akao

#endif // AKAO_EVOLVE_NODE_DEFINITION_V1_HPP