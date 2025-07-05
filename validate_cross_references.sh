#!/bin/bash

# Validation script for cross-reference logic between rules and philosophies

echo "=== AKAO CROSS-REFERENCE VALIDATION ==="

VALIDATION_ERRORS=0

# Function to extract philosophy reference from rule file
get_philosophy_ref() {
    local rule_file="$1"
    grep "  philosophy:" "$rule_file" | head -1 | cut -d'"' -f2
}

# Function to check if philosophy file exists
check_philosophy_exists() {
    local philosophy_id="$1"
    # Convert akao:philosophy::category:name:v1 to philosophies/category/name.v1.yaml
    local path=$(echo "$philosophy_id" | sed 's/akao:philosophy:://' | sed 's/:v1/.v1.yaml/' | sed 's/:/\//')
    local full_path="/home/x/Projects/akao/philosophies/$path"
    
    if [ -f "$full_path" ]; then
        echo "✓ Philosophy exists: $full_path"
        return 0
    else
        echo "✗ MISSING philosophy: $full_path (referenced by rule)"
        return 1
    fi
}

# Function to check rule-philosophy ID consistency
check_id_consistency() {
    local rule_file="$1"
    local rule_id=$(grep "  id:" "$rule_file" | head -1 | cut -d'"' -f2)
    local philosophy_ref=$(get_philosophy_ref "$rule_file")
    
    # Extract category and name from rule ID
    local rule_category=$(echo "$rule_id" | cut -d':' -f4)
    local rule_name=$(echo "$rule_id" | cut -d':' -f5)
    
    # Extract category and name from philosophy reference
    local phil_category=$(echo "$philosophy_ref" | cut -d':' -f4)
    local phil_name=$(echo "$philosophy_ref" | cut -d':' -f5)
    
    if [ "$rule_category" = "$phil_category" ] && [ "$rule_name" = "$phil_name" ]; then
        echo "✓ ID consistency: $rule_file"
        return 0
    else
        echo "✗ ID MISMATCH: $rule_file"
        echo "   Rule: $rule_category/$rule_name"
        echo "   Philosophy: $phil_category/$phil_name"
        return 1
    fi
}

echo "Checking all rule files..."

find /home/x/Projects/akao/rules -name "*.yaml" -type f | while read rule_file; do
    echo "Validating: $rule_file"
    
    # Check philosophy reference exists
    philosophy_ref=$(get_philosophy_ref "$rule_file")
    if [ -z "$philosophy_ref" ]; then
        echo "✗ MISSING philosophy reference in: $rule_file"
        ((VALIDATION_ERRORS++))
        continue
    fi
    
    # Check philosophy file exists
    if ! check_philosophy_exists "$philosophy_ref"; then
        ((VALIDATION_ERRORS++))
    fi
    
    # Check ID consistency
    if ! check_id_consistency "$rule_file"; then
        ((VALIDATION_ERRORS++))
    fi
    
    echo "---"
done

echo "=== VALIDATION SUMMARY ==="
if [ $VALIDATION_ERRORS -eq 0 ]; then
    echo "✅ ALL VALIDATIONS PASSED"
else
    echo "❌ $VALIDATION_ERRORS VALIDATION ERRORS FOUND"
fi
