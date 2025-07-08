#pragma once

/**
 * @file universal_parser.hpp
 * @brief Universal parser header with zero-dependency architecture
 * @details Header file for unified parsing interface with language-specific parsers and AST generation
 * 
 * ID: akao:component:core:engine:parser:universal-parser:v1
 * Scope: Multi-language parsing with unified AST generation and semantic normalization
 * Dependencies: language_detectors.hpp, ast_generator.hpp
 */

#include "language_detectors.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>

namespace akao {
namespace core {
namespace engine {
namespace parser {

// Forward declarations
class ASTGenerator;
class UnifiedASTNode;
class SymbolTable;
class TypeChecker;
class DependencyAnalyzer;

/**
 * @brief Unified AST node types
 */
enum class UnifiedNodeType {
    PROGRAM,
    MODULE,
    FUNCTION_DECLARATION,
    VARIABLE_DECLARATION,
    CLASS_DECLARATION,
    INTERFACE_DECLARATION,
    EXPRESSION,
    STATEMENT,
    IDENTIFIER,
    LITERAL,
    BINARY_OPERATION,
    UNARY_OPERATION,
    FUNCTION_CALL,
    MEMBER_ACCESS,
    ARRAY_ACCESS,
    ASSIGNMENT,
    IF_STATEMENT,
    LOOP_STATEMENT,
    RETURN_STATEMENT,
    BLOCK_STATEMENT,
    COMMENT
};

/**
 * @brief Base unified AST node
 */
class UnifiedASTNode {
public:
    UnifiedNodeType type;
    std::string source_location;
    int line_number = 0;
    int column_number = 0;
    std::vector<std::unique_ptr<UnifiedASTNode>> children;
    std::map<std::string, std::string> attributes;
    
    virtual ~UnifiedASTNode() = default;
    virtual std::string to_string() const = 0;
};

/**
 * @brief Specific AST node types
 */
class ProgramNode : public UnifiedASTNode {
public:
    std::string name;
    std::vector<std::string> imports;
    std::string to_string() const override;
};

class FunctionDeclarationNode : public UnifiedASTNode {
public:
    std::string name;
    std::vector<std::string> parameters;
    std::string return_type;
    std::unique_ptr<UnifiedASTNode> body;
    std::string to_string() const override;
};

class VariableDeclarationNode : public UnifiedASTNode {
public:
    std::string name;
    std::string type;
    std::unique_ptr<UnifiedASTNode> initializer;
    std::string to_string() const override;
};

class ClassDeclarationNode : public UnifiedASTNode {
public:
    std::string name;
    std::vector<std::string> base_classes;
    std::vector<std::unique_ptr<UnifiedASTNode>> members;
    std::string to_string() const override;
};

class IdentifierNode : public UnifiedASTNode {
public:
    std::string name;
    UnifiedASTNode* resolved_symbol = nullptr;
    std::string to_string() const override;
};

class FunctionCallNode : public UnifiedASTNode {
public:
    std::string function_name;
    std::vector<std::unique_ptr<UnifiedASTNode>> arguments;
    UnifiedASTNode* resolved_function = nullptr;
    std::string to_string() const override;
};

/**
 * @brief Unified AST structure
 */
struct UnifiedAST {
    std::unique_ptr<UnifiedASTNode> root;
    LanguageType source_language;
    std::string source_name;
    bool is_valid = false;
    std::string error_message;
    std::map<std::string, std::string> metadata;
};

/**
 * @brief Language-specific AST (placeholder for language parsers)
 */
struct LanguageAST {
    LanguageType language;
    std::unique_ptr<void> ast_root; // Language-specific AST structure
    bool is_valid = false;
    std::string error_message;
    std::map<std::string, std::string> metadata;
};

/**
 * @brief Language parser result
 */
struct LanguageParseResult {
    bool success = false;
    std::unique_ptr<LanguageAST> language_ast;
    std::string error_message;
    int error_line = 0;
    int error_column = 0;
    double parse_time = 0.0;
};

/**
 * @brief Symbol table for semantic analysis
 */
class SymbolTable {
public:
    void add_function(const std::string& name, UnifiedASTNode* node);
    void add_variable(const std::string& name, UnifiedASTNode* node);
    void add_class(const std::string& name, UnifiedASTNode* node);
    
