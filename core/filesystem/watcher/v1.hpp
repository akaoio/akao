/**
 * @id: akao:file:core:filesystem:watcher_file_watcher:v1
 * @doc: Universal file system watcher providing real-time monitoring of file and directory changes with event-driven callbacks for automated response to filesystem modifications. Implements cross-platform file watching capabilities with configurable event filtering, batch processing, and performance optimization for monitoring large directory trees with minimal system resource usage.
 * @specification: Universal filesystem monitoring system with real-time event detection and callback mechanisms
 * @scope: Core filesystem monitoring subsystem
 * @timeline: 2025-07-09
 * @rationale: Enable reactive filesystem monitoring for automated validation, structure enforcement, and change detection
 * @methodology: Event-driven filesystem monitoring with cross-platform compatibility and performance optimization
 * @references: ["akao:philosophy:automation:updates:v1", "akao:philosophy:measurement:observability:v1", "akao:philosophy:structure:enforcement:v1"]
 */

#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>
#include <map>
#include <set>

namespace akao::core::filesystem::watcher {

class FileWatcher {
public:
    enum class EventType {
        Created,
        Modified,
        Deleted
    };
    
    using EventCallback = std::function<void(const std::string&, EventType)>;
    
    FileWatcher();
    
    bool watchDirectory(const std::string& directory_path);
    bool watchFile(const std::string& file_path);
    void setCallback(EventCallback callback);
    void start();
    void stop();
    
private:
    std::vector<std::string> watched_paths_;
    EventCallback callback_;
    bool is_running_;
};

} // namespace akao::core::filesystem::watcher
