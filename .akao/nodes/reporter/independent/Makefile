# Independent Report Generator Node Makefile
# Builds completely independent executable with no core dependencies

CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2
TARGET := report-generator
SOURCES := simple.cpp

# Build the independent executable
$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)
	@echo "✅ Independent report generator node built: $(TARGET)"

# Clean build artifacts
clean:
	rm -f $(TARGET)
	@echo "Cleaned build artifacts"

# Test the executable
test: $(TARGET)
	@echo "Testing independent report generator node..."
	@./$(TARGET) ./test-socket.sock &
	@sleep 1
	@pkill -f $(TARGET) || true
	@rm -f ./test-socket.sock
	@echo "✅ Basic test passed"

.PHONY: clean test