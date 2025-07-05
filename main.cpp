#include "interfaces/cli/parser/command_parser.hpp"
#include "interfaces/cli/executor/command_executor.hpp"
#include "interfaces/cli/formatter/output_formatter.hpp"
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
    std::cout << "  validate <path>     Validate project against Akao rules\n";
    std::cout << "  init [path]         Initialize new Akao project\n";
    std::cout << "  generate <type>     Generate project templates\n";
    std::cout << "  check <path>        Check project structure compliance\n";
    std::cout << "  trace <path>        Trace violation sources and dependencies\n";
    std::cout << "  report <path>       Generate comprehensive compliance report\n";
    std::cout << "  fix <path>          Automatically fix violations where possible\n";
    std::cout << "  config <action>     Manage configuration (get, set, list)\n";
    std::cout << "  self-validate       Validate Akao against its own rules\n";
    std::cout << "  status [path]       Show project and tool status\n\n";
    
    std::cout << "Examples:\n";
    std::cout << "  " << program_name << " validate ./my-project\n";
    std::cout << "  " << program_name << " init --type cpp ./new-project\n";
    std::cout << "  " << program_name << " report --output report.yaml ./project\n";
    std::cout << "  " << program_name << " fix --dry-run ./project\n";
    std::cout << "  " << program_name << " self-validate\n\n";
    
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
        // Early help/version checks
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            if (arg == "--help" || arg == "-h") {
                printUsage(argv[0]);
                return 0;
            } else if (arg == "--version") {
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
        
        // Register default Akao commands
        auto default_commands = akao::interfaces::cli::parser::akao_commands::getDefaultCommands();
        for (const auto& command : default_commands) {
            parser.registerCommand(command);
        }
        
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
        
        // Execute command
        auto execution_result = executor.execute(parse_result);
        
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
