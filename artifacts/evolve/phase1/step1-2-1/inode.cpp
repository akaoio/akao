/**
 * @id: akao:file:core:node:interface:inode_impl:v1
 * @doc: Implementation of standard node interface system including validation results, node definitions, capabilities, examples, and registry helpers providing complete infrastructure for node-based workflow system with comprehensive validation and metadata support.
 * @specification: Complete implementation of INode interface system and supporting classes
 * @scope: Core node interface implementation
 * @timeline: 2025-07-10
 * @rationale: Provide robust implementation of node interface system with full validation
 * @methodology: Complete class implementations with validation, serialization, and utility functions
 * @references: ["akao:class:core:node:interface:inode:v1", "akao:class:core:node:interface:nodedefinition:v1"]
 */

#include "inode.hpp"
#include "node_definition.hpp"
#include <sstream>
#include <algorithm>
#include <regex>
#include <chrono>

namespace akao::node {

// ============================================================================
// ValidationResult Implementation
// ============================================================================

bool ValidationResult::hasWarnings() const {
    return std::any_of(errors_.begin(), errors_.end(),
                      [](const ValidationError& error) {
                          return error.severity == Severity::WARNING;
                      });
}

void ValidationResult::addError(const ValidationError& error) {
    errors_.push_back(error);
    if (error.severity == Severity::ERROR || error.severity == Severity::FATAL) {
        is_valid_ = false;
    }
}

void ValidationResult::addError(const std::string& param_name, const std::string& message,
                                Severity severity, const std::string& code) {
    addError(ValidationError(param_name, message, severity, code));
}

std::vector<ValidationResult::ValidationError> ValidationResult::getErrorsBySeverity(Severity severity) const {
    std::vector<ValidationError> filtered;
    std::copy_if(errors_.begin(), errors_.end(), std::back_inserter(filtered),
                [severity](const ValidationError& error) {
                    return error.severity == severity;
                });
    return filtered;
}

size_t ValidationResult::getWarningCount() const {
    return getErrorsBySeverity(Severity::WARNING).size();
}

size_t ValidationResult::getFatalErrorCount() const {
    return getErrorsBySeverity(Severity::FATAL).size();
}

std::string ValidationResult::toString() const {
    std::stringstream ss;
    ss << "ValidationResult{valid: " << (isValid() ? "true" : "false");
    ss << ", errors: " << errors_.size();
    
    if (!errors_.empty()) {
        ss << ", details: [";
        for (size_t i = 0; i < errors_.size(); ++i) {
            if (i > 0) ss << ", ";
            const auto& error = errors_[i];
            ss << "{param: \"" << error.parameter_name << "\", message: \"" << error.error_message << "\"}";
        }
        ss << "]";
    }
    
    ss << "}";
    return ss.str();
}

// ============================================================================
// INode Default Implementations
// ============================================================================

size_t INode::getEstimatedMemoryUsage(const NodeContext& context) const {
    // Basic estimation based on input data size
    size_t memory = 0;
    
    // Add input data size
    for (const auto& item : context.getInputItems()) {
        memory += item.getJson().toString().size();
        for (const auto& [key, binary] : item.getBinary()) {
            memory += binary.getSize();
        }
    }
    
    // Add parameter memory
    for (const auto& [key, value] : context.getAllParameters()) {
        memory += key.size() + value.toString().size();
    }
    
    // Add base overhead (estimated)
    memory += 1024; // 1KB base overhead
    
    return memory;
}

std::chrono::milliseconds INode::getEstimatedExecutionTime(const NodeContext& context) const {
    // Basic estimation based on input count and complexity
    size_t item_count = context.getInputCount();
    
    // Base time: 10ms + 1ms per item
    auto base_time = std::chrono::milliseconds(10 + item_count);
    
    // Adjust based on data complexity
    for (const auto& item : context.getInputItems()) {
        size_t data_size = item.getJson().toString().size();
        if (data_size > 10000) { // Large data adds time
            base_time += std::chrono::milliseconds(5);
        }
    }
    
    return base_time;
}

// ============================================================================
// NodeCapabilities Implementation
// ============================================================================

std::string NodeCapabilities::toString() const {
    std::stringstream ss;
    ss << "NodeCapabilities{";
    ss << "streaming: " << (supports_streaming ? "true" : "false");
    ss << ", batch: " << (supports_batch_processing ? "true" : "false");
    ss << ", parallel: " << (supports_parallel_execution ? "true" : "false");
    ss << ", max_memory: " << max_memory_mb << "MB";
    ss << ", max_time: " << max_execution_time_seconds << "s";
    ss << ", batch_size: " << recommended_batch_size;
    ss << "}";
    return ss.str();
}

// ============================================================================
// NodeDefinition Implementation
// ============================================================================

NodeDefinition::NodeDefinition(const std::string& node_id, const std::string& node_type, const std::string& version)
    : node_id_(node_id), node_type_(node_type), version_(version) {
    // Set defaults
    display_name_ = node_type;
    description_ = "Node of type " + node_type;
    author_ = "Akao Framework";
    license_ = "MIT";
    color_ = "#1f77b4"; // Default blue color
}

ValidationResult NodeDefinition::validate() const {
    ValidationResult result;
    
    // Validate basic required fields
    if (node_id_.empty()) {
        result.addError("node_id", "Node ID cannot be empty", ValidationResult::Severity::ERROR);
    } else if (!NodeRegistryHelpers::isValidNodeId(node_id_)) {
        result.addError("node_id", "Invalid node ID format: " + node_id_, ValidationResult::Severity::ERROR);
    }
    
    if (node_type_.empty()) {
        result.addError("node_type", "Node type cannot be empty", ValidationResult::Severity::ERROR);
    } else if (!NodeRegistryHelpers::isValidNodeType(node_type_)) {
        result.addError("node_type", "Invalid node type format: " + node_type_, ValidationResult::Severity::ERROR);
    }
    
    if (version_.empty()) {
        result.addError("version", "Version cannot be empty", ValidationResult::Severity::ERROR);
    } else if (!NodeRegistryHelpers::isValidVersion(version_)) {
        result.addError("version", "Invalid version format: " + version_, ValidationResult::Severity::ERROR);
    }
    
    if (display_name_.empty()) {
        result.addError("display_name", "Display name cannot be empty", ValidationResult::Severity::WARNING);
    }
    
    if (description_.empty()) {
        result.addError("description", "Description should not be empty", ValidationResult::Severity::WARNING);
    }
    
    // Validate parameters
    for (const auto& param : parameters_) {
        auto param_result = param.validate(param.getDefaultValue());
        if (!param_result.isValid()) {
            result.addError("parameter_" + param.getName(), 
                          "Parameter validation failed: " + param_result.toString(),
                          ValidationResult::Severity::ERROR);
        }
    }
    
    // Validate capabilities
    if (capabilities_.max_memory_mb == 0) {
        result.addError("capabilities", "Max memory should be greater than 0", ValidationResult::Severity::WARNING);
    }
    
    if (capabilities_.max_execution_time_seconds == 0) {
        result.addError("capabilities", "Max execution time should be greater than 0", ValidationResult::Severity::WARNING);
    }
    
    return result;
}

std::string NodeDefinition::toJson() const {
    // Basic JSON serialization (would use proper JSON library in production)
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"node_id\": \"" << node_id_ << "\",\n";
    ss << "  \"node_type\": \"" << node_type_ << "\",\n";
    ss << "  \"version\": \"" << version_ << "\",\n";
    ss << "  \"display_name\": \"" << display_name_ << "\",\n";
    ss << "  \"description\": \"" << description_ << "\",\n";
    ss << "  \"author\": \"" << author_ << "\",\n";
    ss << "  \"license\": \"" << license_ << "\",\n";
    ss << "  \"categories\": [";
    for (size_t i = 0; i < categories_.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << "\"" << categories_[i] << "\"";
    }
    ss << "],\n";
    ss << "  \"parameters\": [";
    for (size_t i = 0; i < parameters_.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << "{\"name\": \"" << parameters_[i].getName() << "\"}"; // Simplified
    }
    ss << "],\n";
    ss << "  \"capabilities\": " << capabilities_.toString() << "\n";
    ss << "}";
    return ss.str();
}

NodeDefinition NodeDefinition::fromJson(const std::string& /* json */) {
    // Placeholder implementation - would use proper JSON parser in production
    return NodeDefinition("unknown", "unknown", "1.0.0");
}

std::string NodeDefinition::toString() const {
    std::stringstream ss;
    ss << "NodeDefinition{";
    ss << "id: \"" << node_id_ << "\"";
    ss << ", type: \"" << node_type_ << "\"";
    ss << ", version: \"" << version_ << "\"";
    ss << ", display_name: \"" << display_name_ << "\"";
    ss << ", parameters: " << parameters_.size();
    ss << ", categories: " << categories_.size();
    ss << "}";
    return ss.str();
}

// ============================================================================
// NodeRegistryHelpers Implementation
// ============================================================================

namespace NodeRegistryHelpers {

bool isValidNodeId(const std::string& node_id) {
    // Node ID should follow pattern: category.name.version (e.g., "parser.yaml.v1")
    std::regex pattern(R"(^[a-z][a-z0-9_]*(\.[a-z][a-z0-9_]*)*\.v[0-9]+$)");
    return std::regex_match(node_id, pattern);
}

bool isValidNodeType(const std::string& node_type) {
    // Node type should be alphanumeric with dots and underscores, but no version suffix
    std::regex pattern(R"(^[a-z][a-z0-9_]*(\.[a-z][a-z0-9_]*)*$)");
    std::regex version_pattern(R"(\.v[0-9]+$)");
    return std::regex_match(node_type, pattern) && !std::regex_search(node_type, version_pattern);
}

bool isValidVersion(const std::string& version) {
    // Version should follow semantic versioning or simple v1, v2, etc.
    std::regex pattern(R"(^(v[0-9]+|[0-9]+\.[0-9]+\.[0-9]+)$)");
    return std::regex_match(version, pattern);
}

std::map<std::string, NodeFactory>& getNodeFactories() {
    static std::map<std::string, NodeFactory> factories;
    return factories;
}

void registerNodeFactory(const std::string& node_type, NodeFactory factory) {
    getNodeFactories()[node_type] = factory;
}

ValidationResult validateNodeDefinition(const NodeDefinition& definition) {
    return definition.validate();
}

ValidationResult validateNodeImplementation(const INode& node) {
    ValidationResult result;
    
    // Test basic interface compliance
    try {
        std::string id = node.getNodeId();
        std::string type = node.getNodeType();
        std::string version = node.getVersion();
        
        if (id.empty()) {
            result.addError("implementation", "Node ID is empty", ValidationResult::Severity::ERROR);
        }
        
        if (type.empty()) {
            result.addError("implementation", "Node type is empty", ValidationResult::Severity::ERROR);
        }
        
        if (version.empty()) {
            result.addError("implementation", "Version is empty", ValidationResult::Severity::ERROR);
        }
        
        // Test definition retrieval
        NodeDefinition def = node.getDefinition();
        auto def_result = def.validate();
        if (!def_result.isValid()) {
            result.addError("definition", "Node definition is invalid: " + def_result.toString(),
                          ValidationResult::Severity::ERROR);
        }
        
        // Test clone functionality
        auto cloned = node.clone();
        if (!cloned) {
            result.addError("implementation", "Clone method returned null", ValidationResult::Severity::ERROR);
        } else if (cloned->getNodeId() != id) {
            result.addError("implementation", "Cloned node has different ID", ValidationResult::Severity::ERROR);
        }
        
    } catch (const std::exception& e) {
        result.addError("implementation", "Exception during validation: " + std::string(e.what()),
                      ValidationResult::Severity::FATAL);
    }
    
    return result;
}

std::vector<std::string> discoverAvailableNodeTypes() {
    std::vector<std::string> types;
    for (const auto& [type, factory] : getNodeFactories()) {
        types.push_back(type);
    }
    return types;
}

std::vector<NodeDefinition> loadNodeDefinitions(const std::string& /* directory */) {
    // Placeholder - would scan directory for node definition files
    std::vector<NodeDefinition> definitions;
    
    // Return some built-in definitions for testing
    NodeDefinition logger("system.logger.v1", "system.logger", "v1");
    logger.setDisplayName("Logger Node");
    logger.setDescription("Logs messages and data for debugging and monitoring");
    logger.addCategory("system");
    logger.addCategory("logging");
    definitions.push_back(logger);
    
    NodeDefinition mapper("data.mapper.v1", "data.mapper", "v1");
    mapper.setDisplayName("Data Mapper");
    mapper.setDescription("Maps and transforms data between different formats");
    mapper.addCategory("data");
    mapper.addCategory("transformation");
    definitions.push_back(mapper);
    
    return definitions;
}

} // namespace NodeRegistryHelpers

} // namespace akao::node