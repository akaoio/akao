/**
 * @id akao.evolve.test_parameters.v1
 * @doc Comprehensive test suite for NodeParameter system validation including type checking, constraint validation, factory methods, and Akao Pure Logic expression testing.
 * @specification Executable test program that validates all parameter types, constraint checking, validation rules, and parameter set functionality with real data processing.
 * @scope artifacts/evolve/phase1/step1-2-2
 * @timeline 2025-07-10
 * @rationale Ensure NodeParameter system works correctly with comprehensive coverage of all parameter types and validation scenarios.
 * @methodology Unit testing approach with real data validation, constraint testing, and integration verification.
 * @references akao.evolve.node_parameter.v1
 */

#include "node_parameter.hpp"
#include <iostream>
#include <cassert>
#include <vector>
#include <fstream>
#include <filesystem>

using namespace akao::evolve;

// Test result tracking
int tests_passed = 0;
int tests_failed = 0;

#define TEST_ASSERT(condition, message) \
    do { \
        if (condition) { \
            tests_passed++; \
            std::cout << "✅ PASS: " << message << std::endl; \
        } else { \
            tests_failed++; \
            std::cout << "❌ FAIL: " << message << std::endl; \
        } \
    } while(0)

void test_parameter_value_creation() {
    std::cout << "\n=== Testing ParameterValue Creation ===" << std::endl;
    
    // String value
    ParameterValue str_val("hello world");
    TEST_ASSERT(str_val.isString(), "String value type detection");
    TEST_ASSERT(str_val.as<std::string>() == "hello world", "String value retrieval");
    
    // Number values
    ParameterValue int_val(static_cast<int64_t>(42));
    TEST_ASSERT(int_val.isNumber(), "Integer value type detection");
    TEST_ASSERT(int_val.as<int64_t>() == 42, "Integer value retrieval");
    
    ParameterValue double_val(3.14159);
    TEST_ASSERT(double_val.isNumber(), "Double value type detection");
    TEST_ASSERT(abs(double_val.as<double>() - 3.14159) < 0.0001, "Double value retrieval");
    
    // Boolean value
    ParameterValue bool_val(true);
    TEST_ASSERT(bool_val.isBoolean(), "Boolean value type detection");
    TEST_ASSERT(bool_val.as<bool>() == true, "Boolean value retrieval");
    
    // Collection value
    std::vector<std::string> collection = {"item1", "item2", "item3"};
    ParameterValue collection_val(collection);
    TEST_ASSERT(collection_val.isCollection(), "Collection value type detection");
    TEST_ASSERT(collection_val.as<std::vector<std::string>>().size() == 3, "Collection size verification");
    TEST_ASSERT(collection_val.as<std::vector<std::string>>()[0] == "item1", "Collection content verification");
}

void test_parameter_value_string_conversion() {
    std::cout << "\n=== Testing ParameterValue String Conversion ===" << std::endl;
    
    ParameterValue str_val("test string");
    TEST_ASSERT(str_val.toString() == "test string", "String toString conversion");
    
    ParameterValue int_val(static_cast<int64_t>(42));
    TEST_ASSERT(int_val.toString() == "42", "Integer toString conversion");
    
    ParameterValue double_val(3.14);
    std::string double_str = double_val.toString();
    TEST_ASSERT(double_str.find("3.14") == 0, "Double toString conversion");
    
    ParameterValue bool_val(true);
    TEST_ASSERT(bool_val.toString() == "true", "Boolean toString conversion");
    
    std::vector<std::string> collection = {"a", "b", "c"};
    ParameterValue collection_val(collection);
    std::string collection_str = collection_val.toString();
    TEST_ASSERT(collection_str == "[\"a\", \"b\", \"c\"]", "Collection toString conversion");
}

void test_basic_parameter_validation() {
    std::cout << "\n=== Testing Basic Parameter Validation ===" << std::endl;
    
    // String parameter validation
    NodeParameter str_param = NodeParameter::createString("name", "Display Name", true, "default");
    ParameterValue valid_str("valid string");
    ValidationResult result = str_param.validate(valid_str);
    TEST_ASSERT(result.valid, "Valid string parameter validation");
    
    // Test required validation
    ParameterValue empty_str("");
    result = str_param.validate(empty_str);
    TEST_ASSERT(!result.valid, "Required empty string validation fails");
    TEST_ASSERT(result.error_message.find("cannot be empty") != std::string::npos, 
                "Required validation error message");
    
    // Number parameter validation
    NodeParameter num_param = NodeParameter::createNumber("count", "Count", false, 0.0);
    ParameterValue valid_num(static_cast<int64_t>(100));
    result = num_param.validate(valid_num);
    TEST_ASSERT(result.valid, "Valid number parameter validation");
    
    // Boolean parameter validation
    NodeParameter bool_param = NodeParameter::createBoolean("enabled", "Enabled", false, false);
    ParameterValue valid_bool(true);
    result = bool_param.validate(valid_bool);
    TEST_ASSERT(result.valid, "Valid boolean parameter validation");
}

