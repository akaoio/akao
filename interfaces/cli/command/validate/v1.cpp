#include "v1.hpp"
#include "../../../../core/engine/validator/universal/v1.hpp"
#include "../../../../core/filesystem/scanner/directory_scanner.hpp"
#include "../../../../core/engine/parser/yaml/parser/v1.hpp"
#include "../../../../core/trace/tracer/violation_tracer.hpp"
#include <filesystem>
#include <iostream>
#include <fstream>

namespace akao::interfaces::cli::commands {

ValidateCommand::ValidateCommand() = default;

executor::ExecutionResult ValidateCommand::execute(const executor::ExecutionContext& context,
                                                  const std::vector<std::string>& args) {
    executor::ExecutionResult result;
    result.success = true;
    result.exit_code = 0;

    try {
        auto options = parseArguments(args);

        // Handle self-validation
        if (options.self_validate) {
            return selfValidate(options, context);
        }

        // Handle violation tracing
        if (!args.empty() && args[0].find("akao:rule::") == 0) {
            return traceViolation(args[0], context);
        }

        // Handle rule category checking
        if (!options.rule_categories.empty()) {
            return checkRuleCategories(options, context);
        }

        // Handle auto-fixing
        if (options.enable_fixing) {
            return autoFixViolations(options, context);
        }

        // Default: comprehensive project validation
        return validateProject(options, context);

    } catch (const std::exception& e) {
        result.success = false;
        result.exit_code = 1;
        result.error_message = "Validation failed: " + std::string(e.what());
        return result;
    }
}

ValidateCommand::ValidationOptions ValidateCommand::parseArguments(const std::vector<std::string>& args) {
    ValidationOptions options;

    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];

        if (arg == "--rules" && i + 1 < args.size()) {
            std::string rules = args[++i];
            if (rules == "all") {
                options.rule_categories = {"structure", "interface", "language", "security", 
                                         "testing", "documentation", "build", "automation",
                                         "measurement", "validation", "visualization"};
            } else {
                options.rule_categories.push_back(rules);
            }
        }
        else if (arg == "--philosophy" && i + 1 < args.size()) {
            options.philosophies.push_back(args[++i]);
        }
        else if (arg == "--trace") {
            options.enable_tracing = true;
        }
        else if (arg == "--fix") {
            options.enable_fixing = true;
        }
        else if (arg == "--self") {
            options.self_validate = true;
        }
        else if (arg == "--dry-run") {
            options.dry_run = true;
        }
        else if (arg == "--export" && i + 1 < args.size()) {
            options.export_file = args[++i];
        }
        else if (arg == "--metrics") {
            options.include_metrics = true;
        }
        else if (arg.find("--") != 0 && options.target_path == ".") {
            // First non-option argument is the target path
            options.target_path = arg;
        }
    }

    return options;
}

