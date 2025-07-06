#include "command_parser.hpp"
#include <algorithm>
#include <sstream>
#include <iostream>
#include <iomanip>

namespace akao::interfaces::cli::parser {

CommandParser::CommandParser() {
    // Register default global options
    global_options_["help"] = "Show help information";
    global_options_["version"] = "Show version information";
    global_options_["config"] = "Configuration file path";
    global_options_["output"] = "Output format (yaml, json, markdown, html)";
    global_options_["verbose"] = "Verbose output level (0-3)";
    
    global_flags_["quiet"] = "Suppress output";
    global_flags_["debug"] = "Enable debug output";
    global_flags_["force"] = "Force operation";
    global_flags_["dry-run"] = "Show what would be done without executing";
}

void CommandParser::registerCommand(const Command& command) {
    commands_[command.name] = command;
    
    // Register aliases
    for (const auto& alias : command.aliases) {
        command_aliases_[alias] = command.name;
    }
}

ParseResult CommandParser::parse(int argc, char* argv[]) {
    std::vector<std::string> args;
    
    // Convert to string vector (skip program name)
    for (int i = 1; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }
    
    return parse(args);
}

ParseResult CommandParser::parse(const std::vector<std::string>& args) {
    ParseResult result;
    result.raw_args = args;
    
    if (args.empty()) {
        result.error_message = "No command specified. Use 'akao help' for usage information.";
        return result;
    }
    
    // Handle global help
    if (args[0] == "help" || args[0] == "--help" || args[0] == "-h") {
        if (args.size() > 1) {
            // Help for specific command
            std::cout << getCommandHelp(args[1]) << std::endl;
        } else {
            // Global help
            std::cout << getGlobalHelp() << std::endl;
        }
        result.success = true;
        result.command_name = "help";
        return result;
    }
    
    // Handle global version
    if (args[0] == "version" || args[0] == "--version" || args[0] == "-v") {
        std::cout << "Akao Framework v1.0.0\n";
        std::cout << "Universal Language of Languages - Framework of Frameworks\n";
        result.success = true;
        result.command_name = "version";
        return result;
    }
    
    return parseCommand(args);
}

ParseResult CommandParser::parseCommand(const std::vector<std::string>& args) {
    ParseResult result;
    
    // Find command
    std::string command_name = findCommand(args[0]);
    if (command_name.empty()) {
        result.error_message = "Unknown command: " + args[0] + ". Use 'akao help' for available commands.";
        return result;
    }
    
    const auto& command = commands_[command_name];
    result.command_name = command_name;
    
    // Parse arguments
    size_t positional_index = 0;
    
    for (size_t i = 1; i < args.size(); ++i) {
        const auto& arg = args[i];
        
        if (isOption(arg)) {
            auto [name, value] = parseOption(arg);
            
            // Handle options without values
            if (value.empty() && i + 1 < args.size() && !isOption(args[i + 1]) && !isFlag(args[i + 1])) {
                value = args[++i];
            }
            
            result.options[name] = value;
            
        } else if (isFlag(arg)) {
            std::string flag_name = parseFlag(arg);
            result.flags[flag_name] = true;
            
        } else {
            // Positional argument
            result.positional_arguments.push_back(arg);
            positional_index++;
        }
    }
    
    // Validate command
    std::string validation_error;
    if (!validateCommand(command, result, validation_error)) {
        result.error_message = validation_error;
        return result;
    }
    
    result.success = true;
    return result;
}

std::string CommandParser::getGlobalHelp() const {
    std::stringstream ss;
    
    ss << "Akao Framework - Universal Language of Languages\n\n";
    ss << "USAGE:\n";
    ss << "    akao <command> [options] [arguments]\n\n";
    ss << "COMMANDS:\n";
    
    // List all commands
    for (const auto& [name, command] : commands_) {
        ss << "    " << std::left << std::setw(15) << name << command.description << "\n";
    }
    
    ss << "\nGLOBAL OPTIONS:\n";
    ss << formatOptionsList(global_options_);
    
    ss << "\nGLOBAL FLAGS:\n";
    ss << formatFlagsList(global_flags_);
    
    ss << "\nUse 'akao help <command>' for more information about a specific command.\n";
    
    return ss.str();
}

std::string CommandParser::getCommandHelp(const std::string& command_name) const {
    std::string actual_command = findCommand(command_name);
    if (actual_command.empty()) {
        return "Unknown command: " + command_name;
    }
    
    const auto& command = commands_.at(actual_command);
    
    std::stringstream ss;
    
    ss << "Command: " << command.name << "\n\n";
    ss << "DESCRIPTION:\n";
    ss << "    " << command.description << "\n\n";
    
    if (!command.usage.empty()) {
        ss << "USAGE:\n";
        ss << "    " << command.usage << "\n\n";
    } else {
        ss << "USAGE:\n";
        ss << "    " << formatCommandUsage(command) << "\n\n";
    }
    
    if (!command.aliases.empty()) {
        ss << "ALIASES:\n";
        ss << "    ";
        for (size_t i = 0; i < command.aliases.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << command.aliases[i];
        }
        ss << "\n\n";
    }
    
    if (!command.options.empty()) {
        ss << "OPTIONS:\n";
        ss << formatOptionsList(command.options);
        ss << "\n";
    }
    
    if (!command.flags.empty()) {
        ss << "FLAGS:\n";
        ss << formatFlagsList(command.flags);
        ss << "\n";
    }
    
    if (!command.positional_args.empty()) {
        ss << "ARGUMENTS:\n";
        for (const auto& arg : command.positional_args) {
            ss << "    " << arg << "\n";
        }
        ss << "\n";
    }
    
    return ss.str();
}

bool CommandParser::hasCommand(const std::string& command_name) const {
    return !findCommand(command_name).empty();
}

std::vector<std::string> CommandParser::getCommandNames() const {
    std::vector<std::string> names;
    for (const auto& [name, command] : commands_) {
        names.push_back(name);
    }
    return names;
}

void CommandParser::setGlobalOptions(const std::map<std::string, std::string>& options) {
    global_options_ = options;
}

void CommandParser::setGlobalFlags(const std::map<std::string, std::string>& flags) {
    global_flags_ = flags;
}

void CommandParser::enableAutoCompletion(bool enabled) {
    auto_completion_enabled_ = enabled;
}

std::vector<std::string> CommandParser::getCompletionSuggestions(const std::vector<std::string>& partial_args) const {
    std::vector<std::string> suggestions;
    
    if (partial_args.empty()) {
        // Suggest all commands
        for (const auto& [name, command] : commands_) {
            suggestions.push_back(name);
        }
    } else if (partial_args.size() == 1) {
        // Suggest commands that start with the partial input
        std::string partial = partial_args[0];
        for (const auto& [name, command] : commands_) {
            if (name.substr(0, partial.length()) == partial) {
                suggestions.push_back(name);
            }
        }
    }
    
    return suggestions;
}

// Private methods implementation
std::string CommandParser::findCommand(const std::string& name) const {
    // Check direct command name
    if (commands_.find(name) != commands_.end()) {
        return name;
    }
    
    // Check aliases
    auto alias_it = command_aliases_.find(name);
    if (alias_it != command_aliases_.end()) {
        return alias_it->second;
    }
    
    return "";
}

bool CommandParser::isOption(const std::string& arg) const {
    return arg.length() > 2 && arg.substr(0, 2) == "--";
}

bool CommandParser::isFlag(const std::string& arg) const {
    return arg.length() > 1 && arg[0] == '-' && arg[1] != '-';
}

std::pair<std::string, std::string> CommandParser::parseOption(const std::string& arg) const {
    std::string option_str = arg.substr(2);  // Remove --
    
    size_t equals_pos = option_str.find('=');
    if (equals_pos != std::string::npos) {
        return {option_str.substr(0, equals_pos), option_str.substr(equals_pos + 1)};
    }
    
    return {option_str, ""};
}

std::string CommandParser::parseFlag(const std::string& arg) const {
    return arg.substr(1);  // Remove -
}

bool CommandParser::validateCommand(const Command& command, const ParseResult& result, std::string& error) const {
    return validateRequiredOptions(command, result, error) &&
           validateOptionChoices(command, result, error);
}

bool CommandParser::validateRequiredOptions(const Command& command, const ParseResult& result, std::string& error) const {
    for (const auto& required_option : command.required_options) {
        if (result.options.find(required_option) == result.options.end()) {
            error = "Required option missing: --" + required_option;
            return false;
        }
    }
    return true;
}

bool CommandParser::validateOptionChoices(const Command& command, const ParseResult& result, std::string& error) const {
    for (const auto& [option_name, choices] : command.option_choices) {
        auto option_it = result.options.find(option_name);
        if (option_it != result.options.end()) {
            const std::string& value = option_it->second;
            if (std::find(choices.begin(), choices.end(), value) == choices.end()) {
                error = "Invalid value for --" + option_name + ": " + value;
                return false;
            }
        }
    }
    return true;
}

std::string CommandParser::formatCommandUsage(const Command& command) const {
    std::stringstream ss;
    ss << "akao " << command.name;
    
    if (!command.options.empty()) {
        ss << " [options]";
    }
    
    if (!command.flags.empty()) {
        ss << " [flags]";
    }
    
    for (const auto& arg : command.positional_args) {
        ss << " <" << arg << ">";
    }
    
    return ss.str();
}

std::string CommandParser::formatOptionsList(const std::map<std::string, std::string>& options) const {
    std::stringstream ss;
    
    for (const auto& [name, description] : options) {
        ss << "    --" << std::left << std::setw(20) << name << description << "\n";
    }
    
    return ss.str();
}

std::string CommandParser::formatFlagsList(const std::map<std::string, std::string>& flags) const {
    std::stringstream ss;
    
    for (const auto& [name, description] : flags) {
        ss << "    -" << std::left << std::setw(21) << name << description << "\n";
    }
    
    return ss.str();
}

// Default Akao commands implementation
namespace akao_commands {

std::vector<Command> getDefaultCommands() {
    return {
        getValidateCommand(),
        getInitCommand(),
        getGenerateCommand(),
        getCheckCommand(),
        getTraceCommand(),
        getReportCommand(),
        getFixCommand(),
        getConfigCommand(),
        getStatusCommand()
    };
}

Command getValidateCommand() {
    Command cmd;
    cmd.name = "validate";
    cmd.description = "Validate project against Akao rules and philosophies";
    cmd.usage = "akao validate [path] [options]";
    cmd.aliases = {"val", "check"};
    
    cmd.options = {
        {"rules", "Rules directory path"},
        {"output", "Output format (yaml, json, markdown, html)"},
        {"exclude", "Exclude patterns"},
        {"severity", "Minimum severity level (error, warning, info)"}
    };
    
    cmd.flags = {
        {"auto-fix", "Automatically fix violations when possible"},
        {"strict", "Enable strict validation mode"},
        {"recursive", "Validate subdirectories recursively"}
    };
    
    cmd.positional_args = {"path"};
    
    return cmd;
}

Command getInitCommand() {
    Command cmd;
    cmd.name = "init";
    cmd.description = "Initialize new project with Akao structure";
    cmd.usage = "akao init <project-type> [path] [options]";
    
    cmd.options = {
        {"template", "Project template to use"},
        {"name", "Project name"},
        {"description", "Project description"}
    };
    
    cmd.flags = {
        {"force", "Overwrite existing files"},
        {"minimal", "Create minimal structure only"}
    };
    
    cmd.positional_args = {"project-type", "path"};
    cmd.required_options = {};
    cmd.option_choices = {
        {"project-type", {"akao", "cpp", "python", "javascript", "rust", "go"}}
    };
    
    return cmd;
}

Command getGenerateCommand() {
    Command cmd;
    cmd.name = "generate";
    cmd.description = "Generate project components";
    cmd.usage = "akao generate <type> [options]";
    cmd.aliases = {"gen"};
    
    cmd.options = {
        {"output", "Output directory"},
        {"template", "Template to use"},
        {"config", "Configuration file"}
    };
    
    cmd.positional_args = {"type"};
    cmd.option_choices = {
        {"type", {"project", "rules", "docs", "tests", "config"}}
    };
    
    return cmd;
}

Command getCheckCommand() {
    Command cmd;
    cmd.name = "check";
    cmd.description = "Check project structure and compliance";
    cmd.usage = "akao check [component] [options]";
    
    cmd.options = {
        {"component", "Component to check (structure, rules, philosophies)"}
    };
    
    cmd.flags = {
        {"fix", "Fix issues found"},
        {"preview", "Preview changes without applying"}
    };
    
    return cmd;
}

Command getTraceCommand() {
    Command cmd;
    cmd.name = "trace";
    cmd.description = "Trace violation details and relationships";
    cmd.usage = "akao trace <violation-id> [options]";
    
    cmd.options = {
        {"format", "Output format"},
        {"depth", "Trace depth level"}
    };
    
    cmd.positional_args = {"violation-id"};
    
    return cmd;
}

Command getReportCommand() {
    Command cmd;
    cmd.name = "report";
    cmd.description = "Generate validation and compliance reports";
    cmd.usage = "akao report [options]";
    
    cmd.options = {
        {"input", "Input trace file or directory"},
        {"output", "Output file path"},
        {"format", "Report format (yaml, json, html, markdown)"},
        {"template", "Report template to use"}
    };
    
    cmd.flags = {
        {"summary", "Generate summary report only"},
        {"detailed", "Generate detailed report"},
        {"stats", "Include statistics"}
    };
    
    return cmd;
}

Command getFixCommand() {
    Command cmd;
    cmd.name = "fix";
    cmd.description = "Automatically fix violations";
    cmd.usage = "akao fix [path] [options]";
    
    cmd.options = {
        {"rules", "Specific rules to apply fixes for"},
        {"severity", "Minimum severity to fix"}
    };
    
    cmd.flags = {
        {"dry-run", "Show what would be fixed without applying"},
        {"backup", "Create backup before fixing"},
        {"interactive", "Ask before each fix"}
    };
    
    return cmd;
}

Command getConfigCommand() {
    Command cmd;
    cmd.name = "config";
    cmd.description = "Manage Akao configuration";
    cmd.usage = "akao config <action> [options]";
    
    cmd.options = {
        {"set", "Set configuration value"},
        {"get", "Get configuration value"},
        {"file", "Configuration file path"}
    };
    
    cmd.positional_args = {"action"};
    cmd.option_choices = {
        {"action", {"set", "get", "list", "reset"}}
    };
    
    return cmd;
}

Command getStatusCommand() {
    Command cmd;
    cmd.name = "status";
    cmd.description = "Show project status and compliance overview";
    cmd.usage = "akao status [path] [options]";
    
    cmd.options = {
        {"format", "Output format"}
    };
    
    cmd.flags = {
        {"summary", "Show summary only"},
        {"verbose", "Show detailed status"}
    };
    
    return cmd;
}

} // namespace akao_commands

} // namespace akao::interfaces::cli::parser
