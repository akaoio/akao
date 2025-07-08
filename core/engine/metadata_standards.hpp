/**
 * @id: akao:file:core:engine:metadata_standards:v1
 * @doc: Core metadata standards implementation providing mandatory identification schema validation and enforcement for all Akao system components
 * @specification: Metadata standards framework with akao namespace validation, mandatory field enforcement, and cross-reference resolution
 * @scope: Core engine metadata validation system for system-wide component identification
 * @timeline: 2025-07-08
 * @rationale: Ensure systematic governance and traceability through mandatory identification standards
 * @methodology: Template-based metadata validation with namespace parsing and automated compliance checking
 * @references: ["akao:artifact:architectural-reformation:phase-2:step-1:identification_schema:v1", "akao:file:core:engine:compliance_validator:v1"]
 */

#ifndef AKAO_CORE_ENGINE_METADATA_STANDARDS_V1_HPP
#define AKAO_CORE_ENGINE_METADATA_STANDARDS_V1_HPP

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <regex>
#include <chrono>

namespace akao {
namespace core {
namespace engine {

/**
 * @id: akao:class:core:engine:akao_namespace:v1
 * @doc: Akao namespace identifier parser and validator for system-wide component identification
 * @specification: Parses and validates akao namespace format with component extraction and validation
 * @scope: Core engine namespace parsing for identification schema compliance
 * @timeline: 2025-07-08
 * @rationale: Provide structured namespace parsing for mandatory identification enforcement
 * @methodology: Regex-based parsing with validation rules and component extraction
 * @references: ["akao:artifact:architectural-reformation:phase-2:step-1:identification_schema:v1"]
 */
class AkaoNamespace {
public:
    enum class ComponentType {
        PHILOSOPHY,
        RULE,
        RULESET,
        ARTIFACT,
        FILE,
        TRACE,
        TEST,
        FUNCTION,
        CLASS,
        DIRECTORY,
        INTERFACE,
        ENGINE,
        FRAMEWORK,
        SYSTEM
    };
    
    struct ParsedNamespace {
        ComponentType type;
        std::string category;
        std::string subcategory;
        std::string name;
        std::string version;
        bool valid;
        std::string error_message;
    };
    
    /**
     * @id: akao:function:core:engine:parse_namespace:v1
     * @doc: Parses akao namespace string into structured components with validation
     * @specification: Parses namespace format akao:[type]:[category]:[subcategory]:[name]:[version]
     * @scope: Namespace parsing for identification schema validation
     * @timeline: 2025-07-08
     * @rationale: Enable structured access to namespace components for validation
     * @methodology: Regex parsing with component validation and error reporting
     * @references: ["akao:class:core:engine:akao_namespace:v1"]
     */
    static ParsedNamespace parseNamespace(const std::string& namespace_id);
    
    /**
     * @id: akao:function:core:engine:validate_namespace_format:v1
     * @doc: Validates akao namespace format compliance with identification schema
     * @specification: Validates namespace format against akao identification pattern requirements
     * @scope: Namespace format validation for compliance enforcement
     * @timeline: 2025-07-08
     * @rationale: Ensure namespace format compliance with mandatory standards
     * @methodology: Regex validation with detailed error reporting
     * @references: ["akao:class:core:engine:akao_namespace:v1"]
     */
    static bool validateNamespaceFormat(const std::string& namespace_id, std::string& error_message);
    
    /**
     * @id: akao:function:core:engine:component_type_to_string:v1
     * @doc: Converts component type enum to string representation for namespace construction
     * @specification: Converts ComponentType enum values to their string equivalents
     * @scope: Component type string conversion for namespace manipulation
     * @timeline: 2025-07-08
     * @rationale: Enable programmatic namespace construction and validation
     * @methodology: Enum to string mapping with validation
     * @references: ["akao:class:core:engine:akao_namespace:v1"]
     */
    static std::string componentTypeToString(ComponentType type);
    
    /**
     * @id: akao:function:core:engine:string_to_component_type:v1
     * @doc: Converts string to component type enum for namespace parsing
     * @specification: Converts string component type to ComponentType enum value
     * @scope: String to enum conversion for namespace parsing
     * @timeline: 2025-07-08
     * @rationale: Enable namespace parsing into structured component types
     * @methodology: String to enum mapping with validation
     * @references: ["akao:class:core:engine:akao_namespace:v1"]
     */
    static std::optional<ComponentType> stringToComponentType(const std::string& type_str);

private:
    static const std::regex namespace_pattern_;
    static const std::map<ComponentType, std::string> type_to_string_;
    static const std::map<std::string, ComponentType> string_to_type_;
};

/**
 * @id: akao:class:core:engine:metadata_schema:v1
 * @doc: Mandatory metadata schema definition and validation for all Akao components
 * @specification: Defines mandatory metadata fields with validation rules and enforcement mechanisms
 * @scope: Core engine metadata schema for system-wide component compliance
 * @timeline: 2025-07-08
 * @rationale: Enforce mandatory metadata requirements for systematic governance
 * @methodology: Schema-based validation with field-specific rules and compliance checking
 * @references: ["akao:artifact:architectural-reformation:phase-2:step-1:identification_schema:v1"]
 */
class MetadataSchema {
public:
    struct MetadataFields {
        std::string specification;
        std::string scope;
        std::string timeline;
        std::string rationale;
        std::string methodology;
        std::vector<std::string> references;
        std::optional<std::string> doc;  // Mandatory for code components
    };
    
