/**
 * @id: akao:class:core:application:cli:v1
 * @doc: Command-line application for Akao C++ workflow orchestrator providing user interface for node and workflow management. Implements full orchestration approach using core infrastructure (YAML parsing, YAML-RPC communication) with no code duplication. Maintains hardcore separation architecture by routing all operations through core orchestrator.
 * @specification: CLI application using core foundation and engine libraries
 * @scope: Core application layer command-line interface
 * @timeline: 2025-07-12
 * @rationale: Provide user-friendly command-line interface leveraging existing core infrastructure
 * @methodology: Command parsing with core YAML-RPC client integration
 * @references: ["akao:class:core:engine:communication:yamlrpc:v1", "akao:class:core:foundation:formats:yaml:v1"]
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "core/foundation/formats/yaml/v1.hpp"
#include "core/engine/communication/yamlrpc/v1.hpp"
#include "config.hpp"

namespace akao::core::application::cli {

enum class CommandType {
    NODE_LIST,
    NODE_RUN,
    NODE_INSTALL,
    NODE_UNINSTALL,
    NODE_ENABLE,
    NODE_DISABLE,
    NODE_SEARCH,
    WORKFLOW_LIST,
    WORKFLOW_RUN,
    WORKFLOW_INSTALL,
    WORKFLOW_UNINSTALL,
    WORKFLOW_ENABLE,
    WORKFLOW_DISABLE,
    WORKFLOW_SEARCH,
    SYSTEM_VERSION,
    SYSTEM_STATUS,
    SYSTEM_VALIDATE,
    SYSTEM_TEST,
    HELP,
    UNKNOWN
};

enum class OutputFormat {
    HUMAN,
    JSON,
    YAML
};

struct GlobalFlags {
    bool verbose = false;
    bool quiet = false;
    bool debug = false;
    OutputFormat output_format = OutputFormat::HUMAN;
};

struct CommandContext {
    CommandType type;
    std::string target;
    std::vector<std::string> arguments;
    std::vector<std::string> parameters;
    GlobalFlags flags;
};

class ArgumentParser {
public:
    CommandContext parse(int argc, char* argv[]);
    void show_help(const std::string& command = "");
    void show_version();

private:
    CommandType parse_command(const std::string& cmd, const std::string& flag = "");
    GlobalFlags parse_global_flags(const std::vector<std::string>& args);
    std::vector<std::string> split_parameters(const std::vector<std::string>& args);
};

class CoreCommunicator {
public:
    CoreCommunicator();
    ~CoreCommunicator();

    bool connect(const std::string& socket_path = "/tmp/akao-core.sock");
    void disconnect();
    bool is_connected() const;

    std::string execute_command(const CommandContext& ctx);

private:
    std::unique_ptr<akao::core::engine::communication::yamlrpc::YamlRpcClient> rpc_client_;
    std::string formatResult(const std::shared_ptr<akao::foundation::formats::yaml::YamlNode>& result, OutputFormat format);
    std::string formatError(const std::string& error_message, int error_code);
};

class CLI {
public:
    CLI();
    ~CLI();

    int run(int argc, char* argv[]);

private:
    void setup_logging(const GlobalFlags& flags);
    std::string format_output(const std::string& data, OutputFormat format);
    void handle_error(const std::string& error, const GlobalFlags& flags);
    bool load_configuration();

    ArgumentParser parser;
    CoreCommunicator communicator;
    ConfigManager config_manager;
};

} // namespace akao::core::application::cli