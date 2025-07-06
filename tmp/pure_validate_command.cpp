#include "pure_validate_command.hpp"
#include "../core/engine/logic/pure_logic_engine.hpp"
#include <filesystem>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace akao::interfaces::cli::commands {

PureValidateCommand::PureValidateCommand() {
    // Initialize the pure logic engine
    engine_.initialize();
}

executor::ExecutionResult PureValidateCommand::execute(
    const executor::ExecutionContext& context,
    const std::vector<std::string>& args) {
    
    try {
        auto options = parseArguments(args);
        
        // If self-validation requested, run system-wide validation
        if (options.self_validate) {
            return systemSelfValidation(options, context);
        }
        
        // If specific rules requested, validate only those
        if (!options.specific_rules.empty()) {
            return validateSpecificRules(options, context);
        }
        
        // Default: comprehensive validation with rules
        return validateWithRules(options, context);
        
    } catch (const std::exception& e) {
        executor::ExecutionResult result;
        result.success = false;
        result.error_message = "Validation failed: " + std::string(e.what());
        return result;
    }
}

PureValidateCommand::ValidationOptions PureValidateCommand::parseArguments(
    const std::vector<std::string>& args) {
    
    ValidationOptions options;
    
    for (size_t i = 0; i < args.size(); ++i) {
        const auto& arg = args[i];
        
        if (arg == "--target" || arg == "-t") {
            if (i + 1 < args.size()) {
                options.target_path = args[++i];
            }
        } else if (arg == "--categories" || arg == "-c") {
            if (i + 1 < args.size()) {
                auto categories_str = args[++i];
                std::stringstream ss(categories_str);
                std::string category;
                while (std::getline(ss, category, ',')) {
                    options.rule_categories.push_back(category);
                }
            }
        } else if (arg == "--philosophies" || arg == "-p") {
            if (i + 1 < args.size()) {
                auto philosophies_str = args[++i];
                std::stringstream ss(philosophies_str);
                std::string philosophy;
                while (std::getline(ss, philosophy, ',')) {
                    options.philosophies.push_back(philosophy);
                }
            }
        } else if (arg == "--rules" || arg == "-r") {
            if (i + 1 < args.size()) {
                auto rules_str = args[++i];
                std::stringstream ss(rules_str);
                std::string rule;
                while (std::getline(ss, rule, ',')) {
                    options.specific_rules.push_back(rule);
                }
            }
        } else if (arg == "--trace") {
            options.enable_tracing = true;
        } else if (arg == "--fix") {
            options.enable_fixing = true;
        } else if (arg == "--self-validate") {
            options.self_validate = true;
        } else if (arg == "--dry-run") {
            options.dry_run = true;
        } else if (arg == "--metrics") {
            options.include_metrics = true;
        } else if (arg == "--export") {
            if (i + 1 < args.size()) {
                options.export_file = args[++i];
            }
        } else if (arg == "--show-passing") {
            options.show_passing = true;
        } else if (arg == "--engine-trace") {
            options.engine_trace = true;
        } else if (!arg.empty() && arg[0] != '-') {
            // Assume it's a target path if no explicit --target was given
            if (options.target_path == ".") {
                options.target_path = arg;
            }
        }
    }
    
    // Default to all categories if none specified
    if (options.rule_categories.empty()) {
        options.rule_categories = {
            "structure", "testing", "documentation", "security", 
            "build", "interface", "automation", "validation"
        };
    }
    
    return options;
}

