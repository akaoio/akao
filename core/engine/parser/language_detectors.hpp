#pragma once

/**
 * @file language_detectors.hpp
 * @brief Automatic language detection for universal parser
 * @details Provides language detection from file content, extensions, and syntax patterns
 * 
 * ID: akao:component:core:engine:parser:language-detectors:v1
 * Scope: Multi-language detection and classification system
 * Dependencies: None (zero-dependency implementation)
 */

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <regex>

namespace akao {
namespace core {
namespace engine {
namespace parser {

/**
 * @brief Supported programming languages
 */
enum class LanguageType {
    UNKNOWN,
    C,
    CPP,
    PYTHON,
    JAVASCRIPT,
    YAML,
    JSON,
    PROLOG,
    PURE_LOGIC
};

/**
 * @brief Language detection confidence level
 */
enum class ConfidenceLevel {
    LOW = 1,
    MEDIUM = 2,
    HIGH = 3,
    CERTAIN = 4
};

/**
 * @brief Language detection result
 */
struct DetectionResult {
    LanguageType language = LanguageType::UNKNOWN;
    ConfidenceLevel confidence = ConfidenceLevel::LOW;
    std::string detected_version;
    std::vector<std::string> detected_features;
    std::map<std::string, double> confidence_scores;
    std::string reasoning;
};

/**
 * @brief File extension patterns
 */
struct ExtensionPattern {
    std::vector<std::string> extensions;
    LanguageType language;
    ConfidenceLevel confidence;
};

/**
 * @brief Content pattern for detection
 */
struct ContentPattern {
    std::string pattern;
    LanguageType language;
    double weight;
    std::string description;
};

/**
 * @brief Syntax signature for language identification
 */
struct SyntaxSignature {
    std::vector<std::string> keywords;
    std::vector<std::string> operators;
    std::string comment_style;
    std::string string_delimiter;
    LanguageType language;
    double match_threshold;
};

/**
 * @brief Abstract base class for language detectors
 */
class LanguageDetector {
public:
    virtual ~LanguageDetector() = default;
    
    /**
     * @brief Detect language from file content
     * @param content File content
     * @return Detection result
     */
    virtual DetectionResult detect_from_content(const std::string& content) = 0;
    
    /**
     * @brief Get supported language
     * @return Language type
     */
    virtual LanguageType get_supported_language() const = 0;
    
    /**
     * @brief Get detector name
     * @return Detector name
     */
    virtual std::string get_detector_name() const = 0;
};

/**
 * @brief C language detector
 */
class CDetector : public LanguageDetector {
public:
    DetectionResult detect_from_content(const std::string& content) override;
    LanguageType get_supported_language() const override { return LanguageType::C; }
    std::string get_detector_name() const override { return "C Language Detector"; }
    
private:
    bool has_c_headers(const std::string& content);
    bool has_c_keywords(const std::string& content);
    bool has_c_syntax_patterns(const std::string& content);
    std::string detect_c_standard(const std::string& content);
};

/**
 * @brief C++ language detector
 */
class CppDetector : public LanguageDetector {
public:
    DetectionResult detect_from_content(const std::string& content) override;
    LanguageType get_supported_language() const override { return LanguageType::CPP; }
    std::string get_detector_name() const override { return "C++ Language Detector"; }
    
private:
    bool has_cpp_headers(const std::string& content);
    bool has_cpp_keywords(const std::string& content);
    bool has_cpp_features(const std::string& content);
    bool has_templates(const std::string& content);
    bool has_namespaces(const std::string& content);
    std::string detect_cpp_standard(const std::string& content);
};

/**
 * @brief Python language detector
 */
class PythonDetector : public LanguageDetector {
public:
    DetectionResult detect_from_content(const std::string& content) override;
    LanguageType get_supported_language() const override { return LanguageType::PYTHON; }
    std::string get_detector_name() const override { return "Python Language Detector"; }
    
private:
    bool has_python_keywords(const std::string& content);
    bool has_python_syntax(const std::string& content);
    bool has_python_imports(const std::string& content);
    bool has_indentation_structure(const std::string& content);
    std::string detect_python_version(const std::string& content);
};

/**
 * @brief JavaScript language detector
 */
class JavaScriptDetector : public LanguageDetector {
public:
    DetectionResult detect_from_content(const std::string& content) override;
    LanguageType get_supported_language() const override { return LanguageType::JAVASCRIPT; }
    std::string get_detector_name() const override { return "JavaScript Language Detector"; }
    
private:
    bool has_javascript_keywords(const std::string& content);
    bool has_javascript_syntax(const std::string& content);
    bool has_modern_features(const std::string& content);
    bool has_nodejs_patterns(const std::string& content);
    bool has_browser_patterns(const std::string& content);
    std::string detect_ecmascript_version(const std::string& content);
};

/**
 * @brief YAML language detector
 */
class YamlDetector : public LanguageDetector {
public:
    DetectionResult detect_from_content(const std::string& content) override;
    LanguageType get_supported_language() const override { return LanguageType::YAML; }
    std::string get_detector_name() const override { return "YAML Language Detector"; }
    
private:
    bool has_yaml_structure(const std::string& content);
    bool has_yaml_syntax(const std::string& content);
    bool has_document_separators(const std::string& content);
    bool has_anchors_and_aliases(const std::string& content);
    std::string detect_yaml_version(const std::string& content);
};

/**
 * @brief JSON language detector
 */
class JsonDetector : public LanguageDetector {
public:
    DetectionResult detect_from_content(const std::string& content) override;
    LanguageType get_supported_language() const override { return LanguageType::JSON; }
    std::string get_detector_name() const override { return "JSON Language Detector"; }
    
private:
    bool has_json_structure(const std::string& content);
    bool has_valid_json_syntax(const std::string& content);
    bool is_json5_format(const std::string& content);
    bool validate_json_format(const std::string& content);
};

/**
 * @brief Prolog language detector
 */
class PrologDetector : public LanguageDetector {
public:
    DetectionResult detect_from_content(const std::string& content) override;
    LanguageType get_supported_language() const override { return LanguageType::PROLOG; }
    std::string get_detector_name() const override { return "Prolog Language Detector"; }
    
private:
    bool has_prolog_facts(const std::string& content);
    bool has_prolog_rules(const std::string& content);
    bool has_prolog_queries(const std::string& content);
    bool has_prolog_syntax(const std::string& content);
    std::string detect_prolog_dialect(const std::string& content);
};

/**
 * @brief Pure Logic (.a) language detector
 */
class PureLogicDetector : public LanguageDetector {
public:
    DetectionResult detect_from_content(const std::string& content) override;
    LanguageType get_supported_language() const override { return LanguageType::PURE_LOGIC; }
    std::string get_detector_name() const override { return "Pure Logic Language Detector"; }
    
private:
    bool has_pure_logic_symbols(const std::string& content);
    bool has_logic_operators(const std::string& content);
    bool has_dual_mode_syntax(const std::string& content);
    bool has_accessibility_features(const std::string& content);
};

/**
 * @brief Main language detection coordinator
 */
class UniversalLanguageDetector {
public:
    UniversalLanguageDetector();
    ~UniversalLanguageDetector();
    
