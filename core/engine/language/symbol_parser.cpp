/**
 * akao:core:engine:language:symbol-parser:v1
 * 
 * Symbol Parser for Pure Logic Language Symbol System
 * 
 * Specification: Advanced symbol parsing and transformation system for dual-mode input
 *                supporting Unicode mathematical symbols and ASCII fallbacks with intelligent
 *                completion and context-aware symbol recognition.
 * 
 * Scope: Symbol recognition, bidirectional transformation, intelligent input completion,
 *        context-sensitive symbol processing, and accessibility-focused symbol handling.
 * 
 * Rationale: Enable mathematical notation while maintaining universal keyboard accessibility
 *           through sophisticated symbol processing and transformation algorithms.
 * 
 * Methodology: Pattern-based symbol recognition with context awareness, bidirectional
 *             transformation algorithms, and intelligent completion system for enhanced usability.
 * 
 * Integration: Pure Logic grammar, lexical analyzer, symbol system configuration,
 *             and editor integration components.
 */

#include "pure_logic_grammar.hpp"
#include <unordered_map>
#include <algorithm>
#include <regex>
#include <unicode/utf8.h>

namespace akao {
namespace language {
namespace pure_logic {

/**
 * Symbol transformation modes
 */
enum class SymbolMode {
    UNICODE_PREFERRED,  // Prefer Unicode symbols, fallback to ASCII
    ASCII_PREFERRED,    // Prefer ASCII equivalents, show Unicode as hints
    MIXED_MODE,         // Allow mixing Unicode and ASCII
    UNICODE_ONLY,       // Only Unicode symbols
    ASCII_ONLY          // Only ASCII equivalents
};

/**
 * Symbol recognition context
 */
enum class SymbolContext {
    LOGICAL_EXPRESSION,    // Logical operators context
    MATHEMATICAL_EXPRESSION, // Mathematical operators context
    SET_EXPRESSION,        // Set operations context
    TYPE_ANNOTATION,       // Type system context
    STRING_LITERAL,        // Inside string literals
    COMMENT,              // Inside comments
    UNKNOWN               // Context not determined
};

/**
 * Symbol transformation result
 */
struct SymbolTransformation {
    std::string transformed_text;
    std::vector<std::pair<size_t, size_t>> symbol_positions; // (start, length) pairs
    std::map<size_t, std::string> symbol_metadata;
    bool transformation_applied;
    std::vector<std::string> warnings;
};

/**
 * Symbol completion suggestion
 */
struct SymbolCompletion {
    std::string trigger_text;      // Text that triggered completion
    std::string unicode_symbol;    // Unicode symbol suggestion
    std::string ascii_equivalent;  // ASCII equivalent
    std::string description;       // Human-readable description
    int priority;                  // Completion priority (higher = more relevant)
    SymbolContext applicable_context; // Context where this completion applies
};

/**
 * Core symbol parser class
 */
class SymbolParser {
public:
    /**
     * Constructor with configuration
     */
    explicit SymbolParser(SymbolMode mode = SymbolMode::MIXED_MODE);
    
    /**
     * Configure symbol processing behavior
     */
    void set_symbol_mode(SymbolMode mode);
    void set_context_sensitivity(bool enabled);
    void set_intelligent_completion(bool enabled);
    void configure_accessibility_options(const std::map<std::string, bool>& options);
    
    /**
     * Symbol transformation methods
     */
    SymbolTransformation transform_ascii_to_unicode(const std::string& text,
                                                   SymbolContext context = SymbolContext::UNKNOWN);
    SymbolTransformation transform_unicode_to_ascii(const std::string& text,
                                                   SymbolContext context = SymbolContext::UNKNOWN);
    SymbolTransformation normalize_symbols(const std::string& text, SymbolMode target_mode);
    
