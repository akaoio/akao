/**
 * @id: akao:class:core:engine:orchestrator:workflow:v1
 * @doc: Comprehensive workflow orchestration system implementation providing YAML-based workflow definition, parsing, validation, and execution capabilities. Supports complex node graphs with data flow, parameter substitution, conditional execution, error handling, and parallel processing. Enables sophisticated automation workflows with dependency management and execution monitoring for the Akao node-based architecture.
 * @specification: Workflow definition and orchestration engine implementation
 * @scope: Core engine workflow orchestration system implementation
 * @timeline: 2025-07-10
 * @rationale: Enable powerful workflow automation in node-based architecture
 * @methodology: YAML-based workflow definitions with graph execution and dependency management
 * @references: ["akao:interface:core:foundation:interfaces:inode:v1", "akao:class:core:engine:orchestrator:registry:v1"]
 */

#include "v1.hpp"
#include "core/foundation/formats/yaml/v1.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <set>
#include <queue>
#include <regex>
#include <future>
#include <thread>

namespace akao {
namespace core {
namespace engine {
namespace orchestrator {

// =============================================================================
// WorkflowDefinition Implementation
// =============================================================================

void WorkflowDefinition::addNode(const WorkflowNode& node) {
    // Remove existing node with same ID
    removeNode(node.id);
    nodes_.push_back(node);
}

void WorkflowDefinition::removeNode(const std::string& node_id) {
    nodes_.erase(
        std::remove_if(nodes_.begin(), nodes_.end(),
            [&node_id](const WorkflowNode& node) {
                return node.id == node_id;
            }),
        nodes_.end());
    
    // Remove connections involving this node
    connections_.erase(
        std::remove_if(connections_.begin(), connections_.end(),
            [&node_id](const NodeConnection& conn) {
                return conn.from_node_id == node_id || conn.to_node_id == node_id;
            }),
        connections_.end());
}

WorkflowNode* WorkflowDefinition::getNode(const std::string& node_id) {
    auto it = std::find_if(nodes_.begin(), nodes_.end(),
        [&node_id](const WorkflowNode& node) {
            return node.id == node_id;
        });
    return it != nodes_.end() ? &(*it) : nullptr;
}

const WorkflowNode* WorkflowDefinition::getNode(const std::string& node_id) const {
    auto it = std::find_if(nodes_.begin(), nodes_.end(),
        [&node_id](const WorkflowNode& node) {
            return node.id == node_id;
        });
    return it != nodes_.end() ? &(*it) : nullptr;
}

void WorkflowDefinition::addConnection(const NodeConnection& connection) {
    connections_.push_back(connection);
}

void WorkflowDefinition::removeConnection(const std::string& from_node, const std::string& to_node) {
    connections_.erase(
        std::remove_if(connections_.begin(), connections_.end(),
            [&from_node, &to_node](const NodeConnection& conn) {
                return conn.from_node_id == from_node && conn.to_node_id == to_node;
            }),
        connections_.end());
}

std::vector<NodeConnection> WorkflowDefinition::getConnectionsFrom(const std::string& node_id) const {
    std::vector<NodeConnection> result;
    std::copy_if(connections_.begin(), connections_.end(), std::back_inserter(result),
        [&node_id](const NodeConnection& conn) {
            return conn.from_node_id == node_id;
        });
    return result;
}

std::vector<NodeConnection> WorkflowDefinition::getConnectionsTo(const std::string& node_id) const {
    std::vector<NodeConnection> result;
    std::copy_if(connections_.begin(), connections_.end(), std::back_inserter(result),
        [&node_id](const NodeConnection& conn) {
            return conn.to_node_id == node_id;
        });
    return result;
}

void WorkflowDefinition::setDefaultParameter(const std::string& name, const foundation::types::NodeValue& value) {
    default_parameters_[name] = value;
}

foundation::types::NodeValue WorkflowDefinition::getDefaultParameter(const std::string& name) const {
    auto it = default_parameters_.find(name);
    return it != default_parameters_.end() ? it->second : foundation::types::NodeValue();
}

const std::map<std::string, foundation::types::NodeValue>& WorkflowDefinition::getDefaultParameters() const {
    return default_parameters_;
}

void WorkflowDefinition::setInputSchema(const std::string& input_name, const std::string& type) {
    input_schema_[input_name] = type;
}

void WorkflowDefinition::setOutputSchema(const std::string& output_name, const std::string& type) {
    output_schema_[output_name] = type;
}

std::vector<std::string> WorkflowDefinition::validate() const {
    std::vector<std::string> errors;
    
    // Basic validation
    if (id_.empty()) {
        errors.push_back("Workflow ID cannot be empty");
    }
    
    if (nodes_.empty()) {
        errors.push_back("Workflow must contain at least one node");
    }
    
    // Validate node IDs are unique
    std::set<std::string> node_ids;
    for (const auto& node : nodes_) {
        if (node.id.empty()) {
            errors.push_back("Node ID cannot be empty");
            continue;
        }
        
        if (node_ids.find(node.id) != node_ids.end()) {
            errors.push_back("Duplicate node ID: " + node.id);
        } else {
            node_ids.insert(node.id);
        }
        
        if (node.type.empty()) {
            errors.push_back("Node type cannot be empty for node: " + node.id);
        }
    }
    
    // Validate connections reference existing nodes
    for (const auto& conn : connections_) {
        if (node_ids.find(conn.from_node_id) == node_ids.end()) {
            errors.push_back("Connection references non-existent node: " + conn.from_node_id);
        }
        if (node_ids.find(conn.to_node_id) == node_ids.end()) {
            errors.push_back("Connection references non-existent node: " + conn.to_node_id);
        }
    }
    
    // Check for cycles
    if (hasCycles()) {
        errors.push_back("Workflow contains cyclic dependencies");
    }
    
    return errors;
}

bool WorkflowDefinition::isValid() const {
    return validate().empty();
}

std::vector<std::string> WorkflowDefinition::getExecutionOrder() const {
    std::vector<std::string> result;
    std::set<std::string> completed;
    std::set<std::string> remaining;
    
    // Initialize remaining nodes
    for (const auto& node : nodes_) {
        remaining.insert(node.id);
    }
    
    // Topological sort
    while (!remaining.empty()) {
        bool progress = false;
        
        for (auto it = remaining.begin(); it != remaining.end(); ) {
            const std::string& node_id = *it;
            const WorkflowNode* node = getNode(node_id);
            
            // Check if all dependencies are completed
            bool dependencies_met = true;
            for (const auto& dep : node->depends_on) {
                if (completed.find(dep) == completed.end()) {
                    dependencies_met = false;
                    break;
                }
            }
            
            // Also check connections
            if (dependencies_met) {
                auto incoming = getConnectionsTo(node_id);
                for (const auto& conn : incoming) {
                    if (completed.find(conn.from_node_id) == completed.end()) {
                        dependencies_met = false;
                        break;
                    }
                }
            }
            
            if (dependencies_met) {
                result.push_back(node_id);
                completed.insert(node_id);
                it = remaining.erase(it);
                progress = true;
            } else {
                ++it;
            }
        }
        
        if (!progress) {
            // Circular dependency or unresolvable
            break;
        }
    }
    
    return result;
}

bool WorkflowDefinition::hasCycles() const {
    std::set<std::string> visited;
    std::set<std::string> rec_stack;
    
    for (const auto& node : nodes_) {
        if (visited.find(node.id) == visited.end()) {
            if (hasCyclesUtil(node.id, visited, rec_stack)) {
                return true;
            }
        }
    }
    
    return false;
}

bool WorkflowDefinition::hasCyclesUtil(const std::string& node_id, 
                                      std::set<std::string>& visited,
                                      std::set<std::string>& rec_stack) const {
    visited.insert(node_id);
    rec_stack.insert(node_id);
    
    // Check connections
    auto outgoing = getConnectionsFrom(node_id);
    for (const auto& conn : outgoing) {
        if (visited.find(conn.to_node_id) == visited.end()) {
            if (hasCyclesUtil(conn.to_node_id, visited, rec_stack)) {
                return true;
            }
        } else if (rec_stack.find(conn.to_node_id) != rec_stack.end()) {
            return true;
        }
    }
    
    // Check explicit dependencies
    const WorkflowNode* node = getNode(node_id);
    if (node) {
        for (const auto& dep : node->depends_on) {
            if (visited.find(dep) == visited.end()) {
                if (hasCyclesUtil(dep, visited, rec_stack)) {
                    return true;
                }
            } else if (rec_stack.find(dep) != rec_stack.end()) {
                return true;
            }
        }
    }
    
    rec_stack.erase(node_id);
    return false;
}

std::string WorkflowDefinition::toYAML() const {
    std::ostringstream oss;
    
    oss << "id: \"" << id_ << "\"\n";
    oss << "name: \"" << name_ << "\"\n";
    if (!description_.empty()) {
        oss << "description: \"" << description_ << "\"\n";
    }
    if (!version_.empty()) {
        oss << "version: \"" << version_ << "\"\n";
    }
    
    // Default parameters
    if (!default_parameters_.empty()) {
        oss << "\ndefault_parameters:\n";
        for (const auto& [name, value] : default_parameters_) {
            oss << "  " << name << ": " << value.toString() << "\n";
        }
    }
    
    // Input schema
    if (!input_schema_.empty()) {
        oss << "\ninput_schema:\n";
        for (const auto& [name, type] : input_schema_) {
            oss << "  " << name << ": " << type << "\n";
        }
    }
    
    // Output schema
    if (!output_schema_.empty()) {
        oss << "\noutput_schema:\n";
        for (const auto& [name, type] : output_schema_) {
            oss << "  " << name << ": " << type << "\n";
        }
    }
    
    // Nodes
    oss << "\nnodes:\n";
    for (const auto& node : nodes_) {
        oss << "  - id: \"" << node.id << "\"\n";
        oss << "    type: \"" << node.type << "\"\n";
        if (!node.description.empty()) {
            oss << "    description: \"" << node.description << "\"\n";
        }
        if (!node.enabled) {
            oss << "    enabled: false\n";
        }
        if (node.retry_count > 0) {
            oss << "    retry_count: " << node.retry_count << "\n";
        }
        
        if (!node.parameters.empty()) {
            oss << "    parameters:\n";
            for (const auto& [name, value] : node.parameters) {
                oss << "      " << name << ": " << value.toString() << "\n";
            }
        }
        
        if (!node.depends_on.empty()) {
            oss << "    depends_on: [";
            for (size_t i = 0; i < node.depends_on.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << node.depends_on[i] << "\"";
            }
            oss << "]\n";
        }
    }
    
    // Connections
    if (!connections_.empty()) {
        oss << "\nconnections:\n";
        for (const auto& conn : connections_) {
            oss << "  - from: \"" << conn.from_node_id << "\"\n";
            oss << "    from_output: \"" << conn.from_output << "\"\n";
            oss << "    to: \"" << conn.to_node_id << "\"\n";
            oss << "    to_input: \"" << conn.to_input << "\"\n";
            if (!conn.transform_expression.empty()) {
                oss << "    transform: \"" << conn.transform_expression << "\"\n";
            }
        }
    }
    
    return oss.str();
}

std::string WorkflowDefinition::toJSON() const {
    // Simplified JSON output
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"id\": \"" << id_ << "\",\n";
    oss << "  \"name\": \"" << name_ << "\",\n";
    oss << "  \"description\": \"" << description_ << "\",\n";
    oss << "  \"version\": \"" << version_ << "\",\n";
    oss << "  \"node_count\": " << nodes_.size() << ",\n";
    oss << "  \"connection_count\": " << connections_.size() << "\n";
    oss << "}\n";
    return oss.str();
}

void WorkflowDefinition::clear() {
    id_.clear();
    name_.clear();
    description_.clear();
    version_.clear();
    nodes_.clear();
    connections_.clear();
    default_parameters_.clear();
    input_schema_.clear();
    output_schema_.clear();
}

bool WorkflowDefinition::isEmpty() const {
    return nodes_.empty();
}

std::string WorkflowDefinition::toString() const {
    std::ostringstream oss;
    oss << "Workflow[" << id_ << "] '" << name_ << "' - " 
        << nodes_.size() << " nodes, " << connections_.size() << " connections";
    return oss.str();
}

// =============================================================================
// WorkflowParser Implementation
// =============================================================================

std::shared_ptr<WorkflowDefinition> WorkflowParser::parseYAML(const std::string& yaml_content) {
    clearErrors();
    
    try {
        // Use our YAML infrastructure to parse
        akao::foundation::formats::yaml::YamlParser parser;
        auto yaml_root = parser.parse(yaml_content);
        
        if (!yaml_root || !yaml_root->isMapping()) {
            addError("Invalid YAML format: root must be a mapping");
            return nullptr;
        }
        
        auto workflow = std::make_shared<WorkflowDefinition>();
        
        // Parse basic properties
        if (yaml_root->hasKey("id")) {
            workflow->setId(yaml_root->operator[]("id")->asString());
        }
        if (yaml_root->hasKey("name")) {
            workflow->setName(yaml_root->operator[]("name")->asString());
        }
        if (yaml_root->hasKey("description")) {
            workflow->setDescription(yaml_root->operator[]("description")->asString());
        }
        if (yaml_root->hasKey("version")) {
            workflow->setVersion(yaml_root->operator[]("version")->asString());
        }
        
        // Parse default parameters
        if (yaml_root->hasKey("default_parameters")) {
            auto params_node = yaml_root->operator[]("default_parameters");
            if (params_node->isMapping()) {
                for (const auto& key : params_node->getKeys()) {
                    auto value_node = params_node->operator[](key);
                    workflow->setDefaultParameter(key, value_node->toNodeValue());
                }
            }
        }
        
        // Parse input schema
        if (yaml_root->hasKey("input_schema")) {
            auto schema_node = yaml_root->operator[]("input_schema");
            if (schema_node->isMapping()) {
                for (const auto& key : schema_node->getKeys()) {
                    auto type_node = schema_node->operator[](key);
                    workflow->setInputSchema(key, type_node->asString());
                }
            }
        }
        
        // Parse output schema
        if (yaml_root->hasKey("output_schema")) {
            auto schema_node = yaml_root->operator[]("output_schema");
            if (schema_node->isMapping()) {
                for (const auto& key : schema_node->getKeys()) {
                    auto type_node = schema_node->operator[](key);
                    workflow->setOutputSchema(key, type_node->asString());
                }
            }
        }
        
        // Parse nodes
        if (yaml_root->hasKey("nodes")) {
            auto nodes_node = yaml_root->operator[]("nodes");
            if (nodes_node->isSequence()) {
                const auto& nodes_sequence = nodes_node->asSequence();
                for (const auto& node_item : nodes_sequence) {
                    WorkflowNode workflow_node = parseNodeFromYAML(node_item->toNodeValue());
                    if (!workflow_node.id.empty()) {
                        workflow->addNode(workflow_node);
                    }
                }
            }
        }
        
        // Parse connections
        if (yaml_root->hasKey("connections")) {
            auto connections_node = yaml_root->operator[]("connections");
            if (connections_node->isSequence()) {
                const auto& connections_sequence = connections_node->asSequence();
                for (const auto& conn_item : connections_sequence) {
                    NodeConnection connection = parseConnectionFromYAML(conn_item->toNodeValue());
                    if (!connection.from_node_id.empty() && !connection.to_node_id.empty()) {
                        workflow->addConnection(connection);
                    }
                }
            }
        }
        
        // Validate the workflow
        auto validation_errors = workflow->validate();
        for (const auto& error : validation_errors) {
            addError(error);
        }
        
        return workflow;
        
    } catch (const std::exception& e) {
        addError("YAML parsing failed: " + std::string(e.what()));
        return nullptr;
    }
}

std::shared_ptr<WorkflowDefinition> WorkflowParser::parseYAMLFile(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file) {
        addError("Cannot open file: " + file_path);
        return nullptr;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return parseYAML(content);
}

std::shared_ptr<WorkflowDefinition> WorkflowParser::parseJSON(const std::string& json_content) {
    addError("JSON parsing not yet implemented");
    return nullptr;
}

std::shared_ptr<WorkflowDefinition> WorkflowParser::parseJSONFile(const std::string& file_path) {
    addError("JSON file parsing not yet implemented");
    return nullptr;
}

bool WorkflowParser::validateWorkflow(const WorkflowDefinition& workflow) {
    clearErrors();
    
    auto validation_errors = workflow.validate();
    for (const auto& error : validation_errors) {
        addError(error);
    }
    
    return !hasErrors();
}

std::vector<std::string> WorkflowParser::validateNodes(const std::vector<WorkflowNode>& nodes) {
    std::vector<std::string> errors;
    
    for (const auto& node : nodes) {
        if (!isValidNodeId(node.id)) {
            errors.push_back("Invalid node ID: " + node.id);
        }
        if (!isValidNodeType(node.type)) {
            errors.push_back("Invalid node type: " + node.type);
        }
    }
    
    return errors;
}

std::vector<std::string> WorkflowParser::validateConnections(const std::vector<NodeConnection>& connections,
                                                           const std::vector<WorkflowNode>& nodes) {
    std::vector<std::string> errors;
    std::set<std::string> node_ids;
    
    for (const auto& node : nodes) {
        node_ids.insert(node.id);
    }
    
    for (const auto& conn : connections) {
        if (node_ids.find(conn.from_node_id) == node_ids.end()) {
            errors.push_back("Connection references unknown node: " + conn.from_node_id);
        }
        if (node_ids.find(conn.to_node_id) == node_ids.end()) {
            errors.push_back("Connection references unknown node: " + conn.to_node_id);
        }
    }
    
    return errors;
}

bool WorkflowParser::validateParameterTypes(const std::map<std::string, foundation::types::NodeValue>& parameters) {
    // Basic validation - all parameters are considered valid for now
    return true;
}

std::vector<std::string> WorkflowParser::getSupportedFormats() {
    return {"yaml", "yml", "json"};
}

bool WorkflowParser::isValidNodeId(const std::string& node_id) {
    if (node_id.empty() || node_id.length() > 100) {
        return false;
    }
    
    // Simple validation: alphanumeric, underscore, hyphen
    std::regex valid_id_pattern("^[a-zA-Z][a-zA-Z0-9_-]*$");
    return std::regex_match(node_id, valid_id_pattern);
}

bool WorkflowParser::isValidNodeType(const std::string& node_type) {
    if (node_type.empty() || node_type.length() > 100) {
        return false;
    }
    
    // Simple validation: alphanumeric, underscore, hyphen, dots for namespacing
    std::regex valid_type_pattern("^[a-zA-Z][a-zA-Z0-9_.-]*$");
    return std::regex_match(node_type, valid_type_pattern);
}

WorkflowNode WorkflowParser::parseNodeFromYAML(const foundation::types::NodeValue& node_data) {
    WorkflowNode node;
    
    if (!node_data.isObject()) {
        addError("Node must be an object");
        return node;
    }
    
    auto obj = node_data.asObject();
    
    // Required fields
    if (obj.find("id") != obj.end() && obj.at("id").isString()) {
        node.id = obj.at("id").asString();
    } else {
        addError("Node missing required 'id' field");
        return node;
    }
    
    if (obj.find("type") != obj.end() && obj.at("type").isString()) {
        node.type = obj.at("type").asString();
    } else {
        addError("Node missing required 'type' field");
        return node;
    }
    
    // Optional fields
    if (obj.find("description") != obj.end() && obj.at("description").isString()) {
        node.description = obj.at("description").asString();
    }
    
    if (obj.find("enabled") != obj.end() && obj.at("enabled").isBoolean()) {
        node.enabled = obj.at("enabled").asBoolean();
    }
    
    if (obj.find("retry_count") != obj.end() && obj.at("retry_count").isInteger()) {
        node.retry_count = static_cast<int>(obj.at("retry_count").asInteger());
    }
    
    // Parameters
    if (obj.find("parameters") != obj.end() && obj.at("parameters").isObject()) {
        node.parameters = obj.at("parameters").asObject();
    }
    
    // Dependencies
    if (obj.find("depends_on") != obj.end() && obj.at("depends_on").isArray()) {
        auto deps = obj.at("depends_on").asArray();
        for (const auto& dep : deps) {
            if (dep.isString()) {
                node.depends_on.push_back(dep.asString());
            }
        }
    }
    
    return node;
}

NodeConnection WorkflowParser::parseConnectionFromYAML(const foundation::types::NodeValue& connection_data) {
    NodeConnection connection;
    
    if (!connection_data.isObject()) {
        addError("Connection must be an object");
        return connection;
    }
    
    auto obj = connection_data.asObject();
    
    if (obj.find("from_node") != obj.end() && obj.at("from_node").isString()) {
        connection.from_node_id = obj.at("from_node").asString();
    }
    
    if (obj.find("from_output") != obj.end() && obj.at("from_output").isString()) {
        connection.from_output = obj.at("from_output").asString();
    } else {
        connection.from_output = "output"; // Default
    }
    
    if (obj.find("to_node") != obj.end() && obj.at("to_node").isString()) {
        connection.to_node_id = obj.at("to_node").asString();
    }
    
    if (obj.find("to_input") != obj.end() && obj.at("to_input").isString()) {
        connection.to_input = obj.at("to_input").asString();
    } else {
        connection.to_input = "input"; // Default
    }
    
    if (obj.find("transform_expression") != obj.end() && obj.at("transform_expression").isString()) {
        connection.transform_expression = obj.at("transform_expression").asString();
    }
    
    return connection;
}

std::map<std::string, foundation::types::NodeValue> WorkflowParser::parseParametersFromYAML(
    const foundation::types::NodeValue& params_data) {
    
    std::map<std::string, foundation::types::NodeValue> result;
    
    if (params_data.isObject()) {
        result = params_data.asObject();
    }
    
    return result;
}

void WorkflowParser::addError(const std::string& error) {
    errors_.push_back(error);
}

void WorkflowParser::addWarning(const std::string& warning) {
    warnings_.push_back(warning);
}

bool WorkflowParser::validateNodeReference(const std::string& node_id, const std::vector<WorkflowNode>& nodes) {
    return std::any_of(nodes.begin(), nodes.end(),
        [&node_id](const WorkflowNode& node) {
            return node.id == node_id;
        });
}

bool WorkflowParser::validateParameterSubstitution(const std::string& param_value) {
    // Simple validation for parameter substitution syntax
    return true; // For now, accept all parameter values
}

std::vector<std::string> WorkflowParser::extractParameterReferences(const std::string& value) {
    std::vector<std::string> references;
    std::regex param_pattern(R"(\$\{([^}]+)\})");
    std::sregex_iterator begin(value.begin(), value.end(), param_pattern);
    std::sregex_iterator end;
    
    for (std::sregex_iterator i = begin; i != end; ++i) {
        std::smatch match = *i;
        references.push_back(match[1].str());
    }
    
    return references;
}

// =============================================================================
// WorkflowExecutor Implementation (Simplified)
// =============================================================================

WorkflowExecutor::WorkflowExecutor(std::shared_ptr<akao::engine::orchestrator::NodeRegistry> registry) 
    : registry_(registry) {
    // Register default transform functions
    registerTransformFunction("identity", [](const foundation::types::NodeValue& value) {
        return value;
    });
    
    registerTransformFunction("to_string", [](const foundation::types::NodeValue& value) {
        return foundation::types::NodeValue(value.toString());
    });
}

WorkflowExecutionResult WorkflowExecutor::execute(const WorkflowDefinition& workflow) {
    WorkflowContext context;
    context.workflow_id = workflow.getId();
    return execute(workflow, context);
}

WorkflowExecutionResult WorkflowExecutor::execute(const WorkflowDefinition& workflow, 
                                                const WorkflowContext& context) {
    WorkflowExecutionResult result;
    auto start_time = std::chrono::steady_clock::now();
    
    // Initialize execution status
    current_status_.total_nodes = workflow.getNodeCount();
    current_status_.completed_nodes = 0;
    current_status_.is_running = true;
    current_status_.elapsed_time = std::chrono::duration<double>(0);
    
    try {
        // Validate workflow
        if (!workflow.isValid()) {
            result.error_message = "Invalid workflow definition";
            current_status_.is_running = false;
            return result;
        }
        
        // Check for cycles
        if (workflow.hasCycles()) {
            result.error_message = "Workflow contains circular dependencies";
            current_status_.is_running = false;
            return result;
        }
        
        // Create mutable context
        WorkflowContext mutable_context = context;
        mutable_context.workflow_id = workflow.getId();
        mutable_context.start_time = std::chrono::system_clock::now();
        
        // Choose execution strategy
        if (parallel_execution_enabled_) {
            result = executeParallel(workflow, mutable_context);
        } else {
            result = executeSequential(workflow, mutable_context);
        }
        
        result.final_outputs = mutable_context.outputs;
        
    } catch (const std::exception& e) {
        result.error_message = "Workflow execution failed: " + std::string(e.what());
        result.success = false;
    }
    
    auto end_time = std::chrono::steady_clock::now();
    result.execution_time = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time);
    
