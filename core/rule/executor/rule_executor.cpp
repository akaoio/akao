#include "rule_executor.hpp"
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
    datalog_engine_ = std::make_unique<DatalogEngine>();
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
        
        // If no specific handler found, try Datalog execution
        if (result.violations.empty() && !rule.datalog_rules.empty()) {
            for (const auto& datalog_rule : rule.datalog_rules) {
                auto datalog_result = executeDatalogQuery(datalog_rule, context);
                result.violations.insert(result.violations.end(), 
                                       datalog_result.violations.begin(), 
                                       datalog_result.violations.end());
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

RuleExecutor::ExecutionResult RuleExecutor::executeDatalogQuery(const std::string& query, 
                                                               const RuleExecutionContext& context) {
    ExecutionResult result = createSuccessResult();
    
    if (!datalog_engine_->initialize(context)) {
        return createFailureResult("Failed to initialize Datalog engine");
    }
    
    auto query_result = datalog_engine_->executeQuery(query);
    
    if (!query_result.success) {
        return createFailureResult("Datalog query failed: " + query_result.error_message);
    }
    
    // Convert query results to violations
    for (const auto& binding : query_result.bindings) {
        auto it_file = binding.find("file");
        auto it_line = binding.find("line");
        auto it_message = binding.find("message");
        
        if (it_file != binding.end() && it_message != binding.end()) {
            engine::validator::Violation violation;
            violation.file_path = it_file->second;
            violation.line_number = it_line != binding.end() ? std::stoul(it_line->second) : 1;
            violation.message = it_message->second;
            violation.detected_at = std::chrono::system_clock::now();
            
            result.violations.push_back(violation);
        }
    }
    
    return result;
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

// DatalogEngine implementation
DatalogEngine::DatalogEngine() {}

DatalogEngine::~DatalogEngine() {}

bool DatalogEngine::initialize(const RuleExecutionContext& context) {
    clear();
    
    // Add facts about the project
    addFact("project", {context.target_path, context.project_type});
    
    // Add facts about files
    for (const auto& file_path : context.discovered_files) {
        std::string filename = std::filesystem::path(file_path).filename();
        std::string extension = std::filesystem::path(file_path).extension();
        
        addFact("file", {filename, file_path, extension});
        
        auto file_type_it = context.file_types.find(file_path);
        if (file_type_it != context.file_types.end()) {
            addFact("file_type", {filename, file_type_it->second});
        }
    }
    
    return true;
}

DatalogEngine::QueryResult DatalogEngine::executeQuery(const std::string& query) {
    QueryResult result;
    result.success = true;
    
    // Simplified query processing
    // In a real implementation, this would be a full Datalog/Prolog parser and evaluator
    
    if (query.find("file(") != std::string::npos) {
        result = processFact(query);
    } else {
        result = processRule(query);
    }
    
    return result;
}

void DatalogEngine::addFact(const std::string& predicate, const std::vector<std::string>& arguments) {
    std::string fact = predicate + "(";
    for (size_t i = 0; i < arguments.size(); ++i) {
        if (i > 0) fact += ", ";
        fact += "\"" + arguments[i] + "\"";
    }
    fact += ")";
    facts_.push_back(fact);
}

void DatalogEngine::addRule(const std::string& head, const std::string& body) {
    rules_.push_back(head + " :- " + body);
}

void DatalogEngine::clear() {
    facts_.clear();
    rules_.clear();
}

DatalogEngine::QueryResult DatalogEngine::processFact(const std::string& query) {
    QueryResult result;
    result.success = true;
    
    for (const auto& fact : facts_) {
        if (unify(query, fact)) {
            std::map<std::string, std::string> binding;
            binding["match"] = fact;
            result.bindings.push_back(binding);
        }
    }
    
    return result;
}

DatalogEngine::QueryResult DatalogEngine::processRule(const std::string& query) {
    QueryResult result;
    result.success = true;
    // Simplified rule processing
    return result;
}

bool DatalogEngine::unify(const std::string& pattern, const std::string& fact) {
    // Simplified unification - just check if fact contains pattern elements
    return fact.find(pattern.substr(0, pattern.find('('))) != std::string::npos;
}

std::vector<std::map<std::string, std::string>> DatalogEngine::findMatches(const std::string& pattern) {
    std::vector<std::map<std::string, std::string>> matches;
    
    for (const auto& fact : facts_) {
        if (unify(pattern, fact)) {
            std::map<std::string, std::string> match;
            match["fact"] = fact;
            matches.push_back(match);
        }
    }
    
    return matches;
}

} // namespace akao::core::rule::executor
