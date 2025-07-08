/**
 * @file ast_generator.cpp
 * @brief AST generator implementation for unified AST creation and semantic normalization
 * @details Converts language-specific ASTs to unified AST with semantic normalization
 * 
 * ID: akao:component:core:engine:parser:ast-generator:v1
 * Scope: Unified AST generation and semantic normalization across languages
 * Dependencies: universal_parser.hpp
 */

#include "ast_generator.hpp"
#include "universal_parser.hpp"
#include <iostream>
#include <algorithm>
#include <sstream>

namespace akao {
namespace core {
namespace engine {
namespace parser {

// ASTGenerator Implementation

ASTGenerator::ASTGenerator() {
    // Initialize default options
    options_.enable_semantic_analysis = true;
    options_.enable_type_checking = true;
    options_.preserve_comments = true;
    options_.generate_debug_info = false;
    options_.normalize_identifiers = true;
    options_.merge_equivalent_nodes = true;
}

UnifiedAST ASTGenerator::generate_unified_ast(const std::unique_ptr<LanguageAST>& language_ast, 
                                              LanguageType language) {
    UnifiedAST unified_ast;
    
    try {
        if (!language_ast || !language_ast->is_valid) {
            unified_ast.is_valid = false;
            unified_ast.error_message = "Invalid language AST provided";
            return unified_ast;
        }
        
        // Convert language-specific AST to unified AST
        unified_ast.root = convert_language_ast(language_ast);
        if (!unified_ast.root) {
            unified_ast.is_valid = false;
            unified_ast.error_message = "Failed to convert language AST";
            return unified_ast;
        }
        
        // Set AST metadata
        unified_ast.source_language = language;
        unified_ast.source_name = language_ast->metadata.count("source_name") ? 
                                  language_ast->metadata.at("source_name") : "<unknown>";
        
        // Copy metadata from language AST
        unified_ast.metadata = language_ast->metadata;
        
        // Perform normalization if enabled
        if (options_.normalize_identifiers) {
            normalize_ast(unified_ast.root.get());
        }
        
        // Perform optimization if enabled
        if (options_.merge_equivalent_nodes) {
            optimize_ast(unified_ast.root.get());
        }
        
        unified_ast.is_valid = true;
        
    } catch (const std::exception& e) {
        unified_ast.is_valid = false;
        unified_ast.error_message = "AST generation exception: " + std::string(e.what());
    }
    
    return unified_ast;
}

void ASTGenerator::set_options(const ASTGeneratorOptions& options) {
    options_ = options;
}

// Private Methods

std::unique_ptr<UnifiedASTNode> ASTGenerator::convert_language_ast(const std::unique_ptr<LanguageAST>& language_ast) {
    if (!language_ast) {
        return nullptr;
    }
    
    // Convert based on source language
    switch (language_ast->language) {
        case LanguageType::C:
            return convert_c_ast(language_ast.get());
            
        case LanguageType::CPP:
            return convert_cpp_ast(language_ast.get());
            
        case LanguageType::PYTHON:
            return convert_python_ast(language_ast.get());
            
        case LanguageType::JAVASCRIPT:
            return convert_javascript_ast(language_ast.get());
            
        case LanguageType::YAML:
            return convert_yaml_ast(language_ast.get());
            
        case LanguageType::JSON:
            return convert_json_ast(language_ast.get());
            
        case LanguageType::PROLOG:
            return convert_prolog_ast(language_ast.get());
            
        case LanguageType::PURE_LOGIC:
            return convert_pure_logic_ast(language_ast.get());
            
        default:
            return create_unknown_ast(language_ast.get());
    }
}

std::unique_ptr<UnifiedASTNode> ASTGenerator::convert_c_ast(LanguageAST* language_ast) {
    // Placeholder implementation for C AST conversion
    auto program = std::make_unique<ProgramNode>();
    program->type = UnifiedNodeType::PROGRAM;
    program->name = "C_Program";
    
    // Would parse C-specific AST structure and convert to unified nodes
    // This is a simplified placeholder implementation
    
    return std::move(program);
}

std::unique_ptr<UnifiedASTNode> ASTGenerator::convert_cpp_ast(LanguageAST* language_ast) {
    // Placeholder implementation for C++ AST conversion
    auto program = std::make_unique<ProgramNode>();
    program->type = UnifiedNodeType::PROGRAM;
    program->name = "CPP_Program";
    
    // Would parse C++-specific AST structure including:
    // - Class declarations with inheritance
    // - Template definitions and instantiations
    // - Namespace declarations
    // - Modern C++ features (auto, lambda, etc.)
    
    return std::move(program);
}

std::unique_ptr<UnifiedASTNode> ASTGenerator::convert_python_ast(LanguageAST* language_ast) {
    // Placeholder implementation for Python AST conversion
    auto program = std::make_unique<ProgramNode>();
    program->type = UnifiedNodeType::PROGRAM;
    program->name = "Python_Program";
    
    // Would parse Python-specific AST structure including:
    // - Indentation-based structure
    // - Dynamic typing information
    // - Import statements
    // - Decorator patterns
    // - Generator and comprehension expressions
    
    return std::move(program);
}

std::unique_ptr<UnifiedASTNode> ASTGenerator::convert_javascript_ast(LanguageAST* language_ast) {
    // Placeholder implementation for JavaScript AST conversion
    auto program = std::make_unique<ProgramNode>();
    program->type = UnifiedNodeType::PROGRAM;
    program->name = "JavaScript_Program";
    
    // Would parse JavaScript-specific AST structure including:
    // - Prototype-based inheritance
    // - Closure patterns
    // - Async/await constructs
    // - Module systems (CommonJS, ES6)
    // - JSX syntax (if applicable)
    
    return std::move(program);
}

std::unique_ptr<UnifiedASTNode> ASTGenerator::convert_yaml_ast(LanguageAST* language_ast) {
    // Placeholder implementation for YAML AST conversion
    auto program = std::make_unique<ProgramNode>();
    program->type = UnifiedNodeType::PROGRAM;
    program->name = "YAML_Document";
    
    // Would parse YAML-specific structure including:
    // - Multi-document format
    // - Anchor and alias references
    // - Custom tags
    // - Nested mapping and sequence structures
    
    return std::move(program);
}

std::unique_ptr<UnifiedASTNode> ASTGenerator::convert_json_ast(LanguageAST* language_ast) {
    // Placeholder implementation for JSON AST conversion
    auto program = std::make_unique<ProgramNode>();
    program->type = UnifiedNodeType::PROGRAM;
    program->name = "JSON_Document";
    
    // Would parse JSON-specific structure including:
    // - Object and array structures
    // - Primitive value types
    // - Nested data structures
    // - JSON5 extensions (if applicable)
    
    return std::move(program);
}

std::unique_ptr<UnifiedASTNode> ASTGenerator::convert_prolog_ast(LanguageAST* language_ast) {
    // Placeholder implementation for Prolog AST conversion
    auto program = std::make_unique<ProgramNode>();
    program->type = UnifiedNodeType::PROGRAM;
    program->name = "Prolog_Program";
    
    // Would parse Prolog-specific structure including:
    // - Fact declarations
    // - Rule definitions with head and body
    // - Query structures
    // - Cut and negation operations
    // - Module system
    
    return std::move(program);
}

std::unique_ptr<UnifiedASTNode> ASTGenerator::convert_pure_logic_ast(LanguageAST* language_ast) {
    // Implementation for Pure Logic AST conversion
    auto program = std::make_unique<ProgramNode>();
    program->type = UnifiedNodeType::PROGRAM;
    program->name = "PureLogic_Program";
    
    // Convert Pure Logic specific structures including:
    // - Logic symbols and operators
    // - Dual-mode syntax (symbolic and textual)
    // - Python-like readability features
    // - Accessibility constructs
    
    return std::move(program);
}

std::unique_ptr<UnifiedASTNode> ASTGenerator::create_unknown_ast(LanguageAST* language_ast) {
    // Fallback for unknown language types
    auto program = std::make_unique<ProgramNode>();
    program->type = UnifiedNodeType::PROGRAM;
    program->name = "Unknown_Program";
    
    return std::move(program);
}

void ASTGenerator::normalize_ast(UnifiedASTNode* node) {
    if (!node) return;
    
    // Normalize current node
    normalize_node(node);
    
    // Recursively normalize children
    for (auto& child : node->children) {
        normalize_ast(child.get());
    }
}

void ASTGenerator::normalize_node(UnifiedASTNode* node) {
    if (!node) return;
    
    switch (node->type) {
        case UnifiedNodeType::IDENTIFIER:
            normalize_identifier(static_cast<IdentifierNode*>(node));
            break;
            
        case UnifiedNodeType::FUNCTION_DECLARATION:
            normalize_function_declaration(static_cast<FunctionDeclarationNode*>(node));
            break;
            
        case UnifiedNodeType::VARIABLE_DECLARATION:
            normalize_variable_declaration(static_cast<VariableDeclarationNode*>(node));
            break;
            
        case UnifiedNodeType::CLASS_DECLARATION:
            normalize_class_declaration(static_cast<ClassDeclarationNode*>(node));
            break;
            
        default:
            // No specific normalization needed
            break;
    }
}

void ASTGenerator::normalize_identifier(IdentifierNode* node) {
    if (!node) return;
    
    // Normalize identifier names for consistency across languages
    if (options_.normalize_identifiers) {
        // Convert to consistent naming convention (e.g., snake_case)
        node->name = normalize_identifier_name(node->name);
    }
}

void ASTGenerator::normalize_function_declaration(FunctionDeclarationNode* node) {
    if (!node) return;
    
    // Normalize function names and parameters
    if (options_.normalize_identifiers) {
        node->name = normalize_identifier_name(node->name);
        
        for (auto& param : node->parameters) {
            param = normalize_identifier_name(param);
        }
    }
    
    // Standardize return type representation
    node->return_type = normalize_type_name(node->return_type);
}

void ASTGenerator::normalize_variable_declaration(VariableDeclarationNode* node) {
    if (!node) return;
    
    // Normalize variable names
    if (options_.normalize_identifiers) {
        node->name = normalize_identifier_name(node->name);
    }
    
    // Standardize type representation
    node->type = normalize_type_name(node->type);
}

void ASTGenerator::normalize_class_declaration(ClassDeclarationNode* node) {
    if (!node) return;
    
    // Normalize class names
    if (options_.normalize_identifiers) {
        node->name = normalize_identifier_name(node->name);
        
        for (auto& base_class : node->base_classes) {
            base_class = normalize_identifier_name(base_class);
        }
    }
}

std::string ASTGenerator::normalize_identifier_name(const std::string& name) {
    // Convert various naming conventions to snake_case
    std::string result;
    bool prev_was_upper = false;
    
    for (size_t i = 0; i < name.length(); ++i) {
        char c = name[i];
        
        if (std::isupper(c)) {
            if (i > 0 && !prev_was_upper) {
                result += '_';
            }
            result += std::tolower(c);
            prev_was_upper = true;
        } else {
            result += c;
            prev_was_upper = false;
        }
    }
    
    return result;
}

std::string ASTGenerator::normalize_type_name(const std::string& type) {
    // Map language-specific types to unified type names
    static const std::map<std::string, std::string> type_mapping = {
        // C/C++ to unified
        {"int", "integer"},
        {"char", "character"},
        {"float", "float32"},
        {"double", "float64"},
        {"bool", "boolean"},
        {"void", "void"},
        
        // Python to unified
        {"str", "string"},
        {"int", "integer"},
        {"float", "float64"},
        {"bool", "boolean"},
        {"None", "void"},
        
        // JavaScript to unified
        {"number", "float64"},
        {"string", "string"},
        {"boolean", "boolean"},
        {"undefined", "void"},
        {"null", "void"}
    };
    
    auto it = type_mapping.find(type);
    return (it != type_mapping.end()) ? it->second : type;
}

void ASTGenerator::optimize_ast(UnifiedASTNode* node) {
    if (!node) return;
    
    // Optimize current node
    optimize_node(node);
    
    // Recursively optimize children
    for (auto& child : node->children) {
        optimize_ast(child.get());
    }
    
    // Merge equivalent children if enabled
    if (options_.merge_equivalent_nodes) {
        merge_equivalent_children(node);
    }
}

void ASTGenerator::optimize_node(UnifiedASTNode* node) {
    if (!node) return;
    
    switch (node->type) {
        case UnifiedNodeType::EXPRESSION:
            optimize_expression(node);
            break;
            
        case UnifiedNodeType::STATEMENT:
            optimize_statement(node);
            break;
            
        case UnifiedNodeType::BLOCK_STATEMENT:
            optimize_block_statement(node);
            break;
            
        default:
            // No specific optimization needed
            break;
    }
}

void ASTGenerator::optimize_expression(UnifiedASTNode* node) {
    // Optimize expression nodes (constant folding, etc.)
    // This is a placeholder for expression optimization
}

void ASTGenerator::optimize_statement(UnifiedASTNode* node) {
    // Optimize statement nodes (dead code elimination, etc.)
    // This is a placeholder for statement optimization
}

void ASTGenerator::optimize_block_statement(UnifiedASTNode* node) {
    // Optimize block statements (flatten nested blocks, etc.)
    // This is a placeholder for block optimization
}

void ASTGenerator::merge_equivalent_children(UnifiedASTNode* node) {
    if (!node || node->children.size() < 2) return;
    
    // Find and merge equivalent consecutive children
    auto it = node->children.begin();
    while (it != node->children.end() && std::next(it) != node->children.end()) {
        auto next_it = std::next(it);
        
        if (are_nodes_equivalent(it->get(), next_it->get())) {
            // Merge nodes
            merge_nodes(it->get(), next_it->get());
            
            // Remove the second node
            node->children.erase(next_it);
        } else {
            ++it;
        }
    }
}

bool ASTGenerator::are_nodes_equivalent(UnifiedASTNode* node1, UnifiedASTNode* node2) {
    if (!node1 || !node2) return false;
    
    // Check if nodes have the same type and can be merged
    if (node1->type != node2->type) return false;
    
    // For now, only merge nodes of the same type with identical attributes
    // This could be expanded with more sophisticated equivalence checking
    return node1->attributes == node2->attributes;
}

void ASTGenerator::merge_nodes(UnifiedASTNode* target, UnifiedASTNode* source) {
    if (!target || !source) return;
    
    // Move children from source to target
    for (auto& child : source->children) {
        target->children.push_back(std::move(child));
    }
    
    // Merge attributes (target takes precedence)
    for (const auto& attr : source->attributes) {
        if (target->attributes.find(attr.first) == target->attributes.end()) {
            target->attributes[attr.first] = attr.second;
        }
    }
}

// UnifiedASTNode implementations

std::string ProgramNode::to_string() const {
    return "Program: " + name + " (imports: " + std::to_string(imports.size()) + ")";
}

std::string FunctionDeclarationNode::to_string() const {
    return "Function: " + name + "(" + std::to_string(parameters.size()) + " params) -> " + return_type;
}

std::string VariableDeclarationNode::to_string() const {
    return "Variable: " + name + " : " + type;
}

std::string ClassDeclarationNode::to_string() const {
    return "Class: " + name + " (bases: " + std::to_string(base_classes.size()) + 
           ", members: " + std::to_string(members.size()) + ")";
}

std::string IdentifierNode::to_string() const {
    return "Identifier: " + name;
}

std::string FunctionCallNode::to_string() const {
    return "FunctionCall: " + function_name + "(" + std::to_string(arguments.size()) + " args)";
}

} // namespace parser
} // namespace engine
} // namespace core
} // namespace akao
