#include "v1.hpp"
#include "../../../engine/logic/pure/v1.hpp"
#include "../../../engine/logic/godel/v1.hpp"
#include <chrono>
#include <algorithm>
#include <regex>
#include <fstream>
#include <filesystem>
#include <thread>
#include <future>

namespace akao::core::rule::executor {

// RuleExecutor implementation
RuleExecutor::RuleExecutor() 
    : strategy_(ExecutionStrategy::SEQUENTIAL)
    , parallel_execution_enabled_(false)
    , max_threads_(std::thread::hardware_concurrency()) {
    // Initialize Pure Logic Engine instead of Datalog
    pure_logic_engine_ = std::make_unique<akao::logic::PureLogicEngine>();
    pure_logic_engine_->initialize();
}

bool RuleExecutor::initialize() {
    // Register built-in rule handlers
    registerRuleHandler("structure:*", 
        [this](const loader::Rule& rule, const RuleExecutionContext& context) {
            return executeStructureRule(rule, context);
        });
    
    registerRuleHandler("interface:*", 
        [this](const loader::Rule& rule, const RuleExecutionContext& context) {
            return executeInterfaceRule(rule, context);
        });
    
    registerRuleHandler("language:*", 
        [this](const loader::Rule& rule, const RuleExecutionContext& context) {
            return executeLanguageRule(rule, context);
        });
    
    registerRuleHandler("security:*", 
        [this](const loader::Rule& rule, const RuleExecutionContext& context) {
            return executeSecurityRule(rule, context);
        });
    
    registerRuleHandler("testing:*", 
        [this](const loader::Rule& rule, const RuleExecutionContext& context) {
            return executeTestingRule(rule, context);
        });
    
    registerRuleHandler("build:*", 
        [this](const loader::Rule& rule, const RuleExecutionContext& context) {
            return executeBuildRule(rule, context);
        });
    
    registerRuleHandler("documentation:*", 
        [this](const loader::Rule& rule, const RuleExecutionContext& context) {
            return executeDocumentationRule(rule, context);
        });
    
    registerRuleHandler("automation:*", 
        [this](const loader::Rule& rule, const RuleExecutionContext& context) {
            return executeAutomationRule(rule, context);
        });
    
    registerRuleHandler("measurement:*", 
        [this](const loader::Rule& rule, const RuleExecutionContext& context) {
            return executeMeasurementRule(rule, context);
        });
    
    registerRuleHandler("validation:*", 
        [this](const loader::Rule& rule, const RuleExecutionContext& context) {
            return executeValidationRule(rule, context);
        });
    
    registerRuleHandler("visualization:*", 
        [this](const loader::Rule& rule, const RuleExecutionContext& context) {
            return executeVisualizationRule(rule, context);
        });
    
    return true;
}

RuleExecutor::ExecutionResult RuleExecutor::executeRule(const loader::Rule& rule, const RuleExecutionContext& context) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    ExecutionResult result = createSuccessResult();
    
    try {
        // Find appropriate handler
        for (const auto& [pattern, handler] : rule_handlers_) {
            if (isRulePatternMatch(pattern, rule.id)) {
                result = handler(rule, context);
                break;
            }
        }
        
        // Execute Pure Logic Engine expressions with mathematical formal proofs
        if (result.violations.empty() && !rule.pure_logic_expressions.empty()) {
            for (const auto& logic_expression : rule.pure_logic_expressions) {
                auto logic_result = executePureLogicExpression(logic_expression, context);
                result.violations.insert(result.violations.end(), 
                                       logic_result.violations.begin(), 
                                       logic_result.violations.end());
            }
        }
        
        // Fallback to datalog_rules for backward compatibility during migration
        if (result.violations.empty() && !rule.datalog_rules.empty()) {
            for (const auto& datalog_rule : rule.datalog_rules) {
                // Convert datalog rule to Pure Logic expression automatically
                std::string pure_logic_expr = convertDatalogToPureLogic(datalog_rule);
                auto logic_result = executePureLogicExpression(pure_logic_expr, context);
                result.violations.insert(result.violations.end(), 
                                       logic_result.violations.begin(), 
                                       logic_result.violations.end());
            }
        }
        
    } catch (const std::exception& e) {
        result = createFailureResult(e.what());
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time);
    result.execution_time_seconds = duration.count();
    
