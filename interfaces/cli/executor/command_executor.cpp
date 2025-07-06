#include "command_executor.hpp"
#include "../../../core/rule/loader/rule_loader.hpp"
#include "../../../core/rule/registry/rule_registry.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <filesystem>

namespace akao::interfaces::cli::executor {

CommandExecutor::CommandExecutor() {
    // Initialize core components
    validator_ = std::make_unique<core::engine::validator::UniversalValidator>();
    enforcer_ = std::make_unique<core::filesystem::enforcer::StructureEnforcer>();
    tracer_ = std::make_unique<core::trace::tracer::ViolationTracer>();
    reporter_ = std::make_unique<core::trace::reporter::ReportGenerator>();

    // Setup command handlers map
    command_handlers_["validate"] = [this](const auto& options, const auto& flags, const auto& args) {
        return executeValidate(options, flags, args);
    };
    command_handlers_["init"] = [this](const auto& options, const auto& flags, const auto& args) {
        return executeInit(options, flags, args);
    };
    command_handlers_["generate"] = [this](const auto& options, const auto& flags, const auto& args) {
        return executeGenerate(options, flags, args);
    };
    command_handlers_["check"] = [this](const auto& options, const auto& flags, const auto& args) {
        return executeCheck(options, flags, args);
    };
    command_handlers_["trace"] = [this](const auto& options, const auto& flags, const auto& args) {
        return executeTrace(options, flags, args);
    };
    command_handlers_["report"] = [this](const auto& options, const auto& flags, const auto& args) {
        return executeReport(options, flags, args);
    };
    command_handlers_["fix"] = [this](const auto& options, const auto& flags, const auto& args) {
        return executeFix(options, flags, args);
    };
    command_handlers_["config"] = [this](const auto& options, const auto& flags, const auto& args) {
        return executeConfig(options, flags, args);
    };
    command_handlers_["self-validate"] = [this](const auto& options, const auto& flags, const auto& args) {
        return executeSelfValidate(options, flags, args);
    };
    command_handlers_["status"] = [this](const auto& options, const auto& flags, const auto& args) {
        return executeStatus(options, flags, args);
    };
    command_handlers_["help"] = [this](const auto& options, const auto& flags, const auto& args) {
        return executeHelp(options, flags, args);
    };
}

bool CommandExecutor::initialize() {
    try {
        // Load configuration if available
        if (!context_.config_file_path.empty()) {
            if (!loadConfiguration(context_.config_file_path)) {
                utils::printWarning("Failed to load configuration file: " + context_.config_file_path);
            }
        }

        // Initialize validator with rules
        if (!validator_->initialize()) {
            utils::printError("Failed to initialize universal validator");
            return false;
        }

        // Initialize other components
        if (!enforcer_->initialize()) {
            utils::printError("Failed to initialize structure enforcer");
            return false;
        }

        if (!tracer_->initialize()) {
            utils::printError("Failed to initialize violation tracer");
            return false;
        }

        if (!reporter_->initialize()) {
            utils::printError("Failed to initialize report generator");
            return false;
        }

        utils::printInfo("Akao CLI executor initialized successfully");
        return true;
    } catch (const std::exception& e) {
        utils::printError("Initialization failed: " + std::string(e.what()));
        return false;
    }
}

void CommandExecutor::setContext(const ExecutionContext& context) {
    context_ = context;
}

const ExecutionContext& CommandExecutor::getContext() const {
    return context_;
}

ExecutionResult CommandExecutor::execute(const parser::ParseResult& parse_result) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    ExecutionResult result;
    
    try {
        // Apply global options and flags
        applyGlobalOptions(parse_result.options);
        applyGlobalFlags(parse_result.flags);
        
        // Find and execute command handler
        auto handler_it = command_handlers_.find(parse_result.command_name);
        if (handler_it == command_handlers_.end()) {
            result = createErrorResult("Unknown command: " + parse_result.command_name);
        } else {
            result = handler_it->second(parse_result.options, parse_result.flags, parse_result.positional_arguments);
        }
        
    } catch (const std::exception& e) {
        result = createErrorResult("Command execution failed: " + std::string(e.what()));
    }
    
    // Calculate execution time
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    result.execution_time_seconds = duration.count() / 1000000.0;
    
    // Log and update statistics
    logExecution(parse_result.command_name, result);
    updateStats(parse_result.command_name, result);
    
    return result;
}

ExecutionResult CommandExecutor::executeValidate(const std::map<std::string, std::string>& options,
                                                const std::map<std::string, bool>& flags,
                                                const std::vector<std::string>& args) {
    
    // Parse target path
    std::string target_path = resolveTargetPath(args);
    
    // Create validation options
    ValidationOptions validation_options(target_path);
    
    // Apply command-line options
    auto rules_it = options.find("rules");
    if (rules_it != options.end()) {
        validation_options.rules_filter = rules_it->second;
    }
    
    auto philosophy_it = options.find("philosophy");
    if (philosophy_it != options.end()) {
        validation_options.philosophy_filter = philosophy_it->second;
    }
    
    auto format_it = options.find("format");
    if (format_it != options.end()) {
        validation_options.output_format = format_it->second;
    }
    
    // Apply flags
    auto trace_it = flags.find("trace");
    if (trace_it != flags.end()) {
        validation_options.include_trace = trace_it->second;
    }
    
    auto fix_it = flags.find("fix");
    if (fix_it != flags.end()) {
        validation_options.auto_fix = fix_it->second;
    }
    
    // Use unified validation
    return performUnifiedValidation(validation_options);
}

