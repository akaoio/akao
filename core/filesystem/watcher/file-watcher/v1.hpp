#ifndef AKAO_CORE_FILESYSTEM_WATCHER_FILE_WATCHER_HPP
#define AKAO_CORE_FILESYSTEM_WATCHER_FILE_WATCHER_HPP

#include <string>
#include <vector>
#include <functional>

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

#endif // AKAO_CORE_FILESYSTEM_WATCHER_FILE_WATCHER_HPP
