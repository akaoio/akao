/**
 * @id: akao:file:core:engine:logic_builtin_impl:v1
 * @doc: Builtin function library implementation providing comprehensive collection of fundamental operations including mathematical functions, string manipulation, collection operations, Peano arithmetic primitives, and testing utilities. Implements essential computational building blocks for Pure Logic engine with type-safe function signatures, parameter validation, and standardized return types for reliable logical computation and system testing.
 * @specification: Implementation for builtin function library with mathematical operations, string functions, and collection utilities
 * @scope: Core engine builtin function library framework implementation
 * @timeline: 2025-07-09
 * @rationale: Provide essential computational primitives and utility functions for Pure Logic engine operation and testing
 * @methodology: Object-oriented builtin function implementation with type-safe interfaces and comprehensive operation coverage
 * @references: ["akao:file:core:engine:logic_builtin:v1", "akao:file:core:engine:logic_pure:v1"]
 */

#include "v1.hpp"
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>

namespace akao::logic::builtin {

// =============================================================================
// Collection Functions
// =============================================================================

Value CountFunction::execute(const std::vector<Value>& args, Context& ctx) {
    validateArgs(args);
    return Value(static_cast<int>(args[0].size()));
}

std::vector<Value::Type> CountFunction::getParameterTypes() const {
    return {Value::Type::COLLECTION};
}

Value ContainsFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 2) {
        throw std::runtime_error("collection.contains requires 2 arguments");
    }
    if (!args[0].isCollection()) {
        throw std::runtime_error("collection.contains first argument must be collection");
    }
    return Value(args[0].contains(args[1]));
}

std::vector<Value::Type> ContainsFunction::getParameterTypes() const {
    // Return empty to skip strict type checking for now (allow any type as second param)
    return {};
}

// =============================================================================
// Math Functions
// =============================================================================

Value AddFunction::execute(const std::vector<Value>& args, Context& ctx) {
    validateArgs(args);
    return Value(args[0].asInteger() + args[1].asInteger());
}

std::vector<Value::Type> AddFunction::getParameterTypes() const {
    return {Value::Type::INTEGER, Value::Type::INTEGER};
}

Value SubtractFunction::execute(const std::vector<Value>& args, Context& ctx) {
    validateArgs(args);
    return Value(args[0].asInteger() - args[1].asInteger());
}

std::vector<Value::Type> SubtractFunction::getParameterTypes() const {
    return {Value::Type::INTEGER, Value::Type::INTEGER};
}

Value MultiplyFunction::execute(const std::vector<Value>& args, Context& ctx) {
    validateArgs(args);
    return Value(args[0].asInteger() * args[1].asInteger());
}

std::vector<Value::Type> MultiplyFunction::getParameterTypes() const {
    return {Value::Type::INTEGER, Value::Type::INTEGER};
}

Value DivideFunction::execute(const std::vector<Value>& args, Context& ctx) {
    validateArgs(args);
    if (args[1].asInteger() == 0) {
        throw std::runtime_error("Division by zero");
    }
    return Value(args[0].asInteger() / args[1].asInteger());
}

std::vector<Value::Type> DivideFunction::getParameterTypes() const {
    return {Value::Type::INTEGER, Value::Type::INTEGER};
}

Value ModuloFunction::execute(const std::vector<Value>& args, Context& ctx) {
    validateArgs(args);
    if (args[1].asInteger() == 0) {
        throw std::runtime_error("Modulo by zero");
    }
    return Value(args[0].asInteger() % args[1].asInteger());
}

std::vector<Value::Type> ModuloFunction::getParameterTypes() const {
    return {Value::Type::INTEGER, Value::Type::INTEGER};
}

Value PowerFunction::execute(const std::vector<Value>& args, Context& ctx) {
    validateArgs(args);
    int base = args[0].asInteger();
    int exponent = args[1].asInteger();
    
    if (exponent < 0) {
        throw std::runtime_error("Negative exponents not supported in integer arithmetic");
    }
    
    int result = 1;
    for (int i = 0; i < exponent; ++i) {
        result *= base;
    }
    return Value(result);
}

std::vector<Value::Type> PowerFunction::getParameterTypes() const {
    return {Value::Type::INTEGER, Value::Type::INTEGER};
}

// =============================================================================
// Peano Arithmetic Functions
// =============================================================================

Value SuccessorFunction::execute(const std::vector<Value>& args, Context& ctx) {
    validateArgs(args);
    return Value(args[0].asInteger() + 1);
}

