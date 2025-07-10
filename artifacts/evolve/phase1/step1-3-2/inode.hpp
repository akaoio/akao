/**
 * @id akao.evolve.inode.v1
 * @doc Standard node interface defining the contract for all node implementations including execution, validation, and metadata access. Provides the foundation for the node-based architecture with comprehensive execution context support.
 * @specification Abstract interface defining node identity, execution methods, parameter validation, and definition access with full integration to NodeContext and ExecutionResult systems.
 * @scope artifacts/evolve/phase1/step1-3-2
 * @timeline 2025-07-10
 * @rationale Establish consistent interface for all node types enabling polymorphic execution, validation, and management within the node registry system.
 * @methodology Abstract base class with pure virtual methods for core functionality and integration with existing parameter and registry systems.
 * @references akao.evolve.node_registry.v1, akao.evolve.node_parameter.v1, akao.evolve.node_definition.v1
 */

#ifndef AKAO_EVOLVE_INODE_V1_HPP
#define AKAO_EVOLVE_INODE_V1_HPP

#include "../step1-3-1/node_definition.hpp"
#include "../step1-2-2/node_parameter.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <sstream>
#include <iomanip>

namespace akao {
namespace evolve {

/**
 * @brief Binary data attachment for node items
 */
struct BinaryData {
    std::vector<uint8_t> data;
    std::string mime_type;
    std::string filename;
    
    BinaryData() = default;
    BinaryData(const std::vector<uint8_t>& d, const std::string& mime = "application/octet-stream", const std::string& fname = "")
        : data(d), mime_type(mime), filename(fname) {}
    
    size_t size() const { return data.size(); }
    bool empty() const { return data.empty(); }
};

/**
 * @brief Execution error information
 */
struct ExecutionError {
    std::string message;
    std::string error_code;
    std::string stack_trace;
    std::chrono::steady_clock::time_point timestamp;
    
    ExecutionError() : timestamp(std::chrono::steady_clock::now()) {}
    ExecutionError(const std::string& msg, const std::string& code = "")
        : message(msg), error_code(code), timestamp(std::chrono::steady_clock::now()) {}
    
    std::string toString() const {
        return error_code.empty() ? message : error_code + ": " + message;
    }
};

/**
 * @brief Single data item flowing between nodes
 */
class NodeItem {
public:
    ParameterValue json;                           // Main data payload
    std::map<std::string, BinaryData> binary;     // Binary attachments
    std::map<std::string, ParameterValue> metadata; // Item metadata
    std::optional<ExecutionError> error;          // Item-level error
    
public:
    NodeItem() = default;
    explicit NodeItem(const ParameterValue& data) : json(data) {}
    
    /**
     * @brief Check if item has an error
     */
    bool hasError() const { return error.has_value(); }
    
    /**
     * @brief Set error for this item
     */
    void setError(const ExecutionError& err) { error = err; }
    void setError(const std::string& message, const std::string& code = "") {
        error = ExecutionError(message, code);
    }
    
    /**
     * @brief Add binary attachment
     */
    void addBinary(const std::string& key, const BinaryData& data) {
        binary[key] = data;
    }
    
    /**
     * @brief Add metadata
     */
    void addMetadata(const std::string& key, const ParameterValue& value) {
        metadata[key] = value;
    }
    
    /**
     * @brief Get metadata value
     */
    std::optional<ParameterValue> getMetadata(const std::string& key) const {
        auto it = metadata.find(key);
        return it != metadata.end() ? std::make_optional(it->second) : std::nullopt;
    }
    
    /**
     * @brief Check if binary attachment exists
     */
    bool hasBinary(const std::string& key) const {
        return binary.find(key) != binary.end();
    }
};

/**
 * @brief Execution status enumeration
 */
enum class ExecutionStatus {
    SUCCESS,        // Execution completed successfully
    ERROR,          // Execution failed with errors
    PARTIAL,        // Some items processed, some failed
    CANCELLED,      // Execution was cancelled
    TIMEOUT         // Execution timed out
};

/**
 * @brief Node execution result
 */
class ExecutionResult {
public:
    std::vector<NodeItem> items;
    ExecutionStatus status;
    std::vector<ExecutionError> errors;
    std::chrono::steady_clock::time_point start_time;
    std::chrono::steady_clock::time_point end_time;
    
public:
    ExecutionResult() : status(ExecutionStatus::SUCCESS), 
                       start_time(std::chrono::steady_clock::now()),
                       end_time(std::chrono::steady_clock::now()) {}
    
    /**
     * @brief Add successful result item
     */
    void addItem(const NodeItem& item) {
        items.push_back(item);
    }
    
    void addItem(const ParameterValue& data) {
        items.emplace_back(data);
    }
    
    /**
     * @brief Add error to result
     */
    void addError(const ExecutionError& error) {
        errors.push_back(error);
        if (status == ExecutionStatus::SUCCESS) {
            status = ExecutionStatus::PARTIAL;
        }
    }
    
    void addError(const std::string& message, const std::string& code = "") {
        addError(ExecutionError(message, code));
    }
    
    /**
     * @brief Set execution status
     */
    void setStatus(ExecutionStatus stat) { status = stat; }
    
