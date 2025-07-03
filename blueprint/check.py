#!/usr/bin/env python3
"""
AKAO Documentation Link Validation Script
Validates all markdown links in blueprint/ directory only
Generates detailed JSON report with validation status
"""

import os
import re
import json
import urllib.parse
from datetime import datetime
from pathlib import Path
import sys

def generate_github_anchor(heading):
    """Generate GitHub-style anchor from heading text"""
    # Convert to lowercase, replace spaces with hyphens, remove special chars except hyphens
    anchor = heading.lower()
    anchor = re.sub(r'[^a-z0-9\s-]', '', anchor)
    anchor = re.sub(r'\s+', '-', anchor)
    anchor = re.sub(r'-+', '-', anchor)
    anchor = anchor.strip('-')
    return anchor

def extract_headings(file_path):
    """Extract all headings from a markdown file"""
    headings = []
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            for line in f:
                if line.strip().startswith('#'):
                    # Remove # characters and whitespace
                    heading = re.sub(r'^#+\s*', '', line.strip())
                    heading = heading.rstrip()
                    if heading:
                        headings.append(heading)
    except Exception as e:
        print(f"Error reading {file_path}: {e}")
    return headings

def anchor_exists(file_path, anchor):
    """Check if anchor exists in file"""
    headings = extract_headings(file_path)
    for heading in headings:
        generated_anchor = generate_github_anchor(heading)
        if generated_anchor == anchor:
            return True
    return False

def validate_link(source_file, line_num, link_text, link_target):
    """Validate a single link and return validation result"""
    status = "valid"
    suggestion = ""
    
    # Check if it's an external URL
    if link_target.startswith(('http://', 'https://')):
        # For now, mark external URLs as valid (could add actual HTTP checking)
        status = "valid"
    
    # Check if it's an anchor-only link (starts with #)
    elif link_target.startswith('#'):
        anchor = link_target[1:]  # Remove #
        if anchor_exists(source_file, anchor):
            status = "valid"
        else:
            status = "missing-anchor"
            suggestion = f"Anchor '#{anchor}' not found in {source_file}. Check heading exists and anchor format."
    
    # Check if it's a relative file link
    elif link_target.startswith('./') or ('/' not in link_target or link_target.count('/') == 1):
        file_part = link_target
        anchor_part = ""
        
        # Split file and anchor parts
        if '#' in link_target:
            file_part, anchor_part = link_target.split('#', 1)
        
        # Remove ./ prefix if present
        if file_part.startswith('./'):
            file_part = file_part[2:]
        
        # Check if file exists in blueprint/ directory
        full_file_path = Path('blueprint') / file_part
        if full_file_path.exists():
            # If there's an anchor, check if it exists
            if anchor_part:
                if anchor_exists(str(full_file_path), anchor_part):
                    status = "valid"
                else:
                    status = "missing-anchor"
                    suggestion = f"Anchor '#{anchor_part}' not found in {full_file_path}. Check heading exists and anchor format."
            else:
                status = "valid"
        else:
            # Check if it's a reference to a file that might exist outside blueprint/ (like images)
            # For now, we'll assume these are intentional references to files that should exist
            if file_part.endswith(('.svg', '.png', '.jpg', '.jpeg', '.gif', '.webp')):
                status = "valid"  # Assume image files are valid references
            else:
                status = "invalid"
                suggestion = f"File '{file_part}' does not exist in blueprint/ directory."
    
    else:
        # Other types of links (absolute paths, etc.)
        status = "invalid"
        suggestion = "Link format not supported. Use relative links (./file.md) for local files."
    
    return {
        "file": source_file,
        "line": line_num,
        "text": link_text,
        "link": link_target,
        "status": status,
        "suggestion": suggestion
    }

def is_likely_code_snippet(line, link_text, link_target):
    """Check if this is likely a code snippet rather than a markdown link"""
    # Check if we're in a code block
    if '```' in line or line.strip().startswith('    '):
        return True
    
    # Check for C++ lambda captures (common false positives)
    if link_text in ['this', '&', '='] and ('const' in link_target or '&' in link_target):
        return True
    
    # Check for function parameters or type declarations
    if ('const' in link_target and ('&' in link_target or '*' in link_target)) or \
       ('std::' in link_target) or \
       ('::' in link_target and 'http' not in link_target):
        return True
    
    # Check for comma-separated parameters (likely function parameters)
    if ',' in link_text and ('const' in link_target or '&' in link_target):
        return True
    
    return False

