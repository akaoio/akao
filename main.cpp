#include "interfaces/cli/parser/command_parser.hpp"
#include "interfaces/cli/executor/command_executor.hpp"
#include "interfaces/cli/formatter/output_formatter.hpp"
#include "interfaces/cli/command/validate/v1.hpp"
#include "interfaces/cli/command/init/v1.hpp"
#include "interfaces/cli/command/generate/v1.hpp"
#include "interfaces/cli/command/build/v1.hpp"
#include "interfaces/cli/command/test/v1.hpp"
#include "interfaces/cli/command/additional/v1.hpp"
#include <iostream>
#include <filesystem>

/**
 * @brief Main entry point for Akao CLI application
 * 
 * Universal validation and enforcement framework that embodies
 * the "rules govern rules" philosophy - Akao validates itself
 * using its own rules and philosophies.
 * 
 * Philosophy compliance:
 * - akao:philosophy:rule:governance:v1 - Self-governing rules system
 * - akao:philosophy:interface:consistency:v1 - Consistent CLI interface
 * - akao:philosophy:automation:updates:v1 - Automated validation and enforcement
 * - akao:philosophy:measurement:observability:v1 - Observable execution metrics
 */

void printUsage(const std::string& program_name) {
    std::cout << "Akao - Universal Validation and Enforcement Framework\n";
    std::cout << "Usage: " << program_name << " [global-options] <command> [command-options] [args...]\n\n";
    
    std::cout << "Global Options:\n";
    std::cout << "  --config <path>     Configuration file path\n";
    std::cout << "  --format <format>   Output format (yaml, json, text, table)\n";
    std::cout << "  --verbose, -v       Verbose output\n";
    std::cout << "  --quiet, -q         Quiet mode (minimal output)\n";
    std::cout << "  --debug             Debug mode with detailed logging\n";
    std::cout << "  --help, -h          Show this help message\n";
    std::cout << "  --version           Show version information\n\n";
    
    std::cout << "Commands:\n";
    std::cout << "  validate <path>     Universal validation - works on ANY project\n";
    std::cout << "  init [path]         Initialize new Akao project\n";
    std::cout << "  generate <type>     Generate universes (projects, frameworks, languages)\n";
    std::cout << "  build [options]     Build with duality support (development/production)\n";
    std::cout << "  test [scope]        Run compliance tests with coverage\n";
    std::cout << "  docs [action]       Documentation generation and serving\n";
    std::cout << "  metrics [action]    Metrics operations and observability\n";
    std::cout << "  security [action]   Security operations and enforcement\n";
    std::cout << "  rules [action]      Rule management operations\n";
    std::cout << "  config [action]     Configuration management\n\n";
    
    std::cout << "Examples:\n";
    std::cout << "  " << program_name << " validate ./my-project\n";
    std::cout << "  " << program_name << " init --type cpp ./new-project\n";
    std::cout << "  " << program_name << " generate project --lang rust\n";
    std::cout << "  " << program_name << " build --mode production\n";
    std::cout << "  " << program_name << " test --coverage\n";
    std::cout << "  " << program_name << " validate --self\n\n";
    
    std::cout << "For detailed help on a specific command, use:\n";
    std::cout << "  " << program_name << " <command> --help\n\n";
    
    std::cout << "Akao embodies 'rules govern rules' - it validates itself using its own philosophies.\n";
}

