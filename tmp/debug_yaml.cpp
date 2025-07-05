#include "core/engine/parser/yaml_parser.hpp"
#include <iostream>

int main() {
    std::cout << "=== DEBUG YAML PARSER ===" << std::endl;
    
    try {
        akao::core::engine::parser::YamlParser parser;
        
        // Test với minimal rule
        std::string simple_yaml = R"(---
metadata:
  id: akao:rule::test:minimal:v1
  name: Minimal Test Rule
description: Test
)";
        
        std::cout << "Testing YAML parsing..." << std::endl;
        auto result = parser.parse(simple_yaml);
        
        std::cout << "Parse successful!" << std::endl;
        std::cout << "Result type: " << (result ? "Valid node" : "Null") << std::endl;
        
        if (result) {
            std::cout << "Node type: " << static_cast<int>(result->getType()) << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cout << "YAML Parser Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
