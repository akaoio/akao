
#pragma once
#include <iostream>
#include <string>

namespace test {

class TestClass {
private:
    int value_;
    std::string name_;
    
public:
    TestClass(int value, const std::string& name) 
        : value_(value), name_(name) {}
    
    virtual ~TestClass() = default;
    
    int getValue() const { return value_; }
    void setValue(int value) { value_ = value; }
    
    virtual void process() = 0;
};

} // namespace test
