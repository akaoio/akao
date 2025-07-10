#!/bin/bash
# Simple build script for YamlParserNode with real dependencies
# @id akao.evolve.build_script.step2-1-2.v1

set -e

echo "🚀 Building YamlParserNode with Real Dependencies"
echo "================================================"

# Create build directory
mkdir -p build/obj

# Compiler settings
CXX="g++"
CXXFLAGS="-std=c++17 -Wall -Wextra -Wpedantic -O2 -g"
INCLUDES="-I../../../../ -I../../phase1/step1-1-1 -I../../phase1/step1-1-2 -I../../phase1/step1-2-1 -I../../phase1/step1-2-2 -I../../phase1/step1-3-1"

echo "📦 Compiling YamlParserNode..."
$CXX $CXXFLAGS $INCLUDES -c yaml_parser_node.cpp -o build/obj/yaml_parser_node.o

echo "📦 Compiling Phase 1 dependencies..."

# Compile Phase 1 dependencies
$CXX $CXXFLAGS $INCLUDES -c ../../phase1/step1-1-1/nodevalue.cpp -o build/obj/nodevalue.o
$CXX $CXXFLAGS $INCLUDES -c ../../phase1/step1-1-2/nodecontext.cpp -o build/obj/nodecontext.o  
$CXX $CXXFLAGS $INCLUDES -c ../../phase1/step1-2-1/inode.cpp -o build/obj/inode.o
$CXX $CXXFLAGS $INCLUDES -c ../../phase1/step1-2-1/node_definition.cpp -o build/obj/node_definition.o
$CXX $CXXFLAGS $INCLUDES -c ../../phase1/step1-2-2/node_parameter.cpp -o build/obj/node_parameter.o

echo "📦 Compiling core YAML parser (if available)..."
if [ -f "../../../../core/engine/parser/yaml/parser/v1.cpp" ]; then
    $CXX $CXXFLAGS $INCLUDES -c ../../../../core/engine/parser/yaml/parser/v1.cpp -o build/obj/yaml_parser.o
    echo "  ✓ YAML parser compiled"
else
    echo "  ⚠️  YAML parser not available - using header-only mode"
fi

if [ -f "../../../../core/engine/parser/yaml/node/v1.cpp" ]; then
    $CXX $CXXFLAGS $INCLUDES -c ../../../../core/engine/parser/yaml/node/v1.cpp -o build/obj/yaml_node.o
    echo "  ✓ YAML node compiled"
else
    echo "  ⚠️  YAML node not available - using header-only mode"
fi

echo "🔗 Linking object files..."
OBJECTS="build/obj/yaml_parser_node.o build/obj/nodevalue.o build/obj/nodecontext.o build/obj/inode.o build/obj/node_definition.o build/obj/node_parameter.o"

# Add YAML objects if they exist
if [ -f "build/obj/yaml_parser.o" ]; then
    OBJECTS="$OBJECTS build/obj/yaml_parser.o"
fi
if [ -f "build/obj/yaml_node.o" ]; then
    OBJECTS="$OBJECTS build/obj/yaml_node.o"
fi

# Create static library
ar rcs build/libyaml_parser_node.a $OBJECTS

echo "✅ YamlParserNode library built successfully!"
echo "📊 Build Summary:"
echo "  - Library: build/libyaml_parser_node.a"
echo "  - Objects: $(echo $OBJECTS | wc -w) files"
echo "  - Real Phase 1 integration: ✓"
echo "  - YAML parser integration: $([ -f build/obj/yaml_parser.o ] && echo '✓' || echo '⚠️ header-only')"

echo ""
echo "🧪 Testing basic functionality..."

# Create simple test
cat > build/test_basic.cpp << 'EOF'
#include "yaml_parser_node.hpp"
#include <iostream>

using namespace akao::evolve::component;

int main() {
    try {
        std::cout << "Creating YamlParserNode..." << std::endl;
        YamlParserNode node;
        
        std::cout << "Node ID: " << node.getNodeId() << std::endl;
        std::cout << "Node Type: " << node.getNodeType() << std::endl;
        std::cout << "Display Name: " << node.getDisplayName() << std::endl;
        std::cout << "Parser Ready: " << (node.isParserReady() ? "Yes" : "No") << std::endl;
        
        auto def = node.getDefinition();
        std::cout << "Definition created successfully" << std::endl;
        
        std::cout << "✅ Basic functionality test PASSED" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test FAILED: " << e.what() << std::endl;
        return 1;
    }
}
EOF

# Compile and run test
mkdir -p build/bin
$CXX $CXXFLAGS $INCLUDES build/test_basic.cpp build/libyaml_parser_node.a -o build/bin/test_basic

echo "Running basic test..."
if build/bin/test_basic; then
    echo ""
    echo "🎉 YamlParserNode implementation is working with real dependencies!"
else
    echo ""
    echo "❌ Basic test failed - implementation needs fixes"
    exit 1
fi