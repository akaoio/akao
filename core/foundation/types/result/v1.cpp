/**
 * @id: akao:file:core:foundation:types:result:v1
 * @doc: Implementation of execution result container for workflow nodes. Provides comprehensive result tracking, error aggregation, timing measurement, and result composition capabilities for complex workflow operations.
 * @specification: ExecutionResult implementation with timing and error handling
 * @scope: Core foundation data types implementation
 * @timeline: 2025-07-10
 * @rationale: Concrete implementation of standardized result container
 * @methodology: Status-based result tracking with error aggregation and timing
 * @references: ["akao:class:core:foundation:types:executionresult:v1"]
 */

#include "v1.hpp"
#include <sstream>

namespace akao {
namespace foundation {
namespace types {

// Timing implementation
std::chrono::milliseconds ExecutionResult::Timing::duration() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
}

// Constructors
ExecutionResult::ExecutionResult() : status_(Status::SUCCESS) {}

ExecutionResult::ExecutionResult(Status status) : status_(status) {}

ExecutionResult::ExecutionResult(Status status, const NodeValue& data) 
    : status_(status), data_(data) {}

ExecutionResult::ExecutionResult(Status status, const std::string& errorMessage) 
    : status_(status) {
    if (status == Status::ERROR) {
        addError(errorMessage);
    }
}

// Static factory methods
ExecutionResult ExecutionResult::success() {
    return ExecutionResult(Status::SUCCESS);
}

ExecutionResult ExecutionResult::success(const NodeValue& data) {
    return ExecutionResult(Status::SUCCESS, data);
}

ExecutionResult ExecutionResult::error(const std::string& message) {
    ExecutionResult result(Status::ERROR);
    result.addError(message);
    return result;
}

ExecutionResult ExecutionResult::error(const std::string& code, const std::string& message) {
    ExecutionResult result(Status::ERROR);
    result.addError(code, message);
    return result;
}

ExecutionResult ExecutionResult::error(const std::string& code, const std::string& message, const std::string& details) {
    ExecutionResult result(Status::ERROR);
    result.addError(code, message, details);
    return result;
}

ExecutionResult ExecutionResult::warning(const NodeValue& data, const std::string& message) {
    ExecutionResult result(Status::WARNING, data);
    result.addWarning(message);
    return result;
}

ExecutionResult ExecutionResult::skipped(const std::string& reason) {
    ExecutionResult result(Status::SKIPPED);
    result.addWarning("Skipped: " + reason);
    return result;
}

ExecutionResult ExecutionResult::timeout() {
    ExecutionResult result(Status::TIMEOUT);
    result.addError("TIMEOUT", "Operation timed out");
    return result;
}

ExecutionResult ExecutionResult::cancelled() {
    return ExecutionResult(Status::CANCELLED);
}

// Status operations
ExecutionResult::Status ExecutionResult::getStatus() const {
    return status_;
}

bool ExecutionResult::isSuccess() const {
    return status_ == Status::SUCCESS;
}

bool ExecutionResult::isError() const {
    return status_ == Status::ERROR;
}

bool ExecutionResult::hasWarnings() const {
    return !warnings_.empty();
}

void ExecutionResult::setStatus(Status status) {
    status_ = status;
}

// Data operations
const NodeValue& ExecutionResult::getData() const {
    return data_;
}

void ExecutionResult::setData(const NodeValue& data) {
    data_ = data;
}

NodeValue& ExecutionResult::getMutableData() {
    return data_;
}

// Error operations
void ExecutionResult::addError(const std::string& message) {
    addError("GENERIC_ERROR", message);
}

void ExecutionResult::addError(const std::string& code, const std::string& message) {
    addError(code, message, "");
}

void ExecutionResult::addError(const std::string& code, const std::string& message, const std::string& details) {
    ErrorInfo error;
    error.code = code;
    error.message = message;
    error.details = details;
    errors_.push_back(error);
    
    if (status_ == Status::SUCCESS) {
        status_ = Status::ERROR;
    }
}

void ExecutionResult::addError(const ErrorInfo& error) {
    errors_.push_back(error);
    
    if (status_ == Status::SUCCESS) {
        status_ = Status::ERROR;
    }
}

const std::vector<ExecutionResult::ErrorInfo>& ExecutionResult::getErrors() const {
    return errors_;
}

std::string ExecutionResult::getErrorSummary() const {
    if (errors_.empty()) {
        return "";
    }
    
    std::ostringstream oss;
    for (size_t i = 0; i < errors_.size(); ++i) {
        if (i > 0) oss << "; ";
        oss << errors_[i].code << ": " << errors_[i].message;
    }
    return oss.str();
}

void ExecutionResult::clearErrors() {
    errors_.clear();
}

// Warning operations
void ExecutionResult::addWarning(const std::string& message) {
    warnings_.push_back(message);
}

const std::vector<std::string>& ExecutionResult::getWarnings() const {
    return warnings_;
}

void ExecutionResult::clearWarnings() {
    warnings_.clear();
}

// Timing operations
void ExecutionResult::startTiming() {
    timing_.startTime = std::chrono::high_resolution_clock::now();
}

void ExecutionResult::endTiming() {
    timing_.endTime = std::chrono::high_resolution_clock::now();
}

const ExecutionResult::Timing& ExecutionResult::getTiming() const {
    return timing_;
}

std::chrono::milliseconds ExecutionResult::getDuration() const {
    return timing_.duration();
}

// Node identification
const std::string& ExecutionResult::getNodeId() const {
    return nodeId_;
}

void ExecutionResult::setNodeId(const std::string& nodeId) {
    nodeId_ = nodeId;
}

// Result composition
void ExecutionResult::merge(const ExecutionResult& other) {
    // Merge errors
    errors_.insert(errors_.end(), other.errors_.begin(), other.errors_.end());
    
    // Merge warnings
    warnings_.insert(warnings_.end(), other.warnings_.begin(), other.warnings_.end());
    
    // Update status (error takes precedence)
    if (other.status_ == Status::ERROR || !errors_.empty()) {
        status_ = Status::ERROR;
    } else if (other.status_ == Status::WARNING || !warnings_.empty()) {
        if (status_ == Status::SUCCESS) {
            status_ = Status::WARNING;
        }
    }
}

ExecutionResult ExecutionResult::combine(const ExecutionResult& other) const {
    ExecutionResult combined = *this;
    combined.merge(other);
    return combined;
}

// String representation
std::string ExecutionResult::toString() const {
    std::ostringstream oss;
    oss << "ExecutionResult{status=" << getStatusString();
    
    if (!nodeId_.empty()) {
        oss << ", nodeId=" << nodeId_;
    }
    
    if (!errors_.empty()) {
        oss << ", errors=" << getErrorSummary();
    }
    
    if (!warnings_.empty()) {
        oss << ", warnings=" << warnings_.size();
    }
    
    oss << "}";
    return oss.str();
}

std::string ExecutionResult::getStatusString() const {
    switch (status_) {
        case Status::SUCCESS: return "SUCCESS";
        case Status::ERROR: return "ERROR";
        case Status::WARNING: return "WARNING";
        case Status::SKIPPED: return "SKIPPED";
        case Status::TIMEOUT: return "TIMEOUT";
        case Status::CANCELLED: return "CANCELLED";
        default: return "UNKNOWN";
    }
}

// Operators
bool ExecutionResult::operator==(const ExecutionResult& other) const {
    return status_ == other.status_ && 
           data_ == other.data_ && 
           errors_.size() == other.errors_.size() &&
           warnings_ == other.warnings_;
}

bool ExecutionResult::operator!=(const ExecutionResult& other) const {
    return !(*this == other);
}

ExecutionResult::operator bool() const {
    return isSuccess();
}

} // namespace types
} // namespace foundation
} // namespace akao