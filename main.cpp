/**
 * @id: akao:file:interface:cli:main:v1
 * @doc: Main entry point for Akao CLI application providing unified validation system with .akao/rules lazy loading, Pure Logic runtime support, and comprehensive rule management
 * @specification: CLI application entry point with unified validation system and .akao/rules workflow
 * @scope: Primary interface for Akao system operations including validation, rule management, and Pure Logic execution
 * @timeline: 2025-07-09
 * @rationale: Provide unified CLI interface with .akao/rules lazy loading and consolidated validation system
 * @methodology: CLI interface with unified validation system and rule management
 * @references: ["akao:file:core:engine:validator:unified:v1", "akao:file:core:engine:logic:core:v1"]
 */

#include "core/engine/logic/core/v1.hpp"
#include "core/engine/validator/v1.hpp"
#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>

void printUsage(const std::string& program_name) {
    std::cout << "Akao - Universal Validation and Enforcement Framework\n";
    std::cout << "Usage: " << program_name << " [options] <command> [args...]\n\n";
    
    std::cout << "Options:\n";
    std::cout << "  --help, -h          Show this help message\n";
    std::cout << "  --version           Show version information\n";
    std::cout << "  --verbose, -v       Verbose output\n";
    std::cout << "  --parallel          Enable parallel rule execution\n";
    std::cout << "  --format <format>   Output format: console, json, structured\n\n";
    
    std::cout << "Commands:\n";
    std::cout << "  validate <path>     Universal validation with .akao/rules lazy loading\n";
    std::cout << "  rules list          List available rules\n";
    std::cout << "  rules enable <id>   Enable a rule\n";
    std::cout << "  rules disable <id>  Disable a rule\n";
    std::cout << "  rules status        Show rule status\n";
    std::cout << "  run <file.a>        Execute Pure Logic .a file\n";
    std::cout << "  init                Initialize .akao/rules directory\n";
    std::cout << "  stats               Show validation statistics\n\n";
    
    std::cout << "Examples:\n";
    std::cout << "  " << program_name << " validate ./my-project\n";
    std::cout << "  " << program_name << " validate --parallel --format json .\n";
    std::cout << "  " << program_name << " rules list --category structure\n";
    std::cout << "  " << program_name << " rules enable structure:one_class_per_file\n";
    std::cout << "  " << program_name << " run validation.a\n";
    std::cout << "  " << program_name << " init\n\n";
}

void printVersion() {
    std::cout << "Akao Version 2.0.0\n";
    std::cout << "Universal Validation and Enforcement Framework\n";
    std::cout << "Features: Unified Validation System, .akao/rules Lazy Loading, Pure Logic Engine\n";
    std::cout << "Built with C++ Standard Library and Advanced Mathematical Validation\n";
    std::cout << "Copyright (c) 2024-2025 Akao Project\n";
}

