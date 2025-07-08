#pragma once

/**
 * @file build_engine.hpp
 * @brief Build system engine for Akao .a file compilation and executable generation
 * @details Provides compilation pipeline, optimization, and build artifact management
 * 
 * ID: akao:component:builders:akao:build-engine:v1
 * Scope: .a file compilation, build pipeline, and executable generation
 * Dependencies: core/engine/language/pure_logic_grammar.hpp, core/engine/runtime/project_manager.hpp
 */

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <filesystem>
#include <chrono>

namespace akao {
namespace builders {
namespace akao {

/**
 * @brief Compilation stage enumeration
 */
enum class CompilationStage {
    PARSING,
    SEMANTIC_ANALYSIS,
    OPTIMIZATION,
    CODE_GENERATION,
    COMPILATION,
    LINKING
};

/**
 * @brief Target platform specification
 */
struct TargetPlatform {
    std::string name;
    std::string compiler;
    std::vector<std::string> compiler_flags;
    std::vector<std::string> linker_flags;
    std::vector<std::string> system_libraries;
};

/**
 * @brief Optimization configuration
 */
struct OptimizationConfig {
    std::string level; // debug, release, performance
    std::vector<std::string> flags;
    std::vector<std::string> features;
    bool enable_dead_code_elimination = true;
    bool enable_constant_folding = true;
    bool enable_inlining = true;
};

/**
 * @brief Build configuration
 */
struct BuildConfiguration {
    TargetPlatform target;
    OptimizationConfig optimization;
    std::string output_directory;
    std::string intermediate_directory;
    std::string executable_name;
    bool verbose = false;
    bool clean_build = false;
    int parallel_jobs = 0; // 0 = auto-detect
};

/**
 * @brief Source file information
 */
struct SourceFile {
    std::string filepath;
    std::string relative_path;
    std::chrono::system_clock::time_point last_modified;
    std::vector<std::string> dependencies;
    bool needs_compilation = true;
};

/**
 * @brief Compilation unit result
 */
struct CompilationUnit {
    std::string source_file;
    std::string object_file;
    std::string generated_cpp;
    bool success = false;
    std::string error_message;
    double compilation_time = 0.0;
    std::vector<std::string> warnings;
};

/**
 * @brief Build stage result
 */
struct BuildStageResult {
    CompilationStage stage;
    bool success = false;
    std::string error_message;
    double stage_time = 0.0;
    std::vector<std::string> output_files;
    std::map<std::string, std::string> metadata;
};

/**
 * @brief Complete build result
 */
struct BuildEngineResult {
    bool success = false;
    std::string executable_path;
    std::vector<CompilationUnit> compilation_units;
    std::vector<BuildStageResult> stage_results;
    std::string error_message;
    double total_build_time = 0.0;
    size_t lines_of_code = 0;
    size_t executable_size = 0;
};

/**
 * @brief Dependency graph node
 */
struct DependencyNode {
    std::string filepath;
    std::vector<std::string> dependencies;
    std::vector<std::string> dependents;
    bool processed = false;
};

/**
 * @brief Build cache entry
 */
struct BuildCacheEntry {
    std::string source_file;
    std::string source_hash;
    std::chrono::system_clock::time_point build_time;
    std::string object_file;
    std::vector<std::string> dependencies;
};

/**
 * @brief AST to C++ code generator
 */
class CodeGenerator {
public:
    CodeGenerator();
    
    /**
     * @brief Generate C++ code from AST
     * @param ast_node Root AST node
     * @param output_file Output C++ file path
     * @return True if successful
     */
    bool generate_code(const void* ast_node, const std::string& output_file);
    
    /**
     * @brief Set generation options
     * @param options Generation configuration
     */
    void set_options(const std::map<std::string, std::string>& options);
    
    /**
     * @brief Get last error message
     * @return Error message
     */
    std::string get_last_error() const;
    
private:
    std::map<std::string, std::string> options_;
    std::string last_error_;
    
    std::string generate_includes();
    std::string generate_function_definitions(const void* ast_node);
    std::string generate_main_function(const void* ast_node);
};

/**
 * @brief Build cache manager
 */
class BuildCache {
public:
    BuildCache(const std::string& cache_directory);
    
    /**
     * @brief Check if file needs compilation
     * @param source_file Source file path
     * @return True if compilation needed
     */
    bool needs_compilation(const std::string& source_file);
    
    /**
     * @brief Update cache entry
     * @param entry Cache entry to update
     */
    void update_cache(const BuildCacheEntry& entry);
    
