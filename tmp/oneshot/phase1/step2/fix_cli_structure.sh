#!/bin/bash

# Fix CLI Command Structure Violations - Phase 1 Step 2
# Resolves one-file-per-directory rule violations in interfaces/cli/command/

echo "🔧 Fixing CLI Command Structure Violations"

# Create backup
echo "📋 Creating backup..."
cp -r interfaces/cli/command interfaces/cli/command_backup

# For each command, split .hpp and .cpp into separate directories
commands=("additional" "build" "generate" "init" "test" "validate")

for cmd in "${commands[@]}"; do
    echo "🔧 Restructuring command: $cmd"
    
    # Create header and implementation directories
    mkdir -p "interfaces/cli/command/$cmd/header"
    mkdir -p "interfaces/cli/command/$cmd/implementation"
    
    # Move files
    if [ -f "interfaces/cli/command/$cmd/v1.hpp" ]; then
        mv "interfaces/cli/command/$cmd/v1.hpp" "interfaces/cli/command/$cmd/header/v1.hpp"
        echo "  ✅ Moved $cmd header to header/ subdirectory"
    fi
    
    if [ -f "interfaces/cli/command/$cmd/v1.cpp" ]; then
        mv "interfaces/cli/command/$cmd/v1.cpp" "interfaces/cli/command/$cmd/implementation/v1.cpp"
        echo "  ✅ Moved $cmd implementation to implementation/ subdirectory"
    fi
done

# Update include paths in the moved files
echo "🔧 Updating include paths..."

# Update .cpp files to reference new header paths
for cmd in "${commands[@]}"; do
    if [ -f "interfaces/cli/command/$cmd/implementation/v1.cpp" ]; then
        sed -i 's/#include "v1\.hpp"/#include "..\/header\/v1.hpp"/g' "interfaces/cli/command/$cmd/implementation/v1.cpp"
        echo "  ✅ Updated include path in $cmd implementation"
    fi
done

# Update main.cpp and other files that include these headers
echo "🔧 Updating references in main.cpp..."
sed -i 's|interfaces/cli/command/\([^/]*\)/v1\.hpp|interfaces/cli/command/\1/header/v1.hpp|g' main.cpp

echo "✅ CLI Command structure violations fixed"
echo "📊 Each command directory now follows one-file-per-directory rule"