#!/bin/bash

echo "🔧 Fixing include paths in CLI command headers"

# Fix include paths in all header files
commands=("additional" "build" "generate" "init" "test" "validate")

for cmd in "${commands[@]}"; do
    header_file="interfaces/cli/command/$cmd/header/v1.hpp"
    if [ -f "$header_file" ]; then
        echo "🔧 Fixing includes in $cmd header..."
        
        # Update relative paths from header/ subdirectory
        sed -i 's|#include "../../executor/|#include "../../../executor/|g' "$header_file"
        sed -i 's|#include "../../formatter/|#include "../../../formatter/|g' "$header_file"
        sed -i 's|#include "../../parser/|#include "../../../parser/|g' "$header_file"
        
        echo "  ✅ Fixed include paths in $cmd header"
    fi
done

echo "✅ All CLI command header include paths fixed"