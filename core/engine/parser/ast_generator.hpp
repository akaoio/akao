#pragma once

/**
 * @file ast_generator.hpp
 * @brief AST generator header for unified AST creation and semantic normalization
 * @details Header file for converting language-specific ASTs to unified AST with semantic normalization
 * 
 * ID: akao:component:core:engine:parser:ast-generator:v1
 * Scope: Unified AST generation and semantic normalization across languages
 * Dependencies: universal_parser.hpp
 */

#include <string>
#include <vector>
#include <map>
#include <memory>

// Forward declarations from universal_parser.hpp
namespace akao {
namespace core {
namespace engine {
namespace parser {
    class UnifiedASTNode;
    class IdentifierNode;
    class FunctionDeclarationNode;
    class VariableDeclarationNode;
    class ClassDeclarationNode;
    struct UnifiedAST;
    struct LanguageAST;
    struct ASTGeneratorOptions;
    enum class LanguageType;
}
}
}
}

namespace akao {
namespace core {
namespace engine {
namespace parser {

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
    
    // Language-specific AST conversion methods
    std::unique_ptr<UnifiedASTNode> convert_language_ast(const std::unique_ptr<LanguageAST>& language_ast);
    std::unique_ptr<UnifiedASTNode> convert_c_ast(LanguageAST* language_ast);
    std::unique_ptr<UnifiedASTNode> convert_cpp_ast(LanguageAST* language_ast);
    std::unique_ptr<UnifiedASTNode> convert_python_ast(LanguageAST* language_ast);
    std::unique_ptr<UnifiedASTNode> convert_javascript_ast(LanguageAST* language_ast);
    std::unique_ptr<UnifiedASTNode> convert_yaml_ast(LanguageAST* language_ast);
    std::unique_ptr<UnifiedASTNode> convert_json_ast(LanguageAST* language_ast);
    std::unique_ptr<UnifiedASTNode> convert_prolog_ast(LanguageAST* language_ast);
    std::unique_ptr<UnifiedASTNode> convert_pure_logic_ast(LanguageAST* language_ast);
    std::unique_ptr<UnifiedASTNode> create_unknown_ast(LanguageAST* language_ast);
    
    // AST normalization methods
    void normalize_ast(UnifiedASTNode* node);
    void normalize_node(UnifiedASTNode* node);
    void normalize_identifier(IdentifierNode* node);
    void normalize_function_declaration(FunctionDeclarationNode* node);
    void normalize_variable_declaration(VariableDeclarationNode* node);
    void normalize_class_declaration(ClassDeclarationNode* node);
    
    // Utility methods
    std::string normalize_identifier_name(const std::string& name);
    std::string normalize_type_name(const std::string& type);
    
    // AST optimization methods
    void optimize_ast(UnifiedASTNode* node);
    void optimize_node(UnifiedASTNode* node);
    void optimize_expression(UnifiedASTNode* node);
    void optimize_statement(UnifiedASTNode* node);
    void optimize_block_statement(UnifiedASTNode* node);
    
    // Node merging methods
    void merge_equivalent_children(UnifiedASTNode* node);
    bool are_nodes_equivalent(UnifiedASTNode* node1, UnifiedASTNode* node2);
    void merge_nodes(UnifiedASTNode* target, UnifiedASTNode* source);
};

} // namespace parser
} // namespace engine
} // namespace core
} // namespace akao
