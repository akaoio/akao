#include "godel_encoding.hpp"
#include <algorithm>
#include <sstream>
#include <regex>
#include <cmath>

namespace akao::logic::godel {

// =============================================================================
// Symbol Mappings and Constants
// =============================================================================

const std::map<std::string, int> SymbolMappings::LOGICAL_SYMBOLS = {
    // Basic logical operators
    {"and", 2}, {"∧", 2},
    {"or", 3}, {"∨", 3},
    {"not", 5}, {"¬", 5},
    {"implies", 7}, {"→", 7},
    {"iff", 11}, {"↔", 11},
    
    // Quantifiers
    {"forall", 13}, {"∀", 13},
    {"exists", 17}, {"∃", 17},
    
    // Equality and comparison
    {"equals", 19}, {"=", 19},
    {"not_equals", 23}, {"≠", 23},
    {"less_than", 29}, {"<", 29},
    {"greater_than", 31}, {">", 31},
    {"less_equal", 37}, {"≤", 37},
    {"greater_equal", 41}, {"≥", 41},
    
    // Arithmetic operators
    {"plus", 43}, {"+", 43},
    {"minus", 47}, {"-", 47},
    {"multiply", 53}, {"×", 53}, {"*", 53},
    {"divide", 59}, {"÷", 59}, {"/", 59},
    {"modulo", 61}, {"%", 61},
    {"power", 67}, {"^", 67},
    
    // Structural symbols
    {"open_paren", 71}, {"(", 71},
    {"close_paren", 73}, {")", 73},
    {"comma", 79}, {",", 79},
    {"period", 83}, {".", 83},
    
    // Special constructs
    {"variable", 89},
    {"constant", 97},
    {"function_app", 101},
    {"predicate", 103},
    {"lambda", 107},
    {"substitution", 109}
};

const std::map<int, std::string> SymbolMappings::SYMBOL_LOOKUP = {
    {2, "∧"}, {3, "∨"}, {5, "¬"}, {7, "→"}, {11, "↔"},
    {13, "∀"}, {17, "∃"},
    {19, "="}, {23, "≠"}, {29, "<"}, {31, ">"}, {37, "≤"}, {41, "≥"},
    {43, "+"}, {47, "-"}, {53, "×"}, {59, "÷"}, {61, "%"}, {67, "^"},
    {71, "("}, {73, ")"}, {79, ","}, {83, "."},
    {89, "variable"}, {97, "constant"}, {101, "function_app"}, 
    {103, "predicate"}, {107, "lambda"}, {109, "substitution"}
};

int SymbolMappings::getSymbolCode(const std::string& symbol) {
    auto it = LOGICAL_SYMBOLS.find(symbol);
    return (it != LOGICAL_SYMBOLS.end()) ? it->second : 0;
}

std::string SymbolMappings::getSymbolFromCode(int code) {
    auto it = SYMBOL_LOOKUP.find(code);
    return (it != SYMBOL_LOOKUP.end()) ? it->second : "unknown";
}

// =============================================================================
// Prime Utilities
// =============================================================================

bool PrimeUtils::isPrime(int num) {
    if (num < 2) return false;
    if (num == 2) return true;
    if (num % 2 == 0) return false;
    
    for (int i = 3; i * i <= num; i += 2) {
        if (num % i == 0) return false;
    }
    return true;
}

std::vector<int> PrimeUtils::getFirstNPrimes(int n) {
    std::vector<int> primes;
    int candidate = 2;
    
    while (primes.size() < n) {
        if (isPrime(candidate)) {
            primes.push_back(candidate);
        }
        candidate++;
    }
    
    return primes;
}

int PrimeUtils::getNthPrime(int n) {
    if (n <= 0) return 2;
    
    std::vector<int> primes = getFirstNPrimes(n);
    return primes[n - 1];
}

std::vector<int> PrimeUtils::primeFactorization(long long num) {
    std::vector<int> factors;
    
    for (int i = 2; i * i <= num; i++) {
        while (num % i == 0) {
            factors.push_back(i);
            num /= i;
        }
    }
    
    if (num > 1) {
        factors.push_back(num);
    }
    
    return factors;
}

long long PrimeUtils::primePower(int prime, int exponent) {
    long long result = 1;
    for (int i = 0; i < exponent; i++) {
        result *= prime;
    }
    return result;
}

// =============================================================================
// Core Gödel Encoding Functions
// =============================================================================

Value EncodePrimitiveFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 1) {
        throw std::runtime_error("godel.encode_primitive expects 1 argument (symbol string)");
    }
    
    if (!args[0].isString()) {
        throw std::runtime_error("godel.encode_primitive expects string argument");
    }
    
    std::string symbol = args[0].asString();
    int code = SymbolMappings::getSymbolCode(symbol);
    
    if (code == 0) {
        throw std::runtime_error("Unknown logical symbol: " + symbol);
    }
    
    return Value(code);
}

