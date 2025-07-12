# AKAO Codebase Comprehensive Audit & Refactoring Analysis

You are a senior software architect tasked with conducting a comprehensive audit and refactoring plan for a heavily corrupted codebase that has undergone multiple refactoring attempts, resulting in significant technical debt and architectural violations.

## MISSION OBJECTIVES

1. **COMPLETE ARCHAEOLOGICAL ANALYSIS** of the entire codebase
2. **IDENTIFY ALL VIOLATIONS** of the akao Contributing Standards
3. **DETECT LEGACY CONTAMINATION** from previous refactoring attempts
4. **PROPOSE SYSTEMATIC REFACTORING STRATEGY** with priority-based cleanup plan
5. **ENSURE ZERO REGRESSION** during the refactoring process

## ANALYSIS REQUIREMENTS

### 1. STRUCTURAL VIOLATIONS DETECTION

**A. Naming Convention Violations:**
- Scan ALL files and folders for naming standard violations
- Identify files with function names in filenames (should be version-only)
- Find directories with version numbers (forbidden)
- Detect underscore usage in file/folder names (forbidden)
- Flag non-kebab-case naming
- Check English grammar violations (plural/singular misuse)
- Find names exceeding length limits (20 chars for files, 30 for dirs)
- Identify compound words with incorrect grammar (e.g., "rules-validator" vs "rule-validator")

**B. Akao Namespace Violations:**
- Find all components missing akao namespace IDs
- Detect duplicate akao IDs across the codebase
- Identify malformed akao namespace patterns
- Check for missing @doc fields in functions/classes
- Validate incomplete metadata headers
- Find broken references in akao ID dependencies

**C. Directory Structure Issues:**
- Locate empty directories
- Find directories missing required `_.yaml` files
- Detect orphaned directories not referenced anywhere
- Identify circular directory dependencies
- Check for improper nesting depth
- Find directories with mixed purposes (violation of single responsibility)

### 2. CODE QUALITY CONTAMINATION

**A. Code Duplication Analysis:**
- Identify identical or near-identical functions across files
- Find duplicated class implementations
- Detect copy-pasted code blocks with minor variations
- Locate redundant utility functions
- Find multiple files implementing the same functionality
- Identify overlapping business logic

**B. Legacy Code Contamination:**
- Find remnants from previous refactoring phases (e.g., "phase6", "old_", "temp_", "backup_")
- Detect commented-out code blocks from previous implementations
- Identify unused imports and dependencies from old versions
- Locate hardcoded values that should be configurable
- Find deprecated patterns still in use
- Detect mixed architectural styles from different refactoring periods

**C. Orphaned Components:**
- Identify files not referenced by any other component
- Find functions never called anywhere in the codebase
- Detect unused classes and interfaces
- Locate isolated code modules with no integration points
- Find test files testing non-existent functionality
- Identify configuration files for removed features

### 3. ARCHITECTURE COMPLIANCE VIOLATIONS

**A. Philosophy-Rule Separation Violations:**
- Find business logic mixed with configuration rules
- Detect rule definitions embedded in philosophical components
- Identify abstract concepts implemented as concrete rules
- Locate rule execution logic in philosophy modules

**B. Universal Parser Compatibility Issues:**
- Find language-specific constructs that break universal parsing
- Detect non-standard syntax extensions
- Identify platform-specific code in universal components
- Locate hardcoded language assumptions

**C. Component Boundary Violations:**
- Find tight coupling between components that should be independent
- Detect circular dependencies between modules
- Identify components accessing internals of other components directly
- Locate shared mutable state between components

### 4. DEPENDENCY GRAPH CORRUPTION

**A. Dependency Analysis:**
- Map all inter-component dependencies
- Identify circular dependencies
- Find unused dependencies
- Detect missing dependencies (broken imports)
- Locate version conflicts between components
- Find dependencies on deprecated or removed components

**B. Integration Point Analysis:**
- Identify broken API contracts between components
- Find inconsistent data formats between integrated components
- Detect missing error handling at integration boundaries
- Locate hardcoded integration assumptions

### 5. TESTING & VALIDATION CONTAMINATION

**A. Test Quality Issues:**
- Find tests with non-deterministic inputs/outputs
- Identify missing test coverage for critical components
- Detect test files testing removed functionality
- Locate flaky tests with inconsistent results
- Find hardcoded test data that should be configurable

**B. Validation Gaps:**
- Identify components missing validation logic
- Find incomplete input sanitization
- Detect missing error handling paths
- Locate components violating defensive programming principles

### 6. LEGACY CONTAMINATION PATTERNS