    struct ValidationResult {
        bool valid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
    };
    
    /**
     * @id: akao:function:core:engine:validate_metadata:v1
     * @doc: Validates metadata fields against mandatory schema requirements
     * @specification: Validates metadata completeness and content quality against schema rules
     * @scope: Metadata validation for compliance enforcement
     * @timeline: 2025-07-08
     * @rationale: Ensure metadata completeness for systematic governance
     * @methodology: Field-by-field validation with constraint checking and error reporting
     * @references: ["akao:class:core:engine:metadata_schema:v1"]
     */
    static ValidationResult validateMetadata(
        const MetadataFields& metadata, 
        AkaoNamespace::ComponentType component_type
    );
    
    /**
     * @id: akao:function:core:engine:validate_specification:v1
     * @doc: Validates specification field content and length requirements
     * @specification: Validates specification field against 10-100 word constraint and technical precision
     * @scope: Specification field validation for metadata schema compliance
     * @timeline: 2025-07-08
     * @rationale: Ensure specification quality and precision for documentation standards
     * @methodology: Word count validation with content quality assessment
     * @references: ["akao:class:core:engine:metadata_schema:v1"]
     */
    static bool validateSpecification(const std::string& specification, std::string& error_message);
    
    /**
     * @id: akao:function:core:engine:validate_timeline:v1
     * @doc: Validates timeline field format against ISO date requirements
     * @specification: Validates timeline field format as YYYY-MM-DD ISO date
     * @scope: Timeline field validation for metadata schema compliance
     * @timeline: 2025-07-08
     * @rationale: Ensure consistent date formatting for temporal tracking
     * @methodology: ISO date format validation with date validity checking
     * @references: ["akao:class:core:engine:metadata_schema:v1"]
     */
    static bool validateTimeline(const std::string& timeline, std::string& error_message);
    
    /**
     * @id: akao:function:core:engine:validate_references:v1
     * @doc: Validates reference field format and resolvability requirements
     * @specification: Validates references as akao namespace IDs or valid URLs with resolution checking
     * @scope: Reference field validation for metadata schema compliance
     * @timeline: 2025-07-08
     * @rationale: Ensure reference integrity for cross-component traceability
     * @methodology: Format validation with resolution checking and dependency tracking
     * @references: ["akao:class:core:engine:metadata_schema:v1"]
     */
    static bool validateReferences(const std::vector<std::string>& references, std::vector<std::string>& errors);

private:
    static bool isValidIsoDate(const std::string& date_str);
    static bool isValidUrl(const std::string& url);
    static bool isResolvableReference(const std::string& reference);
    static int countWords(const std::string& text);
};

/**
 * @id: akao:class:core:engine:naming_conventions:v1
 * @doc: Naming convention validation and enforcement for files, directories, and code components
 * @specification: Validates naming conventions against akao standards with grammar rule enforcement
 * @scope: Naming convention validation for system-wide component compliance
 * @timeline: 2025-07-08
 * @rationale: Ensure consistent naming for systematic organization and governance
 * @methodology: Pattern-based validation with grammar rule enforcement and style checking
 * @references: ["akao:artifact:architectural-reformation:phase-2:step-1:identification_schema:v1"]
 */
class NamingConventions {
public:
    enum class NamingContext {
        DIRECTORY,
        FILE,
        FUNCTION,
        CLASS,
        VARIABLE
    };
    
    /**
     * @id: akao:function:core:engine:validate_naming:v1
     * @doc: Validates component naming against context-specific conventions
     * @specification: Validates naming compliance with akao naming standards for specific contexts
     * @scope: Component naming validation for convention compliance
     * @timeline: 2025-07-08
     * @rationale: Ensure naming consistency for systematic organization
     * @methodology: Context-specific validation with grammar rule enforcement
     * @references: ["akao:class:core:engine:naming_conventions:v1"]
     */
    static bool validateNaming(
        const std::string& name, 
        NamingContext context, 
        std::string& error_message
    );
    
