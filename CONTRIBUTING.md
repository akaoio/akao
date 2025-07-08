# Contributing to Akao

## Unified Identification Standard

All system components must use the akao namespace identification scheme:

### **Pattern**: `akao:[type]:[category]:[subcategory]:[name]:[version]`

**Component Types:**
- `philosophy` - Abstract principles and concepts
- `rule` - Concrete executable logic  
- `ruleset` - Collections of related rules
- `artifact` - Development artifacts and plans
- `file` - Source code files and documentation
- `trace` - Runtime execution traces
- `test` - Test cases and validation

**Examples:**
- Philosophy: `akao:philosophy:automation:updates:v1`
- Rule: `akao:rule:structure:file_organization:v1` 
- Artifact: `akao:artifact:feature-parser:phase-1:step-2:v1`
- File: `akao:file:core:engine:validator:v1`
- Trace: `akao:trace:session-20250708:violation-001:v1`

## Core Principles

1. **Archaeological Preservation**: Document all development actions for future analysis
2. **Systematic Governance**: Conform to identification and metadata standards
3. **Deterministic Validation**: Include reproducible testing with deterministic inputs/outputs
4. **Architectural Coherence**: Maintain philosophy-rule separation

## Workflow

### 1. Branch Setup
```bash
git checkout -b feature-description  # Use only a-z, A-Z, 0-9, "-"
mkdir -p artifacts/feature-description
touch artifacts/feature-description/{plan.yaml,checklist.md,quality.yaml}
```

### 2. Required plan.yaml Structure
```yaml
id: "akao:artifact:feature-description:plan:v1"
metadata:
  specification: "Feature description"
  scope: "Affected components"
  timeline: "Timeline"
  rationale: "Justification"
  methodology: "Implementation approach"
  references: ["URLs"]
phases:
  - id: "akao:artifact:feature-description:phase-1:v1"
    steps:
      - id: "akao:artifact:feature-description:phase-1:step-1:v1"
        inputs: []
        outputs: []
        validation: {}
```

### 3. File Headers (Mandatory)
```cpp
/**
 * @id: akao:file:[category]:[subcategory]:[name]:v1
 * @specification: Purpose and functionality
 * @scope: Location within system architecture
 * @timeline: Creation/modification date
 * @rationale: Justification for existence
 * @methodology: Implementation approach
 * @references: [dependencies, references]
 */
```

### 4. Pre-Commit Validation
```bash
akao validate --comprehensive
akao test --deterministic
akao analyze --compliance
```

## Requirements

- All IDs must follow akao namespace pattern
- All files must include metadata headers
- Phase/step directories: `artifacts/{branch}/{phase}/{step}/{unit_tests,inputs,outputs,assets}/`
- Deterministic tests with reproducible inputs/outputs
- Philosophy-rule separation (no mixing)
- Universal parser compatibility (standard language constructs only)

**Remember**: Every contribution becomes archaeological data for future intelligent agents.