    /**
     * Symbol recognition and validation
     */
    bool is_valid_symbol(const std::string& symbol) const;
    bool is_unicode_symbol(const std::string& symbol) const;
    bool is_ascii_equivalent(const std::string& symbol) const;
    std::string get_symbol_type(const std::string& symbol) const;
    
    /**
     * Intelligent completion system
     */
    std::vector<SymbolCompletion> get_completions(const std::string& partial_input,
                                                 SymbolContext context = SymbolContext::UNKNOWN,
                                                 size_t max_suggestions = 10);
    bool should_trigger_completion(const std::string& text, size_t cursor_position) const;
    std::string apply_completion(const std::string& text, size_t cursor_position,
                                const SymbolCompletion& completion);
    
    /**
     * Context analysis
     */
    SymbolContext analyze_context(const std::string& text, size_t position) const;
    std::vector<std::string> extract_symbols(const std::string& text) const;
    std::map<std::string, int> analyze_symbol_usage(const std::string& text) const;
    
    /**
     * Bidirectional conversion utilities
     */
    std::string unicode_to_ascii(const std::string& unicode_symbol) const;
    std::string ascii_to_unicode(const std::string& ascii_text) const;
    std::vector<std::string> get_ascii_alternatives(const std::string& unicode_symbol) const;
    std::vector<std::string> get_unicode_alternatives(const std::string& ascii_text) const;
    
    /**
     * Validation and error checking
     */
    std::vector<std::string> validate_symbol_usage(const std::string& text) const;
    std::vector<std::string> check_consistency(const std::string& text) const;
    bool has_accessibility_issues(const std::string& text) const;

private:
    SymbolMode symbol_mode_;
    bool context_sensitivity_enabled_;
    bool intelligent_completion_enabled_;
    std::map<std::string, bool> accessibility_options_;
    
    // Symbol mapping tables
    std::unordered_map<std::string, std::string> unicode_to_ascii_map_;
    std::unordered_map<std::string, std::string> ascii_to_unicode_map_;
    std::unordered_map<std::string, std::vector<std::string>> ascii_alternatives_map_;
    std::unordered_map<std::string, std::vector<std::string>> keyboard_input_map_;
    
    // Symbol metadata
    std::unordered_map<std::string, std::string> symbol_descriptions_;
    std::unordered_map<std::string, std::string> symbol_categories_;
    std::unordered_map<std::string, int> symbol_precedence_;
    std::unordered_map<std::string, SymbolContext> symbol_contexts_;
    
    // Completion data
    std::vector<SymbolCompletion> completion_database_;
    std::unordered_map<std::string, std::vector<size_t>> completion_index_;
    
    // Pattern recognition
    std::vector<std::regex> symbol_patterns_;
    std::vector<std::regex> context_patterns_;
    
    // Initialization methods
    void initialize_symbol_mappings();
    void initialize_completion_database();
    void initialize_pattern_recognition();
    void load_symbol_metadata();
    
    // Symbol recognition helpers
    std::vector<std::pair<size_t, std::string>> find_symbols_in_text(const std::string& text) const;
    bool matches_symbol_pattern(const std::string& text, size_t position) const;
    std::string extract_symbol_at_position(const std::string& text, size_t position) const;
    
    // Context analysis helpers
    SymbolContext determine_context_at_position(const std::string& text, size_t position) const;
    bool is_in_string_literal(const std::string& text, size_t position) const;
    bool is_in_comment(const std::string& text, size_t position) const;
    std::string get_surrounding_context(const std::string& text, size_t position, 
                                       size_t context_size = 50) const;
    
    // Transformation algorithms
    SymbolTransformation apply_symbol_transformations(const std::string& text,
                                                     const std::unordered_map<std::string, std::string>& mapping,
                                                     SymbolContext context) const;
    void apply_context_specific_rules(SymbolTransformation& result, SymbolContext context) const;
    void validate_transformation_result(SymbolTransformation& result) const;
    