executor::ExecutionResult ValidateCommand::validateProject(const ValidationOptions& options,
                                                          const executor::ExecutionContext& context) {
    executor::ExecutionResult result;
    result.success = true;
    result.exit_code = 0;

    // Initialize universal validator
    core::engine::validator::UniversalValidator validator;
    if (!validator.initialize()) {
        result.success = false;
        result.exit_code = 1;
        result.error_message = "Failed to initialize validator";
        return result;
    }
    
    // Configure validation through public methods
    validator.enableAutoFix(options.enable_fixing);
    validator.setOutputFormat(context.output_format);

    // Perform validation - use the basic validate method for now
    core::engine::validator::ValidationResult validation_result = validator.validate(options.target_path);

    // Format results
    result.data["validation"] = "completed";
    result.data["target"] = options.target_path;
    result.data["timestamp"] = std::to_string(std::time(nullptr));
    result.data["total_rules"] = std::to_string(validation_result.getTotalRulesExecuted());
    result.data["passed"] = std::to_string(validation_result.getRulesPassed());
    result.data["failed"] = std::to_string(validation_result.getRulesFailed());
    result.data["compliance_rate"] = std::to_string(validation_result.getOverallComplianceScore());

    // Add violations
    const auto& violations = validation_result.getViolations();
    std::string violations_json = "[";
    for (size_t i = 0; i < violations.size(); ++i) {
        const auto& violation = violations[i];
        if (i > 0) violations_json += ",";
        violations_json += "{";
        violations_json += "\"id\":\"" + violation.id + "\",";
        violations_json += "\"rule\":\"" + violation.rule_id + "\",";
        violations_json += "\"file\":\"" + violation.file_path + "\",";
        violations_json += "\"line\":" + std::to_string(violation.line_number) + ",";
        violations_json += "\"message\":\"" + violation.message + "\",";
        violations_json += "\"suggestion\":\"" + violation.suggestion + "\",";
        violations_json += "\"severity\":\"" + violation.severity + "\"";
        violations_json += "}";
    }
    violations_json += "]";
    result.data["violations"] = violations_json;

    // Add trace data if enabled
    if (options.enable_tracing) {
        std::string trace_data = "{";
        for (const auto& violation : violations) {
            auto trace = generateTraceData(violation.id);
            trace_data += "\"" + violation.id + "\":{";
            for (const auto& [key, value] : trace) {
                trace_data += "\"" + key + "\":\"" + value + "\",";
            }
            if (!trace.empty()) trace_data.pop_back(); // Remove trailing comma
            trace_data += "},";
        }
        if (!violations.empty()) trace_data.pop_back(); // Remove trailing comma
        trace_data += "}";
        result.data["trace"] = trace_data;
    }

    // Export results if requested
    if (!options.export_file.empty()) {
        if (!exportResults(options.export_file, result.data)) {
            result.data["warning"] = "Failed to export results to " + options.export_file;
        }
    }

    // Set exit code based on violations
    if (validation_result.getRulesFailed() > 0) {
        result.exit_code = 1;
    }

    return result;
}

executor::ExecutionResult ValidateCommand::selfValidate(const ValidationOptions& options,
                                                       const executor::ExecutionContext& context) {
    executor::ExecutionResult result;
    result.success = true;
    result.exit_code = 0;

    // Self-validation: validate Akao against its own rules
    ValidationOptions self_options = options;
    self_options.target_path = "."; // Current directory (Akao itself)
    self_options.rule_categories = {"structure", "interface", "language", "security"};
    self_options.enable_tracing = true;

    result.data["self_validation"] = "true";
    result.data["message"] = "Akao validating itself using its own rules";

    return validateProject(self_options, context);
}

executor::ExecutionResult ValidateCommand::traceViolation(const std::string& violation_id,
                                                         const executor::ExecutionContext& context) {
    executor::ExecutionResult result;
    result.success = true;
    result.exit_code = 0;

    try {
        // Generate detailed trace for specific violation
        auto trace_data = generateTraceData(violation_id);

        result.data["trace_target"] = violation_id;
        result.data["trace_data"] = "{";
        for (const auto& [key, value] : trace_data) {
            result.data["trace_data"] += "\"" + key + "\":\"" + value + "\",";
        }
        if (!trace_data.empty()) {
            result.data["trace_data"].pop_back(); // Remove trailing comma
        }
        result.data["trace_data"] += "}";

    } catch (const std::exception& e) {
        result.success = false;
        result.exit_code = 1;
        result.error_message = "Failed to trace violation: " + std::string(e.what());
    }

    return result;
}

executor::ExecutionResult ValidateCommand::checkRuleCategories(const ValidationOptions& options,
                                                              const executor::ExecutionContext& context) {
    executor::ExecutionResult result;
    result.success = true;
    result.exit_code = 0;

    result.data["operation"] = "check_rules";
    result.data["categories"] = "[";
    for (size_t i = 0; i < options.rule_categories.size(); ++i) {
        if (i > 0) result.data["categories"] += ",";
        result.data["categories"] += "\"" + options.rule_categories[i] + "\"";
    }
    result.data["categories"] += "]";

    // Perform targeted validation
    return validateProject(options, context);
}

executor::ExecutionResult ValidateCommand::autoFixViolations(const ValidationOptions& options,
                                                            const executor::ExecutionContext& context) {
    executor::ExecutionResult result;
    result.success = true;
    result.exit_code = 0;

    result.data["operation"] = "auto_fix";
    result.data["dry_run"] = options.dry_run ? "true" : "false";

    // First, validate to find violations
    auto validation_result = validateProject(options, context);
    if (!validation_result.success) {
        return validation_result;
    }

    // TODO: Implement actual auto-fixing logic
    result.data["fixes_applied"] = "0";
    result.data["message"] = "Auto-fix functionality not yet implemented";

    return result;
}

