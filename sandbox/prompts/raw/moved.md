
## Project Restructure: Node Architecture Changes

### Completed Changes
- **Moved**: `nodes/builtin` → `.akao/nodes`
- **Action Required**: Update unit tests in `tests/` folder to reflect this change

### New Architecture Requirements

#### Core System
- **Core must be a pure orchestrator** - no builtin nodes allowed
- All nodes are treated equally regardless of origin
- All nodes must reside in `./akao/nodes`
- All workflows follow the same pattern

#### Node Structure Requirements
Each C++ node must be **self-contained** with this structure:
```
.akao/nodes/nodename/
├── tests/
│   ├── v1.cpp
│   └── v1.hpp
├── build/
│   ├── {nodename}/          # executable
│   └── tests/               # unit test builds
└── [node implementation files]
```

#### Testing Strategy
- **Root `tests/` folder**: Platform unit tests only (not node-specific tests)
- **Node `tests/` folders**: Self-contained unit tests for each node
- Each node must function independently of the akao platform

#### Migration Tasks
1. Fix existing unit tests in root `tests/` folder
2. Move node-specific tests from root to respective node directories
3. Ensure each moved node has proper self-contained test structure

#### Outstanding Question
**How will akao discover and load required nodes for operation?**

This restructure creates a cleaner separation between the orchestration platform and the individual processing nodes.