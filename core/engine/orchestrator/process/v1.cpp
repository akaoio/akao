/**
 * @id: akao:class:core:engine:orchestrator:process:v1
 * @doc: Process management system implementation for external node lifecycle management providing robust process spawning, monitoring, resource management, and health tracking. Manages the complete lifecycle of external node processes including startup, shutdown, restart, resource limits, environment setup, and crash recovery. Integrates with the discovery system to provide seamless external node process management for the YAML-RPC communication architecture.
 * @specification: External node process lifecycle management implementation with monitoring and resource control
 * @scope: Core engine orchestrator process management system implementation
 * @timeline: 2025-07-11
 * @rationale: Enable robust external node process management for core-node independence
 * @methodology: Process spawning with resource monitoring, lifecycle management, and health tracking
 * @references: ["akao:directory:core:engine:orchestrator:process:v1", "akao:class:core:engine:orchestrator:discovery:v1"]
 */

#include "v1.hpp"
#include <sys/wait.h>
#include <sys/resource.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <errno.h>

namespace akao {
namespace core {
namespace engine {
namespace orchestrator {
namespace process {

// =============================================================================
// ProcessStats Implementation
// =============================================================================

std::chrono::duration<double> ProcessStats::uptime() const {
    if (!is_running()) {
        return std::chrono::duration<double>(0);
    }
    return std::chrono::system_clock::now() - start_time;
}

double ProcessStats::cpu_usage_percent() const {
    auto up = uptime();
    if (up.count() == 0) {
        return 0.0;
    }
    return (cpu_time.count() / up.count()) * 100.0;
}

double ProcessStats::memory_usage_mb() const {
    return static_cast<double>(memory_usage_kb) / 1024.0;
}

bool ProcessStats::is_running() const {
    return pid > 0 && kill(pid, 0) == 0;
}

// =============================================================================
// ExternalNodeProcess Implementation
// =============================================================================

ExternalNodeProcess::ExternalNodeProcess(const std::string& node_id)
    : node_id_(node_id), state_(ProcessState::STOPPED), should_restart_(false),
      monitoring_(false), monitor_interval_(std::chrono::seconds(5)) {
    stats_.pid = -1;
}

ExternalNodeProcess::~ExternalNodeProcess() {
    stopMonitoring();
    if (isRunning()) {
        stop();
    }
}

void ExternalNodeProcess::setExecutable(const std::string& path) {
    executable_path_ = path;
}

void ExternalNodeProcess::setArguments(const std::vector<std::string>& args) {
    args_ = args;
}

void ExternalNodeProcess::setEnvironment(const std::map<std::string, std::string>& env) {
    environment_ = env;
}

void ExternalNodeProcess::setWorkingDirectory(const std::string& dir) {
    working_directory_ = dir;
}

void ExternalNodeProcess::setLimits(const ProcessLimits& limits) {
    limits_ = limits;
}

bool ExternalNodeProcess::start() {
    std::lock_guard<std::mutex> lock(state_mutex_);
    
    if (state_ == ProcessState::RUNNING || state_ == ProcessState::STARTING) {
        return true;
    }
    
    setState(ProcessState::STARTING);
    
    // Prepare launch configuration
    ProcessLauncher::LaunchConfig config;
    config.executable = executable_path_;
    config.args = args_;
    config.environment = environment_;
    config.working_directory = working_directory_;
    config.limits = limits_;
    
    // Launch process
    auto result = ProcessLauncher::launch(config);
    if (!result.success) {
        setState(ProcessState::FAILED);
        return false;
    }
    
    // Update stats
    stats_.pid = result.pid;
    stats_.start_time = std::chrono::system_clock::now();
    stats_.restart_count = 0;
    
    setState(ProcessState::RUNNING);
    return true;
}

bool ExternalNodeProcess::stop(std::chrono::seconds timeout) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    
    if (state_ == ProcessState::STOPPED || stats_.pid <= 0) {
        return true;
    }
    
    setState(ProcessState::STOPPING);
    should_restart_ = false;
    
    bool success = ProcessLauncher::terminate(stats_.pid, timeout);
    if (success) {
        setState(ProcessState::STOPPED);
        stats_.pid = -1;
    } else {
        // Force kill if terminate failed
        ProcessLauncher::kill(stats_.pid);
        setState(ProcessState::STOPPED);
        stats_.pid = -1;
    }
    