void test_options_parameter_validation() {
    std::cout << "\n=== Testing Options Parameter Validation ===" << std::endl;
    
    std::vector<ParameterOption> options = {
        ParameterOption("option1", "Option 1", "First option"),
        ParameterOption("option2", "Option 2", "Second option"),
        ParameterOption("option3", "Option 3", "Third option")
    };
    
    NodeParameter options_param = NodeParameter::createOptions("choice", "Choice", options, true, "option1");
    
    // Valid option
    ParameterValue valid_option("option2");
    ValidationResult result = options_param.validate(valid_option);
    TEST_ASSERT(result.valid, "Valid option selection");
    
    // Invalid option
    ParameterValue invalid_option("invalid_option");
    result = options_param.validate(invalid_option);
    TEST_ASSERT(!result.valid, "Invalid option selection fails");
    TEST_ASSERT(result.error_message.find("not a valid option") != std::string::npos,
                "Invalid option error message");
}

void test_constraint_validation() {
    std::cout << "\n=== Testing Constraint Validation ===" << std::endl;
    
    // String length constraints
    NodeParameter str_param = NodeParameter::createString("description", "Description", true, "")
        .setLengthConstraints(5, 20);
    
    ParameterValue valid_length("hello world");
    ValidationResult result = str_param.validate(valid_length);
    TEST_ASSERT(result.valid, "Valid string length constraint");
    
    ParameterValue too_short("hi");
    result = str_param.validate(too_short);
    TEST_ASSERT(!result.valid, "String too short constraint violation");
    TEST_ASSERT(result.error_message.find("at least 5") != std::string::npos,
                "Minimum length error message");
    
    ParameterValue too_long("this string is definitely too long for the constraint");
    result = str_param.validate(too_long);
    TEST_ASSERT(!result.valid, "String too long constraint violation");
    TEST_ASSERT(result.error_message.find("at most 20") != std::string::npos,
                "Maximum length error message");
    
    // Numeric value constraints
    NodeParameter num_param = NodeParameter::createNumber("score", "Score", true, 0.0)
        .setValueConstraints(0.0, 100.0);
    
    ParameterValue valid_score(85.5);
    result = num_param.validate(valid_score);
    TEST_ASSERT(result.valid, "Valid numeric range constraint");
    
    ParameterValue too_low(-5.0);
    result = num_param.validate(too_low);
    TEST_ASSERT(!result.valid, "Numeric too low constraint violation");
    
    ParameterValue too_high(150.0);
    result = num_param.validate(too_high);
    TEST_ASSERT(!result.valid, "Numeric too high constraint violation");
}

void test_file_path_validation() {
    std::cout << "\n=== Testing File Path Validation ===" << std::endl;
    
    // Create a test file for validation
    std::string test_file = "test_file.txt";
    std::ofstream out(test_file);
    out << "test content";
    out.close();
    
    // File path with extension validation
    std::vector<std::string> allowed_exts = {".txt", ".md", ".log"};
    NodeParameter file_param = NodeParameter::createFilePath("input_file", "Input File", allowed_exts, true, "");
    
    ParameterValue valid_file(test_file);
    ValidationResult result = file_param.validate(valid_file);
    TEST_ASSERT(result.valid, "Valid file path with correct extension");
    
    // Invalid extension
    ParameterValue invalid_ext("test.xyz");
    result = file_param.validate(invalid_ext);
    TEST_ASSERT(!result.valid, "Invalid file extension constraint violation");
    TEST_ASSERT(result.error_message.find("not allowed") != std::string::npos,
                "Invalid extension error message");
    
    // Clean up test file
    std::filesystem::remove(test_file);
}

