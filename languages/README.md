# Languages Directory

This directory contains language-specific adapters for Akao's universal validation system.

## Structure

- `cpp/` - C++ language adapter and validators
- `rust/` - Rust language adapter and validators  
- `python/` - Python language adapter and validators
- `javascript/` - JavaScript language adapter and validators
- `go/` - Go language adapter and validators
- `common/` - Common language operations and utilities

## Philosophy Compliance

- `akao:philosophy:language:isolation:v1` - Each language is isolated in its own adapter
- `akao:philosophy:interface:consistency:v1` - All adapters follow consistent interface patterns

## Usage

Language adapters are automatically detected and used by the universal validator
based on the project being validated.