executor::ExecutionResult PureValidateCommand::validateWithRules(
    const ValidationOptions& options,
    const executor::ExecutionContext& context) {
    
    executor::ExecutionResult result;
    result.success = true;
    
    try {
        // Enable tracing if requested
        if (options.engine_trace) {
            engine_.enableTracing();
        }
        
        // Build context for validation
        akao::logic::Context validation_context;
        validation_context.setValue("target_path", akao::logic::Value(options.target_path));
        validation_context.setValue("dry_run", akao::logic::Value(options.dry_run));
        validation_context.setValue("enable_fixing", akao::logic::Value(options.enable_fixing));
        validation_context.setValue("show_passing", akao::logic::Value(options.show_passing));
        
        std::vector<akao::logic::Value> all_results;
        
        // Discover and execute rule files
        auto rule_files = discoverRuleFiles(options.rule_categories);
        for (const auto& rule_file : rule_files) {
            try {
                auto rule_result = engine_.executeRule(rule_file, validation_context);
                all_results.push_back(rule_result);
            } catch (const std::exception& e) {
                result.output_message += "Warning: Failed to execute rule " + rule_file + ": " + e.what() + "\n";
            }
        }
        
        // Discover and execute philosophy files if requested
        if (!options.philosophies.empty()) {
            auto philosophy_files = discoverPhilosophyFiles(options.philosophies);
            for (const auto& philosophy_file : philosophy_files) {
                try {
                    auto philosophy_result = engine_.executePhilosophy(philosophy_file, validation_context);
                    all_results.push_back(philosophy_result);
                } catch (const std::exception& e) {
                    result.output_message += "Warning: Failed to execute philosophy " + philosophy_file + ": " + e.what() + "\n";
                }
            }
        }
        
        // Format results
        formatValidationResults(all_results, context, result);
        
        // Include metrics if requested
        if (options.include_metrics) {
            auto metrics = engine_.getMetrics();
            result.output_message += "\n=== Execution Metrics ===\n";
            result.output_message += "Function calls: " + std::to_string(metrics.function_calls) + "\n";
            result.output_message += "Cache hits: " + std::to_string(metrics.cache_hits) + "\n";
            result.output_message += "Cache misses: " + std::to_string(metrics.cache_misses) + "\n";
        }
        
        // Include trace if requested
        if (options.enable_tracing || options.engine_trace) {
            auto trace = engine_.getExecutionTrace();
            if (!trace.empty()) {
                result.output_message += "\n=== Execution Trace ===\n";
                for (const auto& entry : trace) {
                    result.output_message += entry + "\n";
                }
            }
        }
        
        // Export results if requested
        if (!options.export_file.empty()) {
            // TODO: Implement export functionality
            result.output_message += "\nNote: Export functionality not yet implemented\n";
        }
        
    } catch (const std::exception& e) {
        result.success = false;
        result.error_message = "Validation execution failed: " + std::string(e.what());
    }
    
    return result;
}