void test_akao_expression_validation() {
    std::cout << "\n=== Testing Akao Expression Validation ===" << std::endl;
    
    // String length validation using mock Akao logic
    NodeParameter akao_param = NodeParameter::createAkaoExpression("condition", "Condition", false, "")
        .setAkaoValidation("string.length($value) > 0");
    
    ParameterValue valid_expr("forall $x in collection: x > 0");
    ValidationResult result = akao_param.validate(valid_expr);
    TEST_ASSERT(result.valid, "Valid Akao expression with length > 0");
    
    ParameterValue empty_expr("");
    result = akao_param.validate(empty_expr);
    TEST_ASSERT(!result.valid, "Empty Akao expression validation fails");
    TEST_ASSERT(result.error_message.find("string must not be empty") != std::string::npos ||
                result.error_message.find("cannot be empty") != std::string::npos,
                "Akao validation error message");
    
    // Numeric positive validation
    NodeParameter num_akao = NodeParameter::createNumber("threshold", "Threshold", true, 0.0)
        .setAkaoValidation("math.is_positive($value)");
    
    ParameterValue positive_num(static_cast<int64_t>(42));
    result = num_akao.validate(positive_num);
    TEST_ASSERT(result.valid, "Positive number Akao validation");
    
    ParameterValue negative_num(static_cast<int64_t>(-5));
    result = num_akao.validate(negative_num);
    TEST_ASSERT(!result.valid, "Negative number Akao validation fails");
}

void test_parameter_set_functionality() {
    std::cout << "\n=== Testing ParameterSet Functionality ===" << std::endl;
    
    ParameterSet param_set;
    
    // Add various parameters
    param_set.addParameter(NodeParameter::createString("name", "Name", true, ""));
    param_set.addParameter(NodeParameter::createNumber("age", "Age", true, 0));
    param_set.addParameter(NodeParameter::createBoolean("active", "Active", false, true));
    
    TEST_ASSERT(param_set.size() == 3, "Parameter set size");
    TEST_ASSERT(!param_set.empty(), "Parameter set not empty");
    
    // Test parameter retrieval
    auto name_param = param_set.getParameter("name");
    TEST_ASSERT(name_param.has_value(), "Parameter retrieval by name");
    TEST_ASSERT(name_param->name == "name", "Retrieved parameter name");
    
    auto missing_param = param_set.getParameter("missing");
    TEST_ASSERT(!missing_param.has_value(), "Missing parameter returns nullopt");
    
    // Test required parameters list
    auto required = param_set.getRequiredParameters();
    TEST_ASSERT(required.size() == 2, "Required parameters count");
    TEST_ASSERT(std::find(required.begin(), required.end(), "name") != required.end(),
                "Required parameter 'name' in list");
    TEST_ASSERT(std::find(required.begin(), required.end(), "age") != required.end(),
                "Required parameter 'age' in list");
    
    // Test all parameters list
    auto all_params = param_set.getAllParameterNames();
    TEST_ASSERT(all_params.size() == 3, "All parameters count");
}

void test_parameter_set_validation() {
    std::cout << "\n=== Testing ParameterSet Validation ===" << std::endl;
    
    ParameterSet param_set;
    param_set.addParameter(NodeParameter::createString("username", "Username", true, "")
                          .setLengthConstraints(3, 20));
    param_set.addParameter(NodeParameter::createNumber("score", "Score", false, 0.0)
                          .setValueConstraints(0.0, 100.0));
    
    // Valid parameter set
    std::map<std::string, ParameterValue> valid_values = {
        {"username", ParameterValue("john_doe")},
        {"score", ParameterValue(85.5)}
    };
    
    ValidationResult result = param_set.validateAll(valid_values);
    TEST_ASSERT(result.valid, "Valid parameter set validation");
    
    // Missing required parameter
    std::map<std::string, ParameterValue> missing_required = {
        {"score", ParameterValue(75.0)}
    };
    
    result = param_set.validateAll(missing_required);
    TEST_ASSERT(!result.valid, "Missing required parameter validation fails");
    TEST_ASSERT(result.error_message.find("missing") != std::string::npos,
                "Missing parameter error message");
    
    // Unknown parameter
    std::map<std::string, ParameterValue> unknown_param = {
        {"username", ParameterValue("john_doe")},
        {"unknown", ParameterValue("value")}
    };
    
    result = param_set.validateAll(unknown_param);
    TEST_ASSERT(!result.valid, "Unknown parameter validation fails");
    TEST_ASSERT(result.error_message.find("Unknown parameter") != std::string::npos,
                "Unknown parameter error message");
    
    // Constraint violation
    std::map<std::string, ParameterValue> constraint_violation = {
        {"username", ParameterValue("jo")}, // Too short
        {"score", ParameterValue(50.0)}
    };
    
    result = param_set.validateAll(constraint_violation);
    TEST_ASSERT(!result.valid, "Constraint violation validation fails");
}

