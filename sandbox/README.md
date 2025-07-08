# Sandbox Directory

## Purpose

The sandbox directory serves as an unrestricted development environment for experimental code, prototypes, and non-production assets that exist outside the formal system architecture.

## Scope

This directory contains:
- Experimental implementations and proof-of-concepts
- Temporary development artifacts
- Research and exploration code
- Third-party integrations testing
- Development utilities and scripts
- Non-production prototypes

## Validation Policy

**No Validation Required**: Contents of this directory are explicitly excluded from:
- System validation processes (`akao validate`)
- Quality assurance checks
- Compliance requirements
- Production deployment pipelines
- Formal code review processes

## Usage Guidelines

### Permitted Activities
- Rapid prototyping and experimentation
- Algorithm testing and validation
- Integration research with external libraries
- Development tool creation
- Learning and educational exercises

### Restrictions
- **No Production Dependencies**: Production code must not depend on sandbox contents
- **No Critical Logic**: Business-critical functionality should not reside here
- **Temporary Nature**: Consider sandbox contents as ephemeral

## Organization

While no formal structure is enforced, consider organizing by:
- `experiments/` - Research and proof-of-concept code
- `prototypes/` - Early-stage implementations
- `tools/` - Development utilities
- `temp/` - Temporary files and quick tests

## Migration Path

When sandbox code matures and proves valuable:
1. Refactor according to system standards
2. Move to appropriate production directories
3. Apply formal validation processes
4. Update documentation and tests

## Warning

⚠️ **Development Use Only**: Contents are not guaranteed to be functional, secure, or maintained. Use at your own discretion.