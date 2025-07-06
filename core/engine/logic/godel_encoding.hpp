#pragma once

#include "pure_logic_engine.hpp"
#include <map>
#include <string>
#include <vector>
#include <cmath>

namespace akao::logic::godel {

/**
 * @brief Enhanced Gödel Numbering System for Phase 4
 * 
 * Implements proper mathematical Gödel encoding using prime factorization
 * for logical formulas, providing bidirectional encoding/decoding with
 * mathematical soundness.
 * 
 * Core principle: Each logical construct gets a unique prime-based encoding
 * that can be perfectly decoded back to the original formula.
 */

// =============================================================================
// Core Gödel Encoding Functions
// =============================================================================

/**
 * @brief Encode basic logical symbols using prime numbers
 * Maps fundamental logical symbols to unique prime numbers
 */
class EncodePrimitiveFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "godel.encode_primitive"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::INTEGER; }
    std::string getDescription() const override { return "Encode basic logical symbols (∧, ∨, ¬, ∀, ∃, =, +, ×, etc.)"; }
};

/**
 * @brief Encode variables with unique numbering
 * Each variable gets a unique identifier based on name and scope
 */
class EncodeVariableFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "godel.encode_variable"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::INTEGER; }
    std::string getDescription() const override { return "Encode variables with unique numbering"; }
};

/**
 * @brief Encode complex terms recursively
 * Handles nested term structures using recursive prime factorization
 */
class EncodeTermFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "godel.encode_term"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::INTEGER; }
    std::string getDescription() const override { return "Encode complex terms recursively"; }
};

/**
 * @brief Complete formula encoding with proper AST parsing
 * Main encoding function that handles complete logical formulas
 */
class EncodeFormulaEnhancedFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "godel.encode_formula"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::INTEGER; }
    std::string getDescription() const override { return "Complete formula encoding with proper AST parsing"; }
};

/**
 * @brief Full bidirectional decoding
 * Decodes Gödel numbers back to their original logical formulas
 */
class DecodeNumberFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "godel.decode_number"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::STRING; }
    std::string getDescription() const override { return "Full bidirectional decoding"; }
};

/**
 * @brief Validation of Gödel numbers
 * Checks if a number represents a valid Gödel encoding
 */
class IsValidEncodingFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "godel.is_valid_encoding"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::BOOLEAN; }
    std::string getDescription() const override { return "Validation of Gödel numbers"; }
};

// =============================================================================
// Arithmetic Translation System
// =============================================================================

/**
 * @brief Convert logical formulas to pure arithmetic statements
 * Translates logic into number theory for self-reference
 */
class TranslateToArithmeticFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "godel.translate_to_arithmetic"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::STRING; }
    std::string getDescription() const override { return "Convert logical formulas to pure arithmetic statements"; }
};

/**
 * @brief Verify encoding correctness
 * Ensures encoded formulas decode correctly
 */
class VerifyEncodingFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "godel.verify_encoding"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::BOOLEAN; }
    std::string getDescription() const override { return "Verify encoding correctness"; }
};

/**
 * @brief Template substitution for self-reference
 * Performs template substitution to create self-referential statements
 */
class SubstituteGodelFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "godel.substitute_godel"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::STRING; }
    std::string getDescription() const override { return "Template substitution for self-reference"; }
};

// =============================================================================
// Enhanced Self-Reference Infrastructure
// =============================================================================

/**
 * @brief Proper diagonal lemma implementation
 * Implements the diagonal lemma for creating self-referential statements
 */
class DiagonalLemmaFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "godel.diagonal_lemma"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::INTEGER; }
    std::string getDescription() const override { return "Proper diagonal lemma implementation"; }
};

/**
 * @brief Generate "This sentence is not provable in system S"
 * Creates the classic Gödel sentence for incompleteness demonstrations
 */
class CreateGodelSentenceFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "godel.create_godel_sentence"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::INTEGER; }
    std::string getDescription() const override { return "Generate 'This sentence is not provable in system S'"; }
};

/**
 * @brief General fixed-point construction
 * Implements fixed-point theorem for arbitrary formulas
 */
class FixedPointTheoremFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override;
    std::string getName() const override { return "godel.fixed_point_theorem"; }
    std::vector<Value::Type> getParameterTypes() const override;
    Value::Type getReturnType() const override { return Value::Type::INTEGER; }
    std::string getDescription() const override { return "General fixed-point construction"; }
};

// =============================================================================
// Utility Functions and Constants
// =============================================================================

/**
 * @brief Prime number utilities for Gödel encoding
 */
class PrimeUtils {
public:
    static std::vector<int> getFirstNPrimes(int n);
    static bool isPrime(int num);
    static int getNthPrime(int n);
    static std::vector<int> primeFactorization(long long num);
    static long long primePower(int prime, int exponent);
};

/**
 * @brief Symbol mappings for logical constructs
 */
class SymbolMappings {
public:
    static const std::map<std::string, int> LOGICAL_SYMBOLS;
    static const std::map<int, std::string> SYMBOL_LOOKUP;
    static int getSymbolCode(const std::string& symbol);
    static std::string getSymbolFromCode(int code);
};

/**
 * @brief Registration function for all Gödel encoding functions
 */
void registerGodelFunctions(PureLogicEngine& engine);

} // namespace akao::logic::godel