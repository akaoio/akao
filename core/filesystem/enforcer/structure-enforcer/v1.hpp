#pragma once

#include "../../scanner/directory-scanner/v1.hpp"
#include "../../../engine/validator/validation_result/v1.hpp"
#include <string>
#include <vector>
#include <map>
#include <functional>

namespace akao::core::filesystem::enforcer {

/**
 * @brief Structure enforcement rule
 */
struct EnforcementRule {
    std::string id;
    std::string name;
    std::string description;
    std::string category;  // "mandatory", "recommended", "optional"
    
    // Path requirements
    std::vector<std::string> required_directories;
    std::vector<std::string> required_files;
    std::vector<std::string> forbidden_paths;
    
    // Structure patterns
    std::map<std::string, std::vector<std::string>> directory_structure;
    std::vector<std::string> naming_patterns;
    
    // Validation criteria
    std::function<bool(const scanner::DirectoryInfo&)> validator;
    
    // Auto-fix configuration
    bool auto_fix_enabled = false;
    std::function<bool(const std::string&)> auto_fix_handler;
    
    // Metadata
    std::vector<std::string> applicable_project_types;
    std::string severity = "error";  // "error", "warning", "info"
};

/**
 * @brief Structure enforcement action
 */
struct EnforcementAction {
    enum Type {
        CREATE_DIRECTORY,
        CREATE_FILE,
        MOVE_FILE,
        RENAME_FILE,
        DELETE_FILE,
        MODIFY_CONTENT,
        VALIDATE_ONLY
    };
    
    Type action_type;
    std::string source_path;
    std::string target_path;
    std::string content;
    std::string reason;
    bool is_reversible = true;
};

/**
 * @brief Enforcement result
 */
struct EnforcementResult {
    bool success = false;
    std::string error_message;
    
    // Violations found
    std::vector<engine::validator::Violation> violations;
    
    // Actions taken or recommended
    std::vector<EnforcementAction> actions_taken;
    std::vector<EnforcementAction> recommended_actions;
    
    // Statistics
    size_t rules_checked = 0;
    size_t rules_passed = 0;
    size_t rules_failed = 0;
    double enforcement_duration_seconds = 0.0;
};

/**
 * @brief Universal structure enforcer for Akao framework
 * 
 * Implements comprehensive structure validation and enforcement that works with
 * any project type. Can detect violations and automatically fix them according
 * to defined rules and patterns.
 * 
 * Philosophy compliance:
 * - akao:philosophy:structure:enforcement:v1 - Automated structure enforcement
 * - akao:philosophy:rule:governance:v1 - Rules govern structure
 * - akao:philosophy:automation:updates:v1 - Automated updates and fixes
 */
class StructureEnforcer {
public:
    /**
     * @brief Enforcement mode
     */
    enum class Mode {
        VALIDATE_ONLY,     // Only check for violations, don't fix
        AUTO_FIX,          // Automatically fix violations when possible
        INTERACTIVE,       // Ask for confirmation before fixes
        REPORT_ONLY        // Generate report without validation
    };

    /**
     * @brief Enforcement configuration
     */
    struct Config {
        Mode mode = Mode::VALIDATE_ONLY;
        bool strict_mode = false;
        bool create_missing_directories = true;
        bool create_missing_files = false;
        bool remove_forbidden_paths = false;
        std::vector<std::string> excluded_paths;
        std::vector<std::string> excluded_rules;
        std::string backup_directory = ".akao_backup";
        bool create_backups = true;
    };

    /**
     * @brief Constructor
     */
    StructureEnforcer();
    
    /**
     * @brief Constructor with configuration
     */
    explicit StructureEnforcer(const Config& config);

    /**
     * @brief Set configuration
     */
    void setConfig(const Config& config);

    /**
     * @brief Get current configuration
     */
    const Config& getConfig() const;

    /**
     * @brief Add enforcement rule
     */
    void addRule(const EnforcementRule& rule);

    /**
     * @brief Load rules from directory
     */
    bool loadRules(const std::string& rules_directory);

    /**
     * @brief Load default Akao structure rules
     */
    void loadDefaultAkaoRules();

    /**
     * @brief Enforce structure on directory
     * @param directory_path Path to directory to enforce
     * @return Enforcement result
     */
    EnforcementResult enforceStructure(const std::string& directory_path);

    /**
     * @brief Validate structure without enforcement
     * @param directory_path Path to directory to validate
     * @return Validation result
     */
    EnforcementResult validateStructure(const std::string& directory_path);

    /**
     * @brief Auto-fix violations in directory
     * @param directory_path Path to directory to fix
     * @param violations Violations to fix
     * @return Fix result
     */
    EnforcementResult autoFix(const std::string& directory_path, 
                             const std::vector<engine::validator::Violation>& violations);