    return true;
}

bool ExternalNodeProcess::restart() {
    if (!stop()) {
        return false;
    }
    
    // Brief delay before restart
    std::this_thread::sleep_for(limits_.restart_delay);
    
    return start();
}

bool ExternalNodeProcess::kill() {
    std::lock_guard<std::mutex> lock(state_mutex_);
    
    if (stats_.pid <= 0) {
        return true;
    }
    
    bool success = ProcessLauncher::kill(stats_.pid);
    setState(ProcessState::STOPPED);
    stats_.pid = -1;
    
    return success;
}

void ExternalNodeProcess::startMonitoring(std::chrono::seconds interval) {
    if (monitoring_) {
        return;
    }
    
    monitor_interval_ = interval;
    monitoring_ = true;
    monitor_thread_ = std::thread(&ExternalNodeProcess::monitoringLoop, this);
}

void ExternalNodeProcess::stopMonitoring() {
    monitoring_ = false;
    if (monitor_thread_.joinable()) {
        monitor_thread_.join();
    }
}

ProcessState ExternalNodeProcess::getState() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(state_mutex_));
    return state_;
}

bool ExternalNodeProcess::isRunning() const {
    return getState() == ProcessState::RUNNING && stats_.is_running();
}

bool ExternalNodeProcess::isHealthy() const {
    return isRunning() && !checkResourceLimits();
}

void ExternalNodeProcess::onStateChange(std::function<void(ProcessState, ProcessState)> callback) {
    state_change_callback_ = callback;
}

void ExternalNodeProcess::onStatsUpdate(std::function<void(const ProcessStats&)> callback) {
    stats_callback_ = callback;
}

void ExternalNodeProcess::monitoringLoop() {
    while (monitoring_) {
        updateStats();
        
        if (isRunning()) {
            if (checkResourceLimits()) {
                setState(ProcessState::RESOURCE_LIMIT);
                stop();
            }
        } else if (state_ == ProcessState::RUNNING) {
            // Process died unexpectedly
            setState(ProcessState::CRASHED);
            
            if (should_restart_ && stats_.restart_count < limits_.max_restart_count) {
                scheduleRestart();
            }
        }
        
        std::this_thread::sleep_for(monitor_interval_);
    }
}

void ExternalNodeProcess::updateStats() {
    if (stats_.pid <= 0) {
        return;
    }
    
    auto new_stats = ProcessLauncher::getProcessStats(stats_.pid);
    if (new_stats.pid > 0) {
        stats_.memory_usage_kb = new_stats.memory_usage_kb;
        stats_.peak_memory_kb = std::max(stats_.peak_memory_kb, new_stats.memory_usage_kb);
        stats_.cpu_time = new_stats.cpu_time;
        stats_.file_descriptors_count = new_stats.file_descriptors_count;
        
        if (stats_callback_) {
            stats_callback_(stats_);
        }
    }
}

void ExternalNodeProcess::setState(ProcessState new_state) {
    ProcessState old_state = state_;
    state_ = new_state;
    
    if (state_change_callback_ && old_state != new_state) {
        state_change_callback_(old_state, new_state);
    }
}

bool ExternalNodeProcess::checkResourceLimits() const {
    if (stats_.memory_usage_mb() > limits_.max_memory_mb) {
        return true;
    }
    
    if (stats_.cpu_usage_percent() > limits_.max_cpu_percent) {
        return true;
    }
    
    if (stats_.uptime() > limits_.timeout) {
        return true;
    }
    
    return false;
}

void ExternalNodeProcess::scheduleRestart() {
    stats_.restart_count++;
    stats_.last_restart = std::chrono::system_clock::now();
    
    // Schedule restart in a separate thread
    std::thread([this]() {
        std::this_thread::sleep_for(limits_.restart_delay);
        if (should_restart_) {
            restart();
        }
    }).detach();
}

// =============================================================================
// ExternalNodeProcessManager Implementation
// =============================================================================

ExternalNodeProcessManager::ExternalNodeProcessManager()
    : global_monitoring_(false), global_monitor_interval_(std::chrono::seconds(10)),
      max_total_processes_(100), max_total_memory_mb_(4096),
      start_time_(std::chrono::system_clock::now()),
      total_processes_started_(0), total_processes_crashed_(0), total_restarts_(0) {
    
    // Set reasonable defaults
    default_limits_.max_memory_mb = 128;
    default_limits_.max_cpu_percent = 100.0;
    default_limits_.timeout = std::chrono::seconds(300);
    default_limits_.max_restart_count = 3;
    default_limits_.restart_delay = std::chrono::seconds(5);
}

ExternalNodeProcessManager::~ExternalNodeProcessManager() {
    stopGlobalMonitoring();
    removeAllProcesses();
}

bool ExternalNodeProcessManager::createProcess(const std::string& node_id, const discovery::NodeManifest& manifest) {
    if (!hasResourcesAvailable()) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(processes_mutex_);
    
    // Remove existing process if it exists
    processes_.erase(node_id);
    
    auto process = std::make_unique<ExternalNodeProcess>(node_id);
    process->setExecutable(manifest.getExecutablePath());
    process->setArguments(manifest.runtime.args);
    process->setEnvironment(manifest.runtime.env);
    process->setWorkingDirectory(manifest.runtime.working_dir);
    process->setLimits(createLimitsFromManifest(manifest));
    
    processes_[node_id] = std::move(process);
    return true;
}

bool ExternalNodeProcessManager::startProcess(const std::string& node_id) {
    auto process = getProcess(node_id);
    if (!process) {
        return false;
    }
    
    if (process->start()) {
        total_processes_started_++;
        process->startMonitoring();
        return true;
    }
    
    return false;
}

bool ExternalNodeProcessManager::stopProcess(const std::string& node_id, std::chrono::seconds timeout) {
    auto process = getProcess(node_id);
    if (!process) {
        return false;
    }
    
    process->stopMonitoring();
    return process->stop(timeout);
}

bool ExternalNodeProcessManager::restartProcess(const std::string& node_id) {
    auto process = getProcess(node_id);
    if (!process) {
        return false;
    }
    
    if (process->restart()) {
        total_restarts_++;
        return true;
    }
    
    return false;
}

void ExternalNodeProcessManager::removeProcess(const std::string& node_id) {
    std::lock_guard<std::mutex> lock(processes_mutex_);
    
    auto it = processes_.find(node_id);
    if (it != processes_.end()) {
        it->second->stopMonitoring();
        if (it->second->isRunning()) {
            it->second->stop();
        }
        processes_.erase(it);
    }
}

std::map<std::string, bool> ExternalNodeProcessManager::startAllProcesses() {
    std::map<std::string, bool> results;
    
    auto process_ids = getProcessIds();
    for (const auto& id : process_ids) {
        results[id] = startProcess(id);
    }
    
    return results;
}

std::map<std::string, bool> ExternalNodeProcessManager::stopAllProcesses(std::chrono::seconds timeout) {
    std::map<std::string, bool> results;
    
    auto process_ids = getProcessIds();
    for (const auto& id : process_ids) {
        results[id] = stopProcess(id, timeout);
    }
    
    return results;
}

void ExternalNodeProcessManager::removeAllProcesses() {
    stopAllProcesses();
    
    std::lock_guard<std::mutex> lock(processes_mutex_);
    processes_.clear();
}

ExternalNodeProcess* ExternalNodeProcessManager::getProcess(const std::string& node_id) {
    std::lock_guard<std::mutex> lock(processes_mutex_);
    auto it = processes_.find(node_id);
    return it != processes_.end() ? it->second.get() : nullptr;
}

const ExternalNodeProcess* ExternalNodeProcessManager::getProcess(const std::string& node_id) const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(processes_mutex_));
    auto it = processes_.find(node_id);
    return it != processes_.end() ? it->second.get() : nullptr;
}

