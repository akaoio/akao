/**
 * @id: akao:file:interface:cli:main:v1
 * @doc: Main entry point for Akao CLI application providing universal validation and enforcement framework with Pure Logic runtime support
 * @specification: CLI application entry point with basic command support and Pure Logic engine integration
 * @scope: Primary interface for Akao system operations including validation, building, testing, and Pure Logic execution
 * @timeline: 2025-01-08
 * @rationale: Provide unified entry point supporting Pure Logic runtime platform capabilities
 * @methodology: Simple CLI interface with Pure Logic execution environment
 * @references: ["akao:file:core:engine:logic:pure:v1", "akao:file:core:engine:compliance_validator:v1"]
 */

#include "core/engine/logic/pure/v1.hpp"
#include "core/engine/compliance-validator/v1.hpp"
#include "core/engine/metadata-standards/v1.hpp"
#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <fstream>

/**
 * @id: akao:function:interface:cli:main:v1
 * @doc: Main entry point function implementing universal validation and enforcement with Pure Logic runtime
 * @specification: CLI application main function with command parsing and Pure Logic execution
 * @scope: Primary system entry point for all Akao operations and command execution
 * @timeline: 2025-07-08
 * @rationale: Provide unified system entry point with Pure Logic runtime capabilities
 * @methodology: Command-line interface with integrated Pure Logic engine
 * @references: ["akao:file:interface:cli:main:v1", "akao:class:core:engine:logic:pure_logic_engine:v1"]
 */

void printUsage(const std::string& program_name) {
    std::cout << "Akao - Universal Validation and Enforcement Framework\n";
    std::cout << "Usage: " << program_name << " [options] <command> [args...]\n\n";
    
    std::cout << "Options:\n";
    std::cout << "  --help, -h          Show this help message\n";
    std::cout << "  --version           Show version information\n";
    std::cout << "  --verbose, -v       Verbose output\n\n";
    
    std::cout << "Commands:\n";
    std::cout << "  validate <path>     Universal validation - works on ANY project\n";
    std::cout << "  run <file.a>        Execute Pure Logic .a file\n";
    std::cout << "  parse <file>        Parse and analyze file\n";
    std::cout << "  test                Run system tests\n";
    std::cout << "  build               Build the project\n\n";
    
    std::cout << "Examples:\n";
    std::cout << "  " << program_name << " validate ./my-project\n";
    std::cout << "  " << program_name << " run main.a\n";
    std::cout << "  " << program_name << " test\n\n";
}

void printVersion() {
    std::cout << "Akao Version 1.0.0\n";
    std::cout << "Universal Validation and Enforcement Framework\n";
    std::cout << "Built with C++ Standard Library and Pure Logic Engine\n";
    std::cout << "Copyright (c) 2024 Akao Project\n";
}

int runValidation(const std::string& path, bool verbose) {
    try {
        akao::core::engine::ComplianceValidator validator;
        
        if (verbose) {
            std::cout << "Validating: " << path << std::endl;
        }
        
        auto report = validator.validateSystemCompliance(path);
        
        if (verbose) {
            std::string report_text = validator.generateComplianceReport(report);
            std::cout << report_text << std::endl;
        }
        
        if (report.system_compliant) {
            std::cout << "✅ Validation passed (" << report.compliance_percentage << "% compliant)" << std::endl;
            return 0;
        } else {
            std::cout << "❌ Validation failed (" << report.compliance_percentage << "% compliant)" << std::endl;
            std::cout << "Critical issues: " << report.blocking_errors.size() << std::endl;
            std::cout << "Warnings: " << report.quality_warnings.size() << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error during validation: " << e.what() << std::endl;
        return 1;
    }
}

int runPureLogicFile(const std::string& filename, bool verbose) {
    try {
        if (verbose) {
            std::cout << "Executing Pure Logic file: " << filename << std::endl;
        }
        
        // Check if file exists
        if (!std::filesystem::exists(filename)) {
            std::cerr << "Error: File not found: " << filename << std::endl;
            return 1;
        }
        
        // Read file content
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file: " << filename << std::endl;
            return 1;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        file.close();
        
        // Initialize Pure Logic engine
        akao::logic::PureLogicEngine engine;
        akao::logic::Context context;
        
        // Execute the logic
        auto result = engine.evaluate(content, context);
        
        if (verbose) {
            std::cout << "Execution result: " << result.toString() << std::endl;
        }
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error executing Pure Logic file: " << e.what() << std::endl;
        return 1;
    }
}

int runTests(bool verbose) {
    try {
        if (verbose) {
            std::cout << "Running system tests..." << std::endl;
        }
        
        // Initialize Pure Logic engine for testing
        akao::logic::PureLogicEngine engine;
        akao::logic::Context context;
        
        // Basic engine test
        auto test_result = engine.evaluate("true", context);
        if (!test_result.isBoolean() || !test_result.asBoolean()) {
            std::cout << "❌ Basic engine test failed" << std::endl;
            return 1;
        }
        
        std::cout << "✅ All tests passed" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error running tests: " << e.what() << std::endl;
        return 1;
    }
}

int main(int argc, char* argv[]) {
    try {
        // Parse command line arguments
        std::vector<std::string> args;
        for (int i = 1; i < argc; ++i) {
            args.emplace_back(argv[i]);
        }
        
        // Check for help or version
        if (args.empty() || args[0] == "--help" || args[0] == "-h") {
            printUsage(argv[0]);
            return 0;
        }
        
        if (args[0] == "--version") {
            printVersion();
            return 0;
        }
        
        // Check for verbose flag
        bool verbose = false;
        auto verbose_it = std::find(args.begin(), args.end(), "--verbose");
        if (verbose_it == args.end()) {
            verbose_it = std::find(args.begin(), args.end(), "-v");
        }
        if (verbose_it != args.end()) {
            verbose = true;
            args.erase(verbose_it);
        }
        
        // Process commands
        if (args.empty()) {
            std::cerr << "Error: No command specified" << std::endl;
            printUsage(argv[0]);
            return 1;
        }
        
        std::string command = args[0];
        
        if (command == "validate") {
            if (args.size() < 2) {
                std::cerr << "Error: validate command requires a path" << std::endl;
                return 1;
            }
            return runValidation(args[1], verbose);
        }
        else if (command == "run") {
            if (args.size() < 2) {
                std::cerr << "Error: run command requires a file" << std::endl;
                return 1;
            }
            return runPureLogicFile(args[1], verbose);
        }
        else if (command == "test") {
            return runTests(verbose);
        }
        else if (command == "build") {
            if (verbose) {
                std::cout << "Building project..." << std::endl;
            }
            std::cout << "✅ Build completed (placeholder)" << std::endl;
            return 0;
        }
        else {
            std::cerr << "Error: Unknown command: " << command << std::endl;
            printUsage(argv[0]);
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred" << std::endl;
        return 1;
    }
}
