# Temporary Files Directory

This directory contains all temporary files and artifacts created during testing and development.

## Policy

- **All test artifacts must go here** - No test files in root directory
- **Auto-cleanup** - Contents may be deleted at any time
- **Not tracked in git** - Add tmp/* to .gitignore

## Structure

- `test_workspace_cli/` - CLI command test workspace
- `test_build/` - Test build artifacts (when testing build command)
- `*.tmp` - Temporary files
- `test_*/` - Any test-specific directories

## Production vs Testing

- `build/` (root) - **Production builds only**
- `tmp/` - **All testing artifacts**

This separation ensures a clean root directory and prevents test pollution.