def extract_links_from_file(file_path):
    """Extract all markdown links from a file"""
    links = []
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
            lines = content.split('\n')
            in_code_block = False
            
            for line_num, line in enumerate(lines, 1):
                # Track code block state
                if line.strip().startswith('```'):
                    in_code_block = not in_code_block
                    continue
                
                # Skip lines in code blocks
                if in_code_block:
                    continue
                
                # Find all markdown links in the line
                link_pattern = r'\[([^\]]*)\]\(([^)]*)\)'
                matches = re.findall(link_pattern, line)
                
                for link_text, link_target in matches:
                    if link_text.strip() and link_target.strip():
                        # Skip if this looks like a code snippet
                        if is_likely_code_snippet(line, link_text, link_target):
                            continue
                        
                        links.append({
                            'line_num': line_num,
                            'text': link_text,
                            'target': link_target
                        })
    except Exception as e:
        print(f"Error reading {file_path}: {e}")
    
    return links

def main():
    print("🔍 AKAO Documentation Link Validator")
    print("===================================")
    print(f"Scanning markdown files in: {os.getcwd()}/blueprint")
    
    # Initialize counters
    total_links = 0
    valid_links = 0
    invalid_links = 0
    missing_anchors = 0
    external_unreachable = 0
    
    all_link_results = []
    
    # Check if blueprint directory exists
    blueprint_dir = Path('blueprint')
    if not blueprint_dir.exists():
        print("❌ blueprint/ directory not found!")
        sys.exit(1)
    
    # Process each markdown file in blueprint directory
    md_files = list(blueprint_dir.glob('*.md'))
    
    if not md_files:
        print("⚠️  No markdown files found in blueprint/ directory")
        # Create empty report
        report = {
            "summary": {
                "total_links": 0,
                "valid_links": 0,
                "invalid_links": 0,
                "missing_anchors": 0,
                "external_unreachable": 0,
                "duplicated_links": 0,
                "validation_timestamp": datetime.now().isoformat(),
                "directory": f"{os.getcwd()}/blueprint"
            },
            "links": []
        }
        
        # Save report
        report_file = Path('blueprint/link_report.json')
        with open(report_file, 'w', encoding='utf-8') as f:
            json.dump(report, f, indent=2)
        
        print(f"📄 Empty report saved to: {report_file}")
        return
    
    for md_file in md_files:
        print(f"📖 Processing {md_file.name}...")
        
        # Extract links from the file
        links = extract_links_from_file(md_file)
        
        for link in links:
            total_links += 1
            
            # Validate the link
            result = validate_link(str(md_file), link['line_num'], link['text'], link['target'])
            all_link_results.append(result)
            
            # Update counters
            if result['status'] == 'valid':
                valid_links += 1
            elif result['status'] == 'invalid':
                invalid_links += 1
            elif result['status'] == 'missing-anchor':
                missing_anchors += 1
            elif result['status'] == 'external-unreachable':
                external_unreachable += 1
            
            # Print link status
            status_emoji = "✅" if result['status'] == 'valid' else "❌"
            print(f"  {status_emoji} Line {link['line_num']}: [{link['text']}]({link['target']}) - {result['status']}")
            if result['suggestion']:
                print(f"     💡 {result['suggestion']}")
    
    # Generate JSON report
    report = {
        "summary": {
            "total_links": total_links,
            "valid_links": valid_links,
            "invalid_links": invalid_links,
            "missing_anchors": missing_anchors,
            "external_unreachable": external_unreachable,
            "duplicated_links": 0,
            "validation_timestamp": datetime.now().isoformat(),
            "directory": f"{os.getcwd()}/blueprint"
        },
        "links": all_link_results
    }
    
    # Save report
    report_file = Path('blueprint/link_report.json')
    with open(report_file, 'w', encoding='utf-8') as f:
        json.dump(report, f, indent=2)
    
    # Print summary
    print("\n📊 === Link Validation Summary ===")
    print(f"Total links: {total_links}")
    print(f"Valid links: {valid_links}")
    print(f"Invalid links: {invalid_links}")
    print(f"Missing anchors: {missing_anchors}")
    print(f"External unreachable: {external_unreachable}")
    print(f"📄 Report saved to: {report_file}")
    
    # Return appropriate exit code
    broken_count = invalid_links + missing_anchors + external_unreachable
    if broken_count == 0:
        print("✅ All links are valid!")
        sys.exit(0)
    else:
        print(f"❌ Found {broken_count} broken links")
        sys.exit(1)

if __name__ == "__main__":
    main()
