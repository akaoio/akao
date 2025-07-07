#include "v1.hpp"
#include <algorithm>
#include <sstream>

namespace akao::logic::cpp {

namespace {
    // Helper function to remove comments from C++ code
    std::string removeComments(const std::string& code) {
        std::string result;
        bool inSingleLineComment = false;
        bool inMultiLineComment = false;
        bool inString = false;
        bool inChar = false;
        
        for (size_t i = 0; i < code.length(); ++i) {
            char c = code[i];
            char next = (i + 1 < code.length()) ? code[i + 1] : '\0';
            
            if (inString) {
                result += c;
                if (c == '"' && (i == 0 || code[i-1] != '\\')) {
                    inString = false;
                }
            } else if (inChar) {
                result += c;
                if (c == '\'' && (i == 0 || code[i-1] != '\\')) {
                    inChar = false;
                }
            } else if (inSingleLineComment) {
                if (c == '\n') {
                    inSingleLineComment = false;
                    result += c;
                }
            } else if (inMultiLineComment) {
                if (c == '*' && next == '/') {
                    inMultiLineComment = false;
                    ++i; // Skip the '/'
                }
            } else {
                if (c == '"') {
                    inString = true;
                    result += c;
                } else if (c == '\'') {
                    inChar = true;
                    result += c;
                } else if (c == '/' && next == '/') {
                    inSingleLineComment = true;
                    ++i; // Skip the second '/'
                } else if (c == '/' && next == '*') {
                    inMultiLineComment = true;
                    ++i; // Skip the '*'
                } else {
                    result += c;
                }
            }
        }
        
        return result;
    }
}

// ExtractClassesFunction
Value ExtractClassesFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 1) {
        throw std::runtime_error("cpp.extract_classes expects 1 argument (code string)");
    }
    
    if (args[0].getType() != Value::Type::STRING) {
        throw std::runtime_error("cpp.extract_classes expects string argument");
    }
    
    std::string code = removeComments(args[0].asString());
    std::vector<Value> classes;
    
    // Regex to match class declarations
    std::regex classRegex(R"(\bclass\s+(\w+)(?:\s*:\s*[^{]+)?\s*\{)");
    std::sregex_iterator iter(code.begin(), code.end(), classRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        classes.emplace_back(match[1].str());
    }
    
    // Also match struct declarations
    std::regex structRegex(R"(\bstruct\s+(\w+)(?:\s*:\s*[^{]+)?\s*\{)");
    iter = std::sregex_iterator(code.begin(), code.end(), structRegex);
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        classes.emplace_back(match[1].str());
    }
    
    return Value(classes);
}

std::vector<Value::Type> ExtractClassesFunction::getParameterTypes() const {
    return {Value::Type::STRING};
}

// CountClassesFunction
Value CountClassesFunction::execute(const std::vector<Value>& args, Context& ctx) {
    ExtractClassesFunction extractor;
    Value result = extractor.execute(args, ctx);
    return Value(static_cast<int>(result.asCollection().size()));
}

std::vector<Value::Type> CountClassesFunction::getParameterTypes() const {
    return {Value::Type::STRING};
}

// ExtractFunctionsFunction
Value ExtractFunctionsFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 1) {
        throw std::runtime_error("cpp.extract_functions expects 1 argument (code string)");
    }
    
    if (args[0].getType() != Value::Type::STRING) {
        throw std::runtime_error("cpp.extract_functions expects string argument");
    }
    
    std::string code = removeComments(args[0].asString());
    std::vector<Value> functions;
    
    // Regex to match function declarations/definitions
    // This is a simplified regex - a full C++ parser would be more accurate
    std::regex functionRegex(R"((?:^|\n)\s*(?:(?:static|virtual|inline|explicit|const|constexpr)\s+)*(?:\w+(?:\s*\*|\s*&)?(?:\s*const)?\s+)+(\w+)\s*\([^)]*\)\s*(?:const\s*)?(?:override\s*)?(?:final\s*)?(?:\s*->\s*\w+\s*)?(?:\s*\{|\s*;))");
    
    std::sregex_iterator iter(code.begin(), code.end(), functionRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        std::string funcName = match[1].str();
        
        // Skip common C++ keywords that might be matched
        if (funcName != "if" && funcName != "for" && funcName != "while" && 
            funcName != "switch" && funcName != "return" && funcName != "break" &&
            funcName != "continue" && funcName != "throw" && funcName != "try") {
            functions.emplace_back(funcName);
        }
    }
    
    return Value(functions);
}

std::vector<Value::Type> ExtractFunctionsFunction::getParameterTypes() const {
    return {Value::Type::STRING};
}

