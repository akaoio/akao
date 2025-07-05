#!/bin/bash

# Script to standardize all philosophy and rule files
# Ensures file names match GIDs and metadata is consistent

cd /home/x/Projects/akao

echo "=== STANDARDIZING PHILOSOPHY FILES ==="

# Function to extract GID from file and verify naming
check_philosophy_file() {
    local file="$1"
    local dir=$(dirname "$file")
    local filename=$(basename "$file")
    
    # Extract GID from file
    local gid=$(grep -E '^\s*gid:\s*"' "$file" | head -1 | sed 's/.*gid:\s*"\([^"]*\)".*/\1/')
    
    if [ -z "$gid" ]; then
        echo "❌ No GID found in $file"
        return 1
    fi
    
    # Extract expected filename from GID
    # Format: akao:philosophy::category:name:v1 -> name.v1.yaml
    local expected_name=$(echo "$gid" | sed 's/.*::\([^:]*\):\([^:]*\):v1/\2.v1.yaml/')
    local expected_category=$(echo "$gid" | sed 's/.*::\([^:]*\):\([^:]*\):v1/\1/')
    
    echo "File: $file"
    echo "  GID: $gid"
    echo "  Expected name: $expected_name"
    echo "  Current name: $filename"
    echo "  Expected category: $expected_category"
    echo "  Current category: $(basename "$dir")"
    
    # Check if filename matches
    if [ "$filename" != "$expected_name" ]; then
        echo "  ❌ FILENAME MISMATCH: $filename should be $expected_name"
        echo "  🔧 Renaming..."
        mv "$file" "$dir/$expected_name"
        echo "  ✅ Renamed to $expected_name"
    else
        echo "  ✅ Filename correct"
    fi
    
    echo ""
}

# Check all philosophy files
find philosophies -name "*.v1.yaml" | while read file; do
    check_philosophy_file "$file"
done

echo "=== STANDARDIZING RULE FILES ==="

# Function to check rule files
check_rule_file() {
    local file="$1"
    local dir=$(dirname "$file")
    local filename=$(basename "$file")
    
    # Extract GID from file
    local gid=$(grep -E '^\s*gid:\s*"' "$file" | head -1 | sed 's/.*gid:\s*"\([^"]*\)".*/\1/')
    
    if [ -z "$gid" ]; then
        echo "❌ No GID found in $file"
        return 1
    fi
    
    # Extract expected filename from GID
    # Format: akao:rule::category:name:v1 -> name.v1.yaml
    local expected_name=$(echo "$gid" | sed 's/.*::\([^:]*\):\([^:]*\):v1/\2.v1.yaml/')
    local expected_category=$(echo "$gid" | sed 's/.*::\([^:]*\):\([^:]*\):v1/\1/')
    
    echo "File: $file"
    echo "  GID: $gid"
    echo "  Expected name: $expected_name"
    echo "  Current name: $filename"
    echo "  Expected category: $expected_category"
    echo "  Current category: $(basename "$dir")"
    
    # Check if filename matches
    if [ "$filename" != "$expected_name" ]; then
        echo "  ❌ FILENAME MISMATCH: $filename should be $expected_name"
        echo "  🔧 Renaming..."
        mv "$file" "$dir/$expected_name"
        echo "  ✅ Renamed to $expected_name"
    else
        echo "  ✅ Filename correct"
    fi
    
    echo ""
}

# Check all rule files
find rules -name "*.v1.yaml" | while read file; do
    check_rule_file "$file"
done

echo "=== SUMMARY ==="
echo "Philosophy files: $(find philosophies -name "*.v1.yaml" | wc -l)"
echo "Rule files: $(find rules -name "*.v1.yaml" | wc -l)"
