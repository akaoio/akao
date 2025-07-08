📋 Akao Universal Standards

**Purpose**: Complete Akao system standardization with archaeological preservation

## 🆔 Namespace Pattern
```
akao:[type]:[category]:[subcategory]:[name]:[version]
```
**Types**: philosophy, rule, artifact, file, function, class, directory
**Examples**: `akao:function:core:engine:validate_rule:v1`

## 📁 Naming Conventions
### Files: `{function-folder}/{version}.{ext}`
- Version-only naming: `v1.cpp`, `v2.hpp`
- kebab-case, no underscores, max 2 words

### Directories: `{domain}/{subdomain}/{function}/`
- No versions, kebab-case, max 30 chars
- Singular functions, plural collections

**Examples**:
```
✅ Good: validator/v1.cpp, core/engine/validator/, tests/unit/
❌ Bad: rule_validator/v1.cpp, validators/v1.cpp, core/engines/
```

## 📝 Metadata Requirements
**8 Mandatory Fields**:
```cpp
/**
 * @id: akao:[type]:[category]:[subcategory]:[name]:v1
 * @doc: User documentation (20-200 words) [MANDATORY for code]
 * @specification: Technical spec (10-100 words)
 * @scope: Architectural location
 * @timeline: YYYY-MM-DD
 * @rationale: Why it exists
 * @methodology: How it works
 * @references: [Related akao IDs/URLs]
 */
```

**Directory Identification**:
```yaml
# _.yaml in every directory
id: "akao:directory:[category]:[name]:v1"
metadata: {specification, scope, timeline, rationale, methodology, references}
```

## 📚 Grammar Rules (STRICT)
- **Functional**: Always singular (`validator/`, `parser/`)
- **Collections**: Always plural (`tests/`, `docs/`, `rules/`)
- **Compounds**: Adjective+Noun (`syntax-parser/` NOT `parser-syntax/`)

## ✅ Validation & Quality
**Blocking Errors**:
- Missing metadata fields, @doc for code, invalid references
- Grammar violations, underscores, versions in directories

**Quality Requirements**:
- Archaeological preservation, philosophy-rule separation
- Universal parser compatibility, deterministic validation

## 🏛️ Core Principles
1. **Archaeological Preservation**: Complete development documentation
2. **Systematic Governance**: Mandatory identification standards
3. **Architectural Coherence**: Philosophy-rule separation
4. **Universal Compliance**: Deterministic validation system

**Remember**: Every component becomes archaeological data for future agents.