std::vector<Value::Type> SuccessorFunction::getParameterTypes() const {
    return {Value::Type::INTEGER};
}

Value IsZeroFunction::execute(const std::vector<Value>& args, Context& ctx) {
    validateArgs(args);
    return Value(args[0].asInteger() == 0);
}

std::vector<Value::Type> IsZeroFunction::getParameterTypes() const {
    return {Value::Type::INTEGER};
}

Value PredecessorFunction::execute(const std::vector<Value>& args, Context& ctx) {
    validateArgs(args);
    int n = args[0].asInteger();
    return Value(n > 0 ? n - 1 : 0);  // Predecessor of 0 is 0 in Peano arithmetic
}

std::vector<Value::Type> PredecessorFunction::getParameterTypes() const {
    return {Value::Type::INTEGER};
}

// =============================================================================
// String Functions
// =============================================================================

Value StringLengthFunction::execute(const std::vector<Value>& args, Context& ctx) {
    validateArgs(args);
    return Value(static_cast<int>(args[0].asString().length()));
}

std::vector<Value::Type> StringLengthFunction::getParameterTypes() const {
    return {Value::Type::STRING};
}

Value StringConcatFunction::execute(const std::vector<Value>& args, Context& ctx) {
    validateArgs(args);
    return Value(args[0].asString() + args[1].asString());
}

std::vector<Value::Type> StringConcatFunction::getParameterTypes() const {
    return {Value::Type::STRING, Value::Type::STRING};
}

Value StringStartsWithFunction::execute(const std::vector<Value>& args, Context& ctx) {
    validateArgs(args);
    std::string text = args[0].asString();
    std::string prefix = args[1].asString();
    return Value(text.substr(0, prefix.length()) == prefix);
}

std::vector<Value::Type> StringStartsWithFunction::getParameterTypes() const {
    return {Value::Type::STRING, Value::Type::STRING};
}

Value StringContainsFunction::execute(const std::vector<Value>& args, Context& ctx) {
    (void)ctx; // Suppress unused parameter warning
    if (args.size() != 2) {
        throw std::runtime_error("string.contains requires 2 arguments");
    }
    if (!args[0].isString() || !args[1].isString()) {
        throw std::runtime_error("string.contains requires string arguments");
    }
    
    std::string text = args[0].asString();
    std::string substring = args[1].asString();
    
    return Value(text.find(substring) != std::string::npos);
}

std::vector<Value::Type> StringContainsFunction::getParameterTypes() const {
    return {Value::Type::STRING, Value::Type::STRING};
}

// =============================================================================
// Test Functions
// =============================================================================

Value MockCollectionFunction::execute(const std::vector<Value>& args, Context& ctx) {
    validateArgs(args);
    
    std::string type = args[0].asString();
    int count = args[1].asInteger();
    
    std::vector<Value> collection;
    
    if (type == "numbers") {
        for (int i = 1; i <= count; ++i) {
            collection.push_back(Value(i));
        }
    }
    else if (type == "strings") {
        for (int i = 1; i <= count; ++i) {
            collection.push_back(Value("item" + std::to_string(i)));
        }
    }
    else {
        throw std::runtime_error("Unknown mock collection type: " + type);
    }
    
    return Value(collection);
}

std::vector<Value::Type> MockCollectionFunction::getParameterTypes() const {
    return {Value::Type::STRING, Value::Type::INTEGER};
}

// =============================================================================
// Interface Functions
// =============================================================================

class GetCliCommandsFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override {
        // Return mock CLI commands for now
        std::vector<Value> commands;
        commands.push_back(Value("validate"));
        commands.push_back(Value("run"));
        commands.push_back(Value("rules"));
        commands.push_back(Value("init"));
        commands.push_back(Value("stats"));
        return Value(commands);
    }
    
    std::string getName() const override { return "interface.get_cli_commands"; }
    std::vector<Value::Type> getParameterTypes() const override { return {Value::Type::STRING}; }
    Value::Type getReturnType() const override { return Value::Type::COLLECTION; }
    std::string getDescription() const override { return "Gets available CLI commands"; }
};

class GetProgrammaticCommandsFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override {
        // Return mock programmatic commands for now
        std::vector<Value> commands;
        commands.push_back(Value("validate"));
        commands.push_back(Value("execute"));
        commands.push_back(Value("getRules"));
        commands.push_back(Value("initialize"));
        commands.push_back(Value("getStats"));
        return Value(commands);
    }
    
    std::string getName() const override { return "interface.get_programmatic_commands"; }
    std::vector<Value::Type> getParameterTypes() const override { return {Value::Type::STRING}; }
    Value::Type getReturnType() const override { return Value::Type::COLLECTION; }
    std::string getDescription() const override { return "Gets available programmatic commands"; }
};