std::vector<std::string> ExternalNodeProcessManager::getProcessIds() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(processes_mutex_));
    std::vector<std::string> ids;
    for (const auto& pair : processes_) {
        ids.push_back(pair.first);
    }
    return ids;
}

std::vector<std::string> ExternalNodeProcessManager::getRunningProcessIds() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(processes_mutex_));
    std::vector<std::string> ids;
    for (const auto& pair : processes_) {
        if (pair.second->isRunning()) {
            ids.push_back(pair.first);
        }
    }
    return ids;
}

size_t ExternalNodeProcessManager::getProcessCount() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(processes_mutex_));
    return processes_.size();
}

size_t ExternalNodeProcessManager::getRunningProcessCount() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(processes_mutex_));
    return std::count_if(processes_.begin(), processes_.end(),
                        [](const auto& pair) { return pair.second->isRunning(); });
}

ExternalNodeProcessManager::ManagerStats ExternalNodeProcessManager::getManagerStats() const {
    ManagerStats stats;
    stats.total_processes = getProcessCount();
    stats.running_processes = getRunningProcessCount();
    stats.total_memory_mb = getTotalMemoryUsage();
    stats.total_cpu_percent = getTotalCpuUsage();
    stats.uptime = std::chrono::system_clock::now() - start_time_;
    stats.total_started = total_processes_started_;
    stats.total_crashed = total_processes_crashed_;
    stats.total_restarts = total_restarts_;
    
    return stats;
}

