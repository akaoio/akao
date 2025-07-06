#include "../core/engine/logic/pure_logic_engine.hpp"
#include <iostream>
#include <cassert>

void testFilesystemFunctions() {
    std::cout << "Testing filesystem functions..." << std::endl;
    
    akao::logic::PureLogicEngine engine;
    akao::logic::Context ctx;
    
    // Test current_directory
    {
        YAML::Node logic;
        logic["function"] = "filesystem.current_directory";
        
        auto result = engine.evaluate(logic, ctx);
        assert(result.getType() == akao::logic::Value::Type::STRING);
        std::cout << "  ✓ current_directory returns: " << result.asString() << std::endl;
    }
    
    // Test get_files on current directory
    {
        YAML::Node logic;
        logic["function"] = "filesystem.get_files";
        logic["argument"]["literal"] = ".";
        
        auto result = engine.evaluate(logic, ctx);
        assert(result.getType() == akao::logic::Value::Type::COLLECTION);
        std::cout << "  ✓ get_files returns " << result.asCollection().size() << " files" << std::endl;
    }
    
    // Test has_extension
    {
        YAML::Node logic;
        logic["function"] = "filesystem.has_extension";
        logic["arguments"] = YAML::Node(YAML::NodeType::Sequence);
        logic["arguments"].push_back(YAML::Node());
        logic["arguments"][0]["literal"] = "test.cpp";
        logic["arguments"].push_back(YAML::Node());
        logic["arguments"][1]["literal"] = ".cpp";
        
        auto result = engine.evaluate(logic, ctx);
        assert(result.getType() == akao::logic::Value::Type::BOOLEAN);
        assert(result.asBoolean() == true);
        std::cout << "  ✓ has_extension works correctly" << std::endl;
    }
    
    std::cout << "✓ All filesystem function tests passed!" << std::endl;
}

void testCppFunctions() {
    std::cout << "Testing C++ analysis functions..." << std::endl;
    
    akao::logic::PureLogicEngine engine;
    akao::logic::Context ctx;
    
    std::string testCode = R"(
        #include <iostream>
        #include <vector>
        
        class TestClass {
        public:
            void testMethod();
        };
        
        struct TestStruct {
            int value;
        };
        
        int main() {
            return 0;
        }
        
        void freeFunction() {
            // Do something
        }
    )";
    
    // Test extract_classes
    {
        YAML::Node logic;
        logic["function"] = "cpp.extract_classes";
        logic["argument"]["literal"] = testCode;
        
        auto result = engine.evaluate(logic, ctx);
        assert(result.getType() == akao::logic::Value::Type::COLLECTION);
        auto classes = result.asCollection();
        assert(classes.size() >= 2); // Should find TestClass and TestStruct
        std::cout << "  ✓ extract_classes found " << classes.size() << " classes" << std::endl;
    }
    
    // Test count_classes
    {
        YAML::Node logic;
        logic["function"] = "cpp.count_classes";
        logic["argument"]["literal"] = testCode;
        
        auto result = engine.evaluate(logic, ctx);
        assert(result.getType() == akao::logic::Value::Type::INTEGER);
        assert(result.asInteger() >= 2);
        std::cout << "  ✓ count_classes returned " << result.asInteger() << std::endl;
    }
    
    // Test get_includes
    {
        YAML::Node logic;
        logic["function"] = "cpp.get_includes";
        logic["argument"]["literal"] = testCode;
        
        auto result = engine.evaluate(logic, ctx);
        assert(result.getType() == akao::logic::Value::Type::COLLECTION);
        auto includes = result.asCollection();
        assert(includes.size() >= 2); // Should find iostream and vector
        std::cout << "  ✓ get_includes found " << includes.size() << " includes" << std::endl;
    }
    
    // Test has_main
    {
        YAML::Node logic;
        logic["function"] = "cpp.has_main";
        logic["argument"]["literal"] = testCode;
        
        auto result = engine.evaluate(logic, ctx);
        assert(result.getType() == akao::logic::Value::Type::BOOLEAN);
        assert(result.asBoolean() == true);
        std::cout << "  ✓ has_main correctly detected main function" << std::endl;
    }
    
    std::cout << "✓ All C++ analysis function tests passed!" << std::endl;
}

