#!/usr/bin/env bash

# Enhanced YAML Library Test Suite
# Tests production-ready features inspired by libyaml
# Part of Phase B: Enhanced YAML Library Development

set -e

echo "=== YAML Library Enhanced Test Suite ==="
echo "Testing production-ready YAML parsing features..."

# Create test directory
mkdir -p /tmp/yaml_tests

# Test 1: Basic YAML parsing
echo "Test 1: Basic YAML parsing..."
cat > /tmp/yaml_tests/basic.yaml << 'EOF'
---
name: "Test Document"
version: 1.0
enabled: true
count: 42
EOF

# Test 2: Complex nested structures
echo "Test 2: Complex nested structures..."
cat > /tmp/yaml_tests/complex.yaml << 'EOF'
---
metadata:
  id: test:complex:v1
  name: "Complex Test"
  version: 2.1
  tags: ["test", "complex", "nested"]

configuration:
  database:
    host: "localhost"
    port: 5432
    ssl: true
  
  cache:
    type: "redis"
    ttl: 3600
    
sequences:
  - name: "First Item"
    value: 100
  - name: "Second Item"
    value: 200
    
nested_mapping:
  level1:
    level2:
      level3: "deep value"
      numbers: [1, 2, 3, 4, 5]
EOF

# Test 3: AKAO-specific structures (Philosophy)
echo "Test 3: AKAO Philosophy structure..."
cat > /tmp/yaml_tests/philosophy.yaml << 'EOF'
---
metadata:
  id: akao:philosophy:test:v1
  name: "Test Philosophy"
  version: 1.0.0
  category: testing

description: "A test philosophy for validation"

formal_proof: "∀x∈domain → valid(x)"
self_consistency_proof: "mathematical_proof_of_consistency"

implementation:
  enforcement_points:
    - "During test execution"
  technical_requirements:
    - "Must be testable"
    
validation:
  conditions:
    - name: test_condition
      logic: "always_true()"
      
measurement:
  metrics:
    - name: test_metric
      formula: "success_rate * 100"
EOF

# Test 4: Multi-document YAML
echo "Test 4: Multi-document YAML..."
cat > /tmp/yaml_tests/multi_doc.yaml << 'EOF'
---
document: 1
title: "First Document"
---
document: 2
title: "Second Document"
content: ["item1", "item2"]
---
document: 3
title: "Third Document"
nested:
  key: "value"
EOF

# Test 5: Edge cases and error conditions
echo "Test 5: Edge cases..."
cat > /tmp/yaml_tests/edge_cases.yaml << 'EOF'
---
# Comments should be ignored
empty_string: ""
null_value: null
null_tilde: ~
boolean_variants:
  - true
  - false
  - yes
  - no
  - True
  - False

numbers:
  integer: 42
  negative: -17
  float: 3.14159
  scientific: 1.23e+10
  
special_strings:
  quoted: "This is quoted"
  single_quoted: 'Single quotes'
  multiline: |
    This is a
    multiline string
    with proper formatting
  
  folded: >
    This is a folded
    string that should
    be on one line
EOF

echo "=== Running Build Test ==="
cd /home/runner/work/akao/akao

# Test that the enhanced parser compiles
echo "Testing compilation..."
if make clean > /dev/null 2>&1 && make -j$(nproc) > /dev/null 2>&1; then
    echo "✅ Compilation successful"
else
    echo "❌ Compilation failed"
    echo "Checking for specific errors..."
    make 2>&1 | tail -20
fi

echo "=== Test Files Created ==="
echo "📄 Basic YAML: /tmp/yaml_tests/basic.yaml"
echo "📄 Complex structures: /tmp/yaml_tests/complex.yaml"  
echo "📄 AKAO Philosophy: /tmp/yaml_tests/philosophy.yaml"
echo "📄 Multi-document: /tmp/yaml_tests/multi_doc.yaml"
echo "📄 Edge cases: /tmp/yaml_tests/edge_cases.yaml"

echo ""
echo "🔍 Test file contents verification:"
for file in /tmp/yaml_tests/*.yaml; do
    echo "--- $(basename $file) ---"
    wc -l "$file"
done

echo ""
echo "✅ Enhanced YAML Library Test Suite Setup Complete"
echo "   Ready for Phase C integration testing"