bool ExternalNodeProcessManager::hasResourcesAvailable() const {
    return getProcessCount() < max_total_processes_ && 
           getTotalMemoryUsage() < max_total_memory_mb_;
}

ProcessLimits ExternalNodeProcessManager::createLimitsFromManifest(const discovery::NodeManifest& manifest) {
    ProcessLimits limits = default_limits_;
    
    // Parse memory limit from manifest
    if (!manifest.resources.memory.empty()) {
        std::regex memory_regex(R"((\d+)(MB|GB|KB)?)");
        std::smatch match;
        if (std::regex_search(manifest.resources.memory, match, memory_regex)) {
            int value = std::stoi(match[1]);
            std::string unit = match[2];
            
            if (unit == "GB") {
                limits.max_memory_mb = value * 1024;
            } else if (unit == "KB") {
                limits.max_memory_mb = value / 1024;
            } else { // MB or no unit
                limits.max_memory_mb = value;
            }
        }
    }
    
    // Parse timeout
    if (manifest.resources.timeout_seconds > 0) {
        limits.timeout = std::chrono::seconds(manifest.resources.timeout_seconds);
    }
    
    return limits;
}

size_t ExternalNodeProcessManager::getTotalMemoryUsage() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(processes_mutex_));
    size_t total = 0;
    for (const auto& pair : processes_) {
        if (pair.second->isRunning()) {
            total += static_cast<size_t>(pair.second->getStats().memory_usage_mb());
        }
    }
    return total;
}

double ExternalNodeProcessManager::getTotalCpuUsage() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(processes_mutex_));
    double total = 0.0;
    for (const auto& pair : processes_) {
        if (pair.second->isRunning()) {
            total += pair.second->getStats().cpu_usage_percent();
        }
    }
    return total;
}

// =============================================================================
// ProcessLauncher Implementation
// =============================================================================

ProcessLauncher::LaunchResult ProcessLauncher::launch(const LaunchConfig& config) {
    LaunchResult result;
    
    if (config.executable.empty()) {
        result.error_message = "Executable path is empty";
        return result;
    }
    
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        if (!setupChildProcess(config)) {
            exit(1);
        }
        
        // Prepare arguments
        std::vector<char*> argv;
        argv.push_back(const_cast<char*>(config.executable.c_str()));
        for (const auto& arg : config.args) {
            argv.push_back(const_cast<char*>(arg.c_str()));
        }
        argv.push_back(nullptr);
        
        // Execute
        execv(config.executable.c_str(), argv.data());
        exit(1); // execv failed
    } else if (pid > 0) {
        // Parent process
        result.success = true;
        result.pid = pid;
    } else {
        // Fork failed
        result.error_message = "Failed to fork process";
        result.error_code = errno;
    }
    
    return result;
}

