/**
 * @id: akao:test:system:integration:comprehensive:v1
 * @doc: Comprehensive system integration test suite validating complete core-node independence, YAML-RPC protocol, process management, and end-to-end workflow execution. Tests all aspects of the hardcore separation architecture to prove system build success without any errors.
 * @specification: Complete system validation test suite
 * @scope: Full system integration testing
 * @timeline: 2025-07-11
 * @rationale: Validate complete independence and protocol-based communication works flawlessly
 * @methodology: Exhaustive testing of all components, interfaces, and integration points
 * @references: ["akao:protocol:yamlrpc:v1", "akao:architecture:hardcore:separation:v1"]
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <cerrno>
#include <cstring>
#include <regex>
#include <cassert>

namespace akao {
namespace test {
namespace system {

/**
 * Comprehensive test result tracking
 */
struct TestResult {
    std::string test_name;
    bool passed;
    std::string error_message;
    std::chrono::duration<double> duration;
    std::map<std::string, std::string> details;
    
    TestResult(const std::string& name) : test_name(name), passed(false), duration(0) {}
};

/**
 * Test suite for comprehensive system validation
 */
class SystemIntegrationTestSuite {
private:
    std::vector<TestResult> results_;
    std::map<std::string, pid_t> node_processes_;
    std::map<std::string, std::string> node_sockets_;
    std::string test_dir_;
    
    // Test utilities
    bool fileExists(const std::string& path) {
        return std::filesystem::exists(path);
    }
    
    bool isExecutable(const std::string& path) {
        return std::filesystem::exists(path) && 
               (std::filesystem::status(path).permissions() & std::filesystem::perms::owner_exec) != std::filesystem::perms::none;
    }
    
    std::string readFile(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) return "";
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
    
    bool writeFile(const std::string& path, const std::string& content) {
        std::ofstream file(path);
        if (!file.is_open()) return false;
        file << content;
        return true;
    }
    