**A. Stupid Legacy Artifacts:**
- Find ridiculous phase naming (e.g., "phase6", "phase_final_v2", "phase_really_final")
- Detect absurd temporary solutions that became permanent
- Identify braindead workarounds that should have been proper fixes
- Locate moronic magic numbers and hardcoded paths
- Find idiotic copy-paste programming patterns
- Detect senseless over-engineering from previous "architects"

**B. Garbage File Patterns:**
- Find files with nonsensical names (e.g., "temp123.cpp", "asdf.hpp", "test_dont_delete.cpp")
- Detect backup files scattered throughout (e.g., "file.cpp.bak", "old_version.cpp")
- Identify experimental code left rotting in production directories
- Locate half-finished features abandoned mid-development
- Find debugging code that was never cleaned up
- Detect personal scratch files developers forgot to remove

**C. Architectural Abominations:**
- Find God objects that do everything
- Detect spaghetti code masquerading as "flexible architecture"
- Identify cargo cult programming patterns copied without understanding
- Locate anti-patterns implemented as "best practices"
- Find premature optimizations that made code unmaintainable
- Detect "enterprise" patterns that add complexity without value

**D. Documentation Disasters:**
- Find outdated comments describing non-existent code
- Detect misleading documentation that contradicts implementation
- Identify TODO comments from 5 years ago never addressed
- Locate copy-pasted documentation with wrong information
- Find empty documentation files created "for compliance"
- Detect documentation written in multiple inconsistent styles

### 7. PERFORMANCE & RESOURCE WASTE

**A. Performance Antipatterns:**
- Find unnecessary memory allocations in tight loops
- Detect redundant computations that could be cached
- Identify inefficient data structures for the use case
- Locate blocking operations on main threads
- Find resource leaks (memory, file handles, connections)
- Detect premature pessimizations disguised as "safety"

**B. Resource Waste Detection:**
- Identify oversized buffers and data structures
- Find unused large dependencies bloating the build
- Detect redundant data copies and transformations
- Locate inefficient algorithms with better alternatives
- Find wasteful logging and debugging overhead in production
- Detect unnecessary abstraction layers adding overhead

## ANALYSIS METHODOLOGY

### PHASE 1: AUTOMATED SCANNING
```bash
# File and folder structure analysis
find . -type f -name "*_*" | grep -E "\.(cpp|hpp|yaml|md)$"  # Underscore violations
find . -type d -name "*[0-9]*" | grep -v "/v[0-9]"  # Version in directory names
find . -type d -empty  # Empty directories
find . -name "*.yaml" -exec grep -L "^id:" {} \;  # Missing akao IDs

# Legacy garbage detection
find . -name "*phase*" -o -name "*old*" -o -name "*temp*" -o -name "*backup*" -o -name "*test*" -o -name "*asdf*" -o -name "*tmp*"
find . -name "*.bak" -o -name "*.orig" -o -name "*~" -o -name "*.swp"
find . -name "*dont_delete*" -o -name "*keep*" -o -name "*save*"

# Code duplication detection
find . -name "*.cpp" -exec md5sum {} \; | sort | uniq -d  # Identical files
find . -name "*.hpp" -exec md5sum {} \; | sort | uniq -d  # Identical headers

# Technical debt markers
grep -r "TODO\|FIXME\|HACK\|XXX\|KLUDGE\|WORKAROUND" --include="*.cpp" --include="*.hpp" .
grep -r "magic.*number\|hardcoded\|temporary\|quick.*fix" --include="*.cpp" --include="*.hpp" .

# Orphaned file detection
find . -name "*.cpp" -o -name "*.hpp" | while read file; do
  basename=$(basename "$file" | sed 's/\.[^.]*$//')
  if ! grep -r "$basename" --exclude="$file" . >/dev/null 2>&1; then
    echo "Orphaned: $file"
  fi
done

# God object detection (files > 1000 lines)
find . -name "*.cpp" -o -name "*.hpp" | xargs wc -l | awk '$1 > 1000 {print}'

# Dead code detection
ctags -R --c++-kinds=+p --fields=+iaS --extra=+q .
awk '/^[a-zA-Z]/ {print $1}' tags | sort | uniq > all_symbols.txt
grep -r "class\|struct\|enum" --include="*.hpp" . | cut -d: -f2 | grep -oE '\b[A-Z][a-zA-Z0-9_]*\b' | sort | uniq > defined_types.txt
```

### PHASE 2: DEPENDENCY GRAPH ANALYSIS
```bash
# Extract all akao IDs and their references
grep -r "@id:" --include="*.cpp" --include="*.hpp" . | cut -d'"' -f2 > all_ids.txt
grep -r "akao:" --include="*.cpp" --include="*.hpp" . | grep -v "@id:" > all_references.txt

# Find broken references
while read id; do
  if ! grep -q "$id" all_references.txt; then
    echo "Unused ID: $id"
  fi
done < all_ids.txt

# Find circular dependencies
python dependency_analyzer.py --detect-cycles --input=. --output=dependency_graph.json

# Analyze include complexity
grep -r "#include" --include="*.cpp" --include="*.hpp" . | cut -d: -f1 | sort | uniq -c | sort -nr | head -20
```