ExecutionResult CommandExecutor::executeInit(const std::map<std::string, std::string>& options,
                                            const std::map<std::string, bool>& flags,
                                            const std::vector<std::string>& args) {
    try {
        std::string target_path = resolveTargetPath(args);
        
        // Ensure target directory exists
        if (!std::filesystem::exists(target_path)) {
            std::filesystem::create_directories(target_path);
        }
        
        utils::printInfo("Initializing Akao project in: " + target_path);
        
        // Create basic Akao project structure
        std::string akao_config_dir = target_path + "/.akao";
        std::filesystem::create_directories(akao_config_dir);
        
        // Create basic configuration
        std::string config_content = R"(# Akao Configuration
project:
  name: "New Akao Project"
  type: "universal"
  version: "1.0.0"

rules:
  enabled: true
  strictness: "standard"
  custom_rules_path: ".akao/rules"

validation:
  auto_validate: true
  fail_on_violations: true
  
output:
  format: "yaml"
  reports_path: ".akao/reports"
)";
        
        std::ofstream config_file(akao_config_dir + "/config.yaml");
        if (config_file.is_open()) {
            config_file << config_content;
            config_file.close();
        }
        
        // Create rules directory
        std::filesystem::create_directories(akao_config_dir + "/rules");
        
        // Create reports directory
        std::filesystem::create_directories(akao_config_dir + "/reports");
        
        utils::printSuccess("Akao project initialized successfully!");
        utils::printInfo("Configuration created at: " + akao_config_dir + "/config.yaml");
        
        return createSuccessResult("Project initialization completed");
        
    } catch (const std::exception& e) {
        ExecutionResult result;
        handleFileSystemError(e, result);
        return result;
    }
}

ExecutionResult CommandExecutor::executeGenerate(const std::map<std::string, std::string>& options,
                                                const std::map<std::string, bool>& flags,
                                                const std::vector<std::string>& args) {
    try {
        if (args.empty()) {
            return createErrorResult("Generate command requires a template type argument");
        }
        
        std::string template_type = args[0];
        std::string target_path = args.size() > 1 ? args[1] : context_.current_directory;
        
        utils::printInfo("Generating " + template_type + " in: " + target_path);
        
        // Use structure enforcer to create project structure
        auto enforcement_result = enforcer_->createProjectStructure(target_path, template_type);
        
        if (enforcement_result.success) {
            utils::printSuccess("Generated " + template_type + " project successfully!");
            return createSuccessResult("Generation completed");
        } else {
            return createErrorResult("Failed to generate " + template_type + " project: " + enforcement_result.error_message);
        }
        
    } catch (const std::exception& e) {
        ExecutionResult result;
        handleFileSystemError(e, result);
        return result;
    }
}

ExecutionResult CommandExecutor::executeCheck(const std::map<std::string, std::string>& options,
                                             const std::map<std::string, bool>& flags,
                                             const std::vector<std::string>& args) {
    
    // Parse target path (or use first positional argument as category)
    std::string target_path;
    std::string category_filter;
    
    if (!args.empty()) {
        // Check if first argument is a path or category
        if (std::filesystem::exists(args[0])) {
            target_path = args[0];
        } else {
            // Treat as category, use current directory as target
            category_filter = args[0];
            target_path = std::filesystem::current_path().string();
        }
    } else {
        target_path = std::filesystem::current_path().string();
    }
    
    utils::printInfo("Checking project: " + target_path);
    if (!category_filter.empty()) {
        utils::printInfo("Category filter: " + category_filter);
    }
    
    // Create validation options
    ValidationOptions validation_options(target_path);
    
    // Apply category filter
    if (!category_filter.empty()) {
        validation_options.rules_filter = category_filter;
    }
    
    // Apply command-line options
    auto category_it = options.find("category");
    if (category_it != options.end()) {
        validation_options.rules_filter = category_it->second;
    }
    
    auto rule_it = options.find("rule");
    if (rule_it != options.end()) {
        validation_options.rules_filter = rule_it->second;
    }
    
    auto philosophy_it = options.find("philosophy");
    if (philosophy_it != options.end()) {
        validation_options.philosophy_filter = philosophy_it->second;
    }
    
    // Apply flags
    auto fix_it = flags.find("fix");
    if (fix_it != flags.end()) {
        validation_options.auto_fix = fix_it->second;
    }
    
    // Use unified validation
    auto result = performUnifiedValidation(validation_options);
    result.output_message = "Check completed";
    
    return result;
}

