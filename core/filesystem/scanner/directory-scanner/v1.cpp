#include "v1.hpp"
#include <chrono>
#include <algorithm>
#include <fstream>
#include <regex>
#include <thread>
#include <future>

namespace akao::core::filesystem::scanner {

DirectoryScanner::DirectoryScanner(const ScanConfig& config) : config_(config) {}

void DirectoryScanner::setConfig(const ScanConfig& config) {
    config_ = config;
}

const ScanConfig& DirectoryScanner::getConfig() const {
    return config_;
}

DirectoryScanner::ScanResult DirectoryScanner::scanDirectory(const std::string& directory_path) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    ScanResult result;
    
    try {
        std::filesystem::path path(directory_path);
        
        if (!std::filesystem::exists(path)) {
            result.error_message = "Directory does not exist: " + directory_path;
            return result;
        }
        
        if (!std::filesystem::is_directory(path)) {
            result.error_message = "Path is not a directory: " + directory_path;
            return result;
        }
        
        // Perform recursive scan
        result.root_directory = scanDirectoryRecursive(path, 0);
        result.success = true;
        
        // Calculate summary statistics
        std::function<void(const DirectoryInfo&)> calculateStats = [&](const DirectoryInfo& dir) {
            result.total_directories++;
            
            for (const auto& file : dir.files) {
                result.total_files++;
                result.total_size_bytes += file.size_bytes;
                result.total_lines += file.line_count;
                
                result.file_type_counts[file.type]++;
                result.extension_counts[file.extension]++;
                
                for (const auto& lang : file.languages) {
                    result.language_counts[lang]++;
                }
            }
            
            for (const auto& subdir : dir.subdirectories) {
                calculateStats(subdir);
            }
        };
        
        calculateStats(result.root_directory);
        
    } catch (const std::exception& e) {
        result.success = false;
        result.error_message = "Scanning failed: " + std::string(e.what());
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time);
    result.scan_duration_seconds = duration.count();
    
    updateStats(result);
    
    return result;
}

FileInfo DirectoryScanner::scanFile(const std::string& file_path) {
    std::filesystem::path path(file_path);
    return analyzeFile(path);
}

DirectoryScanner::ScanResult DirectoryScanner::quickScan(const std::string& directory_path) {
    // Temporarily disable content analysis for quick scan
    auto original_config = config_;
    config_.analyze_content = false;
    config_.detect_languages = false;
    config_.calculate_metrics = false;
    
    auto result = scanDirectory(directory_path);
    
    // Restore original config
    config_ = original_config;
    
    return result;
}

std::vector<FileInfo> DirectoryScanner::scanForPatterns(const std::string& directory_path, 
                                                       const std::vector<std::string>& patterns) {
    std::vector<FileInfo> matching_files;
    
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory_path)) {
            if (entry.is_regular_file()) {
                std::string file_path = entry.path().string();
                
                // Check if file matches any pattern
                for (const auto& pattern : patterns) {
                    if (matchesPattern(file_path, pattern)) {
                        matching_files.push_back(analyzeFile(entry.path()));
                        break;
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        // Log error but continue
    }
    
    return matching_files;
}

std::vector<FileInfo> DirectoryScanner::findFilesByType(const std::string& directory_path, 
                                                       const std::string& file_type) {
    std::vector<FileInfo> matching_files;
    
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory_path)) {
            if (entry.is_regular_file() && !shouldExclude(entry.path())) {
                auto file_info = analyzeFile(entry.path());
                if (file_info.type == file_type) {
                    matching_files.push_back(file_info);
                }
            }
        }
    } catch (const std::exception& e) {
        // Log error but continue
    }
    
    return matching_files;
}

std::vector<FileInfo> DirectoryScanner::findFilesByLanguage(const std::string& directory_path, 
                                                           const std::string& language) {
    std::vector<FileInfo> matching_files;
    
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory_path)) {
            if (entry.is_regular_file() && !shouldExclude(entry.path())) {
                auto file_info = analyzeFile(entry.path());
                if (std::find(file_info.languages.begin(), file_info.languages.end(), language) 
                    != file_info.languages.end()) {
                    matching_files.push_back(file_info);
                }
            }
        }
    } catch (const std::exception& e) {
        // Log error but continue
    }
    
    return matching_files;
}

std::string DirectoryScanner::detectProjectType(const std::string& directory_path) {
    std::filesystem::path path(directory_path);
    
    if (isAkaoProject(path)) return "akao";
    if (isCppProject(path)) return "cpp";
    if (isPythonProject(path)) return "python";
    if (isJavaScriptProject(path)) return "javascript";
    if (isRustProject(path)) return "rust";
    if (isGoProject(path)) return "go";
    
    return "unknown";
}

