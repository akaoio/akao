#!/bin/bash

# Create all the new philosophies we added

# Security explicit behavior rule
cat > "rules/security/explicit_behavior.v1.yaml" << 'RULE'
---
# Rule: No external implicit behavior enforcement
# Implements: akao:philosophy::security:explicit_behavior:v1

metadata:
  gid: "akao:rule::security:explicit_behavior:v1"
  name: "No external implicit behavior enforcement"
  version: "1.0.0"
  category: "security"
  philosophy_gid: "akao:philosophy::security:explicit_behavior:v1"
  created_at: "2025-01-05T00:00:00Z"

description: |
  Enforces that every dependency, path, and output must be declared explicitly.

validation_logic:
  conditions:
    - name: "check_explicit_behavior"
      implementation: "validate_explicit_external_behavior()"

test_cases:
  - name: "explicit_dependencies_pass"
    type: "compliance"
    expected: "validation_success"
    
  - name: "implicit_dependency_fails"
    type: "violation"
    expected: "validation_failure_with_gid"

dependencies:
  requires: []
  recommends: ["akao:rule::validation:universal:v1"]

audit:
  violation_severity: "HIGH"
  auto_fix_available: false
RULE

# Continue for other new philosophies...
echo "Created security explicit behavior files"