ExecutionResult CommandExecutor::executeTrace(const std::map<std::string, std::string>& options,
                                             const std::map<std::string, bool>& flags,
                                             const std::vector<std::string>& args) {
    try {
        std::string target_path = resolveTargetPath(args);
        
        utils::printInfo("Tracing violations in: " + target_path);
        
        // Perform validation first to get violations
        auto validation_result = validator_->validate(target_path);
        
        if (validation_result.getViolations().empty()) {
            utils::printSuccess("No violations to trace - project is compliant!");
            return createSuccessResult("No violations found");
        }
        
        // Trace each violation
        for (const auto& violation : validation_result.getViolations()) {
            std::string trace_result = tracer_->traceViolation(violation);
            
            if (!context_.quiet_mode) {
                std::cout << "Violation: " << violation.message << std::endl;
                std::cout << "  File: " << violation.file_path << std::endl;
                std::cout << "  Rule: " << violation.rule_id << std::endl;
                std::cout << "  Trace: " << trace_result << std::endl;
                if (!violation.suggestion.empty()) {
                    std::cout << "  Suggestion: " << violation.suggestion << std::endl;
                }
                std::cout << std::endl;
            }
        }
        
        ExecutionResult result = createSuccessResult("Violation tracing completed");
        result.violations_found = validation_result.getViolations().size();
        
        return result;
        
    } catch (const std::exception& e) {
        ExecutionResult result;
        handleValidationError(e, result);
        return result;
    }
}

ExecutionResult CommandExecutor::executeReport(const std::map<std::string, std::string>& options,
                                              const std::map<std::string, bool>& flags,
                                              const std::vector<std::string>& args) {
    try {
        std::string target_path = resolveTargetPath(args);
        std::string output_path = resolveOutputPath(options);
        
        utils::printInfo("Generating compliance report for: " + target_path);
        
        // First validate to get violations
        auto validation_result = validator_->validate(target_path);
        
        // Trace all violations to populate the tracer
        for (const auto& violation : validation_result.getViolations()) {
            tracer_->traceViolation(violation);
        }
        
        // Generate comprehensive report from tracer
        auto report = reporter_->generateComplianceReport(*tracer_);
        
        std::string formatted_report = formatComplianceReport(report);
        
        // Output to file
        if (!output_path.empty()) {
            std::ofstream output_file(output_path);
            if (output_file.is_open()) {
                output_file << formatted_report;
                output_file.close();
                utils::printSuccess("Report generated: " + output_path);
            } else {
                return createErrorResult("Failed to write report to: " + output_path);
            }
        }
        
        // Print summary unless quiet mode
        if (!context_.quiet_mode) {
            std::cout << formatted_report << std::endl;
        }
        
        ExecutionResult result = createSuccessResult("Report generation completed");
        result.violations_found = validation_result.getViolations().size();
        
        return result;
        
    } catch (const std::exception& e) {
        ExecutionResult result;
        handleValidationError(e, result);
        return result;
    }
}

ExecutionResult CommandExecutor::executeFix(const std::map<std::string, std::string>& options,
                                           const std::map<std::string, bool>& flags,
                                           const std::vector<std::string>& args) {
    try {
        std::string target_path = resolveTargetPath(args);
        
        utils::printInfo("Attempting to fix violations in: " + target_path);
        
        // First validate to get violations
        auto validation_result = validator_->validate(target_path);
        
        if (validation_result.getViolations().empty()) {
            utils::printSuccess("No violations to fix - project is already compliant!");
            return createSuccessResult("No violations found");
        }
        
        size_t fixed_count = 0;
        
        // Attempt to fix each violation
        for (const auto& violation : validation_result.getViolations()) {
            try {
                auto enforcement_result = enforcer_->autoFix(target_path, {violation});
                if (enforcement_result.success && !enforcement_result.actions_taken.empty()) {
                    fixed_count++;
                    if (context_.verbose_mode) {
                        utils::printSuccess("Fixed: " + violation.message);
                    }
                } else {
                    if (context_.verbose_mode) {
                        utils::printWarning("Could not auto-fix: " + violation.message);
                    }
                }
            } catch (const std::exception& fix_error) {
                if (context_.verbose_mode) {
                    utils::printError("Fix failed for: " + violation.message + " - " + fix_error.what());
                }
            }
        }
        
        ExecutionResult result = createSuccessResult("Fix operation completed");
        result.violations_found = validation_result.getViolations().size();
        result.violations_fixed = fixed_count;
        
        if (!context_.quiet_mode) {
            utils::printInfo("Fixed " + std::to_string(fixed_count) + " out of " + 
                           std::to_string(validation_result.getViolations().size()) + " violations");
            
            if (fixed_count < validation_result.getViolations().size()) {
                utils::printWarning("Some violations require manual intervention");
            }
        }
        
        return result;
        
    } catch (const std::exception& e) {
        ExecutionResult result;
        handleFileSystemError(e, result);
        return result;
    }
}

