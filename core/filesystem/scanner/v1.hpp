/**
 * @id: akao:file:core:filesystem:scanner_directory_scanner:v1
 * @doc: Universal directory scanner providing comprehensive filesystem analysis, project structure introspection, and content discovery capabilities for any project type. Implements deep scanning with language detection, file type classification, content analysis, and project pattern recognition with configurable inclusion/exclusion rules and performance optimization for large codebases.
 * @specification: Universal filesystem scanning system with content analysis, language detection, and project introspection
 * @scope: Core filesystem scanning subsystem
 * @timeline: 2025-07-09
 * @rationale: Provide comprehensive filesystem analysis capabilities for structure validation, project discovery, and content introspection
 * @methodology: Recursive scanning algorithms with content analysis, language detection, and configurable filtering
 * @references: ["akao:philosophy:language:isolation:v1", "akao:philosophy:structure:enforcement:v1", "akao:philosophy:measurement:observability:v1"]
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <filesystem>
#include <functional>

namespace akao::core::filesystem::scanner {

/**
 * @brief File information structure
 */
struct FileInfo {
    std::string path;
    std::string relative_path;
    std::string filename;
    std::string extension;
    std::string type;  // "source", "config", "build", "docs", "test", "other"
    size_t size_bytes;
    std::filesystem::file_time_type last_modified;
    
    // Content analysis
    size_t line_count = 0;
    std::vector<std::string> languages;
    std::map<std::string, std::string> metadata;
};

/**
 * @brief Directory information structure
 */
struct DirectoryInfo {
    std::string path;
    std::string name;
    size_t total_files = 0;
    size_t total_subdirectories = 0;
    std::map<std::string, size_t> file_type_counts;
    std::vector<FileInfo> files;
    std::vector<DirectoryInfo> subdirectories;
};

/**
 * @brief Scanning configuration
 */
struct ScanConfig {
    // Inclusion/exclusion patterns
    std::vector<std::string> include_patterns;
    std::vector<std::string> exclude_patterns;
    std::vector<std::string> exclude_directories = {".git", ".svn", "node_modules", "__pycache__", "target", "build"};
    
    // File size limits
    size_t max_file_size_bytes = 100 * 1024 * 1024; // 100MB
    
    // Analysis options
    bool analyze_content = true;
    bool detect_languages = true;
    bool calculate_metrics = true;
    bool follow_symlinks = false;
    
    // Performance settings
    size_t max_depth = 50;
    bool parallel_scanning = true;
    size_t max_threads = 0; // 0 = auto-detect
};

/**
 * @brief Universal directory scanner for Akao framework
 * 
 * Implements comprehensive filesystem scanning and analysis that works with
 * any project type and language. Provides deep introspection into project
 * structure, file types, and content patterns.
 * 
 * Philosophy compliance:
 * - akao:philosophy:language:isolation:v1 - Language-agnostic scanning
 * - akao:philosophy:structure:enforcement:v1 - Structure analysis and validation
 * - akao:philosophy:measurement:observability:v1 - Comprehensive metrics collection
 */
class DirectoryScanner {
public:
    /**
     * @brief Scanning result
     */
    struct ScanResult {
        bool success = false;
        std::string error_message;
        DirectoryInfo root_directory;
        
        // Summary statistics
        size_t total_files = 0;
        size_t total_directories = 0;
        size_t total_size_bytes = 0;
        size_t total_lines = 0;
        
        // File type distribution
        std::map<std::string, size_t> file_type_counts;
        std::map<std::string, size_t> extension_counts;
        std::map<std::string, size_t> language_counts;
        
        // Analysis metadata
        double scan_duration_seconds = 0.0;
        std::vector<std::string> warnings;
        std::vector<std::string> skipped_files;
    };

    /**
     * @brief Constructor
     */
    explicit DirectoryScanner(const ScanConfig& config = ScanConfig{});

    /**
     * @brief Set scanning configuration
     */
    void setConfig(const ScanConfig& config);

    /**
     * @brief Get current configuration
     */
    const ScanConfig& getConfig() const;

    /**
     * @brief Scan a directory recursively
     * @param directory_path Path to the directory to scan
     * @return Scanning result with complete directory structure
     */
    ScanResult scanDirectory(const std::string& directory_path);

    /**
     * @brief Scan a single file
     * @param file_path Path to the file to scan
     * @return File information
     */
    FileInfo scanFile(const std::string& file_path);

    /**
     * @brief Quick scan for basic structure only (no content analysis)
     * @param directory_path Path to the directory to scan
     * @return Basic directory structure
     */
    ScanResult quickScan(const std::string& directory_path);