### PHASE 3: SEMANTIC ANALYSIS
```bash
# Function and class duplication analysis
ctags -R --c++-kinds=+p --fields=+iaS --extra=+q .
awk '/^[a-zA-Z]/ {print $1}' tags | sort | uniq -d  # Duplicate symbols

# Hardcode detection
grep -r "localhost\|127\.0\.0\.1\|hardcoded\|magic.*number" --include="*.cpp" --include="*.hpp" .
grep -rE "\"[^\"]{20,}\"" --include="*.cpp" --include="*.hpp" .  # Long string literals
grep -rE "\b[0-9]{4,}\b" --include="*.cpp" --include="*.hpp" .  # Magic numbers

# Anti-pattern detection
grep -r "Singleton\|GlobalState\|Manager.*Manager" --include="*.cpp" --include="*.hpp" .
grep -r "Factory.*Factory\|Abstract.*Abstract" --include="*.cpp" --include="*.hpp" .

# Performance issue detection
grep -r "new.*loop\|malloc.*loop\|vector.*push_back.*loop" --include="*.cpp" .
grep -r "std::endl\|printf.*loop\|cout.*loop" --include="*.cpp" .
```

### PHASE 4: CONTENT ANALYSIS
```bash
# Documentation quality
find . -name "*.md" -exec wc -w {} \; | awk '$1 < 10 {print $2}' # Tiny docs
find . -name "README*" -empty  # Empty READMEs
grep -r "TODO:\|FIXME:\|XXX:" --include="*.md" .  # Unfinished docs

# Test quality analysis
find . -path "*/test*" -name "*.cpp" | xargs grep -L "assert\|EXPECT\|CHECK" # Tests without assertions
find . -path "*/test*" -name "*.cpp" | xargs grep -l "sleep\|wait\|delay" # Non-deterministic tests

# Configuration mess
find . -name "*.json" -o -name "*.yaml" -o -name "*.ini" -o -name "*.cfg" | xargs grep -l "localhost\|127.0.0.1\|password\|secret"
```

## REFACTORING STRATEGY REQUIREMENTS

### PRIORITY CLASSIFICATION
Classify all findings into:
1. **CRITICAL** - Blocks system functionality or violates core principles
2. **HIGH** - Significant technical debt or maintenance burden  
3. **MEDIUM** - Quality improvements with moderate impact
4. **LOW** - Minor improvements or cosmetic changes

### REFACTORING PLAN STRUCTURE
For each violation category, provide:

```yaml
violation_category: "Specific violation type"
priority: "CRITICAL|HIGH|MEDIUM|LOW"
impact_analysis:
  affected_components: ["list of components"]
  risk_level: "HIGH|MEDIUM|LOW"
  effort_estimate: "hours/days"
  dependencies: ["blocking items"]
  blast_radius: "scope of potential damage"

refactoring_strategy:
  approach: "Detailed refactoring approach"
  steps:
    - step: "Specific action"
      validation: "How to verify success"
      rollback_plan: "If something goes wrong"
      testing_strategy: "How to test this change"
  
  new_structure:
    files_to_create: ["new file paths with akao compliance"]
    files_to_modify: ["existing file paths"]
    files_to_delete: ["obsolete file paths"]
    directories_to_restructure: ["directory changes"]
    renames_required: ["old_path -> new_path"]

validation_plan:
  tests_to_update: ["test files needing changes"]
  new_tests_required: ["new test coverage needed"]
  regression_tests: ["tests to ensure no breaking changes"]
  performance_benchmarks: ["performance tests to run"]
  integration_tests: ["integration points to verify"]

risk_mitigation:
  backup_strategy: "How to backup current state"
  rollback_triggers: ["conditions that require rollback"]
  monitoring_required: ["what to monitor during/after change"]
  communication_plan: "How to coordinate with team"
```

### CLEANUP CATEGORIES

**A. Immediate Deletion (Zero Risk):**
- Empty directories
- Backup files (*.bak, *.orig, *~)
- Temporary files clearly marked as such
- Orphaned test files for removed features
- Documentation for non-existent components

**B. Safe Refactoring (Low Risk):**
- Renaming files/directories to akao compliance
- Adding missing metadata headers
- Consolidating duplicate utility functions
- Removing unused imports
- Standardizing code formatting

