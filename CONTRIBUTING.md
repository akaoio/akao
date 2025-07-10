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
- `function` - Individual functions and methods
- `class` - Classes, structs, and data types
- `directory` - Folders and directory structures

**Examples:**
- Philosophy: `akao:philosophy:automation:updates:v1`
- Rule: `akao:rule:structure:file_organization:v1` 
- Artifact: `akao:artifact:feature-parser:phase-1:step-2:v1`
- File: `akao:file:core:engine:validator:v1`
- Trace: `akao:trace:session-20250708:violation-001:v1`

### **Component Identification Requirements**

All code components must include akao namespace identification:

**Functions:**
```cpp
/**
 * @id: akao:function:core:engine:validate_rule:v1
 * @doc: Validates rule syntax and semantic correctness against engine standards
 * @specification: Validates rule syntax and semantic correctness
 * @scope: Core engine rule validation module
 * @timeline: 2025-07-08
 * @rationale: Ensure rule integrity before execution
 * @methodology: Multi-stage validation pipeline
 * @references: ["akao:class:core:engine:rule_validator:v1"]
 */
bool validateRule(const Rule& rule) { ... }
```

**Classes:**
```cpp
/**
 * @id: akao:class:core:engine:rule_validator:v1
 * @doc: Main rule validation engine class providing comprehensive rule checking
 * @specification: Main rule validation engine class
 * @scope: Core engine validation subsystem
 * @timeline: 2025-07-08
 * @rationale: Centralized rule validation logic
 * @methodology: Object-oriented validation framework
 * @references: ["akao:function:core:engine:validate_rule:v1"]
 */
class RuleValidator { ... };
```

**Folders/Directories:**
Each directory must contain an `_.yaml` file:
```yaml
id: "akao:directory:core:engine:v1"
metadata:
  specification: "Core engine components"
  scope: "Rule validation and execution"
  timeline: "2025-07-08"
  rationale: "Central processing logic"
  methodology: "Object-oriented architecture"
  references: ["akao:file:core:engine:validator:v1"]
```

## Naming Conventions

### **File Naming Standards**

**Mandatory Pattern:**
```
{function_folder}/{version}.{ext}
```

**Rules:**
- File names MUST NOT contain function/feature names
- File names MUST be version-only: `{version}.{ext}` or `{version}`
- Use lowercase with hyphens: `kebab-case`
- Only allowed characters: `a-z`, `0-9`, `-`, `.`
- NO underscores (_) allowed in file names and folder names
- **Maximum 2 words in version names (prefer single words)**
- **STRICT English grammar: singular/plural forms MUST be correct**
- **NO grammatically incorrect English names allowed**
- Maximum version name: 20 characters

**Grammar Rules:**
- Use **singular** for single entities: `validator/`, `parser/`, `config/`
- Use **plural** for collections: `tests/`, `docs/`, `rules/`
- Compound words must be grammatically correct: `rule-validator` (not `rules-validator`)
- Adjective-noun order: `syntax-parser` (not `parser-syntax`)

**File Structure:**
```
validator/v1.cpp             # Singular - handles one validation
parser/v1.hpp                # Singular - one parser instance  
tester/v2.cpp                # Singular - one test runner
config/v1.yaml               # Singular - one configuration
rule/v1.md                   # Singular - one rule definition
```

**Examples:**
```
✅ Good (Correct Grammar):
validator/v1.cpp             # Singular - one validator
parser/v2.hpp                # Singular - one parser
rule-validator/v1.cpp        # Compound: rule (singular) + validator (singular)
syntax-parser/v2.hpp         # Adjective + noun (correct order)

✅ Collections (Plural when appropriate):
tests/runner/v1.cpp          # Plural directory for collection
docs/guide/v1.md             # Plural directory for collection
rules/loader/v1.cpp          # Plural directory for collection

❌ Bad (Incorrect Grammar):
validators/v1.cpp            # Wrong: file should be singular
parsers/v2.hpp               # Wrong: unnecessary plural
rules-validator/v1.cpp       # Wrong: rules (plural) + validator (singular)
parser-syntax/v2.hpp         # Wrong: noun + adjective (wrong order)
rule-validators/v1.cpp       # Wrong: mixed singular-plural
test-runner/v1.cpp           # Questionable: prefer tester/v1.cpp
```