    /**
     * @brief Scan for specific file patterns
     * @param directory_path Path to the directory to scan
     * @param patterns File patterns to match (glob-style)
     * @return Files matching the patterns
     */
    std::vector<FileInfo> scanForPatterns(const std::string& directory_path, 
                                         const std::vector<std::string>& patterns);

    /**
     * @brief Find files by type
     * @param directory_path Path to the directory to scan
     * @param file_type File type to find ("source", "config", "build", etc.)
     * @return Files of the specified type
     */
    std::vector<FileInfo> findFilesByType(const std::string& directory_path, 
                                         const std::string& file_type);

    /**
     * @brief Find files by language
     * @param directory_path Path to the directory to scan
     * @param language Language to find ("cpp", "python", "javascript", etc.)
     * @return Files in the specified language
     */
    std::vector<FileInfo> findFilesByLanguage(const std::string& directory_path, 
                                             const std::string& language);

    /**
     * @brief Detect project type based on directory structure
     * @param directory_path Path to the directory to analyze
     * @return Detected project type
     */
    std::string detectProjectType(const std::string& directory_path);

    /**
     * @brief Get project languages distribution
     * @param directory_path Path to the directory to analyze
     * @return Language distribution map
     */
    std::map<std::string, size_t> getLanguageDistribution(const std::string& directory_path);

    /**
     * @brief Check if directory structure matches expected pattern
     * @param directory_path Path to the directory to check
     * @param expected_structure Expected structure pattern
     * @return True if structure matches
     */
    bool validateStructure(const std::string& directory_path, 
                          const std::map<std::string, std::vector<std::string>>& expected_structure);

    /**
     * @brief Get scanning statistics
     */
    struct ScanningStats {
        size_t total_scans = 0;
        size_t successful_scans = 0;
        size_t failed_scans = 0;
        double total_scan_time = 0.0;
        size_t total_files_scanned = 0;
        size_t total_bytes_scanned = 0;
    };

    const ScanningStats& getStats() const;

    /**
     * @brief Clear scanning statistics
     */
    void clearStats();

private:
    ScanConfig config_;
    ScanningStats stats_;

    // Core scanning methods
    DirectoryInfo scanDirectoryRecursive(const std::filesystem::path& path, size_t current_depth);
    FileInfo analyzeFile(const std::filesystem::path& path);
    
    // File type detection
    std::string detectFileType(const std::filesystem::path& path);
    std::vector<std::string> detectLanguages(const std::filesystem::path& path);
    
    // Content analysis
    size_t countLines(const std::string& file_path);
    std::map<std::string, std::string> extractMetadata(const std::filesystem::path& path);
    
    // Pattern matching
    bool matchesPattern(const std::string& path, const std::string& pattern);
    bool shouldExclude(const std::filesystem::path& path);
    bool shouldInclude(const std::filesystem::path& path);
    
    // Language detection by extension
    std::string detectLanguageByExtension(const std::string& extension);
    
    // Project type detection helpers
    bool isAkaoProject(const std::filesystem::path& path);
    bool isCppProject(const std::filesystem::path& path);
    bool isPythonProject(const std::filesystem::path& path);
    bool isJavaScriptProject(const std::filesystem::path& path);
    bool isRustProject(const std::filesystem::path& path);
    bool isGoProject(const std::filesystem::path& path);
    
    // Statistics helpers
    void updateStats(const ScanResult& result);
    
    // Error handling
    void addWarning(ScanResult& result, const std::string& warning);
    void addSkippedFile(ScanResult& result, const std::string& file_path, const std::string& reason);
};

/**
 * @brief Utility functions for filesystem operations
 */
namespace utils {

/**
 * @brief Check if path is safe to access (no path traversal attacks)
 */
bool isSafePath(const std::string& path);

/**
 * @brief Normalize path separators for current platform
 */
std::string normalizePath(const std::string& path);

/**
 * @brief Get relative path from base
 */
std::string getRelativePath(const std::string& path, const std::string& base);

/**
 * @brief Check if path matches glob pattern
 */
bool matchesGlob(const std::string& path, const std::string& pattern);

/**
 * @brief Get file size safely
 */
size_t getFileSize(const std::filesystem::path& path);

/**
 * @brief Check if file is text file (not binary)
 */
bool isTextFile(const std::filesystem::path& path);

/**
 * @brief Get file content type (MIME-like)
 */
std::string getContentType(const std::filesystem::path& path);

} // namespace utils

} // namespace akao::core::filesystem::scanner
