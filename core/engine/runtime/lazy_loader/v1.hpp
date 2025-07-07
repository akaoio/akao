#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>
#include <mutex>
#include <chrono>

namespace akao::core::engine::runtime {

/**
 * @brief Lazy Loading System for Dynamic Component Management
 * 
 * Provides lazy loading capabilities for philosophies, rules, and rulesets
 * to avoid runtime restarts during component modifications. Supports hot-reload
 * and efficient memory usage through on-demand loading.
 */
class LazyLoader {
public:
    LazyLoader();
    ~LazyLoader();

    // Component Loading
    template<typename T>
    std::shared_ptr<T> loadComponent(const std::string& component_id);
    
    template<typename T>
    std::shared_ptr<T> getOrLoadComponent(const std::string& component_id);
    
    bool unloadComponent(const std::string& component_id);
    bool reloadComponent(const std::string& component_id);
    
    // Cache Management
    void clearCache();
    void clearExpiredCache();
    size_t getCacheSize() const;
    
    // Hot-reload Support
    bool enableHotReload(const std::string& component_id);
    bool disableHotReload(const std::string& component_id);
    void scanForChanges();
    
    // Statistics
    struct LoadingStats {
        size_t cache_hits = 0;
        size_t cache_misses = 0;
        size_t hot_reloads = 0;
        std::chrono::milliseconds total_load_time{0};
    };
    
    LoadingStats getStats() const;
    void resetStats();

private:
    mutable std::mutex loader_mutex_;
    
    // Component cache
    std::unordered_map<std::string, std::shared_ptr<void>> component_cache_;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> load_timestamps_;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> file_timestamps_;
    
    // Hot-reload tracking
    std::unordered_set<std::string> hot_reload_enabled_;
    
    // Statistics
    mutable LoadingStats stats_;
    
    // Internal helpers
    bool hasFileChanged(const std::string& component_id) const;
    std::string getComponentPath(const std::string& component_id) const;
    std::chrono::steady_clock::time_point getFileTimestamp(const std::string& file_path) const;
    
    // Component factories
    std::shared_ptr<void> createPhilosophy(const std::string& philosophy_id);
    std::shared_ptr<void> createRule(const std::string& rule_id);
    std::shared_ptr<void> createRuleset(const std::string& ruleset_id);
};

// Template specializations would be defined in the implementation file
// or as explicit instantiations for specific component types

} // namespace akao::core::engine::runtime