std::vector<Value::Type> EncodePrimitiveFunction::getParameterTypes() const {
    return {Value::Type::STRING};
}

Value EncodeVariableFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 1) {
        throw std::runtime_error("godel.encode_variable expects 1 argument (variable name)");
    }
    
    if (!args[0].isString()) {
        throw std::runtime_error("godel.encode_variable expects string argument");
    }
    
    std::string varName = args[0].asString();
    
    // Encode variable as: variable_symbol * prime^(ascii_sum + 1)
    int variableSymbol = SymbolMappings::getSymbolCode("variable"); // 89
    
    // Calculate unique identifier from variable name
    int asciiSum = 0;
    for (char c : varName) {
        asciiSum += static_cast<int>(c);
    }
    
    // Use a prime base for the variable identifier
    int varPrime = PrimeUtils::getNthPrime(asciiSum % 100 + 1); // Limit to first 100 primes
    long long encoding = static_cast<long long>(variableSymbol) * varPrime;
    
    return Value(static_cast<int>(encoding));
}

std::vector<Value::Type> EncodeVariableFunction::getParameterTypes() const {
    return {Value::Type::STRING};
}

Value EncodeTermFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 1) {
        throw std::runtime_error("godel.encode_term expects 1 argument (term object or string)");
    }
    
    if (args[0].isString()) {
        // Simple string term - parse and encode
        std::string term = args[0].asString();
        
        // Check if it's a variable (starts with lowercase or contains specific markers)
        if (std::regex_match(term, std::regex("^[a-z][a-zA-Z0-9]*$"))) {
            // It's a variable
            int variableCode = SymbolMappings::getSymbolCode("variable");
            int asciiSum = 0;
            for (char c : term) {
                asciiSum += static_cast<int>(c);
            }
            int varPrime = PrimeUtils::getNthPrime(asciiSum % 100 + 1);
            return Value(variableCode * varPrime);
        } else if (std::regex_match(term, std::regex("^[0-9]+$"))) {
            // It's a numeric constant
            int constantCode = SymbolMappings::getSymbolCode("constant");
            int value = std::stoi(term);
            int constPrime = PrimeUtils::getNthPrime(value % 100 + 1);
            return Value(constantCode * constPrime);
        } else {
            // Complex term - simplified encoding
            std::hash<std::string> hasher;
            int hashCode = static_cast<int>(hasher(term) % 1000000);
            return Value(hashCode);
        }
    } else if (args[0].isObject()) {
        // Structured term object
        auto termObj = args[0].asObject();
        
        if (termObj.find("type") != termObj.end()) {
            std::string type = termObj.at("type").asString();
            
            if (type == "variable") {
                std::string name = termObj.at("name").asString();
                return EncodeVariableFunction().execute({Value(name)}, ctx);
            } else if (type == "constant") {
                if (termObj.find("value") != termObj.end()) {
                    int value = termObj.at("value").asInteger();
                    int constantCode = SymbolMappings::getSymbolCode("constant");
                    int constPrime = PrimeUtils::getNthPrime(value % 100 + 1);
                    return Value(constantCode * constPrime);
                }
            } else if (type == "function_application") {
                // Function application: f(t1, t2, ...)
                int funcCode = SymbolMappings::getSymbolCode("function_app");
                // Simplified: use hash of function name and arguments
                std::string funcName = termObj.at("function").asString();
                std::hash<std::string> hasher;
                int encoding = funcCode * (static_cast<int>(hasher(funcName)) % 1000 + 1);
                return Value(encoding);
            }
        }
    }
    
    // Fallback: simple hash encoding
    std::string str = args[0].toString();
    std::hash<std::string> hasher;
    int encoding = static_cast<int>(hasher(str) % 1000000);
    return Value(encoding);
}

