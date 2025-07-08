#include "core/engine/parser/yaml/parser/v1.hpp"
#include "core/engine/parser/yaml/node/v1.hpp"
#include <iostream>

int main() {
    akao::core::engine::parser::YamlParser parser;
    
    std::string yaml_content = R"(project:
  name: yaml-parser-test
  version: 2.0.0
  active: true
  count: 4
dependencies:
  - core-lib
  - utils-lib
  - test-framework)";
    
    try {
        auto result = parser.parse(yaml_content);
        if (result) {
            std::cout << "Parser returned a result" << std::endl;
            std::cout << "Type: " << (int)result->getType() << std::endl;
            
            if (result->isMapping()) {
                std::cout << "Result is mapping" << std::endl;
                std::cout << "Size: " << result->size() << std::endl;
                
                auto keys = result->getKeys();
                std::cout << "Keys: ";
                for (const auto& key : keys) {
                    std::cout << key << " ";
                }
                std::cout << std::endl;
                
                auto project_node = (*result)["project"];
                if (project_node) {
                    std::cout << "Project node found" << std::endl;
                    std::cout << "Project type: " << (int)project_node->getType() << std::endl;
                    if (project_node->isMapping()) {
                        std::cout << "Project is mapping with size: " << project_node->size() << std::endl;
                    }
                } else {
                    std::cout << "Project node not found" << std::endl;
                }
            } else {
                std::cout << "Result is not mapping" << std::endl;
            }
        } else {
            std::cout << "Parser returned null" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }
    
    return 0;
}
