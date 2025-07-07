cmake_minimum_required(VERSION 3.16)
project(akao VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Include directories
include_directories(${CMAKE_SOURCE_DIR})

# Collect all source files
file(GLOB_RECURSE CORE_SOURCES 
    "core/**/*.cpp"
)

file(GLOB_RECURSE INTERFACE_SOURCES 
    "interfaces/**/*.cpp"
)

# Main executable
add_executable(akao
    main.cpp
    ${CORE_SOURCES}
    ${INTERFACE_SOURCES}
)

# Complete integration test executable
add_executable(complete_test
    complete_integration_test.cpp
    ${CORE_SOURCES}
)

# Link libraries (no external dependencies)
# target_link_libraries(akao ${YAML_CPP_LIBRARIES})
# target_compile_options(akao PRIVATE ${YAML_CPP_CFLAGS_OTHER})

# Set output directories
set_target_properties(akao PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

set_target_properties(complete_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Installation
install(TARGETS akao complete_test
    RUNTIME DESTINATION bin
)

# Install philosophies and rules
install(DIRECTORY philosophies/ 
    DESTINATION share/akao/philosophies
    FILES_MATCHING PATTERN "*.yaml"
)

install(DIRECTORY rules/ 
    DESTINATION share/akao/rules
    FILES_MATCHING PATTERN "*.yaml"
)

# Development targets
add_custom_target(clean-all
    COMMAND ${CMAKE_BUILD_TOOL} clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/bin
)

# Self-validation target
add_custom_target(self-validate
    COMMAND ${CMAKE_BINARY_DIR}/bin/akao self-validate
    DEPENDS akao
    COMMENT "Running Akao self-validation"
)

# Testing configuration
enable_testing()

# Test executable with its own main
if(EXISTS "${CMAKE_SOURCE_DIR}/tests")
    # Include test directories
    include_directories(tests)
    
    # Collect test sources
    file(GLOB_RECURSE TEST_SOURCES 
        "tests/compliance/*.cpp"
        "tests/cli/*.cpp"
        "tests/unit/*.cpp"
    )
    
    # Create test executable
    add_executable(akao_tests
        tests/test_runner.cpp
        ${TEST_SOURCES}
        ${CORE_SOURCES}
        ${INTERFACE_SOURCES}
    )
    
    # Set test output directory
    set_target_properties(akao_tests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
    
    # Individual test targets
    add_custom_target(test-self-validation
        COMMAND ${CMAKE_BINARY_DIR}/bin/akao_tests self-validation
        DEPENDS akao_tests
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running Akao self-validation tests"
    )
    
    add_custom_target(test-philosophy-compliance
        COMMAND ${CMAKE_BINARY_DIR}/bin/akao_tests philosophy-compliance
        DEPENDS akao_tests
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running Akao philosophy compliance tests"
    )
    
    add_custom_target(test-all-compliance
        COMMAND ${CMAKE_BINARY_DIR}/bin/akao_tests all
        DEPENDS akao_tests
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running all Akao compliance tests"
    )
    
    # CTest integration
    add_test(NAME akao_self_validation 
        COMMAND akao_tests self-validation
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
    
    add_test(NAME akao_philosophy_compliance 
        COMMAND akao_tests philosophy-compliance
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
    
    message(STATUS "Testing enabled with targets: test-self-validation, test-philosophy-compliance, test-all-compliance")
endif()

# Documentation generation (if available)
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    
    if(EXISTS ${DOXYGEN_IN})
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        
        add_custom_target(docs
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
        )
    endif()
endif()

# Print configuration summary
message(STATUS "")
message(STATUS "Akao Configuration Summary:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Source directory: ${CMAKE_SOURCE_DIR}")
message(STATUS "  Binary directory: ${CMAKE_BINARY_DIR}")
message(STATUS "")
message(STATUS "Components:")
message(STATUS "  Core sources: ${CORE_SOURCES}")
message(STATUS "  Interface sources: ${INTERFACE_SOURCES}")
message(STATUS "")
message(STATUS "Build targets:")
message(STATUS "  akao           - Main executable")
message(STATUS "  self-validate  - Run Akao self-validation")
message(STATUS "  clean-all      - Clean all build artifacts")
if(DOXYGEN_FOUND)
message(STATUS "  docs           - Generate documentation")
endif()
message(STATUS "")
