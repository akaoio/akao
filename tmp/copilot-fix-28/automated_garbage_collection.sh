#!/bin/bash

# AKAO Automated Garbage Collection System
# Manages branch-specific temporary directories and cleanup

AKAO_ROOT="/home/runner/work/akao/akao"
TMP_DIR="$AKAO_ROOT/tmp"
CURRENT_BRANCH=$(git branch --show-current | tr '/' '-')

# Create branch-specific directory if it doesn't exist
create_branch_tmp() {
    local branch_dir="$TMP_DIR/$CURRENT_BRANCH"
    if [ ! -d "$branch_dir" ]; then
        mkdir -p "$branch_dir"
        echo "Created branch-specific tmp directory: $branch_dir"
    fi
}

# Clean up old temporary files (older than 7 days)
cleanup_old_files() {
    echo "Cleaning up temporary files older than 7 days..."
    find "$TMP_DIR" -type f -mtime +7 -delete
    find "$TMP_DIR" -type d -empty -delete
}

# Clean up orphaned files (not in branch directories)
cleanup_orphaned_files() {
    echo "Moving orphaned files to branch-specific directory..."
    local branch_dir="$TMP_DIR/$CURRENT_BRANCH"
    
    # Move files from root tmp to branch-specific directory
    find "$TMP_DIR" -maxdepth 1 -type f -exec mv {} "$branch_dir/" \; 2>/dev/null
}

# Report cleanup statistics
report_cleanup_stats() {
    echo "Garbage Collection Report:"
    echo "========================="
    echo "Current branch: $CURRENT_BRANCH"
    echo "Branch tmp directory: $TMP_DIR/$CURRENT_BRANCH"
    echo "Total tmp directories: $(find $TMP_DIR -type d | wc -l)"
    echo "Total tmp files: $(find $TMP_DIR -type f | wc -l)"
    echo "Disk usage: $(du -sh $TMP_DIR | cut -f1)"
}

# Main execution
main() {
    echo "🧹 AKAO Automated Garbage Collection System"
    echo "==========================================="
    
    cd "$AKAO_ROOT" || exit 1
    
    create_branch_tmp
    cleanup_orphaned_files
    cleanup_old_files
    report_cleanup_stats
    
    echo "✅ Garbage collection completed successfully"
}

# Execute if run directly
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    main "$@"
fi