ExecutionResult CommandExecutor::executeConfig(const std::map<std::string, std::string>& options,
                                              const std::map<std::string, bool>& flags,
                                              const std::vector<std::string>& args) {
    try {
        if (args.empty()) {
            return createErrorResult("Config command requires an action (get, set, list)");
        }
        
        std::string action = args[0];
        
        if (action == "list") {
            utils::printInfo("Current configuration:");
            for (const auto& [key, value] : context_.global_config) {
                std::cout << "  " << key << ": " << value << std::endl;
            }
        } else if (action == "get" && args.size() > 1) {
            std::string key = args[1];
            auto it = context_.global_config.find(key);
            if (it != context_.global_config.end()) {
                std::cout << it->second << std::endl;
            } else {
                return createErrorResult("Configuration key not found: " + key);
            }
        } else if (action == "set" && args.size() > 2) {
            std::string key = args[1];
            std::string value = args[2];
            context_.global_config[key] = value;
            utils::printSuccess("Set " + key + " = " + value);
        } else {
            return createErrorResult("Invalid config command usage");
        }
        
        return createSuccessResult("Configuration operation completed");
        
    } catch (const std::exception& e) {
        ExecutionResult result;
        handleConfigurationError(e, result);
        return result;
    }
}

ExecutionResult CommandExecutor::executeSelfValidate(const std::map<std::string, std::string>& options,
                                                    const std::map<std::string, bool>& flags,
                                                    const std::vector<std::string>& args) {
    
    utils::printInfo("Performing Akao self-validation...");
    
    // Self-validate using current directory
    ValidationOptions validation_options(std::filesystem::current_path().string());
    
    // Apply any format options
    auto format_it = options.find("format");
    if (format_it != options.end()) {
        validation_options.output_format = format_it->second;
    }
    
    // Self-validation should be comprehensive (no filters)
    auto result = performUnifiedValidation(validation_options);
    
    // Update message to reflect self-validation context
    if (result.success) {
        result.output_message = "Self-validation completed";
        if (!context_.quiet_mode && result.violations_found == 0) {
            utils::printSuccess("Akao passes self-validation - all philosophies and rules satisfied!");
        }
    } else {
        result.output_message = "Self-validation completed";
        if (!context_.quiet_mode) {
            utils::printError("Akao failed self-validation!");
        }
    }
    
    return result;
}

ExecutionResult CommandExecutor::executeStatus(const std::map<std::string, std::string>& options,
                                              const std::map<std::string, bool>& flags,
                                              const std::vector<std::string>& args) {
    try {
        std::string target_path = resolveTargetPath(args);
        
        utils::printInfo("Akao Status Report");
        std::cout << "==================" << std::endl;
        std::cout << "Target: " << target_path << std::endl;
        std::cout << "Configuration: " << context_.config_file_path << std::endl;
        std::cout << "Output Format: " << context_.output_format << std::endl;
        std::cout << std::endl;
        
        // Show execution statistics
        std::cout << "Execution Statistics:" << std::endl;
        std::cout << "  Total Commands: " << stats_.total_commands_executed << std::endl;
        std::cout << "  Successful: " << stats_.successful_commands << std::endl;
        std::cout << "  Failed: " << stats_.failed_commands << std::endl;
        std::cout << "  Total Time: " << utils::formatExecutionTime(stats_.total_execution_time) << std::endl;
        std::cout << std::endl;
        
        // Show command breakdown
        if (!stats_.command_execution_counts.empty()) {
            std::cout << "Command Usage:" << std::endl;
            for (const auto& [command, count] : stats_.command_execution_counts) {
                double avg_time = stats_.command_execution_times.at(command) / count;
                std::cout << "  " << command << ": " << count << " times (avg: " 
                         << utils::formatExecutionTime(avg_time) << ")" << std::endl;
            }
        }
        
        return createSuccessResult("Status report completed");
        
    } catch (const std::exception& e) {
        ExecutionResult result;
        handleConfigurationError(e, result);
        return result;
    }
}

ExecutionResult CommandExecutor::executeHelp(const std::map<std::string, std::string>& options,
                                            const std::map<std::string, bool>& flags,
                                            const std::vector<std::string>& args) {
    ExecutionResult result = createSuccessResult("Help displayed");
    
    try {
        // Initialize command parser to get help
        parser::CommandParser parser;
        auto default_commands = parser::akao_commands::getDefaultCommands();
        for (const auto& command : default_commands) {
            parser.registerCommand(command);
        }
        
        if (args.empty()) {
            // Show global help
            std::cout << parser.getGlobalHelp() << std::endl;
        } else {
            // Show command-specific help
            std::string command_name = args[0];
            std::cout << parser.getCommandHelp(command_name) << std::endl;
        }
        
        return result;
        
    } catch (const std::exception& e) {
        return createErrorResult("Help display failed: " + std::string(e.what()));
    }
}

const CommandExecutor::ExecutionStats& CommandExecutor::getStats() const {
    return stats_;
}

void CommandExecutor::clearStats() {
    stats_ = ExecutionStats{};
}

// Helper method implementations
ExecutionResult CommandExecutor::createSuccessResult(const std::string& message) {
    ExecutionResult result;
    result.success = true;
    result.exit_code = 0;
    result.output_message = message;
    return result;
}

ExecutionResult CommandExecutor::createErrorResult(const std::string& error, int exit_code) {
    ExecutionResult result;
    result.success = false;
    result.exit_code = exit_code;
    result.error_message = error;
    return result;
}

