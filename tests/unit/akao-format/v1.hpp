/**
 * @id: akao:file:test:unit:akao_format_header:v1
 * @doc: Header file for Akao Pure Logic (.a) format unit tests providing test class definitions and helper functions for comprehensive syntax testing and engine integration validation.
 * @specification: Test header for .a format unit tests
 * @scope: Unit testing header for .a format parser
 * @timeline: 2025-07-09
 * @rationale: Provide structured testing framework for .a format parser validation
 * @methodology: Systematic test organization with assertion helpers and test categorization
 * @references: ["akao:file:test:unit:akao_format:v1", "akao:file:core:engine:logic_parser_akao:v1"]
 */

#pragma once

#include "../../../core/engine/logic/core/v1.hpp"
#include "../../../core/engine/logic/parser/akao/v1.hpp"
#include <string>
#include <vector>

namespace akao::test::unit {

/**
 * @brief Test suite for .a format parser and engine integration
 */
class AkaoFormatTestSuite {
public:
    /**
     * @brief Run all .a format tests
     */
    static void runAllTests();
    
    /**
     * @brief Test basic literal parsing
     */
    static void testBasicLiterals();
    
    /**
     * @brief Test operator parsing and precedence
     */
    static void testOperators();
    
    /**
     * @brief Test function call syntax
     */
    static void testFunctionCalls();
    
    /**
     * @brief Test quantifier syntax
     */
    static void testQuantifiers();
    
    /**
     * @brief Test conditional expressions
     */
    static void testConditionals();
    
    /**
     * @brief Test error handling
     */
    static void testErrorHandling();
    
    /**
     * @brief Test format detection
     */
    static void testFormatDetection();

private:
    /**
     * @brief Helper function to assert test conditions
     */
    static void assert_test(bool condition, const std::string& test_name);
    
    /**
     * @brief Test statistics
     */
    static int tests_passed;
    static int tests_total;
};

/**
 * @brief Sample .a format expressions for testing
 */
class AkaoFormatExamples {
public:
    /**
     * @brief Basic syntax examples
     */
    static const std::vector<std::string> BASIC_EXAMPLES;
    
    /**
     * @brief Complex expression examples
     */
    static const std::vector<std::string> COMPLEX_EXAMPLES;
    
    /**
     * @brief Error case examples
     */
    static const std::vector<std::string> ERROR_EXAMPLES;
    
    /**
     * @brief YAML equivalent examples for comparison
     */
    static const std::vector<std::pair<std::string, std::string>> YAML_COMPARISONS;
};

} // namespace akao::test::unit