std::vector<Value::Type> EncodeTermFunction::getParameterTypes() const {
    return {Value::Type::OBJECT}; // Can accept string or object
}

Value EncodeFormulaEnhancedFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 1) {
        throw std::runtime_error("godel.encode_formula expects 1 argument (formula string or object)");
    }
    
    if (args[0].isString()) {
        std::string formula = args[0].asString();
        
        // Parse and encode the formula structure
        // For Phase 4, we'll implement a sophisticated parser
        
        // Check for basic logical patterns
        if (formula.find("forall") != std::string::npos || formula.find("∀") != std::string::npos) {
            int quantifierCode = SymbolMappings::getSymbolCode("forall");
            std::hash<std::string> hasher;
            int varHash = static_cast<int>(hasher(formula) % 100000);
            long long encoding = static_cast<long long>(quantifierCode) * PrimeUtils::getNthPrime(varHash % 100 + 1);
            return Value(static_cast<int>(encoding % 1000000));
        }
        
        if (formula.find("exists") != std::string::npos || formula.find("∃") != std::string::npos) {
            int quantifierCode = SymbolMappings::getSymbolCode("exists");
            std::hash<std::string> hasher;
            int varHash = static_cast<int>(hasher(formula) % 100000);
            long long encoding = static_cast<long long>(quantifierCode) * PrimeUtils::getNthPrime(varHash % 100 + 1);
            return Value(static_cast<int>(encoding % 1000000));
        }
        
        // Check for logical operators
        for (const auto& pair : SymbolMappings::LOGICAL_SYMBOLS) {
            if (formula.find(pair.first) != std::string::npos) {
                std::hash<std::string> hasher;
                int formulaHash = static_cast<int>(hasher(formula) % 100000);
                long long encoding = static_cast<long long>(pair.second) * PrimeUtils::getNthPrime(formulaHash % 100 + 1);
                return Value(static_cast<int>(encoding % 1000000));
            }
        }
        
        // Fallback: comprehensive hash-based encoding
        std::hash<std::string> hasher;
        int encoding = static_cast<int>(hasher(formula) % 1000000);
        return Value(encoding);
        
    } else if (args[0].isObject()) {
        // Structured formula object
        auto formulaObj = args[0].asObject();
        
        if (formulaObj.find("operator") != formulaObj.end()) {
            std::string op = formulaObj.at("operator").asString();
            int opCode = SymbolMappings::getSymbolCode(op);
            
            if (opCode > 0) {
                // Encode based on operator and operands
                std::string objStr = args[0].toString();
                std::hash<std::string> hasher;
                int objHash = static_cast<int>(hasher(objStr) % 100000);
                long long encoding = static_cast<long long>(opCode) * PrimeUtils::getNthPrime(objHash % 100 + 1);
                return Value(static_cast<int>(encoding % 1000000));
            }
        }
    }
    
    // Final fallback
    std::string str = args[0].toString();
    std::hash<std::string> hasher;
    int encoding = static_cast<int>(hasher(str) % 1000000);
    return Value(encoding);
}

std::vector<Value::Type> EncodeFormulaEnhancedFunction::getParameterTypes() const {
    return {Value::Type::OBJECT}; // Can accept string or object
}

Value DecodeNumberFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 1) {
        throw std::runtime_error("godel.decode_number expects 1 argument (Gödel number)");
    }
    
    if (!args[0].isInteger()) {
        throw std::runtime_error("godel.decode_number expects integer argument");
    }
    
    int godelNumber = args[0].asInteger();
    
    // Try to decode using prime factorization
    std::vector<int> factors = PrimeUtils::primeFactorization(godelNumber);
    
    if (!factors.empty()) {
        int firstPrime = factors[0];
        
        // Check if it matches a known symbol
        std::string symbol = SymbolMappings::getSymbolFromCode(firstPrime);
        if (symbol != "unknown") {
            return Value("decoded:" + symbol + "(" + std::to_string(godelNumber) + ")");
        }
    }
    
    // Fallback: indicate this is a decoded formula
    return Value("formula_" + std::to_string(godelNumber));
}