void CommandExecutor::logExecution(const std::string& command, const ExecutionResult& result) {
    if (context_.debug_mode) {
        utils::printDebug("Command: " + command + 
                         " | Success: " + (result.success ? "true" : "false") +
                         " | Time: " + utils::formatExecutionTime(result.execution_time_seconds), true);
    }
}

void CommandExecutor::updateStats(const std::string& command, const ExecutionResult& result) {
    stats_.total_commands_executed++;
    if (result.success) {
        stats_.successful_commands++;
    } else {
        stats_.failed_commands++;
    }
    
    stats_.total_execution_time += result.execution_time_seconds;
    stats_.command_execution_counts[command]++;
    stats_.command_execution_times[command] += result.execution_time_seconds;
}

std::string CommandExecutor::resolveTargetPath(const std::vector<std::string>& args) {
    if (!args.empty()) {
        return std::filesystem::absolute(args[0]).string();
    }
    return context_.current_directory;
}

std::string CommandExecutor::resolveOutputPath(const std::map<std::string, std::string>& options) {
    auto it = options.find("output");
    if (it != options.end()) {
        return std::filesystem::absolute(it->second).string();
    }
    return "";
}

std::string CommandExecutor::resolveOutputFormat(const std::map<std::string, std::string>& options) {
    auto it = options.find("format");
    if (it != options.end()) {
        return it->second;
    }
    return context_.output_format;
}

