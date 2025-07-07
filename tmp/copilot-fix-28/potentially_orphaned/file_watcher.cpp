#include "file_watcher.hpp"

namespace akao::core::filesystem::watcher {

FileWatcher::FileWatcher() : is_running_(false) {}

bool FileWatcher::watchDirectory(const std::string& directory_path) {
    // TODO: Implement directory watching
    watched_paths_.push_back(directory_path);
    return true;
}

bool FileWatcher::watchFile(const std::string& file_path) {
    // TODO: Implement file watching
    watched_paths_.push_back(file_path);
    return true;
}

void FileWatcher::setCallback(EventCallback callback) {
    callback_ = callback;
}

void FileWatcher::start() {
    // TODO: Implement file watching start
    is_running_ = true;
}

void FileWatcher::stop() {
    // TODO: Implement file watching stop
    is_running_ = false;
}

} // namespace akao::core::filesystem::watcher