    // Completion algorithms
    std::vector<SymbolCompletion> find_matching_completions(const std::string& partial_input,
                                                           SymbolContext context) const;
    void rank_completions(std::vector<SymbolCompletion>& completions,
                         const std::string& partial_input,
                         SymbolContext context) const;
    bool is_valid_completion_context(const SymbolCompletion& completion, 
                                    SymbolContext context) const;
    
    // Utility methods
    std::string normalize_input(const std::string& input) const;
    std::vector<std::string> tokenize_for_completion(const std::string& text) const;
    bool is_word_boundary(const std::string& text, size_t position) const;
    size_t find_word_start(const std::string& text, size_t position) const;
    size_t find_word_end(const std::string& text, size_t position) const;
};

/**
 * Symbol parser implementation
 */

SymbolParser::SymbolParser(SymbolMode mode) 
    : symbol_mode_(mode)
    , context_sensitivity_enabled_(true)
    , intelligent_completion_enabled_(true)
{
    // Initialize default accessibility options
    accessibility_options_["high_contrast"] = false;
    accessibility_options_["large_symbols"] = false;
    accessibility_options_["screen_reader_friendly"] = false;
    accessibility_options_["keyboard_only"] = false;
    
    // Initialize all symbol mappings and data
    initialize_symbol_mappings();
    initialize_completion_database();
    initialize_pattern_recognition();
    load_symbol_metadata();
}

void SymbolParser::initialize_symbol_mappings() {
    // Logical operators
    unicode_to_ascii_map_["∧"] = "and";
    unicode_to_ascii_map_["∨"] = "or";
    unicode_to_ascii_map_["¬"] = "not";
    unicode_to_ascii_map_["→"] = "implies";
    unicode_to_ascii_map_["↔"] = "iff";
    unicode_to_ascii_map_["⊕"] = "xor";
    
    // Quantifiers
    unicode_to_ascii_map_["∀"] = "forall";
    unicode_to_ascii_map_["∃"] = "exists";
    unicode_to_ascii_map_["∃!"] = "exists_unique";
    
    // Set operators
    unicode_to_ascii_map_["∈"] = "in";
    unicode_to_ascii_map_["∉"] = "not_in";
    unicode_to_ascii_map_["⊆"] = "subset";
    unicode_to_ascii_map_["⊂"] = "proper_subset";
    unicode_to_ascii_map_["∪"] = "union";
    unicode_to_ascii_map_["∩"] = "intersection";
    unicode_to_ascii_map_["∖"] = "difference";
    
    // Comparison operators
    unicode_to_ascii_map_["≠"] = "!=";
    unicode_to_ascii_map_["≤"] = "<=";
    unicode_to_ascii_map_["≥"] = ">=";
    unicode_to_ascii_map_["≈"] = "~=";
    
    // Special symbols
    unicode_to_ascii_map_["⊤"] = "true";
    unicode_to_ascii_map_["⊥"] = "false";
    unicode_to_ascii_map_["∞"] = "infinity";
    unicode_to_ascii_map_["∅"] = "empty_set";
    unicode_to_ascii_map_["λ"] = "\\";
    
    // Create reverse mapping
    for (const auto& pair : unicode_to_ascii_map_) {
        ascii_to_unicode_map_[pair.second] = pair.first;
    }
    
    // Alternative keyboard inputs
    keyboard_input_map_["and"] = {"and", "/\\", "&"};
    keyboard_input_map_["or"] = {"or", "\\/", "|"};
    keyboard_input_map_["not"] = {"not", "~", "!"};
    keyboard_input_map_["implies"] = {"implies", "->", "=>"};
    keyboard_input_map_["iff"] = {"iff", "<->", "<=>"};
    keyboard_input_map_["forall"] = {"forall", "all", "\\forall"};
    keyboard_input_map_["exists"] = {"exists", "some", "\\exists"};
    keyboard_input_map_["in"] = {"in", "elem", "\\in"};
    keyboard_input_map_["subset"] = {"subset", "<=", "\\subseteq"};
    keyboard_input_map_["union"] = {"union", "\\cup"};
    keyboard_input_map_["intersection"] = {"intersection", "\\cap"};
    keyboard_input_map_["infinity"] = {"infinity", "inf", "\\infty"};
    keyboard_input_map_["empty_set"] = {"empty_set", "\\emptyset"};
}

void SymbolParser::initialize_completion_database() {
    // Logical operators
    completion_database_.push_back({
        "and", "∧", "and", "Logical conjunction (AND)", 10, SymbolContext::LOGICAL_EXPRESSION
    });
    completion_database_.push_back({
        "or", "∨", "or", "Logical disjunction (OR)", 10, SymbolContext::LOGICAL_EXPRESSION
    });
    completion_database_.push_back({
        "not", "¬", "not", "Logical negation (NOT)", 10, SymbolContext::LOGICAL_EXPRESSION
    });
    completion_database_.push_back({
        "implies", "→", "implies", "Logical implication", 9, SymbolContext::LOGICAL_EXPRESSION
    });
    completion_database_.push_back({
        "iff", "↔", "iff", "Logical biconditional (if and only if)", 8, SymbolContext::LOGICAL_EXPRESSION
    });
    
    // Quantifiers
    completion_database_.push_back({
        "forall", "∀", "forall", "Universal quantifier", 9, SymbolContext::LOGICAL_EXPRESSION
    });
    completion_database_.push_back({
        "exists", "∃", "exists", "Existential quantifier", 9, SymbolContext::LOGICAL_EXPRESSION
    });
    completion_database_.push_back({
        "exists_unique", "∃!", "exists_unique", "Unique existence quantifier", 7, SymbolContext::LOGICAL_EXPRESSION
    });
    
    // Set operations
    completion_database_.push_back({
        "in", "∈", "in", "Set membership", 8, SymbolContext::SET_EXPRESSION
    });
    completion_database_.push_back({
        "subset", "⊆", "subset", "Subset or equal", 7, SymbolContext::SET_EXPRESSION
    });
    completion_database_.push_back({
        "union", "∪", "union", "Set union", 7, SymbolContext::SET_EXPRESSION
    });
    completion_database_.push_back({
        "intersection", "∩", "intersection", "Set intersection", 7, SymbolContext::SET_EXPRESSION
    });
    
    // Mathematical constants
    completion_database_.push_back({
        "infinity", "∞", "infinity", "Infinity", 6, SymbolContext::MATHEMATICAL_EXPRESSION
    });
    completion_database_.push_back({
        "empty_set", "∅", "empty_set", "Empty set", 6, SymbolContext::SET_EXPRESSION
    });
    
    // Build completion index for fast lookup
    for (size_t i = 0; i < completion_database_.size(); ++i) {
        const auto& completion = completion_database_[i];
        completion_index_[completion.trigger_text].push_back(i);
        
        // Also index partial matches
        for (size_t len = 1; len <= completion.trigger_text.length(); ++len) {
            std::string prefix = completion.trigger_text.substr(0, len);
            completion_index_[prefix].push_back(i);
        }
    }
}

void SymbolParser::initialize_pattern_recognition() {
    // Patterns for symbol recognition
    symbol_patterns_.emplace_back(R"([∀∃∧∨¬→↔⊕∈∉⊆⊂∪∩∖≠≤≥≈⊤⊥∞∅λ])");  // Unicode symbols
    symbol_patterns_.emplace_back(R"(\b(and|or|not|implies|iff|xor|forall|exists|in|subset|union|intersection)\b)"); // ASCII keywords
    
    // Context recognition patterns
    context_patterns_.emplace_back(R"(predicate\s+\w+\s*\([^)]*\)\s*:)");  // Predicate definition
    context_patterns_.emplace_back(R"(\b(let|type|fact|rule|prove)\s+)");   // Statement keywords
    context_patterns_.emplace_back(R"(\{[^}]*\})");                         // Set expressions
    context_patterns_.emplace_back(R"("([^"\\]|\\.)*")");                   // String literals
    context_patterns_.emplace_back(R"(/\*.*?\*/)");                         // Block comments
    context_patterns_.emplace_back(R"(//.*$)");                             // Line comments
}

void SymbolParser::load_symbol_metadata() {
    // Symbol descriptions
    symbol_descriptions_["∧"] = "Logical conjunction (AND)";
    symbol_descriptions_["∨"] = "Logical disjunction (OR)";
    symbol_descriptions_["¬"] = "Logical negation (NOT)";
    symbol_descriptions_["→"] = "Logical implication";
    symbol_descriptions_["↔"] = "Logical biconditional";
    symbol_descriptions_["∀"] = "Universal quantifier";
    symbol_descriptions_["∃"] = "Existential quantifier";
    symbol_descriptions_["∈"] = "Set membership";
    symbol_descriptions_["⊆"] = "Subset or equal";
    symbol_descriptions_["∪"] = "Set union";
    symbol_descriptions_["∩"] = "Set intersection";
    
    // Symbol categories
    symbol_categories_["∧"] = "logical_operator";
    symbol_categories_["∨"] = "logical_operator";
    symbol_categories_["¬"] = "logical_operator";
    symbol_categories_["→"] = "logical_operator";
    symbol_categories_["↔"] = "logical_operator";
    symbol_categories_["∀"] = "quantifier";
    symbol_categories_["∃"] = "quantifier";
    symbol_categories_["∈"] = "set_operator";
    symbol_categories_["⊆"] = "set_operator";
    symbol_categories_["∪"] = "set_operator";
    symbol_categories_["∩"] = "set_operator";
    
    // Operator precedence (higher number = higher precedence)
    symbol_precedence_["¬"] = 4;
    symbol_precedence_["∧"] = 3;
    symbol_precedence_["∨"] = 2;
    symbol_precedence_["⊕"] = 2;
    symbol_precedence_["→"] = 1;
    symbol_precedence_["↔"] = 0;
}

SymbolTransformation SymbolParser::transform_ascii_to_unicode(const std::string& text, SymbolContext context) {
    return apply_symbol_transformations(text, ascii_to_unicode_map_, context);
}

SymbolTransformation SymbolParser::transform_unicode_to_ascii(const std::string& text, SymbolContext context) {
    return apply_symbol_transformations(text, unicode_to_ascii_map_, context);
}

std::vector<SymbolCompletion> SymbolParser::get_completions(const std::string& partial_input,
                                                           SymbolContext context,
                                                           size_t max_suggestions) {
    if (!intelligent_completion_enabled_) {
        return {};
    }
    
    std::vector<SymbolCompletion> results = find_matching_completions(partial_input, context);
    rank_completions(results, partial_input, context);
    
    if (results.size() > max_suggestions) {
        results.resize(max_suggestions);
    }
    
    return results;
}

SymbolContext SymbolParser::analyze_context(const std::string& text, size_t position) const {
    if (!context_sensitivity_enabled_) {
        return SymbolContext::UNKNOWN;
    }
    
    return determine_context_at_position(text, position);
}

std::string SymbolParser::unicode_to_ascii(const std::string& unicode_symbol) const {
    auto it = unicode_to_ascii_map_.find(unicode_symbol);
    return (it != unicode_to_ascii_map_.end()) ? it->second : unicode_symbol;
}

std::string SymbolParser::ascii_to_unicode(const std::string& ascii_text) const {
    auto it = ascii_to_unicode_map_.find(ascii_text);
    return (it != ascii_to_unicode_map_.end()) ? it->second : ascii_text;
}

// Additional implementation methods would continue here...
// [Implementation details for remaining methods would follow the same pattern]

} // namespace pure_logic
} // namespace language  
} // namespace akao
