/**
 * @id: akao:class:nodes:external:cpp:v1
 * @doc: Advanced C++ source code analyzer node for comprehensive static analysis and architectural compliance checking. Provides class counting, function analysis, dependency tracking, complexity metrics, and coding standard validation. Designed specifically for Akao's one-class-per-file rule and modern C++ compliance checking.
 * @specification: C++ analyzer node with advanced static analysis capabilities
 * @scope: External C++ analysis node
 * @timeline: 2025-07-10
 * @rationale: Enable sophisticated C++ code validation for architectural compliance workflows
 * @methodology: Custom C++ parsing with zero external dependencies and comprehensive analysis
 * @references: ["akao:interface:core:foundation:interfaces:inode:v1"]
 */

#pragma once

#include "core/foundation/interfaces/inode/v1.hpp"
#include <string>
#include <vector>
#include <set>
#include <map>
#include <regex>
#include <memory>

namespace akao {
namespace nodes {
namespace external {

/**
 * Information about a C++ class found in source code
 */
struct ClassInfo {
    std::string name;
    std::string fullName; // Including namespace
    std::string type; // "class", "struct", "union"
    std::vector<std::string> baseClasses;
    std::vector<std::string> methods;
    std::vector<std::string> fields;
    bool isTemplate = false;
    bool isAbstract = false;
    size_t lineNumber = 0;
    size_t lineCount = 0;
    std::string accessLevel = "public"; // Default for struct
    
    ClassInfo() = default;
};

/**
 * Information about a C++ function
 */
struct FunctionInfo {
    std::string name;
    std::string returnType;
    std::vector<std::string> parameters;
    std::string className; // Empty if global function
    bool isVirtual = false;
    bool isPure = false;
    bool isConst = false;
    bool isStatic = false;
    bool isInline = false;
    bool isTemplate = false;
    size_t lineNumber = 0;
    size_t lineCount = 0;
    size_t complexity = 1; // Cyclomatic complexity
    
    FunctionInfo() = default;
};

/**
 * Information about C++ includes and dependencies
 */
struct IncludeInfo {
    std::string header;
    bool isSystemHeader = false; // <> vs ""
    bool isLocalHeader = false;
    size_t lineNumber = 0;
    
    IncludeInfo() = default;
};

/**
 * Comprehensive analysis results for a C++ file
 */
struct CppFileAnalysis {
    std::string filePath;
    std::vector<ClassInfo> classes;
    std::vector<FunctionInfo> functions;
    std::vector<IncludeInfo> includes;
    std::vector<std::string> namespaces;
    std::map<std::string, size_t> metrics; // Various code metrics
    std::vector<std::string> violations; // Coding standard violations
    bool hasOneClassPerFile = true;
    bool usesModernCpp = false;
    bool hasProperHeaders = false;
    size_t totalLines = 0;
    size_t codeLines = 0;
    size_t commentLines = 0;
    size_t blankLines = 0;
    
    CppFileAnalysis() = default;
};

/**
 * Configuration for C++ analysis
 */
struct CppAnalysisConfig {
    bool enforceOneClassPerFile = true;
    bool checkModernCpp = true;
    bool analyzeComplexity = true;
    bool checkNamingConventions = true;
    bool validateIncludes = true;
    bool collectMetrics = true;
    std::vector<std::string> requiredHeaders;
    std::vector<std::string> bannedHeaders;
    std::set<std::string> modernCppFeatures;
    size_t maxComplexity = 10;
    size_t maxClassSize = 500;
    size_t maxFunctionSize = 50;
    
    CppAnalysisConfig() {
        // Initialize modern C++ features to check for
        modernCppFeatures = {
            "auto", "nullptr", "override", "final", "constexpr",
            "std::unique_ptr", "std::shared_ptr", "std::move",
            "lambda", "range-based for", "std::", "using namespace std"
        };
    }
};

/**
 * Advanced C++ source code analyzer with comprehensive static analysis capabilities
 */
class CppAnalyzerNode : public foundation::interfaces::INode {
private:
    std::string nodeId_;
    std::string nodeType_;
    std::string version_;
    std::string description_;
    
    // Core analysis methods
    CppFileAnalysis analyzeFile(const std::string& filePath, const CppAnalysisConfig& config) const;
    CppFileAnalysis analyzeContent(const std::string& content, const CppAnalysisConfig& config) const;
    std::vector<ClassInfo> extractClasses(const std::string& content) const;
    std::vector<FunctionInfo> extractFunctions(const std::string& content) const;
    std::vector<IncludeInfo> extractIncludes(const std::string& content) const;
    std::vector<std::string> extractNamespaces(const std::string& content) const;
    
    // Validation methods
    std::vector<std::string> validateOneClassPerFile(const CppFileAnalysis& analysis) const;
    std::vector<std::string> validateModernCpp(const CppFileAnalysis& analysis, const CppAnalysisConfig& config) const;
    std::vector<std::string> validateModernCppContent(const std::string& content, const CppAnalysisConfig& config) const;
    std::vector<std::string> validateComplexity(const CppFileAnalysis& analysis, const CppAnalysisConfig& config) const;
    std::vector<std::string> validateNamingConventions(const CppFileAnalysis& analysis) const;
    std::vector<std::string> validateIncludes(const CppFileAnalysis& analysis, const CppAnalysisConfig& config) const;
    
    // Metrics calculation
    void calculateMetrics(CppFileAnalysis& analysis, const std::string& content) const;
    size_t calculateCyclomaticComplexity(const std::string& functionBody) const;
    void calculateLineCounts(CppFileAnalysis& analysis, const std::string& content) const;
    
    // Parsing utilities
    std::string removeComments(const std::string& content) const;
    std::string removeStrings(const std::string& content) const;
    std::vector<std::string> tokenize(const std::string& content) const;
    bool isClassDeclaration(const std::string& line) const;
    bool isFunctionDeclaration(const std::string& line) const;
    std::string extractClassName(const std::string& line) const;
    std::string extractFunctionName(const std::string& line) const;
    
    // Configuration parsing
    CppAnalysisConfig parseConfiguration(const foundation::interfaces::NodeParameters& params) const;
    foundation::types::NodeValue convertAnalysisToNodeValue(const CppFileAnalysis& analysis) const;
    foundation::types::NodeValue convertClassInfoToNodeValue(const ClassInfo& classInfo) const;
    foundation::types::NodeValue convertFunctionInfoToNodeValue(const FunctionInfo& functionInfo) const;
    
public:
    CppAnalyzerNode();
    ~CppAnalyzerNode() = default;
    
    // INode interface implementation
    std::string getNodeId() const override;
    std::string getNodeType() const override;
    std::string getVersion() const override;
    std::string getDescription() const override;
    
    foundation::interfaces::ValidationResult validate(const foundation::interfaces::NodeParameters& params) const override;
    foundation::types::ExecutionResult execute(const foundation::interfaces::NodeContext& context,
                                             const foundation::interfaces::NodeParameters& params) override;
    foundation::types::NodeValue getParameterSchema() const override;
    
    // Static utility methods for direct C++ analysis
    static size_t countClasses(const std::string& content);
    static bool hasModernCpp(const std::string& content);
    static bool hasProperHeaders(const std::string& content);
    static std::vector<std::string> getCppClasses(const std::string& content);
    static std::vector<std::string> getCppIncludes(const std::string& content);
    static bool validateOneClassRule(const std::string& filePath);
    static size_t calculateComplexity(const std::string& content);
    static bool usesStandardLibrary(const std::string& content);
};

} // namespace external
} // namespace nodes
} // namespace akao