### **Directory Naming Standards**

**Rules:**
- Directory names MUST NOT contain version numbers
- Use lowercase with hyphens: `kebab-case` (NOT snake_case)
- Only allowed characters: `a-z`, `0-9`, `-`
- **Maximum 2 words in directory names (prefer single words)**
- **STRICT English grammar: singular/plural forms MUST be correct**
- **Singular for functional units, plural for collections**
- **NO grammatically incorrect English names allowed**
- Maximum 30 characters
- Represent function/feature only

**Grammar Rules for Directories:**
- **Functional directories**: Use singular (`engine/`, `parser/`, `validator/`)
- **Collection directories**: Use plural (`tests/`, `docs/`, `rules/`, `artifacts/`)
- **Domain directories**: Context-dependent (`core/`, `languages/`)
- **Compound names**: Grammatically correct (`rule-engine/`, `syntax-parser/`)

**Directory Structure Pattern:**
```
{domain}/{subdomain}/{function}/
```

**Examples:**
```
✅ Good (Correct Grammar):
core/engine/validator/           # Singular functional units
core/parser/lexer/              # Singular functional units  
languages/cpp/adapter/          # Singular functional unit
tests/unit/runner/              # Plural collection + singular runner
docs/architecture/guide/        # Plural collection + singular guide
rules/automation/loader/        # Plural collection + singular loader

✅ Compound Names (Grammatically Correct):
core/engine/rule-validator/     # rule (singular) + validator (singular)
core/parser/syntax-analyzer/    # syntax (adjective) + analyzer (singular)
languages/cpp/language-adapter/ # language (singular) + adapter (singular)

❌ Bad (Incorrect Grammar):
core/engines/validator/         # Wrong: engines should be singular
core/parsers/lexer/            # Wrong: parsers should be singular
test/unit/runner/              # Wrong: should be tests/ (plural collection)
doc/architecture/guide/        # Wrong: should be docs/ (plural collection)
rule/automation/loader/        # Wrong: should be rules/ (plural collection)
core/engine/rules-validator/   # Wrong: rules (plural) + validator (singular)
core/parser/analyzer-syntax/   # Wrong: noun + adjective (wrong order)
languages/cpp/adapters/        # Wrong: unnecessary plural for functional unit
```

**Directory Contents:**
Each function directory contains versioned files:
```
validator/                   # Single word preferred
├── _.yaml       # Special file, underscore allowed
├── v1.cpp
├── v1.hpp
├── v2.cpp
├── v2.hpp
└── README.md           # Special file, uppercase allowed
```

### **Special Files**

**Required in every directory:**
- `_.yaml` - Directory identification
- `README.md` - Directory purpose and contents (if contains multiple files)

**Optional but recommended:**
- `CHANGELOG.md` - Change history
- `TODO.md` - Pending tasks

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

### 1.0. Test and Example File Organization (MANDATORY)

**Development Test Files:**
- **ALL** test files created during development MUST go in `artifacts/` directory
- **NO** development test files allowed in `examples/` directory
- `examples/` is reserved ONLY for exhibition-quality demonstration files

**AI Agent Development Guidelines:**
- **Artifacts should be NOTES/THOUGHTS only**: No full code implementations
- **Artifacts maximum 120 lines per file**: Keep artifacts concise and focused
- **Direct codebase development**: AI agents should code directly into the main codebase
- **Branch-based development**: Each feature uses a separate branch for isolated development
- **Commit after each step**: Immediate commit after completing each development step
- **No large artifacts**: Avoid creating extensive code artifacts that duplicate main codebase

**File Placement Rules:**
```bash
✅ Correct Development Artifact Placement:
artifacts/godel-development/thought-process.md              # Development notes (< 120 lines)
artifacts/parser-fixes/approach-analysis.md                # Analysis notes (< 120 lines)
artifacts/feature-branch/step-checklist.md                 # Planning notes (< 120 lines)
artifacts/debugging/issue-investigation.md                  # Debug notes (< 120 lines)

✅ Correct Exhibition Placement:
examples/godel-exhibition.a                                # Final exhibition piece

❌ Wrong Placement:
examples/test-undecidability.a                            # NO tests in examples/
examples/debug-parser.a                                   # NO debug files in examples/
artifacts/feature-branch/full-implementation.cpp           # NO full code in artifacts/
artifacts/parser-fixes/complete-solution.hpp               # NO complete implementations in artifacts/
```