int runValidation(const std::string& path, bool verbose, bool parallel, const std::string& format) {
    try {
        if (verbose) {
            std::cout << "🔍 Initializing Unified Validation System..." << std::endl;
        }
        
        // Configure validation options
        akao::core::engine::validator::UnifiedValidator::ValidationConfig config;
        config.enable_universal_validation = true;
        config.enable_compliance_checking = true;
        config.enable_metadata_validation = true;
        config.parallel_execution = parallel;
        config.rules_directory = ".akao/rules";
        
        // Create unified validation system (zero hardcoded logic)
        auto validator = akao::core::engine::validator::UnifiedValidatorFactory::createWithConfig(".", config);
        
        if (verbose) {
            std::cout << "📋 Loading rules for target: " << path << std::endl;
        }
        
        // Run validation
        auto result = validator->validate(path);
        
        // Output results
        if (format == "json") {
            std::cout << "{\n";
            std::cout << "  \"path\": \"" << result.target_path << "\",\n";
            std::cout << "  \"valid\": " << (result.isCompliant() ? "true" : "false") << ",\n";
            std::cout << "  \"violations\": " << result.getViolationCount() << ",\n";
            std::cout << "  \"rules_executed\": " << result.total_rules_executed << ",\n";
            std::cout << "  \"files_analyzed\": " << result.total_files_analyzed << ",\n";
            std::cout << "  \"duration_seconds\": " << result.getExecutionDuration().count() << "\n";
            std::cout << "}\n";
        } else if (format == "structured") {
            std::cout << "VALIDATION_RESULT\n";
            std::cout << "path=" << result.target_path << "\n";
            std::cout << "valid=" << (result.isCompliant() ? "true" : "false") << "\n";
            std::cout << "violations=" << result.getViolationCount() << "\n";
            std::cout << "rules_executed=" << result.total_rules_executed << "\n";
            std::cout << "files_analyzed=" << result.total_files_analyzed << "\n";
            std::cout << "duration_seconds=" << result.getExecutionDuration().count() << "\n";
        } else {
            // Console format
            std::cout << "\n📊 Validation Results:\n";
            std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
            std::cout << "Path: " << result.target_path << "\n";
            std::cout << "Status: " << (result.isCompliant() ? "✅ PASSED" : "❌ FAILED") << "\n";
            std::cout << "Violations: " << result.getViolationCount() << "\n";
            std::cout << "Rules Executed: " << result.total_rules_executed << "\n";
            std::cout << "Files Analyzed: " << result.total_files_analyzed << "\n";
            std::cout << "Duration: " << std::fixed << std::setprecision(3) << result.getExecutionDuration().count() << "s\n";
            std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
            
            // Show violations if any
            if (!result.violations.empty()) {
                std::cout << "\n🔍 Violations Found:\n";
                for (const auto& violation : result.violations) {
                    std::cout << "  • " << violation.rule_id << " (" << violation.severity << ")\n";
                    std::cout << "    " << violation.message << "\n";
                    if (!violation.file_path.empty()) {
                        std::cout << "    File: " << violation.file_path;
                        if (violation.line_number > 0) {
                            std::cout << ":" << violation.line_number;
                        }
                        std::cout << "\n";
                    }
                    if (!violation.suggestion.empty()) {
                        std::cout << "    Suggestion: " << violation.suggestion << "\n";
                    }
                    std::cout << "\n";
                }
            }
        }
        
        return result.isCompliant() ? 0 : 1;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Validation error: " << e.what() << std::endl;
        return 1;
    }
}

