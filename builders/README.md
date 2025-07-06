# Builders Directory

This directory contains build system adapters for Akao's build duality support.

## Structure

- `cmake/` - CMake build system adapter
- `make/` - Make build system adapter
- `ninja/` - Ninja build system adapter
- `cargo/` - Rust Cargo build system adapter
- `common/` - Common build operations and utilities

## Philosophy Compliance

- `akao:philosophy:build:duality:v1` - Support for both development and production modes
- `akao:philosophy:interface:consistency:v1` - All builders follow consistent interface patterns

## Usage

Build adapters are automatically selected based on the project's build configuration
and the `akao build` command requirements.
