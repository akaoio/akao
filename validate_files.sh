#!/bin/bash

# Script to check and report file naming and metadata consistency

cd /home/x/Projects/akao

echo "=== PHILOSOPHY FILES VALIDATION ==="
echo ""

total_files=0
correct_files=0
incorrect_files=0

# Check philosophy files
find philosophies -name "*.v1.yaml" | while read file; do
    total_files=$((total_files + 1))
    
    echo "📄 Checking: $file"
    
    # Extract GID
    gid=$(awk '/gid:/ {gsub(/[^"]*"([^"]*)".*/, "\\1"); print; exit}' "$file")
    
    if [ -z "$gid" ]; then
        echo "  ❌ No GID found"
        incorrect_files=$((incorrect_files + 1))
        continue
    fi
    
    # Parse GID: akao:philosophy::category:name:v1
    category=$(echo "$gid" | cut -d':' -f4)
    name=$(echo "$gid" | cut -d':' -f5)
    
    # Expected path and filename
    expected_path="philosophies/$category/$name.v1.yaml"
    current_path="$file"
    
    echo "  📍 GID: $gid"
    echo "  📂 Expected: $expected_path"
    echo "  📂 Current:  $current_path"
    
    if [ "$current_path" = "$expected_path" ]; then
        echo "  ✅ PATH CORRECT"
        correct_files=$((correct_files + 1))
    else
        echo "  ❌ PATH MISMATCH"
        echo "  🔧 Should rename to: $expected_path"
        incorrect_files=$((incorrect_files + 1))
        
        # Create directory if needed and rename
        mkdir -p "philosophies/$category"
        mv "$current_path" "$expected_path"
        echo "  ✅ RENAMED TO CORRECT PATH"
    fi
    
    echo ""
done

echo "=== RULES FILES VALIDATION ==="
echo ""

# Check rule files
find rules -name "*.v1.yaml" | while read file; do
    echo "📄 Checking: $file"
    
    # Extract GID
    gid=$(awk '/gid:/ {gsub(/[^"]*"([^"]*)".*/, "\\1"); print; exit}' "$file")
    
    if [ -z "$gid" ]; then
        echo "  ❌ No GID found"
        continue
    fi
    
    # Parse GID: akao:rule::category:name:v1
    category=$(echo "$gid" | cut -d':' -f4)
    name=$(echo "$gid" | cut -d':' -f5)
    
    # Expected path and filename
    expected_path="rules/$category/$name.v1.yaml"
    current_path="$file"
    
    echo "  📍 GID: $gid"
    echo "  📂 Expected: $expected_path"
    echo "  📂 Current:  $current_path"
    
    if [ "$current_path" = "$expected_path" ]; then
        echo "  ✅ PATH CORRECT"
    else
        echo "  ❌ PATH MISMATCH"
        echo "  🔧 Should rename to: $expected_path"
        
        # Create directory if needed and rename
        mkdir -p "rules/$category"
        mv "$current_path" "$expected_path"
        echo "  ✅ RENAMED TO CORRECT PATH"
    fi
    
    echo ""
done

echo "=== FINAL SUMMARY ==="
echo "Philosophy files: $(find philosophies -name "*.v1.yaml" | wc -l)"
echo "Rule files: $(find rules -name "*.v1.yaml" | wc -l)"
