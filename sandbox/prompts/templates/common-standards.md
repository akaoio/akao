📋 Akao Universal Standards

**Purpose**: Standardize the entire Akao system with archaeological preservation and systematic governance

## 🆔 Namespace Pattern (Mandatory)

### Pattern:
```
akao:[type]:[category]:[subcategory]:[name]:[version]
```

### Component Types:
- `philosophy` - Abstract principles and concepts
- `rule` - Concrete executable logic  
- `ruleset` - Collections of related rules
- `artifact` - Development artifacts and plans
- `file` - Source code files and documentation
- `trace` - Runtime execution traces
- `test` - Test cases and validation
- `function` - Individual functions and methods
- `class` - Classes, structs, and data types
- `directory` - Folders and directory structures

### Examples:
- Philosophy: `akao:philosophy:automation:updates:v1`
- Rule: `akao:rule:structure:file_organization:v1` 
- Function: `akao:function:core:engine:validate_rule:v1`
- Class: `akao:class:core:engine:rule_validator:v1`
- Directory: `akao:directory:core:engine:v1`

## 📁 Naming Conventions

### File Naming (Mandatory Pattern):
```
{function-folder}/{version}.{ext}
```

**Rules:**
- File names MUST NOT contain function/feature names
- File names MUST be version-only: `{version}.{ext}` or `{version}`
- Use lowercase with hyphens: `kebab-case`
- Only allowed characters: `a-z`, `0-9`, `-`, `.`
- **NO underscores (_)** allowed in file names
- **Maximum 2 words** (prefer single words)
- Maximum version name: 20 characters

**Examples:**
```
✅ Good:
validator/v1.cpp
parser/v2.hpp
rule-validator/v1.cpp
syntax-parser/v2.hpp

❌ Bad:
rule_validator/v1.cpp        # underscore not allowed
validator.cpp                # no folder structure
rule-validator-v1.cpp        # version in filename
validators/v1.cpp            # wrong plural
```

### Directory Naming (Mandatory):
```
{domain}/{subdomain}/{function}/
```

**Rules:**
- Directory names MUST NOT contain version numbers
- Use lowercase with hyphens: `kebab-case`
- Only allowed characters: `a-z`, `0-9`, `-`
- **Maximum 2 words** (prefer single words)
- **NO underscores (_)** allowed
- Maximum 30 characters
- Represent function/feature only

**Examples:**
```
✅ Good:
core/engine/validator/
core/parser/lexer/
tests/unit/runner/
docs/architecture/guide/

❌ Bad:
core/engine/validator-v1/    # version in directory
core/engine/rule_validator/  # underscore not allowed
core/engines/validator/      # wrong plural
```

## 📝 Metadata Requirements

### Universal Metadata Fields (ALL Components):
Every component with an akao ID MUST include these 8 fields:

```cpp
/**
 * @id: akao:[type]:[category]:[subcategory]:[name]:v1
 * @doc: User-facing documentation (20-200 words) for auto-generation [MANDATORY for code]
 * @specification: Technical specification (10-100 words) for developers
 * @scope: Architectural location and role within the system
 * @timeline: Creation/modification date (YYYY-MM-DD format)
 * @rationale: Business/technical justification for existence
 * @methodology: Implementation approach, algorithms, or design patterns
 * @references: [List of related akao IDs and external URLs]
 */
```

### Directory Identification (Mandatory):
```yaml
# _.yaml in every directory
id: "akao:directory:[category]:[subdomain]:[name]:v1"
metadata:
  specification: "Clear description of directory purpose"
  scope: "Architectural role and contents"
  timeline: "YYYY-MM-DD"
  rationale: "Why this directory exists"
  methodology: "Organization approach"
  references: ["akao:file:...", "akao:class:..."]
```

### Quality Standards:
- **@doc**: 20-200 words, user-friendly description for automatic documentation
- **specification**: 10-100 words, technical specification for developers
- **scope**: Precise architectural location (e.g., "Core engine validation subsystem")
- **timeline**: ISO date format (YYYY-MM-DD)
- **rationale**: Clear business/technical necessity (why it exists)
- **methodology**: Implementation details (how it works)
- **references**: Valid akao IDs or external URLs

## 📚 Grammar Compliance (STRICT)

### Mandatory Grammar Rules:

**Singular vs Plural Usage:**
- **Functional components**: Always singular (`validator/`, `parser/`, `engine/`)
- **Collection directories**: Always plural (`tests/`, `docs/`, `rules/`, `artifacts/`)
- **Mixed usage is FORBIDDEN**: No `rules-validator/` or `test-runners/`

**Compound Word Rules:**
- **Adjective + Noun**: `syntax-parser/` (correct), NOT `parser-syntax/`
- **Noun + Noun**: `rule-validator/` (both singular), NOT `rules-validator/`
- **Modifier + Base**: `unit-tester/` (correct), NOT `tester-unit/`

**Common Violations (FORBIDDEN):**
```
❌ Wrong Pluralization:
validators/     → validator/
parsers/        → parser/
configs/        → config/
engines/        → engine/

❌ Wrong Word Order:
parser-syntax/  → syntax-parser/
validator-rule/ → rule-validator/
analyzer-data/  → data-analyzer/

❌ Mixed Singular/Plural:
rules-validator/    → rule-validator/
tests-runner/       → test-runner/ or tests/runner/
docs-generator/     → doc-generator/ or docs/generator/
```

## ✅ Validation Rules

### Blocking Errors (Must Fix):
- Missing any metadata field = compilation/validation error
- Missing @doc field for code components = documentation generation error
- Invalid akao ID references = dependency validation error
- Malformed timeline = format validation error
- Empty or generic descriptions = quality validation error
- @doc field too short (<20 words) or too long (>200 words) = format validation error
- Grammatically incorrect names = grammar validation error
- Underscore usage in names = naming convention error
- Version numbers in directory names = structure validation error

### Quality Assurance Requirements:
- **No grammatically incorrect names**
- **No mixed singular/plural usage** 
- **No missing @doc fields for code components**
- **No namespace pattern violations**
- **Universal parser compatibility** (standard language constructs only)
- **Philosophy-rule separation** (no mixing)
- **Deterministic validation** with reproducible inputs/outputs

## 🏛️ Core Principles

1. **Archaeological Preservation**: Document all development actions for future analysis
2. **Systematic Governance**: Conform to identification and metadata standards
3. **Deterministic Validation**: Include reproducible testing with deterministic inputs/outputs
4. **Architectural Coherence**: Maintain philosophy-rule separation

## 🔧 Implementation Standards

### Code Quality Requirements:
- **Universal parser compatibility** (standard language constructs only)
- **Philosophy-rule separation** (no mixing)
- **Deterministic validation** with reproducible inputs/outputs
- **Archaeological preservation** through complete documentation

### Reference System:
- All references must be valid akao IDs or external URLs
- Cross-references between components must be maintained
- Dependency tracking through reference chains mandatory

**Remember**: Every contribution becomes archaeological data for future intelligent agents. Complete compliance is mandatory for system integrity.