**Directory Purposes:**
- **`artifacts/`**: Development thinking notes, analysis, planning documents (notes only, max 120 lines)
- **`examples/`**: Polished, exhibition-quality demonstrations for public display
- **Main codebase**: Direct implementation of all actual code

**AI Agent Development Workflow:**
1. **Create branch**: `git checkout -b feature-name`
2. **Plan in artifacts**: Write brief planning notes in `artifacts/feature-name/`
3. **Code directly**: Implement directly into main codebase directories
4. **Commit immediately**: `git commit` after each completed step
5. **Keep artifacts minimal**: Only thinking notes, not code implementations

**Enforcement:**
- Development test files in `examples/` = validation error
- Code implementations in `artifacts/` = validation error
- Artifacts exceeding 120 lines = validation error
- Multiple similar examples = consolidation required
- `examples/` must contain ONLY final, polished demonstration files

### 1.1. Architecture Research (MANDATORY)
Before creating any plan.yaml, conduct comprehensive system analysis:

**A. Codebase Analysis:**
```bash
# Scan for existing similar components
find . -name "*.cpp" -o -name "*.hpp" | xargs grep -l "similar_functionality"
find . -name "*.yaml" | xargs grep "akao:" | sort | uniq
grep -r "@id:" --include="*.cpp" --include="*.hpp" . | cut -d'"' -f2 | sort
```

**B. Conflict Detection:**
- Check existing akao namespace IDs for conflicts
- Verify function/class names for duplications
- Analyze directory structure for naming consistency
- Review existing implementations for functionality overlap

**C. Integration Analysis:**
- Map dependencies with existing components
- Identify affected interfaces and APIs
- Assess philosophy-rule separation impact
- Validate universal parser compatibility

### 2. Required plan.yaml Structure
**ONLY create after completing architecture research**

```yaml
id: "akao:artifact:feature-description:plan:v1"
metadata:
  specification: "Feature description"
  scope: "Affected components"
  timeline: "Timeline"
  rationale: "Justification"
  methodology: "Implementation approach"
  references: ["URLs"]

# MANDATORY: Architecture research results
architecture_research:
  existing_components:
    similar_functions: ["akao:function:...", "akao:function:..."]
    related_classes: ["akao:class:...", "akao:class:..."]
    namespace_usage: ["akao:directory:...", "akao:file:..."]
  
  conflict_analysis:
    name_conflicts: []  # Must be empty or resolved
    functionality_overlaps: []  # Must be addressed
    integration_impacts: ["component1", "component2"]
  
  implementation_strategy:
    new_components: ["component_list"]
    extensions: ["existing_component_modifications"]
    refactoring_required: ["components_needing_changes"]
    dependencies: ["required_existing_components"]

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
 * @doc: Comprehensive documentation for automatic generation
 * @specification: Purpose and functionality
 * @scope: Location within system architecture
 * @timeline: Creation/modification date
 * @rationale: Justification for existence
 * @methodology: Implementation approach
 * @references: [dependencies, references]
 */
```

### 3.1. Function/Class Documentation (Mandatory)
All functions and classes MUST include complete metadata with @doc:
```cpp
/**
 * @id: akao:[function|class]:[category]:[subcategory]:[name]:v1
 * @doc: Detailed documentation for automatic generation (user-facing description)
 * @specification: Clear description of purpose and behavior
 * @scope: Component location and architectural role  
 * @timeline: Creation/modification date (YYYY-MM-DD)
 * @rationale: Business justification and necessity
 * @methodology: Implementation approach and algorithms
 * @references: [List of related akao IDs and external references]
 */
```

