#!/bin/bash

# Simple test with just README.md
set -euo pipefail

echo "Testing with just README.md..."

# Create temp dir
TEMP_DIR="/tmp/simple_test_$$"
mkdir -p "$TEMP_DIR"
cd /home/runner/work/akao/akao/tmp

echo "Extracting links from README.md..."

# Extract all lines with links
grep -n '\[.*\](.*)' README.md > "$TEMP_DIR/readme_links.txt" || true

echo "Found $(wc -l < "$TEMP_DIR/readme_links.txt") lines with links"

# Show first few
echo "First 5 links:"
head -5 "$TEMP_DIR/readme_links.txt"

echo ""
echo "Processing first 3 lines..."

# Process just first 3 lines
head -3 "$TEMP_DIR/readme_links.txt" | while IFS=: read -r line_num line_content; do
    echo "Line $line_num:"
    
    # Extract patterns
    echo "$line_content" | grep -oE '\[[^\]]*\]\([^)]*\)' > "$TEMP_DIR/patterns_${line_num}.txt" || true
    
    echo "  Found $(wc -l < "$TEMP_DIR/patterns_${line_num}.txt") patterns"
    
    # Show patterns
    while read -r pattern; do
        if [[ -n "$pattern" ]]; then
            echo "    Pattern: $pattern"
            # Parse
            text=$(echo "$pattern" | sed 's/^\[\([^]]*\)\].*/\1/')
            target=$(echo "$pattern" | sed 's/.*(\([^)]*\))$/\1/')
            echo "      Text: '$text'"
            echo "      Target: '$target'"
        fi
    done < "$TEMP_DIR/patterns_${line_num}.txt"
    
    echo ""
done

rm -rf "$TEMP_DIR"