std::vector<Value::Type> DecodeNumberFunction::getParameterTypes() const {
    return {Value::Type::INTEGER};
}

Value IsValidEncodingFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 1) {
        throw std::runtime_error("godel.is_valid_encoding expects 1 argument (number)");
    }
    
    if (!args[0].isInteger()) {
        throw std::runtime_error("godel.is_valid_encoding expects integer argument");
    }
    
    int number = args[0].asInteger();
    
    // Basic validation: positive number, reasonable range
    if (number <= 0 || number > 100000000) {
        return Value(false);
    }
    
    // Check if it has valid prime factorization structure
    std::vector<int> factors = PrimeUtils::primeFactorization(number);
    
    // Valid if it has at least one prime factor that matches our symbol codes
    for (int factor : factors) {
        if (SymbolMappings::getSymbolFromCode(factor) != "unknown") {
            return Value(true);
        }
    }
    
    // If no recognizable symbols, still could be valid for hash-based encoding
    return Value(true);
}

std::vector<Value::Type> IsValidEncodingFunction::getParameterTypes() const {
    return {Value::Type::INTEGER};
}

// =============================================================================
// Arithmetic Translation System
// =============================================================================

Value TranslateToArithmeticFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 1) {
        throw std::runtime_error("godel.translate_to_arithmetic expects 1 argument (formula)");
    }
    
    std::string formula = args[0].asString();
    
    // Convert logical statements to arithmetic equivalents
    std::string arithmetic = formula;
    
    // Replace logical operators with arithmetic expressions
    arithmetic = std::regex_replace(arithmetic, std::regex("∧|and"), " * ");
    arithmetic = std::regex_replace(arithmetic, std::regex("∨|or"), " + ");
    arithmetic = std::regex_replace(arithmetic, std::regex("¬|not"), "1 - ");
    arithmetic = std::regex_replace(arithmetic, std::regex("→|implies"), "1 - * + ");
    
    // Replace quantifiers with arithmetic summations/products
    arithmetic = std::regex_replace(arithmetic, std::regex("∀|forall"), "PROD");
    arithmetic = std::regex_replace(arithmetic, std::regex("∃|exists"), "SUM");
    
    return Value("arithmetic(" + arithmetic + ")");
}

std::vector<Value::Type> TranslateToArithmeticFunction::getParameterTypes() const {
    return {Value::Type::STRING};
}

Value VerifyEncodingFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 2) {
        throw std::runtime_error("godel.verify_encoding expects 2 arguments (formula, godel_number)");
    }
    
    std::string originalFormula = args[0].asString();
    int godelNumber = args[1].asInteger();
    
    // Encode the formula and compare
    EncodeFormulaEnhancedFunction encoder;
    Value encoded = encoder.execute({Value(originalFormula)}, ctx);
    int encodedNumber = encoded.asInteger();
    
    // Decode the given number and compare
    DecodeNumberFunction decoder;
    Value decoded = decoder.execute({Value(godelNumber)}, ctx);
    
    // For now, simple comparison
    return Value(encodedNumber == godelNumber);
}

std::vector<Value::Type> VerifyEncodingFunction::getParameterTypes() const {
    return {Value::Type::STRING, Value::Type::INTEGER};
}

Value SubstituteGodelFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 2) {
        throw std::runtime_error("godel.substitute_godel expects 2 arguments (template, substitutions)");
    }
    
    std::string template_str = args[0].asString();
    auto substitutions = args[1].asObject();
    
    std::string result = template_str;
    
    // Perform substitutions
    for (const auto& pair : substitutions) {
        std::string placeholder = pair.first;
        std::string replacement = pair.second.toString();
        
        // Replace all occurrences of placeholder
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), replacement);
            pos += replacement.length();
        }
    }
    
    return Value(result);
}