void printVersion() {
    std::cout << "Akao Version 1.0.0\n";
    std::cout << "Universal Validation and Enforcement Framework\n";
    std::cout << "Built with C++ Standard Library and native YAML parsing\n";
    std::cout << "Copyright (c) 2024 Akao Project\n";
    
    // Count actual loaded rules and philosophies
    int philosophy_count = 0;
    int rule_count = 0;
    
    try {
        // Count philosophy files
        if (std::filesystem::exists("philosophies")) {
            for (const auto& entry : std::filesystem::recursive_directory_iterator("philosophies")) {
                if (entry.is_regular_file() && entry.path().extension() == ".yaml") {
                    philosophy_count++;
                }
            }
        }
        
        // Count rule files (excluding index files)
        if (std::filesystem::exists("rules")) {
            for (const auto& entry : std::filesystem::recursive_directory_iterator("rules")) {
                if (entry.is_regular_file() && entry.path().extension() == ".yaml") {
                    std::string filename = entry.path().filename().string();
                    if (filename != "index.yaml") {
                        rule_count++;
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        // Fallback to directory existence check if counting fails
        philosophy_count = std::filesystem::exists("philosophies") ? -1 : 0;
        rule_count = std::filesystem::exists("rules") ? -1 : 0;
    }
    
    std::cout << "\nPhilosophies: " << philosophy_count << " loaded\n";
    std::cout << "Rules: " << rule_count << " loaded\n";
}

int main(int argc, char* argv[]) {
    try {
        // Early help/version checks - only for global help
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            if ((arg == "--help" || arg == "-h") && i == 1) {
                // Only show global help if --help is the first argument
                printUsage(argv[0]);
                return 0;
            } else if (arg == "--version" && i == 1) {
                // Only show version if --version is the first argument
                printVersion();
                return 0;
            }
        }
        
        // Convert arguments to vector
        std::vector<std::string> args;
        for (int i = 1; i < argc; ++i) {
            args.emplace_back(argv[i]);
        }
        
        // Parse command line arguments
        akao::interfaces::cli::parser::CommandParser parser;
        
        // Create command instances
        auto validate_cmd = std::make_shared<akao::interfaces::cli::commands::ValidateCommand>();
        auto init_cmd = std::make_shared<akao::interfaces::cli::commands::InitCommand>();
        auto generate_cmd = std::make_shared<akao::interfaces::cli::commands::GenerateCommand>();
        auto build_cmd = std::make_shared<akao::interfaces::cli::commands::BuildCommand>();
        auto test_cmd = std::make_shared<akao::interfaces::cli::commands::TestCommand>();
        auto docs_cmd = std::make_shared<akao::interfaces::cli::commands::DocsCommand>();
        auto metrics_cmd = std::make_shared<akao::interfaces::cli::commands::MetricsCommand>();
        auto security_cmd = std::make_shared<akao::interfaces::cli::commands::SecurityCommand>();
        auto rules_cmd = std::make_shared<akao::interfaces::cli::commands::RulesCommand>();
        auto config_cmd = std::make_shared<akao::interfaces::cli::commands::ConfigCommand>();

        // Register commands manually since we now have individual command classes
        akao::interfaces::cli::parser::Command validate_command;
        validate_command.name = "validate";
        validate_command.description = "Universal validation engine - works on ANY project";
        validate_command.usage = validate_cmd->getUsage();
        validate_command.options = validate_cmd->getSupportedOptions();
        validate_command.flags = validate_cmd->getSupportedFlags();
        parser.registerCommand(validate_command);

        akao::interfaces::cli::parser::Command init_command;
        init_command.name = "init";
        init_command.description = "Initialize new Akao project";
        init_command.usage = init_cmd->getUsage();
        init_command.options = init_cmd->getSupportedOptions();
        init_command.flags = init_cmd->getSupportedFlags();
        parser.registerCommand(init_command);

        akao::interfaces::cli::parser::Command generate_command;
        generate_command.name = "generate";
        generate_command.description = "Generate universes - projects, frameworks, languages";
        generate_command.usage = generate_cmd->getUsage();
        generate_command.options = generate_cmd->getSupportedOptions();
        generate_command.flags = generate_cmd->getSupportedFlags();
        parser.registerCommand(generate_command);

        akao::interfaces::cli::parser::Command build_command;
        build_command.name = "build";
        build_command.description = "Build with duality support";
        build_command.usage = build_cmd->getUsage();
        build_command.options = build_cmd->getSupportedOptions();
        build_command.flags = build_cmd->getSupportedFlags();
        parser.registerCommand(build_command);

        akao::interfaces::cli::parser::Command test_command;
        test_command.name = "test";
        test_command.description = "Run compliance tests";
        test_command.usage = test_cmd->getUsage();
        test_command.options = test_cmd->getSupportedOptions();
        test_command.flags = test_cmd->getSupportedFlags();
        parser.registerCommand(test_command);

        akao::interfaces::cli::parser::Command docs_command;
        docs_command.name = "docs";
        docs_command.description = "Documentation commands";
        docs_command.usage = docs_cmd->getUsage();
        docs_command.options = docs_cmd->getSupportedOptions();
        docs_command.flags = docs_cmd->getSupportedFlags();
        parser.registerCommand(docs_command);

        akao::interfaces::cli::parser::Command metrics_command;
        metrics_command.name = "metrics";
        metrics_command.description = "Metrics operations";
        metrics_command.usage = metrics_cmd->getUsage();
        metrics_command.options = metrics_cmd->getSupportedOptions();
        metrics_command.flags = metrics_cmd->getSupportedFlags();
        parser.registerCommand(metrics_command);

        akao::interfaces::cli::parser::Command security_command;
        security_command.name = "security";
        security_command.description = "Security operations";
        security_command.usage = security_cmd->getUsage();
        security_command.options = security_cmd->getSupportedOptions();
        security_command.flags = security_cmd->getSupportedFlags();
        parser.registerCommand(security_command);

        akao::interfaces::cli::parser::Command rules_command;
        rules_command.name = "rules";
        rules_command.description = "Rule management";
        rules_command.usage = rules_cmd->getUsage();
        rules_command.options = rules_cmd->getSupportedOptions();
        rules_command.flags = rules_cmd->getSupportedFlags();
        parser.registerCommand(rules_command);

        akao::interfaces::cli::parser::Command config_command;
        config_command.name = "config";
        config_command.description = "Configuration management";
        config_command.usage = config_cmd->getUsage();
        config_command.options = config_cmd->getSupportedOptions();
        config_command.flags = config_cmd->getSupportedFlags();
        parser.registerCommand(config_command);
        
        auto parse_result = parser.parse(args);
        
        if (!parse_result.success) {
            std::cerr << "Error: " << parse_result.error_message << std::endl;
            std::cerr << "Use --help for usage information." << std::endl;
            return 1;
        }
        
        // Setup execution context
        akao::interfaces::cli::executor::ExecutionContext context;
        context.current_directory = std::filesystem::current_path().string();
        
        // Apply global configuration from parse result
        auto format_it = parse_result.options.find("format");
        if (format_it != parse_result.options.end()) {
            context.output_format = format_it->second;
        }
        
        auto config_it = parse_result.options.find("config");
        if (config_it != parse_result.options.end()) {
            context.config_file_path = config_it->second;
        }
        
        auto verbose_it = parse_result.flags.find("verbose");
        if (verbose_it != parse_result.flags.end()) {
            context.verbose_mode = verbose_it->second;
        }
        
        auto quiet_it = parse_result.flags.find("quiet");
        if (quiet_it != parse_result.flags.end()) {
            context.quiet_mode = quiet_it->second;
        }
        
        auto debug_it = parse_result.flags.find("debug");
        if (debug_it != parse_result.flags.end()) {
            context.debug_mode = debug_it->second;
        }
        
        // Initialize command executor
        akao::interfaces::cli::executor::CommandExecutor executor;
        executor.setContext(context);
        
        if (!executor.initialize()) {
            std::cerr << "Error: Failed to initialize Akao executor" << std::endl;
            return 1;
        }
        
        // Check for command-specific help flag or option
        auto help_flag = parse_result.flags.find("help");
        auto help_option = parse_result.options.find("help");
        
        if ((help_flag != parse_result.flags.end() && help_flag->second) ||
            (help_option != parse_result.options.end())) {
            // Show command-specific help
            std::cout << parser.getCommandHelp(parse_result.command_name) << std::endl;
            return 0;
        }
        
        // Execute command using the appropriate command instance
        akao::interfaces::cli::executor::ExecutionResult execution_result;
        
        if (parse_result.command_name == "validate") {
            execution_result = validate_cmd->execute(context, parse_result.positional_arguments);
        } else if (parse_result.command_name == "init") {
            execution_result = init_cmd->execute(context, parse_result.positional_arguments);
        } else if (parse_result.command_name == "generate") {
            execution_result = generate_cmd->execute(context, parse_result.positional_arguments);
        } else if (parse_result.command_name == "build") {
            execution_result = build_cmd->execute(context, parse_result.positional_arguments);
        } else if (parse_result.command_name == "test") {
            execution_result = test_cmd->execute(context, parse_result.positional_arguments);
        } else if (parse_result.command_name == "docs") {
            execution_result = docs_cmd->execute(context, parse_result.positional_arguments);
        } else if (parse_result.command_name == "metrics") {
            execution_result = metrics_cmd->execute(context, parse_result.positional_arguments);
        } else if (parse_result.command_name == "security") {
            execution_result = security_cmd->execute(context, parse_result.positional_arguments);
        } else if (parse_result.command_name == "rules") {
            execution_result = rules_cmd->execute(context, parse_result.positional_arguments);
        } else if (parse_result.command_name == "config") {
            execution_result = config_cmd->execute(context, parse_result.positional_arguments);
        } else {
            execution_result.success = false;
            execution_result.exit_code = 1;
            execution_result.error_message = "Unknown command: " + parse_result.command_name;
        }
        
        // Setup output formatter
        akao::interfaces::cli::formatter::FormatOptions format_options;
        format_options.format = context.output_format;
        format_options.colored_output = !context.quiet_mode;
        format_options.include_metadata = context.verbose_mode;
        format_options.compact_mode = context.quiet_mode;
        format_options.show_progress = context.verbose_mode;
        
        akao::interfaces::cli::formatter::OutputFormatter formatter(format_options);
        
        // Format and display results unless quiet mode
        if (!context.quiet_mode || !execution_result.success) {
            std::string formatted_output = formatter.formatExecutionResult(execution_result);
            
            if (execution_result.success) {
                std::cout << formatted_output << std::endl;
            } else {
                std::cerr << formatted_output << std::endl;
            }
        }
        
        // Display execution statistics in verbose mode
        if (context.verbose_mode) {
            auto stats = executor.getStats();
            std::string stats_output = formatter.formatExecutionStats(stats);
            std::cout << "\n" << stats_output << std::endl;
        }
        
        return execution_result.exit_code;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred" << std::endl;
        return 1;
    }
}