    /**
     * @brief Mark execution as complete
     */
    void markComplete() {
        end_time = std::chrono::steady_clock::now();
        if (errors.empty() && status == ExecutionStatus::SUCCESS) {
            // Keep success status
        } else if (!errors.empty() && items.empty()) {
            status = ExecutionStatus::ERROR;
        } else if (!errors.empty() && !items.empty()) {
            status = ExecutionStatus::PARTIAL;
        }
    }
    
    /**
     * @brief Get execution duration
     */
    std::chrono::milliseconds getDuration() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    }
    
    /**
     * @brief Check if execution was successful
     */
    bool isSuccess() const { return status == ExecutionStatus::SUCCESS && errors.empty(); }
    
    /**
     * @brief Get summary string
     */
    std::string getSummary() const {
        std::ostringstream oss;
        oss << "Items: " << items.size() << ", Errors: " << errors.size() 
            << ", Duration: " << getDuration().count() << "ms"
            << ", Status: ";
        switch (status) {
            case ExecutionStatus::SUCCESS: oss << "SUCCESS"; break;
            case ExecutionStatus::ERROR: oss << "ERROR"; break;
            case ExecutionStatus::PARTIAL: oss << "PARTIAL"; break;
            case ExecutionStatus::CANCELLED: oss << "CANCELLED"; break;
            case ExecutionStatus::TIMEOUT: oss << "TIMEOUT"; break;
        }
        return oss.str();
    }
};

/**
 * @brief Forward declaration for AkaoLogicEngine
 */
class AkaoLogicEngine;

/**
 * @brief Node execution context providing input data and configuration
 */
class NodeContext {
private:
    std::vector<NodeItem> input_items_;
    std::map<std::string, ParameterValue> parameters_;
    std::shared_ptr<AkaoLogicEngine> logic_engine_;
    
public:
    NodeContext() = default;
    NodeContext(const std::vector<NodeItem>& items, const std::map<std::string, ParameterValue>& params)
        : input_items_(items), parameters_(params) {}
    
    /**
     * @brief Get input items
     */
    const std::vector<NodeItem>& getInputItems() const { return input_items_; }
    std::vector<NodeItem>& getInputItems() { return input_items_; }
    
    /**
     * @brief Add input item
     */
    void addInputItem(const NodeItem& item) { input_items_.push_back(item); }
    void addInputItem(const ParameterValue& data) { input_items_.emplace_back(data); }
    
    /**
     * @brief Get parameter value
     */
    ParameterValue getParameter(const std::string& name) const {
        auto it = parameters_.find(name);
        return it != parameters_.end() ? it->second : ParameterValue();
    }
    
    /**
     * @brief Check if parameter exists
     */
    bool hasParameter(const std::string& name) const {
        return parameters_.find(name) != parameters_.end();
    }
    
    /**
     * @brief Set parameter value
     */
    void setParameter(const std::string& name, const ParameterValue& value) {
        parameters_[name] = value;
    }
    
    /**
     * @brief Get all parameters
     */
    const std::map<std::string, ParameterValue>& getParameters() const { return parameters_; }
    
    /**
     * @brief Set Akao logic engine
     */
    void setLogicEngine(std::shared_ptr<AkaoLogicEngine> engine) { logic_engine_ = engine; }
    
    /**
     * @brief Execute Akao logic expression (mock implementation for now)
     */
    ParameterValue executeAkaoLogic(const std::string& expression) const {
        // Mock implementation - in real system would use logic_engine_
        if (expression == "true") return ParameterValue(true);
        if (expression == "false") return ParameterValue(false);
        if (expression.find("input.count") != std::string::npos) {
            return ParameterValue(static_cast<int64_t>(input_items_.size()));
        }
        return ParameterValue(std::string("mock_result"));
    }
    
    /**
     * @brief Get input item count
     */
    size_t getInputCount() const { return input_items_.size(); }
    
    /**
     * @brief Check if has input items
     */
    bool hasInput() const { return !input_items_.empty(); }
};

// Use ValidationResult from node_parameter.hpp (already defined)

/**
 * @brief Standard node interface for all implementations
 */
class INode {
public:
    virtual ~INode() = default;
    
    /**
     * @brief Get unique node identifier
     */
    virtual std::string getNodeId() const = 0;
    
    /**
     * @brief Get node type identifier
     */
    virtual std::string getNodeType() const = 0;
    
    /**
     * @brief Get complete node definition
     */
    virtual NodeDefinition getDefinition() const = 0;
    
    /**
     * @brief Execute node with given context
     */
    virtual ExecutionResult execute(const NodeContext& context) = 0;
    
    /**
     * @brief Validate node parameters
     */
    virtual ValidationResult validate(const std::map<std::string, ParameterValue>& parameters) = 0;
    
    /**
     * @brief Get node display name (optional override)
     */
    virtual std::string getDisplayName() const {
        return getDefinition().display_name;
    }
    
    /**
     * @brief Get node description (optional override)
     */
    virtual std::string getDescription() const {
        return getDefinition().description;
    }
    
    /**
     * @brief Check if node can handle given input
     */
    virtual bool canProcess(const NodeContext& /*context*/) const {
        // Default implementation - can be overridden
        return true;
    }
    
    /**
     * @brief Get estimated execution time (optional)
     */
    virtual std::chrono::milliseconds getEstimatedDuration(const NodeContext& /*context*/) const {
        // Default implementation
        return std::chrono::milliseconds(100);
    }
};

} // namespace evolve
} // namespace akao

#endif // AKAO_EVOLVE_INODE_V1_HPP