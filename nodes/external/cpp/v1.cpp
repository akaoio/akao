/**
 * @id: akao:file:nodes:external:cpp:v1
 * @doc: Implementation of advanced C++ source code analyzer node providing comprehensive static analysis, architectural compliance checking, and coding standard validation. Features custom C++ parsing with zero external dependencies for maximum compatibility and performance.
 * @specification: C++ analyzer node implementation with comprehensive static analysis
 * @scope: External C++ analysis node implementation
 * @timeline: 2025-07-10
 * @rationale: Concrete implementation of C++ analysis for validation workflows
 * @methodology: Custom parsing with advanced pattern matching and semantic analysis
 * @references: ["akao:class:nodes:external:cpp:v1"]
 */

#include "v1.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace akao {
namespace nodes {
namespace external {

CppAnalyzerNode::CppAnalyzerNode()
    : nodeId_("akao:external:cpp-analyzer")
    , nodeType_("cpp")
    , version_("1.0.0")
    , description_("Advanced C++ source code analyzer with comprehensive static analysis capabilities") {
}

std::string CppAnalyzerNode::getNodeId() const {
    return nodeId_;
}

std::string CppAnalyzerNode::getNodeType() const {
    return nodeType_;
}

std::string CppAnalyzerNode::getVersion() const {
    return version_;
}

std::string CppAnalyzerNode::getDescription() const {
    return description_;
}

foundation::interfaces::ValidationResult CppAnalyzerNode::validate(const foundation::interfaces::NodeParameters& params) const {
    foundation::interfaces::ValidationResult result;
    
    // Check required parameters
    if (!params.hasParameter("filePath") && !params.hasParameter("content")) {
        result.addError("Missing required parameter: either 'filePath' or 'content' must be provided");
        return result;
    }
    
    // If filePath is provided, check if file exists
    if (params.hasParameter("filePath")) {
        std::string filePath = params.getParameter("filePath").asString();
        if (filePath.empty()) {
            result.addError("Parameter 'filePath' cannot be empty");
            return result;
        }
        
        // Check if file exists and is readable
        std::ifstream file(filePath);
        if (!file.is_open()) {
            result.addError("Cannot read file: " + filePath);
            return result;
        }
        
        // Validate file extension
        if (filePath.find(".cpp") == std::string::npos && 
            filePath.find(".hpp") == std::string::npos &&
            filePath.find(".cc") == std::string::npos &&
            filePath.find(".cxx") == std::string::npos &&
            filePath.find(".h") == std::string::npos) {
            result.addWarning("File does not have a recognized C++ extension: " + filePath);
        }
    }
    
    // Validate optional configuration parameters
    if (params.hasParameter("maxComplexity")) {
        auto complexity = params.getParameter("maxComplexity");
        if (!complexity.isInteger() || complexity.asInteger() < 1) {
            result.addError("Parameter 'maxComplexity' must be a positive integer");
        }
    }
    
    if (params.hasParameter("maxClassSize")) {
        auto size = params.getParameter("maxClassSize");
        if (!size.isInteger() || size.asInteger() < 1) {
            result.addError("Parameter 'maxClassSize' must be a positive integer");
        }
    }
    
    return result;
}

foundation::types::ExecutionResult CppAnalyzerNode::execute(const foundation::interfaces::NodeContext& context,
                                                           const foundation::interfaces::NodeParameters& params) {
    try {
        // Parse configuration
        CppAnalysisConfig config = parseConfiguration(params);
        
        // Get file content
        std::string content;
        std::string filePath;
        
        if (params.hasParameter("filePath")) {
            filePath = params.getParameter("filePath").asString();
            std::ifstream file(filePath);
            if (!file.is_open()) {
                return foundation::types::ExecutionResult::error("Cannot read file: " + filePath);
            }
            
            std::stringstream buffer;
            buffer << file.rdbuf();
            content = buffer.str();
        } else {
            content = params.getParameter("content").asString();
            filePath = "inline_content";
        }
        
        // Perform analysis
        CppFileAnalysis analysis;
        if (filePath != "inline_content") {
            analysis = analyzeFile(filePath, config);
        } else {
            analysis = analyzeContent(content, config);
        }
        
        // Convert results to NodeValue
        foundation::types::NodeValue resultData = convertAnalysisToNodeValue(analysis);
        
        // Add execution metadata
        resultData.setMetadata("analysisType", foundation::types::NodeValue("cpp_static_analysis"));
        resultData.setMetadata("nodeId", foundation::types::NodeValue(nodeId_));
        resultData.setMetadata("filePath", foundation::types::NodeValue(filePath));
        resultData.setMetadata("configUsed", foundation::types::NodeValue(true));
        
        return foundation::types::ExecutionResult::success(resultData);
        
    } catch (const std::exception& e) {
        return foundation::types::ExecutionResult::error("C++ analysis failed: " + std::string(e.what()));
    }
}

foundation::types::NodeValue CppAnalyzerNode::getParameterSchema() const {
    foundation::types::NodeValue::Object schema;
    
    // Input parameters (either filePath or content)
    foundation::types::NodeValue::Object filePathParam;
    filePathParam["type"] = foundation::types::NodeValue("string");
    filePathParam["required"] = foundation::types::NodeValue(false);
    filePathParam["description"] = foundation::types::NodeValue("Path to C++ source file to analyze");
    schema["filePath"] = foundation::types::NodeValue(filePathParam);
    
    foundation::types::NodeValue::Object contentParam;
    contentParam["type"] = foundation::types::NodeValue("string");
    contentParam["required"] = foundation::types::NodeValue(false);
    contentParam["description"] = foundation::types::NodeValue("C++ source code content to analyze directly");
    schema["content"] = foundation::types::NodeValue(contentParam);
    
    // Configuration parameters
    foundation::types::NodeValue::Object enforceOneClassParam;
    enforceOneClassParam["type"] = foundation::types::NodeValue("boolean");
    enforceOneClassParam["required"] = foundation::types::NodeValue(false);
    enforceOneClassParam["default"] = foundation::types::NodeValue(true);
    enforceOneClassParam["description"] = foundation::types::NodeValue("Enforce one-class-per-file rule");
    schema["enforceOneClassPerFile"] = foundation::types::NodeValue(enforceOneClassParam);
    
    foundation::types::NodeValue::Object checkModernParam;
    checkModernParam["type"] = foundation::types::NodeValue("boolean");
    checkModernParam["required"] = foundation::types::NodeValue(false);
    checkModernParam["default"] = foundation::types::NodeValue(true);
    checkModernParam["description"] = foundation::types::NodeValue("Check for modern C++ features usage");
    schema["checkModernCpp"] = foundation::types::NodeValue(checkModernParam);
    
    foundation::types::NodeValue::Object analyzeComplexityParam;
    analyzeComplexityParam["type"] = foundation::types::NodeValue("boolean");
    analyzeComplexityParam["required"] = foundation::types::NodeValue(false);
    analyzeComplexityParam["default"] = foundation::types::NodeValue(true);
    analyzeComplexityParam["description"] = foundation::types::NodeValue("Analyze cyclomatic complexity");
    schema["analyzeComplexity"] = foundation::types::NodeValue(analyzeComplexityParam);
    
    foundation::types::NodeValue::Object maxComplexityParam;
    maxComplexityParam["type"] = foundation::types::NodeValue("integer");
    maxComplexityParam["required"] = foundation::types::NodeValue(false);
    maxComplexityParam["default"] = foundation::types::NodeValue(static_cast<int64_t>(10));
    maxComplexityParam["description"] = foundation::types::NodeValue("Maximum allowed cyclomatic complexity");
    schema["maxComplexity"] = foundation::types::NodeValue(maxComplexityParam);
    
    foundation::types::NodeValue::Object maxClassSizeParam;
    maxClassSizeParam["type"] = foundation::types::NodeValue("integer");
    maxClassSizeParam["required"] = foundation::types::NodeValue(false);
    maxClassSizeParam["default"] = foundation::types::NodeValue(static_cast<int64_t>(500));
    maxClassSizeParam["description"] = foundation::types::NodeValue("Maximum allowed class size in lines");
    schema["maxClassSize"] = foundation::types::NodeValue(maxClassSizeParam);
    
    foundation::types::NodeValue::Object requiredHeadersParam;
    requiredHeadersParam["type"] = foundation::types::NodeValue("array");
    requiredHeadersParam["required"] = foundation::types::NodeValue(false);
    requiredHeadersParam["description"] = foundation::types::NodeValue("List of required header files");
    schema["requiredHeaders"] = foundation::types::NodeValue(requiredHeadersParam);
    
    return foundation::types::NodeValue(schema);
}

CppAnalysisConfig CppAnalyzerNode::parseConfiguration(const foundation::interfaces::NodeParameters& params) const {
    CppAnalysisConfig config;
    
    if (params.hasParameter("enforceOneClassPerFile")) {
        config.enforceOneClassPerFile = params.getParameter("enforceOneClassPerFile").asBoolean();
    }
    
    if (params.hasParameter("checkModernCpp")) {
        config.checkModernCpp = params.getParameter("checkModernCpp").asBoolean();
    }
    
    if (params.hasParameter("analyzeComplexity")) {
        config.analyzeComplexity = params.getParameter("analyzeComplexity").asBoolean();
    }
    
    if (params.hasParameter("maxComplexity")) {
        config.maxComplexity = static_cast<size_t>(params.getParameter("maxComplexity").asInteger());
    }
    
    if (params.hasParameter("maxClassSize")) {
        config.maxClassSize = static_cast<size_t>(params.getParameter("maxClassSize").asInteger());
    }
    
    if (params.hasParameter("requiredHeaders")) {
        auto headers = params.getParameter("requiredHeaders");
        if (headers.isArray()) {
            for (const auto& header : headers.asArray()) {
                config.requiredHeaders.push_back(header.asString());
            }
        }
    }
    
    return config;
}

CppFileAnalysis CppAnalyzerNode::analyzeFile(const std::string& filePath, const CppAnalysisConfig& config) const {
    CppFileAnalysis analysis;
    analysis.filePath = filePath;
    
    // Read file content if filePath is provided
    std::string content;
    if (filePath != "inline_content") {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot read file: " + filePath);
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        content = buffer.str();
    } else {
        // This should not happen - use analyzeContent for inline content
        throw std::runtime_error("Invalid usage: use analyzeContent for inline content");
    }
    
    // Extract structural information
    analysis.classes = extractClasses(content);
    analysis.functions = extractFunctions(content);
    analysis.includes = extractIncludes(content);
    analysis.namespaces = extractNamespaces(content);
    
    // Calculate metrics
    calculateMetrics(analysis, content);
    
    // Perform validations
    if (config.enforceOneClassPerFile) {
        auto violations = validateOneClassPerFile(analysis);
        analysis.violations.insert(analysis.violations.end(), violations.begin(), violations.end());
    }
    
    if (config.checkModernCpp) {
        auto violations = validateModernCpp(analysis, config);
        analysis.violations.insert(analysis.violations.end(), violations.begin(), violations.end());
    }
    
    if (config.analyzeComplexity) {
        auto violations = validateComplexity(analysis, config);
        analysis.violations.insert(analysis.violations.end(), violations.begin(), violations.end());
    }
    
    if (config.checkNamingConventions) {
        auto violations = validateNamingConventions(analysis);
        analysis.violations.insert(analysis.violations.end(), violations.begin(), violations.end());
    }
    
    if (config.validateIncludes) {
        auto violations = validateIncludes(analysis, config);
        analysis.violations.insert(analysis.violations.end(), violations.begin(), violations.end());
    }
    
    // Set analysis flags
    analysis.hasOneClassPerFile = analysis.classes.size() <= 1;
    analysis.usesModernCpp = hasModernCpp(content);
    analysis.hasProperHeaders = hasProperHeaders(content);
    
    return analysis;
}

CppFileAnalysis CppAnalyzerNode::analyzeContent(const std::string& content, const CppAnalysisConfig& config) const {
    CppFileAnalysis analysis;
    analysis.filePath = "inline_content";
    
    // Extract structural information
    analysis.classes = extractClasses(content);
    analysis.functions = extractFunctions(content);
    analysis.includes = extractIncludes(content);
    analysis.namespaces = extractNamespaces(content);
    
    // Calculate metrics
    calculateMetrics(analysis, content);
    
    // Perform validations
    if (config.enforceOneClassPerFile) {
        auto violations = validateOneClassPerFile(analysis);
        analysis.violations.insert(analysis.violations.end(), violations.begin(), violations.end());
    }
    
    if (config.checkModernCpp) {
        auto violations = validateModernCppContent(content, config);
        analysis.violations.insert(analysis.violations.end(), violations.begin(), violations.end());
    }
    
    if (config.analyzeComplexity) {
        auto violations = validateComplexity(analysis, config);
        analysis.violations.insert(analysis.violations.end(), violations.begin(), violations.end());
    }
    
    if (config.checkNamingConventions) {
        auto violations = validateNamingConventions(analysis);
        analysis.violations.insert(analysis.violations.end(), violations.begin(), violations.end());
    }
    
    if (config.validateIncludes) {
        auto violations = validateIncludes(analysis, config);
        analysis.violations.insert(analysis.violations.end(), violations.begin(), violations.end());
    }
    
    // Set analysis flags
    analysis.hasOneClassPerFile = analysis.classes.size() <= 1;
    analysis.usesModernCpp = hasModernCpp(content);
    analysis.hasProperHeaders = hasProperHeaders(content);
    
    return analysis;
}

std::vector<ClassInfo> CppAnalyzerNode::extractClasses(const std::string& content) const {
    std::vector<ClassInfo> classes;
    
    // Remove comments and strings for cleaner parsing
    std::string cleanContent = removeComments(removeStrings(content));
    
    std::istringstream stream(cleanContent);
    std::string line;
    size_t lineNumber = 0;
    
    std::regex classRegex(R"((class|struct|union)\s+([A-Za-z_][A-Za-z0-9_]*))");
    std::smatch match;
    
    while (std::getline(stream, line)) {
        ++lineNumber;
        
        if (std::regex_search(line, match, classRegex)) {
            ClassInfo classInfo;
            classInfo.type = match[1].str();
            classInfo.name = match[2].str();
            classInfo.fullName = classInfo.name; // Simplified - could extract namespace
            classInfo.lineNumber = lineNumber;
            classInfo.accessLevel = (classInfo.type == "struct") ? "public" : "private";
            
            // Check for template
            if (line.find("template") != std::string::npos) {
                classInfo.isTemplate = true;
            }
            
            classes.push_back(classInfo);
        }
    }
    
    return classes;
}

std::vector<FunctionInfo> CppAnalyzerNode::extractFunctions(const std::string& content) const {
    std::vector<FunctionInfo> functions;
    
    std::string cleanContent = removeComments(removeStrings(content));
    std::istringstream stream(cleanContent);
    std::string line;
    size_t lineNumber = 0;
    
    // Simplified function detection - matches function declarations/definitions
    std::regex functionRegex(R"(([A-Za-z_][A-Za-z0-9_:]*)\s+([A-Za-z_][A-Za-z0-9_]*)\s*\([^)]*\)\s*[{;])");
    std::smatch match;
    
    while (std::getline(stream, line)) {
        ++lineNumber;
        
        if (std::regex_search(line, match, functionRegex)) {
            FunctionInfo funcInfo;
            funcInfo.returnType = match[1].str();
            funcInfo.name = match[2].str();
            funcInfo.lineNumber = lineNumber;
            
            // Check for various modifiers
            if (line.find("virtual") != std::string::npos) {
                funcInfo.isVirtual = true;
            }
            if (line.find("static") != std::string::npos) {
                funcInfo.isStatic = true;
            }
            if (line.find("inline") != std::string::npos) {
                funcInfo.isInline = true;
            }
            if (line.find("const") != std::string::npos) {
                funcInfo.isConst = true;
            }
            if (line.find(" = 0") != std::string::npos) {
                funcInfo.isPure = true;
            }
            
            functions.push_back(funcInfo);
        }
    }
    
    return functions;
}

std::vector<IncludeInfo> CppAnalyzerNode::extractIncludes(const std::string& content) const {
    std::vector<IncludeInfo> includes;
    
    std::istringstream stream(content);
    std::string line;
    size_t lineNumber = 0;
    
    std::regex includeRegex(R"(#include\s*[<"]([^>"]+)[>"])");
    std::smatch match;
    
    while (std::getline(stream, line)) {
        ++lineNumber;
        
        if (std::regex_search(line, match, includeRegex)) {
            IncludeInfo includeInfo;
            includeInfo.header = match[1].str();
            includeInfo.lineNumber = lineNumber;
            includeInfo.isSystemHeader = (line.find('<') != std::string::npos);
            includeInfo.isLocalHeader = (line.find('"') != std::string::npos);
            
            includes.push_back(includeInfo);
        }
    }
    
    return includes;
}

std::vector<std::string> CppAnalyzerNode::extractNamespaces(const std::string& content) const {
    std::vector<std::string> namespaces;
    std::set<std::string> uniqueNamespaces;
    
    std::regex namespaceRegex(R"(namespace\s+([A-Za-z_][A-Za-z0-9_]*))");
    std::sregex_iterator iter(content.begin(), content.end(), namespaceRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string ns = (*iter)[1].str();
        if (uniqueNamespaces.find(ns) == uniqueNamespaces.end()) {
            uniqueNamespaces.insert(ns);
            namespaces.push_back(ns);
        }
    }
    
    return namespaces;
}

std::vector<std::string> CppAnalyzerNode::validateOneClassPerFile(const CppFileAnalysis& analysis) const {
    std::vector<std::string> violations;
    
    if (analysis.classes.size() > 1) {
        violations.push_back("Multiple classes found in single file (" + 
                           std::to_string(analysis.classes.size()) + " classes). Violates one-class-per-file rule.");
        
        for (const auto& cls : analysis.classes) {
            violations.push_back("  - Class '" + cls.name + "' at line " + std::to_string(cls.lineNumber));
        }
    }
    
    return violations;
}

std::vector<std::string> CppAnalyzerNode::validateModernCpp(const CppFileAnalysis& analysis, const CppAnalysisConfig& config) const {
    std::vector<std::string> violations;
    
    // Read file content to check for modern C++ features
    std::ifstream file(analysis.filePath);
    if (file.is_open()) {
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        
        // Check for modern C++ features
        bool usesAuto = content.find("auto ") != std::string::npos;
        bool usesNullptr = content.find("nullptr") != std::string::npos;
        bool usesSmartPtrs = content.find("std::unique_ptr") != std::string::npos || 
                           content.find("std::shared_ptr") != std::string::npos;
        bool usesOverride = content.find("override") != std::string::npos;
        
        if (!usesAuto && content.find("int ") != std::string::npos) {
            violations.push_back("Consider using 'auto' for type deduction");
        }
        
        if (!usesNullptr && content.find("NULL") != std::string::npos) {
            violations.push_back("Use 'nullptr' instead of 'NULL' for null pointers");
        }
        
        if (content.find("new ") != std::string::npos && !usesSmartPtrs) {
            violations.push_back("Consider using smart pointers instead of raw 'new'");
        }
        
        if (content.find("virtual ") != std::string::npos && !usesOverride) {
            violations.push_back("Use 'override' keyword for virtual function overrides");
        }
    }
    
    return violations;
}

std::vector<std::string> CppAnalyzerNode::validateModernCppContent(const std::string& content, const CppAnalysisConfig& config) const {
    std::vector<std::string> violations;
    
    // Check for modern C++ features
    bool usesAuto = content.find("auto ") != std::string::npos;
    bool usesNullptr = content.find("nullptr") != std::string::npos;
    bool usesSmartPtrs = content.find("std::unique_ptr") != std::string::npos || 
                       content.find("std::shared_ptr") != std::string::npos;
    bool usesOverride = content.find("override") != std::string::npos;
    
    if (!usesAuto && content.find("int ") != std::string::npos) {
        violations.push_back("Consider using 'auto' for type deduction");
    }
    
    if (!usesNullptr && content.find("NULL") != std::string::npos) {
        violations.push_back("Use 'nullptr' instead of 'NULL' for null pointers");
    }
    
    if (content.find("new ") != std::string::npos && !usesSmartPtrs) {
        violations.push_back("Consider using smart pointers instead of raw 'new'");
    }
    
    if (content.find("virtual ") != std::string::npos && !usesOverride) {
        violations.push_back("Use 'override' keyword for virtual function overrides");
    }
    
    return violations;
}

std::vector<std::string> CppAnalyzerNode::validateComplexity(const CppFileAnalysis& analysis, const CppAnalysisConfig& config) const {
    std::vector<std::string> violations;
    
    for (const auto& func : analysis.functions) {
        if (func.complexity > config.maxComplexity) {
            violations.push_back("Function '" + func.name + "' has high complexity (" + 
                               std::to_string(func.complexity) + " > " + std::to_string(config.maxComplexity) + ")");
        }
    }
    
    for (const auto& cls : analysis.classes) {
        if (cls.lineCount > config.maxClassSize) {
            violations.push_back("Class '" + cls.name + "' is too large (" + 
                               std::to_string(cls.lineCount) + " > " + std::to_string(config.maxClassSize) + " lines)");
        }
    }
    
    return violations;
}

std::vector<std::string> CppAnalyzerNode::validateNamingConventions(const CppFileAnalysis& analysis) const {
    std::vector<std::string> violations;
    
    // Check class naming (should be PascalCase)
    std::regex pascalCaseRegex("^[A-Z][a-zA-Z0-9]*$");
    for (const auto& cls : analysis.classes) {
        if (!std::regex_match(cls.name, pascalCaseRegex)) {
            violations.push_back("Class '" + cls.name + "' should use PascalCase naming");
        }
    }
    
    // Check function naming (should be camelCase)
    std::regex camelCaseRegex("^[a-z][a-zA-Z0-9]*$");
    for (const auto& func : analysis.functions) {
        if (!std::regex_match(func.name, camelCaseRegex) && 
            func.name != "main" && 
            !func.isStatic && 
            func.name.find("_") == std::string::npos) { // Allow some exceptions
            violations.push_back("Function '" + func.name + "' should use camelCase naming");
        }
    }
    
    return violations;
}

std::vector<std::string> CppAnalyzerNode::validateIncludes(const CppFileAnalysis& analysis, const CppAnalysisConfig& config) const {
    std::vector<std::string> violations;
    
    // Check for required headers
    std::set<std::string> presentHeaders;
    for (const auto& inc : analysis.includes) {
        presentHeaders.insert(inc.header);
    }
    
    for (const auto& required : config.requiredHeaders) {
        if (presentHeaders.find(required) == presentHeaders.end()) {
            violations.push_back("Missing required header: " + required);
        }
    }
    
    // Check for banned headers
    for (const auto& inc : analysis.includes) {
        for (const auto& banned : config.bannedHeaders) {
            if (inc.header == banned) {
                violations.push_back("Banned header found: " + banned);
            }
        }
    }
    
    return violations;
}

void CppAnalyzerNode::calculateMetrics(CppFileAnalysis& analysis, const std::string& content) const {
    calculateLineCounts(analysis, content);
    
    analysis.metrics["classCount"] = analysis.classes.size();
    analysis.metrics["functionCount"] = analysis.functions.size();
    analysis.metrics["includeCount"] = analysis.includes.size();
    analysis.metrics["namespaceCount"] = analysis.namespaces.size();
    analysis.metrics["totalLines"] = analysis.totalLines;
    analysis.metrics["codeLines"] = analysis.codeLines;
    analysis.metrics["commentLines"] = analysis.commentLines;
    analysis.metrics["blankLines"] = analysis.blankLines;
    
    // Calculate average complexity
    size_t totalComplexity = 0;
    for (const auto& func : analysis.functions) {
        totalComplexity += func.complexity;
    }
    if (!analysis.functions.empty()) {
        analysis.metrics["averageComplexity"] = totalComplexity / analysis.functions.size();
    } else {
        analysis.metrics["averageComplexity"] = 0;
    }
}

void CppAnalyzerNode::calculateLineCounts(CppFileAnalysis& analysis, const std::string& content) const {
    std::istringstream stream(content);
    std::string line;
    
    analysis.totalLines = 0;
    analysis.codeLines = 0;
    analysis.commentLines = 0;
    analysis.blankLines = 0;
    
    bool inMultiLineComment = false;
    
    while (std::getline(stream, line)) {
        analysis.totalLines++;
        
        // Trim whitespace
        std::string trimmed = line;
        trimmed.erase(0, trimmed.find_first_not_of(" \t"));
        trimmed.erase(trimmed.find_last_not_of(" \t") + 1);
        
        if (trimmed.empty()) {
            analysis.blankLines++;
        } else if (trimmed.find("//") == 0) {
            analysis.commentLines++;
        } else if (trimmed.find("/*") == 0) {
            analysis.commentLines++;
            inMultiLineComment = true;
        } else if (inMultiLineComment) {
            analysis.commentLines++;
            if (trimmed.find("*/") != std::string::npos) {
                inMultiLineComment = false;
            }
        } else {
            analysis.codeLines++;
        }
    }
}

std::string CppAnalyzerNode::removeComments(const std::string& content) const {
    std::string result = content;
    
    // Remove single-line comments
    std::regex singleLineComment(R"(//.*$)");
    result = std::regex_replace(result, singleLineComment, "");
    
    // Remove multi-line comments (simplified)
    std::regex multiLineComment(R"(/\*.*?\*/)");
    result = std::regex_replace(result, multiLineComment, "");
    
    return result;
}

std::string CppAnalyzerNode::removeStrings(const std::string& content) const {
    std::string result;
    bool inString = false;
    bool inChar = false;
    bool escape = false;
    
    for (size_t i = 0; i < content.length(); ++i) {
        char c = content[i];
        
        if (escape) {
            escape = false;
            if (inString || inChar) {
                result += ' '; // Replace with space
            } else {
                result += c;
            }
            continue;
        }
        
        if (c == '\\') {
            escape = true;
            if (!inString && !inChar) {
                result += c;
            }
            continue;
        }
        
        if (c == '"' && !inChar) {
            inString = !inString;
            result += ' '; // Replace string delimiters with space
        } else if (c == '\'' && !inString) {
            inChar = !inChar;
            result += ' '; // Replace char delimiters with space
        } else if (inString || inChar) {
            result += ' '; // Replace string/char content with space
        } else {
            result += c;
        }
    }
    
    return result;
}

foundation::types::NodeValue CppAnalyzerNode::convertAnalysisToNodeValue(const CppFileAnalysis& analysis) const {
    foundation::types::NodeValue::Object result;
    
    result["filePath"] = foundation::types::NodeValue(analysis.filePath);
    result["hasOneClassPerFile"] = foundation::types::NodeValue(analysis.hasOneClassPerFile);
    result["usesModernCpp"] = foundation::types::NodeValue(analysis.usesModernCpp);
    result["hasProperHeaders"] = foundation::types::NodeValue(analysis.hasProperHeaders);
    result["totalLines"] = foundation::types::NodeValue(static_cast<int64_t>(analysis.totalLines));
    result["codeLines"] = foundation::types::NodeValue(static_cast<int64_t>(analysis.codeLines));
    result["commentLines"] = foundation::types::NodeValue(static_cast<int64_t>(analysis.commentLines));
    result["blankLines"] = foundation::types::NodeValue(static_cast<int64_t>(analysis.blankLines));
    
    // Convert classes
    foundation::types::NodeValue::Array classesArray;
    for (const auto& cls : analysis.classes) {
        classesArray.push_back(convertClassInfoToNodeValue(cls));
    }
    result["classes"] = foundation::types::NodeValue(classesArray);
    
    // Convert functions
    foundation::types::NodeValue::Array functionsArray;
    for (const auto& func : analysis.functions) {
        functionsArray.push_back(convertFunctionInfoToNodeValue(func));
    }
    result["functions"] = foundation::types::NodeValue(functionsArray);
    
    // Convert includes
    foundation::types::NodeValue::Array includesArray;
    for (const auto& inc : analysis.includes) {
        foundation::types::NodeValue::Object includeObj;
        includeObj["header"] = foundation::types::NodeValue(inc.header);
        includeObj["isSystemHeader"] = foundation::types::NodeValue(inc.isSystemHeader);
        includeObj["lineNumber"] = foundation::types::NodeValue(static_cast<int64_t>(inc.lineNumber));
        includesArray.push_back(foundation::types::NodeValue(includeObj));
    }
    result["includes"] = foundation::types::NodeValue(includesArray);
    
    // Convert namespaces
    foundation::types::NodeValue::Array namespacesArray;
    for (const auto& ns : analysis.namespaces) {
        namespacesArray.push_back(foundation::types::NodeValue(ns));
    }
    result["namespaces"] = foundation::types::NodeValue(namespacesArray);
    
    // Convert metrics
    foundation::types::NodeValue::Object metricsObj;
    for (const auto& metric : analysis.metrics) {
        try {
            metricsObj[metric.first] = foundation::types::NodeValue(static_cast<int64_t>(metric.second));
        } catch (const std::exception& e) {
            // Skip problematic metrics
            continue;
        }
    }
    result["metrics"] = foundation::types::NodeValue(metricsObj);
    
    // Convert violations
    foundation::types::NodeValue::Array violationsArray;
    for (const auto& violation : analysis.violations) {
        violationsArray.push_back(foundation::types::NodeValue(violation));
    }
    result["violations"] = foundation::types::NodeValue(violationsArray);
    
    return foundation::types::NodeValue(result);
}

foundation::types::NodeValue CppAnalyzerNode::convertClassInfoToNodeValue(const ClassInfo& classInfo) const {
    foundation::types::NodeValue::Object obj;
    
    obj["name"] = foundation::types::NodeValue(classInfo.name);
    obj["fullName"] = foundation::types::NodeValue(classInfo.fullName);
    obj["type"] = foundation::types::NodeValue(classInfo.type);
    obj["isTemplate"] = foundation::types::NodeValue(classInfo.isTemplate);
    obj["isAbstract"] = foundation::types::NodeValue(classInfo.isAbstract);
    obj["lineNumber"] = foundation::types::NodeValue(static_cast<int64_t>(classInfo.lineNumber));
    obj["lineCount"] = foundation::types::NodeValue(static_cast<int64_t>(classInfo.lineCount));
    obj["accessLevel"] = foundation::types::NodeValue(classInfo.accessLevel);
    
    // Convert base classes
    foundation::types::NodeValue::Array baseClassesArray;
    for (const auto& base : classInfo.baseClasses) {
        baseClassesArray.push_back(foundation::types::NodeValue(base));
    }
    obj["baseClasses"] = foundation::types::NodeValue(baseClassesArray);
    
    return foundation::types::NodeValue(obj);
}

foundation::types::NodeValue CppAnalyzerNode::convertFunctionInfoToNodeValue(const FunctionInfo& functionInfo) const {
    foundation::types::NodeValue::Object obj;
    
    obj["name"] = foundation::types::NodeValue(functionInfo.name);
    obj["returnType"] = foundation::types::NodeValue(functionInfo.returnType);
    obj["className"] = foundation::types::NodeValue(functionInfo.className);
    obj["isVirtual"] = foundation::types::NodeValue(functionInfo.isVirtual);
    obj["isPure"] = foundation::types::NodeValue(functionInfo.isPure);
    obj["isConst"] = foundation::types::NodeValue(functionInfo.isConst);
    obj["isStatic"] = foundation::types::NodeValue(functionInfo.isStatic);
    obj["isInline"] = foundation::types::NodeValue(functionInfo.isInline);
    obj["isTemplate"] = foundation::types::NodeValue(functionInfo.isTemplate);
    obj["lineNumber"] = foundation::types::NodeValue(static_cast<int64_t>(functionInfo.lineNumber));
    obj["lineCount"] = foundation::types::NodeValue(static_cast<int64_t>(functionInfo.lineCount));
    obj["complexity"] = foundation::types::NodeValue(static_cast<int64_t>(functionInfo.complexity));
    
    return foundation::types::NodeValue(obj);
}

// Static utility methods
size_t CppAnalyzerNode::countClasses(const std::string& content) {
    std::regex classRegex(R"((class|struct|union)\s+[A-Za-z_][A-Za-z0-9_]*)");
    std::sregex_iterator iter(content.begin(), content.end(), classRegex);
    std::sregex_iterator end;
    
    return std::distance(iter, end);
}

bool CppAnalyzerNode::hasModernCpp(const std::string& content) {
    return content.find("auto ") != std::string::npos ||
           content.find("nullptr") != std::string::npos ||
           content.find("std::unique_ptr") != std::string::npos ||
           content.find("std::shared_ptr") != std::string::npos ||
           content.find("override") != std::string::npos ||
           content.find("constexpr") != std::string::npos;
}

bool CppAnalyzerNode::hasProperHeaders(const std::string& content) {
    return content.find("#include") != std::string::npos &&
           content.find("#pragma once") != std::string::npos;
}

std::vector<std::string> CppAnalyzerNode::getCppClasses(const std::string& content) {
    std::vector<std::string> classes;
    std::regex classRegex(R"((class|struct|union)\s+([A-Za-z_][A-Za-z0-9_]*))");
    std::sregex_iterator iter(content.begin(), content.end(), classRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        classes.push_back((*iter)[2].str());
    }
    
    return classes;
}

std::vector<std::string> CppAnalyzerNode::getCppIncludes(const std::string& content) {
    std::vector<std::string> includes;
    std::regex includeRegex(R"(#include\s*[<"]([^>"]+)[>"])");
    std::sregex_iterator iter(content.begin(), content.end(), includeRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        includes.push_back((*iter)[1].str());
    }
    
    return includes;
}

bool CppAnalyzerNode::validateOneClassRule(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return countClasses(content) <= 1;
}

size_t CppAnalyzerNode::calculateComplexity(const std::string& content) {
    // Simplified complexity calculation
    size_t complexity = 1; // Base complexity
    
    // Count decision points
    std::vector<std::string> keywords = {"if", "else", "while", "for", "switch", "case", "catch", "&&", "||", "?"};
    
    for (const auto& keyword : keywords) {
        size_t pos = 0;
        while ((pos = content.find(keyword, pos)) != std::string::npos) {
            complexity++;
            pos += keyword.length();
        }
    }
    
    return complexity;
}

bool CppAnalyzerNode::usesStandardLibrary(const std::string& content) {
    return content.find("std::") != std::string::npos ||
           content.find("#include <") != std::string::npos;
}

} // namespace external
} // namespace nodes
} // namespace akao