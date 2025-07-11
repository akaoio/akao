# Comprehensive System Integration Test Suite Makefile
# Tests complete core-node independence architecture

CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -I..
TARGET := system_integration_test
SOURCES := system_integration_test.cpp

# Build directories
BUILD_DIR := build
TEST_RESULTS_DIR := results

# Create directories
$(shell mkdir -p $(BUILD_DIR) $(TEST_RESULTS_DIR))

# Build the test suite
$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/$(TARGET) $(SOURCES)
	@echo "✅ System integration test suite built: $(BUILD_DIR)/$(TARGET)"

# Build simple validation test
simple_validation_test: simple_validation_test.cpp
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/simple_validation_test simple_validation_test.cpp
	@echo "✅ Simple validation test built: $(BUILD_DIR)/simple_validation_test"

# Run the comprehensive test suite
test: $(TARGET)
	@echo "🚀 Running comprehensive system integration test suite..."
	@echo "=================================================="
	@cd .. && ./test/$(BUILD_DIR)/$(TARGET) 2>&1 | tee test/$(TEST_RESULTS_DIR)/test_results_$(shell date +%Y%m%d_%H%M%S).log
	@echo "📊 Test results saved to test/$(TEST_RESULTS_DIR)/"

# Run quick smoke test (basic functionality only)
smoke: simple_validation_test
	@echo "🔥 Running simple validation test..."
	@cd .. && ./test/$(BUILD_DIR)/simple_validation_test

# Build all nodes first
build-nodes:
	@echo "🔧 Building all nodes..."
	@cd ../.akao/nodes/file && make clean && make
	@cd ../.akao/nodes/logic/independent && make clean && make
	@cd ../.akao/nodes/reporter/independent && make clean && make
	@cd ../.akao/nodes/yaml/independent && make clean && make
	@echo "✅ All nodes built successfully"

# Build core (if makefile exists)
build-core:
	@echo "🔧 Building core..."
	@if [ -f ../core/Makefile ]; then cd ../core && make clean && make; else echo "⚠️ Core makefile not found, skipping"; fi

# Full build and test cycle
full-test: build-nodes build-core test
	@echo "🎯 Full build and test cycle completed"

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(TEST_RESULTS_DIR)
	@echo "Cleaned test artifacts"

# Install test dependencies (if any)
install-deps:
	@echo "📦 Installing test dependencies..."
	@echo "✅ No additional dependencies required"

# Generate test documentation
docs:
	@echo "📚 Generating test documentation..."
	@echo "# System Integration Test Suite" > README.md
	@echo "" >> README.md
	@echo "## Overview" >> README.md
	@echo "Comprehensive test suite validating complete core-node independence architecture." >> README.md
	@echo "" >> README.md
	@echo "## Test Categories" >> README.md
	@echo "1. Build System Validation" >> README.md
	@echo "2. Node Independence Validation" >> README.md
	@echo "3. Node Process Management" >> README.md
	@echo "4. YAML-RPC Protocol Validation" >> README.md
	@echo "5. Node Functional Validation" >> README.md
	@echo "6. Error Handling and Recovery" >> README.md
	@echo "7. Performance and Load Testing" >> README.md
	@echo "8. Core-Node Integration" >> README.md
	@echo "9. System Stability" >> README.md
	@echo "10. Resource Management" >> README.md
	@echo "" >> README.md
	@echo "## Usage" >> README.md
	@echo '```bash' >> README.md
	@echo "make full-test  # Build everything and run all tests" >> README.md
	@echo "make test       # Run tests only" >> README.md
	@echo "make smoke      # Run quick smoke test" >> README.md
	@echo '```' >> README.md
	@echo "✅ Documentation generated: README.md"

# Help target
help:
	@echo "🧪 Akao System Integration Test Suite"
	@echo "====================================="
	@echo "Available targets:"
	@echo "  full-test     - Build all components and run comprehensive tests"
	@echo "  test          - Run comprehensive test suite"
	@echo "  smoke         - Run quick smoke test"
	@echo "  build-nodes   - Build all independent nodes"
	@echo "  build-core    - Build core system"
	@echo "  clean         - Clean build artifacts"
	@echo "  install-deps  - Install test dependencies"
	@echo "  docs          - Generate test documentation"
	@echo "  help          - Show this help message"
	@echo ""
	@echo "Test validation includes:"
	@echo "  ✅ Build system validation"
	@echo "  ✅ Node independence validation"
	@echo "  ✅ YAML-RPC protocol testing"
	@echo "  ✅ Process management validation"
	@echo "  ✅ Functional testing of all nodes"
	@echo "  ✅ Error handling and recovery"
	@echo "  ✅ Performance and load testing"
	@echo "  ✅ System stability testing"
	@echo "  ✅ Resource management validation"

.PHONY: test smoke full-test build-nodes build-core clean install-deps docs help