/**
 * @id: akao:class:core:engine:orchestrator:process:v1
 * @doc: Process management system for external node lifecycle management providing robust process spawning, monitoring, resource management, and health tracking. Manages the complete lifecycle of external node processes including startup, shutdown, restart, resource limits, environment setup, and crash recovery. Integrates with the discovery system to provide seamless external node process management for the YAML-RPC communication architecture.
 * @specification: External node process lifecycle management with monitoring and resource control
 * @scope: Core engine orchestrator process management system
 * @timeline: 2025-07-11
 * @rationale: Enable robust external node process management for core-node independence
 * @methodology: Process spawning with resource monitoring, lifecycle management, and health tracking
 * @references: ["akao:directory:core:engine:orchestrator:process:v1", "akao:class:core:engine:orchestrator:discovery:v1"]
 */

#pragma once

#include "core/engine/orchestrator/discovery/v1.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>
#include <future>

namespace akao {
namespace core {
namespace engine {
namespace orchestrator {
namespace process {

/**
 * Process resource limits and configuration
 */
struct ProcessLimits {
    size_t max_memory_mb = 128;        // Maximum memory usage in MB
    double max_cpu_percent = 100.0;    // Maximum CPU usage percentage
    int max_file_descriptors = 1024;   // Maximum open file descriptors
    std::chrono::seconds timeout = std::chrono::seconds(30); // Execution timeout
    int max_restart_count = 3;         // Maximum restart attempts
    std::chrono::seconds restart_delay = std::chrono::seconds(5); // Delay between restarts
    bool enable_core_dumps = false;    // Allow core dumps on crash
    int nice_level = 0;                // Process priority (-20 to 19)
};

/**
 * Process runtime statistics
 */
struct ProcessStats {
    pid_t pid = -1;
    std::chrono::system_clock::time_point start_time;
    std::chrono::duration<double> cpu_time = std::chrono::duration<double>(0);
    size_t memory_usage_kb = 0;
    size_t peak_memory_kb = 0;
    int file_descriptors_count = 0;
    int exit_code = 0;
    std::string exit_reason;
    int restart_count = 0;
    std::chrono::system_clock::time_point last_restart;
    
    // Runtime metrics
    std::chrono::duration<double> uptime() const;
    double cpu_usage_percent() const;
    double memory_usage_mb() const;
    bool is_running() const;
};

/**
 * Process state and status
 */
enum class ProcessState {
    STOPPED,        // Not running
    STARTING,       // Being spawned
    RUNNING,        // Active and healthy
    STOPPING,       // Being terminated
    CRASHED,        // Exited unexpectedly
    FAILED,         // Failed to start
    TIMEOUT,        // Execution timeout
    RESOURCE_LIMIT  // Hit resource limit
};

/**
 * External node process instance
 */
class ExternalNodeProcess {
private:
    std::string node_id_;
    std::string executable_path_;
    std::vector<std::string> args_;
    std::map<std::string, std::string> environment_;
    std::string working_directory_;
    
    ProcessLimits limits_;
    ProcessStats stats_;
    ProcessState state_;
    
    std::atomic<bool> should_restart_;
    std::mutex state_mutex_;
    
    // Process monitoring
    std::thread monitor_thread_;
    std::atomic<bool> monitoring_;
    std::chrono::seconds monitor_interval_;
    
    // Callbacks
    std::function<void(ProcessState, ProcessState)> state_change_callback_;
    std::function<void(const ProcessStats&)> stats_callback_;
    
public:
    ExternalNodeProcess(const std::string& node_id);
    ~ExternalNodeProcess();
    
    // Configuration
    void setExecutable(const std::string& path);
    void setArguments(const std::vector<std::string>& args);
    void setEnvironment(const std::map<std::string, std::string>& env);
    void setWorkingDirectory(const std::string& dir);
    void setLimits(const ProcessLimits& limits);
    
    // Lifecycle
    bool start();
    bool stop(std::chrono::seconds timeout = std::chrono::seconds(10));
    bool restart();
    bool kill();
    
    // Monitoring
    void startMonitoring(std::chrono::seconds interval = std::chrono::seconds(5));
    void stopMonitoring();
    bool isMonitoring() const { return monitoring_; }
    
    // Status
    ProcessState getState() const;
    const ProcessStats& getStats() const { return stats_; }
    bool isRunning() const;
    bool isHealthy() const;
    
    // Callbacks
    void onStateChange(std::function<void(ProcessState, ProcessState)> callback);
    void onStatsUpdate(std::function<void(const ProcessStats&)> callback);
    
    // Configuration access
    const std::string& getNodeId() const { return node_id_; }
    const std::string& getExecutablePath() const { return executable_path_; }
    const ProcessLimits& getLimits() const { return limits_; }
    
private:
    void monitoringLoop();
    void updateStats();
    void setState(ProcessState new_state);
    bool checkResourceLimits() const;
    void handleProcessExit(int exit_code);
    void scheduleRestart();
    
