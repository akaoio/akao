# 🔍 DEPENDENCY AUDIT RESULTS - PHASE 1.1

**Status**: CRITICAL COUPLING DETECTED ❌

## Current Dependency Analysis

### Node → Core Dependencies (8 critical violations)

#### 1. Interface Dependencies (4 nodes)
```bash
.akao/nodes/file/v1.hpp:#include "core/foundation/interfaces/inode/v1.hpp"
.akao/nodes/logic/v1.hpp:#include "core/foundation/interfaces/inode/v1.hpp"
.akao/nodes/reporter/v1.hpp:#include "core/foundation/interfaces/inode/v1.hpp"
.akao/nodes/yaml/v1.hpp:#include "core/foundation/interfaces/inode/v1.hpp"
```

#### 2. Registry Dependencies (4 nodes)
```bash
.akao/nodes/file/v1.cpp:#include "core/engine/orchestrator/registry/registrar.hpp"
.akao/nodes/logic/v1.cpp:#include "core/engine/orchestrator/registry/registrar.hpp"
.akao/nodes/reporter/v1.cpp:#include "core/engine/orchestrator/registry/registrar.hpp"
.akao/nodes/yaml/v1.cpp:#include "core/engine/orchestrator/registry/registrar.hpp"
```

### Core → Node Dependencies (1 violation)

#### 1. Direct Node Include
```bash
core/engine/orchestrator/workflow/v1.cpp:#include ".akao/nodes/yaml/v1.hpp"
```

## Critical Coupling Points

### 1. **INode Interface Dependency**
**Current**: All nodes inherit from `core/foundation/interfaces/inode/v1.hpp`
```cpp
class FilesystemScannerNode : public akao::foundation::interfaces::INode
```

**Impact**: 
- Nodes cannot compile without core
- Tight coupling of interface definition
- Core types (NodeValue, ExecutionResult) embedded in nodes

### 2. **Registry Auto-Registration**
**Current**: All nodes use `core/engine/orchestrator/registry/registrar.hpp`
```cpp
#include "core/engine/orchestrator/registry/registrar.hpp"
static akao::engine::orchestrator::NodeRegistrar<FilesystemScannerNode> registrar;
```

**Impact**:
- Automatic registration requires core dependency
- Build-time coupling of nodes to registry
- Static initialization dependencies

### 3. **Core Data Types**
**Dependencies identified in node code**:
- `akao::foundation::types::NodeValue`
- `akao::foundation::types::ExecutionResult`
- `akao::foundation::types::ValidationResult`
- `akao::foundation::interfaces::NodeContext`
- `akao::foundation::interfaces::NodeParameters`

### 4. **Workflow Engine Coupling**
**Current**: Core directly includes YAML node
```cpp
core/engine/orchestrator/workflow/v1.cpp:#include ".akao/nodes/yaml/v1.hpp"
```

**Impact**:
- Core cannot compile without YAML node
- Breaks separation principle
- Creates circular dependency risk

## Severity Assessment

### 🚨 CRITICAL (Immediate Action Required)
1. **Interface dependency**: All nodes depend on core interface
2. **Registry dependency**: All nodes depend on core registry
3. **Core→Node include**: Core includes specific node

### ⚠️ HIGH (Phase 2 Priority)
1. **Data type coupling**: Nodes use core data structures
2. **Build system coupling**: Makefile compiles nodes with core

### 📋 MEDIUM (Design Decisions Needed)
1. **Namespace coupling**: Shared namespace structure
2. **Metadata format**: `_.yaml` format dependencies

## Breaking Strategy

### Phase 1: Remove Direct Includes
1. **Extract interfaces**: Create protocol-based communication
2. **Remove registrar**: Replace with discovery mechanism
3. **Fix core includes**: Remove direct node dependencies

### Phase 2: Protocol Design
1. **YAML-RPC definition**: Replace interface calls
2. **Data serialization**: Replace core data types
3. **Discovery mechanism**: Replace auto-registration

### Phase 3: Implementation
1. **Node isolation**: Make nodes standalone executables
2. **Communication layer**: Implement YAML-RPC transport
3. **Process management**: External node lifecycle

## Next Actions

### Immediate (Phase 1.2)
1. **Design YAML-RPC protocol**: Define message format
2. **Create communication interfaces**: Protocol specification
3. **Plan data serialization**: NodeValue → YAML mapping

### Implementation Ready
1. **Extract YAML to core**: Move infrastructure components
2. **Remove interface dependencies**: Replace with protocol
3. **Create process management**: External node lifecycle

## Compliance with CONTRIBUTING.md

### Following Guidelines ✅
- **Archaeological**: Documenting all coupling points
- **Systematic**: Using identification standards
- **Deterministic**: Creating reproducible analysis
- **Architectural**: Planning core-node separation

### File Organization ✅
- **Artifacts**: Documentation in `sandbox/hardcore/`
- **Version control**: Following branch strategy
- **Metadata**: Complete dependency mapping

This audit reveals significant architectural debt that must be addressed to achieve true core-node independence.