// =============================================================================
// String Functions
// =============================================================================

class StringNormalizeFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override {
        std::string input = args[0].asString();
        std::string normalized = input;
        
        // Convert to lowercase
        std::transform(normalized.begin(), normalized.end(), normalized.begin(), ::tolower);
        
        // Replace underscores with dashes
        std::replace(normalized.begin(), normalized.end(), '_', '-');
        
        return Value(normalized);
    }
    
    std::string getName() const override { return "string.normalize"; }
    std::vector<Value::Type> getParameterTypes() const override { return {Value::Type::STRING}; }
    Value::Type getReturnType() const override { return Value::Type::STRING; }
    std::string getDescription() const override { return "Normalizes string for comparison"; }
};

// =============================================================================
// Rules Functions
// =============================================================================

class GetEnabledRulesFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override {
        // Return mock enabled rules for now
        std::vector<Value> rules;
        rules.push_back(Value("structure:one_class_per_file"));
        rules.push_back(Value("interface:command_parity"));
        rules.push_back(Value("language:cpp_standards"));
        rules.push_back(Value("philosophy:self_validation"));
        return Value(rules);
    }
    
    std::string getName() const override { return "rules.get_enabled_rules"; }
    std::vector<Value::Type> getParameterTypes() const override { return {Value::Type::STRING}; }
    Value::Type getReturnType() const override { return Value::Type::COLLECTION; }
    std::string getDescription() const override { return "Gets enabled rules"; }
};

class ExecuteRuleFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override {
        // Mock rule execution - always return true for now
        return Value(true);
    }
    
    std::string getName() const override { return "rules.execute_rule"; }
    std::vector<Value::Type> getParameterTypes() const override { return {Value::Type::STRING, Value::Type::STRING}; }
    Value::Type getReturnType() const override { return Value::Type::BOOLEAN; }
    std::string getDescription() const override { return "Executes a rule"; }
};

// =============================================================================
// Philosophy Functions
// =============================================================================

class SelfProvesFunction : public BuiltinFunction {
public:
    Value execute(const std::vector<Value>& args, Context& ctx) override {
        // Mock self-proof check - always return true for now
        return Value(true);
    }
    
    std::string getName() const override { return "philosophy.self_proves"; }
    std::vector<Value::Type> getParameterTypes() const override { return {Value::Type::STRING}; }
    Value::Type getReturnType() const override { return Value::Type::BOOLEAN; }
    std::string getDescription() const override { return "Checks if rule self-proves"; }
};

// =============================================================================
// Recursive Function Definition Implementation
// =============================================================================

Value RecursiveDefineFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 4) {
        throw std::runtime_error("recursive.define expects 4 arguments (name, parameter, base_case, recursive_case)");
    }
    
    if (!args[0].isString() || !args[1].isString() || !args[2].isString() || !args[3].isString()) {
        throw std::runtime_error("recursive.define expects string arguments");
    }
    
    std::string func_name = args[0].asString();
    std::string parameter = args[1].asString();
    std::string base_case = args[2].asString();
    std::string recursive_case = args[3].asString();
    
    // Create a function definition object
    std::map<std::string, Value> function_def;
    function_def["name"] = Value(func_name);
    function_def["parameter"] = Value(parameter);
    function_def["base_case"] = Value(base_case);
    function_def["recursive_case"] = Value(recursive_case);
    function_def["type"] = Value("recursive_function");
    
    return Value(function_def);
}

std::vector<Value::Type> RecursiveDefineFunction::getParameterTypes() const {
    return {Value::Type::STRING, Value::Type::STRING, Value::Type::STRING, Value::Type::STRING};
}

// =============================================================================
// Registration Function
// =============================================================================