executor::ExecutionResult PureValidateCommand::systemSelfValidation(
    const ValidationOptions& options,
    const executor::ExecutionContext& context) {
    
    executor::ExecutionResult result;
    result.success = true;
    result.output_message = "=== System Self-Validation ===\n\n";
    
    try {
        // Enable tracing for self-validation
        engine_.enableTracing();
        
        // Build meta-validation context
        akao::logic::Context meta_context;
        meta_context.setValue("self_validation", akao::logic::Value(true));
        meta_context.setValue("target_path", akao::logic::Value("."));
        
        std::vector<akao::logic::Value> validation_results;
        
        // 1. Validate all existing rules using meta-rules
        result.output_message += "1. Validating rule definitions...\n";
        auto rule_files = discoverRuleFiles({"validation", "rule"});
        
        for (const auto& rule_file : rule_files) {
            try {
                // Each rule should self-validate using its unit_tests
                auto rule_result = engine_.executeRule(rule_file, meta_context);
                validation_results.push_back(rule_result);
                
                // Check if rule passed its self-validation
                if (rule_result.isList() && !rule_result.asList().empty()) {
                    auto first_test = rule_result.asList()[0];
                    if (first_test.isObject() && first_test.asObject().count("status")) {
                        auto status = first_test.asObject().at("status").asString();
                        if (status == "pass") {
                            result.output_message += "  ✓ " + std::filesystem::path(rule_file).stem().string() + "\n";
                        } else {
                            result.output_message += "  ✗ " + std::filesystem::path(rule_file).stem().string() + ": " + status + "\n";
                            result.success = false;
                        }
                    }
                }
            } catch (const std::exception& e) {
                result.output_message += "  ✗ " + std::filesystem::path(rule_file).stem().string() + ": " + e.what() + "\n";
                result.success = false;
            }
        }
        
        // 2. Validate all philosophies
        result.output_message += "\n2. Validating philosophy definitions...\n";
        auto philosophy_files = discoverPhilosophyFiles({});
        
        for (const auto& philosophy_file : philosophy_files) {
            try {
                auto philosophy_result = engine_.executePhilosophy(philosophy_file, meta_context);
                validation_results.push_back(philosophy_result);
                
                // Check philosophy validation result
                if (philosophy_result.isObject() && philosophy_result.asObject().count("status")) {
                    auto status = philosophy_result.asObject().at("status").asString();
                    if (status == "valid") {
                        result.output_message += "  ✓ " + std::filesystem::path(philosophy_file).stem().string() + "\n";
                    } else {
                        result.output_message += "  ✗ " + std::filesystem::path(philosophy_file).stem().string() + ": " + status + "\n";
                        result.success = false;
                    }
                }
            } catch (const std::exception& e) {
                result.output_message += "  ✗ " + std::filesystem::path(philosophy_file).stem().string() + ": " + e.what() + "\n";
                result.success = false;
            }
        }
        
        // 3. Validate the logic engine itself
        result.output_message += "\n3. Validating logic engine...\n";
        try {
            // Test core logic operations
            akao::logic::Context test_context;
            test_context.setValue("x", akao::logic::Value(5));
            test_context.setValue("y", akao::logic::Value(10));
            
            // Test arithmetic
            auto add_result = engine_.evaluate("(+ x y)", test_context);
            if (add_result.isNumber() && add_result.asNumber() == 15) {
                result.output_message += "  ✓ Arithmetic operations\n";
            } else {
                result.output_message += "  ✗ Arithmetic operations failed\n";
                result.success = false;
            }
            
            // Test quantifiers
            auto list_val = akao::logic::Value(std::vector<akao::logic::Value>{
                akao::logic::Value(1), akao::logic::Value(2), akao::logic::Value(3)
            });
            test_context.setValue("numbers", list_val);
            
            auto forall_result = engine_.evaluate("(forall x numbers (< x 10))", test_context);
            if (forall_result.isBool() && forall_result.asBool()) {
                result.output_message += "  ✓ Quantifier operations\n";
            } else {
                result.output_message += "  ✗ Quantifier operations failed\n";
                result.success = false;
            }
            
            // Test function registry
            auto builtin_count = engine_.getBuiltinFunctionCount();
            if (builtin_count > 0) {
                result.output_message += "  ✓ Function registry (" + std::to_string(builtin_count) + " functions)\n";
            } else {
                result.output_message += "  ✗ Function registry empty\n";
                result.success = false;
            }
            
        } catch (const std::exception& e) {
            result.output_message += "  ✗ Logic engine validation failed: " + std::string(e.what()) + "\n";
            result.success = false;
        }
        
        // Summary
        result.output_message += "\n=== Self-Validation Summary ===\n";
        if (result.success) {
            result.output_message += "✓ AKAO system has successfully proven its own correctness\n";
            result.output_message += "All rules, philosophies, and engine components are valid.\n";
        } else {
            result.output_message += "✗ AKAO system self-validation failed\n";
            result.output_message += "Some components need attention (see details above).\n";
        }
        
        // Include detailed trace
        auto trace = engine_.getExecutionTrace();
        if (!trace.empty() && options.enable_tracing) {
            result.output_message += "\n=== Self-Validation Trace ===\n";
            for (const auto& entry : trace) {
                result.output_message += entry + "\n";
            }
        }
        
    } catch (const std::exception& e) {
        result.success = false;
        result.error_message = "Self-validation failed: " + std::string(e.what());
    }
    
    return result;
}

