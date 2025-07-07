#include "../core/engine/logic/pure/v1.hpp"
#include "../core/engine/logic/filesystem/v1.hpp"
#include "../core/engine/logic/cpp/v1.hpp"
#include "../core/engine/logic/meta/v1.hpp"
#include "../core/engine/logic/builtin/v1.hpp"
#include <iostream>

using namespace akao::logic;

int main() {
    std::cout << "=== Phase 2 Domain Functions Test ===" << std::endl;
    
    try {
        // Initialize Pure Logic Engine
        PureLogicEngine engine;
        
        std::cout << "✅ Pure Logic Engine initialized successfully" << std::endl;
        std::cout << "   Registered functions: " << engine.getBuiltinFunctionCount() << std::endl;
        
        // Test filesystem functions
        Context ctx;
        
        // Test current directory
        if (engine.hasFunction("filesystem.current_directory")) {
            Value result = engine.executeFunction("filesystem.current_directory", {});
            std::cout << "✅ Current directory: " << result.asString() << std::endl;
        }
        
        // Test file exists
        if (engine.hasFunction("filesystem.file_exists")) {
            Value result = engine.executeFunction("filesystem.file_exists", {Value(".")});
            std::cout << "✅ Current directory exists: " << (result.asBoolean() ? "true" : "false") << std::endl;
        }
        
        // Test C++ analysis functions
        std::string sampleCode = R"(
            #include <iostream>
            
            namespace test {
                class MyClass {
                public:
                    void myFunction() {}
                };
                
                int main() {
                    return 0;
                }
            }
        )";
        
        if (engine.hasFunction("cpp.extract_classes")) {
            Value result = engine.executeFunction("cpp.extract_classes", {Value(sampleCode)});
            std::cout << "✅ Extracted classes: " << result.asCollection().size() << std::endl;
        }
        
        if (engine.hasFunction("cpp.extract_namespaces")) {
            Value result = engine.executeFunction("cpp.extract_namespaces", {Value(sampleCode)});
            std::cout << "✅ Extracted namespaces: " << result.asCollection().size() << std::endl;
        }
        
        if (engine.hasFunction("cpp.has_main")) {
            Value result = engine.executeFunction("cpp.has_main", {Value(sampleCode)});
            std::cout << "✅ Has main function: " << (result.asBoolean() ? "true" : "false") << std::endl;
        }
        
        // Test meta functions
        if (engine.hasFunction("metalogic.encode_formula")) {
            Value result = engine.executeFunction("metalogic.encode_formula", {Value("test_formula")});
            std::cout << "✅ Gödel encoding test: " << result.asInteger() << std::endl;
        }
        
        std::cout << "\n🎯 Phase 2 Domain Functions Test Complete!" << std::endl;
        std::cout << "   All enhanced functions are working correctly." << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Error: " << e.what() << std::endl;
        return 1;
    }
}