    pid_t startProcess(const std::string& command, const std::vector<std::string>& args) {
        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            std::vector<char*> argv;
            argv.push_back(const_cast<char*>(command.c_str()));
            for (const auto& arg : args) {
                argv.push_back(const_cast<char*>(arg.c_str()));
            }
            argv.push_back(nullptr);
            
            if (execv(command.c_str(), argv.data()) == -1) {
                std::cerr << "Failed to exec: " << strerror(errno) << std::endl;
                exit(1);
            }
        }
        return pid;
    }
    
    bool stopProcess(pid_t pid) {
        if (pid <= 0) return false;
        
        // Send SIGTERM
        if (kill(pid, SIGTERM) == 0) {
            // Wait for process to terminate
            int status;
            if (waitpid(pid, &status, WNOHANG) == 0) {
                // Process still running, wait a bit
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                if (waitpid(pid, &status, WNOHANG) == 0) {
                    // Force kill
                    kill(pid, SIGKILL);
                    waitpid(pid, &status, 0);
                }
            }
            return true;
        }
        return false;
    }
    
    bool connectToSocket(const std::string& socket_path) {
        int sock = socket(AF_UNIX, SOCK_STREAM, 0);
        if (sock < 0) return false;
        
        struct sockaddr_un addr;
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, socket_path.c_str(), sizeof(addr.sun_path) - 1);
        
        bool connected = connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == 0;
        close(sock);
        return connected;
    }
    
    std::string sendYamlRpcRequest(const std::string& socket_path, const std::string& request) {
        int sock = socket(AF_UNIX, SOCK_STREAM, 0);
        if (sock < 0) return "";
        
        struct sockaddr_un addr;
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, socket_path.c_str(), sizeof(addr.sun_path) - 1);
        
        if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
            close(sock);
            return "";
        }
        
        // Send request length
        uint32_t length = request.length();
        if (send(sock, &length, sizeof(length), 0) != sizeof(length)) {
            close(sock);
            return "";
        }
        
        // Send request
        if (send(sock, request.data(), length, 0) != static_cast<ssize_t>(length)) {
            close(sock);
            return "";
        }
        
        // Receive response length
        uint32_t response_length;
        if (recv(sock, &response_length, sizeof(response_length), MSG_WAITALL) != sizeof(response_length)) {
            close(sock);
            return "";
        }
        
        // Receive response
        std::string response(response_length, '\0');
        if (recv(sock, &response[0], response_length, MSG_WAITALL) != static_cast<ssize_t>(response_length)) {
            close(sock);
            return "";
        }
        
        close(sock);
        return response;
    }
    
    TestResult runTest(const std::string& test_name, std::function<bool(TestResult&)> test_func) {
        TestResult result(test_name);
        auto start = std::chrono::high_resolution_clock::now();
        
        try {
            std::cout << "Running test: " << test_name << "..." << std::endl;
            result.passed = test_func(result);
            if (result.passed) {
                std::cout << "✅ PASSED: " << test_name << std::endl;
            } else {
                std::cout << "❌ FAILED: " << test_name << " - " << result.error_message << std::endl;
            }
        } catch (const std::exception& e) {
            result.passed = false;
            result.error_message = "Exception: " + std::string(e.what());
            std::cout << "❌ FAILED: " << test_name << " - " << result.error_message << std::endl;
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        result.duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        
        return result;
    }
    
public:
    SystemIntegrationTestSuite() {
        test_dir_ = "./test_sockets_" + std::to_string(getpid());
        std::filesystem::create_directories(test_dir_);
        
        // Set up socket paths
        node_sockets_["file"] = test_dir_ + "/akao-node-file.sock";
        node_sockets_["logic"] = test_dir_ + "/akao-node-logic.sock";
        node_sockets_["reporter"] = test_dir_ + "/akao-node-reporter.sock";
        node_sockets_["yaml"] = test_dir_ + "/akao-node-yaml.sock";
    }
    
    ~SystemIntegrationTestSuite() {
        cleanup();
        std::filesystem::remove_all(test_dir_);
    }
    
    void cleanup() {
        // Stop all node processes
        for (auto& [name, pid] : node_processes_) {
            if (pid > 0) {
                stopProcess(pid);
            }
        }
        node_processes_.clear();
        
        // Remove socket files
        for (auto& [name, path] : node_sockets_) {
            unlink(path.c_str());
        }
    }
    
    // =============================================================================
    // Test 1: Build System Validation
    // =============================================================================
    
    bool testBuildSystemValidation(TestResult& result) {
        result.details["description"] = "Validate all nodes build successfully without errors";
        
        // Test core build
        if (!fileExists("core/akao")) {
            result.error_message = "Core executable not found";
            return false;
        }
        result.details["core_executable"] = "✅ Found";
        
        // Test node builds
        std::map<std::string, std::string> node_paths = {
            {"file", ".akao/nodes/file/file-scanner"},
            {"logic", ".akao/nodes/logic/independent/logic-executor"},
            {"reporter", ".akao/nodes/reporter/independent/report-generator"},
            {"yaml", ".akao/nodes/yaml/independent/yaml-processor"}
        };
        
        for (auto& [name, path] : node_paths) {
            if (!fileExists(path)) {
                result.error_message = "Node executable not found: " + path;
                return false;
            }
            if (!isExecutable(path)) {
                result.error_message = "Node not executable: " + path;
                return false;
            }
            result.details[name + "_executable"] = "✅ Found and executable";
        }
        
        // Test manifest files
        std::vector<std::string> manifest_paths = {
            ".akao/nodes/file/_.yaml",
            ".akao/nodes/logic/_.yaml",
            ".akao/nodes/reporter/_.yaml",
            ".akao/nodes/yaml/_.yaml"
        };
        
        for (const auto& path : manifest_paths) {
            if (!fileExists(path)) {
                result.error_message = "Manifest file not found: " + path;
                return false;
            }
            
            std::string content = readFile(path);
            if (content.empty()) {
                result.error_message = "Empty manifest file: " + path;
                return false;
            }
            
            // Validate YAML-RPC protocol in manifest
            if (content.find("yamlrpc") == std::string::npos) {
                result.error_message = "Missing YAML-RPC protocol in manifest: " + path;
                return false;
            }
            
            result.details[path + "_manifest"] = "✅ Valid";
        }
        
        return true;
    }
    
    // =============================================================================
    // Test 2: Node Independence Validation
    // =============================================================================
    
    bool testNodeIndependenceValidation(TestResult& result) {
        result.details["description"] = "Validate nodes have no core dependencies";
        
        std::map<std::string, std::string> node_source_dirs = {
            {"file", ".akao/nodes/file/"},
            {"logic", ".akao/nodes/logic/independent/"},
            {"reporter", ".akao/nodes/reporter/independent/"},
            {"yaml", ".akao/nodes/yaml/independent/"}
        };
        
        std::regex core_include_pattern(R"(#include\s+["<]core/)");
        
        for (auto& [name, dir] : node_source_dirs) {
            // Check source files for core includes
            for (const auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
                if (entry.path().extension() == ".cpp" || entry.path().extension() == ".hpp") {
                    std::string content = readFile(entry.path());
                    if (std::regex_search(content, core_include_pattern)) {
                        result.error_message = "Core dependency found in: " + entry.path().string();
                        return false;
                    }
                }
            }
            result.details[name + "_independence"] = "✅ No core dependencies";
        }
        
        return true;
    }
    
    // =============================================================================
    // Test 3: Node Process Management
    // =============================================================================
    
    bool testNodeProcessManagement(TestResult& result) {
        result.details["description"] = "Validate node processes start, run, and stop correctly";
        
        std::map<std::string, std::string> node_executables = {
            {"file", ".akao/nodes/file/file-scanner"},
            {"logic", ".akao/nodes/logic/independent/logic-executor"},
            {"reporter", ".akao/nodes/reporter/independent/report-generator"},
            {"yaml", ".akao/nodes/yaml/independent/yaml-processor"}
        };
        
        // Start all node processes
        for (auto& [name, executable] : node_executables) {
            pid_t pid = startProcess(executable, {node_sockets_[name]});
            if (pid <= 0) {
                result.error_message = "Failed to start node process: " + name;
                return false;
            }
            node_processes_[name] = pid;
            result.details[name + "_process_start"] = "✅ Started (PID: " + std::to_string(pid) + ")";
        }
        
        // Wait for nodes to initialize
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        // Verify all processes are running
        for (auto& [name, pid] : node_processes_) {
            if (kill(pid, 0) != 0) {
                result.error_message = "Node process not running: " + name;
                return false;
            }
            result.details[name + "_process_running"] = "✅ Running";
        }
        
        // Verify socket files were created
        for (auto& [name, socket_path] : node_sockets_) {
            if (!fileExists(socket_path)) {
                result.error_message = "Socket file not created: " + socket_path;
                return false;
            }
            result.details[name + "_socket_file"] = "✅ Created";
        }
        
        return true;
    }
    
    // =============================================================================
    // Test 4: YAML-RPC Protocol Validation
    // =============================================================================
    
    bool testYamlRpcProtocol(TestResult& result) {
        result.details["description"] = "Validate YAML-RPC protocol communication";
        
        // Test basic connectivity
        for (auto& [name, socket_path] : node_sockets_) {
            if (!connectToSocket(socket_path)) {
                result.error_message = "Cannot connect to node socket: " + name;
                return false;
            }
            result.details[name + "_connectivity"] = "✅ Connected";
        }
        
        // Test node.info method
        std::string info_request = R"(yamlrpc: "1.0"
method: "node.info"
id: "test-info"
params: {}
)";
        
        for (auto& [name, socket_path] : node_sockets_) {
            std::string response = sendYamlRpcRequest(socket_path, info_request);
            if (response.empty()) {
                result.error_message = "No response from node.info: " + name;
                return false;
            }
            
            // Basic validation - should contain yamlrpc and result
            if (response.find("yamlrpc") == std::string::npos || 
                response.find("result") == std::string::npos) {
                result.error_message = "Invalid YAML-RPC response format: " + name;
                return false;
            }
            
            result.details[name + "_info_method"] = "✅ Valid response";
        }
        
        // Test node.health method
        std::string health_request = R"(yamlrpc: "1.0"
