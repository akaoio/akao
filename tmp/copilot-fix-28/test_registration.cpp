#include "../core/engine/logic/pure/v1.hpp"
#include <iostream>

using namespace akao::logic;

int main() {
    std::cout << "=== Phase 2 Function Registration Test ===" << std::endl;
    
    try {
        // Initialize Pure Logic Engine
        PureLogicEngine engine;
        
        std::cout << "✅ Pure Logic Engine initialized successfully" << std::endl;
        std::cout << "   Registered functions: " << engine.getBuiltinFunctionCount() << std::endl;
        
        // Test specific function presence
        std::vector<std::string> expectedFunctions = {
            // Phase 1 functions
            "filesystem.get_files",
            "filesystem.get_cpp_files", 
            "filesystem.current_directory",
            "cpp.extract_classes",
            "cpp.count_classes",
            "cpp.has_main",
            "metalogic.encode_formula",
            
            // Phase 2 enhanced functions
            "filesystem.list_subdirs",
            "filesystem.file_exists",
            "filesystem.is_directory",
            "filesystem.get_filename",
            "cpp.count_functions",
            "cpp.extract_namespaces",
            "cpp.has_class",
            "cpp.get_line_count"
        };
        
        int found = 0;
        for (const auto& funcName : expectedFunctions) {
            if (engine.hasFunction(funcName)) {
                std::cout << "✅ " << funcName << " - registered" << std::endl;
                found++;
            } else {
                std::cout << "❌ " << funcName << " - NOT FOUND" << std::endl;
            }
        }
        
        std::cout << "\n🎯 Phase 2 Registration Summary:" << std::endl;
        std::cout << "   Found: " << found << "/" << expectedFunctions.size() << " expected functions" << std::endl;
        std::cout << "   Total registered: " << engine.getBuiltinFunctionCount() << " functions" << std::endl;
        
        if (found == expectedFunctions.size()) {
            std::cout << "✅ All Phase 2 domain functions successfully registered!" << std::endl;
            return 0;
        } else {
            std::cout << "❌ Some functions missing" << std::endl;
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Error: " << e.what() << std::endl;
        return 1;
    }
}