    /**
     * @brief Clear build cache
     */
    void clear_cache();
    
    /**
     * @brief Get cache statistics
     * @return Cache hit rate and size
     */
    std::map<std::string, double> get_statistics() const;
    
private:
    std::string cache_directory_;
    std::map<std::string, BuildCacheEntry> cache_entries_;
    
    void load_cache();
    void save_cache();
    std::string calculate_file_hash(const std::string& filepath);
};

/**
 * @brief Dependency resolver
 */
class DependencyResolver {
public:
    DependencyResolver();
    
    /**
     * @brief Build dependency graph
     * @param source_files List of source files
     * @return Dependency graph
     */
    std::vector<DependencyNode> build_dependency_graph(const std::vector<std::string>& source_files);
    
    /**
     * @brief Get build order
     * @param dependency_graph Dependency graph
     * @return Ordered list of files for compilation
     */
    std::vector<std::string> get_build_order(const std::vector<DependencyNode>& dependency_graph);
    
    /**
     * @brief Check for circular dependencies
     * @param dependency_graph Dependency graph
     * @return True if circular dependencies found
     */
    bool has_circular_dependencies(const std::vector<DependencyNode>& dependency_graph);
    
private:
    void resolve_file_dependencies(const std::string& filepath, std::vector<std::string>& dependencies);
    bool has_cycle_recursive(const std::vector<DependencyNode>& graph, 
                             const std::string& node, 
                             std::map<std::string, int>& states);
};

/**
 * @brief Main build engine
 */
class BuildEngine {
public:
    BuildEngine();
    ~BuildEngine();
    
    /**
     * @brief Set build configuration
     * @param config Build configuration
     */
    void set_configuration(const BuildConfiguration& config);
    
    /**
     * @brief Add source file to build
     * @param filepath Source file path
     */
    void add_source_file(const std::string& filepath);
    
    /**
     * @brief Add source files from directory
     * @param directory Directory path
     * @param recursive Recursive search
     */
    void add_source_directory(const std::string& directory, bool recursive = true);
    
    /**
     * @brief Build project
     * @return Build result
     */
    BuildEngineResult build();
    
    /**
     * @brief Clean build artifacts
     * @return True if successful
     */
    bool clean();
    
    /**
     * @brief Get supported target platforms
     * @return Map of platform names to configurations
     */
    std::map<std::string, TargetPlatform> get_supported_platforms() const;
    
    /**
     * @brief Get build statistics
     * @return Build performance statistics
     */
    std::map<std::string, double> get_build_statistics() const;
    
private:
    BuildConfiguration config_;
    std::vector<SourceFile> source_files_;
    std::unique_ptr<CodeGenerator> code_generator_;
    std::unique_ptr<BuildCache> build_cache_;
    std::unique_ptr<DependencyResolver> dependency_resolver_;
    
    void initialize_target_platforms();
    void initialize_optimization_configs();
    
    BuildStageResult run_parsing_stage();
    BuildStageResult run_semantic_analysis_stage();
    BuildStageResult run_optimization_stage();
    BuildStageResult run_code_generation_stage();
    BuildStageResult run_compilation_stage();
    BuildStageResult run_linking_stage();
    
    CompilationUnit compile_source_file(const SourceFile& source);
    bool link_object_files(const std::vector<std::string>& object_files, 
                           const std::string& output_file);
    
    void discover_source_files(const std::string& directory, bool recursive);
    void update_source_file_dependencies();
    
    std::string generate_build_info() const;
    void write_build_metadata(const BuildEngineResult& result);
    
    bool validate_build_configuration() const;
    void setup_build_environment();
    void cleanup_build_environment();
    
    std::vector<std::string> get_compiler_command(const std::string& source_file, 
                                                   const std::string& output_file) const;
    std::vector<std::string> get_linker_command(const std::vector<std::string>& object_files, 
                                                 const std::string& output_file) const;
};

/**
 * @brief Build engine factory
 */
class BuildEngineFactory {
public:
    /**
     * @brief Create build engine for target platform
     * @param platform_name Platform name
     * @return Build engine instance
     */
    static std::unique_ptr<BuildEngine> create_for_platform(const std::string& platform_name);
    
    /**
     * @brief Get available platforms
     * @return List of supported platform names
     */
    static std::vector<std::string> get_available_platforms();
    
    /**
     * @brief Auto-detect current platform
     * @return Current platform name
     */
    static std::string detect_current_platform();
};

} // namespace akao
} // namespace builders
} // namespace akao