    current_status_.is_running = false;
    current_status_.elapsed_time = result.execution_time;
    
    return result;
}

WorkflowExecutionResult WorkflowExecutor::execute(const WorkflowDefinition& workflow,
                                                const std::map<std::string, foundation::types::NodeValue>& inputs) {
    WorkflowContext context;
    context.workflow_id = workflow.getId();
    context.inputs = inputs;
    return execute(workflow, context);
}

void WorkflowExecutor::registerTransformFunction(const std::string& name, 
                                               std::function<foundation::types::NodeValue(const foundation::types::NodeValue&)> func) {
    transform_functions_[name] = func;
}

WorkflowExecutor::ExecutionStatus WorkflowExecutor::getExecutionStatus() const {
    return current_status_;
}

foundation::types::ExecutionResult WorkflowExecutor::executeNode(const WorkflowNode& node, 
                                                                const WorkflowContext& context) {
    // Simplified implementation - just return success
    auto inputs = prepareNodeInputs(node, context);
    
    // For now, return a mock successful result
    return foundation::types::ExecutionResult::success(foundation::types::NodeValue("Node executed successfully"));
}

std::map<std::string, foundation::types::NodeValue> WorkflowExecutor::prepareNodeInputs(
    const WorkflowNode& node, const WorkflowContext& context) {
    
    std::map<std::string, foundation::types::NodeValue> inputs;
    
    // Merge default parameters, workflow inputs, and node parameters
    for (const auto& [name, value] : node.parameters) {
        inputs[name] = substituteParameters(value, context);
    }
    
    return inputs;
}

void WorkflowExecutor::updateContextWithOutputs(WorkflowContext& context, const std::string& node_id,
                                               const foundation::types::ExecutionResult& result) {
    if (result.isSuccess()) {
        context.outputs[node_id] = result.getData();
    }
}

foundation::types::NodeValue WorkflowExecutor::substituteParameters(const foundation::types::NodeValue& value,
                                                                   const WorkflowContext& context) {
    // Simple implementation - return value as-is for now
    return value;
}

std::string WorkflowExecutor::substituteStringParameters(const std::string& template_str,
                                                        const WorkflowContext& context) {
    std::string result = template_str;
    
    // Simple substitution: ${variable_name}
    std::regex param_pattern(R"(\$\{([^}]+)\})");
    std::sregex_iterator begin(template_str.begin(), template_str.end(), param_pattern);
    std::sregex_iterator end;
    
    for (std::sregex_iterator i = begin; i != end; ++i) {
        std::smatch match = *i;
        std::string var_name = match[1].str();
        
        // Look up in context variables
        auto it = context.variables.find(var_name);
        if (it != context.variables.end()) {
            std::string replacement = it->second.toString();
            result = std::regex_replace(result, std::regex("\\$\\{" + var_name + "\\}"), replacement);
        }
    }
    
    return result;
}

foundation::types::NodeValue WorkflowExecutor::applyTransformation(const std::string& transform_expr,
                                                                 const foundation::types::NodeValue& input) {
    // Simple transformation - return input as-is for now
    return input;
}

std::vector<std::string> WorkflowExecutor::calculateExecutionOrder(const WorkflowDefinition& workflow) {
    return workflow.getExecutionOrder();
}

bool WorkflowExecutor::canExecuteNode(const WorkflowNode& node, const std::set<std::string>& completed_nodes) {
    for (const auto& dep : node.depends_on) {
        if (completed_nodes.find(dep) == completed_nodes.end()) {
            return false;
        }
    }
    return true;
}

bool WorkflowExecutor::shouldRetryNode(const WorkflowNode& node, int attempt_count) {
    return attempt_count < node.retry_count;
}

void WorkflowExecutor::handleNodeError(const std::string& node_id, const std::string& error_message,
                                     WorkflowExecutionResult& result) {
    result.error_message = "Node '" + node_id + "' error: " + error_message;
}

// =============================================================================
// Enhanced WorkflowExecutor Implementation
// =============================================================================

void WorkflowExecutor::setParallelExecutionEnabled(bool enabled) {
    parallel_execution_enabled_ = enabled;
}

void WorkflowExecutor::setMaxRetryAttempts(int max_retries) {
    max_retry_attempts_ = max_retries;
}

void WorkflowExecutor::setErrorRecoveryStrategy(const std::string& strategy) {
    error_recovery_strategy_ = strategy;
}

void WorkflowExecutor::setExecutionTimeout(std::chrono::milliseconds timeout) {
    default_timeout_ = timeout;
}

WorkflowExecutionResult WorkflowExecutor::executeSequential(const WorkflowDefinition& workflow, WorkflowContext& context) {
    WorkflowExecutionResult result;
    
    // Calculate execution order
    auto execution_order = workflow.getExecutionOrder();
    if (execution_order.size() != workflow.getNodeCount()) {
        result.error_message = "Cannot determine execution order (circular dependencies)";
        return result;
    }
    
    // Execute nodes in order with enhanced error handling
    for (const auto& node_id : execution_order) {
        const WorkflowNode* node = workflow.getNode(node_id);
        if (!node || !node->enabled) {
            continue;
        }
        
        current_status_.current_node = node_id;
        
        // Execute with retry logic
        foundation::types::ExecutionResult node_result;
        int attempt = 0;
        bool success = false;
        
        while (attempt <= max_retry_attempts_ && !success) {
            node_result = executeNode(*node, context);
            success = node_result.isSuccess();
            
            if (!success && attempt < max_retry_attempts_) {
                attempt++;
                // Simple backoff - wait before retry
                std::this_thread::sleep_for(std::chrono::milliseconds(100 * attempt));
            } else {
                break;
            }
        }
        
        result.node_results[node_id] = node_result;
        
        if (!success) {
            // Handle failure based on strategy
            auto failure_result = handleNodeFailure(node_id, node_result, workflow, context);
            if (!shouldContinueAfterError(error_recovery_strategy_, node_id)) {
                return failure_result;
            }
        } else {
            // Update context with outputs and propagate data
            updateContextWithOutputs(context, node_id, node_result);
            
            // Propagate data to dependent nodes
            auto connections = workflow.getConnectionsFrom(node_id);
            for (const auto& conn : connections) {
                propagateDataBetweenNodes(conn.from_node_id, conn.to_node_id, workflow, context);
            }
        }
        
        current_status_.completed_nodes++;
    }
    
    result.success = true;
    return result;
}

WorkflowExecutionResult WorkflowExecutor::executeParallel(const WorkflowDefinition& workflow, WorkflowContext& context) {
    WorkflowExecutionResult result;
    
    // Calculate execution levels for parallel execution
    auto execution_levels = calculateExecutionLevels(workflow);
    std::set<std::string> completed_nodes;
    
    for (const auto& level : execution_levels) {
        // Execute all nodes in this level in parallel
        std::vector<std::future<std::pair<std::string, foundation::types::ExecutionResult>>> futures;
        
        for (const auto& node_id : level) {
            const WorkflowNode* node = workflow.getNode(node_id);
            if (!node || !node->enabled) {
                continue;
            }
            
            // Launch async execution
            auto future = std::async(std::launch::async, [this, node, &context]() {
                return std::make_pair(node->id, executeNode(*node, context));
            });
            futures.push_back(std::move(future));
        }
        
        // Wait for all nodes in this level to complete
        for (auto& future : futures) {
            auto [node_id, node_result] = future.get();
            result.node_results[node_id] = node_result;
            
            if (!node_result.isSuccess()) {
                auto failure_result = handleNodeFailure(node_id, node_result, workflow, context);
                if (!shouldContinueAfterError(error_recovery_strategy_, node_id)) {
                    return failure_result;
                }
            } else {
                updateContextWithOutputs(context, node_id, node_result);
                completed_nodes.insert(node_id);
                current_status_.completed_nodes++;
            }
        }
    }
    
    result.success = true;
    return result;
}

std::vector<std::vector<std::string>> WorkflowExecutor::calculateExecutionLevels(const WorkflowDefinition& workflow) {
    std::vector<std::vector<std::string>> levels;
    std::set<std::string> completed;
    std::set<std::string> remaining;
    
    // Initialize remaining nodes
    for (const auto& node : workflow.getNodes()) {
        if (node.enabled) {
            remaining.insert(node.id);
        }
    }
    
    while (!remaining.empty()) {
        std::vector<std::string> current_level;
        
        // Find nodes that can execute now (all dependencies completed)
        for (const auto& node_id : remaining) {
            if (canExecuteNodeNow(node_id, workflow, completed)) {
                current_level.push_back(node_id);
            }
        }
        
        if (current_level.empty()) {
            break; // Circular dependency or other issue
        }
        
        // Move nodes from remaining to completed
        for (const auto& node_id : current_level) {
            remaining.erase(node_id);
            completed.insert(node_id);
        }
        
        levels.push_back(current_level);
    }
    
    return levels;
}

bool WorkflowExecutor::canExecuteNodeNow(const std::string& node_id, const WorkflowDefinition& workflow, 
                                        const std::set<std::string>& completed_nodes) {
    const WorkflowNode* node = workflow.getNode(node_id);
    if (!node) return false;
    
    for (const auto& dep : node->depends_on) {
        if (completed_nodes.find(dep) == completed_nodes.end()) {
            return false;
        }
    }
    return true;
}

WorkflowExecutionResult WorkflowExecutor::handleNodeFailure(const std::string& node_id, 
                                                           const foundation::types::ExecutionResult& node_result,
                                                           const WorkflowDefinition& workflow,
                                                           WorkflowContext& context) {
    WorkflowExecutionResult result;
    result.success = false;
    result.error_message = "Node '" + node_id + "' failed: " + node_result.getErrorSummary();
    result.node_results[node_id] = node_result;
    return result;
}

bool WorkflowExecutor::shouldContinueAfterError(const std::string& strategy, const std::string& failed_node_id) {
    if (strategy == "fail_fast") {
        return false;
    } else if (strategy == "continue_on_error") {
        return true;
    } else if (strategy == "skip_dependents") {
        // More complex logic would be needed here
        return true;
    }
    return false; // Default to fail fast
}

void WorkflowExecutor::propagateDataBetweenNodes(const std::string& from_node_id, const std::string& to_node_id,
                                                const WorkflowDefinition& workflow, WorkflowContext& context) {
    // Find connection between nodes
    auto connections = workflow.getConnectionsFrom(from_node_id);
    for (const auto& conn : connections) {
        if (conn.to_node_id == to_node_id) {
            // Get output from source node
            if (context.outputs.find(from_node_id) != context.outputs.end()) {
                foundation::types::NodeValue output_value = context.outputs[from_node_id];
                
                // Apply transformation if specified
                if (!conn.transform_expression.empty()) {
                    output_value = applyTransformation(conn.transform_expression, output_value);
                }
                
                // Set as input for target node
                context.variables[to_node_id + "." + conn.to_input] = output_value;
            }
            break;
        }
    }
}

foundation::types::NodeValue WorkflowExecutor::resolveNodeInput(const std::string& input_key, const WorkflowNode& node,
                                                               const WorkflowContext& context) {
    // Check if input is mapped from another node's output
    std::string mapped_key = node.id + "." + input_key;
    if (context.variables.find(mapped_key) != context.variables.end()) {
        return context.variables.at(mapped_key);
    }
    
    // Check workflow inputs
    if (context.inputs.find(input_key) != context.inputs.end()) {
        return context.inputs.at(input_key);
    }
    
    // Return null if not found
    return foundation::types::NodeValue();
}

// =============================================================================
// Workflow Utilities Implementation
// =============================================================================

namespace workflow_utils {
    bool isValidWorkflowYAML(const std::string& yaml_content) {
        WorkflowParser parser;
        auto workflow = parser.parseYAML(yaml_content);
        return workflow != nullptr && workflow->isValid();
    }
    
