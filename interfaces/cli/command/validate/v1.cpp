#include "v1.hpp"
#include "../../../../core/engine/validator/universal/v1.hpp"
#include "../../../../core/filesystem/scanner/directory_scanner.hpp"
#include "../../../../core/engine/parser/yaml/parser/v1.hpp"
#include "../../../../core/trace/tracer/violation_tracer.hpp"
#include "../../../../core/engine/self_validation/v1.hpp"
#include "../../../../core/engine/phase6/v1.hpp"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <iomanip>

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

        // Handle Phase 6 integration testing
        if (options.phase6_integration) {
            return executePhase6Integration(options, context);
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
        else if (arg == "--phase6") {
            options.phase6_integration = true;
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

    std::cout << "Debug: Starting validateProject with target: " << options.target_path << std::endl;

    // Initialize universal validator
    core::engine::validator::UniversalValidator validator;
    if (!validator.initialize()) {
        result.success = false;
        result.exit_code = 1;
        result.error_message = "Failed to initialize validator";
        return result;
    }
    
    // Debug: Check if rules were loaded
    size_t loaded_rules_count = validator.getLoadedRulesCount();
    std::cout << "Debug: Loaded " << loaded_rules_count << " rules" << std::endl;
    
    // Configure validation through public methods
    validator.enableAutoFix(options.enable_fixing);
    validator.setOutputFormat(context.output_format);

    // Perform validation - use the basic validate method for now
    core::engine::validator::ValidationResult validation_result = validator.validate(options.target_path);

    std::cout << "Debug: Validation completed. Total rules executed: " << validation_result.getTotalRulesExecuted() << std::endl;

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

    // Set files processed count from validation result
    result.files_processed = validation_result.getFilesProcessed();

    return result;
}

executor::ExecutionResult ValidateCommand::selfValidate(const ValidationOptions& options,
                                                       const executor::ExecutionContext& context) {
    executor::ExecutionResult result;
    result.success = true;
    result.exit_code = 0;
    
    std::cout << "🎯 Initiating Phase 3: Self-Validation Architecture Implementation" << std::endl;
    
    try {
        // Create Phase 3 self-validation orchestrator
        akao::core::engine::self_validation::SelfValidationOrchestrator orchestrator;
        
        // Execute comprehensive Phase 3 self-validation
        auto validation_result = orchestrator.executePhase3SelfValidation();
        
        // Set execution result based on validation outcome
        result.success = validation_result.overall_success;
        result.exit_code = validation_result.overall_success ? 0 : 1;
        
        // Populate result data
        result.data["validation_type"] = "phase3_self_validation";
        result.data["architectural_compliance"] = std::to_string(validation_result.architectural_compliance_score);
        result.data["philosophical_consistency"] = std::to_string(validation_result.philosophical_consistency_score);
        result.data["mathematical_soundness"] = std::to_string(validation_result.mathematical_soundness_score);
        result.data["overall_quality"] = std::to_string(validation_result.overall_quality_score);
        result.data["total_issues"] = std::to_string(validation_result.total_issues_found);
        result.data["critical_issues"] = std::to_string(validation_result.critical_issues);
        result.data["files_analyzed"] = std::to_string(validation_result.reflection_result.files_analyzed);
        result.data["functions_analyzed"] = std::to_string(validation_result.reflection_result.functions_analyzed);
        
        // Set result messages
        if (validation_result.overall_success) {
            result.output_message = "✅ Phase 3: Self-Validation Architecture Implementation COMPLETE\n" +
                                   validation_result.executive_summary;
        } else {
            result.output_message = "⚠️ Phase 3: Self-Validation requires attention\n" +
                                   validation_result.executive_summary;
        }
        
        // Export detailed report if requested
        if (!options.export_file.empty()) {
            std::ofstream report_file(options.export_file);
            if (report_file.is_open()) {
                report_file << validation_result.detailed_report;
                report_file.close();
                result.data["exported_report"] = options.export_file;
            }
        }
        
        // Set performance metrics
        result.files_processed = validation_result.reflection_result.files_analyzed;
        result.violations_found = validation_result.total_issues_found;
        
        std::cout << "\n📊 Phase 3 Self-Validation Summary:" << std::endl;
        std::cout << "   Overall Success: " << (validation_result.overall_success ? "✅ YES" : "❌ NO") << std::endl;
        std::cout << "   Files Analyzed: " << validation_result.reflection_result.files_analyzed << std::endl;
        std::cout << "   Functions Analyzed: " << validation_result.reflection_result.functions_analyzed << std::endl;
        std::cout << "   Issues Found: " << validation_result.total_issues_found << std::endl;
        std::cout << "   Quality Score: " << std::fixed << std::setprecision(1) 
                  << (validation_result.overall_quality_score * 100) << "%" << std::endl;
        
    } catch (const std::exception& e) {
        result.success = false;
        result.exit_code = 1;
        result.error_message = "Phase 3 self-validation failed: " + std::string(e.what());
        std::cerr << "❌ Phase 3 self-validation error: " << e.what() << std::endl;
        
        // Fallback to traditional self-validation
        std::cout << "🔄 Falling back to traditional self-validation..." << std::endl;
        ValidationOptions self_options = options;
        self_options.target_path = ".";
        self_options.rule_categories = {"structure", "interface", "language", "security"};
        self_options.enable_tracing = true;
        
        return validateProject(self_options, context);
    }

    return result;
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
        {"metrics", "Include detailed metrics in output"},
        {"phase6", "Execute Phase 6 complete system integration testing"}
    };
}

