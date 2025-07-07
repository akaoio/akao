#pragma once

#include "../../executor/command_executor.hpp"
#include <string>
#include <vector>
#include <map>

namespace akao::interfaces::cli::commands {

class TestCommand {
public:
    TestCommand();
    ~TestCommand() = default;

    executor::ExecutionResult execute(const executor::ExecutionContext& context,
                                    const std::vector<std::string>& args);

    std::string getHelp() const;
    std::string getUsage() const;
    std::map<std::string, std::string> getSupportedOptions() const;
    std::map<std::string, std::string> getSupportedFlags() const;

private:
    struct TestOptions {
        std::string scope = "all";        // all, unit, integration, compliance
        bool show_coverage = false;
        std::vector<std::string> rule_categories;
        std::vector<std::string> philosophies;
        bool generate_missing = false;
    };

    TestOptions parseArguments(const std::vector<std::string>& args);
    executor::ExecutionResult runAllTests(const TestOptions& options, const executor::ExecutionContext& context);
    executor::ExecutionResult runUnitTests(const TestOptions& options, const executor::ExecutionContext& context);
    executor::ExecutionResult runIntegrationTests(const TestOptions& options, const executor::ExecutionContext& context);
    executor::ExecutionResult runComplianceTests(const TestOptions& options, const executor::ExecutionContext& context);
};

} // namespace akao::interfaces::cli::commands
