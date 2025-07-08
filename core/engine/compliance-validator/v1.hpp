/**
 * @id: akao:file:core:engine:compliance_validator:v1
 * @doc: Automated compliance validation system for enforcing identification standards and metadata requirements across all Akao components
 * @specification: Comprehensive compliance validation with automated checking, enforcement mechanisms, and violation reporting
 * @scope: Core engine compliance validation for system-wide governance enforcement
 * @timeline: 2025-07-08
 * @rationale: Ensure continuous compliance adherence and prevent governance degradation through automation
 * @methodology: Multi-stage validation pipeline with blocking errors, quality warnings, and automated enforcement
 * @references: ["akao:file:core:engine:metadata_standards:v1", "akao:artifact:architectural-reformation:phase-2:step-1:identification_schema:v1"]
 */

#ifndef AKAO_CORE_ENGINE_COMPLIANCE_VALIDATOR_V1_HPP
#define AKAO_CORE_ENGINE_COMPLIANCE_VALIDATOR_V1_HPP

#include "../metadata-standards/v1.hpp"
#include <string>
#include <vector>
#include <optional>

namespace akao {
namespace core {
namespace engine {

/**
 * @id: akao:class:core:engine:compliance_validator:v1
 * @doc: Automated compliance validation system with comprehensive checking and enforcement capabilities
 * @specification: Multi-level compliance validation with blocking errors, quality warnings, and automated enforcement
 * @scope: Core engine compliance validation for system-wide governance enforcement
 * @timeline: 2025-07-08
 * @rationale: Automate compliance checking to prevent governance degradation and ensure standards adherence
 * @methodology: Multi-stage validation pipeline with configurable enforcement levels and detailed reporting
 * @references: ["akao:file:core:engine:metadata_standards:v1"]
 */
class ComplianceValidator {
public:
    enum class ValidationLevel {
        BLOCKING_ERRORS,      // Must fix before proceeding
        QUALITY_WARNINGS,     // Should fix for quality
        BEST_PRACTICES       // Recommended improvements
    };
    
    struct ValidationIssue {
        ValidationLevel level;
        std::string component_id;
        std::string issue_type;
        std::string description;
        std::string file_path;
        int line_number;
        std::string suggested_fix;
    };
    
    struct ComplianceReport {
        int total_components_scanned;
        int compliant_components;
        int non_compliant_components;
        std::vector<ValidationIssue> blocking_errors;
        std::vector<ValidationIssue> quality_warnings;
        std::vector<ValidationIssue> best_practice_suggestions;
        double compliance_percentage;
        bool system_compliant;
        
        // Legacy compatibility fields
        bool overall_compliance;
        std::vector<ValidationIssue> critical_issues;
        std::vector<ValidationIssue> warnings;
    };
    
    /**
     * @id: akao:function:core:engine:validate_system_compliance:v1
     * @doc: Performs comprehensive system-wide compliance validation with detailed reporting
     * @specification: Scans entire system for compliance violations with multi-level issue classification
     * @scope: System-wide compliance validation for complete governance enforcement
     * @timeline: 2025-07-08
     * @rationale: Ensure comprehensive compliance monitoring across all system components
     * @methodology: Recursive filesystem scanning with component-by-component validation and aggregated reporting
     * @references: ["akao:class:core:engine:compliance_validator:v1"]
     */
    static ComplianceReport validateSystemCompliance(const std::string& root_path);
    
    /**
     * @id: akao:function:core:engine:validate_component_compliance:v1
     * @doc: Validates individual component compliance against identification and metadata standards
     * @specification: Component-specific compliance validation with detailed issue identification
     * @scope: Individual component compliance validation for targeted enforcement
     * @timeline: 2025-07-08
     * @rationale: Enable focused compliance validation for specific components
     * @methodology: Component analysis with metadata extraction and standards verification
     * @references: ["akao:class:core:engine:compliance_validator:v1"]
     */
    static std::vector<ValidationIssue> validateComponentCompliance(
        const std::string& file_path,
        AkaoNamespace::ComponentType expected_type
    );
    
    /**
     * @id: akao:function:core:engine:validate_directory_structure:v1
     * @doc: Validates directory structure compliance with naming conventions and metadata requirements
     * @specification: Directory structure validation with naming convention checking and metadata file verification
     * @scope: Directory structure compliance validation for filesystem organization
     * @timeline: 2025-07-08
     * @rationale: Ensure directory organization compliance with akao standards
     * @methodology: Directory traversal with naming validation and metadata file checking
     * @references: ["akao:class:core:engine:compliance_validator:v1"]
     */
    static std::vector<ValidationIssue> validateDirectoryStructure(const std::string& directory_path);
    