    UnifiedASTNode* lookup(const std::string& name);
    UnifiedASTNode* lookup_function(const std::string& name);
    UnifiedASTNode* lookup_variable(const std::string& name);
    UnifiedASTNode* lookup_class(const std::string& name);
    
    void push_scope();
    void pop_scope();
    
private:
    std::vector<std::map<std::string, UnifiedASTNode*>> scopes_;
    std::map<std::string, UnifiedASTNode*> functions_;
    std::map<std::string, UnifiedASTNode*> classes_;
};

/**
 * @brief Type information
 */
struct TypeInfo {
    std::string type_name;
    bool is_primitive = false;
    bool is_pointer = false;
    bool is_array = false;
    std::vector<std::string> generic_parameters;
    std::map<std::string, std::string> attributes;
};

/**
 * @brief Dependency information
 */
struct DependencyInfo {
    std::string name;
    std::string type; // module, function, class, etc.
    std::string source_location;
    bool is_external = false;
};

/**
 * @brief Semantic analysis results
 */
struct SemanticResult {
    bool success = false;
    std::unique_ptr<SymbolTable> symbol_table;
    std::map<UnifiedASTNode*, TypeInfo> type_info;
    std::vector<DependencyInfo> dependencies;
    std::string error_message;
};

/**
 * @brief Symbol resolution result
 */
struct SymbolResolutionResult {
    bool success = false;
    std::unique_ptr<SymbolTable> symbol_table;
    std::string error_message;
};

/**
 * @brief Type checking result
 */
struct TypeCheckingResult {
    bool success = false;
    std::map<UnifiedASTNode*, TypeInfo> type_info;
    std::string error_message;
};

/**
 * @brief Dependency analysis result
 */
struct DependencyResult {
    bool success = false;
    std::vector<DependencyInfo> dependencies;
    std::string error_message;
};

/**
 * @brief Universal parser result
 */
struct ParseResult {
    bool success = false;
    LanguageType detected_language = LanguageType::UNKNOWN;
    ConfidenceLevel confidence = ConfidenceLevel::LOW;
    UnifiedAST unified_ast;
    SemanticResult semantic_info;
    std::string source_name;
    std::string error_message;
    int error_line = 0;
    int error_column = 0;
    double parse_time = 0.0;
};

/**
 * @brief Parse options
 */
struct ParseOptions {
    bool enable_semantic_analysis = true;
    bool enable_type_checking = true;
    bool preserve_comments = true;
    bool generate_debug_info = false;
    bool strict_mode = false;
    int max_errors = 10;
    std::vector<std::string> include_paths;
    std::map<std::string, std::string> preprocessor_definitions;
};

/**
 * @brief Language-specific statistics
 */
struct LanguageStatistics {
    uint32_t total_files = 0;
    uint32_t successful_files = 0;
    uint32_t failed_files = 0;
    double total_time = 0.0;
    double average_time = 0.0;
};

/**
 * @brief Parser statistics
 */
struct ParserStatistics {
    uint32_t total_files_parsed = 0;
    uint32_t successful_parses = 0;
    uint32_t failed_parses = 0;
    double average_parse_time = 0.0;
    double total_parse_time = 0.0;
    std::map<LanguageType, LanguageStatistics> language_statistics;
};

/**
 * @brief Internal parser statistics
 */
struct InternalStatistics {
    uint32_t total_files_parsed = 0;
    uint32_t successful_parses = 0;
    uint32_t failed_parses = 0;
    double total_parse_time = 0.0;
    std::map<LanguageType, LanguageStatistics> language_stats;
};

/**
 * @brief Abstract base class for language-specific parsers
 */
class LanguageParser {
public:
    virtual ~LanguageParser() = default;
    
