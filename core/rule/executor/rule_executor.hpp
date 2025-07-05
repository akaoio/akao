#pragma once

#include "../loader/rule_loader.hpp"
#include "../../engine/validator/validation_result.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace akao::core::rule::executor {

/**
 * @brief Execution context for rule processing
 * Contains all necessary information for rule execution
 */
struct RuleExecutionContext {
    std::string target_path;
    std::map<std::string, std::string> project_metadata;
    std::vector<std::string> discovered_files;
    std::map<std::string, std::string> file_types;
    std::map<std::string, std::string> file_contents_cache;
    
    // Project type information
    std::string project_type;
    std::vector<std::string> languages;
    std::vector<std::string> frameworks;
    
    // Configuration
    bool strict_mode = false;
    bool auto_fix_enabled = false;
    std::vector<std::string> excluded_paths;
};

/**
 * @brief Core rule execution engine for Akao framework
 * 
 * Implements the universal rule executor that can process any type of rule
 * against any type of project. Supports Datalog/Prolog logic queries and
 * structured rule execution patterns.
 * 
 * Philosophy compliance:
 * - akao:philosophy:rule:governance:v1 - Rules govern other rules
 * - akao:philosophy:validation:universal:v1 - Universal validation capability
 * - akao:philosophy:language:isolation:v1 - Language-agnostic execution
 */
class RuleExecutor {
public:
    /**
     * @brief Rule execution result
     */
    struct ExecutionResult {
        bool success;
        std::vector<engine::validator::Violation> violations;
        std::string error_message;
        double execution_time_seconds;
        size_t files_processed;
        size_t lines_processed;
    };

    /**
     * @brief Rule execution strategy
     */
    enum class ExecutionStrategy {
        SEQUENTIAL,    // Execute rules one by one
        PARALLEL,      // Execute rules in parallel (when safe)
        OPTIMIZED,     // Smart execution ordering based on dependencies
        DATALOG_ONLY   // Only execute Datalog/Prolog queries
    };

    /**
     * @brief Constructor
     */
    RuleExecutor();

    /**
     * @brief Initialize the executor
     */
    bool initialize();

    /**
     * @brief Execute a single rule against the given context
     * @param rule The rule to execute
     * @param context The execution context
     * @return Execution result with violations found
     */
    ExecutionResult executeRule(const loader::Rule& rule, const RuleExecutionContext& context);

    /**
     * @brief Execute multiple rules against the given context
     * @param rules The rules to execute
     * @param context The execution context
     * @param strategy The execution strategy to use
     * @return Combined execution result
     */
    ExecutionResult executeRules(const std::vector<std::shared_ptr<loader::Rule>>& rules,
                                const RuleExecutionContext& context,
                                ExecutionStrategy strategy = ExecutionStrategy::SEQUENTIAL);

    /**
     * @brief Execute all rules in a category
     * @param category The rule category (e.g., "structure", "interface")
     * @param context The execution context
     * @return Execution result
     */
    ExecutionResult executeCategory(const std::string& category, const RuleExecutionContext& context);

    /**
     * @brief Execute a Datalog/Prolog query
     * @param query The Datalog query string
     * @param context The execution context
     * @return Query result with violations
     */
    ExecutionResult executeDatalogQuery(const std::string& query, const RuleExecutionContext& context);

    /**
     * @brief Register a custom rule handler
     * @param rule_pattern The rule pattern (e.g., "structure:*", "interface:command_parity")
     * @param handler The handler function
     */
    void registerRuleHandler(const std::string& rule_pattern,
                           std::function<ExecutionResult(const loader::Rule&, const RuleExecutionContext&)> handler);

    /**
     * @brief Set execution strategy
     */
    void setExecutionStrategy(ExecutionStrategy strategy);

    /**
     * @brief Enable/disable parallel execution
     */
    void setParallelExecution(bool enabled, size_t max_threads = 0);

    /**
     * @brief Get execution statistics
     */
    struct ExecutionStats {
        size_t total_rules_executed = 0;
        size_t successful_executions = 0;
        size_t failed_executions = 0;
        double total_execution_time = 0.0;
        std::map<std::string, size_t> category_execution_counts;
        std::map<std::string, double> category_execution_times;
    };

