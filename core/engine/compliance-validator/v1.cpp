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

#include "../metadata-standards/v1.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <sstream>
#include <regex>
#include <set>
#include <chrono>
#include <iomanip>
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

// Implementation of key methods

ComplianceValidator::ComplianceReport ComplianceValidator::validateSystemCompliance(const std::string& root_path) {
    ComplianceReport report = {};
    std::vector<ValidationIssue> all_issues;
    
    try {
        // Scan all files in the system
        auto all_files = getAllFiles(root_path);
        report.total_components_scanned = all_files.size();
        
        for (const auto& file_path : all_files) {
            auto file_issues = validateFileCompliance(file_path);
            all_issues.insert(all_issues.end(), file_issues.begin(), file_issues.end());
            
            // Check if component is compliant (no blocking errors)
            bool has_blocking_errors = std::any_of(file_issues.begin(), file_issues.end(),
                [](const ValidationIssue& issue) {
                    return issue.level == ValidationLevel::BLOCKING_ERRORS;
                });
            
            if (has_blocking_errors) {
                report.non_compliant_components++;
            } else {
                report.compliant_components++;
            }
        }
        
        // Scan all directories
        auto all_directories = getAllDirectories(root_path);
        for (const auto& dir_path : all_directories) {
            auto dir_issues = validateDirectoryStructure(dir_path);
            all_issues.insert(all_issues.end(), dir_issues.begin(), dir_issues.end());
        }
        
        // Categorize issues and calculate metrics
        categorizeIssues(all_issues, report);
        report.compliance_percentage = calculateCompliancePercentage(report);
        report.system_compliant = report.blocking_errors.empty();
        
    } catch (const std::exception& e) {
        // Add error to report
        ValidationIssue system_error = {
            ValidationLevel::BLOCKING_ERRORS,
            "system",
            "validation_error",
            "System validation failed: " + std::string(e.what()),
            root_path,
            0,
            "Check system accessibility and permissions"
        };
        report.blocking_errors.push_back(system_error);
        report.system_compliant = false;
    }
    
    return report;
}

