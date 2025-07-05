#!/bin/bash

# Simple validation script for rule-philosophy cross-references

echo "=== SIMPLE CROSS-REFERENCE CHECK ==="

errors=0

echo "Checking each rule file has corresponding philosophy..."

for rule_file in /home/x/Projects/akao/rules/*/*.yaml; do
    if [ -f "$rule_file" ]; then
        # Get philosophy reference from rule
        philosophy_ref=$(grep "  philosophy:" "$rule_file" | head -1 | cut -d'"' -f2)
        
        if [ -z "$philosophy_ref" ]; then
            echo "❌ NO philosophy reference: $rule_file"
            ((errors++))
            continue
        fi
        
        # Convert philosophy ID to file path
        # akao:philosophy::category:name:v1 -> philosophies/category/name.v1.yaml
        philosophy_path=$(echo "$philosophy_ref" | sed 's/akao:philosophy:://' | sed 's/:v1/.v1.yaml/' | sed 's/:/\//')
        philosophy_full_path="/home/x/Projects/akao/philosophies/$philosophy_path"
        
        if [ -f "$philosophy_full_path" ]; then
            echo "✅ $(basename "$rule_file") -> $(basename "$philosophy_full_path")"
        else
            echo "❌ MISSING philosophy: $philosophy_full_path"
            echo "   Referenced by: $rule_file"
            ((errors++))
        fi
    fi
done

echo ""
echo "=== SUMMARY ==="
if [ $errors -eq 0 ]; then
    echo "✅ ALL CROSS-REFERENCES VALID"
else
    echo "❌ $errors MISSING PHILOSOPHY FILES"
fi
echo "Checking format compliance..."

# Check for old format usage
echo "Checking for 'gid:' usage in rules..."
if find /home/x/Projects/akao/rules -name "*.yaml" -exec grep -l "gid:" {} \; | head -5; then
    echo "✗ Found 'gid:' usage in rules"
else
    echo "✓ No 'gid:' usage in rules"
fi

echo "Checking for 'philosophy_gid:' usage in rules..."
if find /home/x/Projects/akao/rules -name "*.yaml" -exec grep -l "philosophy_gid:" {} \; | head -5; then
    echo "✗ Found 'philosophy_gid:' usage in rules"
else
    echo "✓ No 'philosophy_gid:' usage in rules"
fi

echo "Checking for 'gid:' usage in philosophies (except index)..."
if find /home/x/Projects/akao/philosophies -name "*.yaml" -not -name "index.yaml" -exec grep -l "gid:" {} \; | head -5; then
    echo "✗ Found 'gid:' usage in philosophies"
else
    echo "✓ No 'gid:' usage in philosophies"
fi

echo ""
echo "Sample rule-philosophy pair check:"
echo "Rule: build/duality.v1.yaml"
echo "  ID: $(grep 'id:' /home/x/Projects/akao/rules/build/duality.v1.yaml | head -1)"
echo "  Philosophy: $(grep 'philosophy:' /home/x/Projects/akao/rules/build/duality.v1.yaml)"
echo "Philosophy: build/duality.v1.yaml"
echo "  ID: $(grep 'id:' /home/x/Projects/akao/philosophies/build/duality.v1.yaml | head -1)"

echo ""
echo "=== VALIDATION COMPLETE ==="