bool CommandExecutor::loadConfiguration(const std::string& config_path) {
    try {
        if (!std::filesystem::exists(config_path)) {
            return false;
        }
        
        // Load and parse configuration file
        std::ifstream config_file(config_path);
        if (!config_file.is_open()) {
            return false;
        }
        
        // Simple key-value parsing for now
        std::string line;
        while (std::getline(config_file, line)) {
            size_t delimiter_pos = line.find(':');
            if (delimiter_pos != std::string::npos) {
                std::string key = line.substr(0, delimiter_pos);
                std::string value = line.substr(delimiter_pos + 1);
                
                // Trim whitespace
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                
                context_.global_config[key] = value;
            }
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

void CommandExecutor::applyGlobalOptions(const std::map<std::string, std::string>& options) {
    auto format_it = options.find("format");
    if (format_it != options.end()) {
        context_.output_format = format_it->second;
    }
    
    auto config_it = options.find("config");
    if (config_it != options.end()) {
        context_.config_file_path = config_it->second;
        loadConfiguration(context_.config_file_path);
    }
}

void CommandExecutor::applyGlobalFlags(const std::map<std::string, bool>& flags) {
    auto verbose_it = flags.find("verbose");
    if (verbose_it != flags.end()) {
        context_.verbose_mode = verbose_it->second;
    }
    
    auto quiet_it = flags.find("quiet");
    if (quiet_it != flags.end()) {
        context_.quiet_mode = quiet_it->second;
    }
    
    auto debug_it = flags.find("debug");
    if (debug_it != flags.end()) {
        context_.debug_mode = debug_it->second;
    }
}

std::string CommandExecutor::formatValidationResult(const core::engine::validator::ValidationResult& result) {
    std::string output_format = context_.output_format;
    
    // Default to table format if no format specified
    if (output_format.empty()) {
        output_format = "table";
    }
    
    std::stringstream ss;
    
    if (output_format == "table") {
        // Default table format (matches CLI specification)
        ss << formatValidationTableOutput(result);
    } else if (utils::isYamlOutput(output_format)) {
        ss << "validation_result:\n";
        ss << "  files_processed: " << result.getFilesProcessed() << "\n";
        ss << "  total_violations: " << result.getViolations().size() << "\n";
        ss << "  execution_time: " << result.getExecutionTime() << "\n";
        ss << "  violations:\n";
        
        for (const auto& violation : result.getViolations()) {
            ss << "    - rule_id: " << violation.rule_id << "\n";
            ss << "      message: " << violation.message << "\n";
            ss << "      file_path: " << violation.file_path << "\n";
            ss << "      line_number: " << violation.line_number << "\n";
            ss << "      severity: " << violation.severity << "\n";
        }
    } else if (utils::isJsonOutput(output_format)) {
        ss << "{\n";
        ss << "  \"validation_result\": {\n";
        ss << "    \"files_processed\": " << result.getFilesProcessed() << ",\n";
        ss << "    \"total_violations\": " << result.getViolations().size() << ",\n";
        ss << "    \"execution_time\": " << result.getExecutionTime() << ",\n";
        ss << "    \"violations\": [\n";
        
        for (size_t i = 0; i < result.getViolations().size(); ++i) {
            const auto& violation = result.getViolations()[i];
            ss << "      {\n";
            ss << "        \"rule_id\": \"" << violation.rule_id << "\",\n";
            ss << "        \"message\": \"" << violation.message << "\",\n";
            ss << "        \"file_path\": \"" << violation.file_path << "\",\n";
            ss << "        \"line_number\": " << violation.line_number << ",\n";
            ss << "        \"severity\": \"" << violation.severity << "\"\n";
            ss << "      }";
            if (i < result.getViolations().size() - 1) ss << ",";
            ss << "\n";
        }
        
        ss << "    ]\n";
        ss << "  }\n";
        ss << "}\n";
    } else {
        // Fallback to table format
        ss << formatValidationTableOutput(result);
    }
    
    return ss.str();
}

std::string CommandExecutor::formatValidationTableOutput(const core::engine::validator::ValidationResult& result) {
    std::stringstream ss;
    
    // Header with emoji and timestamp
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    if (result.getViolations().empty()) {
        ss << "✅ AKAO VALIDATION REPORT\n";
    } else {
        ss << "❌ AKAO VALIDATION REPORT\n";
    }
    
    ss << "📊 Project: " << result.getTargetPath() << "\n";
    ss << "🕒 Timestamp: " << std::put_time(std::gmtime(&time_t), "%Y-%m-%dT%H:%M:%SZ") << "\n";
    
    // Count violations by category
    std::map<std::string, int> category_violations;
    std::map<std::string, int> category_total;
    
    // Initialize categories from the violations
    for (const auto& violation : result.getViolations()) {
        std::string category = extractCategoryFromRuleId(violation.rule_id);
        category_violations[category]++;
        category_total[category]++; // For now, assume 1 rule per violation
    }
    
    // Add categories that might have 0 violations
    std::vector<std::string> all_categories = {
        "Structure", "Interface", "Language", "Security", 
        "Testing", "Automation", "Measurement", "Validation",
        "Visualization", "Documentation", "Governance", "Verification"
    };
    
    for (const auto& cat : all_categories) {
        if (category_total.find(cat) == category_total.end()) {
            category_total[cat] = 0; // No rules found for this category
            category_violations[cat] = 0;
        }
    }
    
    // Calculate totals
    int total_rules = 0;
    int total_violations = result.getViolations().size();
    
    for (const auto& [cat, count] : category_total) {
        total_rules += count;
    }
    
    int total_passed = total_rules - total_violations;
    
    ss << "🏛️ Rules: " << total_rules << " checked, " << total_passed << " passed, " << total_violations << " failed\n\n";
    
    // Create table
    ss << "┌─────────────────────────┬──────────┬──────────┬───────────┐\n";
    ss << "│ Category                │ Rules    │ Passed   │ Failed    │\n";
    ss << "├─────────────────────────┼──────────┼──────────┼───────────┤\n";
    
    for (const auto& cat : all_categories) {
        if (category_total[cat] > 0 || category_violations[cat] > 0) {
            int rules = std::max(category_total[cat], 1); // At least 1 if there are violations
            int failed = category_violations[cat];
            int passed = rules - failed;
            
            ss << "│ " << std::left << std::setw(23) << cat 
               << " │ " << std::right << std::setw(8) << rules
               << " │ " << std::right << std::setw(8) << passed 
               << " │ " << std::right << std::setw(9) << failed << " │\n";
        }
    }
    
    ss << "├─────────────────────────┼──────────┼──────────┼───────────┤\n";
    ss << "│ " << std::left << std::setw(23) << "TOTAL"
       << " │ " << std::right << std::setw(8) << total_rules
       << " │ " << std::right << std::setw(8) << total_passed
       << " │ " << std::right << std::setw(9) << total_violations << " │\n";
    ss << "└─────────────────────────┴──────────┴──────────┴───────────┘\n";
    
    // Show violations if any
    if (!result.getViolations().empty()) {
        ss << "\n❌ VIOLATIONS:\n";
        for (const auto& violation : result.getViolations()) {
            ss << violation.rule_id << ":" << violation.file_path << ":" << violation.line_number << "\n";
            ss << "  📁 File: " << violation.file_path << ":" << violation.line_number << "\n";
            ss << "  📋 Rule: " << extractRuleNameFromId(violation.rule_id) << "\n";
            ss << "  🎯 Issue: " << violation.message << "\n";
            ss << "  💡 Suggestion: " << violation.suggestion << "\n";
            if (!violation.philosophy_id.empty()) {
                ss << "  📚 Philosophy: " << violation.philosophy_id << "\n";
            }
            ss << "\n";
        }
    }
    
    return ss.str();
}

std::string CommandExecutor::extractCategoryFromRuleId(const std::string& rule_id) {
    // Extract category from rule_id like "akao:rule::structure:class_separation:v1"
    size_t start = rule_id.find("::") + 2;
    if (start == std::string::npos + 2) return "Unknown";
    
    size_t end = rule_id.find(":", start);
    if (end == std::string::npos) return "Unknown";
    
    std::string category = rule_id.substr(start, end - start);
    
    // Capitalize first letter
    if (!category.empty()) {
        category[0] = std::toupper(category[0]);
    }
    
    return category;
}

std::string CommandExecutor::extractRuleNameFromId(const std::string& rule_id) {
    // Extract rule name from rule_id like "akao:rule::structure:class_separation:v1"
    size_t start = rule_id.find("::") + 2;
    if (start == std::string::npos + 2) return "Unknown Rule";
    
    size_t category_end = rule_id.find(":", start);
    if (category_end == std::string::npos) return "Unknown Rule";
    
    size_t rule_start = category_end + 1;
    size_t rule_end = rule_id.find(":", rule_start);
    if (rule_end == std::string::npos) rule_end = rule_id.length();
    
    std::string rule_name = rule_id.substr(rule_start, rule_end - rule_start);
    
    // Convert underscore to space and capitalize
    std::replace(rule_name.begin(), rule_name.end(), '_', ' ');
    if (!rule_name.empty()) {
        rule_name[0] = std::toupper(rule_name[0]);
        for (size_t i = 1; i < rule_name.length(); ++i) {
            if (rule_name[i-1] == ' ') {
                rule_name[i] = std::toupper(rule_name[i]);
            }
        }
    }
    
    return rule_name;
}

ExecutionResult CommandExecutor::performUnifiedValidation(const ValidationOptions& options) {
    try {
        // Validate target path exists
        if (!std::filesystem::exists(options.target_path)) {
            return createErrorResult("Target path does not exist: " + options.target_path);
        }
        
        utils::printInfo("Validating: " + options.target_path);
        
        // Perform validation using the universal validator
        auto validation_result = validator_->validate(options.target_path);
        
        // Filter results if needed
        if (!options.rules_filter.empty() && options.rules_filter != "all") {
            validation_result = filterValidationResultsByCategory(validation_result, options.rules_filter);
        }
        
        if (!options.philosophy_filter.empty()) {
            validation_result = filterValidationResultsByPhilosophy(validation_result, options.philosophy_filter);
        }
        
        // Create execution result
        ExecutionResult result = createSuccessResult("Validation completed");
        result.violations_found = validation_result.getViolations().size();
        result.files_processed = validation_result.getFilesProcessed();
        
        // Format and output results unless quiet mode
        if (!context_.quiet_mode) {
            // Use specified format or default
            std::string original_format = context_.output_format;
            if (!options.output_format.empty()) {
                context_.output_format = options.output_format;
            }
            
            std::string formatted_result = formatValidationResult(validation_result);
            std::cout << formatted_result << std::endl;
            
            // Restore original format
            context_.output_format = original_format;
            
            // Set result status based on violations
            if (validation_result.getViolations().empty()) {
                utils::printSuccess("No violations found - project is compliant!");
            } else {
                utils::printWarning("Found " + std::to_string(validation_result.getViolations().size()) + " violations");
                result.success = false;
                result.exit_code = 1;
            }
        }
        
        // Auto-fix if requested
        if (options.auto_fix && !validation_result.getViolations().empty()) {
            utils::printInfo("Attempting to auto-fix violations...");
            // TODO: Implement auto-fix logic
            utils::printWarning("Auto-fix functionality not yet implemented");
        }
        
        return result;
        
    } catch (const std::exception& e) {
        return createErrorResult("Validation failed: " + std::string(e.what()));
    }
}

core::engine::validator::ValidationResult CommandExecutor::filterValidationResultsByCategory(
    const core::engine::validator::ValidationResult& original_result,
    const std::string& category) {
    
    // Create a new result with filtered violations
    core::engine::validator::ValidationResult filtered_result(
        original_result.getTargetPath(),
        original_result.getTargetType()
    );
    
    // Copy metadata
    filtered_result.setFilesProcessed(original_result.getFilesProcessed());
    filtered_result.setTotalRulesExecuted(original_result.getTotalRulesExecuted());
    
    // Filter violations by category
    std::string target_category = category;
    std::transform(target_category.begin(), target_category.end(), target_category.begin(), ::tolower);
    
    for (const auto& violation : original_result.getViolations()) {
        std::string violation_category = extractCategoryFromRuleId(violation.rule_id);
        std::transform(violation_category.begin(), violation_category.end(), violation_category.begin(), ::tolower);
        
        if (violation_category == target_category) {
            filtered_result.addViolation(violation);
        }
    }
    
    return filtered_result;
}

core::engine::validator::ValidationResult CommandExecutor::filterValidationResultsByPhilosophy(
    const core::engine::validator::ValidationResult& original_result,
    const std::string& philosophy) {
    
    // Create a new result with filtered violations
    core::engine::validator::ValidationResult filtered_result(
        original_result.getTargetPath(),
        original_result.getTargetType()
    );
    
    // Copy metadata
    filtered_result.setFilesProcessed(original_result.getFilesProcessed());
    filtered_result.setTotalRulesExecuted(original_result.getTotalRulesExecuted());
    
    // Filter violations by philosophy
    for (const auto& violation : original_result.getViolations()) {
        if (violation.philosophy_id.find(philosophy) != std::string::npos) {
            filtered_result.addViolation(violation);
        }
    }
    
    return filtered_result;
}

std::string CommandExecutor::formatComplianceReport(const core::trace::reporter::Report& report) {
    std::stringstream ss;
    
    ss << "Compliance Report\n";
    ss << "================\n";
    ss << "Generated: " << std::chrono::duration_cast<std::chrono::seconds>(
           report.generated_at.time_since_epoch()).count() << "\n";
    ss << "Target: " << report.project_path << "\n";
    ss << "Total Violations: " << report.total_violations << "\n";
    
    // Calculate compliance score
    double compliance_percentage = 100.0;
    if (report.total_files_analyzed > 0) {
        compliance_percentage = 100.0 * (1.0 - (static_cast<double>(report.total_violations) / report.total_files_analyzed));
    }
    ss << "Compliance Score: " << compliance_percentage << "%\n\n";
    
    // Add sections if available
    auto summary_it = report.sections.find(core::trace::reporter::ReportSection::EXECUTIVE_SUMMARY);
    if (summary_it != report.sections.end()) {
        ss << "Executive Summary:\n" << summary_it->second << "\n\n";
    }
    
    auto recommendations_it = report.sections.find(core::trace::reporter::ReportSection::RECOMMENDATIONS);
    if (recommendations_it != report.sections.end()) {
        ss << "Recommendations:\n" << recommendations_it->second << "\n";
    }
    
    return ss.str();
}

std::string CommandExecutor::formatExecutionSummary(const ExecutionResult& result) {
    std::stringstream ss;
    
    ss << "Execution Summary:\n";
    ss << "  Status: " << (result.success ? "SUCCESS" : "FAILED") << "\n";
    ss << "  Exit Code: " << result.exit_code << "\n";
    ss << "  Execution Time: " << utils::formatExecutionTime(result.execution_time_seconds) << "\n";
    ss << "  Files Processed: " << utils::formatFileCount(result.files_processed) << "\n";
    
    if (result.violations_found > 0) {
        ss << "  Violations Found: " << utils::formatViolationCount(result.violations_found, "total") << "\n";
    }
    
    if (result.violations_fixed > 0) {
        ss << "  Violations Fixed: " << result.violations_fixed << "\n";
    }
    
    if (!result.output_message.empty()) {
        ss << "  Message: " << result.output_message << "\n";
    }
    
    if (!result.error_message.empty()) {
        ss << "  Error: " << result.error_message << "\n";
    }
    
    return ss.str();
}

void CommandExecutor::handleValidationError(const std::exception& e, ExecutionResult& result) {
    result.success = false;
    result.exit_code = 2;
    result.error_message = "Validation error: " + std::string(e.what());
    utils::printError(result.error_message);
}

void CommandExecutor::handleFileSystemError(const std::exception& e, ExecutionResult& result) {
    result.success = false;
    result.exit_code = 3;
    result.error_message = "Filesystem error: " + std::string(e.what());
    utils::printError(result.error_message);
}

void CommandExecutor::handleConfigurationError(const std::exception& e, ExecutionResult& result) {
    result.success = false;
    result.exit_code = 4;
    result.error_message = "Configuration error: " + std::string(e.what());
    utils::printError(result.error_message);
}

// Utility functions implementation
namespace utils {

void printColored(const std::string& message, const std::string& color) {
    if (!supportsColors()) {
        std::cout << message;
        return;
    }
    
    std::string color_code;
    if (color == "red") color_code = "\033[31m";
    else if (color == "green") color_code = "\033[32m";
    else if (color == "yellow") color_code = "\033[33m";
    else if (color == "blue") color_code = "\033[34m";
    else if (color == "magenta") color_code = "\033[35m";
    else if (color == "cyan") color_code = "\033[36m";
    else color_code = "\033[0m"; // default
    
    std::cout << color_code << message << "\033[0m";
}

void printSuccess(const std::string& message) {
    printColored("✓ " + message, "green");
    std::cout << std::endl;
}

void printError(const std::string& message) {
    printColored("✗ " + message, "red");
    std::cout << std::endl;
}

void printWarning(const std::string& message) {
    printColored("⚠ " + message, "yellow");
    std::cout << std::endl;
}

void printInfo(const std::string& message) {
    printColored("ℹ " + message, "cyan");
    std::cout << std::endl;
}

void printDebug(const std::string& message, bool debug_enabled) {
    if (!debug_enabled) return;
    printColored("🐛 " + message, "magenta");
    std::cout << std::endl;
}

std::string formatExecutionTime(double seconds) {
    if (seconds < 0.001) {
        return std::to_string(static_cast<int>(seconds * 1000000)) + "μs";
    } else if (seconds < 1.0) {
        return std::to_string(static_cast<int>(seconds * 1000)) + "ms";
    } else {
        return std::to_string(seconds) + "s";
    }
}

std::string formatFileCount(size_t count) {
    if (count == 1) {
        return "1 file";
    }
    return std::to_string(count) + " files";
}

std::string formatViolationCount(size_t count, const std::string& severity) {
    if (count == 0) {
        return "no violations";
    } else if (count == 1) {
        return "1 " + severity + " violation";
    }
    return std::to_string(count) + " " + severity + " violations";
}

bool isJsonOutput(const std::string& format) {
    return format == "json" || format == "JSON";
}

bool isYamlOutput(const std::string& format) {
    return format == "yaml" || format == "YAML" || format == "yml";
}

bool supportsColors() {
    // Simple check for color support
    const char* term = std::getenv("TERM");
    if (!term) return false;
    
    std::string term_str(term);
    return term_str.find("color") != std::string::npos ||
           term_str.find("xterm") != std::string::npos ||
           term_str.find("screen") != std::string::npos;
}

} // namespace utils

} // namespace akao::interfaces::cli::executor