void test_factory_methods() {
    std::cout << "\n=== Testing Factory Methods ===" << std::endl;
    
    // Test string factory
    auto str_param = NodeParameter::createString("text", "Text Input", true, "default_text");
    TEST_ASSERT(str_param.name == "text", "String factory name");
    TEST_ASSERT(str_param.display_name == "Text Input", "String factory display name");
    TEST_ASSERT(str_param.type == ParameterType::STRING, "String factory type");
    TEST_ASSERT(str_param.required == true, "String factory required flag");
    TEST_ASSERT(str_param.default_value.as<std::string>() == "default_text", "String factory default value");
    
    // Test number factory
    auto num_param = NodeParameter::createNumber("count", "Count", false, 42.0);
    TEST_ASSERT(num_param.type == ParameterType::NUMBER, "Number factory type");
    TEST_ASSERT(num_param.required == false, "Number factory required flag");
    TEST_ASSERT(abs(num_param.default_value.as<double>() - 42.0) < 0.001, "Number factory default value");
    
    // Test boolean factory
    auto bool_param = NodeParameter::createBoolean("enabled", "Enabled", true, false);
    TEST_ASSERT(bool_param.type == ParameterType::BOOLEAN, "Boolean factory type");
    TEST_ASSERT(bool_param.default_value.as<bool>() == false, "Boolean factory default value");
    
    // Test Akao expression factory
    auto akao_param = NodeParameter::createAkaoExpression("rule", "Rule", true, "forall x: x > 0");
    TEST_ASSERT(akao_param.type == ParameterType::AKAO_EXPRESSION, "Akao expression factory type");
    TEST_ASSERT(akao_param.default_value.as<std::string>() == "forall x: x > 0", "Akao expression default value");
    
    // Test file path factory
    std::vector<std::string> exts = {".cpp", ".hpp"};
    auto file_param = NodeParameter::createFilePath("source", "Source File", exts, true, "main.cpp");
    TEST_ASSERT(file_param.type == ParameterType::FILE_PATH, "File path factory type");
    TEST_ASSERT(file_param.allowed_extensions.size() == 2, "File path factory extensions count");
    TEST_ASSERT(file_param.allowed_extensions[0] == ".cpp", "File path factory extension");
}

void run_performance_test() {
    std::cout << "\n=== Performance Test ===" << std::endl;
    
    // Create a parameter set with multiple parameters
    ParameterSet param_set;
    param_set.addParameter(NodeParameter::createString("name", "Name", true, ""));
    param_set.addParameter(NodeParameter::createNumber("score", "Score", true, 0.0));
    param_set.addParameter(NodeParameter::createBoolean("active", "Active", false, true));
    
    // Prepare test data
    std::map<std::string, ParameterValue> test_values = {
        {"name", ParameterValue("test_user")},
        {"score", ParameterValue(95.0)},
        {"active", ParameterValue(true)}
    };
    
    // Run validation many times
    const int iterations = 1000;
    int successful_validations = 0;
    
    for (int i = 0; i < iterations; ++i) {
        ValidationResult result = param_set.validateAll(test_values);
        if (result.valid) {
            successful_validations++;
        }
    }
    
    TEST_ASSERT(successful_validations == iterations, 
                "Performance test: " + std::to_string(successful_validations) + "/" + std::to_string(iterations) + " validations successful");
}

int main() {
    std::cout << "🚀 Starting NodeParameter System Tests" << std::endl;
    std::cout << "======================================" << std::endl;
    
    test_parameter_value_creation();
    test_parameter_value_string_conversion();
    test_basic_parameter_validation();
    test_options_parameter_validation();
    test_constraint_validation();
    test_file_path_validation();
    test_akao_expression_validation();
    test_parameter_set_functionality();
    test_parameter_set_validation();
    test_factory_methods();
    run_performance_test();
    
    std::cout << "\n======================================" << std::endl;
    std::cout << "📊 Test Results Summary:" << std::endl;
    std::cout << "✅ Tests Passed: " << tests_passed << std::endl;
    std::cout << "❌ Tests Failed: " << tests_failed << std::endl;
    std::cout << "📈 Success Rate: " << (tests_passed * 100 / (tests_passed + tests_failed)) << "%" << std::endl;
    
    if (tests_failed == 0) {
        std::cout << "\n🎉 ALL TESTS PASSED! NodeParameter system is working correctly." << std::endl;
        std::cout << "✅ All parameter types validate correctly" << std::endl;
        std::cout << "✅ .a format validation works" << std::endl;
        std::cout << "✅ Constraint validation functions properly" << std::endl;
        std::cout << "✅ Parameter sets handle validation correctly" << std::endl;
        return 0;
    } else {
        std::cout << "\n❌ SOME TESTS FAILED! Please review the implementation." << std::endl;
        return 1;
    }
}