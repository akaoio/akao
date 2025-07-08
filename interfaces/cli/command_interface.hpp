#pragma once

/**
 * @file command_interface.hpp
 * @brief Command interface for Akao Pure Logic runtime environment
 * @details Provides Node.js-like command interface for .a file execution,
 *          project management, build operations, and development workflow
 * 
 * ID: akao:component:interface:cli:command-interface:v1
 * Scope: Primary user interaction layer for Akao runtime
 * Dependencies: core/engine/language/symbol_parser.cpp, core/engine/runtime/project_manager.cpp
 */

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace akao {
namespace interfaces {
namespace cli {

/**
 * @brief Command execution result structure
 */
struct CommandResult {
    bool success = false;
    int exit_code = 0;
    std::string output;
    std::string error;
    double execution_time = 0.0;
};

/**
 * @brief Command argument structure
 */
struct CommandArgs {
    std::string command;
    std::vector<std::string> arguments;
    std::map<std::string, std::string> options;
    std::string working_directory;
};

/**
 * @brief Abstract base class for command implementations
 */
class Command {
public:
    virtual ~Command() = default;
    virtual CommandResult execute(const CommandArgs& args) = 0;
    virtual std::string get_help() const = 0;
    virtual std::string get_description() const = 0;
};

/**
 * @brief Execute .a files command implementation
 */
class ExecuteCommand : public Command {
public:
    CommandResult execute(const CommandArgs& args) override;
    std::string get_help() const override;
    std::string get_description() const override;
    
private:
    bool validate_file(const std::string& filepath);
    CommandResult execute_file(const std::string& filepath, const std::vector<std::string>& args);
};

/**
 * @brief Development server command implementation
 */
class StartCommand : public Command {
public:
    CommandResult execute(const CommandArgs& args) override;
    std::string get_help() const override;
    std::string get_description() const override;
    
private:
    void start_file_watcher(const std::string& directory);
    void handle_file_change(const std::string& filepath);
    bool should_reload_on_change(const std::string& filepath);
};

/**
 * @brief Script execution command implementation
 */
class RunCommand : public Command {
public:
    CommandResult execute(const CommandArgs& args) override;
    std::string get_help() const override;
    std::string get_description() const override;
    
private:
    std::string resolve_script(const std::string& script_name);
    CommandResult execute_script(const std::string& script_path, const std::vector<std::string>& args);
};

/**
 * @brief Build command implementation
 */
class BuildCommand : public Command {
public:
    CommandResult execute(const CommandArgs& args) override;
    std::string get_help() const override;
    std::string get_description() const override;
    
private:
    std::string determine_build_target();
    bool configure_build_environment();
    CommandResult compile_project(const std::string& target);
};

/**
 * @brief Test execution command implementation
 */
class TestCommand : public Command {
public:
    CommandResult execute(const CommandArgs& args) override;
    std::string get_help() const override;
    std::string get_description() const override;
    
private:
    std::vector<std::string> discover_test_files(const std::string& pattern);
    CommandResult run_test_file(const std::string& test_file);
    void generate_test_report(const std::vector<CommandResult>& results);
};

/**
 * @brief Validation command implementation
 */
class ValidateCommand : public Command {
public:
    CommandResult execute(const CommandArgs& args) override;
    std::string get_help() const override;
    std::string get_description() const override;
    
private:
    CommandResult run_compliance_validation();
    CommandResult run_syntax_validation();
    CommandResult run_semantic_validation();
};

/**
 * @brief Help command implementation
 */
class HelpCommand : public Command {
public:
    CommandResult execute(const CommandArgs& args) override;
    std::string get_help() const override;
    std::string get_description() const override;
    
    void set_command_registry(const std::map<std::string, std::unique_ptr<Command>>* registry);
    
private:
    const std::map<std::string, std::unique_ptr<Command>>* command_registry_ = nullptr;
};

/**
 * @brief Main command interface coordinator
 */
class CommandInterface {
public:
    CommandInterface();
    ~CommandInterface();
    
    /**
     * @brief Parse and execute command from command line arguments
     * @param argc Argument count
     * @param argv Argument vector
     * @return Command execution result
     */
    CommandResult execute(int argc, char* argv[]);
    
    /**
     * @brief Parse and execute command from string
     * @param command_line Command line string
     * @return Command execution result
     */
    CommandResult execute(const std::string& command_line);
    
    /**
     * @brief Register custom command
     * @param name Command name
     * @param command Command implementation
     */
    void register_command(const std::string& name, std::unique_ptr<Command> command);
    
    /**
     * @brief Get available commands
     * @return Map of command names to descriptions
     */
    std::map<std::string, std::string> get_available_commands() const;
    
    /**
     * @brief Set global options
     * @param options Global option map
     */
    void set_global_options(const std::map<std::string, std::string>& options);
    
private:
    std::map<std::string, std::unique_ptr<Command>> commands_;
    std::map<std::string, std::string> global_options_;
    
    void initialize_built_in_commands();
    CommandArgs parse_arguments(int argc, char* argv[]);
    CommandArgs parse_command_line(const std::string& command_line);
    void show_version_info();
    void show_general_help();
};

/**
 * @brief Utility functions for command interface
 */
namespace utils {
    /**
     * @brief Split command line into tokens
     * @param command_line Input command line
     * @return Vector of tokens
     */
    std::vector<std::string> split_command_line(const std::string& command_line);
    
    /**
     * @brief Parse command options from arguments
     * @param args Argument vector
     * @return Map of options
     */
    std::map<std::string, std::string> parse_options(const std::vector<std::string>& args);
    
    /**
     * @brief Format error message with context
     * @param message Error message
     * @param context Additional context
     * @return Formatted error string
     */
    std::string format_error(const std::string& message, const std::string& context = "");
    
    /**
     * @brief Validate .a file syntax
     * @param filepath Path to .a file
     * @return True if valid, false otherwise
     */
    bool validate_akao_file(const std::string& filepath);
    
    /**
     * @brief Get current working directory
     * @return Current directory path
     */
    std::string get_current_directory();
    
    /**
     * @brief Check if file exists and is readable
     * @param filepath File path to check
     * @return True if file exists and is readable
     */
    bool file_exists(const std::string& filepath);
}

} // namespace cli
} // namespace interfaces
} // namespace akao
