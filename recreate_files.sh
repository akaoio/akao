#!/bin/bash

# Script to recreate all philosophy and rule files that were accidentally deleted

# Define all the philosophies and their details
declare -A PHILOSOPHIES=(
    ["structure/enforcement"]="Structure is enforced|Every file, class, and folder must follow rule-driven structure"
    ["structure/isolation"]="One class per folder|Every folder must contain exactly one class and its tests"
    ["validation/universal"]="Universal validation|Any project can be validated using akao validate, including Akao itself"
    ["testing/coverage"]="No rules without tests|Each rule must have measurable test coverage"
    ["measurement/traceability"]="Every rule is traceable|Violations include Global Rule Identifier, stack trace, file, line, and suggestion"
    ["measurement/observability"]="Every action is measurable|Coverage, logs, hash, audit state enforced per commit using GIDs"
    ["documentation/generation"]="Documentation is code|All documentation must be generated from actual rules and tests"
    ["language/isolation"]="One language per scope|Multi-language repositories must isolate languages clearly"
    ["interface/consistency"]="CLI equals Web equals TUI|All tooling must behave identically across user interface modes"
    ["rule/governance"]="Rules can be toggled, not skipped|All disabled rules tracked by GID with reason and audit trail"
    ["automation/updates"]="Auto-update all documentation|Whenever rules change, documentation must change automatically"
    ["build/duality"]="Everything must build dev plus prod|Development and production builds are mandatory"
)

# Create philosophy and rule files
create_philosophy_file() {
    local path="$1"
    local name="$2"
    local desc="$3"
    local category=$(echo "$path" | cut -d'/' -f1)
    local filename=$(echo "$path" | cut -d'/' -f2)
    
    cat > "/home/x/Projects/akao/philosophies/$path.v1.yaml" << EOF
---
# Akao Philosophy: $name
# $desc

metadata:
  gid: "akao:philosophy::$path:v1"
  name: "$name"
  version: "1.0.0"
  category: "$category"
  created_at: "2025-01-05T00:00:00Z"
  updated_at: "2025-01-05T00:00:00Z"
  author: "akao-framework"
  
description: |
  $desc

formal_logic:
  domain: "$category"
  predicate: "${filename}_${category}"
  axioms:
    - "Core axiom for $name"
  invariants:
    - "Core invariant for $name"
  
validation:
  conditions:
    - name: "check_${filename}_compliance"
      description: "Validates $name compliance"
      logic: "elements.every(e => e.complies_with_${filename}_rules())"

implementation:
  enforcement_points:
    - "Core enforcement point"
    
  technical_requirements:
    - "Core technical requirement"
    
  user_interface:
    - "akao validate --${filename}"

measurement:
  metrics:
    - name: "${filename}_compliance_rate"
      description: "Percentage of elements following $name rules"
      formula: "compliant_elements / total_elements * 100"

dependencies:
  requires: []
  recommends: []

audit:
  compliance_checks:
    - "Core compliance check for $name"
    
  violation_severity: "MEDIUM"
  auto_fix_available: false
EOF
}

create_rule_file() {
    local path="$1"
    local name="$2"
    local category=$(echo "$path" | cut -d'/' -f1)
    
    cat > "/home/x/Projects/akao/rules/$path.v1.yaml" << EOF
---
# Rule: $name enforcement
# Implements: akao:philosophy::$path:v1

metadata:
  gid: "akao:rule::$path:v1"
  name: "$name enforcement"
  version: "1.0.0"
  category: "$category"
  philosophy_gid: "akao:philosophy::$path:v1"
  created_at: "2025-01-05T00:00:00Z"

description: |
  Enforces the $name philosophy.

validation_logic:
  conditions:
    - name: "check_${path//\//_}_compliance"
      implementation: "validate_${path//\//_}_compliance()"

test_cases:
  - name: "${path//\//_}_compliance_pass"
    type: "compliance"
    expected: "validation_success"
    
  - name: "${path//\//_}_violation_fail"
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

# Create all files
for key in "${!PHILOSOPHIES[@]}"; do
    IFS='|' read -r name desc <<< "${PHILOSOPHIES[$key]}"
    echo "Creating $key..."
    create_philosophy_file "$key" "$name" "$desc"
    create_rule_file "$key" "$name"
done

echo "All philosophy and rule files have been recreated!"