    /**
     * @brief Parse content in specific language
     * @param content Source content
     * @param source_name Source identifier
     * @return Language-specific parse result
     */
    virtual LanguageParseResult parse(const std::string& content, 
                                      const std::string& source_name) = 0;
    
    /**
     * @brief Get supported language
     * @return Language type
     */
    virtual LanguageType get_language() const = 0;
    
    /**
     * @brief Get parser name
     * @return Parser name
     */
    virtual std::string get_name() const = 0;
};

/**
 * @brief Language-specific parser implementations
 */
class CParser : public LanguageParser {
public:
    LanguageParseResult parse(const std::string& content, const std::string& source_name) override;
    LanguageType get_language() const override { return LanguageType::C; }
    std::string get_name() const override { return "C Parser"; }
};

class CppParser : public LanguageParser {
public:
    LanguageParseResult parse(const std::string& content, const std::string& source_name) override;
    LanguageType get_language() const override { return LanguageType::CPP; }
    std::string get_name() const override { return "C++ Parser"; }
};

class PythonParser : public LanguageParser {
public:
    LanguageParseResult parse(const std::string& content, const std::string& source_name) override;
    LanguageType get_language() const override { return LanguageType::PYTHON; }
    std::string get_name() const override { return "Python Parser"; }
};

class JavaScriptParser : public LanguageParser {
public:
    LanguageParseResult parse(const std::string& content, const std::string& source_name) override;
    LanguageType get_language() const override { return LanguageType::JAVASCRIPT; }
    std::string get_name() const override { return "JavaScript Parser"; }
};

class YamlParser : public LanguageParser {
public:
    LanguageParseResult parse(const std::string& content, const std::string& source_name) override;
    LanguageType get_language() const override { return LanguageType::YAML; }
    std::string get_name() const override { return "YAML Parser"; }
};

class JsonParser : public LanguageParser {
public:
    LanguageParseResult parse(const std::string& content, const std::string& source_name) override;
    LanguageType get_language() const override { return LanguageType::JSON; }
    std::string get_name() const override { return "JSON Parser"; }
};

class PrologParser : public LanguageParser {
public:
    LanguageParseResult parse(const std::string& content, const std::string& source_name) override;
    LanguageType get_language() const override { return LanguageType::PROLOG; }
    std::string get_name() const override { return "Prolog Parser"; }
};

class PureLogicParser : public LanguageParser {
public:
    LanguageParseResult parse(const std::string& content, const std::string& source_name) override;
    LanguageType get_language() const override { return LanguageType::PURE_LOGIC; }
    std::string get_name() const override { return "Pure Logic Parser"; }
};

/**
 * @brief AST generator options
 */
struct ASTGeneratorOptions {
    bool enable_semantic_analysis = true;
    bool enable_type_checking = true;
    bool preserve_comments = true;
    bool generate_debug_info = false;
    bool normalize_identifiers = true;
    bool merge_equivalent_nodes = true;
};

/**
 * @brief AST generator for unified AST creation
 */
class ASTGenerator {
public:
    ASTGenerator();
    
    /**
     * @brief Generate unified AST from language-specific AST
     * @param language_ast Language-specific AST
     * @param language Source language
     * @return Unified AST
     */
    UnifiedAST generate_unified_ast(const std::unique_ptr<LanguageAST>& language_ast, 
                                    LanguageType language);
    
    /**
     * @brief Set generation options
     * @param options AST generation options
     */
    void set_options(const ASTGeneratorOptions& options);
    
private:
    ASTGeneratorOptions options_;
    
    std::unique_ptr<UnifiedASTNode> convert_language_ast(const std::unique_ptr<LanguageAST>& language_ast);
    void normalize_ast(UnifiedASTNode* node);
    void optimize_ast(UnifiedASTNode* node);
};

/**
 * @brief Type checker for semantic analysis
 */
class TypeChecker {
public:
    explicit TypeChecker(SymbolTable* symbol_table);
    
