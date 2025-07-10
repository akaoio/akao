# Makefile for Phase 1, Step 1.2.1: INode Interface Definition
# @id: akao:file:build:node:interface:makefile:v1
# @doc: Build configuration for INode interface system enabling compilation of interface definitions, implementations, and comprehensive test suite with proper dependency management and optimization settings.

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
INCLUDES = -I../step1-1-1 -I../step1-1-2 -I.
DEPS_DIR = ../step1-1-1 ../step1-1-2

# Source files
INTERFACE_SOURCES = inode.cpp node_definition.cpp
DEPS_SOURCES = ../step1-1-1/nodevalue.cpp ../step1-1-2/nodecontext.cpp
TEST_SOURCE = test_simple.cpp

# Object files
INTERFACE_OBJECTS = $(INTERFACE_SOURCES:.cpp=.o)
DEPS_OBJECTS = $(DEPS_SOURCES:.cpp=.o)
TEST_OBJECT = $(TEST_SOURCE:.cpp=.o)

# Targets
TARGET = test_simple
LIBRARY = libinode.a

.PHONY: all clean test run-test build-deps

all: $(TARGET)

# Build dependencies first
build-deps:
	@echo "Building dependencies..."
	@for dir in $(DEPS_DIR); do \
		if [ -f $$dir/Makefile ]; then \
			echo "Building $$dir..."; \
			$(MAKE) -C $$dir; \
		fi; \
	done

# Main test executable
$(TARGET): build-deps $(INTERFACE_OBJECTS) $(DEPS_OBJECTS) $(TEST_OBJECT)
	@echo "Linking $(TARGET)..."
	$(CXX) $(CXXFLAGS) -o $@ $(INTERFACE_OBJECTS) $(DEPS_OBJECTS) $(TEST_OBJECT)

# Static library for the interface system
$(LIBRARY): $(INTERFACE_OBJECTS) $(DEPS_OBJECTS)
	@echo "Creating static library $(LIBRARY)..."
	ar rcs $@ $(INTERFACE_OBJECTS) $(DEPS_OBJECTS)

# Interface object files
%.o: %.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Dependency object files
../step1-1-1/%.o: ../step1-1-1/%.cpp
	@echo "Compiling dependency $<..."
	$(MAKE) -C ../step1-1-1 $(notdir $@)

../step1-1-2/%.o: ../step1-1-2/%.cpp
	@echo "Compiling dependency $<..."
	$(MAKE) -C ../step1-1-2 $(notdir $@)

# Test targets
test: $(TARGET)
	@echo "=== Running INode Interface Tests ==="
	./$(TARGET)

run-test: test

# Validation target
validate: $(TARGET)
	@echo "=== Validating INode Interface Implementation ==="
	./$(TARGET)
	@echo "✓ Step 1.2.1 validation complete"
	@echo "✓ Mock node executes successfully"
	@echo "✓ Parameters validate correctly"

# Debug build
debug: CXXFLAGS += -g -DDEBUG
debug: clean $(TARGET)

# Check compilation without running
check: build-deps $(INTERFACE_OBJECTS) $(DEPS_OBJECTS) $(TEST_OBJECT)
	@echo "✓ All files compile successfully"

# Clean targets
clean:
	@echo "Cleaning build artifacts..."
	rm -f $(INTERFACE_OBJECTS) $(TEST_OBJECT) $(TARGET) $(LIBRARY)

clean-all: clean
	@for dir in $(DEPS_DIR); do \
		if [ -f $$dir/Makefile ]; then \
			echo "Cleaning $$dir..."; \
			$(MAKE) -C $$dir clean; \
		fi; \
	done

# Help
help:
	@echo "Available targets:"
	@echo "  all         - Build the test executable (default)"
	@echo "  $(TARGET)   - Build the test executable"
	@echo "  $(LIBRARY)  - Build static library"
	@echo "  test        - Build and run tests"
	@echo "  run-test    - Alias for test"
	@echo "  validate    - Run validation tests"
	@echo "  debug       - Build debug version"
	@echo "  check       - Check compilation without running"
	@echo "  build-deps  - Build dependency modules"
	@echo "  clean       - Remove build artifacts"
	@echo "  clean-all   - Clean this and all dependencies"
	@echo "  help        - Show this help"

# Dependencies
inode.o: inode.hpp node_definition.hpp ../step1-1-1/nodevalue.hpp ../step1-1-2/nodecontext.hpp
node_definition.o: node_definition.hpp inode.hpp ../step1-1-1/nodevalue.hpp
test_inode_interface_simple.o: inode.hpp node_definition.hpp ../step1-1-1/nodevalue.hpp ../step1-1-2/nodecontext.hpp