### 3.2. Directory Identification (Mandatory)
All directories MUST contain `_.yaml` with full metadata:
```yaml
id: "akao:directory:[category]:[subcategory]:[name]:v1"
metadata:
  specification: "Clear description of directory purpose"
  scope: "Architectural role and contents"
  timeline: "YYYY-MM-DD"
  rationale: "Why this directory exists"
  methodology: "Organization and structure approach"
  references: ["akao:file:...", "akao:class:..."]
```

### 4. Pre-Commit Validation
```bash
# Architecture compliance check
akao research --validate-conflicts
akao research --check-duplications
akao research --verify-integration

# Standard validation
akao validate --comprehensive
akao test --deterministic
akao analyze --compliance
```

## Requirements

- **MANDATORY architecture research before any plan.yaml creation**
- **Conflict detection and resolution required**
- **No duplicate functionality allowed**
- **Integration impact analysis mandatory**
- All IDs must follow akao namespace pattern
- **All components with IDs MUST include complete metadata headers**
- **@doc field MANDATORY for all code components (functions, classes, files)**
- **Metadata fields (specification, scope, timeline, rationale, methodology, references) are MANDATORY**
- All files must include metadata headers
- All functions and classes must include @id documentation with full metadata and @doc
- All directories must contain `_.yaml` identification file with complete metadata
- **File names MUST follow pattern: {function-folder}/{version}.{ext}**
- **File names: only a-z, 0-9, -, . allowed (NO underscores)**
- **Directory names MUST NOT contain version numbers**
- **File names MUST NOT contain function names (version-only)**
- **Maximum 2 words in folder/file names (prefer single words)**
- **STRICT English grammar compliance MANDATORY**
- **Singular for functional units, plural for collections**
- **NO grammatically incorrect English names allowed**
- **Maximum file version name: 20 characters**
- **Maximum directory name length: 30 characters**
- Phase/step directories: `artifacts/{branch}/{phase}/{step}/{unit_tests,inputs,outputs,assets}/`
- Deterministic tests with reproducible inputs/outputs
- Philosophy-rule separation (no mixing)
- Universal parser compatibility (standard language constructs only)

**Remember**: Every contribution becomes archaeological data for future intelligent agents.

## Mandatory Metadata Requirements

### **Universal Metadata Fields**
Every component with an akao ID MUST include these fields:

1. **@id**: Full akao namespace identifier
2. **@doc**: User-facing documentation for automatic generation (MANDATORY for code components)
3. **@specification**: Clear, concise description of purpose and functionality
4. **@scope**: Architectural location and role within the system
5. **@timeline**: Creation/modification date (YYYY-MM-DD format)
6. **@rationale**: Business/technical justification for existence
7. **@methodology**: Implementation approach, algorithms, or design patterns
8. **@references**: Array of related akao IDs and external references

### **Documentation Field Requirements**
- **@doc**: 20-200 words, user-friendly description for automatic documentation generation
- **@specification**: 10-100 words, technical specification for developers
- **@doc vs @specification**: @doc for end-users, @specification for developers

### **Code Component Documentation (MANDATORY)**
All functions, classes, and files MUST include @doc field:
```cpp
/**
 * @id: akao:function:core:engine:validate_rule:v1
 * @doc: Validates rule syntax and semantic correctness against engine standards. 
         Returns true if rule passes all validation checks, false otherwise.
         Used by the rule engine before executing any rule logic.
 * @specification: Validates rule syntax and semantic correctness
 * // ...other metadata fields...
 */
```

### **Metadata Quality Standards**
- **@doc**: 20-200 words, user-friendly description for automatic documentation
- **specification**: 10-100 words, technical specification for developers
- **scope**: Precise architectural location (e.g., "Core engine validation subsystem")
- **timeline**: ISO date format (YYYY-MM-DD)
- **rationale**: Clear business/technical necessity (why it exists)
- **methodology**: Implementation details (how it works)
- **references**: Valid akao IDs or external URLs

### **Validation Rules**
- Missing any metadata field = compilation/validation error
- Missing @doc field for code components = documentation generation error
- Invalid akao ID references = dependency validation error
- Malformed timeline = format validation error
- Empty or generic descriptions = quality validation error
- @doc field too short (<20 words) or too long (>200 words) = format validation error

