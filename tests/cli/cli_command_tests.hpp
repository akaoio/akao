#pragma once

#include "../../interfaces/cli/commands/validate_command.hpp"
#include "../../interfaces/cli/commands/init_command.hpp"
#include "../../interfaces/cli/commands/generate_command.hpp"
#include "../../interfaces/cli/commands/build_command.hpp"
#include "../../interfaces/cli/commands/test_command.hpp"
#include "../../interfaces/cli/commands/additional_commands.hpp"
#include <string>
#include <vector>
#include <memory>

namespace akao::tests::cli {

/**
 * @brief Comprehensive test suite for all CLI commands
 * 
 * Tests every command, option, and flag to ensure 100% coverage
 * and production-ready functionality.
 */
class CLICommandTests {
public:
    struct TestResult {
        std::string test_name;
        bool passed = false;
        std::string error_message;
        std::vector<std::string> details;
    };

    struct SuiteResult {
        std::vector<TestResult> test_results;
        int tests_passed = 0;
        int tests_failed = 0;
        bool overall_success = false;
    };

    CLICommandTests();
    ~CLICommandTests() = default;

    /**
     * @brief Run all CLI command tests
     */
    SuiteResult runAllTests();

    /**
     * @brief Test validate command
     */
    TestResult testValidateCommand();

    /**
     * @brief Test init command
     */
    TestResult testInitCommand();

    /**
     * @brief Test generate command
     */
    TestResult testGenerateCommand();

    /**
     * @brief Test build command
     */
    TestResult testBuildCommand();

    /**
     * @brief Test test command
     */
    TestResult testTestCommand();

    /**
     * @brief Test docs command
     */
    TestResult testDocsCommand();

    /**
     * @brief Test metrics command
     */
    TestResult testMetricsCommand();

    /**
     * @brief Test security command
     */
    TestResult testSecurityCommand();

    /**
     * @brief Test rules command
     */
    TestResult testRulesCommand();

    /**
     * @brief Test config command
     */
    TestResult testConfigCommand();

    /**
     * @brief Test all output formats
     */
    TestResult testOutputFormats();

    /**
     * @brief Test help functionality
     */
    TestResult testHelpFunctionality();

    /**
     * @brief Test error handling
     */
    TestResult testErrorHandling();

private:
    std::unique_ptr<interfaces::cli::commands::ValidateCommand> validate_cmd_;
    std::unique_ptr<interfaces::cli::commands::InitCommand> init_cmd_;
    std::unique_ptr<interfaces::cli::commands::GenerateCommand> generate_cmd_;
    std::unique_ptr<interfaces::cli::commands::BuildCommand> build_cmd_;
    std::unique_ptr<interfaces::cli::commands::TestCommand> test_cmd_;
    std::unique_ptr<interfaces::cli::commands::DocsCommand> docs_cmd_;
    std::unique_ptr<interfaces::cli::commands::MetricsCommand> metrics_cmd_;
    std::unique_ptr<interfaces::cli::commands::SecurityCommand> security_cmd_;
    std::unique_ptr<interfaces::cli::commands::RulesCommand> rules_cmd_;
    std::unique_ptr<interfaces::cli::commands::ConfigCommand> config_cmd_;

    std::string test_workspace_path_;

    /**
     * @brief Helper to create test execution context
     */
    interfaces::cli::executor::ExecutionContext createTestContext(
        const std::string& format = "text");

    /**
     * @brief Helper to run command and check result
     */
    bool runCommandAndCheck(
        const std::function<interfaces::cli::executor::ExecutionResult()>& command_func,
        bool should_succeed = true,
        const std::string& expected_message = "");

    /**
     * @brief Helper to clean up test files
     */
    void cleanupTestFiles();
};

} // namespace akao::tests::cli