int runRulesCommand(const std::vector<std::string>& args, bool verbose) {
    try {
        if (args.empty()) {
            std::cerr << "❌ Rules command requires subcommand: list, enable, disable, status" << std::endl;
            return 1;
        }
        
        auto validator = akao::core::engine::validator::UnifiedValidatorFactory::create(".");
        
        if (args[0] == "list") {
            auto rules = validator->getAvailableRules();
            
            std::cout << "📋 Available Rules:\n";
            std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
            
            std::map<std::string, std::vector<akao::core::engine::validator::RuleConfig>> by_category;
            for (const auto& rule : rules) {
                by_category[rule.category].push_back(rule);
            }
            
            for (const auto& [category, category_rules] : by_category) {
                std::cout << "\n📁 " << category << ":\n";
                for (const auto& rule : category_rules) {
                    std::cout << "  " << (rule.enabled ? "✅" : "❌") << " " << rule.rule_id << "\n";
                    std::cout << "      " << rule.description << "\n";
                    std::cout << "      Format: " << rule.format << ", Severity: " << rule.severity << "\n";
                    if (verbose) {
                        std::cout << "      File: " << rule.file_path << "\n";
                    }
                }
            }
            
        } else if (args[0] == "enable" && args.size() > 1) {
            if (validator->enableRule(args[1])) {
                std::cout << "✅ Rule enabled: " << args[1] << std::endl;
            } else {
                std::cout << "❌ Failed to enable rule: " << args[1] << std::endl;
                return 1;
            }
            
        } else if (args[0] == "disable" && args.size() > 1) {
            if (validator->disableRule(args[1])) {
                std::cout << "✅ Rule disabled: " << args[1] << std::endl;
            } else {
                std::cout << "❌ Failed to disable rule: " << args[1] << std::endl;
                return 1;
            }
            
        } else if (args[0] == "status") {
            auto enabled_rules = validator->getEnabledRules();
            auto all_rules = validator->getAvailableRules();
            
            std::cout << "📊 Rule Status:\n";
            std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
            std::cout << "Total Rules: " << all_rules.size() << "\n";
            std::cout << "Enabled Rules: " << enabled_rules.size() << "\n";
            std::cout << "Disabled Rules: " << (all_rules.size() - enabled_rules.size()) << "\n";
            
        } else {
            std::cerr << "❌ Unknown rules subcommand: " << args[0] << std::endl;
            return 1;
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Rules command error: " << e.what() << std::endl;
        return 1;
    }
}

int runPureLogicFile(const std::string& filename, bool verbose) {
    try {
        if (verbose) {
            std::cout << "🚀 Executing Pure Logic file: " << filename << std::endl;
        }
        
        if (!std::filesystem::exists(filename)) {
            std::cerr << "❌ File not found: " << filename << std::endl;
            return 1;
        }
        
        // Read file content
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "❌ Cannot open file: " << filename << std::endl;
            return 1;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        
        // Execute with Pure Logic Engine
        akao::logic::PureLogicEngine engine;
        engine.initialize();  // Initialize engine with builtin functions
        akao::logic::Context ctx;
        
        auto result = engine.executeAkaoFormat(content, ctx);
        
        if (verbose) {
            std::cout << "📊 Execution result: " << result.toString() << std::endl;
        } else {
            std::cout << result.toString() << std::endl;
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Pure Logic execution error: " << e.what() << std::endl;
        return 1;
    }
}

int initializeRulesDirectory(bool verbose) {
    try {
        if (verbose) {
            std::cout << "🔧 Initializing .akao/rules directory..." << std::endl;
        }
        
        if (akao::core::engine::validator::UnifiedValidatorFactory::initializeRulesDirectory(".")) {
            std::cout << "✅ .akao/rules directory initialized successfully" << std::endl;
            std::cout << "📁 Created directory structure:" << std::endl;
            std::cout << "  .akao/rules/enabled/    - Enabled rules" << std::endl;
            std::cout << "  .akao/rules/disabled/   - Disabled rules" << std::endl;
            std::cout << "  .akao/rules/examples/   - Example rules" << std::endl;
            std::cout << "  .akao/rules/config.yaml - Rules configuration" << std::endl;
            return 0;
        } else {
            std::cerr << "❌ Failed to initialize .akao/rules directory" << std::endl;
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Initialization error: " << e.what() << std::endl;
        return 1;
    }
}

int showStats(bool verbose) {
    try {
        auto validator = akao::core::engine::validator::UnifiedValidatorFactory::create(".");
        auto stats = validator->getStatistics();
        
        std::cout << "📊 Validation Statistics:\n";
        std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        std::cout << "Total Validations: " << stats.total_validations << "\n";
        std::cout << "Successful Validations: " << stats.successful_validations << "\n";
        std::cout << "Failed Validations: " << stats.failed_validations << "\n";
        std::cout << "Rules Executed: " << stats.rules_executed << "\n";
        std::cout << "Violations Found: " << stats.violations_found << "\n";
        std::cout << "Total Time: " << std::fixed << std::setprecision(3) << stats.total_execution_time.count() << "s\n";
        
        if (verbose && !stats.category_validation_count.empty()) {
            std::cout << "\n📁 Category Statistics:\n";
            for (const auto& [category, count] : stats.category_validation_count) {
                std::cout << "  " << category << ": " << count << " validations\n";
            }
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Stats error: " << e.what() << std::endl;
        return 1;
    }
}

int main(int argc, char* argv[]) {
    std::vector<std::string> args(argv, argv + argc);
    std::string program_name = args[0];
    
    // Parse options
    bool verbose = false;
    bool parallel = false;
    std::string format = "console";
    
    std::vector<std::string> command_args;
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printUsage(program_name);
            return 0;
        } else if (arg == "--version") {
            printVersion();
            return 0;
        } else if (arg == "--verbose" || arg == "-v") {
            verbose = true;
        } else if (arg == "--parallel") {
            parallel = true;
        } else if (arg == "--format" && i + 1 < argc) {
            format = argv[++i];
        } else {
            command_args.push_back(arg);
        }
    }
    
    if (command_args.empty()) {
        std::cerr << "❌ No command specified. Use --help for usage information." << std::endl;
        return 1;
    }
    
    std::string command = command_args[0];
    std::vector<std::string> command_params(command_args.begin() + 1, command_args.end());
    
    // Execute commands
    if (command == "validate") {
        std::string path = command_params.empty() ? "." : command_params[0];
        return runValidation(path, verbose, parallel, format);
        
    } else if (command == "rules") {
        return runRulesCommand(command_params, verbose);
        
    } else if (command == "run") {
        if (command_params.empty()) {
            std::cerr << "❌ No file specified for run command" << std::endl;
            return 1;
        }
        return runPureLogicFile(command_params[0], verbose);
        
    } else if (command == "init") {
        return initializeRulesDirectory(verbose);
        
    } else if (command == "stats") {
        return showStats(verbose);
        
    } else {
        std::cerr << "❌ Unknown command: " << command << std::endl;
        std::cerr << "Use --help for usage information." << std::endl;
        return 1;
    }
    
    return 0;
}