executor::ExecutionResult ValidateCommand::executePhase6Integration(const ValidationOptions& options,
                                                                   const executor::ExecutionContext& context) {
    executor::ExecutionResult result;
    result.success = true;
    result.exit_code = 0;

    try {
        std::ostringstream output;
        output << "=== AKAO Phase 6: Complete System Integration and Final Validation ===\n\n";

        // Initialize Phase 6 System Integrator
        akao::core::engine::phase6::Phase6SystemIntegrator integrator(context.current_directory, context.verbose_mode);

        // Execute Phase 6 integration
        auto phase6_result = integrator.executePhase6Integration();

        // Generate output based on results
        if (phase6_result.success) {
            output << "✅ Phase 6 Integration: SUCCESS\n\n";
            output << "Overall Score: " << std::fixed << std::setprecision(1) << phase6_result.overall_score << "%\n";
            output << "Execution Time: " << phase6_result.end_to_end_latency_ms << " ms\n\n";

            // Component status summary
            output << "Component Integration Results:\n";
            for (const auto& component : phase6_result.component_status) {
                std::string status = component.second ? "✅ PASS" : "❌ FAIL";
                output << "  " << component.first << ": " << status << "\n";
            }
            output << "\n";

            // Quality metrics
            output << "Quality Metrics:\n";
            output << "  Architectural Compliance: " << std::fixed << std::setprecision(1) << phase6_result.architectural_compliance_score << "%\n";
            output << "  Philosophical Alignment: " << std::fixed << std::setprecision(1) << phase6_result.philosophical_alignment_score << "%\n";
            output << "  Production Readiness: " << std::fixed << std::setprecision(1) << phase6_result.production_readiness_score << "%\n\n";

            // Production certification status
            if (phase6_result.production_readiness_score >= 75.0) {
                output << "🎉 PRODUCTION CERTIFICATION: APPROVED\n";
                output << "System is ready for production deployment.\n\n";
            } else {
                output << "⚠️  PRODUCTION CERTIFICATION: PENDING\n";
                output << "System requires improvements before production deployment.\n\n";
            }

            result.success = true;
            result.exit_code = 0;
            result.output_message = output.str();

            // Include detailed report if verbose
            if (context.verbose_mode) {
                std::string detailed_report = integrator.generateFinalCertificationReport(phase6_result);
                result.output_message += "\n" + detailed_report;
            }

        } else {
            output << "❌ Phase 6 Integration: FAILED\n\n";
            output << "Critical Issues Detected:\n";
            for (const auto& issue : phase6_result.critical_issues) {
                output << "  🚨 " << issue << "\n";
            }
            output << "\n";

            if (!phase6_result.warnings.empty()) {
                output << "Warnings:\n";
                for (const auto& warning : phase6_result.warnings) {
                    output << "  ⚠️  " << warning << "\n";
                }
                output << "\n";
            }

            if (!phase6_result.recommendations.empty()) {
                output << "Recommendations:\n";
                for (const auto& recommendation : phase6_result.recommendations) {
                    output << "  💡 " << recommendation << "\n";
                }
                output << "\n";
            }

            result.success = false;
            result.exit_code = 1;
            result.error_message = "Phase 6 integration validation failed";
            result.output_message = output.str();
        }

        // Set performance metrics
        result.execution_time_seconds = phase6_result.end_to_end_latency_ms / 1000.0;
        result.files_processed = phase6_result.total_components_tested;

        return result;

    } catch (const std::exception& e) {
        result.success = false;
        result.exit_code = 1;
        result.error_message = "Phase 6 integration execution failed: " + std::string(e.what());
        return result;
    }
}

} // namespace akao::interfaces::cli::commands