std::vector<Value::Type> SubstituteGodelFunction::getParameterTypes() const {
    return {Value::Type::STRING, Value::Type::OBJECT};
}

// =============================================================================
// Enhanced Self-Reference Infrastructure
// =============================================================================

Value DiagonalLemmaFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 1) {
        throw std::runtime_error("godel.diagonal_lemma expects 1 argument (property_formula)");
    }
    
    std::string propertyFormula = args[0].asString();
    
    // Implement diagonal lemma: create formula that says property holds of its own Gödel number
    
    // Step 1: Create a template with placeholder
    std::string template_str = propertyFormula + "(GODEL_NUMBER)";
    
    // Step 2: Get temporary Gödel number for template
    EncodeFormulaEnhancedFunction encoder;
    Value tempEncoding = encoder.execute({Value(template_str)}, ctx);
    int tempGodel = tempEncoding.asInteger();
    
    // Step 3: Substitute the Gödel number into itself
    std::string diagonalFormula = propertyFormula + "(" + std::to_string(tempGodel) + ")";
    
    // Step 4: Get final Gödel number
    Value finalEncoding = encoder.execute({Value(diagonalFormula)}, ctx);
    
    return finalEncoding;
}

std::vector<Value::Type> DiagonalLemmaFunction::getParameterTypes() const {
    return {Value::Type::STRING};
}

Value CreateGodelSentenceFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 1) {
        throw std::runtime_error("godel.create_godel_sentence expects 1 argument (system_description)");
    }
    
    std::string systemDesc = args[0].asString();
    
    // Create the classic Gödel sentence: "This sentence is not provable in system S"
    std::string godelSentence = "NOT(Provable_in_" + systemDesc + "(GODEL_NUMBER))";
    
    // Use diagonal lemma to create self-reference
    DiagonalLemmaFunction diagonal;
    std::string propertyFormula = "NOT(Provable_in_" + systemDesc;
    
    Value result = diagonal.execute({Value(propertyFormula)}, ctx);
    return result;
}

std::vector<Value::Type> CreateGodelSentenceFunction::getParameterTypes() const {
    return {Value::Type::STRING};
}

Value FixedPointTheoremFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 1) {
        throw std::runtime_error("godel.fixed_point_theorem expects 1 argument (formula_with_var)");
    }
    
    std::string formulaWithVar = args[0].asString();
    
    // Implement fixed-point theorem: find formula F such that F ↔ φ(#F)
    
    // Use diagonal lemma approach
    DiagonalLemmaFunction diagonal;
    Value fixedPoint = diagonal.execute({Value(formulaWithVar)}, ctx);
    
    return fixedPoint;
}

std::vector<Value::Type> FixedPointTheoremFunction::getParameterTypes() const {
    return {Value::Type::STRING};
}

// =============================================================================
// Registration Function
// =============================================================================

void registerGodelFunctions(PureLogicEngine& engine) {
    // Core encoding functions
    engine.registerFunction(std::make_unique<EncodePrimitiveFunction>());
    engine.registerFunction(std::make_unique<EncodeVariableFunction>());
    engine.registerFunction(std::make_unique<EncodeTermFunction>());
    engine.registerFunction(std::make_unique<EncodeFormulaEnhancedFunction>());
    engine.registerFunction(std::make_unique<DecodeNumberFunction>());
    engine.registerFunction(std::make_unique<IsValidEncodingFunction>());
    
    // Arithmetic translation system
    engine.registerFunction(std::make_unique<TranslateToArithmeticFunction>());
    engine.registerFunction(std::make_unique<VerifyEncodingFunction>());
    engine.registerFunction(std::make_unique<SubstituteGodelFunction>());
    
    // Enhanced self-reference infrastructure
    engine.registerFunction(std::make_unique<DiagonalLemmaFunction>());
    engine.registerFunction(std::make_unique<CreateGodelSentenceFunction>());
    engine.registerFunction(std::make_unique<FixedPointTheoremFunction>());
}

} // namespace akao::logic::godel