std::vector<ComplianceValidator::ValidationIssue> ComplianceValidator::validateFileCompliance(const std::string& file_path) {
    std::vector<ValidationIssue> issues;
    
    try {
        // Read file content
        std::ifstream file(file_path);
        if (!file.is_open()) {
            issues.push_back({
                ValidationLevel::BLOCKING_ERRORS,
                "unknown",
                "file_access",
                "Cannot read file: " + file_path,
                file_path,
                0,
                "Check file permissions and existence"
            });
            return issues;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        
        // Detect component type
        auto component_type = detectComponentType(file_path);
        
        // Check naming conventions
        auto naming_issues = checkNamingConventions(file_path);
        for (const auto& issue_desc : naming_issues) {
            issues.push_back({
                ValidationLevel::BLOCKING_ERRORS,
                "unknown",
                "naming_convention",
                issue_desc,
                file_path,
                0,
                "Rename file according to akao naming conventions"
            });
        }
        
        // Check for required header format (for code files)
        if (isCodeFile(file_path)) {
            if (!hasRequiredHeaderFormat(content)) {
                issues.push_back({
                    ValidationLevel::BLOCKING_ERRORS,
                    "unknown",
                    "missing_header",
                    "Missing required akao header format",
                    file_path,
                    1,
                    "Add complete akao namespace header with metadata"
                });
                return issues; // Can't proceed without header
            }
            
            // Extract and validate metadata
            auto metadata_opt = extractMetadataFromFile(file_path);
            if (!metadata_opt.has_value()) {
                issues.push_back({
                    ValidationLevel::BLOCKING_ERRORS,
                    "unknown",
                    "metadata_extraction",
                    "Failed to extract metadata from file header",
                    file_path,
                    1,
                    "Ensure complete akao metadata in file header"
                });
                return issues;
            }
            
            auto metadata_issues = checkMetadataCompleteness(metadata_opt.value(), component_type);
            for (const auto& issue_desc : metadata_issues) {
                issues.push_back({
                    ValidationLevel::BLOCKING_ERRORS,
                    extractNamespaceId(content),
                    "metadata_incomplete",
                    issue_desc,
                    file_path,
                    1,
                    "Complete missing metadata fields"
                });
            }
            
            // Validate namespace ID format
            if (!hasValidNamespaceId(content)) {
                issues.push_back({
                    ValidationLevel::BLOCKING_ERRORS,
                    "unknown",
                    "invalid_namespace",
                    "Invalid or missing akao namespace ID",
                    file_path,
                    1,
                    "Add valid akao namespace ID in header"
                });
            }
        }
        
    } catch (const std::exception& e) {
        issues.push_back({
            ValidationLevel::BLOCKING_ERRORS,
            "unknown",
            "validation_error",
            "File validation failed: " + std::string(e.what()),
            file_path,
            0,
            "Check file format and accessibility"
        });
    }
    
    return issues;
}

std::optional<MetadataSchema::MetadataFields> ComplianceValidator::extractMetadataFromFile(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        return std::nullopt;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    MetadataSchema::MetadataFields metadata;
    
    // Extract metadata fields using regex patterns
    metadata.specification = extractFieldValue(content, "@specification");
    metadata.scope = extractFieldValue(content, "@scope");
    metadata.timeline = extractFieldValue(content, "@timeline");
    metadata.rationale = extractFieldValue(content, "@rationale");
    metadata.methodology = extractFieldValue(content, "@methodology");
    metadata.references = extractReferences(content);
    
    // Extract doc field if present
    std::string doc_value = extractFieldValue(content, "@doc");
    if (!doc_value.empty()) {
        metadata.doc = doc_value;
    }
    
    // Validate that we extracted at least some metadata
    if (metadata.specification.empty() && metadata.scope.empty() && 
        metadata.timeline.empty() && metadata.rationale.empty()) {
        return std::nullopt;
    }
    
    return metadata;
}

std::string ComplianceValidator::generateComplianceReport(const ComplianceReport& report) {
    std::ostringstream oss;
    
    oss << "=== AKAO SYSTEM COMPLIANCE REPORT ===\n\n";
    oss << "Scan Date: " << std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count() << "\n";
    oss << "System Status: " << (report.system_compliant ? "COMPLIANT" : "NON-COMPLIANT") << "\n\n";
    
    oss << "SUMMARY STATISTICS:\n";
    oss << "- Total Components Scanned: " << report.total_components_scanned << "\n";
    oss << "- Compliant Components: " << report.compliant_components << "\n";
    oss << "- Non-Compliant Components: " << report.non_compliant_components << "\n";
    oss << "- Compliance Percentage: " << std::fixed << std::setprecision(1) 
        << report.compliance_percentage << "%\n\n";
    
    if (!report.blocking_errors.empty()) {
        oss << "BLOCKING ERRORS (MUST FIX):\n";
        for (const auto& issue : report.blocking_errors) {
            oss << "- " << formatIssueDescription(issue) << "\n";
        }
        oss << "\n";
    }
    
    if (!report.quality_warnings.empty()) {
        oss << "QUALITY WARNINGS (SHOULD FIX):\n";
        for (const auto& issue : report.quality_warnings) {
            oss << "- " << formatIssueDescription(issue) << "\n";
        }
        oss << "\n";
    }
    
    if (!report.best_practice_suggestions.empty()) {
        oss << "BEST PRACTICE SUGGESTIONS (RECOMMENDED):\n";
        for (const auto& issue : report.best_practice_suggestions) {
            oss << "- " << formatIssueDescription(issue) << "\n";
        }
        oss << "\n";
    }
    
    oss << "=== END COMPLIANCE REPORT ===\n";
    
    return oss.str();
}

// Helper function implementations

std::string ComplianceValidator::extractFieldValue(const std::string& content, const std::string& field_name) {
    std::regex field_regex("@" + field_name + ":\\s*(.+?)(?=\\n|$)");
    std::smatch match;
    if (std::regex_search(content, match, field_regex)) {
        return match[1].str();
    }
    return "";
}

std::vector<std::string> ComplianceValidator::extractReferences(const std::string& content) {
    std::vector<std::string> references;
    std::regex ref_regex("@references:\\s*\\[([^\\]]+)\\]");
    std::smatch match;
    if (std::regex_search(content, match, ref_regex)) {
        std::string refs_str = match[1].str();
        std::regex item_regex("\"([^\"]+)\"");
        std::sregex_iterator iter(refs_str.begin(), refs_str.end(), item_regex);
        std::sregex_iterator end;
        for (; iter != end; ++iter) {
            references.push_back((*iter)[1].str());
        }
    }
    return references;
}

std::string ComplianceValidator::extractNamespaceId(const std::string& content) {
    std::regex id_regex("@id:\\s*(.+?)(?=\\n|$)");
    std::smatch match;
    if (std::regex_search(content, match, id_regex)) {
        return match[1].str();
    }
    return "";
}

AkaoNamespace::ComponentType ComplianceValidator::detectComponentType(const std::string& file_path) {
    std::filesystem::path path(file_path);
    std::string extension = path.extension().string();
    
    if (extension == ".cpp" || extension == ".hpp" || extension == ".h" || extension == ".c") {
        return AkaoNamespace::ComponentType::FILE;
    } else if (extension == ".yaml" || extension == ".yml") {
        return AkaoNamespace::ComponentType::ARTIFACT;
    } else if (extension == ".md") {
        return AkaoNamespace::ComponentType::FILE;
    }
    
    return AkaoNamespace::ComponentType::FILE;
}

bool ComplianceValidator::isCodeFile(const std::string& file_path) {
    std::filesystem::path path(file_path);
    std::string extension = path.extension().string();
    return (extension == ".cpp" || extension == ".hpp" || extension == ".h" || extension == ".c");
}

bool ComplianceValidator::isDocumentationFile(const std::string& file_path) {
    std::filesystem::path path(file_path);
    std::string extension = path.extension().string();
    return (extension == ".md" || extension == ".txt" || extension == ".rst");
}

bool ComplianceValidator::isConfigFile(const std::string& file_path) {
    std::filesystem::path path(file_path);
    std::string extension = path.extension().string();
    return (extension == ".yaml" || extension == ".yml" || extension == ".json");
}

bool ComplianceValidator::hasRequiredHeaderFormat(const std::string& file_content) {
    return file_content.find("@id:") != std::string::npos &&
           file_content.find("@doc:") != std::string::npos;
}

bool ComplianceValidator::hasValidNamespaceId(const std::string& file_content) {
    std::regex id_regex("@id:\\s*akao:[a-z]+:[a-z]+:[a-z-]+:[a-z-]+:v\\d+");
    return std::regex_search(file_content, id_regex);
}

std::vector<std::string> ComplianceValidator::checkNamingConventions(const std::string& file_path) {
    std::vector<std::string> issues;
    std::filesystem::path path(file_path);
    std::string filename = path.filename().string();
    
    // Check for underscores in filename
    if (filename.find('_') != std::string::npos) {
        issues.push_back("File name contains underscores: " + filename);
    }
    
    // Check for version pattern
    if (filename.find("v1.") == std::string::npos && filename != "v1") {
        issues.push_back("File name does not follow version pattern: " + filename);
    }
    
    return issues;
}

std::vector<std::string> ComplianceValidator::checkMetadataCompleteness(
    const MetadataSchema::MetadataFields& metadata,
    AkaoNamespace::ComponentType component_type) {
    
    std::vector<std::string> issues;
    
    if (metadata.specification.empty()) {
        issues.push_back("Missing @specification field");
    }
    if (metadata.scope.empty()) {
        issues.push_back("Missing @scope field");
    }
    if (metadata.timeline.empty()) {
        issues.push_back("Missing @timeline field");
    }
    if (metadata.rationale.empty()) {
        issues.push_back("Missing @rationale field");
    }
    if (metadata.methodology.empty()) {
        issues.push_back("Missing @methodology field");
    }
    if (metadata.references.empty()) {
        issues.push_back("Missing @references field");
    }
    
    // Check @doc field for code files
    if (component_type == AkaoNamespace::ComponentType::FILE && 
        (!metadata.doc.has_value() || metadata.doc.value().empty())) {
        issues.push_back("Missing @doc field for code component");
    }
    
    return issues;
}

std::vector<std::string> ComplianceValidator::getAllFiles(const std::string& directory_path) {
    std::vector<std::string> files;
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory_path)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().string());
            }
        }
    } catch (const std::exception& e) {
        // Handle filesystem errors silently
    }
    return files;
}

