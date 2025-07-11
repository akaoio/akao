# Contributing to Akao

**⚠️ PROVISIONAL DOCUMENTATION**: This project is in active design phase using "vibe coding" methodology. All guidelines are work-in-progress and subject to change as the C++ workflow orchestrator architecture evolves.

## 1. Identification Standard

1. **Pattern**: `akao:[type]:[category]:[subcategory]:[name]:[version]`
2. **Component Types**: core, node, workflow, orchestrator, artifact, test, function, class, directory
3. **Examples**: `akao:node:filesystem:scanner:v1`, `akao:workflow:validation:project:v1`

## 2. Component Requirements

4. **Functions**: Include @id, @doc, @specification, @scope, @timeline, @rationale, @methodology, @references
5. **Classes**: Include complete metadata headers with @doc field
6. **Directories**: Contain `_.yaml` with full metadata

## 3. File Naming

7. **Pattern**: `{function_folder}/{version}.{ext}`
8. **Characters**: Only a-z, 0-9, -, . (NO underscores)
9. **Version-only**: File names MUST NOT contain function names
10. **Grammar**: Singular for functions, plural for collections
11. **Maximum**: 2 words preferred, 20 characters max
12. **Examples**: `validator/v1.cpp`, `tests/runner/v1.cpp`

## 4. Directory Naming

13. **Characters**: Only a-z, 0-9, - (NO underscores, NO versions)
14. **Grammar**: Singular for functional units, plural for collections
15. **Maximum**: 2 words preferred, 30 characters max
16. **Pattern**: `{domain}/{subdomain}/{function}/`
17. **Examples**: `core/engine/validator/`, `tests/unit/runner/`

## 5. Special Files

18. **Required**: `_.yaml` in container directories
19. **Optional**: `README.md`, `CHANGELOG.md`, `TODO.md`

## 6. Core Principles

20. **Archaeological**: Document all development actions
21. **Systematic**: Conform to identification standards
22. **Deterministic**: Include reproducible testing
23. **Architectural**: Maintain core-node separation

## 7. Workflow

24. **Branch**: `git checkout -b feature-description`
25. **Artifacts**: Create `artifacts/feature-description/{plan.yaml,checklist.md,quality.yaml}`

## 8. Development Rules

26. **Test Files**: ALL development tests in `artifacts/` directory
27. **Examples**: ONLY exhibition-quality files in `examples/`
28. **Artifacts**: Notes/thoughts only, max 120 lines per file
29. **Development**: Code directly into main codebase
30. **Commits**: Immediate commit after each step

## 9. Architecture Research

31. **Codebase**: Scan existing components for conflicts
32. **Conflicts**: Check namespace IDs, function names, directories
33. **Integration**: Map dependencies and interfaces
34. **Required**: Complete before creating plan.yaml

## 10. Plan Structure

35. **ID**: `akao:artifact:feature-description:plan:v1`
36. **Research**: Include existing_components, conflict_analysis, implementation_strategy
37. **Phases**: Define phases with steps, inputs, outputs, validation

## 11. Headers

38. **Files**: Include @id, @doc, @specification, @scope, @timeline, @rationale, @methodology, @references
39. **Functions**: Complete metadata with @doc field (20-200 words)
40. **Classes**: Full metadata documentation
41. **Directories**: `_.yaml` with complete metadata

## 12. Validation

42. **Pre-commit**: `akao validate`
43. **System**: `akao status`
44. **Testing**: `akao test`

## 13. Requirements

45. **Metadata**: All components with IDs MUST include complete metadata
46. **Documentation**: @doc field MANDATORY for code components
47. **Naming**: Follow patterns exactly, enforce grammar rules
48. **Structure**: Maintain folder/file segregation
49. **Testing**: Deterministic with reproducible inputs/outputs

## 14. Folder Structure

50. **Segregation**: NO mixing files and folders at same level
51. **Hierarchy**: NO parent-child components at same level
52. **Container**: Subdirectories only, optional `_.yaml`
53. **Implementation**: Version files only, NO `_.yaml`
54. **Root**: Only system config files allowed

## 15. Current Architecture

55. **Core**: Pure orchestrator in `core/` directory
56. **Nodes**: Self-contained in `.akao/nodes/` directory
57. **Node Structure**: Each node has `tests/`, `build/`, implementation files
58. **Tests**: Root `tests/` for platform tests, node `tests/` for self-contained tests
59. **Discovery**: Core scans `.akao/nodes/` for available processing nodes

## 16. Grammar Rules