// GetIncludesFunction
Value GetIncludesFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 1) {
        throw std::runtime_error("cpp.get_includes expects 1 argument (code string)");
    }
    
    if (args[0].getType() != Value::Type::STRING) {
        throw std::runtime_error("cpp.get_includes expects string argument");
    }
    
    std::string code = args[0].asString();
    std::vector<Value> includes;
    
    // Regex to match #include statements
    std::regex includeRegex(R"(^\s*#include\s*[<"]([^>"]+)[>"])");
    std::stringstream ss(code);
    std::string line;
    
    while (std::getline(ss, line)) {
        std::smatch match;
        if (std::regex_search(line, match, includeRegex)) {
            includes.emplace_back(match[1].str());
        }
    }
    
    return Value(includes);
}

std::vector<Value::Type> GetIncludesFunction::getParameterTypes() const {
    return {Value::Type::STRING};
}

// HasMainFunction
Value HasMainFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 1) {
        throw std::runtime_error("cpp.has_main expects 1 argument (code string)");
    }
    
    if (args[0].getType() != Value::Type::STRING) {
        throw std::runtime_error("cpp.has_main expects string argument");
    }
    
    std::string code = removeComments(args[0].asString());
    
    // Regex to match main function
    std::regex mainRegex(R"(\bint\s+main\s*\([^)]*\)\s*(?:\{|;))");
    
    return Value(std::regex_search(code, mainRegex));
}

std::vector<Value::Type> HasMainFunction::getParameterTypes() const {
    return {Value::Type::STRING};
}

// Phase 2: Enhanced C++ analysis functions implementation
Value CountFunctionsFunction::execute(const std::vector<Value>& args, Context& ctx) {
    ExtractFunctionsFunction extractor;
    Value result = extractor.execute(args, ctx);
    return Value(static_cast<int>(result.asCollection().size()));
}

std::vector<Value::Type> CountFunctionsFunction::getParameterTypes() const {
    return {Value::Type::STRING};
}

Value ExtractNamespacesFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 1) {
        throw std::runtime_error("cpp.extract_namespaces expects 1 argument (code string)");
    }
    
    if (args[0].getType() != Value::Type::STRING) {
        throw std::runtime_error("cpp.extract_namespaces expects string argument");
    }
    
    std::string code = removeComments(args[0].asString());
    std::vector<Value> namespaces;
    
    // Regex to match namespace declarations
    std::regex namespaceRegex(R"(\bnamespace\s+(\w+)(?:\s*\{|\s*=))");
    std::sregex_iterator iter(code.begin(), code.end(), namespaceRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        namespaces.emplace_back(match[1].str());
    }
    
    return Value(namespaces);
}

std::vector<Value::Type> ExtractNamespacesFunction::getParameterTypes() const {
    return {Value::Type::STRING};
}

Value HasClassFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 2) {
        throw std::runtime_error("cpp.has_class expects 2 arguments (code string, class name)");
    }
    
    if (args[0].getType() != Value::Type::STRING || args[1].getType() != Value::Type::STRING) {
        throw std::runtime_error("cpp.has_class expects string arguments");
    }
    
    std::string code = removeComments(args[0].asString());
    std::string className = args[1].asString();
    
    // Create regex pattern to find the specific class
    std::string pattern = R"(\b(?:class|struct)\s+)" + className + R"((?:\s*:\s*[^{]+)?\s*\{)";
    std::regex classRegex(pattern);
    
    return Value(std::regex_search(code, classRegex));
}

std::vector<Value::Type> HasClassFunction::getParameterTypes() const {
    return {Value::Type::STRING, Value::Type::STRING};
}

Value GetLineCountFunction::execute(const std::vector<Value>& args, Context& ctx) {
    if (args.size() != 1) {
        throw std::runtime_error("cpp.get_line_count expects 1 argument (code string)");
    }
    
    if (args[0].getType() != Value::Type::STRING) {
        throw std::runtime_error("cpp.get_line_count expects string argument");
    }
    
    std::string code = args[0].asString();
    int lineCount = std::count(code.begin(), code.end(), '\n') + 1;
    
    return Value(lineCount);
}

std::vector<Value::Type> GetLineCountFunction::getParameterTypes() const {
    return {Value::Type::STRING};
}

// Registration function
void registerCppFunctions(PureLogicEngine& engine) {
    engine.registerFunction(std::make_unique<ExtractClassesFunction>());
    engine.registerFunction(std::make_unique<CountClassesFunction>());
    engine.registerFunction(std::make_unique<ExtractFunctionsFunction>());
    engine.registerFunction(std::make_unique<GetIncludesFunction>());
    engine.registerFunction(std::make_unique<HasMainFunction>());
    
    // Phase 2: Enhanced functions
    engine.registerFunction(std::make_unique<CountFunctionsFunction>());
    engine.registerFunction(std::make_unique<ExtractNamespacesFunction>());
    engine.registerFunction(std::make_unique<HasClassFunction>());
    engine.registerFunction(std::make_unique<GetLineCountFunction>());
}

} // namespace akao::logic::cpp