    /**
     * @id: akao:function:core:engine:validate_directory_naming:v1
     * @doc: Validates directory naming conventions including grammar rules and length constraints
     * @specification: Validates directory names against kebab-case, singular/plural, and length rules
     * @scope: Directory naming validation for filesystem organization compliance
     * @timeline: 2025-07-08
     * @rationale: Ensure directory naming consistency for systematic organization
     * @methodology: Pattern validation with grammar rule checking and length constraints
     * @references: ["akao:class:core:engine:naming_conventions:v1"]
     */
    static bool validateDirectoryNaming(const std::string& name, std::string& error_message);
    
    /**
     * @id: akao:function:core:engine:validate_file_naming:v1
     * @doc: Validates file naming conventions including version patterns and case requirements
     * @specification: Validates file names against version-based naming and kebab-case requirements
     * @scope: File naming validation for filesystem organization compliance
     * @timeline: 2025-07-08
     * @rationale: Ensure file naming consistency for systematic organization
     * @methodology: Pattern validation with version format checking and case validation
     * @references: ["akao:class:core:engine:naming_conventions:v1"]
     */
    static bool validateFileNaming(const std::string& name, std::string& error_message);

private:
    static bool isKebabCase(const std::string& name);
    static bool isCamelCase(const std::string& name);
    static bool isPascalCase(const std::string& name);
    static bool hasVersionPattern(const std::string& name);
    static bool isValidLength(const std::string& name, size_t max_length);
    static bool containsUnderscores(const std::string& name);
};

/**
 * @id: akao:class:core:engine:cross_reference_resolver:v1
 * @doc: Cross-reference resolution system for component dependency tracking and validation
 * @specification: Resolves cross-references between components with dependency tracking and integrity checking
 * @scope: Cross-reference resolution for system-wide component relationship management
 * @timeline: 2025-07-08
 * @rationale: Enable component relationship tracking and dependency management
 * @methodology: Reference resolution with dependency tracking and circular dependency detection
 * @references: ["akao:artifact:architectural-reformation:phase-2:step-1:identification_schema:v1"]
 */
class CrossReferenceResolver {
public:
    struct ReferenceInfo {
        std::string reference_id;
        bool resolvable;
        std::string resolution_path;
        std::string error_message;
    };
    
    struct DependencyGraph {
        std::map<std::string, std::vector<std::string>> forward_dependencies;
        std::map<std::string, std::vector<std::string>> backward_dependencies;
        std::vector<std::vector<std::string>> circular_dependencies;
    };
    
    /**
     * @id: akao:function:core:engine:resolve_reference:v1
     * @doc: Resolves single component reference to verify existence and accessibility
     * @specification: Resolves component reference with existence verification and path resolution
     * @scope: Single reference resolution for component validation
     * @timeline: 2025-07-08
     * @rationale: Verify reference integrity for component dependency validation
     * @methodology: Reference lookup with existence checking and path resolution
     * @references: ["akao:class:core:engine:cross_reference_resolver:v1"]
     */
    static ReferenceInfo resolveReference(const std::string& reference_id);
    
    /**
     * @id: akao:function:core:engine:build_dependency_graph:v1
     * @doc: Builds system-wide dependency graph from component references
     * @specification: Constructs dependency graph with forward/backward relationships and cycle detection
     * @scope: System-wide dependency analysis for reference integrity validation
     * @timeline: 2025-07-08
     * @rationale: Enable dependency analysis and circular dependency prevention
     * @methodology: Graph construction with cycle detection and relationship mapping
     * @references: ["akao:class:core:engine:cross_reference_resolver:v1"]
     */
    static DependencyGraph buildDependencyGraph(const std::vector<std::string>& component_ids);
    
    /**
     * @id: akao:function:core:engine:detect_circular_dependencies:v1
     * @doc: Detects circular dependencies in component reference graph
     * @specification: Identifies circular dependency cycles in component reference relationships
     * @scope: Circular dependency detection for reference integrity validation
     * @timeline: 2025-07-08
     * @rationale: Prevent circular dependencies that compromise system integrity
     * @methodology: Graph cycle detection with path analysis and reporting
     * @references: ["akao:class:core:engine:cross_reference_resolver:v1"]
     */
    static std::vector<std::vector<std::string>> detectCircularDependencies(const DependencyGraph& graph);

private:
    static bool componentExists(const std::string& component_id);
    static std::string resolveComponentPath(const std::string& component_id);
    static void dfsVisit(
        const std::string& node,
        const std::map<std::string, std::vector<std::string>>& graph,
        std::map<std::string, int>& colors,
        std::vector<std::string>& path,
        std::vector<std::vector<std::string>>& cycles
    );
};

} // namespace engine
} // namespace core
} // namespace akao

#endif // AKAO_CORE_ENGINE_METADATA_STANDARDS_V1_HPP
