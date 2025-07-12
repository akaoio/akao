# Akao Core Foundation Test Suite
# Professional build configuration following project standards

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -I../
LDFLAGS = -pthread

CORE_SOURCES = ../core/foundation/types/value/v1.cpp ../core/foundation/types/result/v1.cpp ../core/foundation/formats/yaml/v1.cpp ../core/engine/communication/yamlrpc/v1.cpp ../core/foundation/interfaces/inode/v1.cpp ../core/engine/orchestrator/discovery/v1.cpp ../core/engine/orchestrator/process/v1.cpp ../core/engine/orchestrator/workflow/v1.cpp ../core/engine/orchestrator/registry/v1.cpp
TEST_SOURCE = test.cpp
TEST_BINARY = test
REAL_NODE_SOURCE = real_node_test.cpp
REAL_NODE_BINARY = real_node_test

.PHONY: all run clean real-nodes

all: $(TEST_BINARY) $(REAL_NODE_BINARY)

$(TEST_BINARY): $(TEST_SOURCE) $(CORE_SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(REAL_NODE_BINARY): $(REAL_NODE_SOURCE) $(CORE_SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

run: $(TEST_BINARY)
	./$(TEST_BINARY)

real-nodes: $(REAL_NODE_BINARY)
	./$(REAL_NODE_BINARY)

clean:
	rm -f $(TEST_BINARY) $(REAL_NODE_BINARY)

.DEFAULT_GOAL := run