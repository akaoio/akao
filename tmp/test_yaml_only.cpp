#include "core/engine/parser/yaml_parser.hpp"
#include <iostream>

int main() {
    std::cout << "Testing YamlParser constructor..." << std::endl;
    
    try {
        akao::core::engine::parser::YamlParser parser;
        std::cout << "YamlParser created successfully!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
        return 1;
    }
}