std::map<std::string, size_t> DirectoryScanner::getLanguageDistribution(const std::string& directory_path) {
    auto result = scanDirectory(directory_path);
    return result.language_counts;
}

bool DirectoryScanner::validateStructure(const std::string& directory_path, 
                                        const std::map<std::string, std::vector<std::string>>& expected_structure) {
    // Simplified structure validation
    try {
        for (const auto& [dir_name, required_files] : expected_structure) {
            std::filesystem::path dir_path = std::filesystem::path(directory_path) / dir_name;
            
            if (!std::filesystem::exists(dir_path) || !std::filesystem::is_directory(dir_path)) {
                return false;
            }
            
            for (const auto& required_file : required_files) {
                std::filesystem::path file_path = dir_path / required_file;
                if (!std::filesystem::exists(file_path)) {
                    return false;
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

const DirectoryScanner::ScanningStats& DirectoryScanner::getStats() const {
    return stats_;
}

void DirectoryScanner::clearStats() {
    stats_ = ScanningStats{};
}

// Private methods implementation
DirectoryInfo DirectoryScanner::scanDirectoryRecursive(const std::filesystem::path& path, size_t current_depth) {
    DirectoryInfo dir_info;
    dir_info.path = path.string();
    dir_info.name = path.filename().string();
    
    if (current_depth >= config_.max_depth) {
        return dir_info;
    }
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (shouldExclude(entry.path())) {
                continue;
            }
            
            if (entry.is_regular_file()) {
                if (shouldInclude(entry.path())) {
                    auto file_info = analyzeFile(entry.path());
                    dir_info.files.push_back(file_info);
                    dir_info.total_files++;
                    dir_info.file_type_counts[file_info.type]++;
                }
            } else if (entry.is_directory()) {
                auto subdir_info = scanDirectoryRecursive(entry.path(), current_depth + 1);
                dir_info.subdirectories.push_back(subdir_info);
                dir_info.total_subdirectories++;
                
                // Aggregate counts from subdirectories
                dir_info.total_files += subdir_info.total_files;
                for (const auto& [type, count] : subdir_info.file_type_counts) {
                    dir_info.file_type_counts[type] += count;
                }
            }
        }
    } catch (const std::exception& e) {
        // Log error but continue
    }
    
    return dir_info;
}

FileInfo DirectoryScanner::analyzeFile(const std::filesystem::path& path) {
    FileInfo file_info;
    file_info.path = path.string();
    file_info.relative_path = std::filesystem::relative(path).string();
    file_info.filename = path.filename().string();
    file_info.extension = path.extension().string();
    
    try {
        file_info.size_bytes = std::filesystem::file_size(path);
        file_info.last_modified = std::filesystem::last_write_time(path);
        
        file_info.type = detectFileType(path);
        
        if (config_.detect_languages) {
            file_info.languages = detectLanguages(path);
        }
        
        if (config_.analyze_content && utils::isTextFile(path)) {
            file_info.line_count = countLines(path.string());
        }
        
        if (config_.calculate_metrics) {
            file_info.metadata = extractMetadata(path);
        }
        
    } catch (const std::exception& e) {
        // Set error in metadata
        file_info.metadata["error"] = e.what();
    }
    
    return file_info;
}

std::string DirectoryScanner::detectFileType(const std::filesystem::path& path) {
    std::string filename = path.filename().string();
    std::string extension = path.extension().string();
    
    // Convert to lowercase for comparison
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);
    
    // Source files
    if (extension == ".cpp" || extension == ".hpp" || extension == ".h" || 
        extension == ".cc" || extension == ".cxx" || extension == ".c" ||
        extension == ".py" || extension == ".js" || extension == ".ts" ||
        extension == ".rs" || extension == ".go" || extension == ".java" ||
        extension == ".kt" || extension == ".swift" || extension == ".rb" ||
        extension == ".php" || extension == ".cs" || extension == ".vb") {
        return "source";
    }
    
    // Configuration files
    if (extension == ".yaml" || extension == ".yml" || extension == ".json" ||
        extension == ".toml" || extension == ".ini" || extension == ".cfg" ||
        extension == ".conf" || extension == ".config" || extension == ".xml") {
        return "config";
    }
    
    // Build files
    if (filename == "makefile" || filename == "cmakelist.txt" || 
        filename == "cargo.toml" || filename == "package.json" ||
        filename == "setup.py" || filename == "pyproject.toml" ||
        filename == "build.gradle" || filename == "pom.xml") {
        return "build";
    }
    
    // Documentation files
    if (extension == ".md" || extension == ".rst" || extension == ".txt" ||
        filename.find("readme") != std::string::npos ||
        filename.find("changelog") != std::string::npos ||
        filename.find("license") != std::string::npos) {
        return "docs";
    }
    
    // Test files
    if (filename.find("test") != std::string::npos ||
        filename.find("spec") != std::string::npos ||
        path.string().find("/test/") != std::string::npos ||
        path.string().find("/tests/") != std::string::npos) {
        return "test";
    }
    
    return "other";
}

std::vector<std::string> DirectoryScanner::detectLanguages(const std::filesystem::path& path) {
    std::vector<std::string> languages;
    std::string extension = path.extension().string();
    
    std::string language = detectLanguageByExtension(extension);
    if (!language.empty()) {
        languages.push_back(language);
    }
    
    return languages;
}

size_t DirectoryScanner::countLines(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        return 0;
    }
    
    size_t line_count = 0;
    std::string line;
    while (std::getline(file, line)) {
        line_count++;
    }
    
    return line_count;
}

std::map<std::string, std::string> DirectoryScanner::extractMetadata(const std::filesystem::path& path) {
    std::map<std::string, std::string> metadata;
    
    metadata["content_type"] = utils::getContentType(path);
    metadata["is_text"] = utils::isTextFile(path) ? "true" : "false";
    
    return metadata;
}

bool DirectoryScanner::matchesPattern(const std::string& path, const std::string& pattern) {
    return utils::matchesGlob(path, pattern);
}

bool DirectoryScanner::shouldExclude(const std::filesystem::path& path) {
    std::string path_str = path.string();
    std::string filename = path.filename().string();
    
    // Check exclude directories
    for (const auto& exclude_dir : config_.exclude_directories) {
        if (path_str.find("/" + exclude_dir + "/") != std::string::npos ||
            path_str.find("\\" + exclude_dir + "\\") != std::string::npos ||
            filename == exclude_dir) {
            return true;
        }
    }
    
    // Check exclude patterns
    for (const auto& pattern : config_.exclude_patterns) {
        if (matchesPattern(path_str, pattern)) {
            return true;
        }
    }
    
    return false;
}

bool DirectoryScanner::shouldInclude(const std::filesystem::path& path) {
    // If no include patterns specified, include everything (subject to exclusions)
    if (config_.include_patterns.empty()) {
        return true;
    }
    
    std::string path_str = path.string();
    
    // Check include patterns
    for (const auto& pattern : config_.include_patterns) {
        if (matchesPattern(path_str, pattern)) {
            return true;
        }
    }
    
    return false;
}

std::string DirectoryScanner::detectLanguageByExtension(const std::string& extension) {
    std::string ext = extension;
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".cpp" || ext == ".hpp" || ext == ".h" || ext == ".cc" || ext == ".cxx") {
        return "cpp";
    } else if (ext == ".py") {
        return "python";
    } else if (ext == ".js") {
        return "javascript";
    } else if (ext == ".ts") {
        return "typescript";
    } else if (ext == ".rs") {
        return "rust";
    } else if (ext == ".go") {
        return "go";
    } else if (ext == ".java") {
        return "java";
    } else if (ext == ".kt") {
        return "kotlin";
    } else if (ext == ".swift") {
        return "swift";
    } else if (ext == ".rb") {
        return "ruby";
    } else if (ext == ".php") {
        return "php";
    } else if (ext == ".cs") {
        return "csharp";
    }
    
    return "";
}

