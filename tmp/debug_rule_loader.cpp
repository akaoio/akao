#include "core/rule/loader/rule_loader.hpp"
#include <iostream>

int main() {
    std::cout << "=== DEBUG RULE LOADER ===" << std::endl;
    
    try {
        std::cout << "Creating RuleLoader with 'tmp' directory..." << std::endl;
        akao::core::rule::loader::RuleLoader loader("tmp");
        std::cout << "RuleLoader created successfully!" << std::endl;
        
        std::cout << "Testing single rule load..." << std::endl;
        
        // Check if file exists
        std::cout << "Checking if tmp/minimal.yaml exists..." << std::endl;
        std::ifstream file("tmp/minimal.yaml");
        if (!file.good()) {
            std::cout << "File tmp/minimal.yaml does not exist!" << std::endl;
            return 1;
        }
        std::cout << "File exists!" << std::endl;
        
        // Test với minimal rule đã tạo
        std::cout << "Calling loadRule..." << std::endl;
        bool success = loader.loadRule("minimal.yaml");
        std::cout << "loadRule returned: " << (success ? "true" : "false") << std::endl;
        
        if (success) {
            std::cout << "Rule loaded successfully!" << std::endl;
            
            // Get loaded rules
            const auto& rules = loader.getRules();
            std::cout << "Total rules loaded: " << rules.size() << std::endl;
            
            if (!rules.empty()) {
                auto first_rule = rules[0];
                std::cout << "First Rule ID: " << first_rule->id << std::endl;
                std::cout << "First Rule Name: " << first_rule->name << std::endl;
            }
        } else {
            std::cout << "Failed to load rule" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cout << "Rule Loader Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
