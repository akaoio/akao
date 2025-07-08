# CMake generated Testfile for 
# Source directory: /workspaces/akao/tests
# Build directory: /workspaces/akao/tests/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(EndToEndWorkflowTest "/workspaces/akao/tests/build/akao_integration_test_runner" "end_to_end_workflow")
set_tests_properties(EndToEndWorkflowTest PROPERTIES  LABELS "integration;workflow" TIMEOUT "300" WORKING_DIRECTORY "/workspaces/akao/tests" _BACKTRACE_TRIPLES "/workspaces/akao/tests/CMakeLists.txt;61;add_test;/workspaces/akao/tests/CMakeLists.txt;0;")
add_test(PerformanceBenchmarkTest "/workspaces/akao/tests/build/akao_integration_test_runner" "performance_benchmark")
set_tests_properties(PerformanceBenchmarkTest PROPERTIES  LABELS "integration;performance" TIMEOUT "600" WORKING_DIRECTORY "/workspaces/akao/tests" _BACKTRACE_TRIPLES "/workspaces/akao/tests/CMakeLists.txt;65;add_test;/workspaces/akao/tests/CMakeLists.txt;0;")
add_test(TransformationValidationTest "/workspaces/akao/tests/build/akao_integration_test_runner" "transformation_validation")
set_tests_properties(TransformationValidationTest PROPERTIES  LABELS "integration;validation" TIMEOUT "180" WORKING_DIRECTORY "/workspaces/akao/tests" _BACKTRACE_TRIPLES "/workspaces/akao/tests/CMakeLists.txt;69;add_test;/workspaces/akao/tests/CMakeLists.txt;0;")
add_test(ComprehensiveIntegrationTest "/workspaces/akao/tests/build/akao_integration_test_runner" "comprehensive")
set_tests_properties(ComprehensiveIntegrationTest PROPERTIES  LABELS "integration;comprehensive" TIMEOUT "900" WORKING_DIRECTORY "/workspaces/akao/tests" _BACKTRACE_TRIPLES "/workspaces/akao/tests/CMakeLists.txt;73;add_test;/workspaces/akao/tests/CMakeLists.txt;0;")
