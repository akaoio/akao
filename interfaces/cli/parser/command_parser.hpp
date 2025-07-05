#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>

namespace akao::interfaces::cli::parser {

/**
 * @brief Command line argument
 */
struct Argument {
    std::string name;
    std::string value;
    bool is_flag = false;  // true for --flag, false for --option=value
    bool is_positional = false;  // true for positional arguments
};

/**
 * @brief Command definition
 */
struct Command {
    std::string name;
    std::string description;
    std::string usage;
    std::vector<std::string> aliases;
    
    // Options and flags
    std::map<std::string, std::string> options;  // option_name -> description
    std::map<std::string, std::string> flags;    // flag_name -> description
    std::vector<std::string> positional_args;   // positional argument names
    
    // Validation
    std::vector<std::string> required_options;
    std::map<std::string, std::vector<std::string>> option_choices;  // option -> valid values
    
    // Handler
    std::function<int(const std::map<std::string, std::string>&)> handler;
};

/**
 * @brief Parsed command result
 */
struct ParseResult {
    bool success = false;
    std::string error_message;
    
    std::string command_name;
    std::map<std::string, std::string> options;
    std::map<std::string, bool> flags;
    std::vector<std::string> positional_arguments;
    
    // Original command line
    std::vector<std::string> raw_args;
};

/**
 * @brief Universal command line parser for Akao framework
 * 
 * Implements comprehensive CLI parsing that supports all Akao commands
 * with consistent interface design and error handling.
 * 
 * Philosophy compliance:
 * - akao:philosophy:interface:consistency:v1 - Consistent CLI interface
 * - akao:philosophy:documentation:generation:v1 - Auto-generated help
 * - akao:philosophy:rule:governance:v1 - Command validation rules
 */
class CommandParser {
public:
    /**
     * @brief Constructor
     */
    CommandParser();

    /**
     * @brief Register a command
     */
    void registerCommand(const Command& command);

    /**
     * @brief Parse command line arguments
     * @param argc Argument count
     * @param argv Argument vector
     * @return Parse result
     */
    ParseResult parse(int argc, char* argv[]);

    /**
     * @brief Parse command line arguments
     * @param args Vector of argument strings
     * @return Parse result
     */
    ParseResult parse(const std::vector<std::string>& args);

    /**
     * @brief Get help text for all commands
     */
    std::string getGlobalHelp() const;

    /**
     * @brief Get help text for specific command
     */
    std::string getCommandHelp(const std::string& command_name) const;

    /**
     * @brief Check if command exists
     */
    bool hasCommand(const std::string& command_name) const;

    /**
     * @brief Get all registered commands
     */
    std::vector<std::string> getCommandNames() const;

    /**
     * @brief Set global options (available to all commands)
     */
    void setGlobalOptions(const std::map<std::string, std::string>& options);

    /**
     * @brief Set global flags (available to all commands)
     */
    void setGlobalFlags(const std::map<std::string, std::string>& flags);

    /**
     * @brief Enable auto-completion support
     */
    void enableAutoCompletion(bool enabled);

    /**
     * @brief Generate auto-completion suggestions
     */
    std::vector<std::string> getCompletionSuggestions(const std::vector<std::string>& partial_args) const;

private:
    std::map<std::string, Command> commands_;
    std::map<std::string, std::string> command_aliases_;  // alias -> command_name
    std::map<std::string, std::string> global_options_;
    std::map<std::string, std::string> global_flags_;
    bool auto_completion_enabled_ = false;

    // Parsing helpers
    ParseResult parseCommand(const std::vector<std::string>& args);
    std::string findCommand(const std::string& name) const;
    bool isOption(const std::string& arg) const;
    bool isFlag(const std::string& arg) const;
    std::pair<std::string, std::string> parseOption(const std::string& arg) const;
    std::string parseFlag(const std::string& arg) const;
    
    // Validation
    bool validateCommand(const Command& command, const ParseResult& result, std::string& error) const;
    bool validateRequiredOptions(const Command& command, const ParseResult& result, std::string& error) const;
    bool validateOptionChoices(const Command& command, const ParseResult& result, std::string& error) const;
    
    // Help generation
    std::string formatCommandUsage(const Command& command) const;
    std::string formatOptionsList(const std::map<std::string, std::string>& options) const;
    std::string formatFlagsList(const std::map<std::string, std::string>& flags) const;
};

/**
 * @brief Default Akao commands
 */
namespace akao_commands {

/**
 * @brief Get all default Akao CLI commands
 */
std::vector<Command> getDefaultCommands();

/**
 * @brief Individual command definitions
 */
Command getValidateCommand();
Command getInitCommand();
Command getGenerateCommand();
Command getCheckCommand();
Command getTraceCommand();
Command getReportCommand();
Command getFixCommand();
Command getConfigCommand();
Command getVersionCommand();
Command getHelpCommand();
Command getSelfValidateCommand();
Command getStatusCommand();

} // namespace akao_commands

} // namespace akao::interfaces::cli::parser