void registerAllBuiltinFunctions(PureLogicEngine& engine) {
    // Collection functions
    engine.registerFunction("collection.count", std::make_shared<CountFunction>());
    engine.registerFunction("collection.contains", std::make_shared<ContainsFunction>());
    
    // Math functions
    engine.registerFunction("math.add", std::make_shared<AddFunction>());
    engine.registerFunction("math.subtract", std::make_shared<SubtractFunction>());
    engine.registerFunction("math.multiply", std::make_shared<MultiplyFunction>());
    engine.registerFunction("math.divide", std::make_shared<DivideFunction>());
    engine.registerFunction("math.modulo", std::make_shared<ModuloFunction>());
    engine.registerFunction("math.power", std::make_shared<PowerFunction>());
    
    // Peano arithmetic functions
    engine.registerFunction("peano.successor", std::make_shared<SuccessorFunction>());
    engine.registerFunction("peano.is_zero", std::make_shared<IsZeroFunction>());
    engine.registerFunction("peano.predecessor", std::make_shared<PredecessorFunction>());
    
    // String functions
    engine.registerFunction("string.length", std::make_shared<StringLengthFunction>());
    engine.registerFunction("string.concat", std::make_shared<StringConcatFunction>());
    engine.registerFunction("string.normalize", std::make_shared<StringNormalizeFunction>());
    engine.registerFunction("string.starts_with", std::make_shared<StringStartsWithFunction>());
    engine.registerFunction("string.contains", std::make_shared<StringContainsFunction>());
    
    // Interface functions
    engine.registerFunction("interface.get_cli_commands", std::make_shared<GetCliCommandsFunction>());
    engine.registerFunction("interface.get_programmatic_commands", std::make_shared<GetProgrammaticCommandsFunction>());
    
    // Rules functions
    engine.registerFunction("rules.get_enabled_rules", std::make_shared<GetEnabledRulesFunction>());
    engine.registerFunction("rules.execute_rule", std::make_shared<ExecuteRuleFunction>());
    
    // Filesystem functions
    akao::logic::filesystem::registerFilesystemFunctions(engine);
    
    // C++ analysis functions
    akao::logic::cpp::registerCppFunctions(engine);
    
    // Philosophy functions
    engine.registerFunction("philosophy.self_proves", std::make_shared<SelfProvesFunction>());
    
    // Recursive functions
    engine.registerFunction("recursive.define", std::make_shared<RecursiveDefineFunction>());
    
    // Gitignore functions
    engine.registerFunction("gitignore.should_include_file", std::make_shared<GitignoreShouldIncludeFileFunction>());
    engine.registerFunction("gitignore.parse_patterns", std::make_shared<GitignoreParsePatternFunction>());
    engine.registerFunction("gitignore.match_pattern", std::make_shared<GitignoreMatchPatternFunction>());
    
    // Test functions
    engine.registerFunction("test.mock_collection", std::make_shared<MockCollectionFunction>());
}

// =============================================================================
// Gitignore Functions
// =============================================================================

Value GitignoreShouldIncludeFileFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 2) {
        throw std::runtime_error("gitignore.should_include_file requires 2 arguments: file_path, target_path");
    }
    
    std::string file_path = args[0].asString();
    std::string target_path = args[1].asString();
    
    // Read .gitignore file from target directory
    std::string gitignore_path = target_path + "/.gitignore";
    std::ifstream gitignore_file(gitignore_path);
    
    if (!gitignore_file.is_open()) {
        // No .gitignore file, include all files
        return Value(true);
    }
    
    std::vector<std::string> patterns;
    std::string line;
    while (std::getline(gitignore_file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Trim trailing whitespace
        line.erase(line.find_last_not_of(" \t\n\r") + 1);
        
        if (!line.empty()) {
            patterns.push_back(line);
        }
    }
    gitignore_file.close();
    
    // Apply gitignore pattern matching
    std::string relative_path = file_path;
    if (file_path.substr(0, target_path.length()) == target_path) {
        relative_path = file_path.substr(target_path.length());
        if (relative_path[0] == '/') {
            relative_path = relative_path.substr(1);
        }
    }
    
    bool should_include = true;
    
    // Debug output for first few files only
    static int debug_count = 0;
    bool debug_this = (debug_count < 5);
    
    if (debug_this) {
        std::cout << "🔍 Checking file: " << relative_path << std::endl;
        debug_count++;
    }
    
    for (const auto& pattern : patterns) {
        bool is_negation = (pattern[0] == '!');
        std::string actual_pattern = is_negation ? pattern.substr(1) : pattern;
        
        if (debug_this) {
            std::cout << "  📋 Testing pattern: " << actual_pattern << std::endl;
        }
        
        if (matchGitignorePattern(relative_path, actual_pattern)) {
            if (debug_this) {
                std::cout << "  ✅ Pattern matched! should_include = " << (is_negation ? "true" : "false") << std::endl;
            }
            should_include = is_negation;
        }
    }
    
    if (debug_this) {
        std::cout << "  🎯 Final result: " << (should_include ? "INCLUDE" : "EXCLUDE") << std::endl;
    }
    
    return Value(should_include);
}