    const ExecutionStats& getExecutionStats() const;

    /**
     * @brief Clear execution statistics
     */
    void clearStats();

private:
    // Execution strategy
    ExecutionStrategy strategy_;
    bool parallel_execution_enabled_;
    size_t max_threads_;

    // Rule handlers
    std::map<std::string, std::function<ExecutionResult(const loader::Rule&, const RuleExecutionContext&)>> rule_handlers_;

    // Statistics
    ExecutionStats stats_;

    // Datalog/Prolog engine
    std::unique_ptr<class DatalogEngine> datalog_engine_;

    // Core execution methods
    ExecutionResult executeStructureRule(const loader::Rule& rule, const RuleExecutionContext& context);
    ExecutionResult executeInterfaceRule(const loader::Rule& rule, const RuleExecutionContext& context);
    ExecutionResult executeLanguageRule(const loader::Rule& rule, const RuleExecutionContext& context);
    ExecutionResult executeSecurityRule(const loader::Rule& rule, const RuleExecutionContext& context);
    ExecutionResult executeTestingRule(const loader::Rule& rule, const RuleExecutionContext& context);
    ExecutionResult executeBuildRule(const loader::Rule& rule, const RuleExecutionContext& context);
    ExecutionResult executeDocumentationRule(const loader::Rule& rule, const RuleExecutionContext& context);
    ExecutionResult executeAutomationRule(const loader::Rule& rule, const RuleExecutionContext& context);
    ExecutionResult executeMeasurementRule(const loader::Rule& rule, const RuleExecutionContext& context);
    ExecutionResult executeValidationRule(const loader::Rule& rule, const RuleExecutionContext& context);
    ExecutionResult executeVisualizationRule(const loader::Rule& rule, const RuleExecutionContext& context);

    // Helper methods
    bool isRulePatternMatch(const std::string& pattern, const std::string& rule_id);
    ExecutionResult createSuccessResult();
    ExecutionResult createFailureResult(const std::string& error_message);
    void updateStats(const ExecutionResult& result, const std::string& category);
    
    // File analysis helpers
    std::vector<std::string> extractFunctionNames(const std::string& file_content, const std::string& language);
    std::vector<std::string> extractClassNames(const std::string& file_content, const std::string& language);
    std::map<std::string, std::string> extractMetadata(const std::string& file_path);
    bool checkFileStructure(const std::string& file_path, const std::vector<std::string>& required_patterns);
    
    // Violation creation helpers
    engine::validator::Violation createViolation(const loader::Rule& rule,
                                                const std::string& file_path,
                                                size_t line_number,
                                                const std::string& message,
                                                const std::string& suggestion = "");
    
    std::string generateViolationId(const std::string& rule_id,
                                   const std::string& file_path,
                                   size_t line_number);
};

/**
 * @brief Simple Datalog/Prolog query engine for rule execution
 * 
 * Implements basic logical query processing for rules that use
 * formal logic expressions.
 */
class DatalogEngine {
public:
    DatalogEngine();
    ~DatalogEngine();

    /**
     * @brief Initialize the engine with facts from context
     */
    bool initialize(const RuleExecutionContext& context);

    /**
     * @brief Execute a Datalog query
     * @param query The query string
     * @return Query results
     */
    struct QueryResult {
        bool success;
        std::vector<std::map<std::string, std::string>> bindings;
        std::string error_message;
    };

    QueryResult executeQuery(const std::string& query);

    /**
     * @brief Add a fact to the knowledge base
     */
    void addFact(const std::string& predicate, const std::vector<std::string>& arguments);

    /**
     * @brief Add a rule to the knowledge base
     */
    void addRule(const std::string& head, const std::string& body);

    /**
     * @brief Clear all facts and rules
     */
    void clear();

private:
    // Simple in-memory knowledge base
    std::vector<std::string> facts_;
    std::vector<std::string> rules_;
    
    // Query processing
    QueryResult processFact(const std::string& query);
    QueryResult processRule(const std::string& query);
    bool unify(const std::string& pattern, const std::string& fact);
    std::vector<std::map<std::string, std::string>> findMatches(const std::string& pattern);
};

} // namespace akao::core::rule::executor