**C. Structural Changes (Medium Risk):**
- Merging duplicate implementations
- Breaking up God objects
- Fixing circular dependencies
- Reorganizing directory structures
- Updating API contracts

**D. Architectural Overhaul (High Risk):**
- Separating philosophy from rules
- Redesigning component boundaries
- Replacing anti-patterns with proper designs
- Performance optimizations
- Major dependency updates

### MIGRATION STRATEGY

1. **Pre-Migration Phase:**
   - Complete backup of current state
   - Establish baseline performance metrics
   - Document all current functionality
   - Set up monitoring and alerting
   - Prepare rollback procedures

2. **Phase 1: Safe Cleanup (Week 1-2):**
   - Delete obvious garbage files
   - Fix naming violations
   - Add missing metadata
   - Remove dead code
   - Update documentation

3. **Phase 2: Code Quality (Week 3-4):**
   - Consolidate duplications
   - Fix circular dependencies
   - Improve test coverage
   - Standardize patterns
   - Performance profiling

4. **Phase 3: Structural Reform (Week 5-8):**
   - Reorganize directory structure
   - Implement akao namespace fully
   - Separate philosophy from rules
   - Fix component boundaries
   - Comprehensive testing

5. **Phase 4: Validation & Optimization (Week 9-10):**
   - Full system testing
   - Performance optimization
   - Documentation completion
   - Team training
   - Monitoring setup

## DELIVERABLES REQUIRED

1. **VIOLATION INVENTORY** - Complete catalog of every issue found
   - Location, severity, impact assessment
   - Root cause analysis
   - Remediation effort estimate
   - Dependencies and blockers

2. **DEPENDENCY GRAPH VISUALIZATION** - Before/after architecture diagrams
   - Current state with all violations highlighted
   - Target state with clean architecture
   - Migration path with intermediate states
   - Risk zones and critical paths

3. **PRIORITIZED CLEANUP ROADMAP** - Step-by-step execution plan
   - Daily tasks for 10-week cleanup
   - Resource requirements and assignments
   - Milestone checkpoints and success criteria
   - Risk mitigation at each phase

4. **AUTOMATED TOOLING SUITE** - Prevention and detection tools
   - Pre-commit hooks for akao compliance
   - Continuous monitoring scripts
   - Automated refactoring tools
   - Quality gates and validation

5. **TESTING STRATEGY** - Comprehensive validation approach
   - Regression test suite expansion
   - Performance benchmark establishment
   - Integration test coverage
   - Chaos engineering for resilience

6. **TEAM ENABLEMENT PACKAGE** - Knowledge transfer materials
   - Training materials for new standards
   - Quick reference guides
   - Common pitfall documentation
   - Best practice examples

## SPECIAL CONSIDERATIONS

1. **Archaeological Preservation** - Document WHY legacy exists before destroying it
2. **Business Continuity** - Ensure zero downtime during migration
3. **Team Velocity** - Maintain development productivity throughout cleanup
4. **Knowledge Transfer** - Capture tribal knowledge before removing old code
5. **Automation First** - Build tools to prevent regression
6. **Incremental Value** - Deliver improvements progressively

## SUCCESS CRITERIA

**Quantitative Metrics:**
- 100% akao namespace compliance
- Zero orphaned files or directories
- Zero code duplication (function level)
- <5% cyclomatic complexity variance
- 90%+ test coverage on refactored components
- <10ms performance regression tolerance

**Qualitative Indicators:**
- Clean dependency graph visualization
- Consistent code patterns throughout
- Self-documenting code structure
- Intuitive directory organization
- Maintainable and extensible architecture

**Process Improvements:**
- Automated quality gates preventing regression
- Developer onboarding time reduced by 50%
- Bug discovery time reduced by 75%
- Feature development velocity increased by 25%
- Code review time reduced by 40%

## RISK ASSESSMENT

**High-Risk Areas:**
- Components with external integrations
- Performance-critical code paths
- Legacy code with unclear business logic
- Shared utilities used across many modules
- Database interaction layers

**Mitigation Strategies:**
- Comprehensive backup before any changes
- Feature flags for gradual rollout
- Canary deployments for validation
- Real-time monitoring during changes
- Immediate rollback capabilities

**Success Validation:**
- All existing functionality preserved
- Performance benchmarks maintained or improved
- Team productivity metrics stable or improved
- Zero customer-impacting incidents
- Successful completion within timeline

Execute this analysis with extreme thoroughness - assume the codebase is in the worst possible state and leave no stone unturned. The goal is complete architectural rehabilitation that will serve as the foundation for all future development.

Remember: This is not just cleanup - it's archaeological reconstruction of a system that has lost its way. Every decision must be documented for future maintainers to understand the journey from chaos to order.