    /**
     * @brief Check types in AST
     * @param root Root AST node
     * @return Type information map
     */
    std::map<UnifiedASTNode*, TypeInfo> check_types(UnifiedASTNode* root);
    
private:
    SymbolTable* symbol_table_;
    
    TypeInfo infer_type(UnifiedASTNode* node);
    bool are_types_compatible(const TypeInfo& type1, const TypeInfo& type2);
    TypeInfo unify_types(const TypeInfo& type1, const TypeInfo& type2);
};

/**
 * @brief Dependency analyzer
 */
class DependencyAnalyzer {
public:
    DependencyAnalyzer();
    
    /**
     * @brief Analyze dependencies in AST
     * @param root Root AST node
     * @return Dependency information
     */
    std::vector<DependencyInfo> analyze(UnifiedASTNode* root);
    
private:
    void collect_dependencies(UnifiedASTNode* node, std::vector<DependencyInfo>& dependencies);
    bool is_external_dependency(const std::string& name);
};

/**
 * @brief Main universal parser class
 */
class UniversalParser {
public:
    UniversalParser();
    ~UniversalParser();
    
    /**
     * @brief Parse file with automatic language detection
     * @param filepath Path to source file
     * @return Parse result with unified AST
     */
    ParseResult parse_file(const std::string& filepath);
    
    /**
     * @brief Parse content with automatic language detection
     * @param content Source content
     * @param source_name Source identifier for error reporting
     * @return Parse result with unified AST
     */
    ParseResult parse_content(const std::string& content, 
                              const std::string& source_name = "<string>");
    
    /**
     * @brief Parse content with specified language
     * @param content Source content
     * @param language Target language type
     * @param source_name Source identifier
     * @return Parse result with unified AST
     */
    ParseResult parse_with_language(const std::string& content, 
                                    LanguageType language,
                                    const std::string& source_name = "<string>");
    
    /**
     * @brief Get supported languages
     * @return List of supported language types
     */
    std::vector<LanguageType> get_supported_languages() const;
    
    /**
     * @brief Register custom language parser
     * @param language Language type
     * @param parser Parser implementation
     */
    void register_language_parser(LanguageType language, 
                                  std::unique_ptr<LanguageParser> parser);
    
    /**
     * @brief Get parser statistics
     * @return Parsing performance statistics
     */
    ParserStatistics get_statistics() const;
    
    /**
     * @brief Set parse options
     * @param options Parse configuration options
     */
    void set_parse_options(const ParseOptions& options);
    
private:
    bool initialized_;
    std::unique_ptr<UniversalLanguageDetector> language_detector_;
    std::unique_ptr<ASTGenerator> ast_generator_;
    std::map<LanguageType, std::unique_ptr<LanguageParser>> language_parsers_;
    ParseOptions parse_options_;
    InternalStatistics statistics_;
    
    void initialize_parsers();
    std::string load_file_content(const std::string& filepath);
    LanguageParser* get_parser_for_language(LanguageType language);
    
    SemanticResult perform_semantic_normalization(const UnifiedAST& ast);
    SymbolResolutionResult resolve_symbols(const UnifiedAST& ast);
    TypeCheckingResult perform_type_checking(const UnifiedAST& ast, 
                                             const std::unique_ptr<SymbolTable>& symbol_table);
    DependencyResult analyze_dependencies(const UnifiedAST& ast);
    
    void traverse_ast_for_symbols(UnifiedASTNode* node, SymbolTable* symbol_table);
    void resolve_symbol_references(UnifiedASTNode* node, SymbolTable* symbol_table);
    void update_statistics(const ParseResult& result);
};

/**
 * @brief Utility function for creating placeholder AST (for implementation)
 */
std::unique_ptr<LanguageAST> create_placeholder_ast(LanguageType language);

} // namespace parser
} // namespace engine
} // namespace core
} // namespace akao