## English Grammar Compliance

### **Mandatory Grammar Rules**

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

**Grammar Validation:**
- All names must pass English grammar checker
- Dictionary verification required for compound words
- Consistency across similar components mandatory

## Folder Structure and File Organization

### **Strict Folder/File Segregation Rules**

**MANDATORY: No mixing of files and folders at the same level**

**Prohibited Mixing:**
- ❌ Files and folders CANNOT coexist at the same directory level
- ❌ Folders and files CANNOT be siblings in the same directory
- ❌ Exception: System/repo configuration files in root folder only

**Parent-Child Hierarchy Rules:**
- ❌ Children/components/parts folders CANNOT exist at the same level as their parent folders
- ❌ Parent and child components MUST maintain strict hierarchical relationship
- ❌ NO sibling relationships between logically related parent-child components

**Allowed Root-Level Configuration Files:**
```
✅ Permitted in root folder only:
README.md
LICENSE
.gitignore
.gitattributes
Makefile
Makefile
package.json
CONTRIBUTING.md
CHANGELOG.md
```

**Special System Manifest File:**
- **ONLY** `_.yaml` is the system's special manifest file
- `_.yaml` can exist independently in any directory
- `_.yaml` serves as directory identification and metadata

### **Folder Content Rules**

**Every folder MUST contain EXACTLY ONE of these two patterns:**

#### **Pattern 1: Container Folder (Subdirectories Only)**
```
folder/
├── _.yaml              # ONLY allowed file
├── subfolder1/
├── subfolder2/
└── subfolder3/
```

**Rules:**
- Contains ONLY subdirectories
- May contain ONLY `_.yaml` file (optional)
- NO other files allowed
- NO version files (v1.cpp, v1.hpp, etc.)

#### **Pattern 2: Implementation Folder (Version Files Only)**
```
folder/
├── v1.cpp              # Implementation file
├── v1.hpp              # Header file
├── v2.cpp              # Optional newer version
└── v2.hpp              # Optional newer version
```

**Rules:**
- Contains ONLY versioned implementation files
- NO `_.yaml` file allowed
- NO subdirectories allowed
- ONLY `v{number}.{ext}` files permitted

### **Examples of Correct Structure:**

```
✅ Correct Structure:
core/                           # Container folder
├── _.yaml                      # System manifest
├── engine/                     # Container folder
│   ├── _.yaml                  # Directory identification
│   ├── validator/              # Implementation folder
│   │   ├── v1.cpp             # Implementation
│   │   └── v1.hpp             # Header
│   └── parser/                # Container folder
│       ├── _.yaml             # Directory identification
│       ├── lexer/             # Implementation folder
│       │   ├── v1.cpp         # Implementation
│       │   └── v1.hpp         # Header
│       └── syntax/            # Implementation folder
│           ├── v1.cpp         # Implementation
│           └── v1.hpp         # Header
└── filesystem/                # Container folder
    ├── _.yaml                 # Directory identification
    └── scanner/               # Implementation folder
        ├── v1.cpp             # Implementation
        └── v1.hpp             # Header
```

### **Examples of Incorrect Structure:**

```
❌ Incorrect Structure:
core/                          # WRONG: mixing files and folders
├── _.yaml                     # File
├── validator.cpp              # WRONG: File at same level as folders
├── engine/                    # Folder
└── parser/                    # Folder

engine/                        # WRONG: mixing implementation and containers
├── _.yaml                     # WRONG: _.yaml in implementation folder
├── v1.cpp                     # Implementation file
├── v1.hpp                     # Header file
└── validator/                 # WRONG: Subfolder in implementation folder

❌ Parent-Child Hierarchy Violations:
core/
├── _.yaml
├── engine/                    # Parent component
├── engine-validator/          # WRONG: Child component at same level as parent
├── engine-parser/             # WRONG: Child component at same level as parent
└── engine-config/             # WRONG: Child component at same level as parent

parser/
├── _.yaml
├── lexer/                     # WRONG: Should be inside parser/, not alongside
├── syntax/                    # WRONG: Should be inside parser/, not alongside
└── grammar/                   # WRONG: Should be inside parser/, not alongside
```

