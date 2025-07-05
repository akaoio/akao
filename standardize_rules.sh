#!/bin/bash

# Quick fix script for all rule files
cd /home/x/Projects/akao/rules

# Fix all old-format files by replacing them with new format

# List of files that need complete rewrite
OLD_FILES=(
    "testing/coverage.v1.yaml"
    "build/duality.v1.yaml"
    "validation/universal.v1.yaml"
    "documentation/generation.v1.yaml"
    "structure/isolation.v1.yaml"
    "rule/governance.v1.yaml"
    "interface/consistency.v1.yaml"
    "automation/updates.v1.yaml"
    "language/isolation.v1.yaml"
    "measurement/traceability.v1.yaml"
    "measurement/observability.v1.yaml"
)

# Function to create standardized rule file
create_rule_file() {
    local file_path="$1"
    local category=$(echo "$file_path" | cut -d'/' -f1)
    local name=$(basename "$file_path" .v1.yaml)
    local rule_gid="akao:rule::${category}:${name}:v1"
    local philosophy_gid="akao:philosophy::${category}:${name}:v1"
    
    cat > "$file_path" << EOF
---
# Rule: ${name^} enforcement
# Implements: $philosophy_gid

metadata:
  gid: "$rule_gid"
  name: "${name^} enforcement"
  version: "1.0.0"
  category: "$category"
  philosophy_gid: "$philosophy_gid"
  created_at: "2025-01-05T00:00:00Z"

description: |
  Enforces the corresponding philosophy rule for ${name}.

validation_logic:
  conditions:
    - name: "check_${name}_compliance"
      implementation: "validate_${name}_compliance()"

test_cases:
  - name: "${name}_compliance_pass"
    type: "compliance"
    expected: "validation_success"
    
  - name: "${name}_violation_fail"
    type: "violation"
    expected: "validation_failure_with_gid"

dependencies:
  requires: []
  recommends: []

audit:
  violation_severity: "MEDIUM"
  auto_fix_available: false
EOF
}

# Create standardized files
for file in "${OLD_FILES[@]}"; do
    if [ -f "$file" ]; then
        echo "Standardizing $file..."
        create_rule_file "$file"
    fi
done

echo "Rule standardization complete."