void testMetaFunctions() {
    std::cout << "Testing meta-logic functions..." << std::endl;
    
    akao::logic::PureLogicEngine engine;
    akao::logic::Context ctx;
    
    // Create test object in context
    std::map<std::string, akao::logic::Value> testObj;
    testObj["name"] = akao::logic::Value("test");
    testObj["value"] = akao::logic::Value(42);
    ctx.bindVariable("testObj", akao::logic::Value(testObj));
    
    // Test has_field
    {
        YAML::Node logic;
        logic["function"] = "has_field";
        logic["arguments"] = YAML::Node(YAML::NodeType::Sequence);
        logic["arguments"].push_back(YAML::Node());
        logic["arguments"][0]["var"] = "testObj";
        logic["arguments"].push_back(YAML::Node());
        logic["arguments"][1]["literal"] = "name";
        
        auto result = engine.evaluate(logic, ctx);
        assert(result.getType() == akao::logic::Value::Type::BOOLEAN);
        assert(result.asBoolean() == true);
        std::cout << "  ✓ has_field works correctly" << std::endl;
    }
    
    // Test get_field
    {
        YAML::Node logic;
        logic["function"] = "get_field";
        logic["arguments"] = YAML::Node(YAML::NodeType::Sequence);
        logic["arguments"].push_back(YAML::Node());
        logic["arguments"][0]["var"] = "testObj";
        logic["arguments"].push_back(YAML::Node());
        logic["arguments"][1]["literal"] = "value";
        
        auto result = engine.evaluate(logic, ctx);
        assert(result.getType() == akao::logic::Value::Type::INTEGER);
        assert(result.asInteger() == 42);
        std::cout << "  ✓ get_field works correctly" << std::endl;
    }
    
    // Test is_well_formed (simplified)
    {
        // Create a simple well-formed expression object
        std::map<std::string, akao::logic::Value> expr;
        expr["operator"] = akao::logic::Value("equals");
        expr["left"] = akao::logic::Value(1);
        expr["right"] = akao::logic::Value(1);
        ctx.bindVariable("testExpr", akao::logic::Value(expr));
        
        YAML::Node logic;
        logic["function"] = "logic.is_well_formed";
        logic["argument"]["var"] = "testExpr";
        
        auto result = engine.evaluate(logic, ctx);
        assert(result.getType() == akao::logic::Value::Type::BOOLEAN);
        assert(result.asBoolean() == true);
        std::cout << "  ✓ is_well_formed works correctly" << std::endl;
    }
    
    std::cout << "✓ All meta-logic function tests passed!" << std::endl;
}

void testIntegration() {
    std::cout << "Testing domain function integration..." << std::endl;
    
    akao::logic::PureLogicEngine engine;
    akao::logic::Context ctx;
    
    // Test simple integration: check if we have .cpp files
    {
        YAML::Node logic;
        logic["operator"] = "greater_than";
        logic["left"]["function"] = "collection.count";
        logic["left"]["argument"]["function"] = "filesystem.get_cpp_files";
        logic["left"]["argument"]["argument"]["literal"] = ".";
        logic["right"]["literal"] = 0;
        
        auto result = engine.evaluate(logic, ctx);
        assert(result.getType() == akao::logic::Value::Type::BOOLEAN);
        std::cout << "  ✓ Integration test - has .cpp files: " << (result.asBoolean() ? "true" : "false") << std::endl;
    }
    
    // Test filesystem and meta integration
    {
        // Create test object and check has_field
        std::map<std::string, akao::logic::Value> testObj;
        testObj["path"] = akao::logic::Value("/workspaces/akao");
        ctx.bindVariable("currentDir", akao::logic::Value(testObj));
        
        YAML::Node logic;
        logic["operator"] = "and";
        logic["left"]["function"] = "has_field";
        logic["left"]["arguments"] = YAML::Node(YAML::NodeType::Sequence);
        logic["left"]["arguments"].push_back(YAML::Node());
        logic["left"]["arguments"][0]["var"] = "currentDir";
        logic["left"]["arguments"].push_back(YAML::Node());
        logic["left"]["arguments"][1]["literal"] = "path";
        
        logic["right"]["operator"] = "greater_than";
        logic["right"]["left"]["function"] = "string.length";
        logic["right"]["left"]["argument"]["function"] = "filesystem.current_directory";
        logic["right"]["right"]["literal"] = 5;
        
        auto result = engine.evaluate(logic, ctx);
        assert(result.getType() == akao::logic::Value::Type::BOOLEAN);
        std::cout << "  ✓ Complex integration test result: " << (result.asBoolean() ? "true" : "false") << std::endl;
    }
    
    std::cout << "✓ All integration tests passed!" << std::endl;
}

int main() {
    std::cout << "=== Testing Phase 2: Domain Functions ===" << std::endl;
    
    try {
        testFilesystemFunctions();
        testCppFunctions();
        testMetaFunctions();
        testIntegration();
        
        std::cout << "\n🎉 ALL PHASE 2 TESTS PASSED! 🎉" << std::endl;
        std::cout << "Domain functions are working correctly." << std::endl;
        std::cout << "Ready to proceed to Phase 3: Rule System" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
}
