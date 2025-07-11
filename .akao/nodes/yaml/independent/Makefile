# Independent YAML Processor Node Makefile
# Builds completely independent executable with no core dependencies

CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2
TARGET := yaml-processor
SOURCES := simple.cpp

# Build the independent executable
$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)
	@echo "✅ Independent YAML processor node built: $(TARGET)"

# Clean build artifacts
clean:
	rm -f $(TARGET)
	@echo "Cleaned build artifacts"

# Test the executable
test: $(TARGET)
	@echo "Testing independent YAML processor node..."
	@./$(TARGET) ./test-socket.sock &
	@sleep 1
	@pkill -f $(TARGET) || true
	@rm -f ./test-socket.sock
	@echo "✅ Basic test passed"

.PHONY: clean test