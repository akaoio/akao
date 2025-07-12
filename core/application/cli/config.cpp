/**
 * @id: akao:class:core:application:cli:config:v1
 * @doc: Configuration management implementation for CLI application providing flexible configuration loading with precedence order and environment variable support.
 */

#include "config.hpp"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <pwd.h>
#include <unistd.h>
#include <algorithm>
#include <cctype>

namespace akao::core::application::cli {

ConfigManager::ConfigManager() : config_loaded_(false) {
    applyDefaults();
    config_file_path_ = getDefaultConfigFilePath();
}

bool ConfigManager::loadConfigFile(const std::string& file_path) {
    std::string path = file_path.empty() ? config_file_path_ : file_path;
    
    if (!std::filesystem::exists(path)) {
        return false; // Not an error - config file is optional
    }
    
    try {
        akao::foundation::formats::yaml::YamlParser parser;
        auto root = parser.parseFile(path);
        
        if (!root) {
            return false;
        }
        
        parseConfigYaml(root);
        config_loaded_ = true;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading config file " << path << ": " << e.what() << std::endl;
        return false;
    }
}

bool ConfigManager::loadFromEnvironment() {
    applyEnvironmentOverrides();
    return true;
}

void ConfigManager::applyDefaults() {
    // Defaults are already set in the struct initialization
    // This method can be extended for more complex default logic
}

void ConfigManager::applyEnvironmentOverrides() {
    // Core connection settings
    config_.core_socket_path = getEnvironmentVar("AKAO_CORE_SOCKET", config_.core_socket_path);
    config_.connection_timeout_seconds = getIntEnvironmentVar("AKAO_CONNECTION_TIMEOUT", config_.connection_timeout_seconds);
    config_.connection_retry_count = getIntEnvironmentVar("AKAO_CONNECTION_RETRIES", config_.connection_retry_count);
    
    // Output settings
    config_.default_output_format = getEnvironmentVar("AKAO_OUTPUT_FORMAT", config_.default_output_format);
    config_.default_verbose = getBoolEnvironmentVar("AKAO_VERBOSE", config_.default_verbose);
    config_.default_quiet = getBoolEnvironmentVar("AKAO_QUIET", config_.default_quiet);
    config_.default_debug = getBoolEnvironmentVar("AKAO_DEBUG", config_.default_debug);
    
    // Directory settings
    config_.nodes_directory = getEnvironmentVar("AKAO_NODES_DIR", config_.nodes_directory);
    config_.workflows_directory = getEnvironmentVar("AKAO_WORKFLOWS_DIR", config_.workflows_directory);
    config_.cache_directory = getEnvironmentVar("AKAO_CACHE_DIR", config_.cache_directory);
    
    // Advanced settings
    config_.enable_shell_completion = getBoolEnvironmentVar("AKAO_SHELL_COMPLETION", config_.enable_shell_completion);
    config_.enable_color_output = getBoolEnvironmentVar("AKAO_COLOR_OUTPUT", config_.enable_color_output);
    config_.max_output_lines = getIntEnvironmentVar("AKAO_MAX_OUTPUT_LINES", config_.max_output_lines);
    
    // Security settings
    config_.allow_external_nodes = getBoolEnvironmentVar("AKAO_ALLOW_EXTERNAL_NODES", config_.allow_external_nodes);
    config_.verify_node_signatures = getBoolEnvironmentVar("AKAO_VERIFY_SIGNATURES", config_.verify_node_signatures);
    config_.trusted_sources_file = getEnvironmentVar("AKAO_TRUSTED_SOURCES", config_.trusted_sources_file);
}

std::string ConfigManager::getCoreSocketPath() const {
    return expandPath(config_.core_socket_path);
}

int ConfigManager::getConnectionTimeout() const {
    return config_.connection_timeout_seconds;
}

std::string ConfigManager::getDefaultOutputFormat() const {
    return config_.default_output_format;
}

std::string ConfigManager::getNodesDirectory() const {
    return expandPath(config_.nodes_directory);
}

std::string ConfigManager::getWorkflowsDirectory() const {
    return expandPath(config_.workflows_directory);
}

bool ConfigManager::validateConfig() {
    // Basic validation
    if (config_.connection_timeout_seconds <= 0) {
        return false;
    }
    
    if (config_.connection_retry_count < 0) {
        return false;
    }
    
    if (config_.default_output_format != "human" && 
        config_.default_output_format != "json" && 
        config_.default_output_format != "yaml") {
        return false;
    }
    
    if (config_.max_output_lines <= 0) {
        return false;
    }
    
    return true;
}

std::vector<std::string> ConfigManager::getConfigErrors() const {
    std::vector<std::string> errors;
    
    if (config_.connection_timeout_seconds <= 0) {
        errors.push_back("connection_timeout_seconds must be positive");
    }
    
    if (config_.connection_retry_count < 0) {
        errors.push_back("connection_retry_count must be non-negative");
    }
    
    if (config_.default_output_format != "human" && 
        config_.default_output_format != "json" && 
        config_.default_output_format != "yaml") {
        errors.push_back("default_output_format must be 'human', 'json', or 'yaml'");
    }
    
    if (config_.max_output_lines <= 0) {
        errors.push_back("max_output_lines must be positive");
    }
    
    return errors;
}

bool ConfigManager::createDefaultConfigFile(const std::string& file_path) {
    std::string path = file_path.empty() ? config_file_path_ : file_path;
    
    // Create directory if it doesn't exist
    std::filesystem::path config_path(path);
    std::filesystem::create_directories(config_path.parent_path());
    
    try {
        std::ofstream file(path);
        if (!file.is_open()) {
            return false;
        }
        
        file << generateDefaultConfigYaml();
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error creating config file " << path << ": " << e.what() << std::endl;
        return false;
    }
}

std::string ConfigManager::getDefaultConfigFilePath() const {
    const char* home = getenv("HOME");
    if (!home) {
        struct passwd* pw = getpwuid(getuid());
        home = pw ? pw->pw_dir : "/tmp";
    }
    
    return std::string(home) + "/.akao/config.yaml";
}

bool ConfigManager::configFileExists() const {
    return std::filesystem::exists(config_file_path_);
}

std::string ConfigManager::expandPath(const std::string& path) const {
    if (path.empty() || path[0] != '~') {
        return path;
    }
    
    const char* home = getenv("HOME");
    if (!home) {
        struct passwd* pw = getpwuid(getuid());
        home = pw ? pw->pw_dir : "/tmp";
    }
    
    return std::string(home) + path.substr(1);
}

std::string ConfigManager::getEnvironmentVar(const std::string& var_name, const std::string& default_value) const {
    const char* value = getenv(var_name.c_str());
    return value ? std::string(value) : default_value;
}

bool ConfigManager::getBoolEnvironmentVar(const std::string& var_name, bool default_value) const {
    const char* value = getenv(var_name.c_str());
    if (!value) {
        return default_value;
    }
    
    std::string str_value(value);
    std::transform(str_value.begin(), str_value.end(), str_value.begin(), ::tolower);
    
    return str_value == "true" || str_value == "1" || str_value == "yes" || str_value == "on";
}

int ConfigManager::getIntEnvironmentVar(const std::string& var_name, int default_value) const {
    const char* value = getenv(var_name.c_str());
    if (!value) {
        return default_value;
    }
    
    try {
        return std::stoi(value);
    } catch (const std::exception&) {
        return default_value;
    }
}

void ConfigManager::parseConfigYaml(const std::shared_ptr<akao::foundation::formats::yaml::YamlNode>& root) {
    if (!root || !root->isMapping()) {
        return;
    }
    
    // Core connection settings
    if (root->hasKey("core")) {
        auto core = root->operator[]("core");
        if (core->hasKey("socket_path")) {
            config_.core_socket_path = core->operator[]("socket_path")->asString();
        }
        if (core->hasKey("connection_timeout")) {
            config_.connection_timeout_seconds = core->operator[]("connection_timeout")->asInteger();
        }
        if (core->hasKey("retry_count")) {
            config_.connection_retry_count = core->operator[]("retry_count")->asInteger();
        }
    }
    
    // Output settings
    if (root->hasKey("output")) {
        auto output = root->operator[]("output");
        if (output->hasKey("format")) {
            config_.default_output_format = output->operator[]("format")->asString();
        }
        if (output->hasKey("verbose")) {
            config_.default_verbose = output->operator[]("verbose")->asBoolean();
        }
        if (output->hasKey("quiet")) {
            config_.default_quiet = output->operator[]("quiet")->asBoolean();
        }
        if (output->hasKey("debug")) {
            config_.default_debug = output->operator[]("debug")->asBoolean();
        }
        if (output->hasKey("max_lines")) {
            config_.max_output_lines = output->operator[]("max_lines")->asInteger();
        }
        if (output->hasKey("enable_color")) {
            config_.enable_color_output = output->operator[]("enable_color")->asBoolean();
        }
    }
    
    // Directory settings
    if (root->hasKey("directories")) {
        auto dirs = root->operator[]("directories");
        if (dirs->hasKey("nodes")) {
            config_.nodes_directory = dirs->operator[]("nodes")->asString();
        }
        if (dirs->hasKey("workflows")) {
            config_.workflows_directory = dirs->operator[]("workflows")->asString();
        }
        if (dirs->hasKey("cache")) {
            config_.cache_directory = dirs->operator[]("cache")->asString();
        }
    }
    
    // Security settings
    if (root->hasKey("security")) {
        auto security = root->operator[]("security");
        if (security->hasKey("allow_external_nodes")) {
            config_.allow_external_nodes = security->operator[]("allow_external_nodes")->asBoolean();
        }
        if (security->hasKey("verify_signatures")) {
            config_.verify_node_signatures = security->operator[]("verify_signatures")->asBoolean();
        }
        if (security->hasKey("trusted_sources_file")) {
            config_.trusted_sources_file = security->operator[]("trusted_sources_file")->asString();
        }
    }
}

std::string ConfigManager::generateDefaultConfigYaml() const {
    return R"(# Akao CLI Configuration File
# This file provides default settings for the akao command-line interface

# Core orchestrator connection settings
core:
  socket_path: "/tmp/akao-core.sock"
  connection_timeout: 30
  retry_count: 3

# Output and display settings
output:
  format: "human"  # human, json, yaml
  verbose: false
  quiet: false
  debug: false
  max_lines: 1000
  enable_color: true

# Directory locations
directories:
  nodes: ".akao/nodes"
  workflows: ".akao/workflows"
  cache: ".akao/cache"

# Security settings
security:
  allow_external_nodes: true
  verify_signatures: false
  trusted_sources_file: ".akao/trusted-sources.yaml"

# Advanced settings
advanced:
  enable_shell_completion: true
)";
}

} // namespace akao::core::application::cli