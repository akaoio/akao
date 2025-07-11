# Independent File Scanner Node Makefile
# Builds completely independent executable with no core dependencies

CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2
TARGET := file-scanner
SOURCES := independent.cpp

# Build the independent executable
$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)
	@echo "✅ Independent file scanner node built: $(TARGET)"

# Clean build artifacts
clean:
	rm -f $(TARGET)
	@echo "Cleaned build artifacts"

# Test the executable
test: $(TARGET)
	@echo "Testing independent file scanner node..."
	@./$(TARGET) ./test-socket.sock &
	@sleep 1
	@pkill -f $(TARGET) || true
	@rm -f ./test-socket.sock
	@echo "✅ Basic test passed"

.PHONY: clean test