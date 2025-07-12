/**
 * @id: akao:file:core:foundation:types:result_util
 * @doc: Standardized error handling utilities for consistent error propagation across the Akao system. Provides Result<T> template for operations that can fail without exceptions, and standardized error context preservation patterns.
 * @specification: Error handling standardization utilities
 * @scope: Core foundation error handling utilities
 * @timeline: 2025-07-12
 * @rationale: Create consistent error handling patterns across all subsystems
 * @methodology: Template-based Result type with error context preservation
 * @references: ["akao:class:core:foundation:types:executionresult:v1"]
 */

#pragma once

#include "result/v1.hpp"
#include <string>
#include <optional>
#include <variant>

namespace akao {
namespace foundation {
namespace types {

/**
 * ERROR HANDLING STANDARDIZATION: Template Result type for consistent error handling
 * 
 * This provides a standardized way to return either a successful value or an error
 * without using exceptions, ensuring consistent error handling across the codebase.
 */
template<typename T>
class Result {
private:
    std::variant<T, ExecutionResult::ErrorInfo> data_;
    bool success_;

public:
    // Success constructors
    Result(const T& value) : data_(value), success_(true) {}
    Result(T&& value) : data_(std::move(value)), success_(true) {}
    
    // Error constructors
    Result(const std::string& error_message) 
        : data_(ExecutionResult::ErrorInfo{"", error_message, "", ""}), success_(false) {}
    
    Result(const std::string& error_code, const std::string& error_message)
        : data_(ExecutionResult::ErrorInfo{error_code, error_message, "", ""}), success_(false) {}
    
    Result(const ExecutionResult::ErrorInfo& error)
        : data_(error), success_(false) {}
    
    // Status checking
    bool isSuccess() const { return success_; }
    bool isError() const { return !success_; }
    
    // Value access (throws if error)
    const T& getValue() const {
        if (!success_) {
            throw std::runtime_error("Attempted to get value from error result");
        }
        return std::get<T>(data_);
    }
    
    T& getValue() {
        if (!success_) {
            throw std::runtime_error("Attempted to get value from error result");
        }
        return std::get<T>(data_);
    }
    
    // Safe value access
    T getValueOr(const T& defaultValue) const {
        return success_ ? std::get<T>(data_) : defaultValue;
    }
    
    // Error access
    const ExecutionResult::ErrorInfo& getError() const {
        if (success_) {
            throw std::runtime_error("Attempted to get error from success result");
        }
        return std::get<ExecutionResult::ErrorInfo>(data_);
    }
    
    std::string getErrorMessage() const {
        return success_ ? "" : std::get<ExecutionResult::ErrorInfo>(data_).message;
    }
    
    // Factory methods
    static Result<T> success(const T& value) {
        return Result<T>(value);
    }
    
    static Result<T> success(T&& value) {
        return Result<T>(std::move(value));
    }
    
    static Result<T> error(const std::string& message) {
        return Result<T>(message);
    }
    
    static Result<T> error(const std::string& code, const std::string& message) {
        return Result<T>(code, message);
    }
    
    // Convert to ExecutionResult
    ExecutionResult toExecutionResult() const {
        if (success_) {
            return ExecutionResult::success();
        } else {
            const auto& err = std::get<ExecutionResult::ErrorInfo>(data_);
            return ExecutionResult::error(err.code, err.message);
        }
    }
};

/**
 * Common error codes for standardized error reporting
 */
namespace ErrorCodes {
    constexpr const char* INVALID_ARGUMENT = "INVALID_ARGUMENT";
    constexpr const char* RESOURCE_UNAVAILABLE = "RESOURCE_UNAVAILABLE";
    constexpr const char* PERMISSION_DENIED = "PERMISSION_DENIED";
    constexpr const char* NOT_FOUND = "NOT_FOUND";
    constexpr const char* ALREADY_EXISTS = "ALREADY_EXISTS";
    constexpr const char* TIMEOUT = "TIMEOUT";
    constexpr const char* NETWORK_ERROR = "NETWORK_ERROR";
    constexpr const char* PARSE_ERROR = "PARSE_ERROR";
    constexpr const char* SYSTEM_ERROR = "SYSTEM_ERROR";
    constexpr const char* INTERNAL_ERROR = "INTERNAL_ERROR";
}

/**
 * Helper macros for consistent error handling
 */
#define AKAO_RETURN_IF_ERROR(result) \
    do { \
        if ((result).isError()) { \
            return (result); \
        } \
    } while(0)

#define AKAO_PROPAGATE_ERROR(result, conversion) \
    do { \
        if ((result).isError()) { \
            return conversion((result).getError()); \
        } \
    } while(0)

} // namespace types
} // namespace foundation
} // namespace akao