bool ProcessLauncher::terminate(pid_t pid, std::chrono::seconds timeout) {
    if (pid <= 0) {
        return false;
    }
    
    // Send SIGTERM
    if (::kill(pid, SIGTERM) != 0) {
        return false;
    }
    
    // Wait for process to exit
    auto deadline = std::chrono::system_clock::now() + timeout;
    while (std::chrono::system_clock::now() < deadline) {
        if (!isRunning(pid)) {
            return true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    return false;
}

bool ProcessLauncher::kill(pid_t pid) {
    if (pid <= 0) {
        return false;
    }
    
    return ::kill(pid, SIGKILL) == 0;
}

bool ProcessLauncher::isRunning(pid_t pid) {
    if (pid <= 0) {
        return false;
    }
    
    return ::kill(pid, 0) == 0;
}

ProcessStats ProcessLauncher::getProcessStats(pid_t pid) {
    ProcessStats stats;
    stats.pid = pid;
    
    if (pid <= 0) {
        return stats;
    }
    
    // Read from /proc/pid/stat
    std::ifstream stat_file("/proc/" + std::to_string(pid) + "/stat");
    if (stat_file.is_open()) {
        std::string line;
        std::getline(stat_file, line);
        std::istringstream iss(line);
        
        // Parse stat fields (simplified)
        std::string comm, state;
        int ppid, pgrp, session, tty_nr, tpgid;
        unsigned flags;
        unsigned long minflt, cminflt, majflt, cmajflt;
        unsigned long utime, stime, cutime, cstime;
        long priority, nice, num_threads, itrealvalue;
        unsigned long long starttime;
        unsigned long vsize;
        long rss;
        
        iss >> stats.pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr >> tpgid
            >> flags >> minflt >> cminflt >> majflt >> cmajflt
            >> utime >> stime >> cutime >> cstime >> priority >> nice >> num_threads
            >> itrealvalue >> starttime >> vsize >> rss;
        
        // Convert values
        stats.memory_usage_kb = rss * getpagesize() / 1024;
        stats.cpu_time = std::chrono::duration<double>((utime + stime) / 100.0); // Assuming 100 HZ
    }
    
    return stats;
}

bool ProcessLauncher::setupChildProcess(const LaunchConfig& config) {
    // Change working directory
    if (!config.working_directory.empty()) {
        if (chdir(config.working_directory.c_str()) != 0) {
            return false;
        }
    }
    
    // Set environment variables
    for (const auto& env : config.environment) {
        if (setenv(env.first.c_str(), env.second.c_str(), 1) != 0) {
            return false;
        }
    }
    
    // Set resource limits
    if (!setResourceLimits(config.limits)) {
        return false;
    }
    
    // Redirect I/O
    if (!redirectIO(config)) {
        return false;
    }
    
    // Setup security
    if (!setupSecurity(config)) {
        return false;
    }
    
    // Create new session if requested
    if (config.create_new_session) {
        setsid();
    }
    
    return true;
}

bool ProcessLauncher::setResourceLimits(const ProcessLimits& limits) {
    // Memory limit
    struct rlimit memory_limit;
    memory_limit.rlim_cur = limits.max_memory_mb * 1024 * 1024;
    memory_limit.rlim_max = limits.max_memory_mb * 1024 * 1024;
    if (setrlimit(RLIMIT_AS, &memory_limit) != 0) {
        return false;
    }
    
    // File descriptor limit
    struct rlimit fd_limit;
    fd_limit.rlim_cur = limits.max_file_descriptors;
    fd_limit.rlim_max = limits.max_file_descriptors;
    if (setrlimit(RLIMIT_NOFILE, &fd_limit) != 0) {
        return false;
    }
    
    // CPU time limit
    struct rlimit cpu_limit;
    cpu_limit.rlim_cur = limits.timeout.count();
    cpu_limit.rlim_max = limits.timeout.count();
    if (setrlimit(RLIMIT_CPU, &cpu_limit) != 0) {
        return false;
    }
    
    // Core dump limit
    struct rlimit core_limit;
    core_limit.rlim_cur = limits.enable_core_dumps ? RLIM_INFINITY : 0;
    core_limit.rlim_max = limits.enable_core_dumps ? RLIM_INFINITY : 0;
    if (setrlimit(RLIMIT_CORE, &core_limit) != 0) {
        return false;
    }
    
    return true;
}

bool ProcessLauncher::redirectIO(const LaunchConfig& config) {
    if (!config.stdout_file.empty()) {
        int fd = open(config.stdout_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            return false;
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }
    
    if (!config.stderr_file.empty()) {
        int fd = open(config.stderr_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            return false;
        }
        dup2(fd, STDERR_FILENO);
        close(fd);
    }
    
    return true;
}

bool ProcessLauncher::setupSecurity(const LaunchConfig& config) {
    // Set nice level
    if (nice(config.limits.nice_level) == -1 && errno != 0) {
        return false;
    }
    
    return true;
}

} // namespace process
} // namespace orchestrator
} // namespace engine
} // namespace core
} // namespace akao