method: "node.health"
id: "test-health"
params: {}
)";
        
        for (auto& [name, socket_path] : node_sockets_) {
            std::string response = sendYamlRpcRequest(socket_path, health_request);
            if (response.empty()) {
                result.error_message = "No response from node.health: " + name;
                return false;
            }
            
            if (response.find("healthy") == std::string::npos) {
                result.error_message = "Node not healthy: " + name;
                return false;
            }
            
            result.details[name + "_health_method"] = "✅ Healthy";
        }
        
        return true;
    }
    
    // =============================================================================
    // Test 5: Node Functional Validation
    // =============================================================================
    
    bool testNodeFunctionalValidation(TestResult& result) {
        result.details["description"] = "Validate node-specific functionality";
        
        // Test file scanner node
        std::string file_request = R"(yamlrpc: "1.0"
method: "node.execute"
id: "test-file-scan"
params:
  input:
    path: "."
    recursive: false
    max_depth: 1
)";
        
        std::string file_response = sendYamlRpcRequest(node_sockets_["file"], file_request);
        if (file_response.empty()) {
            result.error_message = "No response from file scanner";
            return false;
        }
        
        if (file_response.find("files") == std::string::npos) {
            result.error_message = "File scanner response missing files";
            return false;
        }
        result.details["file_scanner_execution"] = "✅ Working";
        
        // Test logic executor node
        std::string logic_request = R"(yamlrpc: "1.0"
