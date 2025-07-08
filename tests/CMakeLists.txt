# CMakeLists.txt for Integration Test Suite
cmake_minimum_required(VERSION 3.16)

project(AkaoIntegrationTests 
    VERSION 1.0.0
    DESCRIPTION "Comprehensive integration test suite for Akao architectural reformation"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../core)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../interfaces)

# Find all core source files
file(GLOB_RECURSE CORE_SOURCES "../core/**/*.cpp")
file(GLOB_RECURSE INTERFACE_SOURCES "../interfaces/**/*.cpp")

# Core Akao Library with built-in YAML support
add_library(akao_core STATIC
    ${CORE_SOURCES}
    ${INTERFACE_SOURCES}
)

# Integration Test Suite
add_executable(akao_integration_tests
    integration/integration_test_suite.cpp
    integration_test_runner.cpp
)

target_link_libraries(akao_integration_tests akao_core)

# Optional: Add individual test executables for specific components
add_executable(akao_performance_tests
    performance/performance_test_runner.cpp
)

add_executable(akao_transformation_tests 
    transformation/transformation_test_runner.cpp
)

target_link_libraries(akao_performance_tests akao_core)
target_link_libraries(akao_transformation_tests akao_core)

# Test data files
configure_file(data/sample_philosophy.yaml ${CMAKE_CURRENT_BINARY_DIR}/data/sample_philosophy.yaml COPYONLY)
configure_file(data/sample_rule.yaml ${CMAKE_CURRENT_BINARY_DIR}/data/sample_rule.yaml COPYONLY)
configure_file(data/sample_akao_program.a ${CMAKE_CURRENT_BINARY_DIR}/data/sample_akao_program.a COPYONLY)

# Custom target to run all tests
add_custom_target(run_integration_tests
    COMMAND akao_integration_tests
    DEPENDS akao_integration_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_target(run_performance_tests
    COMMAND akao_performance_tests
    DEPENDS akao_performance_tests  
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_target(run_transformation_tests
    COMMAND akao_transformation_tests
    DEPENDS akao_transformation_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_target(run_all_tests
    DEPENDS run_integration_tests run_performance_tests run_transformation_tests
)