    // System utilities
    bool setProcessLimits(pid_t pid);
    ProcessStats getSystemStats(pid_t pid);
    bool isProcessAlive(pid_t pid);
    void cleanupProcess();
};

/**
 * Process manager for multiple external nodes
 */
class ExternalNodeProcessManager {
private:
    std::map<std::string, std::unique_ptr<ExternalNodeProcess>> processes_;
    std::mutex processes_mutex_;
    
    // Global monitoring
    std::thread global_monitor_thread_;
    std::atomic<bool> global_monitoring_;
    std::chrono::seconds global_monitor_interval_;
    
    // Resource management
    ProcessLimits default_limits_;
    size_t max_total_processes_;
    size_t max_total_memory_mb_;
    
    // Statistics
    std::chrono::system_clock::time_point start_time_;
    std::atomic<size_t> total_processes_started_;
    std::atomic<size_t> total_processes_crashed_;
    std::atomic<size_t> total_restarts_;
    
public:
    ExternalNodeProcessManager();
    ~ExternalNodeProcessManager();
    
    // Process management
    bool createProcess(const std::string& node_id, const discovery::NodeManifest& manifest);
    bool startProcess(const std::string& node_id);
    bool stopProcess(const std::string& node_id, std::chrono::seconds timeout = std::chrono::seconds(10));
    bool restartProcess(const std::string& node_id);
    void removeProcess(const std::string& node_id);
    
    // Batch operations
    std::map<std::string, bool> startAllProcesses();
    std::map<std::string, bool> stopAllProcesses(std::chrono::seconds timeout = std::chrono::seconds(10));
    std::map<std::string, bool> restartAllProcesses();
    void removeAllProcesses();
    
    // Process access
    ExternalNodeProcess* getProcess(const std::string& node_id);
    const ExternalNodeProcess* getProcess(const std::string& node_id) const;
    std::vector<std::string> getProcessIds() const;
    std::vector<std::string> getRunningProcessIds() const;
    
    // Configuration
    void setDefaultLimits(const ProcessLimits& limits);
    void setMaxTotalProcesses(size_t max_processes);
    void setMaxTotalMemory(size_t max_memory_mb);
    
    // Global monitoring
    void startGlobalMonitoring(std::chrono::seconds interval = std::chrono::seconds(10));
    void stopGlobalMonitoring();
    bool isGlobalMonitoringEnabled() const { return global_monitoring_; }
    
    // Statistics
    size_t getProcessCount() const;
    size_t getRunningProcessCount() const;
    size_t getTotalMemoryUsage() const;
    double getTotalCpuUsage() const;
    
    struct ManagerStats {
        size_t total_processes;
        size_t running_processes;
        size_t crashed_processes;
        size_t total_memory_mb;
        double total_cpu_percent;
        std::chrono::duration<double> uptime;
        size_t total_started;
        size_t total_crashed;
        size_t total_restarts;
    };
    
    ManagerStats getManagerStats() const;
    std::map<std::string, ProcessStats> getAllProcessStats() const;
    
    // Health checking
    std::map<std::string, bool> performHealthCheck();
    bool isSystemHealthy() const;
    
private:
    void globalMonitoringLoop();
    void checkSystemResources();
    void cleanupFailedProcesses();
    bool hasResourcesAvailable() const;
    ProcessLimits createLimitsFromManifest(const discovery::NodeManifest& manifest);
    
    // System utilities
    size_t getSystemMemoryUsage() const;
    double getSystemCpuUsage() const;
    void updateGlobalStats();
};

/**
 * Process launcher with advanced configuration
 */
class ProcessLauncher {
public:
    struct LaunchConfig {
        std::string executable;
        std::vector<std::string> args;
        std::map<std::string, std::string> environment;
        std::string working_directory;
        ProcessLimits limits;
        
        // Security
        bool create_new_session = true;
        bool isolate_namespace = false;
        std::string run_as_user;
        std::string run_as_group;
        
        // I/O redirection
        std::string stdout_file;
        std::string stderr_file;
        bool capture_output = false;
    };
    
    struct LaunchResult {
        bool success = false;
        pid_t pid = -1;
        std::string error_message;
        int error_code = 0;
    };
    
    static LaunchResult launch(const LaunchConfig& config);
    static bool terminate(pid_t pid, std::chrono::seconds timeout = std::chrono::seconds(10));
    static bool kill(pid_t pid);
    static bool isRunning(pid_t pid);
    static ProcessStats getProcessStats(pid_t pid);
    
private:
    static bool setupChildProcess(const LaunchConfig& config);
    static bool setResourceLimits(const ProcessLimits& limits);
    static bool redirectIO(const LaunchConfig& config);
    static bool setupSecurity(const LaunchConfig& config);
};

} // namespace process
} // namespace orchestrator
} // namespace engine
} // namespace core
} // namespace akao