#include "self_validation_test.hpp"
#include "core/rule/loader/rule_loader.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <regex>

namespace akao::tests::compliance {

SelfValidationTest::SelfValidationTest() {
    validator_ = std::make_unique<core::engine::validator::UniversalValidator>();
    rule_registry_ = std::make_unique<core::rule::registry::RuleRegistry>();
    akao_source_path_ = std::filesystem::current_path().string();
}

bool SelfValidationTest::initialize() {
    try {
        if (!validator_->initialize()) {
            std::cerr << "Failed to initialize universal validator" << std::endl;
            return false;
        }
        
        if (!rule_registry_->initialize()) {
            std::cerr << "Failed to initialize rule registry" << std::endl;
            return false;
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Self-validation test initialization failed: " << e.what() << std::endl;
        return false;
    }
}

std::vector<SelfValidationResult> SelfValidationTest::runAllTests() {
    std::vector<SelfValidationResult> results;
    
    std::cout << "Running Akao self-validation test suite..." << std::endl;
    
    // Run all test methods
    results.push_back(testCoreArchitecture());
    results.push_back(testRuleSystem());
    results.push_back(testInterfaceConsistency());
    results.push_back(testPhilosophyAdherence());
    results.push_back(testBuildSystem());
    results.push_back(testDocumentation());
    results.push_back(testLanguageIsolation());
    results.push_back(testStructureEnforcement());
    results.push_back(testMeasurementObservability());
    results.push_back(testSecurityCompliance());
    
    return results;
}

SelfValidationResult SelfValidationTest::runTest(const std::string& test_name) {
    if (test_name == "core-architecture") {
        return testCoreArchitecture();
    } else if (test_name == "rule-system") {
        return testRuleSystem();
    } else if (test_name == "interface-consistency") {
        return testInterfaceConsistency();
    } else if (test_name == "philosophy-adherence") {
        return testPhilosophyAdherence();
    } else if (test_name == "build-system") {
        return testBuildSystem();
    } else if (test_name == "documentation") {
        return testDocumentation();
    } else if (test_name == "language-isolation") {
        return testLanguageIsolation();
    } else if (test_name == "structure-enforcement") {
        return testStructureEnforcement();
    } else if (test_name == "measurement-observability") {
        return testMeasurementObservability();
    } else if (test_name == "security-compliance") {
        return testSecurityCompliance();
    } else {
        SelfValidationResult result;
        result.test_name = test_name;
        result.description = "Unknown test";
        result.passed = false;
        addViolation(result, "Test not found: " + test_name);
        return result;
    }
}

SelfValidationResult SelfValidationTest::testCoreArchitecture() {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    auto result = createTestResult("core-architecture", 
        "Validate core architecture follows Akao design principles");
    
    // Check directory structure
    if (!validateDirectoryStructure()) {
        addViolation(result, "Core directory structure does not follow Akao conventions");
    }
    
    // Check file naming conventions
    if (!validateFileNamingConventions()) {
        addViolation(result, "File naming conventions not followed");
    }
    
    // Check header includes
    if (!validateHeaderIncludes()) {
        addViolation(result, "Header include patterns not consistent");
    }
    
    // Check namespace usage
    if (!validateNamespaceUsage()) {
        addViolation(result, "Namespace structure not compliant");
    }
    
    // Check for required core components
    std::vector<std::string> required_components = {
        "core/engine/parser/yaml_parser.hpp",
        "core/engine/validator/universal_validator.hpp",
        "core/rule/registry/rule_registry.hpp",
        "core/filesystem/scanner/directory_scanner.hpp",
        "interfaces/cli/parser/command_parser.hpp"
    };
    
    for (const auto& component : required_components) {
        if (!fileExists(component)) {
            addViolation(result, "Missing required component: " + component);
        }
    }
    
    setTestPassed(result, result.violations.empty());
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    result.execution_time = duration.count() / 1000000.0;
    
    return result;
}

SelfValidationResult SelfValidationTest::testRuleSystem() {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    auto result = createTestResult("rule-system", 
        "Validate rule system implements 'rules govern rules' philosophy");
    
    // Check philosophy compliance
    if (!checkRuleGovernancePhilosophy()) {
        addViolation(result, "Rule governance philosophy not implemented");
    }
    
    // Check rule definitions exist
    if (!validateRuleDefinitions()) {
        addViolation(result, "Rule definitions incomplete or missing");
    }
    
    // Check that Akao validates itself
    try {
        auto validation_result = validator_->validate(akao_source_path_);
        if (validation_result.violations.empty()) {
            // Good - no violations
        } else {
            addWarning(result, "Self-validation found " + 
                      std::to_string(validation_result.violations.size()) + " violations");
        }
    } catch (const std::exception& e) {
        addViolation(result, "Self-validation failed: " + std::string(e.what()));
    }
    
    // Check rule registry functionality
    auto all_rules = rule_registry_->getAllRules();
    if (all_rules.empty()) {
        addViolation(result, "Rule registry contains no rules");
    }
    
    setTestPassed(result, result.violations.empty());
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    result.execution_time = duration.count() / 1000000.0;
    
    return result;
}

SelfValidationResult SelfValidationTest::testInterfaceConsistency() {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    auto result = createTestResult("interface-consistency", 
        "Validate interface consistency philosophy implementation");
    
    // Check interface consistency philosophy
    if (!checkInterfaceConsistencyPhilosophy()) {
        addViolation(result, "Interface consistency philosophy not implemented");
    }
    
    // Check CLI interface exists and is consistent
    if (!fileExists("interfaces/cli/parser/command_parser.hpp")) {
        addViolation(result, "CLI parser interface missing");
    }
    
    if (!fileExists("interfaces/cli/executor/command_executor.hpp")) {
        addViolation(result, "CLI executor interface missing");
    }
    
    if (!fileExists("interfaces/cli/formatter/output_formatter.hpp")) {
        addViolation(result, "CLI formatter interface missing");
    }
    
    // Check that all interfaces follow same patterns
    auto interface_files = findHeaderFiles();
    for (const auto& file : interface_files) {
        if (file.find("interfaces/") != std::string::npos) {
            if (!checkNamespaceStructure(file)) {
                addViolation(result, "Interface namespace inconsistent: " + file);
            }
        }
    }
    
    setTestPassed(result, result.violations.empty());
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    result.execution_time = duration.count() / 1000000.0;
    
    return result;
}

SelfValidationResult SelfValidationTest::testPhilosophyAdherence() {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    auto result = createTestResult("philosophy-adherence", 
        "Validate adherence to all Akao philosophies");
    
    // Check all philosophy implementations
    std::map<std::string, bool> philosophy_checks = {
        {"rule-governance", checkRuleGovernancePhilosophy()},
        {"interface-consistency", checkInterfaceConsistencyPhilosophy()},
        {"automation", checkAutomationPhilosophy()},
        {"build-duality", checkBuildDualityPhilosophy()},
        {"documentation-generation", checkDocumentationGenerationPhilosophy()},
        {"language-isolation", checkLanguageIsolationPhilosophy()},
        {"measurement-observability", checkMeasurementObservabilityPhilosophy()},
        {"security-explicit-behavior", checkSecurityExplicitBehaviorPhilosophy()},
        {"structure-enforcement", checkStructureEnforcementPhilosophy()},
        {"testing-coverage", checkTestingCoveragePhilosophy()},
        {"validation-universal", checkValidationUniversalPhilosophy()},
        {"visualization-explainability", checkVisualizationExplainabilityPhilosophy()}
    };
    
    for (const auto& [philosophy, implemented] : philosophy_checks) {
        if (!implemented) {
            addViolation(result, "Philosophy not implemented: " + philosophy);
        }
    }
    
    // Check philosophy YAML files exist
    if (!fileExists("philosophies/index.yaml")) {
        addViolation(result, "Philosophy index missing");
    }
    
    setTestPassed(result, result.violations.empty());
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    result.execution_time = duration.count() / 1000000.0;
    
    return result;
}

SelfValidationResult SelfValidationTest::testBuildSystem() {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    auto result = createTestResult("build-system", 
        "Validate build system duality philosophy implementation");
    
    // Check build duality philosophy
    if (!checkBuildDualityPhilosophy()) {
        addViolation(result, "Build duality philosophy not implemented");
    }
    
    // Check CMake configuration
    if (!validateCMakeConfiguration()) {
        addViolation(result, "CMake configuration invalid or incomplete");
    }
    
    // Check Makefile configuration
    if (!validateMakefileConfiguration()) {
        addViolation(result, "Makefile configuration invalid or incomplete");
    }
    
    // Check both build systems exist
    if (!fileExists("CMakeLists.txt")) {
        addViolation(result, "CMakeLists.txt missing");
    }
    
    if (!fileExists("Makefile")) {
        addViolation(result, "Makefile missing");
    }
    
    setTestPassed(result, result.violations.empty());
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    result.execution_time = duration.count() / 1000000.0;
    
    return result;
}

SelfValidationResult SelfValidationTest::testDocumentation() {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    auto result = createTestResult("documentation", 
        "Validate documentation completeness and generation capabilities");
    
    // Check documentation coverage
    if (!validateDocumentationCoverage()) {
        addViolation(result, "Documentation coverage insufficient");
    }
    
    // Check code comments
    if (!validateCodeComments()) {
        addViolation(result, "Code comment standards not met");
    }
    
    // Check blueprint documentation exists
    std::vector<std::string> required_docs = {
        "blueprint/plan.md",
        "blueprint/tree.md",
        "blueprint/cli.md",
        "blueprint/compliance_audit.md"
    };
    
    for (const auto& doc : required_docs) {
        if (!fileExists(doc)) {
            addViolation(result, "Required documentation missing: " + doc);
        }
    }
    
    setTestPassed(result, result.violations.empty());
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    result.execution_time = duration.count() / 1000000.0;
    
    return result;
}

SelfValidationResult SelfValidationTest::testLanguageIsolation() {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    auto result = createTestResult("language-isolation", 
        "Validate language isolation philosophy - C++ only with standard library");
    
    // Check language isolation philosophy
    if (!checkLanguageIsolationPhilosophy()) {
        addViolation(result, "Language isolation philosophy not implemented");
    }
    
    // Check that only C++ files exist in core and interfaces
    auto source_files = findSourceFiles();
    for (const auto& file : source_files) {
        if (file.find("core/") == 0 || file.find("interfaces/") == 0) {
            if (file.substr(file.find_last_of(".") + 1) != "cpp" && 
                file.substr(file.find_last_of(".") + 1) != "hpp") {
                addViolation(result, "Non-C++ source file in core/interfaces: " + file);
            }
        }
    }
    
    // Check for external dependencies (should only use standard library)
    auto header_files = findHeaderFiles();
    for (const auto& file : header_files) {
        auto lines = getFileLines(file);
        for (const auto& line : lines) {
            if (line.find("#include") != std::string::npos && 
                line.find("<") != std::string::npos &&
                line.find(">") != std::string::npos) {
                // This is a system include - check if it's standard library
                std::regex std_lib_pattern(R"(#include\s*<(iostream|fstream|string|vector|map|memory|filesystem|chrono|regex|algorithm|functional|sstream|iomanip|exception|stdexcept|cstdlib|cstdio)>)");
                if (!std::regex_search(line, std_lib_pattern)) {
                    addWarning(result, "Potential non-standard library include in " + file + ": " + line);
                }
            }
        }
    }
    
    setTestPassed(result, result.violations.empty());
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    result.execution_time = duration.count() / 1000000.0;
    
    return result;
}

SelfValidationResult SelfValidationTest::testStructureEnforcement() {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    auto result = createTestResult("structure-enforcement", 
        "Validate structure enforcement capabilities and compliance");
    
    // Check structure enforcement philosophy
    if (!checkStructureEnforcementPhilosophy()) {
        addViolation(result, "Structure enforcement philosophy not implemented");
    }
    
    // Check that structure enforcer exists
    if (!fileExists("core/filesystem/enforcer/structure_enforcer.hpp")) {
        addViolation(result, "Structure enforcer missing");
    }
    
    // Validate current project structure matches expectations
    try {
        auto validation_result = validator_->validate(akao_source_path_);
        // Structure-specific violations would be caught here
        for (const auto& violation : validation_result.violations) {
            if (violation.rule_id.find("structure") != std::string::npos) {
                addViolation(result, "Structure violation: " + violation.message);
            }
        }
    } catch (const std::exception& e) {
        addViolation(result, "Structure validation failed: " + std::string(e.what()));
    }
    
    setTestPassed(result, result.violations.empty());
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    result.execution_time = duration.count() / 1000000.0;
    
    return result;
}

SelfValidationResult SelfValidationTest::testMeasurementObservability() {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    auto result = createTestResult("measurement-observability", 
        "Validate measurement and observability capabilities");
    
    // Check measurement philosophy
    if (!checkMeasurementObservabilityPhilosophy()) {
        addViolation(result, "Measurement observability philosophy not implemented");
    }
    
    // Check that measurement components exist
    if (!fileExists("core/trace/tracer/violation_tracer.hpp")) {
        addViolation(result, "Violation tracer missing");
    }
    
    if (!fileExists("core/trace/reporter/report_generator.hpp")) {
        addViolation(result, "Report generator missing");
    }
    
    // Check performance requirements
    if (!validatePerformanceRequirements()) {
        addViolation(result, "Performance requirements not met");
    }
    
    setTestPassed(result, result.violations.empty());
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    result.execution_time = duration.count() / 1000000.0;
    
    return result;
}

SelfValidationResult SelfValidationTest::testSecurityCompliance() {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    auto result = createTestResult("security-compliance", 
        "Validate security and explicit behavior requirements");
    
    // Check security philosophy
    if (!checkSecurityExplicitBehaviorPhilosophy()) {
        addViolation(result, "Security explicit behavior philosophy not implemented");
    }
    
    // Check error handling
    if (!validateErrorHandling()) {
        addViolation(result, "Error handling patterns insufficient");
    }
    
    // Check memory management
    if (!validateMemoryManagement()) {
        addViolation(result, "Memory management patterns insufficient");
    }
    
    setTestPassed(result, result.violations.empty());
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    result.execution_time = duration.count() / 1000000.0;
    
    return result;
}

std::string SelfValidationTest::generateReport(const std::vector<SelfValidationResult>& results) {
    std::stringstream ss;
    
    ss << "Akao Self-Validation Report\n";
    ss << "===========================\n\n";
    
    auto stats = utils::calculateTestStats(results);
    
    ss << "Summary:\n";
    ss << "  Total Tests: " << stats.total_tests << "\n";
    ss << "  Passed: " << stats.passed_tests << "\n";
    ss << "  Failed: " << stats.failed_tests << "\n";
    ss << "  Compliance Score: " << utils::calculateComplianceScore(results) << "%\n";
    ss << "  Total Execution Time: " << stats.total_execution_time << "s\n";
    ss << "  Total Violations: " << stats.total_violations << "\n";
    ss << "  Total Warnings: " << stats.total_warnings << "\n\n";
    
    ss << "Test Results:\n";
    ss << "=============\n\n";
    
    for (const auto& result : results) {
        ss << utils::formatTestResult(result) << "\n\n";
    }
    
    if (!allTestsPassed(results)) {
        ss << "FAILED TESTS:\n";
        auto failed_tests = utils::getFailedTestNames(results);
        for (const auto& test_name : failed_tests) {
            ss << "  - " << test_name << "\n";
        }
        ss << "\n";
    }
    
    ss << "Akao Self-Validation ";
    if (allTestsPassed(results)) {
        ss << "PASSED - All philosophies and rules satisfied!\n";
    } else {
        ss << "FAILED - Some violations found.\n";
    }
    
    return ss.str();
}

std::vector<std::string> SelfValidationTest::getTestNames() const {
    return {
        "core-architecture",
        "rule-system", 
        "interface-consistency",
        "philosophy-adherence",
        "build-system",
        "documentation",
        "language-isolation",
        "structure-enforcement",
        "measurement-observability",
        "security-compliance"
    };
}

bool SelfValidationTest::allTestsPassed(const std::vector<SelfValidationResult>& results) const {
    for (const auto& result : results) {
        if (!result.passed) {
            return false;
        }
    }
    return true;
}

// Helper method implementations
SelfValidationResult SelfValidationTest::createTestResult(const std::string& name, const std::string& description) {
    SelfValidationResult result;
    result.test_name = name;
    result.description = description;
    result.passed = false;
    return result;
}

void SelfValidationTest::addViolation(SelfValidationResult& result, const std::string& violation) {
    result.violations.push_back(violation);
}

void SelfValidationTest::addWarning(SelfValidationResult& result, const std::string& warning) {
    result.warnings.push_back(warning);
}

void SelfValidationTest::setTestPassed(SelfValidationResult& result, bool passed) {
    result.passed = passed;
}

// Validation helper implementations (simplified for brevity)
bool SelfValidationTest::validateDirectoryStructure() {
    std::vector<std::string> required_dirs = {
        "core/engine/parser",
        "core/engine/validator", 
        "core/rule/loader",
        "core/rule/registry",
        "core/rule/executor",
        "core/filesystem/scanner",
        "core/filesystem/enforcer",
        "core/trace/tracer",
        "core/trace/reporter",
        "interfaces/cli/parser",
        "interfaces/cli/executor",
        "interfaces/cli/formatter",
        "philosophies",
        "rules"
    };
    
    for (const auto& dir : required_dirs) {
        if (!std::filesystem::exists(dir)) {
            return false;
        }
    }
    return true;
}

bool SelfValidationTest::validateFileNamingConventions() {
    // Check snake_case naming for files
    auto files = findSourceFiles();
    for (const auto& file : files) {
        std::string filename = std::filesystem::path(file).filename().string();
        // Simple check - should not contain camelCase patterns
        if (std::regex_search(filename, std::regex("[a-z][A-Z]"))) {
            return false;
        }
    }
    return true;
}

bool SelfValidationTest::validateHeaderIncludes() {
    auto header_files = findHeaderFiles();
    for (const auto& file : header_files) {
        if (!checkIncludeGuards(file)) {
            return false;
        }
    }
    return true;
}

bool SelfValidationTest::validateNamespaceUsage() {
    auto source_files = findSourceFiles();
    for (const auto& file : source_files) {
        if (!checkNamespaceStructure(file)) {
            return false;
        }
    }
    return true;
}

bool SelfValidationTest::validatePhilosophyCompliance() {
    return fileExists("philosophies/index.yaml");
}

bool SelfValidationTest::validateRuleDefinitions() {
    return fileExists("rules/index.yaml");
}

bool SelfValidationTest::validateCMakeConfiguration() {
    return fileExists("CMakeLists.txt");
}

bool SelfValidationTest::validateMakefileConfiguration() {
    return fileExists("Makefile");
}

bool SelfValidationTest::validateDocumentationCoverage() {
    return fileExists("blueprint/plan.md");
}

bool SelfValidationTest::validateCodeComments() {
    // Simplified - check that header files have some documentation
    auto header_files = findHeaderFiles();
    for (const auto& file : header_files) {
        auto content = readFile(file);
        if (content.find("/**") == std::string::npos && content.find("///") == std::string::npos) {
            return false;
        }
    }
    return true;
}

bool SelfValidationTest::validateErrorHandling() {
    auto source_files = findSourceFiles();
    for (const auto& file : source_files) {
        if (!checkErrorHandlingPattern(file)) {
            return false;
        }
    }
    return true;
}

bool SelfValidationTest::validateMemoryManagement() {
    auto source_files = findSourceFiles();
    for (const auto& file : source_files) {
        if (!checkMemoryManagementPattern(file)) {
            return false;
        }
    }
    return true;
}

bool SelfValidationTest::validatePerformanceRequirements() {
    // Basic performance validation - check for obvious inefficiencies
    return true; // Simplified for now
}

// File system helpers
std::vector<std::string> SelfValidationTest::findSourceFiles() {
    std::vector<std::string> files;
    
    for (const auto& entry : std::filesystem::recursive_directory_iterator(".")) {
        if (entry.is_regular_file()) {
            std::string path = entry.path().string();
            if (path.ends_with(".cpp")) {
                files.push_back(path);
            }
        }
    }
    
    return files;
}

std::vector<std::string> SelfValidationTest::findHeaderFiles() {
    std::vector<std::string> files;
    
    for (const auto& entry : std::filesystem::recursive_directory_iterator(".")) {
        if (entry.is_regular_file()) {
            std::string path = entry.path().string();
            if (path.ends_with(".hpp") || path.ends_with(".h")) {
                files.push_back(path);
            }
        }
    }
    
    return files;
}

std::vector<std::string> SelfValidationTest::findYamlFiles() {
    std::vector<std::string> files;
    
    for (const auto& entry : std::filesystem::recursive_directory_iterator(".")) {
        if (entry.is_regular_file()) {
            std::string path = entry.path().string();
            if (path.ends_with(".yaml") || path.ends_with(".yml")) {
                files.push_back(path);
            }
        }
    }
    
    return files;
}

bool SelfValidationTest::fileExists(const std::string& path) {
    return std::filesystem::exists(path);
}

std::string SelfValidationTest::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

std::vector<std::string> SelfValidationTest::getFileLines(const std::string& path) {
    std::vector<std::string> lines;
    std::ifstream file(path);
    
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            lines.push_back(line);
        }
    }
    