60. **Singular**: validator, parser, engine (functional units)
61. **Plural**: tests, docs, rules (collections)
62. **Compound**: rule-validator (both singular), syntax-parser (adjective-noun)
63. **Forbidden**: rules-validator, parser-syntax, mixed forms

## 17. Structure Patterns

64. **Container**: `folder/` with `_.yaml` and subdirectories only
65. **Implementation**: `folder/` with `v1.cpp`, `v1.hpp` only
66. **NO mixing**: Files and folders cannot coexist (except root config)

## 18. Workflow System

67. **Location**: `.akao/workflows/` directory
68. **Format**: YAML-based workflow definitions
69. **Execution**: `akao run --workflow <name>`
70. **Management**: Enable/disable through workflow configuration

## 19. Node Architecture

71. **Self-contained**: Each node independent of akao platform
72. **Structure**: `{nodename}/tests/`, `{nodename}/build/`, implementation files
73. **Discovery**: Orchestrator scans `.akao/nodes/` directory
74. **Equality**: All nodes treated equally regardless of origin

## 20. Validation Rules

75. **Missing metadata**: Compilation/validation error
76. **Missing @doc**: Documentation generation error
77. **Invalid references**: Dependency validation error
78. **Grammar violations**: Format validation error
79. **Structure violations**: Build system error
80. **Workflow format**: Parsing error for invalid YAML syntax
81. **Mixed content**: Validation error for files/folders mixing
82. **Parent-child**: Validation error for same-level components
83. **Empty folders**: Validation error without proper content
84. **Workflow configuration**: Validation error for invalid workflow config

## 21. Enforcement

85. **Pre-commit**: Structure, workflow, naming, grammar validation
86. **Build**: Folder structure and workflow organization compliance
87. **CI/CD**: Repository structure and comprehensive compliance checks

## 22. Development Workflow

88. **Branch**: Create feature branch
89. **Plan**: Write brief planning notes in artifacts
90. **Code**: Implement directly into main codebase
91. **Test**: Create self-contained node tests
92. **Commit**: Immediate commit after each step
93. **Validate**: Run comprehensive validation before merge

## 23. Quality Standards

94. **@doc**: 20-200 words, user-friendly description
95. **specification**: 10-100 words, technical specification
96. **scope**: Precise architectural location
97. **timeline**: ISO date format (YYYY-MM-DD)
98. **rationale**: Clear business/technical necessity
99. **methodology**: Implementation details
100. **references**: Valid akao IDs or external URLs

## 24. File Organization

101. **Artifacts**: Development notes, analysis, planning (max 120 lines)
102. **Examples**: Polished demonstration files only
103. **Main**: Direct implementation of actual code
104. **Separation**: NO mixing development and exhibition files

## 25. Integration Rules

105. **Dependencies**: Map with existing components
106. **Interfaces**: Identify affected APIs
107. **Core**: Maintain separation from nodes
108. **Parser**: Ensure universal compatibility

## 26. Component Documentation

109. **Functions**: @id with full metadata and @doc
110. **Classes**: Complete metadata headers
111. **Files**: Include identification headers
112. **Directories**: `_.yaml` with complete metadata

## 27. Naming Standards

113. **File names**: Version-only, NO function names
114. **Directory names**: NO version numbers
115. **Grammar**: Strict English compliance
116. **Characters**: Limited character set, NO underscores
117. **Length**: Maximum limits enforced

## 28. Structure Examples

118. **Correct**: `core/engine/validator/v1.cpp`
119. **Incorrect**: `core/validators/v1.cpp`
120. **Container**: `core/` with `_.yaml` and subdirectories
121. **Implementation**: `validator/` with version files only

## 29. Workflow Management

122. **List**: `akao list --workflow`
123. **Run**: `akao run --workflow <name>`
124. **Install**: `akao install --workflow <source>`
125. **Format**: YAML-based workflow definitions

## 30. System Architecture

126. **Core**: Pure workflow orchestrator
127. **Nodes**: External processing units in `.akao/nodes/`
128. **Tests**: Platform tests in root, node tests in node directories
129. **Discovery**: Dynamic node loading and registration
130. **Equality**: All nodes treated uniformly

## 31. Metadata Requirements

131. **Universal**: All components with akao IDs need complete metadata
132. **Documentation**: @doc mandatory for automatic generation
133. **Quality**: Proper word counts and format validation
134. **References**: Valid akao IDs and external URLs

## 32. Validation Mechanisms

135. **Structure**: Folder organization and file placement
136. **Grammar**: English language compliance
137. **Workflows**: YAML syntax and configuration validation

**Total Clauses: 137** - Every contribution becomes archaeological data for future intelligent agents.