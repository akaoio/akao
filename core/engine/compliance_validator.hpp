/**
 * @id: akao:file:core:engine:compliance_validator_header:v1
 * @doc: Header file for Akao compliance validation system providing systematic governance and archaeological preservation through automated compliance checking
 * @specification: Compliance validator header with validation framework declarations and systematic governance capabilities
 * @scope: Core engine compliance validation header for systematic governance enforcement
 * @timeline: 2025-01-08
 * @rationale: Provide header declarations for compliance validation supporting systematic governance and archaeological preservation
 * @methodology: Header-only declarations with implementation separation for clean build architecture
 * @references: ["akao:file:core:engine:compliance_validator", "akao:file:core:engine:metadata_standards"]
 */

#ifndef AKAO_CORE_ENGINE_COMPLIANCE_VALIDATOR_HPP
#define AKAO_CORE_ENGINE_COMPLIANCE_VALIDATOR_HPP

#include <string>
#include <vector>
#include <map>
#include "metadata_standards.hpp"

namespace akao::core::engine {

class ComplianceValidator {
public:
    struct ValidationIssue {
        std::string type;
        std::string severity;
        std::string message;
        std::string file_path;
        int line_number;
        std::string suggestion;
    };
    
    struct ComplianceReport {
        bool overall_compliance;
        double compliance_percentage;
        std::vector<ValidationIssue> critical_issues;
        std::vector<ValidationIssue> warnings;
        std::vector<ValidationIssue> suggestions;
        std::map<std::string, int> issue_counts;
        std::string summary;
    };
    
    ComplianceValidator();
    ~ComplianceValidator();
    
    // Main validation methods
    ComplianceReport validateSystemCompliance(const std::string& root_path);
    ValidationIssue validateFileCompliance(const std::string& file_path);
    
    // Metadata extraction and validation
    MetadataSchema::MetadataFields extractMetadataFromFile(const std::string& file_path);
    
    // Report generation
    std::string generateComplianceReport(const ComplianceReport& report);
    
private:
    // Helper methods (to be implemented)
    std::string extractFieldValue(const std::string& content, const std::string& field_name);
    std::vector<std::string> extractReferences(const std::string& content);
    AkaoNamespace::ComponentType detectComponentType(const std::string& file_path);
    bool checkNamingConventions(const std::string& file_path);
    bool isCodeFile(const std::string& file_path);
    bool hasRequiredHeaderFormat(const std::string& file_path);
    std::vector<ValidationIssue> checkMetadataCompleteness(const MetadataSchema::MetadataFields& metadata, AkaoNamespace::ComponentType type);
    std::string extractNamespaceId(const std::string& content);
    bool hasValidNamespaceId(const std::string& file_path);
    std::vector<std::string> getAllFiles(const std::string& root_path);
    std::vector<std::string> getAllDirectories(const std::string& root_path);
    std::vector<ValidationIssue> validateDirectoryStructure(const std::string& root_path);
    void categorizeIssues(const std::vector<ValidationIssue>& issues, ComplianceReport& report);
    double calculateCompliancePercentage(const ComplianceReport& report);
    std::string formatIssueDescription(const ValidationIssue& issue);
};

} // namespace akao::core::engine

#endif // AKAO_CORE_ENGINE_COMPLIANCE_VALIDATOR_HPP
