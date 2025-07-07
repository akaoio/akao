#!/bin/bash

echo "🔧 Fixing include paths in CLI command implementation files"

# Fix include paths in all implementation files
commands=("additional" "build" "generate" "init" "test" "validate")

for cmd in "${commands[@]}"; do
    impl_file="interfaces/cli/command/$cmd/implementation/v1.cpp"
    if [ -f "$impl_file" ]; then
        echo "🔧 Fixing includes in $cmd implementation..."
        
        # Update relative paths from implementation/ subdirectory
        sed -i 's|#include "../../|#include "../../../|g' "$impl_file"
        sed -i 's|#include "../../../../core/|#include "../../../../../core/|g' "$impl_file"
        sed -i 's|#include "../../../../interfaces/|#include "../../../../../interfaces/|g' "$impl_file"
        
        echo "  ✅ Fixed include paths in $cmd implementation"
    fi
done

echo "✅ All CLI command implementation include paths fixed"