    /**
     * @brief Preview actions that would be taken
     * @param directory_path Path to directory to analyze
     * @return Preview of actions
     */
    std::vector<EnforcementAction> previewActions(const std::string& directory_path);

    /**
     * @brief Create project structure from template
     * @param directory_path Path where to create structure
     * @param project_type Type of project ("akao", "cpp", "python", etc.)
     * @return Creation result
     */
    EnforcementResult createProjectStructure(const std::string& directory_path, 
                                            const std::string& project_type);

    /**
     * @brief Migrate project to new structure
     * @param directory_path Path to project to migrate
     * @param target_structure Target structure definition
     * @return Migration result
     */
    EnforcementResult migrateStructure(const std::string& directory_path,
                                     const std::map<std::string, std::vector<std::string>>& target_structure);

    /**
     * @brief Get available project templates
     */
    std::vector<std::string> getAvailableTemplates() const;

    /**
     * @brief Register custom project template
     */
    void registerTemplate(const std::string& template_name,
                         const std::map<std::string, std::vector<std::string>>& structure);

    /**
     * @brief Get enforcement statistics
     */
    struct EnforcementStats {
        size_t total_enforcements = 0;
        size_t successful_enforcements = 0;
        size_t failed_enforcements = 0;
        size_t total_violations_found = 0;
        size_t total_violations_fixed = 0;
        double total_enforcement_time = 0.0;
        std::map<std::string, size_t> rule_violation_counts;
    };

    const EnforcementStats& getStats() const;

    /**
     * @brief Clear statistics
     */
    void clearStats();

    /**
     * @brief Initialize enforcer
     */
    bool initialize();
    
private:
    Config config_;
    std::vector<EnforcementRule> rules_;
    std::map<std::string, std::map<std::string, std::vector<std::string>>> project_templates_;
    EnforcementStats stats_;
    std::unique_ptr<scanner::DirectoryScanner> scanner_;

    // Core enforcement methods
    EnforcementResult checkRules(const scanner::DirectoryInfo& directory_info, 
                               const std::string& base_path);
    
    bool checkRule(const EnforcementRule& rule, 
                  const scanner::DirectoryInfo& directory_info,
                  std::vector<engine::validator::Violation>& violations);
    
    std::vector<EnforcementAction> generateActions(const std::vector<engine::validator::Violation>& violations);
    
    bool executeAction(const EnforcementAction& action);
    
    // Rule checking methods
    bool checkRequiredDirectories(const EnforcementRule& rule,
                                const scanner::DirectoryInfo& directory_info,
                                std::vector<engine::validator::Violation>& violations);
    
    bool checkRequiredFiles(const EnforcementRule& rule,
                          const scanner::DirectoryInfo& directory_info,
                          std::vector<engine::validator::Violation>& violations);
    
    bool checkForbiddenPaths(const EnforcementRule& rule,
                           const scanner::DirectoryInfo& directory_info,
                           std::vector<engine::validator::Violation>& violations);
    
    bool checkNamingPatterns(const EnforcementRule& rule,
                           const scanner::DirectoryInfo& directory_info,
                           std::vector<engine::validator::Violation>& violations);
    
    // Action execution methods
    bool createDirectory(const std::string& path);
    bool createFile(const std::string& path, const std::string& content = "");
    bool moveFile(const std::string& source, const std::string& target);
    bool renameFile(const std::string& source, const std::string& target);
    bool deleteFile(const std::string& path);
    bool modifyFileContent(const std::string& path, const std::string& content);
    
    // Backup and recovery
    bool createBackup(const std::string& path);
    bool restoreBackup(const std::string& path);
    std::string getBackupPath(const std::string& original_path);
    
    // Template management
    void initializeDefaultTemplates();
    std::map<std::string, std::vector<std::string>> getTemplateStructure(const std::string& template_name);
    std::string generateDefaultFileContent(const std::string& filename, const std::string& project_type);
    
    // Utility methods
    bool isPathExcluded(const std::string& path);
    bool isRuleExcluded(const std::string& rule_id);
    bool pathExists(const std::string& path);
    std::string normalizePath(const std::string& path);
    
    engine::validator::Violation createViolation(const EnforcementRule& rule,
                                                const std::string& path,
                                                const std::string& message,
                                                const std::string& suggestion = "");
    
    void updateStats(const EnforcementResult& result);
};

/**
 * @brief Default Akao project structure rules
 */
namespace akao_rules {

/**
 * @brief Get mandatory Akao directory structure rules
 */
std::vector<EnforcementRule> getMandatoryRules();

/**
 * @brief Get recommended Akao structure rules
 */
std::vector<EnforcementRule> getRecommendedRules();

/**
 * @brief Get optional Akao structure rules
 */
std::vector<EnforcementRule> getOptionalRules();

/**
 * @brief Get complete Akao project template
 */
std::map<std::string, std::vector<std::string>> getAkaoTemplate();

} // namespace akao_rules

} // namespace akao::core::filesystem::enforcer