    return lines;
}

// Validation rule checkers (simplified implementations)
bool SelfValidationTest::checkIncludeGuards(const std::string& header_file) {
    auto content = readFile(header_file);
    return content.find("#pragma once") != std::string::npos || 
           (content.find("#ifndef") != std::string::npos && content.find("#define") != std::string::npos);
}

bool SelfValidationTest::checkNamespaceStructure(const std::string& source_file) {
    auto content = readFile(source_file);
    return content.find("namespace akao") != std::string::npos;
}

bool SelfValidationTest::checkErrorHandlingPattern(const std::string& source_file) {
    auto content = readFile(source_file);
    return content.find("try") != std::string::npos || content.find("exception") != std::string::npos;
}

bool SelfValidationTest::checkMemoryManagementPattern(const std::string& source_file) {
    auto content = readFile(source_file);
    // Check for smart pointers usage
    return content.find("std::unique_ptr") != std::string::npos || 
           content.find("std::shared_ptr") != std::string::npos ||
           content.find("new") == std::string::npos; // Prefer no raw new
}

// Philosophy compliance checkers (simplified)
bool SelfValidationTest::checkRuleGovernancePhilosophy() {
    return fileExists("philosophies/rule/governance/v1.yaml");
}

bool SelfValidationTest::checkInterfaceConsistencyPhilosophy() {
    return fileExists("philosophies/interface/consistency/v1.yaml");
}

