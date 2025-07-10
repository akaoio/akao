/**
 * @id: akao:file:core:foundation:types:result:v1
 * @doc: Standard execution result container for all workflow nodes. Provides consistent status reporting, error handling, and data return mechanisms across the entire node ecosystem. Enables proper error propagation and result composition in complex workflows.
 * @specification: Execution result container with status and error handling
 * @scope: Core foundation data types subsystem
 * @timeline: 2025-07-10
 * @rationale: Standardize execution results and error handling across nodes
 * @methodology: Result container with status enum and optional data/error info
 * @references: ["akao:class:core:foundation:types:executionresult:v1"]
 */

#pragma once

#include "../../types/value/v1.hpp"
#include <string>
#include <vector>
#include <chrono>

namespace akao {
namespace foundation {
namespace types {

/**
 * @id: akao:class:core:foundation:types:executionresult:v1
 * @doc: Container for workflow node execution results. Tracks execution status, holds result data, captures error information, and provides timing metrics. Used by all nodes to return consistent execution information to the workflow engine for proper error handling and data flow.
 * @specification: Standard execution result with status, data, and error tracking
 * @scope: Core foundation data types
 * @timeline: 2025-07-10
 * @rationale: Consistent result handling across heterogeneous node ecosystem
 * @methodology: Status enum with optional data and error detail containers
 * @references: ["akao:class:core:foundation:types:nodevalue:v1"]
 */
class ExecutionResult {
public:
    enum class Status {
        SUCCESS,
        ERROR,
        WARNING,
        SKIPPED,
        TIMEOUT,
        CANCELLED
    };

    struct ErrorInfo {
        std::string code;
        std::string message;
        std::string details;
        std::string stackTrace;
    };

    struct Timing {
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;
        std::chrono::milliseconds duration() const;
    };

private:
    Status status_;
    NodeValue data_;
    std::vector<ErrorInfo> errors_;
    std::vector<std::string> warnings_;
    Timing timing_;
    std::string nodeId_;

public:
    // Constructors
    ExecutionResult();
    explicit ExecutionResult(Status status);
    ExecutionResult(Status status, const NodeValue& data);
    ExecutionResult(Status status, const std::string& errorMessage);

    // Static factory methods
    static ExecutionResult success();
    static ExecutionResult success(const NodeValue& data);
    static ExecutionResult error(const std::string& message);
    static ExecutionResult error(const std::string& code, const std::string& message);
    static ExecutionResult error(const std::string& code, const std::string& message, const std::string& details);
    static ExecutionResult warning(const NodeValue& data, const std::string& message);
    static ExecutionResult skipped(const std::string& reason);
    static ExecutionResult timeout();
    static ExecutionResult cancelled();

    // Status operations
    Status getStatus() const;
    bool isSuccess() const;
    bool isError() const;
    bool hasWarnings() const;
    void setStatus(Status status);

    // Data operations
    const NodeValue& getData() const;
    void setData(const NodeValue& data);
    NodeValue& getMutableData();

    // Error operations
    void addError(const std::string& message);
    void addError(const std::string& code, const std::string& message);
    void addError(const std::string& code, const std::string& message, const std::string& details);
    void addError(const ErrorInfo& error);
    const std::vector<ErrorInfo>& getErrors() const;
    std::string getErrorSummary() const;
    void clearErrors();

    // Warning operations
    void addWarning(const std::string& message);
    const std::vector<std::string>& getWarnings() const;
    void clearWarnings();

    // Timing operations
    void startTiming();
    void endTiming();
    const Timing& getTiming() const;
    std::chrono::milliseconds getDuration() const;

    // Node identification
    const std::string& getNodeId() const;
    void setNodeId(const std::string& nodeId);

    // Result composition
    void merge(const ExecutionResult& other);
    ExecutionResult combine(const ExecutionResult& other) const;

    // String representation
    std::string toString() const;
    std::string getStatusString() const;

    // Operators
    bool operator==(const ExecutionResult& other) const;
    bool operator!=(const ExecutionResult& other) const;
    explicit operator bool() const; // Returns true if successful
};

} // namespace types
} // namespace foundation
} // namespace akao