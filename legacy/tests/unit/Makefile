# Akao Pure Logic Language Unit Tests Makefile
# @id: akao:file:tests:unit:makefile
# @doc: Build system for Pure Logic language unit tests providing compilation and execution of comprehensive test suite with archaeological preservation
# @specification: Makefile for Pure Logic language unit test compilation and execution
# @scope: Build system for unit testing framework
# @timeline: 2025-01-08
# @rationale: Provide automated build and test execution for Pure Logic language validation
# @methodology: Make-based build system with test compilation and execution targets
# @references: ["akao:file:tests:unit:pure_logic_language_test_runner", "akao:file:core:engine:language:pure_logic_grammar", "akao:file:core:engine:language:symbol_parser"]

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I../../ -O2
TARGET = pure_logic_language_tests
SOURCES = pure_logic_language_test_runner.cpp

# Default target
all: $(TARGET)

# Build the test executable
$(TARGET): $(SOURCES)
	@echo "🔨 Compiling Pure Logic Language Tests..."
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)
	@echo "✅ Compilation successful!"

# Run the tests
test: $(TARGET)
	@echo "🚀 Running Pure Logic Language Test Suite..."
	@echo "============================================"
	./$(TARGET)

# Clean build artifacts
clean:
	@echo "🧹 Cleaning build artifacts..."
	rm -f $(TARGET)
	@echo "✅ Clean complete!"

# Run tests with verbose output
test-verbose: $(TARGET)
	@echo "🔍 Running Pure Logic Language Tests (Verbose Mode)..."
	@echo "====================================================="
	./$(TARGET) 2>&1 | tee test_results.log
	@echo ""
	@echo "📄 Test results saved to test_results.log"

# Quick validation (compile and test)
validate: clean all test

# Archaeological preservation target
preserve: test-verbose
	@echo "🏛️ Preserving archaeological test data..."
	@mkdir -p ../../artifacts/oneshot/test-results/
	@cp test_results.log ../../artifacts/oneshot/test-results/pure_logic_language_tests_$(shell date +%Y%m%d_%H%M%S).log
	@echo "✅ Test results preserved for future intelligent agents"

# Help target
help:
	@echo "Akao Pure Logic Language Unit Tests"
	@echo "==================================="
	@echo ""
	@echo "Available targets:"
	@echo "  all          - Build the test executable"
	@echo "  test         - Run the test suite"
	@echo "  test-verbose - Run tests with verbose output and logging"
	@echo "  clean        - Remove build artifacts"
	@echo "  validate     - Clean, build, and test (full validation)"
	@echo "  preserve     - Run tests and preserve results archaeologically"
	@echo "  help         - Show this help message"
	@echo ""
	@echo "Example usage:"
	@echo "  make validate    # Full validation cycle"
	@echo "  make test        # Quick test run"
	@echo "  make preserve    # Archaeological preservation"

.PHONY: all test clean test-verbose validate preserve help
