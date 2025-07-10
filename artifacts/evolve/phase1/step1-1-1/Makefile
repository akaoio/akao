# Makefile for NodeValue and NodeItem Test Suite
# Follows Akao project standards with C++17 and comprehensive testing

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -I.
TARGET = test_nodevalue
SOURCES = nodevalue.cpp test_nodevalue.cpp

# Default target
all: $(TARGET)

# Build the test executable
$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

# Run the tests
test: $(TARGET)
	./$(TARGET)

# Clean build artifacts
clean:
	rm -f $(TARGET)

# Force rebuild
rebuild: clean $(TARGET)

# Help target
help:
	@echo "Available targets:"
	@echo "  all       - Build test executable (default)"
	@echo "  test      - Build and run tests"
	@echo "  clean     - Remove build artifacts"
	@echo "  rebuild   - Clean and rebuild"
	@echo "  help      - Show this help"

.PHONY: all test clean rebuild help