#!/bin/bash

# Mass standardization script for all philosophy and rule files
# Converts gid -> id, philosophy_gid -> philosophy

echo "=== MASS STANDARDIZATION OF AKAO PHILOSOPHY/RULE FILES ==="

# Function to standardize a single file
standardize_file() {
    local file="$1"
    local type="$2"  # "rule" or "philosophy"
    
    echo "Standardizing $type: $file"
    
    # Create backup
    cp "$file" "$file.backup"
    
    # Basic replacements
    sed -i 's/gid:/id:/g' "$file"
    sed -i 's/philosophy_gid:/philosophy:/g' "$file"
    
    # For rules: update philosophy reference format
    if [ "$type" = "rule" ]; then
        # Update the header comment - remove gid reference
        sed -i 's/# Implements: akao:philosophy::/# Philosophy: /g' "$file"
        
        # Make sure philosophy field uses correct format
        local category=$(basename $(dirname "$file"))
        local name=$(basename "$file" .v1.yaml)
        sed -i "s/philosophy: \"akao:philosophy::.*\"/philosophy: \"$category\/$name\"/" "$file"
    fi
    
    echo "✓ Standardized: $file"
}

# Process all rule files
echo "Processing rule files..."
find /home/x/Projects/akao/rules -name "*.yaml" -type f | while read file; do
    standardize_file "$file" "rule"
done

# Process all philosophy files (except index.yaml)
echo "Processing philosophy files..."
find /home/x/Projects/akao/philosophies -name "*.yaml" -type f | grep -v index.yaml | while read file; do
    standardize_file "$file" "philosophy"
done

echo "=== STANDARDIZATION COMPLETE ==="
echo "Backup files created with .backup extension"
echo "Please review changes before committing"