// Project type detection helpers
bool DirectoryScanner::isAkaoProject(const std::filesystem::path& path) {
    return std::filesystem::exists(path / "philosophies") &&
           std::filesystem::exists(path / "rules");
}

bool DirectoryScanner::isCppProject(const std::filesystem::path& path) {
    return std::filesystem::exists(path / "CMakeLists.txt") ||
           std::filesystem::exists(path / "Makefile") ||
           std::filesystem::exists(path / "configure") ||
           std::filesystem::exists(path / "meson.build");
}

bool DirectoryScanner::isPythonProject(const std::filesystem::path& path) {
    return std::filesystem::exists(path / "setup.py") ||
           std::filesystem::exists(path / "pyproject.toml") ||
           std::filesystem::exists(path / "requirements.txt") ||
           std::filesystem::exists(path / "Pipfile");
}

bool DirectoryScanner::isJavaScriptProject(const std::filesystem::path& path) {
    return std::filesystem::exists(path / "package.json") ||
           std::filesystem::exists(path / "yarn.lock") ||
           std::filesystem::exists(path / "package-lock.json");
}

bool DirectoryScanner::isRustProject(const std::filesystem::path& path) {
    return std::filesystem::exists(path / "Cargo.toml") ||
           std::filesystem::exists(path / "Cargo.lock");
}

