#include "../../core/engine/language/symbol_parser.cpp"
#include <iostream>
#include <cassert>

int main() {
    using namespace akao::core::engine::language;
    
    std::cout << "Testing three-character operators fix..." << std::endl;
    
    // Test the exact failing case from testASCIILogicSymbols
    SymbolParser parser3("<-> <=>");
    auto tokens3 = parser3.tokenize();
    
    std::cout << "Input: \"<-> <=>\"" << std::endl;
    std::cout << "Token count: " << tokens3.size() << std::endl;
    
    for (size_t i = 0; i < tokens3.size(); i++) {
        std::cout << "Token " << i << ": type=" << (int)tokens3[i].type << ", value=\"" << tokens3[i].value << "\"" << std::endl;
    }
    
    if (tokens3.size() == 3) {
        std::cout << "✅ SUCCESS: Three-character operator fix works!" << std::endl;
        std::cout << "✅ ASCII Logic Symbols test should now pass" << std::endl;
        return 0;
    } else {
        std::cout << "❌ FAILED: Expected 3 tokens, got " << tokens3.size() << std::endl;
        return 1;
    }
}
