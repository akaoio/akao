#!/bin/bash

# Simple test to check one file
set -euo pipefail

echo "Testing link extraction from README.md..."

# Extract just the first few links
grep -n '\[.*\](.*)' README.md | head -5 | while IFS=: read -r line_num line_content; do
    echo "Line $line_num: $line_content"
    
    # Extract links from this line
    echo "$line_content" | grep -oE '\[[^\]]*\]\([^)]*\)' | while read -r link_match; do
        echo "  Found link: $link_match"
        
        # Parse link components
        link_text=$(echo "$link_match" | sed 's/\[\([^]]*\)\].*/\1/')
        link_target=$(echo "$link_match" | sed 's/.*(\([^)]*\)).*/\1/')
        
        echo "    Text: '$link_text'"
        echo "    Target: '$link_target'"
        echo ""
    done
done