#!/bin/bash

# Phase C: Integration Testing with Existing YAML Files
# Tests enhanced YAML library against real AKAO repository files

echo "=== Phase C: Integration Testing ==="

# Compile the enhanced YAML library
cd /home/runner/work/akao/akao
echo "Compiling enhanced YAML library..."
tests/yaml/compile_yaml_test.sh > /dev/null 2>&1

if [ $? -ne 0 ]; then
    echo "❌ Failed to compile enhanced YAML library"
    exit 1
fi

echo "✅ Enhanced YAML library compiled successfully"

# Create integration test program
cat > /tmp/integration_test.cpp << 'EOF'
#include "core/engine/parser/yaml_parser/v1.hpp"
#include "core/engine/parser/yaml_validator/v1.hpp"
#include <iostream>
#include <filesystem>

using namespace akao::core::engine::parser;

int main() {
    YamlParser parser;
    YamlValidator validator(YamlValidator::ValidationLevel::AKAO_SPEC);
    
    int total_files = 0;
    int successful_parses = 0;
    int validation_passes = 0;
    
    std::cout << "🔍 Testing existing YAML files in repository...\n" << std::endl;
    
    // Test files from different directories
    std::vector<std::string> test_files = {
        "rulesets/structure/cpp_standards/v1.yaml",
        "rulesets/index.yaml",
        "philosophies/automation/updates/v1.yaml",
        "philosophies/testing/coverage/v1.yaml",
        "philosophies/measurement/observability/v1.yaml",
        "philosophies/validation/universal/v1.yaml"
    };
    
    for (const auto& file_path : test_files) {
        total_files++;
        std::cout << "📄 Testing: " << file_path << std::endl;
        
        try {
            auto root = parser.parseFile(file_path);
            if (root) {
                successful_parses++;
                std::cout << "   ✅ Parsing: SUCCESS" << std::endl;
                
                // Determine file type and validate accordingly
                auto validation_result = validator.validate(root);
                
                if (file_path.find("philosophy") != std::string::npos) {
                    validation_result = validator.validateAkaoPhilosophy(root);
                } else if (file_path.find("ruleset") != std::string::npos) {
                    validation_result = validator.validateAkaoRuleset(root);
                }
                
                if (validation_result.is_valid) {
                    validation_passes++;
                    std::cout << "   ✅ Validation: PASS" << std::endl;
                } else {
                    std::cout << "   ⚠️  Validation: " << validation_result.errors.size() 
                              << " errors, " << validation_result.warnings.size() 
                              << " warnings" << std::endl;
                }
                
                // Show some content
                if (root->hasKey("metadata") && (*root)["metadata"]->hasKey("id")) {
                    std::cout << "   📋 ID: " << (*(*root)["metadata"])["id"]->asString() << std::endl;
                }
                
            } else {
                std::cout << "   ❌ Parsing: FAILED - NULL root" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "   ❌ Exception: " << e.what() << std::endl;
        }
        
        std::cout << std::endl;
    }
    
    // Summary
    std::cout << "=== Integration Test Summary ===" << std::endl;
    std::cout << "📊 Total files tested: " << total_files << std::endl;
    std::cout << "✅ Successful parses: " << successful_parses << "/" << total_files 
              << " (" << (successful_parses * 100 / total_files) << "%)" << std::endl;
    std::cout << "✅ Validation passes: " << validation_passes << "/" << total_files 
              << " (" << (validation_passes * 100 / total_files) << "%)" << std::endl;
    
    if (successful_parses == total_files) {
        std::cout << "\n🎉 Phase C: Integration Testing COMPLETE - All files parsed successfully!" << std::endl;
        return 0;
    } else {
        std::cout << "\n⚠️  Phase C: Some files failed to parse" << std::endl;
        return 1;
    }
}
EOF

# Compile integration test
echo "Compiling integration test..."
mkdir -p /tmp/yaml_build
g++ -std=c++17 -I. /tmp/integration_test.cpp \
    /tmp/yaml_build/yaml_node.o \
    /tmp/yaml_build/yaml_parser.o \
    /tmp/yaml_build/yaml_validator.o \
    -o /tmp/yaml_build/integration_test

if [ $? -eq 0 ]; then
    echo "✅ Integration test compiled successfully"
    echo ""
    /tmp/yaml_build/integration_test
else
    echo "❌ Failed to compile integration test"
    exit 1
fi