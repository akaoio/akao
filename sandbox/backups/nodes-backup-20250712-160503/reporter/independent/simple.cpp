/**
 * @id: akao:node:reporter:generator:independent:v1
 * @doc: Independent report generator node with YAML-RPC communication. Simplified implementation.
 */

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>

class SimpleReportServer {
private:
    std::string socket_path_;
    int server_socket_;
    bool running_;
    
public:
    SimpleReportServer(const std::string& socket_path) 
        : socket_path_(socket_path), server_socket_(-1), running_(false) {}
    
    bool start() {
        server_socket_ = socket(AF_UNIX, SOCK_STREAM, 0);
        if (server_socket_ < 0) {
            std::cerr << "Failed to create socket: " << strerror(errno) << std::endl;
            return false;
        }
        
        struct sockaddr_un addr;
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, socket_path_.c_str(), sizeof(addr.sun_path) - 1);
        
        unlink(socket_path_.c_str());
        
        if (bind(server_socket_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            std::cerr << "Failed to bind socket: " << strerror(errno) << std::endl;
            close(server_socket_);
            return false;
        }
        
        if (listen(server_socket_, 5) < 0) {
            std::cerr << "Failed to listen: " << strerror(errno) << std::endl;
            close(server_socket_);
            return false;
        }
        
        running_ = true;
        return true;
    }
    
    void run() {
        while (running_) {
            int client = accept(server_socket_, nullptr, nullptr);
            if (client >= 0) {
                // Simple echo response for now
                char buffer[1024];
                recv(client, buffer, sizeof(buffer), 0);
                const char* response = "yamlrpc: \"1.0\"\nresult:\n  report: \"# Sample Report\\nGenerated successfully\"\n";
                send(client, response, strlen(response), 0);
                close(client);
            }
        }
    }
    
    void stop() {
        running_ = false;
        if (server_socket_ >= 0) {
            close(server_socket_);
            unlink(socket_path_.c_str());
        }
    }
    
    bool isRunning() const { return running_; }
};

int main(int argc, char* argv[]) {
    std::string socket_path = "/tmp/akao-node-reporter.sock";
    
    if (argc > 1) {
        socket_path = argv[1];
    }
    
    SimpleReportServer server(socket_path);
    
    if (!server.start()) {
        std::cerr << "Failed to start report generator node server" << std::endl;
        return 1;
    }
    
    std::cout << "Report generator node started on " << socket_path << std::endl;
    
    // Run server in background
    std::thread server_thread([&server]() {
        server.run();
    });
    
    // Wait for shutdown signal
    while (server.isRunning()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    server.stop();
    server_thread.join();
    
    return 0;
}