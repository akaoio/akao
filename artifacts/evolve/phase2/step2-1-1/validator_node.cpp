/**
 * @id akao.evolve.validator_node.v1
 * @doc Implementation of ValidatorNode converting UnifiedValidator functionality to node-based architecture with complete parameter handling, validation mode support, and result transformation.
 * @specification Complete ValidatorNode implementation providing all UnifiedValidator capabilities through INode interface with parameter validation, execution modes, and comprehensive error handling.
 * @scope artifacts/evolve/phase2/step2-1-1
 * @timeline 2025-07-10
 * @rationale Enable existing validation functionality in node workflows while maintaining full compatibility and functionality of the original validator system.
 * @methodology Direct integration with UnifiedValidator using adapter pattern, parameter mapping, and result conversion with proper error handling and metadata support.
 * @references akao.evolve.validator_node.v1, akao.core.engine.validator.v1
 */

#include "validator_node.hpp"
#include <sstream>
#include <algorithm>
#include <chrono>

namespace akao {
namespace evolve {
namespace component {

// Utility function implementations
std::string validationModeToString(ValidationMode mode) {
    switch (mode) {
        case ValidationMode::UNIVERSAL: return "universal";
        case ValidationMode::COMPLIANCE: return "compliance";
        case ValidationMode::METADATA: return "metadata";
        case ValidationMode::NAMESPACE: return "namespace";
        case ValidationMode::PHILOSOPHY: return "philosophy";
        case ValidationMode::PHASED: return "phased";
        case ValidationMode::RULE_DRIVEN: return "rule_driven";
        case ValidationMode::SINGLE_FILE: return "single_file";
        case ValidationMode::DIRECTORY: return "directory";
    }
    return "universal";
}

std::optional<ValidationMode> stringToValidationMode(const std::string& mode_str) {
    if (mode_str == "universal") return ValidationMode::UNIVERSAL;
    if (mode_str == "compliance") return ValidationMode::COMPLIANCE;
    if (mode_str == "metadata") return ValidationMode::METADATA;
    if (mode_str == "namespace") return ValidationMode::NAMESPACE;
    if (mode_str == "philosophy") return ValidationMode::PHILOSOPHY;
    if (mode_str == "phased") return ValidationMode::PHASED;
    if (mode_str == "rule_driven") return ValidationMode::RULE_DRIVEN;
    if (mode_str == "single_file") return ValidationMode::SINGLE_FILE;
    if (mode_str == "directory") return ValidationMode::DIRECTORY;
    return std::nullopt;
}

// ValidatorNode implementation
ValidatorNode::ValidatorNode() {
    // Initialize with default configuration
    cached_config_ = akao::core::engine::validator::ValidationConfig();
    validator_ = std::make_unique<akao::core::engine::validator::UnifiedValidator>(cached_config_);
}

std::string ValidatorNode::getNodeId() const {
    return "akao.validator.v1";
}

std::string ValidatorNode::getNodeType() const {
    return "akao.validator.v1";
}

NodeDefinition ValidatorNode::getDefinition() const {
    auto def = NodeDefinition::createValidationNode(getNodeId(), "Validator Node");
    def.setDescription("Performs comprehensive validation using the unified validation system including rule execution, compliance checking, and metadata validation");
    
    auto params = std::make_unique<ParameterSet>();
    
    // Target path parameter
    params->addParameter(NodeParameter::createString("target_path", "Target Path", false, ".")
        .setDescription("Path to validate (file or directory). If not specified, uses input items as paths"));
    
    // Validation mode parameter
    params->addParameter(NodeParameter::createOptions("validation_mode", "Validation Mode",
        {
            ParameterOption("universal", "Universal", "Complete validation using all available rules"),
            ParameterOption("compliance", "Compliance", "Compliance checking only"),
            ParameterOption("metadata", "Metadata", "Metadata validation only"),
            ParameterOption("namespace", "Namespace", "Namespace validation only"),
            ParameterOption("philosophy", "Philosophy", "Philosophy validation only"),
            ParameterOption("phased", "Phased", "Phase-aware validation pipeline"),
            ParameterOption("rule_driven", "Rule Driven", "Validation using specific rules"),
            ParameterOption("single_file", "Single File", "Single file validation"),
            ParameterOption("directory", "Directory", "Directory validation")
        }, false, "universal"));
    
    // Rule configuration parameters
    params->addParameter(NodeParameter::createString("rules_directory", "Rules Directory", false, ".akao/rules")
        .setDescription("Directory containing validation rules"));
    
    params->addParameter(NodeParameter::createString("rule_ids", "Rule IDs", false, "")
        .setDescription("Comma-separated list of specific rule IDs to execute (for rule_driven mode)"));
    
    params->addParameter(NodeParameter::createString("file_patterns", "File Patterns", false, "*.cpp,*.hpp,*.yaml")
        .setDescription("Comma-separated list of file patterns to include in validation"));
    
    // Validation configuration parameters
    params->addParameter(NodeParameter::createBoolean("enable_universal_validation", "Enable Universal Validation", false, true));
    params->addParameter(NodeParameter::createBoolean("enable_compliance_checking", "Enable Compliance Checking", false, true));
    params->addParameter(NodeParameter::createBoolean("enable_metadata_validation", "Enable Metadata Validation", false, true));
    params->addParameter(NodeParameter::createBoolean("enable_namespace_validation", "Enable Namespace Validation", false, true));
    
    // Performance and behavior parameters
    params->addParameter(NodeParameter::createBoolean("parallel_execution", "Parallel Execution", false, false));
    params->addParameter(NodeParameter::createBoolean("fail_fast", "Fail Fast", false, false));
    params->addParameter(NodeParameter::createBoolean("enable_auto_fix", "Enable Auto Fix", false, false));
    
    // Output format parameters
    params->addParameter(NodeParameter::createOptions("output_format", "Output Format",
        {
            ParameterOption("violations", "Violations", "Output individual violations as separate items"),
            ParameterOption("summary", "Summary", "Output validation summary only"),
            ParameterOption("detailed", "Detailed", "Output detailed results with statistics"),
            ParameterOption("report", "Report", "Output formatted validation report")
        }, false, "violations"));
    
    params->addParameter(NodeParameter::createBoolean("include_metadata", "Include Metadata", false, true)
        .setDescription("Include validation metadata in results"));
    
    def.setParameters(std::move(params));
    
    def.setDocumentation(
        "ValidatorNode provides comprehensive validation capabilities by wrapping the UnifiedValidator. "
        "It supports multiple validation modes, rule-driven validation, and various output formats. "
        "The node can validate files or directories and provides detailed violation reporting.",
        
        "Example usage:\n"
        "- Universal validation: Set validation_mode='universal', target_path='./src'\n"
        "- Rule-driven validation: Set validation_mode='rule_driven', rule_ids=['rule1', 'rule2']\n"
        "- Single file validation: Set validation_mode='single_file', target_path='file.cpp'"
    );
    
    return def;
}

ExecutionResult ValidatorNode::execute(const NodeContext& context) {
    ExecutionResult result;
    result.start_time = std::chrono::steady_clock::now();
    
    try {
        // Get validation mode
        std::string mode_str = context.hasParameter("validation_mode") ? 
            context.getParameter("validation_mode").toString() : "universal";
        
        auto mode_opt = stringToValidationMode(mode_str);
        if (!mode_opt.has_value()) {
            result.addError("Invalid validation mode: " + mode_str);
            result.markComplete();
            return result;
        }
        
        ValidationMode mode = mode_opt.value();
        
        // Execute validation based on mode
        result = executeValidation(context, mode);
        
    } catch (const std::exception& e) {
        result.addError("ValidatorNode execution failed: " + std::string(e.what()));
    }
    
    result.markComplete();
    return result;
}

akao::evolve::ValidationResult ValidatorNode::validate(const std::map<std::string, ParameterValue>& parameters) {
    // Validate validation_mode parameter
    auto mode_it = parameters.find("validation_mode");
    if (mode_it != parameters.end()) {
        std::string mode = mode_it->second.toString();
        auto mode_opt = stringToValidationMode(mode);
        if (!mode_opt.has_value()) {
            return akao::evolve::ValidationResult::failure("Invalid validation mode: " + mode);
        }
    }
    
    // Validate target_path if provided
    auto path_it = parameters.find("target_path");
    if (path_it != parameters.end()) {
        std::string path = path_it->second.toString();
        if (!path.empty() && !std::filesystem::exists(path)) {
            return akao::evolve::ValidationResult::failure("Target path does not exist: " + path);
        }
    }
    
    // Validate rules_directory if provided
    auto rules_it = parameters.find("rules_directory");
    if (rules_it != parameters.end()) {
        std::string rules_dir = rules_it->second.toString();
        if (!rules_dir.empty() && !std::filesystem::exists(rules_dir)) {
            return akao::evolve::ValidationResult::failure("Rules directory does not exist: " + rules_dir);
        }
    }
    
    return akao::evolve::ValidationResult::success();
}

ExecutionResult ValidatorNode::executeValidation(const NodeContext& context, ValidationMode mode) {
    ExecutionResult result;
    
    // Resolve target path
    std::string target_path = resolveTargetPath(context);
    if (!validateTargetPath(target_path, result)) {
        return result;
    }
    
    // Create validator configuration
    auto config = createConfigFromParameters(context);
    initializeValidator(config);
    
    if (!isValidatorReady()) {
        result.addError("Validator initialization failed");
        return result;
    }
    
    try {
        akao::core::engine::validator::ValidationResult validator_result;
        
        // Execute validation based on mode
        switch (mode) {
            case ValidationMode::UNIVERSAL:
                validator_result = validator_->validateUniversal(target_path);
                break;
            case ValidationMode::COMPLIANCE:
                validator_result = validator_->validateCompliance(target_path);
                break;
            case ValidationMode::METADATA:
                validator_result = validator_->validateMetadata(target_path);
                break;
            case ValidationMode::NAMESPACE:
                validator_result = validator_->validateNamespace(target_path);
                break;
            case ValidationMode::PHILOSOPHY:
                validator_result = validator_->validatePhilosophy(target_path);
                break;
            case ValidationMode::PHASED:
                validator_result = validator_->validatePhased(target_path);
                break;
            case ValidationMode::RULE_DRIVEN: {
                auto rule_ids = getRuleIdsFromParameters(context);
                validator_result = validator_->validateWithRules(target_path, rule_ids);
                break;
            }
            case ValidationMode::SINGLE_FILE:
                validator_result = validator_->validateFile(target_path);
                break;
            case ValidationMode::DIRECTORY:
                validator_result = validator_->validateDirectory(target_path);
                break;
        }
        
        // Convert validator result to execution result
        result = convertValidationResult(validator_result);
        
    } catch (const std::exception& e) {
        result.addError("Validation execution failed: " + std::string(e.what()));
    }
    
    return result;
}

std::vector<NodeItem> ValidatorNode::convertViolationsToItems(
    const std::vector<akao::core::engine::validator::Violation>& violations) const {
    
    std::vector<NodeItem> items;
    items.reserve(violations.size());
    
    for (const auto& violation : violations) {
        items.push_back(convertViolationToItem(violation));
    }
    
    return items;
}

ExecutionResult ValidatorNode::convertValidationResult(
    const akao::core::engine::validator::ValidationResult& validator_result) const {
    
    ExecutionResult result;
    
    // Determine output format
    std::string output_format = "violations"; // Default, could be from parameters
    
    if (output_format == "violations") {
        // Output individual violations as separate items
        auto violation_items = convertViolationsToItems(validator_result.violations);
        for (auto& item : violation_items) {
            result.addItem(std::move(item));
        }
    } else if (output_format == "summary") {
        // Output validation summary only
        NodeItem summary_item;
        summary_item.json = ParameterValue(validator_result.summary);
        summary_item.addMetadata("is_valid", ParameterValue(validator_result.is_valid));
        summary_item.addMetadata("violation_count", ParameterValue(static_cast<int64_t>(validator_result.violations.size())));
        summary_item.addMetadata("target_path", ParameterValue(validator_result.target_path));
        result.addItem(summary_item);
    } else if (output_format == "detailed") {
        // Output detailed results with statistics
        NodeItem detailed_item;
        
        // Create detailed summary
        std::ostringstream detailed_summary;
        detailed_summary << "Validation Result for " << validator_result.target_path << "\n";
        detailed_summary << "Status: " << (validator_result.is_valid ? "VALID" : "INVALID") << "\n";
        detailed_summary << "Violations: " << validator_result.violations.size() << "\n";
        detailed_summary << "Rules Executed: " << validator_result.total_rules_executed << "\n";
        detailed_summary << "Files Analyzed: " << validator_result.total_files_analyzed << "\n";
        detailed_summary << "Execution Time: " << validator_result.execution_duration.count() << "s\n";
        
        detailed_item.json = ParameterValue(detailed_summary.str());
        
        // Add comprehensive metadata
        auto stats = getValidationStatistics(validator_result);
        for (const auto& [key, value] : stats) {
            detailed_item.addMetadata(key, value);
        }
        
        result.addItem(detailed_item);
    }
    
    // Set execution status based on validation result
    if (!validator_result.is_valid && !validator_result.violations.empty()) {
        result.setStatus(ExecutionStatus::PARTIAL);
        for (const auto& violation : validator_result.violations) {
            result.addError(violation.message, violation.rule_id);
        }
    }
    
    return result;
}

akao::core::engine::validator::ValidationConfig 
ValidatorNode::createConfigFromParameters(const NodeContext& context) const {
    
    akao::core::engine::validator::ValidationConfig config;
    
    // Set configuration from parameters
    config.enable_universal_validation = context.hasParameter("enable_universal_validation") ?
        context.getParameter("enable_universal_validation").as<bool>() : true;
    
    config.enable_compliance_checking = context.hasParameter("enable_compliance_checking") ?
        context.getParameter("enable_compliance_checking").as<bool>() : true;
    
    config.enable_metadata_validation = context.hasParameter("enable_metadata_validation") ?
        context.getParameter("enable_metadata_validation").as<bool>() : true;
    
    config.enable_namespace_validation = context.hasParameter("enable_namespace_validation") ?
        context.getParameter("enable_namespace_validation").as<bool>() : true;
    
    config.rules_directory = context.hasParameter("rules_directory") ?
        context.getParameter("rules_directory").toString() : ".akao/rules";
    
    config.parallel_execution = context.hasParameter("parallel_execution") ?
        context.getParameter("parallel_execution").as<bool>() : false;
    
    config.enable_auto_fix = context.hasParameter("enable_auto_fix") ?
        context.getParameter("enable_auto_fix").as<bool>() : false;
    
    // File patterns
    if (context.hasParameter("file_patterns")) {
        std::string patterns_str = context.getParameter("file_patterns").toString();
        if (!patterns_str.empty()) {
            // Parse comma-separated file patterns
            config.file_patterns.clear();
            std::stringstream ss(patterns_str);
            std::string pattern;
            
            while (std::getline(ss, pattern, ',')) {
                // Trim whitespace
                pattern.erase(0, pattern.find_first_not_of(" \t"));
                pattern.erase(pattern.find_last_not_of(" \t") + 1);
                if (!pattern.empty()) {
                    config.file_patterns.push_back(pattern);
                }
            }
        }
    }
    
    return config;
}

std::string ValidatorNode::resolveTargetPath(const NodeContext& context) const {
    // Check if target_path parameter is provided
    if (context.hasParameter("target_path")) {
        std::string path = context.getParameter("target_path").toString();
        if (!path.empty()) {
            return path;
        }
    }
    
    // If no parameter, use first input item as target path
    if (context.hasInput()) {
        const auto& items = context.getInputItems();
        if (!items.empty()) {
            return items[0].json.toString();
        }
    }
    
    // Default to current directory
    return ".";
}

std::vector<std::string> ValidatorNode::getRuleIdsFromParameters(const NodeContext& context) const {
    if (!context.hasParameter("rule_ids")) {
        return {};
    }
    
    std::string rule_ids_str = context.getParameter("rule_ids").toString();
    if (rule_ids_str.empty()) {
        return {};
    }
    
    // Parse comma-separated rule IDs
    std::vector<std::string> rule_ids;
    std::stringstream ss(rule_ids_str);
    std::string rule_id;
    
    while (std::getline(ss, rule_id, ',')) {
        // Trim whitespace
        rule_id.erase(0, rule_id.find_first_not_of(" \t"));
        rule_id.erase(rule_id.find_last_not_of(" \t") + 1);
        if (!rule_id.empty()) {
            rule_ids.push_back(rule_id);
        }
    }
    
    return rule_ids;
}

bool ValidatorNode::validateTargetPath(const std::string& target_path, ExecutionResult& result) const {
    if (target_path.empty()) {
        result.addError("Target path cannot be empty");
        return false;
    }
    
    if (!std::filesystem::exists(target_path)) {
        result.addError("Target path does not exist: " + target_path);
        return false;
    }
    
    return true;
}

ParameterValue ValidatorNode::createViolationSummary(
    const std::vector<akao::core::engine::validator::Violation>& violations) const {
    
    std::ostringstream summary;
    summary << "Found " << violations.size() << " violations:\n";
    
    for (const auto& violation : violations) {
        summary << "- " << violation.rule_id << ": " << violation.message;
        if (!violation.file_path.empty()) {
            summary << " (in " << violation.file_path << ")";
        }
        summary << "\n";
    }
    
    return ParameterValue(summary.str());
}

NodeItem ValidatorNode::convertViolationToItem(
    const akao::core::engine::validator::Violation& violation) const {
    
    NodeItem item;
    
    // Main violation message as primary data
    item.json = ParameterValue(violation.message);
    
    // Add all violation details as metadata
    item.addMetadata("id", ParameterValue(violation.id));
    item.addMetadata("rule_id", ParameterValue(violation.rule_id));
    item.addMetadata("rule_name", ParameterValue(violation.rule_name));
    item.addMetadata("description", ParameterValue(violation.description));
    item.addMetadata("file_path", ParameterValue(violation.file_path));
    item.addMetadata("line_number", ParameterValue(static_cast<int64_t>(violation.line_number)));
    item.addMetadata("column_number", ParameterValue(static_cast<int64_t>(violation.column_number)));
    item.addMetadata("severity", ParameterValue(violation.severity));
    item.addMetadata("rule_category", ParameterValue(violation.rule_category));
    item.addMetadata("suggestion", ParameterValue(violation.suggestion));
    item.addMetadata("auto_fix_available", ParameterValue(violation.auto_fix_available));
    
    if (!violation.philosophy_id.empty()) {
        item.addMetadata("philosophy_id", ParameterValue(violation.philosophy_id));
    }
    
    if (!violation.trace_id.empty()) {
        item.addMetadata("trace_id", ParameterValue(violation.trace_id));
    }
    
    return item;
}

std::map<std::string, ParameterValue> ValidatorNode::getValidationStatistics(
    const akao::core::engine::validator::ValidationResult& validator_result) const {
    
    std::map<std::string, ParameterValue> stats;
    
    stats["is_valid"] = ParameterValue(validator_result.is_valid);
    stats["violation_count"] = ParameterValue(static_cast<int64_t>(validator_result.violations.size()));
    stats["target_path"] = ParameterValue(validator_result.target_path);
    stats["validation_type"] = ParameterValue(validator_result.validation_type);
    stats["total_rules_executed"] = ParameterValue(static_cast<int64_t>(validator_result.total_rules_executed));
    stats["total_files_analyzed"] = ParameterValue(static_cast<int64_t>(validator_result.total_files_analyzed));
    stats["execution_duration_seconds"] = ParameterValue(validator_result.execution_duration.count());
    stats["summary"] = ParameterValue(validator_result.summary);
    
    return stats;
}

void ValidatorNode::initializeValidator(const akao::core::engine::validator::ValidationConfig& config) {
    cached_config_ = config;
    validator_ = std::make_unique<akao::core::engine::validator::UnifiedValidator>(config);
}

bool ValidatorNode::isValidatorReady() const {
    return validator_ != nullptr && validator_->isInitialized();
}

const akao::core::engine::validator::UnifiedValidator* ValidatorNode::getValidator() const {
    return validator_.get();
}

// Registration and factory functions
void registerValidatorNode(NodeRegistry& registry) {
    auto definition = ValidatorNode().getDefinition();
    registry.registerNode<ValidatorNode>(definition);
}

std::unique_ptr<INode> createValidatorNode() {
    return std::make_unique<ValidatorNode>();
}

} // namespace component
} // namespace evolve
} // namespace akao