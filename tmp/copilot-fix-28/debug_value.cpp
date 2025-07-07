#include "../core/engine/logic/pure_logic_engine.hpp"
#include <iostream>
#include <string>

using namespace akao::logic;

int main() {
    std::cout << "Debugging Value types..." << std::endl;
    
    // Test string value creation with explicit std::string
    std::string hello_str = "hello";
    Value str_val(hello_str);
    std::cout << "Created string value with std::string constructor" << std::endl;
    std::cout << "Type enum values:" << std::endl;
    std::cout << "  BOOLEAN = " << static_cast<int>(Value::Type::BOOLEAN) << std::endl;
    std::cout << "  INTEGER = " << static_cast<int>(Value::Type::INTEGER) << std::endl;
    std::cout << "  STRING = " << static_cast<int>(Value::Type::STRING) << std::endl;
    
    std::cout << "Actual type: " << static_cast<int>(str_val.getType()) << std::endl;
    std::cout << "isString(): " << str_val.isString() << std::endl;
    std::cout << "isBoolean(): " << str_val.isBoolean() << std::endl;
    
    // Test with string literal directly
    Value str_val2("hello");
    std::cout << "\nString literal constructor:" << std::endl;
    std::cout << "Type: " << static_cast<int>(str_val2.getType()) << std::endl;
    std::cout << "isString(): " << str_val2.isString() << std::endl;
    
    return 0;
}