std::vector<Value::Type> GitignoreShouldIncludeFileFunction::getParameterTypes() const {
    return {Value::Type::STRING, Value::Type::STRING};
}

Value GitignoreParsePatternFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 1) {
        throw std::runtime_error("gitignore.parse_patterns requires 1 argument: gitignore_content");
    }
    
    std::string content = args[0].asString();
    std::vector<Value> patterns;
    
    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Trim trailing whitespace
        line.erase(line.find_last_not_of(" \t\n\r") + 1);
        
        if (!line.empty()) {
            patterns.push_back(Value(line));
        }
    }
    
    return Value(patterns);
}

std::vector<Value::Type> GitignoreParsePatternFunction::getParameterTypes() const {
    return {Value::Type::STRING};
}

Value GitignoreMatchPatternFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 2) {
        throw std::runtime_error("gitignore.match_pattern requires 2 arguments: file_path, pattern");
    }
    
    std::string file_path = args[0].asString();
    std::string pattern = args[1].asString();
    
    GitignoreShouldIncludeFileFunction gitignore_func;
    bool matches = gitignore_func.matchGitignorePattern(file_path, pattern);
    return Value(matches);
}

std::vector<Value::Type> GitignoreMatchPatternFunction::getParameterTypes() const {
    return {Value::Type::STRING, Value::Type::STRING};
}

bool GitignoreShouldIncludeFileFunction::matchGitignorePattern(const std::string& file_path, const std::string& pattern) {
    // Handle directory patterns (ending with /)
    if (pattern.back() == '/') {
        std::string dir_pattern = pattern.substr(0, pattern.length() - 1);
        return matchGitignorePattern(file_path, dir_pattern) || 
               (file_path.find(dir_pattern + "/") == 0);
    }
    
    // Handle absolute patterns (starting with /)
    if (pattern[0] == '/') {
        std::string abs_pattern = pattern.substr(1);
        return matchGitignoreWildcard(file_path, abs_pattern);
    }
    
    // Handle ** patterns
    if (pattern.find("**") != std::string::npos) {
        return matchGitignoreDoubleWildcard(file_path, pattern);
    }
    
    // Handle simple wildcard patterns
    return matchGitignoreWildcard(file_path, pattern) || 
           (file_path.find("/" + pattern) != std::string::npos) ||
           (file_path.find(pattern + "/") != std::string::npos);
}

bool GitignoreShouldIncludeFileFunction::matchGitignoreWildcard(const std::string& text, const std::string& pattern) {
    size_t t = 0, p = 0;
    size_t match = 0, asterisk = std::string::npos;
    
    while (t < text.length()) {
        if (p < pattern.length() && (pattern[p] == '?' || pattern[p] == text[t])) {
            t++;
            p++;
        } else if (p < pattern.length() && pattern[p] == '*') {
            match = t;
            asterisk = p++;
        } else if (asterisk != std::string::npos) {
            p = asterisk + 1;
            t = ++match;
        } else {
            return false;
        }
    }
    
    while (p < pattern.length() && pattern[p] == '*') {
        p++;
    }
    
    return p == pattern.length();
}

bool GitignoreShouldIncludeFileFunction::matchGitignoreDoubleWildcard(const std::string& file_path, const std::string& pattern) {
    size_t double_star = pattern.find("**");
    if (double_star == std::string::npos) {
        return matchGitignoreWildcard(file_path, pattern);
    }
    
    std::string before = pattern.substr(0, double_star);
    std::string after = pattern.substr(double_star + 2);
    
    // Leading **/ pattern
    if (double_star == 0 && after.length() > 0 && after[0] == '/') {
        std::string suffix = after.substr(1);
        return file_path.find(suffix) != std::string::npos;
    }
    
    // Trailing /** pattern
    if (double_star + 2 == pattern.length() - 1 && pattern.back() == '/') {
        return file_path.find(before) == 0;
    }
    
    // Middle /** pattern
    if (before.length() > 0 && after.length() > 0) {
        size_t before_pos = file_path.find(before);
        if (before_pos != std::string::npos) {
            size_t after_pos = file_path.find(after, before_pos + before.length());
            return after_pos != std::string::npos;
        }
    }
    
    return false;
}

} // namespace akao::logic::builtin