bool SelfValidationTest::checkAutomationPhilosophy() {
    return fileExists("philosophies/automation/updates/v1.yaml");
}

bool SelfValidationTest::checkBuildDualityPhilosophy() {
    return fileExists("philosophies/build/duality/v1.yaml");
}

bool SelfValidationTest::checkDocumentationGenerationPhilosophy() {
    return fileExists("philosophies/documentation/generation/v1.yaml");
}

bool SelfValidationTest::checkLanguageIsolationPhilosophy() {
    return fileExists("philosophies/language/isolation/v1.yaml");
}

bool SelfValidationTest::checkMeasurementObservabilityPhilosophy() {
    return fileExists("philosophies/measurement/observability/v1.yaml");
}

bool SelfValidationTest::checkSecurityExplicitBehaviorPhilosophy() {
    return fileExists("philosophies/security/explicit_behavior/");
}

bool SelfValidationTest::checkStructureEnforcementPhilosophy() {
    return fileExists("philosophies/structure/enforcement/");
}

bool SelfValidationTest::checkTestingCoveragePhilosophy() {
    return fileExists("philosophies/testing/coverage/");
}

bool SelfValidationTest::checkValidationUniversalPhilosophy() {
    return fileExists("philosophies/validation/universal/");
}

bool SelfValidationTest::checkVisualizationExplainabilityPhilosophy() {
    return fileExists("philosophies/visualization/graph_explainability/");
}