    /**
     * @brief Detect language from file path
     * @param filepath File path with extension
     * @return Detection result
     */
    DetectionResult detect_from_filepath(const std::string& filepath);
    
    /**
     * @brief Detect language from file content
     * @param content File content
     * @param filepath Optional file path for additional context
     * @return Detection result
     */
    DetectionResult detect_from_content(const std::string& content, 
                                        const std::string& filepath = "");
    
    /**
     * @brief Detect language with combined methods
     * @param content File content
     * @param filepath File path
     * @return Detection result with highest confidence
     */
    DetectionResult detect_language(const std::string& content, 
                                    const std::string& filepath);
    
    /**
     * @brief Register custom language detector
     * @param detector Language detector implementation
     */
    void register_detector(std::unique_ptr<LanguageDetector> detector);
    
    /**
     * @brief Get supported languages
     * @return List of supported language types
     */
    std::vector<LanguageType> get_supported_languages() const;
    
    /**
     * @brief Get language name from type
     * @param language Language type
     * @return Human-readable language name
     */
    static std::string get_language_name(LanguageType language);
    
    /**
     * @brief Get file extensions for language
     * @param language Language type
     * @return List of file extensions
     */
    static std::vector<std::string> get_language_extensions(LanguageType language);
    
private:
    std::vector<std::unique_ptr<LanguageDetector>> detectors_;
    std::vector<ExtensionPattern> extension_patterns_;
    std::vector<ContentPattern> content_patterns_;
    std::vector<SyntaxSignature> syntax_signatures_;
    
    void initialize_built_in_detectors();
    void initialize_extension_patterns();
    void initialize_content_patterns();
    void initialize_syntax_signatures();
    
    DetectionResult detect_by_extension(const std::string& filepath);
    DetectionResult detect_by_content_patterns(const std::string& content);
    DetectionResult detect_by_syntax_signatures(const std::string& content);
    
    DetectionResult combine_results(const std::vector<DetectionResult>& results);
    double calculate_confidence_score(const DetectionResult& result);
    
    std::string extract_file_extension(const std::string& filepath);
    std::vector<std::string> extract_keywords(const std::string& content);
    std::vector<std::string> extract_operators(const std::string& content);
    
    bool matches_pattern(const std::string& content, const std::string& pattern);
    double calculate_keyword_similarity(const std::vector<std::string>& keywords1,
                                        const std::vector<std::string>& keywords2);
};

/**
 * @brief Utility functions for language detection
 */
namespace detection_utils {
    /**
     * @brief Extract shebang line from content
     * @param content File content
     * @return Shebang line or empty string
     */
    std::string extract_shebang(const std::string& content);
    
    /**
     * @brief Count specific pattern occurrences
     * @param content File content
     * @param pattern Pattern to count
     * @return Number of occurrences
     */
    int count_pattern_occurrences(const std::string& content, const std::string& pattern);
    
    /**
     * @brief Extract comment blocks
     * @param content File content
     * @param comment_style Comment style pattern
     * @return List of comment blocks
     */
    std::vector<std::string> extract_comments(const std::string& content, 
                                               const std::string& comment_style);
    
    /**
     * @brief Calculate indentation consistency
     * @param content File content
     * @return Indentation consistency score (0.0 to 1.0)
     */
    double calculate_indentation_consistency(const std::string& content);
    
    /**
     * @brief Detect encoding format
     * @param content File content
     * @return Detected encoding
     */
    std::string detect_encoding(const std::string& content);
    
    /**
     * @brief Normalize whitespace for analysis
     * @param content File content
     * @return Normalized content
     */
    std::string normalize_whitespace(const std::string& content);
    
    /**
     * @brief Split content into lines
     * @param content File content
     * @return Vector of lines
     */
    std::vector<std::string> split_lines(const std::string& content);
    
    /**
     * @brief Check if line is likely code
     * @param line Line content
     * @return True if line appears to be code
     */
    bool is_code_line(const std::string& line);
}

} // namespace parser
} // namespace engine
} // namespace core
} // namespace akao
