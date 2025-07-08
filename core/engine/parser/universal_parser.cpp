/**
 * @file universal_parser.cpp
 * @brief Universal parser implementation with zero-dependency architecture
 * @details Provides unified parsing interface with language-specific parsers and AST generation
 * 
 * ID: akao:component:core:engine:parser:universal-parser:v1
 * Scope: Multi-language parsing with unified AST generation and semantic normalization
 * Dependencies: language_detectors.hpp, ast_generator.cpp
 */

#include "universal_parser.hpp"
#include "language_detectors.hpp"
#include "ast_generator.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <memory>

namespace akao {
namespace core {
namespace engine {
namespace parser {

// UniversalParser Implementation

UniversalParser::UniversalParser() 
    : initialized_(false)
    , language_detector_(std::make_unique<UniversalLanguageDetector>())
    , ast_generator_(std::make_unique<ASTGenerator>())
{
    initialize_parsers();
}

UniversalParser::~UniversalParser() = default;

ParseResult UniversalParser::parse_file(const std::string& filepath) {
    ParseResult result;
    auto start_time = std::chrono::high_resolution_clock::now();
    
    try {
        // Load file content
        std::string content = load_file_content(filepath);
        if (content.empty()) {
            result.success = false;
            result.error_message = "Cannot read file: " + filepath;
            return result;
        }
        
        // Parse content with file context
        result = parse_content(content, filepath);
        
    } catch (const std::exception& e) {
        result.success = false;
        result.error_message = "File parsing exception: " + std::string(e.what());
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    result.parse_time = std::chrono::duration<double>(end_time - start_time).count();
    
    return result;
}

ParseResult UniversalParser::parse_content(const std::string& content, const std::string& source_name) {
    ParseResult result;
    auto start_time = std::chrono::high_resolution_clock::now();
    
    try {
        // Detect language
        DetectionResult detection = language_detector_->detect_language(content, source_name);
        if (detection.language == LanguageType::UNKNOWN) {
            result.success = false;
            result.error_message = "Unable to detect language for: " + source_name;
            return result;
        }
        
        // Select appropriate parser
        auto parser = get_parser_for_language(detection.language);
        if (!parser) {
            result.success = false;
            result.error_message = "No parser available for language: " + 
                                   UniversalLanguageDetector::get_language_name(detection.language);
            return result;
        }
        
        // Parse content with language-specific parser
        LanguageParseResult lang_result = parser->parse(content, source_name);
        if (!lang_result.success) {
            result.success = false;
            result.error_message = lang_result.error_message;
            result.error_line = lang_result.error_line;
            result.error_column = lang_result.error_column;
            return result;
        }
        
        // Generate unified AST
        UnifiedAST unified_ast = ast_generator_->generate_unified_ast(
            lang_result.language_ast, detection.language);
        
        if (!unified_ast.is_valid) {
            result.success = false;
            result.error_message = "AST generation failed: " + unified_ast.error_message;
            return result;
        }
        
        // Perform semantic normalization
        SemanticResult semantic_result = perform_semantic_normalization(unified_ast);
        if (!semantic_result.success) {
            result.success = false;
            result.error_message = "Semantic normalization failed: " + semantic_result.error_message;
            return result;
        }
        
        result.success = true;
        result.detected_language = detection.language;
        result.confidence = detection.confidence;
        result.unified_ast = std::move(unified_ast);
        result.semantic_info = semantic_result;
        result.source_name = source_name;
        
    } catch (const std::exception& e) {
        result.success = false;
        result.error_message = "Content parsing exception: " + std::string(e.what());
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    result.parse_time = std::chrono::duration<double>(end_time - start_time).count();
    
    return result;
}

ParseResult UniversalParser::parse_with_language(const std::string& content, 
                                                  LanguageType language,
                                                  const std::string& source_name) {
    ParseResult result;
    auto start_time = std::chrono::high_resolution_clock::now();
    
    try {
        // Get parser for specified language
        auto parser = get_parser_for_language(language);
        if (!parser) {
            result.success = false;
            result.error_message = "No parser available for language: " + 
                                   UniversalLanguageDetector::get_language_name(language);
            return result;
        }
        
        // Parse content directly
        LanguageParseResult lang_result = parser->parse(content, source_name);
        if (!lang_result.success) {
            result.success = false;
            result.error_message = lang_result.error_message;
            result.error_line = lang_result.error_line;
            result.error_column = lang_result.error_column;
            return result;
        }
        
        // Generate unified AST
        UnifiedAST unified_ast = ast_generator_->generate_unified_ast(
            lang_result.language_ast, language);
        
        if (!unified_ast.is_valid) {
            result.success = false;
            result.error_message = "AST generation failed: " + unified_ast.error_message;
            return result;
        }
        
        // Perform semantic normalization
        SemanticResult semantic_result = perform_semantic_normalization(unified_ast);
        if (!semantic_result.success) {
            result.success = false;
            result.error_message = "Semantic normalization failed: " + semantic_result.error_message;
            return result;
        }
        
        result.success = true;
        result.detected_language = language;
        result.confidence = ConfidenceLevel::CERTAIN;
        result.unified_ast = std::move(unified_ast);
        result.semantic_info = semantic_result;
        result.source_name = source_name;
        
    } catch (const std::exception& e) {
        result.success = false;
        result.error_message = "Language-specific parsing exception: " + std::string(e.what());
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    result.parse_time = std::chrono::duration<double>(end_time - start_time).count();
    
    return result;
}

std::vector<LanguageType> UniversalParser::get_supported_languages() const {
    std::vector<LanguageType> languages;
    for (const auto& parser_pair : language_parsers_) {
        languages.push_back(parser_pair.first);
    }
    return languages;
}

void UniversalParser::register_language_parser(LanguageType language, 
                                                std::unique_ptr<LanguageParser> parser) {
    language_parsers_[language] = std::move(parser);
}

ParserStatistics UniversalParser::get_statistics() const {
    ParserStatistics stats;
    stats.total_files_parsed = statistics_.total_files_parsed;
    stats.successful_parses = statistics_.successful_parses;
    stats.failed_parses = statistics_.failed_parses;
    stats.average_parse_time = statistics_.total_parse_time / 
                               std::max(1u, statistics_.total_files_parsed);
    stats.total_parse_time = statistics_.total_parse_time;
    
    // Language-specific statistics
    for (const auto& lang_stat : statistics_.language_stats) {
        stats.language_statistics[lang_stat.first] = lang_stat.second;
    }
    
    return stats;
}

void UniversalParser::set_parse_options(const ParseOptions& options) {
    parse_options_ = options;
    
    // Configure AST generator
    ASTGeneratorOptions ast_options;
    ast_options.enable_semantic_analysis = options.enable_semantic_analysis;
    ast_options.enable_type_checking = options.enable_type_checking;
    ast_options.preserve_comments = options.preserve_comments;
    ast_options.generate_debug_info = options.generate_debug_info;
    ast_generator_->set_options(ast_options);
}

// Private Methods

void UniversalParser::initialize_parsers() {
    if (initialized_) return;
    
    // Register built-in language parsers
    register_language_parser(LanguageType::C, std::make_unique<CParser>());
    register_language_parser(LanguageType::CPP, std::make_unique<CppParser>());
    register_language_parser(LanguageType::PYTHON, std::make_unique<PythonParser>());
    register_language_parser(LanguageType::JAVASCRIPT, std::make_unique<JavaScriptParser>());
    register_language_parser(LanguageType::YAML, std::make_unique<YamlParser>());
    register_language_parser(LanguageType::JSON, std::make_unique<JsonParser>());
    register_language_parser(LanguageType::PROLOG, std::make_unique<PrologParser>());
    register_language_parser(LanguageType::PURE_LOGIC, std::make_unique<PureLogicParser>());
    
    // Initialize statistics
    statistics_.total_files_parsed = 0;
    statistics_.successful_parses = 0;
    statistics_.failed_parses = 0;
    statistics_.total_parse_time = 0.0;
    
    initialized_ = true;
}

std::string UniversalParser::load_file_content(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return "";
    }
    
    std::ostringstream content;
    content << file.rdbuf();
    return content.str();
}

LanguageParser* UniversalParser::get_parser_for_language(LanguageType language) {
    auto it = language_parsers_.find(language);
    return (it != language_parsers_.end()) ? it->second.get() : nullptr;
}

SemanticResult UniversalParser::perform_semantic_normalization(const UnifiedAST& ast) {
    SemanticResult result;
    
    try {
        // Perform symbol resolution
        SymbolResolutionResult symbol_result = resolve_symbols(ast);
        if (!symbol_result.success) {
            result.success = false;
            result.error_message = "Symbol resolution failed: " + symbol_result.error_message;
            return result;
        }
        
        // Perform type checking
        TypeCheckingResult type_result = perform_type_checking(ast, symbol_result.symbol_table);
        if (!type_result.success) {
            result.success = false;
            result.error_message = "Type checking failed: " + type_result.error_message;
            return result;
        }
        
        // Perform dependency analysis
        DependencyResult dep_result = analyze_dependencies(ast);
        if (!dep_result.success) {
            result.success = false;
            result.error_message = "Dependency analysis failed: " + dep_result.error_message;
            return result;
        }
        
        result.success = true;
        result.symbol_table = symbol_result.symbol_table;
        result.type_info = type_result.type_info;
        result.dependencies = dep_result.dependencies;
        
    } catch (const std::exception& e) {
        result.success = false;
        result.error_message = "Semantic normalization exception: " + std::string(e.what());
    }
    
    return result;
}

SymbolResolutionResult UniversalParser::resolve_symbols(const UnifiedAST& ast) {
    SymbolResolutionResult result;
    
    try {
        // Create symbol table
        result.symbol_table = std::make_unique<SymbolTable>();
        
        // Traverse AST and collect symbols
        traverse_ast_for_symbols(ast.root.get(), result.symbol_table.get());
        
        // Resolve symbol references
        resolve_symbol_references(ast.root.get(), result.symbol_table.get());
        
        result.success = true;
        
    } catch (const std::exception& e) {
        result.success = false;
        result.error_message = "Symbol resolution exception: " + std::string(e.what());
    }
    
    return result;
}

TypeCheckingResult UniversalParser::perform_type_checking(const UnifiedAST& ast, 
                                                           const std::unique_ptr<SymbolTable>& symbol_table) {
    TypeCheckingResult result;
    
    try {
        // Create type checker
        TypeChecker type_checker(symbol_table.get());
        
        // Perform type checking
        result.type_info = type_checker.check_types(ast.root.get());
        
        result.success = true;
        
    } catch (const std::exception& e) {
        result.success = false;
        result.error_message = "Type checking exception: " + std::string(e.what());
    }
    
    return result;
}

DependencyResult UniversalParser::analyze_dependencies(const UnifiedAST& ast) {
    DependencyResult result;
    
    try {
        // Create dependency analyzer
        DependencyAnalyzer analyzer;
        
        // Analyze dependencies
        result.dependencies = analyzer.analyze(ast.root.get());
        
        result.success = true;
        
    } catch (const std::exception& e) {
        result.success = false;
        result.error_message = "Dependency analysis exception: " + std::string(e.what());
    }
    
    return result;
}

void UniversalParser::traverse_ast_for_symbols(UnifiedASTNode* node, SymbolTable* symbol_table) {
    if (!node || !symbol_table) return;
    
    // Process current node for symbols
    switch (node->type) {
        case UnifiedNodeType::FUNCTION_DECLARATION:
            {
                auto func_node = static_cast<FunctionDeclarationNode*>(node);
                symbol_table->add_function(func_node->name, func_node);
            }
            break;
            
        case UnifiedNodeType::VARIABLE_DECLARATION:
            {
                auto var_node = static_cast<VariableDeclarationNode*>(node);
                symbol_table->add_variable(var_node->name, var_node);
            }
            break;
            
        case UnifiedNodeType::CLASS_DECLARATION:
            {
                auto class_node = static_cast<ClassDeclarationNode*>(node);
                symbol_table->add_class(class_node->name, class_node);
            }
            break;
            
        default:
            break;
    }
    
    // Recursively process child nodes
    for (auto& child : node->children) {
        traverse_ast_for_symbols(child.get(), symbol_table);
    }
}

void UniversalParser::resolve_symbol_references(UnifiedASTNode* node, SymbolTable* symbol_table) {
    if (!node || !symbol_table) return;
    
    // Resolve references in current node
    switch (node->type) {
        case UnifiedNodeType::IDENTIFIER:
            {
                auto id_node = static_cast<IdentifierNode*>(node);
                id_node->resolved_symbol = symbol_table->lookup(id_node->name);
            }
            break;
            
        case UnifiedNodeType::FUNCTION_CALL:
            {
                auto call_node = static_cast<FunctionCallNode*>(node);
                call_node->resolved_function = symbol_table->lookup_function(call_node->function_name);
            }
            break;
            
        default:
            break;
    }
    
    // Recursively resolve in child nodes
    for (auto& child : node->children) {
        resolve_symbol_references(child.get(), symbol_table);
    }
}

void UniversalParser::update_statistics(const ParseResult& result) {
    statistics_.total_files_parsed++;
    
    if (result.success) {
        statistics_.successful_parses++;
    } else {
        statistics_.failed_parses++;
    }
    
    statistics_.total_parse_time += result.parse_time;
    
    // Update language-specific statistics
    if (result.detected_language != LanguageType::UNKNOWN) {
        auto& lang_stats = statistics_.language_stats[result.detected_language];
        lang_stats.total_files++;
        lang_stats.total_time += result.parse_time;
        
        if (result.success) {
            lang_stats.successful_files++;
        } else {
            lang_stats.failed_files++;
        }
    }
}

// Language-Specific Parser Implementations (Placeholder)

// CParser Implementation
LanguageParseResult CParser::parse(const std::string& content, const std::string& source_name) {
    LanguageParseResult result;
    
    // Placeholder implementation - would use actual C parser
    result.success = true;
    result.language_ast = create_placeholder_ast(LanguageType::C);
    
    return result;
}

// CppParser Implementation
LanguageParseResult CppParser::parse(const std::string& content, const std::string& source_name) {
    LanguageParseResult result;
    
    // Placeholder implementation - would use actual C++ parser
    result.success = true;
    result.language_ast = create_placeholder_ast(LanguageType::CPP);
    
    return result;
}

// PythonParser Implementation
LanguageParseResult PythonParser::parse(const std::string& content, const std::string& source_name) {
    LanguageParseResult result;
    
    // Placeholder implementation - would use actual Python parser
    result.success = true;
    result.language_ast = create_placeholder_ast(LanguageType::PYTHON);
    
    return result;
}

// Additional parser implementations would follow similar pattern...

std::unique_ptr<LanguageAST> create_placeholder_ast(LanguageType language) {
    auto ast = std::make_unique<LanguageAST>();
    ast->language = language;
    ast->is_valid = true;
    // Would create actual AST structure based on parsed content
    return ast;
}

} // namespace parser
} // namespace engine
} // namespace core
} // namespace akao