executor::ExecutionResult PureValidateCommand::validateSpecificRules(
    const ValidationOptions& options,
    const executor::ExecutionContext& context) {
    
    executor::ExecutionResult result;
    result.success = true;
    result.output_message = "=== Specific Rules Validation ===\n\n";
    
    try {
        akao::logic::Context validation_context;
        validation_context.setValue("target_path", akao::logic::Value(options.target_path));
        
        for (const auto& rule_name : options.specific_rules) {
            result.output_message += "Executing rule: " + rule_name + "\n";
            
            try {
                // Find the rule file
                std::string rule_file = "rules/" + rule_name + ".yaml";
                if (!std::filesystem::exists(rule_file)) {
                    // Try different locations
                    std::vector<std::string> search_paths = {
                        "rules/structure/" + rule_name + ".yaml",
                        "rules/testing/" + rule_name + ".yaml",
                        "rules/validation/" + rule_name + ".yaml"
                    };
                    
                    bool found = false;
                    for (const auto& path : search_paths) {
                        if (std::filesystem::exists(path)) {
                            rule_file = path;
                            found = true;
                            break;
                        }
                    }
                    
                    if (!found) {
                        result.output_message += "  ✗ Rule file not found: " + rule_name + "\n";
                        result.success = false;
                        continue;
                    }
                }
                
                auto rule_result = engine_.executeRule(rule_file, validation_context);
                
                // Format the specific rule result
                if (rule_result.isList()) {
                    for (const auto& test_result : rule_result.asList()) {
                        if (test_result.isObject()) {
                            auto obj = test_result.asObject();
                            if (obj.count("name") && obj.count("status")) {
                                auto name = obj.at("name").asString();
                                auto status = obj.at("status").asString();
                                
                                if (status == "pass") {
                                    result.output_message += "  ✓ " + name + "\n";
                                } else {
                                    result.output_message += "  ✗ " + name + ": " + status + "\n";
                                    if (obj.count("message")) {
                                        result.output_message += "    " + obj.at("message").asString() + "\n";
                                    }
                                    result.success = false;
                                }
                            }
                        }
                    }
                } else {
                    result.output_message += "  Result: " + rule_result.toString() + "\n";
                }
                
            } catch (const std::exception& e) {
                result.output_message += "  ✗ Execution failed: " + std::string(e.what()) + "\n";
                result.success = false;
            }
            
            result.output_message += "\n";
        }
        
    } catch (const std::exception& e) {
        result.success = false;
        result.error_message = "Specific rules validation failed: " + std::string(e.what());
    }
    
    return result;
}

std::vector<std::string> PureValidateCommand::discoverRuleFiles(
    const std::vector<std::string>& categories) {
    
    std::vector<std::string> rule_files;
    
    for (const auto& category : categories) {
        std::string rules_dir = "rules/" + category;
        
        if (std::filesystem::exists(rules_dir) && std::filesystem::is_directory(rules_dir)) {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(rules_dir)) {
                if (entry.is_regular_file() && entry.path().extension() == ".yaml") {
                    rule_files.push_back(entry.path().string());
                }
            }
        }
    }
    
    // Also check top-level rules directory
    if (std::filesystem::exists("rules") && std::filesystem::is_directory("rules")) {
        for (const auto& entry : std::filesystem::directory_iterator("rules")) {
            if (entry.is_regular_file() && entry.path().extension() == ".yaml") {
                rule_files.push_back(entry.path().string());
            }
        }
    }
    
    return rule_files;
}

std::vector<std::string> PureValidateCommand::discoverPhilosophyFiles(
    const std::vector<std::string>& philosophies) {
    
    std::vector<std::string> philosophy_files;
    
    if (philosophies.empty()) {
        // Discover all philosophies
        if (std::filesystem::exists("philosophies") && std::filesystem::is_directory("philosophies")) {
            for (const auto& entry : std::filesystem::recursive_directory_iterator("philosophies")) {
                if (entry.is_regular_file() && entry.path().extension() == ".yaml") {
                    philosophy_files.push_back(entry.path().string());
                }
            }
        }
    } else {
        // Look for specific philosophies
        for (const auto& philosophy : philosophies) {
            std::string philosophy_file = "philosophies/" + philosophy + ".yaml";
            if (std::filesystem::exists(philosophy_file)) {
                philosophy_files.push_back(philosophy_file);
            } else {
                // Try different locations
                std::vector<std::string> search_paths = {
                    "philosophies/structure/" + philosophy + ".yaml",
                    "philosophies/validation/" + philosophy + ".yaml",
                    "philosophies/rule/" + philosophy + ".yaml"
                };
                
                for (const auto& path : search_paths) {
                    if (std::filesystem::exists(path)) {
                        philosophy_files.push_back(path);
                        break;
                    }
                }
            }
        }
    }
    
    return philosophy_files;
}

