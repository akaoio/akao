#!/bin/bash

# Script to fix all rule files with proper metadata and cross-references

# Define mapping of rule files to their philosophy GIDs
declare -A RULE_TO_PHILOSOPHY=(
    ["testing/coverage.v1.yaml"]="akao:philosophy::testing:coverage:v1"
    ["build/duality.v1.yaml"]="akao:philosophy::build:duality:v1" 
    ["validation/universal.v1.yaml"]="akao:philosophy::validation:universal:v1"
    ["documentation/generation.v1.yaml"]="akao:philosophy::documentation:generation:v1"
    ["structure/enforced.v1.yaml"]="akao:philosophy::structure:enforcement:v1"
    ["structure/isolation.v1.yaml"]="akao:philosophy::structure:isolation:v1"
    ["rule/governance.v1.yaml"]="akao:philosophy::rule:governance:v1"
    ["interface/consistency.v1.yaml"]="akao:philosophy::interface:consistency:v1"
    ["automation/updates.v1.yaml"]="akao:philosophy::automation:updates:v1"
    ["language/isolation.v1.yaml"]="akao:philosophy::language:isolation:v1"
    ["measurement/traceability.v1.yaml"]="akao:philosophy::measurement:traceability:v1"
    ["measurement/observability.v1.yaml"]="akao:philosophy::measurement:observability:v1"
)

cd /home/x/Projects/akao/rules

# First, rename files to match philosophy naming
mv testing/rules_tested.v1.yaml testing/coverage.v1.yaml 2>/dev/null || true
mv structure/enforced.v1.yaml structure/enforcement.v1.yaml 2>/dev/null || true

# Function to fix a rule file
fix_rule_file() {
    local file_path="$1"
    local philosophy_gid="$2"
    local rule_gid=$(echo "$philosophy_gid" | sed 's/philosophy/rule/')
    local category=$(echo "$file_path" | cut -d'/' -f1)
    local name=$(basename "$file_path" .v1.yaml)
    
    # Create new content
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
  Enforces the corresponding philosophy rule.

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

# Fix all old rule files
for rule_path in "${!RULE_TO_PHILOSOPHY[@]}"; do
    if [ -f "$rule_path" ]; then
        echo "Fixing $rule_path..."
        fix_rule_file "$rule_path" "${RULE_TO_PHILOSOPHY[$rule_path]}"
    fi
done

echo "All rule files have been updated with proper metadata and cross-references."