    updateStats(result, rule.category);
    
    return result;
}

RuleExecutor::ExecutionResult RuleExecutor::executeRules(const std::vector<std::shared_ptr<loader::Rule>>& rules,
                                                        const RuleExecutionContext& context,
                                                        ExecutionStrategy strategy) {
    ExecutionResult combined_result = createSuccessResult();
    auto start_time = std::chrono::high_resolution_clock::now();
    
    if (strategy == ExecutionStrategy::PARALLEL && parallel_execution_enabled_) {
        // Parallel execution
        std::vector<std::future<ExecutionResult>> futures;
        
        for (const auto& rule : rules) {
            futures.emplace_back(std::async(std::launch::async, [this, &rule, &context]() {
                return executeRule(*rule, context);
            }));
        }
        
        // Collect results
        for (auto& future : futures) {
            auto result = future.get();
            combined_result.violations.insert(combined_result.violations.end(),
                                            result.violations.begin(), result.violations.end());
            combined_result.files_processed += result.files_processed;
            combined_result.lines_processed += result.lines_processed;
        }
        
    } else {
        // Sequential execution
        for (const auto& rule : rules) {
            auto result = executeRule(*rule, context);
            combined_result.violations.insert(combined_result.violations.end(),
                                            result.violations.begin(), result.violations.end());
            combined_result.files_processed += result.files_processed;
            combined_result.lines_processed += result.lines_processed;
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time);
    combined_result.execution_time_seconds = duration.count();
    
    return combined_result;
}

RuleExecutor::ExecutionResult RuleExecutor::executeCategory(const std::string& category, 
                                                          const RuleExecutionContext& context) {
    // This would need access to rule registry - simplified implementation
    ExecutionResult result = createSuccessResult();
    return result;
}

RuleExecutor::ExecutionResult RuleExecutor::executePureLogicExpression(const std::string& logic_expression, 
                                                                      const RuleExecutionContext& context) {
    ExecutionResult result = createSuccessResult();
    
    try {
        // Create execution context for Pure Logic Engine
        akao::logic::Context logic_context;
        
        // Set up context variables from RuleExecutionContext
        logic_context.bindVariable("target_path", akao::logic::Value(context.target_path));
        logic_context.bindVariable("project_type", akao::logic::Value(context.project_type));
        
        // Add discovered files as a collection
        std::vector<akao::logic::Value> files;
        for (const auto& file : context.discovered_files) {
            files.push_back(akao::logic::Value(file));
        }
        logic_context.bindVariable("discovered_files", akao::logic::Value(files));
        
        // Execute the Pure Logic expression with mathematical formal proofs
        auto logic_result = pure_logic_engine_->evaluate(logic_expression, logic_context);
        
        // Convert logic result to violations
        if (logic_result.isBoolean() && !logic_result.asBoolean()) {
            // Logic expression evaluated to false - this indicates a violation
            engine::validator::Violation violation;
            violation.file_path = context.target_path;
            violation.line_number = 1;
            violation.message = "Pure Logic validation failed: " + logic_expression;
            violation.detected_at = std::chrono::system_clock::now();
            violation.rule_category = "pure_logic";
            violation.severity = "MEDIUM";
            
            result.violations.push_back(violation);
        } else if (logic_result.isCollection()) {
            // Logic expression returned a collection of violations
            auto violations_list = logic_result.asCollection();
            for (const auto& violation_data : violations_list) {
                if (violation_data.isObject()) {
                    auto violation_obj = violation_data.asObject();
                    
                    engine::validator::Violation violation;
                    violation.file_path = violation_obj.count("file") ? violation_obj.at("file").asString() : context.target_path;
                    violation.line_number = violation_obj.count("line") ? violation_obj.at("line").asInteger() : 1;
                    violation.message = violation_obj.count("message") ? violation_obj.at("message").asString() : "Pure Logic violation detected";
                    violation.detected_at = std::chrono::system_clock::now();
                    violation.rule_category = "pure_logic";
                    violation.severity = "MEDIUM";
                    
                    result.violations.push_back(violation);
                }
            }
        }
        
    } catch (const std::exception& e) {
        return createFailureResult("Pure Logic execution failed: " + std::string(e.what()));
    }
    
    return result;
}

// Backward compatibility: Convert datalog to Pure Logic expression
std::string RuleExecutor::convertDatalogToPureLogic(const std::string& datalog_rule) {
    // Simple conversion from datalog to Pure Logic syntax
    // This is a temporary bridge during migration
    
    std::string pure_logic = datalog_rule;
    
    // Convert basic datalog patterns to Pure Logic expressions
    std::regex violation_pattern(R"((\w+)_violation\(([^)]+)\)\s*:-\s*(.+)\.)");
    std::regex compliant_pattern(R"((\w+)_compliant\(([^)]+)\)\s*:-\s*(.+)\.)");
    
    std::smatch match;
    if (std::regex_search(datalog_rule, match, violation_pattern)) {
        // Convert violation rule to Pure Logic
        std::string rule_type = match[1].str();
        std::string file_var = match[2].str();
        std::string conditions = match[3].str();
        
        pure_logic = "forall(" + file_var + ", " + 
                     "implies(and(file_exists(" + file_var + "), " + conditions + "), " +
                     "not(" + rule_type + "_compliant(" + file_var + "))))";
    } else if (std::regex_search(datalog_rule, match, compliant_pattern)) {
        // Convert compliance rule to Pure Logic
        std::string rule_type = match[1].str();
        std::string file_var = match[2].str();
        std::string conditions = match[3].str();
        
        pure_logic = "forall(" + file_var + ", " +
                     "implies(file_exists(" + file_var + "), " +
                     "equals(" + rule_type + "_compliant(" + file_var + "), " + conditions + ")))";
    }
    
    return pure_logic;
}

void RuleExecutor::registerRuleHandler(const std::string& rule_pattern,
                                     std::function<ExecutionResult(const loader::Rule&, const RuleExecutionContext&)> handler) {
    rule_handlers_[rule_pattern] = handler;
}

void RuleExecutor::setExecutionStrategy(ExecutionStrategy strategy) {
    strategy_ = strategy;
}

void RuleExecutor::setParallelExecution(bool enabled, size_t max_threads) {
    parallel_execution_enabled_ = enabled;
    if (max_threads > 0) {
        max_threads_ = max_threads;
    }
}

const RuleExecutor::ExecutionStats& RuleExecutor::getExecutionStats() const {
    return stats_;
}

void RuleExecutor::clearStats() {
    stats_ = ExecutionStats{};
}

// Category-specific rule execution implementations
RuleExecutor::ExecutionResult RuleExecutor::executeStructureRule(const loader::Rule& rule, 
                                                                const RuleExecutionContext& context) {
    ExecutionResult result = createSuccessResult();
    
    if (rule.target == "class_separation") {
        // Check for one class per file
        for (const auto& file_path : context.discovered_files) {
            if (context.file_types.at(file_path) == "source") {
                std::ifstream file(file_path);
                if (!file.is_open()) continue;
                
                std::string line;
                size_t line_number = 1;
                int class_count = 0;
                
                while (std::getline(file, line)) {
                    if (line.find("class ") != std::string::npos || 
                        line.find("struct ") != std::string::npos) {
                        class_count++;
                        if (class_count > 1) {
                            auto violation = createViolation(rule, file_path, line_number,
                                "Multiple classes found in single file",
                                "Move additional classes to separate files");
                            result.violations.push_back(violation);
                            break;
                        }
                    }
                    line_number++;
                }
                
                result.files_processed++;
                result.lines_processed += line_number;
            }
        }
    } else if (rule.target == "file_organization") {
        // Check for proper file organization
        for (const auto& file_path : context.discovered_files) {
            std::filesystem::path path(file_path);
            std::string extension = path.extension().string();
            
            // Check if header files are in proper locations
            if (extension == ".hpp" || extension == ".h") {
                if (file_path.find("/include/") == std::string::npos && 
                    file_path.find("/headers/") == std::string::npos) {
                    auto violation = createViolation(rule, file_path, 1,
                        "Header file not in proper include directory",
                        "Move header files to include/ or headers/ directory");
                    result.violations.push_back(violation);
                }
            }
        }
    }
    
    return result;
}

RuleExecutor::ExecutionResult RuleExecutor::executeInterfaceRule(const loader::Rule& rule, 
                                                               const RuleExecutionContext& context) {
    ExecutionResult result = createSuccessResult();
    
    if (rule.target == "command_parity") {
        // Check for CLI/API command parity
        // This would require more sophisticated analysis
    } else if (rule.target == "parameter_consistency") {
        // Check for consistent parameter naming
        // Simplified implementation
    }
    
    return result;
}

RuleExecutor::ExecutionResult RuleExecutor::executeLanguageRule(const loader::Rule& rule, 
                                                              const RuleExecutionContext& context) {
    ExecutionResult result = createSuccessResult();
    
    if (rule.target == "single_primary_lang") {
        // Check for single primary language
        std::map<std::string, int> language_counts;
        
        for (const auto& file_path : context.discovered_files) {
            std::string ext = std::filesystem::path(file_path).extension();
            
            if (ext == ".cpp" || ext == ".hpp") language_counts["cpp"]++;
            else if (ext == ".py") language_counts["python"]++;
            else if (ext == ".js") language_counts["javascript"]++;
            else if (ext == ".rs") language_counts["rust"]++;
            else if (ext == ".go") language_counts["go"]++;
        }
        
        int primary_languages = 0;
        for (const auto& [lang, count] : language_counts) {
            if (count > 5) { // Threshold for primary language
                primary_languages++;
            }
        }
        
        if (primary_languages > 1) {
            auto violation = createViolation(rule, context.target_path, 1,
                "Multiple primary languages detected",
                "Choose one primary language for the project");
            result.violations.push_back(violation);
        }
    }
    
    return result;
}

RuleExecutor::ExecutionResult RuleExecutor::executeSecurityRule(const loader::Rule& rule, 
                                                              const RuleExecutionContext& context) {
    ExecutionResult result = createSuccessResult();
    
    if (rule.target == "behavior_definition") {
        // Check for explicit security behavior definition
        bool found_security_config = false;
        
        for (const auto& file_path : context.discovered_files) {
            if (file_path.find("security") != std::string::npos ||
                file_path.find("config") != std::string::npos) {
                found_security_config = true;
                break;
            }
        }
        
        if (!found_security_config) {
            auto violation = createViolation(rule, context.target_path, 1,
                "No explicit security configuration found",
                "Create security configuration files");
            result.violations.push_back(violation);
        }
    }
    
    return result;
}

RuleExecutor::ExecutionResult RuleExecutor::executeTestingRule(const loader::Rule& rule, 
                                                             const RuleExecutionContext& context) {
    ExecutionResult result = createSuccessResult();
    
    if (rule.target == "coverage_enforcement") {
        // Check for test coverage
        size_t source_files = 0;
        size_t test_files = 0;
        
        for (const auto& file_path : context.discovered_files) {
            if (context.file_types.at(file_path) == "source") {
                if (file_path.find("test") != std::string::npos ||
                    file_path.find("spec") != std::string::npos) {
                    test_files++;
                } else {
                    source_files++;
                }
            }
        }
        
        if (source_files > 0 && test_files == 0) {
            auto violation = createViolation(rule, context.target_path, 1,
                "No test files found for source files",
                "Create test files for source code");
            result.violations.push_back(violation);
        }
    }
    
    return result;
}

RuleExecutor::ExecutionResult RuleExecutor::executeBuildRule(const loader::Rule& rule, 
                                                           const RuleExecutionContext& context) {
    ExecutionResult result = createSuccessResult();
    
    if (rule.target == "configuration_modes") {
        // Check for build configuration modes
        bool has_debug_config = false;
        bool has_release_config = false;
        
        for (const auto& file_path : context.discovered_files) {
            std::ifstream file(file_path);
            if (!file.is_open()) continue;
            
            std::string content((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
            
            if (content.find("DEBUG") != std::string::npos || 
                content.find("debug") != std::string::npos) {
                has_debug_config = true;
            }
            if (content.find("RELEASE") != std::string::npos || 
                content.find("release") != std::string::npos) {
                has_release_config = true;
            }
        }
        
        if (!has_debug_config || !has_release_config) {
            auto violation = createViolation(rule, context.target_path, 1,
                "Missing build configuration modes",
                "Define both debug and release build configurations");
            result.violations.push_back(violation);
        }
    }
    
    return result;
}

RuleExecutor::ExecutionResult RuleExecutor::executeDocumentationRule(const loader::Rule& rule, 
                                                                    const RuleExecutionContext& context) {
    ExecutionResult result = createSuccessResult();
    
    if (rule.target == "auto_generation") {
        // Check for documentation generation setup
        bool has_doc_config = false;
        
        for (const auto& file_path : context.discovered_files) {
            if (file_path.find("Doxyfile") != std::string::npos ||
                file_path.find("docs") != std::string::npos ||
                file_path.find("README") != std::string::npos) {
                has_doc_config = true;
                break;
            }
        }
        
        if (!has_doc_config) {
            auto violation = createViolation(rule, context.target_path, 1,
                "No documentation generation setup found",
                "Set up automatic documentation generation");
            result.violations.push_back(violation);
        }
    }
    
    return result;
}

RuleExecutor::ExecutionResult RuleExecutor::executeAutomationRule(const loader::Rule& rule, 
                                                                 const RuleExecutionContext& context) {
    return createSuccessResult(); // Simplified
}

RuleExecutor::ExecutionResult RuleExecutor::executeMeasurementRule(const loader::Rule& rule, 
                                                                  const RuleExecutionContext& context) {
    return createSuccessResult(); // Simplified
}

RuleExecutor::ExecutionResult RuleExecutor::executeValidationRule(const loader::Rule& rule, 
                                                                 const RuleExecutionContext& context) {
    return createSuccessResult(); // Simplified
}

RuleExecutor::ExecutionResult RuleExecutor::executeVisualizationRule(const loader::Rule& rule, 
                                                                    const RuleExecutionContext& context) {
    return createSuccessResult(); // Simplified
}

// Helper methods
bool RuleExecutor::isRulePatternMatch(const std::string& pattern, const std::string& rule_id) {
    if (pattern.back() == '*') {
        std::string prefix = pattern.substr(0, pattern.length() - 1);
        return rule_id.substr(0, prefix.length()) == prefix;
    }
    return pattern == rule_id;
}

RuleExecutor::ExecutionResult RuleExecutor::createSuccessResult() {
    ExecutionResult result;
    result.success = true;
    result.execution_time_seconds = 0.0;
    result.files_processed = 0;
    result.lines_processed = 0;
    return result;
}

RuleExecutor::ExecutionResult RuleExecutor::createFailureResult(const std::string& error_message) {
    ExecutionResult result;
    result.success = false;
    result.error_message = error_message;
    result.execution_time_seconds = 0.0;
    result.files_processed = 0;
    result.lines_processed = 0;
    return result;
}

void RuleExecutor::updateStats(const ExecutionResult& result, const std::string& category) {
    stats_.total_rules_executed++;
    if (result.success) {
        stats_.successful_executions++;
    } else {
        stats_.failed_executions++;
    }
    stats_.total_execution_time += result.execution_time_seconds;
    stats_.category_execution_counts[category]++;
    stats_.category_execution_times[category] += result.execution_time_seconds;
}

engine::validator::Violation RuleExecutor::createViolation(const loader::Rule& rule,
                                                          const std::string& file_path,
                                                          size_t line_number,
                                                          const std::string& message,
                                                          const std::string& suggestion) {
    engine::validator::Violation violation;
    violation.id = generateViolationId(rule.id, file_path, line_number);
    violation.rule_id = rule.id;
    violation.rule_name = rule.name;
    violation.rule_category = rule.category;
    violation.philosophy_id = rule.philosophies.empty() ? "" : rule.philosophies[0];
    violation.file_path = file_path;
    violation.line_number = line_number;
    violation.column_number = 1;
    violation.message = message;
    violation.suggestion = suggestion;
    violation.severity = rule.severity;
    violation.auto_fix_available = rule.auto_fix;
    violation.detected_at = std::chrono::system_clock::now();
    
    return violation;
}

std::string RuleExecutor::generateViolationId(const std::string& rule_id,
                                             const std::string& file_path,
                                             size_t line_number) {
    std::string filename = std::filesystem::path(file_path).filename();
    return rule_id + ":violation:" + filename + ":" + std::to_string(line_number);
}

} // namespace akao::core::rule::executor