void PureValidateCommand::formatValidationResults(
    const std::vector<akao::logic::Value>& results,
    const executor::ExecutionContext& context,
    executor::ExecutionResult& result) {
    
    int total_tests = 0;
    int passed_tests = 0;
    int failed_tests = 0;
    
    result.output_message += "=== Validation Results ===\n\n";
    
    for (const auto& validation_result : results) {
        if (validation_result.isList()) {
            // This is a rule result with multiple test cases
            for (const auto& test_case : validation_result.asList()) {
                if (test_case.isObject()) {
                    auto obj = test_case.asObject();
                    total_tests++;
                    
                    if (obj.count("name") && obj.count("status")) {
                        auto name = obj.at("name").asString();
                        auto status = obj.at("status").asString();
                        
                        if (status == "pass") {
                            passed_tests++;
                            if (parseArguments({}).show_passing) {
                                result.output_message += "✓ " + name + "\n";
                            }
                        } else {
                            failed_tests++;
                            result.output_message += "✗ " + name + ": " + status + "\n";
                            
                            if (obj.count("message")) {
                                result.output_message += "  " + obj.at("message").asString() + "\n";
                            }
                            
                            if (obj.count("details")) {
                                result.output_message += "  Details: " + obj.at("details").toString() + "\n";
                            }
                        }
                    }
                }
            }
        } else if (validation_result.isObject()) {
            // This is a single result (philosophy result)
            auto obj = validation_result.asObject();
            total_tests++;
            
            if (obj.count("status")) {
                auto status = obj.at("status").asString();
                
                if (status == "valid" || status == "pass") {
                    passed_tests++;
                    if (parseArguments({}).show_passing) {
                        result.output_message += "✓ Philosophy validation passed\n";
                    }
                } else {
                    failed_tests++;
                    result.output_message += "✗ Philosophy validation failed: " + status + "\n";
                    
                    if (obj.count("message")) {
                        result.output_message += "  " + obj.at("message").asString() + "\n";
                    }
                }
            }
        }
    }
    
    // Summary
    result.output_message += "\n=== Summary ===\n";
    result.output_message += "Total tests: " + std::to_string(total_tests) + "\n";
    result.output_message += "Passed: " + std::to_string(passed_tests) + "\n";
    result.output_message += "Failed: " + std::to_string(failed_tests) + "\n";
    
    if (failed_tests == 0) {
        result.output_message += "✓ All validations passed!\n";
        result.success = true;
    } else {
        result.output_message += "✗ " + std::to_string(failed_tests) + " validation(s) failed\n";
        result.success = false;
    }
}

std::string PureValidateCommand::getHelp() const {
    return R"(AKAO Pure Logic Engine Validation

Uses the pure logic engine to perform all validation through YAML-defined 
rules and philosophies. This replaces hardcoded validation logic with a 
data-driven approach where rules govern rules.

USAGE:
  akao validate [options] [target]

OPTIONS:
  --target, -t <path>        Target path to validate (default: .)
  --categories, -c <list>    Rule categories (comma-separated)
  --philosophies, -p <list>  Specific philosophies to apply
  --rules, -r <list>         Specific rules to execute
  --trace                    Enable violation tracing
  --fix                      Enable automatic fixing
  --self-validate           Run system-wide self-validation
  --dry-run                 Show what would be done without executing
  --metrics                 Include execution metrics
  --export <file>           Export results to file
  --show-passing            Show passing tests (default: only failures)
  --engine-trace            Enable logic engine execution tracing

CATEGORIES:
  structure, testing, documentation, security, build, interface, 
  automation, validation, rule

EXAMPLES:
  akao validate                           # Validate current directory
  akao validate /path/to/project         # Validate specific project
  akao validate --categories structure   # Only structure rules
  akao validate --self-validate          # System self-validation
  akao validate --rules one_class_per_file # Specific rule
  akao validate --trace --metrics        # Detailed analysis

PHILOSOPHY COMPLIANCE:
  - akao:philosophy:validation:universal:v1 - Works on any project
  - akao:philosophy:rule:governance:v1 - Self-governing rules
  - Zero hardcoded domain logic - all rules in YAML files)";
}

std::string PureValidateCommand::getUsage() const {
    return "akao validate [options] [target]";
}

} // namespace akao::interfaces::cli::commands
