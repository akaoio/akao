cmake_minimum_required(VERSION 3.16)

# Test configuration
enable_testing()

# Include directories
include_directories(${CMAKE_SOURCE_DIR})

# Collect core sources
file(GLOB_RECURSE CORE_SOURCES 
    "${CMAKE_SOURCE_DIR}/core/**/*.cpp"
)

# Collect interface sources
file(GLOB_RECURSE INTERFACE_SOURCES 
    "${CMAKE_SOURCE_DIR}/interfaces/**/*.cpp"
)

# Collect test sources
file(GLOB_RECURSE TEST_SOURCES 
    "compliance/*.cpp"
)

# Create test executable
add_executable(akao_tests
    ${TEST_SOURCES}
    ${CORE_SOURCES}  # Include core sources for testing
    ${INTERFACE_SOURCES}  # Include interface sources for testing
)

# Set test properties
set_target_properties(akao_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests
)

# Add individual tests
add_test(NAME self_validation 
    COMMAND akao_tests self-validation
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_test(NAME philosophy_compliance 
    COMMAND akao_tests philosophy-compliance
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# Test targets
add_custom_target(test-self-validation
    COMMAND ${CMAKE_BINARY_DIR}/bin/tests/akao_tests self-validation
    DEPENDS akao_tests
    COMMENT "Running self-validation tests"
)

add_custom_target(test-philosophy-compliance
    COMMAND ${CMAKE_BINARY_DIR}/bin/tests/akao_tests philosophy-compliance
    DEPENDS akao_tests
    COMMENT "Running philosophy compliance tests"
)

add_custom_target(test-all
    COMMAND ${CMAKE_BINARY_DIR}/bin/tests/akao_tests all
    DEPENDS akao_tests
    COMMENT "Running all compliance tests"
)

# Integration with main build
add_dependencies(akao_tests akao)

message(STATUS "Test configuration:")
message(STATUS "  Test sources: ${TEST_SOURCES}")
message(STATUS "  Test executable: akao_tests")
message(STATUS "  Test targets: test-self-validation, test-philosophy-compliance, test-all")