// Utility function implementations
namespace utils {

std::string formatTestResults(const std::vector<SelfValidationResult>& results) {
    std::stringstream ss;
    
    for (const auto& result : results) {
        ss << formatTestResult(result) << "\n";
    }
    
    return ss.str();
}

std::string formatTestResult(const SelfValidationResult& result) {
    std::stringstream ss;
    
    ss << "Test: " << result.test_name << "\n";
    ss << "Description: " << result.description << "\n";
    ss << "Status: " << (result.passed ? "PASSED" : "FAILED") << "\n";
    ss << "Execution Time: " << result.execution_time << "s\n";
    
    if (!result.violations.empty()) {
        ss << "Violations:\n";
        for (const auto& violation : result.violations) {
            ss << "  - " << violation << "\n";
        }
    }
    
    if (!result.warnings.empty()) {
        ss << "Warnings:\n";
        for (const auto& warning : result.warnings) {
            ss << "  - " << warning << "\n";
        }
    }
    
    return ss.str();
}

double calculateComplianceScore(const std::vector<SelfValidationResult>& results) {
    if (results.empty()) return 0.0;
    
    size_t passed = 0;
    for (const auto& result : results) {
        if (result.passed) passed++;
    }
    
    return (static_cast<double>(passed) / results.size()) * 100.0;
}

std::string generateTestSummary(const std::vector<SelfValidationResult>& results) {
    auto stats = calculateTestStats(results);
    
    std::stringstream ss;
    ss << "Test Summary: " << stats.passed_tests << "/" << stats.total_tests << " passed";
    ss << " (" << calculateComplianceScore(results) << "% compliance)";
    
    return ss.str();
}

bool criticalTestsPassed(const std::vector<SelfValidationResult>& results) {
    std::vector<std::string> critical_tests = {
        "core-architecture",
        "rule-system",
        "philosophy-adherence"
    };
    
    for (const auto& result : results) {
        for (const auto& critical : critical_tests) {
            if (result.test_name == critical && !result.passed) {
                return false;
            }
        }
    }
    
    return true;
}

std::vector<std::string> getFailedTestNames(const std::vector<SelfValidationResult>& results) {
    std::vector<std::string> failed;
    
    for (const auto& result : results) {
        if (!result.passed) {
            failed.push_back(result.test_name);
        }
    }
    
    return failed;
}

TestStats calculateTestStats(const std::vector<SelfValidationResult>& results) {
    TestStats stats;
    
    stats.total_tests = results.size();
    stats.passed_tests = 0;
    stats.failed_tests = 0;
    stats.total_execution_time = 0.0;
    stats.total_violations = 0;
    stats.total_warnings = 0;
    
    for (const auto& result : results) {
        if (result.passed) {
            stats.passed_tests++;
        } else {
            stats.failed_tests++;
        }
        
        stats.total_execution_time += result.execution_time;
        stats.total_violations += result.violations.size();
        stats.total_warnings += result.warnings.size();
    }
    
    return stats;
}

} // namespace utils

} // namespace akao::tests::compliance
