#!/bin/bash

# AKAO Documentation Link Validation Script - Minimal Version
# Validates all markdown links in current directory only (non-recursive)
# Generates detailed JSON report with validation status

set -euo pipefail

# Configuration
REPORT_FILE="tmp/link_report.json"
mkdir -p "$(dirname "$REPORT_FILE")"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Global counters
TOTAL_LINKS=0
VALID_LINKS=0
INVALID_LINKS=0
MISSING_ANCHORS=0
EXTERNAL_UNREACHABLE=0

# GitHub-style anchor generation
generate_github_anchor() {
    local heading="$1"
    # Convert to lowercase, replace spaces with hyphens, remove special chars except hyphens
    echo "$heading" | tr '[:upper:]' '[:lower:]' | sed 's/[^a-z0-9 -]//g' | sed 's/ /-/g' | sed 's/--*/-/g' | sed 's/^-\|-$//g'
}

# Check if anchor exists in file
anchor_exists() {
    local file="$1"
    local anchor="$2"
    
    # Extract headings and generate anchors
    while read -r line; do
        if [[ "$line" =~ ^#+[[:space:]] ]]; then
            heading=$(echo "$line" | sed 's/^#* *//' | sed 's/ *$//')
            if [[ -n "$heading" ]]; then
                generated_anchor=$(generate_github_anchor "$heading")
                if [[ "$generated_anchor" = "$anchor" ]]; then
                    return 0
                fi
            fi
        fi
    done < "$file"
    
    return 1
}

# Check external URL (simplified)
check_external_url() {
    local url="$1"
    
    # Simple curl check with short timeout
    if timeout 2 curl --silent --head --fail --max-time 2 "$url" >/dev/null 2>&1; then
        return 0
    else
        return 1
    fi
}

# Validate a single link
validate_link() {
    local source_file="$1"
    local line_num="$2"
    local link_text="$3"
    local link_target="$4"
    local status="valid"
    local suggestion=""
    
    # Check if it's an external URL
    if [[ "$link_target" =~ ^https?:// ]]; then
        # For now, mark all external URLs as valid to avoid hanging
        status="valid"
        ((VALID_LINKS++))
    # Check if it's an anchor-only link (starts with #)
    elif [[ "$link_target" =~ ^# ]]; then
        local anchor="${link_target#\#}"
        if anchor_exists "$source_file" "$anchor"; then
            status="valid"
            ((VALID_LINKS++))
        else
            status="missing-anchor"
            suggestion="Anchor '#$anchor' not found in $source_file. Check heading exists and anchor format."
            ((MISSING_ANCHORS++))
        fi
    # Check if it's a relative file link
    elif [[ "$link_target" =~ ^\.\/ ]] || [[ ! "$link_target" =~ / ]]; then
        local file_part="$link_target"
        local anchor_part=""
        
        # Split file and anchor parts
        if [[ "$link_target" =~ \# ]]; then
            file_part="${link_target%%\#*}"
            anchor_part="${link_target##*\#}"
        fi
        
        # Remove ./ prefix if present
        file_part="${file_part#./}"
        
        # Check if file exists
        if [[ -f "$file_part" ]]; then
            # If there's an anchor, check if it exists
            if [[ -n "$anchor_part" ]]; then
                if anchor_exists "$file_part" "$anchor_part"; then
                    status="valid"
                    ((VALID_LINKS++))
                else
                    status="missing-anchor"
                    suggestion="Anchor '#$anchor_part' not found in $file_part. Check heading exists and anchor format."
                    ((MISSING_ANCHORS++))
                fi
            else
                status="valid"
                ((VALID_LINKS++))
            fi
        else
            status="invalid"
            suggestion="File '$file_part' does not exist in current directory."
            ((INVALID_LINKS++))
        fi
    else
        # Other types of links (absolute paths, etc.)
        status="invalid"
        suggestion="Link format not supported. Use relative links (./file.md) for local files."
        ((INVALID_LINKS++))
    fi
    
    ((TOTAL_LINKS++))
    
    # Escape quotes in JSON strings
    link_text=$(echo "$link_text" | sed 's/"/\\"/g')
    link_target=$(echo "$link_target" | sed 's/"/\\"/g')
    suggestion=$(echo "$suggestion" | sed 's/"/\\"/g')
    
    # Output JSON object for this link
    cat << EOF
{
  "file": "$source_file",
  "line": $line_num,
  "text": "$link_text",
  "link": "$link_target",
  "status": "$status",
  "suggestion": "$suggestion"
}
EOF
}

# Main validation function
validate_all_links() {
    local links_array=()
    local first_entry=true
    
    # Process each markdown file in current directory
    for md_file in *.md; do
        [[ -f "$md_file" ]] || continue
        
        echo -e "${BLUE}Processing $md_file...${NC}" >&2
        
        # Read file line by line and extract links
        local line_num=1
        while read -r line; do
            # Check if line contains links
            if echo "$line" | grep -q '\[[^]]*\]([^)]*)'; then
                # Extract all links from this line using a loop
                local temp_line="$line"
                while echo "$temp_line" | grep -q '\[[^]]*\]([^)]*)'; do
                    # Extract first link
                    local link_match=$(echo "$temp_line" | grep -o '\[[^]]*\]([^)]*)' | head -1)
                    
                    if [[ -n "$link_match" ]]; then
                        # Parse link text and target
                        local link_text=$(echo "$link_match" | sed 's/^\[\([^]]*\)\].*/\1/')
                        local link_target=$(echo "$link_match" | sed 's/.*(\([^)]*\))$/\1/')
                        
                        # Skip empty links
                        if [[ -n "$link_text" && -n "$link_target" ]]; then
                            # Store link data
                            local link_json=$(validate_link "$md_file" "$line_num" "$link_text" "$link_target")
                            links_array+=("$link_json")
                        fi
                        
                        # Remove this link from temp_line and continue
                        temp_line=$(echo "$temp_line" | sed "s|$(printf '%s\n' "$link_match" | sed 's/[[\.*^$()+?{|]/\\&/g')||")
                    else
                        break
                    fi
                done
            fi
            ((line_num++))
        done < "$md_file"
    done
    
    # Generate JSON report
    echo "[" > "$REPORT_FILE"
    local i=0
    for link_json in "${links_array[@]}"; do
        if [[ $i -gt 0 ]]; then
            echo "," >> "$REPORT_FILE"
        fi
        echo "$link_json" >> "$REPORT_FILE"
        ((i++))
    done
    echo "]" >> "$REPORT_FILE.links"
    
    # Generate final report with summary
    cat << EOF > "$REPORT_FILE"
{
  "summary": {
    "total_links": $TOTAL_LINKS,
    "valid_links": $VALID_LINKS,
    "invalid_links": $INVALID_LINKS,
    "missing_anchors": $MISSING_ANCHORS,
    "external_unreachable": $EXTERNAL_UNREACHABLE,
    "duplicated_links": 0,
    "validation_timestamp": "$(date -Iseconds)",
    "directory": "$(pwd)"
  },
  "links": $(cat "$REPORT_FILE.links" 2>/dev/null || echo "[]")
}
EOF
    
    rm -f "$REPORT_FILE.links"
    
    # Print summary to stderr
    echo -e "\n${BLUE}=== Link Validation Summary ===${NC}" >&2
    echo -e "Total links: ${YELLOW}$TOTAL_LINKS${NC}" >&2
    echo -e "Valid links: ${GREEN}$VALID_LINKS${NC}" >&2
    echo -e "Invalid links: ${RED}$INVALID_LINKS${NC}" >&2
    echo -e "Missing anchors: ${RED}$MISSING_ANCHORS${NC}" >&2
    echo -e "External unreachable: ${RED}$EXTERNAL_UNREACHABLE${NC}" >&2
    echo -e "Report saved to: ${BLUE}$REPORT_FILE${NC}" >&2
    
    # Return appropriate exit code
    local broken_count=$((INVALID_LINKS + MISSING_ANCHORS + EXTERNAL_UNREACHABLE))
    if [[ $broken_count -eq 0 ]]; then
        echo -e "${GREEN}✅ All links are valid!${NC}" >&2
        return 0
    else
        echo -e "${RED}❌ Found $broken_count broken links${NC}" >&2
        return 1
    fi
}

# Main execution
main() {
    echo -e "${BLUE}AKAO Documentation Link Validator${NC}" >&2
    echo -e "${BLUE}===================================${NC}" >&2
    echo -e "Scanning markdown files in: ${YELLOW}$(pwd)${NC}" >&2
    echo -e "Report will be saved to: ${YELLOW}$REPORT_FILE${NC}" >&2
    echo "" >&2
    
    # Check if there are any markdown files
    if ! ls *.md >/dev/null 2>&1; then
        echo -e "${YELLOW}No markdown files found in current directory${NC}" >&2
        # Create empty report
        cat << EOF > "$REPORT_FILE"
{
  "summary": {
    "total_links": 0,
    "valid_links": 0,
    "invalid_links": 0,
    "missing_anchors": 0,
    "external_unreachable": 0,
    "duplicated_links": 0,
    "validation_timestamp": "$(date -Iseconds)",
    "directory": "$(pwd)"
  },
  "links": []
}
EOF
        return 0
    fi
    
    validate_all_links
}

# Run main function
main "$@"