    /**
     * @id: akao:function:core:engine:validate_file_compliance:v1
     * @doc: Validates individual file compliance including naming, metadata, and content standards
     * @specification: File compliance validation with header analysis, naming verification, and content checking
     * @scope: Individual file compliance validation for content and format standards
     * @timeline: 2025-07-08
     * @rationale: Ensure file-level compliance with identification and metadata requirements
     * @methodology: File parsing with header analysis, naming validation, and content verification
     * @references: ["akao:class:core:engine:compliance_validator:v1"]
     */
    static std::vector<ValidationIssue> validateFileCompliance(const std::string& file_path);
    
    /**
     * @id: akao:function:core:engine:extract_metadata_from_file:v1
     * @doc: Extracts metadata from file headers for compliance validation
     * @specification: Parses file headers to extract akao namespace identification and metadata fields
     * @scope: Metadata extraction for file compliance validation
     * @timeline: 2025-07-08
     * @rationale: Enable automated metadata extraction for compliance checking
     * @methodology: Header parsing with pattern matching and field extraction
     * @references: ["akao:class:core:engine:compliance_validator:v1"]
     */
    static std::optional<MetadataSchema::MetadataFields> extractMetadataFromFile(const std::string& file_path);
    
    /**
     * @id: akao:function:core:engine:generate_compliance_report:v1
     * @doc: Generates comprehensive compliance report with statistics and actionable recommendations
     * @specification: Creates detailed compliance report with issue categorization and improvement recommendations
     * @scope: Compliance reporting for governance monitoring and improvement planning
     * @timeline: 2025-07-08
     * @rationale: Provide actionable compliance insights for systematic improvement
     * @methodology: Issue aggregation with statistical analysis and recommendation generation
     * @references: ["akao:class:core:engine:compliance_validator:v1"]
     */
    static std::string generateComplianceReport(const ComplianceReport& report);
    
    /**
     * @id: akao:function:core:engine:enforce_compliance:v1
     * @doc: Enforces compliance by blocking operations for components with blocking errors
     * @specification: Implements compliance enforcement with configurable blocking mechanisms
     * @scope: Compliance enforcement for preventing non-compliant component operations
     * @timeline: 2025-07-08
     * @rationale: Prevent system degradation through automated compliance enforcement
     * @methodology: Validation-based blocking with configurable enforcement levels
     * @references: ["akao:class:core:engine:compliance_validator:v1"]
     */
    static bool enforceCompliance(
        const std::string& component_path,
        ValidationLevel minimum_level = ValidationLevel::BLOCKING_ERRORS
    );

private:
    // Helper functions for metadata extraction
    static std::string extractFieldValue(const std::string& content, const std::string& field_name);
    static std::vector<std::string> extractReferences(const std::string& content);
    static std::string extractNamespaceId(const std::string& content);
    
    // File type detection
    static AkaoNamespace::ComponentType detectComponentType(const std::string& file_path);
    static bool isCodeFile(const std::string& file_path);
    static bool isDocumentationFile(const std::string& file_path);
    static bool isConfigFile(const std::string& file_path);
    
    // Validation helpers
    static bool hasRequiredHeaderFormat(const std::string& file_content);
    static bool hasValidNamespaceId(const std::string& file_content);
    static std::vector<std::string> checkNamingConventions(const std::string& file_path);
    static std::vector<std::string> checkMetadataCompleteness(
        const MetadataSchema::MetadataFields& metadata,
        AkaoNamespace::ComponentType component_type
    );
    
    // Filesystem helpers
    static std::vector<std::string> getAllFiles(const std::string& directory_path);
    static std::vector<std::string> getAllDirectories(const std::string& directory_path);
    static bool hasDirectoryMetadata(const std::string& directory_path);
    
    // Report generation helpers
    static void categorizeIssues(
        const std::vector<ValidationIssue>& all_issues,
        ComplianceReport& report
    );
    static double calculateCompliancePercentage(const ComplianceReport& report);
    static std::string formatIssueDescription(const ValidationIssue& issue);
};

} // namespace engine
} // namespace core
} // namespace akao

#endif // AKAO_CORE_ENGINE_COMPLIANCE_VALIDATOR_V1_HPP
