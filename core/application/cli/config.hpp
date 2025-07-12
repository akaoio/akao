/**
 * @id: akao:class:core:application:cli:config:v1
 * @doc: Configuration management for CLI application supporting both file-based configuration and environment variables. Provides centralized configuration loading with precedence order: command line flags > environment variables > config file > defaults.
 * @specification: CLI configuration management with YAML config file support
 * @scope: Core application CLI configuration
 * @timeline: 2025-07-12
 * @rationale: Enable flexible CLI configuration for different deployment scenarios
 * @methodology: YAML configuration file parsing with environment variable overrides
 * @references: ["akao:class:core:foundation:formats:yaml:v1"]
 */

#pragma once

#include "core/foundation/formats/yaml/v1.hpp"
#include <string>
#include <map>
#include <filesystem>

namespace akao::core::application::cli {

struct CLIConfig {
    // Core connection settings
    std::string core_socket_path = "/tmp/akao-core.sock";
    int connection_timeout_seconds = 30;
    int connection_retry_count = 3;
    
    // Default output settings
    std::string default_output_format = "human"; // human, json, yaml
    bool default_verbose = false;
    bool default_quiet = false;
    bool default_debug = false;
    
    // Node and workflow settings
    std::string nodes_directory = ".akao/nodes";
    std::string workflows_directory = ".akao/workflows";
    std::string cache_directory = ".akao/cache";
    
    // Advanced settings
    bool enable_shell_completion = true;
    bool enable_color_output = true;
    int max_output_lines = 1000;
    
    // Security settings
    bool allow_external_nodes = true;
    bool verify_node_signatures = false;
    std::string trusted_sources_file = ".akao/trusted-sources.yaml";
};

class ConfigManager {
private:
    CLIConfig config_;
    std::string config_file_path_;
    bool config_loaded_;

public:
    ConfigManager();
    ~ConfigManager() = default;

    // Configuration loading
    bool loadConfigFile(const std::string& file_path = "");
    bool loadFromEnvironment();
    void applyDefaults();
    
    // Configuration access
    const CLIConfig& getConfig() const { return config_; }
    CLIConfig& getMutableConfig() { return config_; }
    
    // Specific getters with fallbacks
    std::string getCoreSocketPath() const;
    int getConnectionTimeout() const;
    std::string getDefaultOutputFormat() const;
    std::string getNodesDirectory() const;
    std::string getWorkflowsDirectory() const;
    
    // Environment variable overrides
    void applyEnvironmentOverrides();
    
    // Configuration validation
    bool validateConfig();
    std::vector<std::string> getConfigErrors() const;
    
    // Configuration file management
    bool createDefaultConfigFile(const std::string& file_path = "");
    std::string getDefaultConfigFilePath() const;
    bool configFileExists() const;

private:
    // Helper methods
    std::string expandPath(const std::string& path) const;
    std::string getEnvironmentVar(const std::string& var_name, const std::string& default_value = "") const;
    bool getBoolEnvironmentVar(const std::string& var_name, bool default_value = false) const;
    int getIntEnvironmentVar(const std::string& var_name, int default_value = 0) const;
    
    // YAML processing
    void parseConfigYaml(const std::shared_ptr<akao::foundation::formats::yaml::YamlNode>& root);
    std::string generateDefaultConfigYaml() const;
};

} // namespace akao::core::application::cli