std::vector<std::string> ValidateCommand::loadRuleCategories(const std::vector<std::string>& categories) {
    std::vector<std::string> rule_files;

    for (const auto& category : categories) {
        std::string rule_dir = "rules/" + category + "/";
        if (std::filesystem::exists(rule_dir)) {
            for (const auto& entry : std::filesystem::directory_iterator(rule_dir)) {
                if (entry.is_regular_file() && entry.path().extension() == ".yaml") {
                    rule_files.push_back(entry.path().string());
                }
            }
        }
    }

    return rule_files;
}

std::map<std::string, std::string> ValidateCommand::generateTraceData(const std::string& violation_id) {
    std::map<std::string, std::string> trace_data;

    // Parse violation ID to extract components
    // Format: akao:rule::category:rule_name:version:violation:file:line
    std::vector<std::string> components;
    std::string current = violation_id;
    size_t pos = 0;
    
    while ((pos = current.find(":")) != std::string::npos) {
        components.push_back(current.substr(0, pos));
        current.erase(0, pos + 1);
    }
    components.push_back(current);

    if (components.size() >= 7) {
        trace_data["rule_category"] = components[2];
        trace_data["rule_name"] = components[3];
        trace_data["rule_version"] = components[4];
        trace_data["file_path"] = components[6];
        if (components.size() > 7) {
            trace_data["line_number"] = components[7];
        }
    }

    trace_data["trace_timestamp"] = std::to_string(std::time(nullptr));
    trace_data["validation_engine"] = "universal_validator";
    trace_data["trace_depth"] = "full";

    return trace_data;
}

bool ValidateCommand::exportResults(const std::string& file_path, const std::map<std::string, std::string>& results) {
    try {
        std::ofstream file(file_path);
        if (!file.is_open()) {
            return false;
        }

        // Export as YAML format
        file << "akao_validation:\n";
        file << "  version: 1.0.0\n";
        for (const auto& [key, value] : results) {
            file << "  " << key << ": " << value << "\n";
        }

        file.close();
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::string ValidateCommand::getHelp() const {
    return R"(akao validate - Universal Validation Engine

USAGE:
    akao validate [OPTIONS] [TARGET]

DESCRIPTION:
    Universal validation that works on ANY project type. Combines functionality
    of check, trace, and self-validate commands.

OPTIONS:
    --rules <category>       Validate specific rule category (structure, interface, etc.)
    --rules all             Validate all rule categories
    --philosophy <name>     Validate specific philosophy compliance
    --trace                 Include detailed violation tracing
    --fix                   Auto-fix violations where possible
    --self                  Self-validate Akao against its own rules
    --dry-run              Show what would be fixed without making changes
    --export <file>         Export results to file
    --metrics              Include detailed metrics in output

EXAMPLES:
    akao validate                          # Validate current directory
    akao validate /path/to/project         # Validate specific project
    akao validate --rules structure        # Check structure rules only
    akao validate --self                   # Self-validate Akao
    akao validate --trace --export report.yaml  # Full trace with export
    akao validate akao:rule::structure:class_separation:v1:violation:src/main.cpp:42  # Trace specific violation

PHILOSOPHY:
    Embodies Universal Validation - works on any project type with consistent
    interface and comprehensive traceability.
)";
}

std::string ValidateCommand::getUsage() const {
    return "akao validate [OPTIONS] [TARGET]";
}

std::map<std::string, std::string> ValidateCommand::getSupportedOptions() const {
    return {
        {"rules", "Rule category to validate (structure, interface, language, etc.)"},
        {"philosophy", "Philosophy to validate against"},
        {"export", "File to export results to"}
    };
}

std::map<std::string, std::string> ValidateCommand::getSupportedFlags() const {
    return {
        {"help", "Show command help"},
        {"trace", "Include detailed violation tracing"},
        {"fix", "Auto-fix violations where possible"},
        {"self", "Self-validate Akao against its own rules"},
        {"dry-run", "Show what would be fixed without making changes"},
        {"metrics", "Include detailed metrics in output"}
    };
}

} // namespace akao::interfaces::cli::commands
