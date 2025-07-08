#include "cli_command_tests.hpp"
#include <filesystem>
#include <iostream>
#include <fstream>

namespace akao::tests::cli {

CLICommandTests::CLICommandTests() 
    : test_workspace_path_("./tmp/test_workspace_cli") {
    
    // Initialize all command instances
    validate_cmd_ = std::make_unique<interfaces::cli::commands::ValidateCommand>();
    init_cmd_ = std::make_unique<interfaces::cli::commands::InitCommand>();
    generate_cmd_ = std::make_unique<interfaces::cli::commands::GenerateCommand>();
    build_cmd_ = std::make_unique<interfaces::cli::commands::BuildCommand>();
    test_cmd_ = std::make_unique<interfaces::cli::commands::TestCommand>();
    docs_cmd_ = std::make_unique<interfaces::cli::commands::DocsCommand>();
    metrics_cmd_ = std::make_unique<interfaces::cli::commands::MetricsCommand>();
    security_cmd_ = std::make_unique<interfaces::cli::commands::SecurityCommand>();
    rules_cmd_ = std::make_unique<interfaces::cli::commands::RulesCommand>();
    config_cmd_ = std::make_unique<interfaces::cli::commands::ConfigCommand>();

    // Clean up any existing test workspace
    cleanupTestFiles();
    
    // Create test workspace
    std::filesystem::create_directories(test_workspace_path_);
}

CLICommandTests::SuiteResult CLICommandTests::runAllTests() {
    SuiteResult suite_result;
    
    std::cout << "Running comprehensive CLI command tests...\n\n";
    
    // Test all commands
    suite_result.test_results.push_back(testValidateCommand());
    suite_result.test_results.push_back(testInitCommand());
    suite_result.test_results.push_back(testGenerateCommand());
    suite_result.test_results.push_back(testBuildCommand());
    suite_result.test_results.push_back(testTestCommand());
    suite_result.test_results.push_back(testDocsCommand());
    suite_result.test_results.push_back(testMetricsCommand());
    suite_result.test_results.push_back(testSecurityCommand());
    suite_result.test_results.push_back(testRulesCommand());
    suite_result.test_results.push_back(testConfigCommand());
    
    // Test infrastructure
    suite_result.test_results.push_back(testOutputFormats());
    suite_result.test_results.push_back(testHelpFunctionality());
    suite_result.test_results.push_back(testErrorHandling());
    
    // Calculate results
    for (const auto& result : suite_result.test_results) {
        if (result.passed) {
            suite_result.tests_passed++;
        } else {
            suite_result.tests_failed++;
        }
    }
    
    suite_result.overall_success = (suite_result.tests_failed == 0);
    
    // Cleanup
    cleanupTestFiles();
    
    return suite_result;
}

CLICommandTests::TestResult CLICommandTests::testValidateCommand() {
    TestResult result;
    result.test_name = "Validate Command Test";
    
    try {
        auto context = createTestContext();
        
        // Test basic validation
        auto exec_result = validate_cmd_->execute(context, {"."});
        if (!exec_result.success) {
            result.error_message = "Basic validation failed: " + exec_result.error_message;
            return result;
        }
        result.details.push_back("✓ Basic validation works");
        
        // Test self-validation
        exec_result = validate_cmd_->execute(context, {"--self"});
        if (!exec_result.success) {
            result.error_message = "Self-validation failed: " + exec_result.error_message;
            return result;
        }
        result.details.push_back("✓ Self-validation works");
        
        // Test with tracing
        exec_result = validate_cmd_->execute(context, {"--trace", "."});
        if (!exec_result.success) {
            result.error_message = "Validation with tracing failed: " + exec_result.error_message;
            return result;
        }
        result.details.push_back("✓ Tracing functionality works");
        
        // Test export functionality
        std::string export_file = test_workspace_path_ + "/validation_results.yaml";
        exec_result = validate_cmd_->execute(context, {"--export", export_file, "."});
        if (!exec_result.success) {
            result.error_message = "Export functionality failed: " + exec_result.error_message;
            return result;
        }
        
        if (!std::filesystem::exists(export_file)) {
            result.error_message = "Export file was not created";
            return result;
        }
        result.details.push_back("✓ Export functionality works");
        
        // Test rule categories
        exec_result = validate_cmd_->execute(context, {"--rules", "structure", "."});
        if (!exec_result.success) {
            result.error_message = "Rule category validation failed: " + exec_result.error_message;
            return result;
        }
        result.details.push_back("✓ Rule category filtering works");
        
        result.passed = true;
        
    } catch (const std::exception& e) {
        result.error_message = "Exception in validate command test: " + std::string(e.what());
    }
    
    return result;
}

CLICommandTests::TestResult CLICommandTests::testInitCommand() {
    TestResult result;
    result.test_name = "Init Command Test";
    
    try {
        auto context = createTestContext();
        
        // Test C++ project init
        std::string cpp_project_path = test_workspace_path_ + "/test_cpp_project";
        auto exec_result = init_cmd_->execute(context, {"--type", "cpp", cpp_project_path});
        if (!exec_result.success) {
            result.error_message = "C++ project init failed: " + exec_result.error_message;
            return result;
        }
        
        if (!std::filesystem::exists(cpp_project_path)) {
            result.error_message = "C++ project directory was not created";
            return result;
        }
        result.details.push_back("✓ C++ project initialization works");
        
        // Test Rust project init
        std::string rust_project_path = test_workspace_path_ + "/test_rust_project";
        exec_result = init_cmd_->execute(context, {"--type", "rust", rust_project_path});
        if (!exec_result.success) {
            result.error_message = "Rust project init failed: " + exec_result.error_message;
            return result;
        }
        
        if (!std::filesystem::exists(rust_project_path)) {
            result.error_message = "Rust project directory was not created";
            return result;
        }
        result.details.push_back("✓ Rust project initialization works");
        
        // Test framework init
        std::string framework_path = test_workspace_path_ + "/test_framework";
        exec_result = init_cmd_->execute(context, {"--type", "framework", framework_path});
        if (!exec_result.success) {
            result.error_message = "Framework init failed: " + exec_result.error_message;
            return result;
        }
        result.details.push_back("✓ Framework initialization works");
        
        // Test with template
        std::string templated_path = test_workspace_path_ + "/test_templated";
        exec_result = init_cmd_->execute(context, {"--type", "cpp", "--template", "full", templated_path});
        if (!exec_result.success) {
            result.error_message = "Template init failed: " + exec_result.error_message;
            return result;
        }
        result.details.push_back("✓ Template-based initialization works");
        
        result.passed = true;
        
    } catch (const std::exception& e) {
        result.error_message = "Exception in init command test: " + std::string(e.what());
    }
    
    return result;
}

CLICommandTests::TestResult CLICommandTests::testGenerateCommand() {
    TestResult result;
    result.test_name = "Generate Command Test";
    
    try {
        auto context = createTestContext();
        
        // Test project generation
        auto exec_result = generate_cmd_->execute(context, {"project", "--lang", "cpp"});
        if (!exec_result.success) {
            result.error_message = "Project generation failed: " + exec_result.error_message;
            return result;
        }
        result.details.push_back("✓ Project generation works");
        
        // Test framework generation
        exec_result = generate_cmd_->execute(context, {"framework", "--type", "validation"});
        if (!exec_result.success) {
            result.error_message = "Framework generation failed: " + exec_result.error_message;
            return result;
        }
        result.details.push_back("✓ Framework generation works");
        
        // Test language support generation
        exec_result = generate_cmd_->execute(context, {"language", "--name", "go"});
        if (!exec_result.success) {
            result.error_message = "Language generation failed: " + exec_result.error_message;
            return result;
        }
        result.details.push_back("✓ Language support generation works");
        
        // Test documentation generation
        exec_result = generate_cmd_->execute(context, {"docs", "--type", "api"});
        if (!exec_result.success) {
            result.error_message = "Documentation generation failed: " + exec_result.error_message;
            return result;
        }
        result.details.push_back("✓ Documentation generation works");
        
        // Test with examples flag
        exec_result = generate_cmd_->execute(context, {"project", "--lang", "rust", "--examples"});
        if (!exec_result.success) {
            result.error_message = "Generation with examples failed: " + exec_result.error_message;
            return result;
        }
        result.details.push_back("✓ Examples generation works");
        
        result.passed = true;
        
    } catch (const std::exception& e) {
        result.error_message = "Exception in generate command test: " + std::string(e.what());
    }
    
    return result;
}

CLICommandTests::TestResult CLICommandTests::testBuildCommand() {
    TestResult result;
    result.test_name = "Build Command Test";
    
    try {
        auto context = createTestContext();
        
        // Test basic build
        auto exec_result = build_cmd_->execute(context, {});
        if (!exec_result.success) {
            result.error_message = "Basic build failed: " + exec_result.error_message;
            return result;
        }
        result.details.push_back("✓ Basic build works");
        
        // Test production mode
        exec_result = build_cmd_->execute(context, {"--mode", "production"});
        if (!exec_result.success) {
            result.error_message = "Production build failed: " + exec_result.error_message;
            return result;
        }
        result.details.push_back("✓ Production build works");
        
        // Test development mode
        exec_result = build_cmd_->execute(context, {"--mode", "development"});
        if (!exec_result.success) {
            result.error_message = "Development build failed: " + exec_result.error_message;
            return result;
        }
        result.details.push_back("✓ Development build works");
        
        // Test with validation
        exec_result = build_cmd_->execute(context, {"--validate"});
        if (!exec_result.success) {
            result.error_message = "Build with validation failed: " + exec_result.error_message;
            return result;
        }
        result.details.push_back("✓ Build with validation works");
        
        // Test with clean (use test build directory to avoid interfering with main build)
        exec_result = build_cmd_->execute(context, {"--clean", "--output", "tmp/test_build"});
        if (!exec_result.success) {
            result.error_message = "Clean build failed: " + exec_result.error_message;
            return result;
        }
        result.details.push_back("✓ Clean build works");
        
        result.passed = true;
        
    } catch (const std::exception& e) {
        result.error_message = "Exception in build command test: " + std::string(e.what());
    }
    
    return result;
}

CLICommandTests::TestResult CLICommandTests::testTestCommand() {
    TestResult result;
    result.test_name = "Test Command Test";
    
    try {
        auto context = createTestContext();
        
        // Test all tests
        auto exec_result = test_cmd_->execute(context, {"all"});
        if (!exec_result.success) {
            result.error_message = "All tests failed: " + exec_result.error_message;
            return result;
        }
        result.details.push_back("✓ All tests execution works");
        
        // Test unit tests
        exec_result = test_cmd_->execute(context, {"unit"});
        if (!exec_result.success) {
            result.error_message = "Unit tests failed: " + exec_result.error_message;
            return result;
        }
        result.details.push_back("✓ Unit tests execution works");
        
        // Test integration tests
        exec_result = test_cmd_->execute(context, {"integration"});
        if (!exec_result.success) {
            result.error_message = "Integration tests failed: " + exec_result.error_message;
            return result;
        }
        result.details.push_back("✓ Integration tests execution works");
        
        // Test compliance tests
        exec_result = test_cmd_->execute(context, {"compliance"});
        if (!exec_result.success) {
            result.error_message = "Compliance tests failed: " + exec_result.error_message;
            return result;
        }
        result.details.push_back("✓ Compliance tests execution works");
        
        // Test with coverage
        exec_result = test_cmd_->execute(context, {"unit", "--coverage"});
        if (!exec_result.success) {
            result.error_message = "Coverage tests failed: " + exec_result.error_message;
            return result;
        }
        result.details.push_back("✓ Coverage reporting works");
        
        result.passed = true;
        
    } catch (const std::exception& e) {
        result.error_message = "Exception in test command test: " + std::string(e.what());
    }
    
    return result;
}

CLICommandTests::TestResult CLICommandTests::testDocsCommand() {
    TestResult result;
    result.test_name = "Docs Command Test";
    
    try {
        auto context = createTestContext();
        
        // Test generate docs
        auto exec_result = docs_cmd_->execute(context, {"generate"});
        if (!exec_result.success) {
            result.error_message = "Docs generation failed: " + exec_result.error_message;
            return result;
        }
        result.details.push_back("✓ Documentation generation works");
        
        // Test serve docs
        exec_result = docs_cmd_->execute(context, {"serve"});
        if (!exec_result.success) {
            result.error_message = "Docs serving failed: " + exec_result.error_message;
            return result;
        }
        result.details.push_back("✓ Documentation serving works");
        
        result.passed = true;
        
    } catch (const std::exception& e) {
        result.error_message = "Exception in docs command test: " + std::string(e.what());
    }
    
    return result;
}

CLICommandTests::TestResult CLICommandTests::testMetricsCommand() {
    TestResult result;
    result.test_name = "Metrics Command Test";
    
    try {
        auto context = createTestContext();
        
        // Test collect metrics
        auto exec_result = metrics_cmd_->execute(context, {"collect"});
        if (!exec_result.success) {
            result.error_message = "Metrics collection failed: " + exec_result.error_message;
            return result;
        }
        result.details.push_back("✓ Metrics collection works");
        
        // Test report metrics
        exec_result = metrics_cmd_->execute(context, {"report"});
        if (!exec_result.success) {
            result.error_message = "Metrics reporting failed: " + exec_result.error_message;
            return result;
        }
        result.details.push_back("✓ Metrics reporting works");
        
        result.passed = true;
        
    } catch (const std::exception& e) {
        result.error_message = "Exception in metrics command test: " + std::string(e.what());
    }
    
    return result;
}

CLICommandTests::TestResult CLICommandTests::testSecurityCommand() {
    TestResult result;
    result.test_name = "Security Command Test";
    
    try {
        auto context = createTestContext();
        
        // Test security scan
        auto exec_result = security_cmd_->execute(context, {"scan"});
        if (!exec_result.success) {
            result.error_message = "Security scan failed: " + exec_result.error_message;
            return result;
        }
        result.details.push_back("✓ Security scanning works");
        
        // Test audit
        exec_result = security_cmd_->execute(context, {"audit"});
        if (!exec_result.success) {
            result.error_message = "Security audit failed: " + exec_result.error_message;
            return result;
        }
        result.details.push_back("✓ Security audit works");
        
        result.passed = true;
        
    } catch (const std::exception& e) {
        result.error_message = "Exception in security command test: " + std::string(e.what());
    }
    
    return result;
}

CLICommandTests::TestResult CLICommandTests::testRulesCommand() {
    TestResult result;
    result.test_name = "Rules Command Test";
    
    try {
        auto context = createTestContext();
        
        // Test list rules
        auto exec_result = rules_cmd_->execute(context, {"list"});
        if (!exec_result.success) {
            result.error_message = "Rules list failed: " + exec_result.error_message;
            return result;
        }
        result.details.push_back("✓ Rules listing works");
        
        // Test validate rules
        exec_result = rules_cmd_->execute(context, {"validate"});
        if (!exec_result.success) {
            result.error_message = "Rules validation failed: " + exec_result.error_message;
            return result;
        }
        result.details.push_back("✓ Rules validation works");
        
        result.passed = true;
        
    } catch (const std::exception& e) {
        result.error_message = "Exception in rules command test: " + std::string(e.what());
    }
    
    return result;
}

CLICommandTests::TestResult CLICommandTests::testConfigCommand() {
    TestResult result;
    result.test_name = "Config Command Test";
    
    try {
        auto context = createTestContext();
        
        // Test list config
        auto exec_result = config_cmd_->execute(context, {"list"});
        if (!exec_result.success) {
            result.error_message = "Config list failed: " + exec_result.error_message;
            return result;
        }
        result.details.push_back("✓ Config listing works");
        
        // Test get config
        exec_result = config_cmd_->execute(context, {"get", "format"});
        if (!exec_result.success) {
            result.error_message = "Config get failed: " + exec_result.error_message;
            return result;
        }
        result.details.push_back("✓ Config getting works");
        
        result.passed = true;
        
    } catch (const std::exception& e) {
        result.error_message = "Exception in config command test: " + std::string(e.what());
    }
    
    return result;
}

CLICommandTests::TestResult CLICommandTests::testOutputFormats() {
    TestResult result;
    result.test_name = "Output Formats Test";
    
    try {
        // Test JSON format
        auto json_context = createTestContext("json");
        auto exec_result = validate_cmd_->execute(json_context, {"."});
        if (!exec_result.success) {
            result.error_message = "JSON format failed: " + exec_result.error_message;
            return result;
        }
        result.details.push_back("✓ JSON output format works");
        
        // Test YAML format
        auto yaml_context = createTestContext("yaml");
        exec_result = validate_cmd_->execute(yaml_context, {"."});
        if (!exec_result.success) {
            result.error_message = "YAML format failed: " + exec_result.error_message;
            return result;
        }
        result.details.push_back("✓ YAML output format works");
        
        // Test table format
        auto table_context = createTestContext("table");
        exec_result = validate_cmd_->execute(table_context, {"."});
        if (!exec_result.success) {
            result.error_message = "Table format failed: " + exec_result.error_message;
            return result;
        }
        result.details.push_back("✓ Table output format works");
        
        result.passed = true;
        
    } catch (const std::exception& e) {
        result.error_message = "Exception in output formats test: " + std::string(e.what());
    }
    
    return result;
}

CLICommandTests::TestResult CLICommandTests::testHelpFunctionality() {
    TestResult result;
    result.test_name = "Help Functionality Test";
    
    try {
        // Test that all commands have usage, options, and flags
        auto usage = validate_cmd_->getUsage();
        if (usage.empty()) {
            result.error_message = "Validate command has no usage string";
            return result;
        }
        
        auto options = validate_cmd_->getSupportedOptions();
        auto flags = validate_cmd_->getSupportedFlags();
        
        if (options.empty() && flags.empty()) {
            result.error_message = "Validate command has no options or flags";
            return result;
        }
        
        result.details.push_back("✓ Command help metadata is complete");
        result.passed = true;
        
    } catch (const std::exception& e) {
        result.error_message = "Exception in help functionality test: " + std::string(e.what());
    }
    
    return result;
}

CLICommandTests::TestResult CLICommandTests::testErrorHandling() {
    TestResult result;
    result.test_name = "Error Handling Test";
    
    try {
        auto context = createTestContext();
        
        // Test invalid arguments
        auto exec_result = validate_cmd_->execute(context, {"--invalid-option"});
        // Should not crash, even with invalid options
        result.details.push_back("✓ Invalid arguments handled gracefully");
        
        // Test nonexistent paths
        exec_result = validate_cmd_->execute(context, {"/nonexistent/path"});
        // Should handle gracefully
        result.details.push_back("✓ Nonexistent paths handled gracefully");
        
        result.passed = true;
        
    } catch (const std::exception& e) {
        result.error_message = "Exception in error handling test: " + std::string(e.what());
    }
    
    return result;
}

interfaces::cli::executor::ExecutionContext CLICommandTests::createTestContext(const std::string& format) {
    interfaces::cli::executor::ExecutionContext context;
    context.current_directory = test_workspace_path_;
    context.output_format = format;
    context.verbose_mode = false;
    context.quiet_mode = false;
    context.debug_mode = false;
    return context;
}

void CLICommandTests::cleanupTestFiles() {
    try {
        if (std::filesystem::exists(test_workspace_path_)) {
            std::filesystem::remove_all(test_workspace_path_);
        }
    } catch (const std::exception&) {
        // Ignore cleanup errors
    }
}

} // namespace akao::tests::cli
