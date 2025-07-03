#!/bin/bash

# Simple test to extract just the first few links
cd /home/runner/work/akao/akao/tmp

echo "Testing with README.md only..."

# Extract just first 3 links
grep -n '\[.*\](.*)' README.md | head -3 | while IFS=: read -r line_num line_content; do
    echo "Line $line_num: Found link line"
    
    # Extract links from this line
    echo "$line_content" | grep -oE '\[[^\]]*\]\([^)]*\)' | head -3 | while read -r link_match; do
        echo "  Link match: $link_match"
        
        # Parse components
        link_text=$(echo "$link_match" | sed 's/^\[\([^]]*\)\].*/\1/')
        link_target=$(echo "$link_match" | sed 's/.*(\([^)]*\))$/\1/')
        
        echo "    Text: '$link_text'"
        echo "    Target: '$link_target'"
        echo ""
    done
done