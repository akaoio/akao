#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
#include <numeric>

// C++ test file for multi-language parsing validation

class TestClass {
private:
    std::vector<int> data;
    std::unique_ptr<int> ptr;
    
public:
    TestClass() : ptr(std::make_unique<int>(42)) {}
    
    void add_element(int value) {
        data.push_back(value);
    }
    
    int get_sum() const {
        return std::accumulate(data.begin(), data.end(), 0);
    }
    
    template<typename T>
    T process_generic(T value) {
        return value * 2;
    }
};

namespace test_namespace {
    void free_function() {
        std::cout << "Free function in namespace" << std::endl;
    }
    
    template<typename Container>
    auto process_container(const Container& c) -> decltype(c.size()) {
        return c.size() * 2;
    }
}

int main() {
    TestClass test;
    test.add_element(1);
    test.add_element(2);
    test.add_element(3);
    
    std::cout << "Sum: " << test.get_sum() << std::endl;
    std::cout << "Processed: " << test.process_generic(5) << std::endl;
    
    test_namespace::free_function();
    
    std::vector<int> vec = {1, 2, 3, 4, 5};
    std::cout << "Container result: " << test_namespace::process_container(vec) << std::endl;
    
    return 0;
}
