#pragma once

#include "../self_proving/v1.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace akao::core::engine::semantic {

/**
 * @brief Semantic disambiguation result
 */
struct DisambiguationResult {
    std::string original_term;
    std::string canonical_meaning;
    std::vector<std::string> alternative_meanings;
    std::map<std::string, std::string> context_mappings;
    bool is_unambiguous = false;
    std::string resolution_method;
};

/**
 * @brief Semantic context for disambiguation
 */
struct SemanticContext {
    std::string domain; // e.g., "philosophy", "rule", "validation"
    std::string scope;  // e.g., "global", "local", "component"
    std::map<std::string, std::string> definitions;
    std::vector<std::string> related_concepts;
};

/**
 * @brief Semantic Disambiguation Framework
 * 
 * Implements Phase 6: Semantic Disambiguation Framework - Final implementation phase
 * 
 * Responsibilities:
 * - Disambiguate terms and concepts across the entire system
 * - Establish canonical meanings for all system terminology
 * - Resolve semantic conflicts between components
 * - Create comprehensive semantic mappings
 * - Ensure semantic consistency across philosophies, rules, and validations
 * 
 * Framework Features:
 * - Context-aware disambiguation
 * - Hierarchical semantic resolution
 * - Cross-domain semantic mapping
 * - Automatic conflict detection and resolution
 * - Semantic validation and verification
 */
class SemanticDisambiguationFramework {
private:
    std::unique_ptr<self_proving::SelfProvingEngine> self_proving_engine_;
    
    // Semantic knowledge base
    std::map<std::string, std::vector<std::string>> semantic_mappings_;
    std::map<std::string, SemanticContext> context_definitions_;
    std::map<std::string, std::string> canonical_definitions_;
    
    // Disambiguation algorithms
    DisambiguationResult disambiguateByContext(const std::string& term, const SemanticContext& context);
    DisambiguationResult disambiguateByFrequency(const std::string& term);
    DisambiguationResult disambiguateByDomain(const std::string& term, const std::string& domain);
    
    // Semantic validation
    bool validateSemanticConsistency();
    bool detectSemanticConflicts();
    void resolveSemanticConflicts();
    
    // Knowledge base management
    void buildSemanticKnowledgeBase();
    void loadDomainSpecificSemantics();

public:
    SemanticDisambiguationFramework();
    ~SemanticDisambiguationFramework();
    
    // Phase 6 Core Implementation
    bool initializeFramework();
    bool processAllSystemSemantics();
    bool validateSemanticIntegrity();
    
    // Disambiguation operations
    DisambiguationResult disambiguate(const std::string& term);
    DisambiguationResult disambiguateInContext(const std::string& term, const SemanticContext& context);
    
    // Framework completion
    bool finalizeSemanticFramework();
    bool generateSemanticDocumentation();
    
    // System-wide semantic operations
    std::vector<DisambiguationResult> disambiguateAllTerms();
    bool establishCanonicalMeanings();
    bool validateCrossComponentSemantics();
    
    // Reporting and analysis
    std::map<std::string, std::string> getCanonicalDefinitions();
    std::vector<std::string> getAmbiguousTerms();
    std::map<std::string, std::vector<std::string>> getSemanticConflicts();
};

} // namespace akao::core::engine::semantic