bool DirectoryScanner::isGoProject(const std::filesystem::path& path) {
    return std::filesystem::exists(path / "go.mod") ||
           std::filesystem::exists(path / "go.sum");
}

void DirectoryScanner::updateStats(const ScanResult& result) {
    stats_.total_scans++;
    if (result.success) {
        stats_.successful_scans++;
    } else {
        stats_.failed_scans++;
    }
    stats_.total_scan_time += result.scan_duration_seconds;
    stats_.total_files_scanned += result.total_files;
    stats_.total_bytes_scanned += result.total_size_bytes;
}

// Utility functions implementation
namespace utils {

bool isSafePath(const std::string& path) {
    // Check for path traversal attempts
    return path.find("..") == std::string::npos &&
           path.find("~") != 0;  // Don't allow home directory access
}

std::string normalizePath(const std::string& path) {
    std::filesystem::path p(path);
    return p.lexically_normal().string();
}

std::string getRelativePath(const std::string& path, const std::string& base) {
    std::filesystem::path p(path);
    std::filesystem::path b(base);
    return std::filesystem::relative(p, b).string();
}

bool matchesGlob(const std::string& path, const std::string& pattern) {
    // Convert glob pattern to regex
    std::string regex_pattern = pattern;
    
    // Escape special regex characters except * and ?
    std::regex special_chars(R"([\[\](){}+.^$|\\])");
    regex_pattern = std::regex_replace(regex_pattern, special_chars, R"(\$&)");
    
    // Convert glob wildcards to regex
    std::regex glob_star(R"(\\\*)");  // * -> .*
    regex_pattern = std::regex_replace(regex_pattern, glob_star, ".*");
    
    std::regex glob_question(R"(\\\?)");  // ? -> .
    regex_pattern = std::regex_replace(regex_pattern, glob_question, ".");
    
    std::regex pattern_regex(regex_pattern);
    return std::regex_match(path, pattern_regex);
}

size_t getFileSize(const std::filesystem::path& path) {
    try {
        return std::filesystem::file_size(path);
    } catch (const std::exception&) {
        return 0;
    }
}

bool isTextFile(const std::filesystem::path& path) {
    std::string extension = path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    // Known text file extensions
    std::set<std::string> text_extensions = {
        ".txt", ".md", ".rst", ".yaml", ".yml", ".json", ".toml", ".ini",
        ".cfg", ".conf", ".config", ".xml", ".html", ".htm", ".css",
        ".js", ".ts", ".py", ".cpp", ".hpp", ".h", ".c", ".cc", ".cxx",
        ".rs", ".go", ".java", ".kt", ".swift", ".rb", ".php", ".cs",
        ".vb", ".sql", ".sh", ".bat", ".ps1", ".dockerfile", ".gitignore"
    };
    
    if (text_extensions.count(extension) > 0) {
        return true;
    }
    
    // Check if file has no extension and might be text
    if (extension.empty()) {
        std::string filename = path.filename().string();
        std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);
        
        std::set<std::string> text_files = {
            "readme", "license", "changelog", "authors", "contributors",
            "makefile", "dockerfile", "gitignore", "gitattributes"
        };
        
        for (const auto& text_file : text_files) {
            if (filename.find(text_file) != std::string::npos) {
                return true;
            }
        }
    }
    
    return false;
}

std::string getContentType(const std::filesystem::path& path) {
    std::string extension = path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".json") return "application/json";
    if (extension == ".yaml" || extension == ".yml") return "text/yaml";
    if (extension == ".xml") return "text/xml";
    if (extension == ".html" || extension == ".htm") return "text/html";
    if (extension == ".css") return "text/css";
    if (extension == ".js") return "application/javascript";
    if (extension == ".cpp" || extension == ".hpp" || extension == ".h") return "text/x-c++src";
    if (extension == ".py") return "text/x-python";
    if (extension == ".rs") return "text/x-rust";
    if (extension == ".go") return "text/x-go";
    
    if (isTextFile(path)) {
        return "text/plain";
    }
    
    return "application/octet-stream";
}

} // namespace utils

} // namespace akao::core::filesystem::scanner
