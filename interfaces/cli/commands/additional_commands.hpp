#pragma once

#include "../executor/command_executor.hpp"
#include <string>
#include <vector>
#include <map>

namespace akao::interfaces::cli::commands {

// Documentation Generation Command
class DocsCommand {
public:
    DocsCommand();
    executor::ExecutionResult execute(const executor::ExecutionContext& context, const std::vector<std::string>& args);
    std::string getHelp() const;
    std::string getUsage() const;
    std::map<std::string, std::string> getSupportedOptions() const;
    std::map<std::string, std::string> getSupportedFlags() const;
};

// Metrics and Observability Command
class MetricsCommand {
public:
    MetricsCommand();
    executor::ExecutionResult execute(const executor::ExecutionContext& context, const std::vector<std::string>& args);
    std::string getHelp() const;
    std::string getUsage() const;
    std::map<std::string, std::string> getSupportedOptions() const;
    std::map<std::string, std::string> getSupportedFlags() const;
};

// Security Operations Command
class SecurityCommand {
public:
    SecurityCommand();
    executor::ExecutionResult execute(const executor::ExecutionContext& context, const std::vector<std::string>& args);
    std::string getHelp() const;
    std::string getUsage() const;
    std::map<std::string, std::string> getSupportedOptions() const;
    std::map<std::string, std::string> getSupportedFlags() const;
};

// Rules Management Command
class RulesCommand {
public:
    RulesCommand();
    executor::ExecutionResult execute(const executor::ExecutionContext& context, const std::vector<std::string>& args);
    std::string getHelp() const;
    std::string getUsage() const;
    std::map<std::string, std::string> getSupportedOptions() const;
    std::map<std::string, std::string> getSupportedFlags() const;
};

// Configuration Management Command
class ConfigCommand {
public:
    ConfigCommand();
    executor::ExecutionResult execute(const executor::ExecutionContext& context, const std::vector<std::string>& args);
    std::string getHelp() const;
    std::string getUsage() const;
    std::map<std::string, std::string> getSupportedOptions() const;
    std::map<std::string, std::string> getSupportedFlags() const;
};

} // namespace akao::interfaces::cli::commands