std::vector<std::string> ComplianceValidator::getAllDirectories(const std::string& directory_path) {
    std::vector<std::string> directories;
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory_path)) {
            if (entry.is_directory()) {
                directories.push_back(entry.path().string());
            }
        }
    } catch (const std::exception& e) {
        // Handle filesystem errors silently
    }
    return directories;
}

bool ComplianceValidator::hasDirectoryMetadata(const std::string& directory_path) {
    std::filesystem::path metadata_file = std::filesystem::path(directory_path) / "_.yaml";
    return std::filesystem::exists(metadata_file);
}

void ComplianceValidator::categorizeIssues(
    const std::vector<ValidationIssue>& all_issues,
    ComplianceReport& report) {
    
    for (const auto& issue : all_issues) {
        switch (issue.level) {
            case ValidationLevel::BLOCKING_ERRORS:
                report.blocking_errors.push_back(issue);
                break;
            case ValidationLevel::QUALITY_WARNINGS:
                report.quality_warnings.push_back(issue);
                break;
            case ValidationLevel::BEST_PRACTICES:
                report.best_practice_suggestions.push_back(issue);
                break;
        }
    }
    
}

double ComplianceValidator::calculateCompliancePercentage(const ComplianceReport& report) {
    if (report.total_components_scanned == 0) {
        return 0.0;
    }
    return (static_cast<double>(report.compliant_components) / report.total_components_scanned) * 100.0;
}

std::string ComplianceValidator::formatIssueDescription(const ValidationIssue& issue) {
    return "[" + issue.component_id + "] " + issue.description + " (in " + issue.file_path + ")";
}

std::vector<ComplianceValidator::ValidationIssue> ComplianceValidator::validateDirectoryStructure(const std::string& directory_path) {
    std::vector<ValidationIssue> issues;
    
    // Check if directory has metadata file
    if (!hasDirectoryMetadata(directory_path)) {
        issues.push_back({
            ValidationLevel::QUALITY_WARNINGS,
            "directory",
            "missing_metadata",
            "Directory missing _.yaml metadata file",
            directory_path,
            0,
            "Create _.yaml file with directory metadata"
        });
    }
    
    return issues;
}

} // namespace engine
} // namespace core
} // namespace akao
