#!/bin/bash

# Fix ID format in all YAML files
# Convert category/name to category:name format

echo "=== FIXING ID FORMAT IN ALL FILES ==="

# Function to fix ID format in a file
fix_id_format() {
    local file="$1"
    local type="$2"  # "rule" or "philosophy"
    
    echo "Fixing ID format in $type: $file"
    
    # Create backup
    cp "$file" "$file.id_fix_backup"
    
    # Get category and name from file path
    local category=$(basename $(dirname "$file"))
    local name=$(basename "$file" .v1.yaml)
    
    if [ "$type" = "rule" ]; then
        # Fix rule ID
        sed -i "s|id: \"akao:rule::$category/|id: \"akao:rule::$category:|g" "$file"
        
        # Fix philosophy reference  
        sed -i "s|philosophy: \"$category/|philosophy: \"akao:philosophy::$category:|g" "$file"
        sed -i "s|philosophy: \"akao:philosophy::$category/|philosophy: \"akao:philosophy::$category:|g" "$file"
        
        # Fix header comment
        sed -i "s|# Implements: akao:philosophy::$category/|# Philosophy: akao:philosophy::$category:|g" "$file"
        
    elif [ "$type" = "philosophy" ]; then
        # Fix philosophy ID
        sed -i "s|id: \"akao:philosophy::$category/|id: \"akao:philosophy::$category:|g" "$file"
    fi
    
    echo "✓ Fixed ID format: $file"
}

# Process all rule files
echo "Processing rule files..."
find /home/x/Projects/akao/rules -name "*.yaml" -type f | while read file; do
    if grep -q 'id:.*/' "$file"; then
        fix_id_format "$file" "rule"
    else
        echo "Skipping (already correct): $file"
    fi
done

# Process all philosophy files (except index.yaml)
echo "Processing philosophy files..."
find /home/x/Projects/akao/philosophies -name "*.yaml" -type f | grep -v index.yaml | while read file; do
    if grep -q 'id:.*/' "$file"; then
        fix_id_format "$file" "philosophy"
    else
        echo "Skipping (already correct): $file"
    fi
done

echo "=== ID FORMAT FIX COMPLETE ==="
echo "Backup files created with .id_fix_backup extension"