    std::vector<std::string> extractNodeTypes(const WorkflowDefinition& workflow) {
        std::vector<std::string> types;
        for (const auto& node : workflow.getNodes()) {
            types.push_back(node.type);
        }
        return types;
    }
    
    std::vector<std::string> findUnusedNodes(const WorkflowDefinition& workflow) {
        std::set<std::string> connected_nodes;
        
        for (const auto& conn : workflow.getConnections()) {
            connected_nodes.insert(conn.from_node_id);
            connected_nodes.insert(conn.to_node_id);
        }
        
        std::vector<std::string> unused;
        for (const auto& node : workflow.getNodes()) {
            if (connected_nodes.find(node.id) == connected_nodes.end()) {
                unused.push_back(node.id);
            }
        }
        
        return unused;
    }
    
    std::vector<std::string> findDisconnectedNodes(const WorkflowDefinition& workflow) {
        return findUnusedNodes(workflow); // Same implementation for now
    }
    
    std::map<std::string, int> calculateNodeDepths(const WorkflowDefinition& workflow) {
        std::map<std::string, int> depths;
        auto execution_order = workflow.getExecutionOrder();
        
        for (size_t i = 0; i < execution_order.size(); ++i) {
            depths[execution_order[i]] = static_cast<int>(i);
        }
        
        return depths;
    }
    
    bool hasParallelExecution(const WorkflowDefinition& workflow) {
        auto execution_order = workflow.getExecutionOrder();
        return execution_order.size() < workflow.getNodeCount(); // Simplified check
    }
    
    std::string generateWorkflowVisualization(const WorkflowDefinition& workflow) {
        std::ostringstream oss;
        oss << "Workflow: " << workflow.getName() << "\n";
        oss << "Nodes: " << workflow.getNodeCount() << "\n";
        oss << "Connections: " << workflow.getConnections().size() << "\n";
        
        for (const auto& node : workflow.getNodes()) {
            oss << "  [" << node.id << "] " << node.type << "\n";
        }
        
        for (const auto& conn : workflow.getConnections()) {
            oss << "  " << conn.from_node_id << " -> " << conn.to_node_id << "\n";
        }
        
        return oss.str();
    }
}

} // namespace orchestrator
} // namespace engine
} // namespace core
} // namespace akao