**Correct Parent-Child Structure:**
```
✅ Proper Hierarchy:
core/
├── _.yaml
└── engine/                    # Parent component
    ├── _.yaml
    ├── validator/             # Child component inside parent
    ├── parser/                # Child component inside parent
    └── config/                # Child component inside parent

parser/                        # Parent component
├── _.yaml
├── lexer/                     # Child component inside parent
├── syntax/                    # Child component inside parent
└── grammar/                   # Child component inside parent
```

## .akao/rules/ Directory Structure

### **Rule Organization Requirements**

**MANDATORY Structure:**
```
.akao/
└── rules/
    ├── enabled/               # Active rules
    │   ├── structure/
    │   │   ├── one-class-per-file.a
    │   │   └── file-organization.a
    │   ├── interface/
    │   │   └── command-parity.a
    │   └── language/
    │       └── cpp-standards.a
    └── disabled/              # Inactive rules
        ├── experimental/
        │   └── new-feature.a
        └── deprecated/
            └── old-rule.a
```

**Rule Activation/Deactivation:**
- **Enable rule**: Move from `disabled/` to `enabled/`
- **Disable rule**: Move from `enabled/` to `disabled/`
- **NEVER modify rule content to enable/disable**
- **NEVER use configuration flags in .yaml/.json/.a files**

**Rule File Requirements:**
- All rule files use `.a` extension (Akao Pure Logic format)
- Rule files contain executable logic expressions
- Rule metadata stored as comments within `.a` files
- Rule organization by category (structure/, interface/, language/, etc.)

### **Akao Pure Logic Format (.a)**

**Definition:**
- `.a` files contain Akao Pure Logic expressions
- Akao Pure Logic is the domain-specific language for rule definition
- Provides concise, mathematical notation for logical expressions
- Alternative to verbose YAML syntax for rule logic

**File Structure:**
```
# Rule metadata (comments)
# id: rule_namespace:rule_name
# name: Human-readable name
# description: Rule description
# category: rule_category
# severity: error|warning|info

# Rule logic (Akao Pure Logic expression)
forall $variable in domain_expression: condition_expression
```

**Management Operations:**
```bash
# Enable a rule
mv .akao/rules/disabled/category/rule.a .akao/rules/enabled/category/rule.a

# Disable a rule  
mv .akao/rules/enabled/category/rule.a .akao/rules/disabled/category/rule.a

# NEVER modify rule content for activation:
❌ WRONG: Edit rule.a to add "enabled: true"
❌ WRONG: Use configuration files to control rule status
✅ CORRECT: Use file system operations (mv) to control rule status
```

**Validation Requirements:**
- All `.a` files must parse correctly as Akao Pure Logic
- Rule activation status determined by file location only
- Rule logic must be deterministic and reproducible
- Rule metadata must follow comment conventions
- Rule categories must align with system architecture

## Validation and Enforcement

### **Structure Validation Rules**

**Folder Structure Compliance:**
- **Mixed content detection** = validation error
- **Missing `_.yaml` in container folders** = validation error (except root)
- **Present `_.yaml` in implementation folders** = validation error
- **Files alongside folders** (except root config files) = validation error
- **Empty folders** without `_.yaml` or version files = validation error
- **Parent-child components at same level** = validation error
- **Child components outside parent hierarchy** = validation error

**Rule System Compliance:**
- **Rules outside enabled/disabled structure** = validation error
- **Configuration-based enable/disable** = validation error
- **Invalid .a format syntax** = parsing error
- **Missing rule metadata** = validation error

### **Enforcement Mechanisms**

**Pre-commit Validation:**
```bash
# Structure validation
akao validate --structure --strict
akao validate --rules --syntax-check
akao validate --naming --grammar-check

# Rule system validation
akao rules --validate-structure
akao rules --check-activation-method
```

**Build System Integration:**
- Make validates folder structure during build
- Make targets check rule organization compliance
- Build fails on structure violations
- Automated `.a` format syntax validation

**CI/CD Pipeline Checks:**
- GitHub Actions validate entire repository structure
- Automated grammar checking for all names
- Rule activation method verification
- Comprehensive compliance reporting