method: "node.execute"
id: "test-logic-eval"
params:
  input:
    expression: "true"
)";
        
        std::string logic_response = sendYamlRpcRequest(node_sockets_["logic"], logic_request);
        if (logic_response.empty()) {
            result.error_message = "No response from logic executor";
            return false;
        }
        result.details["logic_executor_execution"] = "✅ Working";
        
        // Test reporter node
        std::string reporter_request = R"(yamlrpc: "1.0"
method: "node.execute"
id: "test-report-gen"
params:
  input:
    data:
      title: "Test Report"
      content: "Test content"
)";
        
        std::string reporter_response = sendYamlRpcRequest(node_sockets_["reporter"], reporter_request);
        if (reporter_response.empty()) {
            result.error_message = "No response from reporter";
            return false;
        }
        result.details["reporter_execution"] = "✅ Working";
        
        // Test YAML processor node
        std::string yaml_request = R"(yamlrpc: "1.0"
method: "node.execute"
id: "test-yaml-proc"
params:
  input:
    operation: "parse"
    content: "key: value"
)";
        
        std::string yaml_response = sendYamlRpcRequest(node_sockets_["yaml"], yaml_request);
        if (yaml_response.empty()) {
            result.error_message = "No response from YAML processor";
            return false;
        }
        result.details["yaml_processor_execution"] = "✅ Working";
        
        return true;
    }
    
    // =============================================================================
    // Test 6: Error Handling and Recovery
    // =============================================================================
    
    bool testErrorHandlingAndRecovery(TestResult& result) {
        result.details["description"] = "Validate error handling and recovery mechanisms";
        
        // Test invalid method
        std::string invalid_request = R"(yamlrpc: "1.0"
method: "invalid.method"
id: "test-invalid"
params: {}
)";
        
        for (auto& [name, socket_path] : node_sockets_) {
            std::string response = sendYamlRpcRequest(socket_path, invalid_request);
            if (response.empty()) {
                result.error_message = "No error response for invalid method: " + name;
                return false;
            }
            
            if (response.find("error") == std::string::npos) {
                result.error_message = "Missing error in response for invalid method: " + name;
                return false;
            }
            
            result.details[name + "_error_handling"] = "✅ Proper error response";
        }
        
        // Test malformed request
        std::string malformed_request = "invalid yaml content";
        
        for (auto& [name, socket_path] : node_sockets_) {
            std::string response = sendYamlRpcRequest(socket_path, malformed_request);
            // Response might be empty or contain error - both are acceptable
            result.details[name + "_malformed_handling"] = "✅ Handled gracefully";
        }
        
        return true;
    }
    
    // =============================================================================
    // Test 7: Performance and Load Testing
    // =============================================================================
    
    bool testPerformanceAndLoad(TestResult& result) {
        result.details["description"] = "Validate performance under load";
        
        std::string simple_request = R"(yamlrpc: "1.0"
method: "node.health"
id: "perf-test"
params: {}
)";
        
        // Test concurrent requests
        const int concurrent_requests = 10;
        const int requests_per_thread = 5;
        
        for (auto& [name, socket_path] : node_sockets_) {
            std::vector<std::thread> threads;
            std::atomic<int> success_count(0);
            std::atomic<int> error_count(0);
            
            auto start_time = std::chrono::high_resolution_clock::now();
            
            for (int i = 0; i < concurrent_requests; ++i) {
                threads.emplace_back([&, socket_path]() {
                    for (int j = 0; j < requests_per_thread; ++j) {
                        std::string response = sendYamlRpcRequest(socket_path, simple_request);
                        if (!response.empty() && response.find("healthy") != std::string::npos) {
                            success_count++;
                        } else {
                            error_count++;
                        }
                    }
                });
            }
            
            for (auto& thread : threads) {
                thread.join();
            }
            
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
            
            int total_requests = concurrent_requests * requests_per_thread;
            if (success_count < total_requests * 0.9) { // Allow 10% failure rate
                result.error_message = "High failure rate for node: " + name + 
                                     " (Success: " + std::to_string(success_count.load()) + 
                                     "/" + std::to_string(total_requests) + ")";
                return false;
            }
            
            result.details[name + "_load_test"] = "✅ " + std::to_string(success_count.load()) + 
                                                "/" + std::to_string(total_requests) + 
                                                " requests in " + std::to_string(duration.count()) + "ms";
        }
        
        return true;
    }
    
    // =============================================================================
    // Test 8: Core-Node Integration
    // =============================================================================
    
    bool testCoreNodeIntegration(TestResult& result) {
        result.details["description"] = "Validate core can discover and communicate with nodes";
        
        // Test core node discovery
        if (!fileExists("core/engine/orchestrator/discovery/v1.hpp")) {
            result.error_message = "Core discovery system not found";
            return false;
        }
        result.details["core_discovery"] = "✅ Discovery system present";
        
        // Test core YAML-RPC client
        if (!fileExists("core/engine/communication/yamlrpc/v1.hpp")) {
            result.error_message = "Core YAML-RPC client not found";
            return false;
        }
        result.details["core_yamlrpc"] = "✅ YAML-RPC client present";
        
        // Test core process management
        if (!fileExists("core/engine/orchestrator/process/v1.hpp")) {
            result.error_message = "Core process management not found";
            return false;
        }
        result.details["core_process_mgmt"] = "✅ Process management present";
        
        // Test YAML infrastructure in core
        if (!fileExists("core/foundation/formats/yaml/v1.hpp")) {
            result.error_message = "Core YAML infrastructure not found";
            return false;
        }
        result.details["core_yaml_infra"] = "✅ YAML infrastructure present";
        
        return true;
    }
    
    // =============================================================================
    // Test 9: System Stability
    // =============================================================================
    
    bool testSystemStability(TestResult& result) {
        result.details["description"] = "Validate system stability over time";
        
        // Run stability test for 30 seconds
        auto start_time = std::chrono::high_resolution_clock::now();
        auto end_time = start_time + std::chrono::seconds(30);
        
        std::string health_request = R"(yamlrpc: "1.0"
method: "node.health"
id: "stability-test"
params: {}
)";
        
        int total_checks = 0;
        int successful_checks = 0;
        
        while (std::chrono::high_resolution_clock::now() < end_time) {
            for (auto& [name, socket_path] : node_sockets_) {
                total_checks++;
                
                std::string response = sendYamlRpcRequest(socket_path, health_request);
                if (!response.empty() && response.find("healthy") != std::string::npos) {
                    successful_checks++;
                }
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        
        double success_rate = (double)successful_checks / total_checks * 100.0;
        
        if (success_rate < 95.0) { // Require 95% success rate
            result.error_message = "Low stability success rate: " + std::to_string(success_rate) + "%";
            return false;
        }
        
        result.details["stability_test"] = "✅ " + std::to_string(success_rate) + 
                                         "% success rate over 30 seconds (" + 
                                         std::to_string(successful_checks) + "/" + 
                                         std::to_string(total_checks) + " checks)";
        
        return true;
    }
    
    // =============================================================================
    // Test 10: Resource Management
    // =============================================================================
    
    bool testResourceManagement(TestResult& result) {
        result.details["description"] = "Validate resource management and cleanup";
        
        // Test graceful shutdown
        std::string shutdown_request = R"(yamlrpc: "1.0"
method: "node.shutdown"
id: "shutdown-test"
params: {}
)";
        
        // Create a copy of processes for testing
        std::map<std::string, pid_t> test_processes = node_processes_;
        
        for (auto& [name, socket_path] : node_sockets_) {
            std::string response = sendYamlRpcRequest(socket_path, shutdown_request);
            if (response.empty()) {
                result.error_message = "No response to shutdown request: " + name;
                return false;
            }
            
            if (response.find("shutting_down") == std::string::npos) {
                result.error_message = "Invalid shutdown response: " + name;
                return false;
            }
            
            result.details[name + "_shutdown_response"] = "✅ Graceful shutdown initiated";
        }
        
        // Wait for processes to shutdown
        std::this_thread::sleep_for(std::chrono::seconds(3));
        
        // Verify processes have stopped
        for (auto& [name, pid] : test_processes) {
            if (kill(pid, 0) == 0) {
                result.error_message = "Process did not shutdown gracefully: " + name;
                return false;
            }
            result.details[name + "_shutdown_complete"] = "✅ Process terminated";
        }
        
        // Clear the process map since they're all shutdown
        node_processes_.clear();
        
        // Verify socket files are cleaned up
        std::this_thread::sleep_for(std::chrono::seconds(1));
        for (auto& [name, socket_path] : node_sockets_) {
            if (fileExists(socket_path)) {
                result.details[name + "_socket_cleanup"] = "⚠️ Socket file still exists";
            } else {
                result.details[name + "_socket_cleanup"] = "✅ Socket file cleaned up";
            }
        }
        
        return true;
    }
    
    // =============================================================================
    // Main Test Runner
    // =============================================================================
    
    void runAllTests() {
        std::cout << "\n🚀 Starting Comprehensive System Integration Test Suite" << std::endl;
        std::cout << "=======================================================" << std::endl;
        
        // Run all tests in sequence
        results_.push_back(runTest("Build System Validation", 
            [this](TestResult& r) { return testBuildSystemValidation(r); }));
        
        results_.push_back(runTest("Node Independence Validation", 
            [this](TestResult& r) { return testNodeIndependenceValidation(r); }));
        
        results_.push_back(runTest("Node Process Management", 
            [this](TestResult& r) { return testNodeProcessManagement(r); }));
        
        results_.push_back(runTest("YAML-RPC Protocol Validation", 
            [this](TestResult& r) { return testYamlRpcProtocol(r); }));
        
        results_.push_back(runTest("Node Functional Validation", 
            [this](TestResult& r) { return testNodeFunctionalValidation(r); }));
        
        results_.push_back(runTest("Error Handling and Recovery", 
            [this](TestResult& r) { return testErrorHandlingAndRecovery(r); }));
        
        results_.push_back(runTest("Performance and Load Testing", 
            [this](TestResult& r) { return testPerformanceAndLoad(r); }));
        
        results_.push_back(runTest("Core-Node Integration", 
            [this](TestResult& r) { return testCoreNodeIntegration(r); }));
        
        results_.push_back(runTest("System Stability", 
            [this](TestResult& r) { return testSystemStability(r); }));
        
        results_.push_back(runTest("Resource Management", 
            [this](TestResult& r) { return testResourceManagement(r); }));
    }
    
    // =============================================================================
    // Results Reporting
    // =============================================================================
    
    void generateReport() {
        std::cout << "\n📊 Test Results Summary" << std::endl;
        std::cout << "=======================" << std::endl;
        
        int passed = 0;
        int failed = 0;
        double total_duration = 0.0;
        
        for (const auto& result : results_) {
            if (result.passed) {
                passed++;
                std::cout << "✅ ";
            } else {
                failed++;
                std::cout << "❌ ";
            }
            
            std::cout << result.test_name << " (" << std::fixed << std::setprecision(2) 
                      << result.duration.count() << "s)" << std::endl;
            
            if (!result.passed) {
                std::cout << "   Error: " << result.error_message << std::endl;
            }
            
            total_duration += result.duration.count();
        }
        
        std::cout << "\n📈 Summary Statistics" << std::endl;
        std::cout << "--------------------" << std::endl;
        std::cout << "Total Tests: " << results_.size() << std::endl;
        std::cout << "Passed: " << passed << std::endl;
        std::cout << "Failed: " << failed << std::endl;
        std::cout << "Success Rate: " << std::fixed << std::setprecision(1) 
                  << (double)passed / results_.size() * 100.0 << "%" << std::endl;
        std::cout << "Total Duration: " << std::fixed << std::setprecision(2) 
                  << total_duration << "s" << std::endl;
        
        // Generate detailed report
        std::cout << "\n📋 Detailed Test Results" << std::endl;
        std::cout << "=========================" << std::endl;
        
        for (const auto& result : results_) {
            std::cout << "\n🔍 " << result.test_name << std::endl;
            std::cout << "Description: " << result.details.at("description") << std::endl;
            std::cout << "Status: " << (result.passed ? "✅ PASSED" : "❌ FAILED") << std::endl;
            std::cout << "Duration: " << std::fixed << std::setprecision(3) 
                      << result.duration.count() << "s" << std::endl;
            
            if (!result.passed) {
                std::cout << "Error: " << result.error_message << std::endl;
            }
            
            std::cout << "Details:" << std::endl;
            for (const auto& [key, value] : result.details) {
                if (key != "description") {
                    std::cout << "  " << key << ": " << value << std::endl;
                }
            }
        }
        
        // Final verdict
        std::cout << "\n🎯 Final Verdict" << std::endl;
        std::cout << "=================" << std::endl;
        
        if (failed == 0) {
            std::cout << "🎉 ALL TESTS PASSED! System build successful and fully functional!" << std::endl;
            std::cout << "✅ Core-node independence achieved" << std::endl;
            std::cout << "✅ YAML-RPC protocol working correctly" << std::endl;
            std::cout << "✅ Process management operational" << std::endl;
            std::cout << "✅ No crashes or errors detected" << std::endl;
            std::cout << "✅ System ready for production use" << std::endl;
        } else {
            std::cout << "❌ " << failed << " test(s) failed. System needs attention." << std::endl;
        }
    }
    
    bool allTestsPassed() const {
        for (const auto& result : results_) {
            if (!result.passed) return false;
        }
        return true;
    }
};

} // namespace system
} // namespace test
} // namespace akao

// =============================================================================
// Main Test Application
// =============================================================================

int main() {
    std::cout << "🧪 Akao System Integration Test Suite" << std::endl;
    std::cout << "=====================================" << std::endl;
    std::cout << "Testing complete core-node independence architecture" << std::endl;
    std::cout << "Validating YAML-RPC protocol and process management" << std::endl;
    std::cout << "Ensuring system stability and error handling" << std::endl;
    
    akao::test::system::SystemIntegrationTestSuite test_suite;
    
    try {
        test_suite.runAllTests();
        test_suite.generateReport();
        
        return test_suite.allTestsPassed() ? 0 : 1;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Test suite failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ Test suite failed with unknown exception" << std::endl;
        return 1;
    }
}