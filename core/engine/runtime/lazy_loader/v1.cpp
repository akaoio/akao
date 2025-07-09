/**
 * @id: akao:file:core:engine:runtime_lazy_loader_impl:v1
 * @doc: Lazy loading system implementation for dynamic component management providing comprehensive lazy loading capabilities for philosophies, rules, and rulesets to avoid runtime restarts during component modifications. Features hot-reload support, efficient memory usage through on-demand loading, cache management with expiration, file change detection, and loading statistics. Implements template-based component loading with type safety, cache hit/miss tracking, and automatic hot-reload scanning for development workflow optimization in the Akao framework.
 * @specification: Implementation for lazy loading system with dynamic component management and hot-reload capabilities
 * @scope: Core engine runtime lazy loading framework implementation with cache management and file change detection
 * @timeline: 2025-07-09
 * @rationale: Provide lazy loading capabilities implementation to avoid runtime restarts and enable efficient component management
 * @methodology: Template-based lazy loading implementation with cache management, hot-reload support, and file change detection
 * @references: ["akao:file:core:engine:runtime_lazy_loader:v1", "akao:file:core:engine:runtime_control:v1"]
 */

#include "v1.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>

namespace akao::core::engine::runtime {

LazyLoader::LazyLoader() = default;

LazyLoader::~LazyLoader() = default;

// Unload Component
bool LazyLoader::unloadComponent(const std::string& component_id) {
    std::lock_guard<std::mutex> lock(loader_mutex_);
    
    component_cache_.erase(component_id);
    load_timestamps_.erase(component_id);
    file_timestamps_.erase(component_id);
    
    std::cout << "Unloaded component: " << component_id << std::endl;
    return true;
}

// Reload Component
bool LazyLoader::reloadComponent(const std::string& component_id) {
    std::lock_guard<std::mutex> lock(loader_mutex_);
    
    // Remove from cache to force reload
    component_cache_.erase(component_id);
    load_timestamps_.erase(component_id);
    
    // Update file timestamp
    std::string file_path = getComponentPath(component_id);
    file_timestamps_[component_id] = getFileTimestamp(file_path);
    
    stats_.hot_reloads++;
    
    std::cout << "Reloaded component: " << component_id << std::endl;
    return true;
}

// Cache Management
void LazyLoader::clearCache() {
    std::lock_guard<std::mutex> lock(loader_mutex_);
    
    component_cache_.clear();
    load_timestamps_.clear();
    file_timestamps_.clear();
    
    std::cout << "Cleared component cache" << std::endl;
}

void LazyLoader::clearExpiredCache() {
    std::lock_guard<std::mutex> lock(loader_mutex_);
    
    auto now = std::chrono::steady_clock::now();
    auto expire_duration = std::chrono::minutes(30); // 30 minute cache expiry
    
    for (auto it = load_timestamps_.begin(); it != load_timestamps_.end();) {
        if (now - it->second > expire_duration) {
            component_cache_.erase(it->first);
            file_timestamps_.erase(it->first);
            it = load_timestamps_.erase(it);
        } else {
            ++it;
        }
    }
}

size_t LazyLoader::getCacheSize() const {
    std::lock_guard<std::mutex> lock(loader_mutex_);
    return component_cache_.size();
}

// Hot-reload Support
bool LazyLoader::enableHotReload(const std::string& component_id) {
    std::lock_guard<std::mutex> lock(loader_mutex_);
    
    hot_reload_enabled_.insert(component_id);
    
    // Initialize file timestamp
    std::string file_path = getComponentPath(component_id);
    file_timestamps_[component_id] = getFileTimestamp(file_path);
    
    std::cout << "Enabled hot-reload for: " << component_id << std::endl;
    return true;
}

bool LazyLoader::disableHotReload(const std::string& component_id) {
    std::lock_guard<std::mutex> lock(loader_mutex_);
    
    hot_reload_enabled_.erase(component_id);
    file_timestamps_.erase(component_id);
    
    std::cout << "Disabled hot-reload for: " << component_id << std::endl;
    return true;
}

void LazyLoader::scanForChanges() {
    std::lock_guard<std::mutex> lock(loader_mutex_);
    
    for (const auto& component_id : hot_reload_enabled_) {
        if (hasFileChanged(component_id)) {
            std::cout << "File changed, reloading: " << component_id << std::endl;
            
            // Remove from cache and update timestamp
            component_cache_.erase(component_id);
            load_timestamps_.erase(component_id);
            
            std::string file_path = getComponentPath(component_id);
            file_timestamps_[component_id] = getFileTimestamp(file_path);
            
            stats_.hot_reloads++;
        }
    }
}

// Statistics
LazyLoader::LoadingStats LazyLoader::getStats() const {
    std::lock_guard<std::mutex> lock(loader_mutex_);
    return stats_;
}

void LazyLoader::resetStats() {
    std::lock_guard<std::mutex> lock(loader_mutex_);
    stats_ = LoadingStats{};
}

// Private Methods
bool LazyLoader::hasFileChanged(const std::string& component_id) const {
    std::string file_path = getComponentPath(component_id);
    auto current_timestamp = getFileTimestamp(file_path);
    
    auto it = file_timestamps_.find(component_id);
    if (it == file_timestamps_.end()) {
        return true; // First time checking
    }
    
    return current_timestamp > it->second;
}

std::string LazyLoader::getComponentPath(const std::string& component_id) const {
    // Parse component ID to determine file path
    // Format: akao:type:category:name:version
    
    if (component_id.find("philosophy") != std::string::npos) {
        // Extract path from philosophy ID
        // Example: akao:philosophy::structure:enforcement:v1 -> philosophies/structure/enforcement/v1.yaml
        size_t start = component_id.find("::") + 2;
        size_t end = component_id.rfind(":v");
        std::string path_part = component_id.substr(start, end - start);
        std::replace(path_part.begin(), path_part.end(), ':', '/');
        return "philosophies/" + path_part + "/v1.yaml";
    } else if (component_id.find("rule::") != std::string::npos) {
        // Extract path from rule ID
        size_t start = component_id.find("::") + 2;
        size_t end = component_id.rfind(":v");
        std::string path_part = component_id.substr(start, end - start);
        std::replace(path_part.begin(), path_part.end(), ':', '/');
        return "rules/" + path_part + "/v1.yaml";
    } else if (component_id.find("ruleset:") != std::string::npos) {
        // Extract path from ruleset ID
        size_t start = component_id.find("ruleset:") + 8;
        size_t end = component_id.rfind(":v");
        std::string path_part = component_id.substr(start, end - start);
        std::replace(path_part.begin(), path_part.end(), ':', '/');
        return "rulesets/" + path_part + "/v1.yaml";
    }
    
    return ""; // Unknown component type
}

std::chrono::steady_clock::time_point LazyLoader::getFileTimestamp(const std::string& file_path) const {
    try {
        if (std::filesystem::exists(file_path)) {
            auto ftime = std::filesystem::last_write_time(file_path);
            return std::chrono::steady_clock::time_point{
                std::chrono::duration_cast<std::chrono::steady_clock::duration>(
                    ftime.time_since_epoch())
            };
        }
    } catch (const std::exception& e) {
        // File doesn't exist or can't be accessed
    }
    
    return std::chrono::steady_clock::time_point{};
}

std::shared_ptr<void> LazyLoader::createPhilosophy(const std::string& philosophy_id) {
    // In a real implementation, this would load and parse the philosophy YAML file
    // For now, return a placeholder
    std::cout << "Loading philosophy: " << philosophy_id << std::endl;
    return std::make_shared<std::string>("Philosophy: " + philosophy_id);
}

std::shared_ptr<void> LazyLoader::createRule(const std::string& rule_id) {
    // In a real implementation, this would load and parse the rule YAML file
    std::cout << "Loading rule: " << rule_id << std::endl;
    return std::make_shared<std::string>("Rule: " + rule_id);
}

std::shared_ptr<void> LazyLoader::createRuleset(const std::string& ruleset_id) {
    // In a real implementation, this would load and parse the ruleset YAML file
    std::cout << "Loading ruleset: " << ruleset_id << std::endl;
    return std::make_shared<std::string>("Ruleset: " + ruleset_id);
}

} // namespace akao::core::engine::runtime
