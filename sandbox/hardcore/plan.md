# 🚀 MINIMAL CORE ARCHITECTURE - HARDCORE PLAN

**⚠️ PROVISIONAL**: This project is in active design phase using "vibe coding" methodology.

## 🎯 OBJECTIVE: COMPLETE CORE-NODE SEPARATION

### Core Principle
**Absolute Independence**: Core và Nodes hoàn toàn tách biệt, giao tiếp qua YAML-RPC protocol

### Architecture Vision

#### Minimal Core (Infrastructure Only)
```
core/
├── foundation/
│   ├── types/              # NodeValue, ExecutionResult
│   ├── interfaces/         # INode, communication interfaces
│   ├── formats/
│   │   └── yaml/          # YAML parser (infrastructure only)
│   └── config/            # Configuration management
├── engine/
│   ├── orchestrator/
│   │   ├── workflow/      # Workflow execution engine
│   │   ├── registry/      # Node discovery & registry
│   │   └── process/       # External node process management
│   └── communication/     # YAML-RPC protocol
└── _.yaml
```

#### All Processing = External Nodes
```
.akao/nodes/
├── file/                  # Filesystem operations
├── logic/                 # Expression evaluation & .a format
├── yaml/                  # YAML data processing (business logic)
├── reporter/              # Output formatting
├── json/                  # JSON processing
├── http/                  # HTTP operations
├── validator/             # Data validation
└── ...                    # Future nodes
```

## 📋 IMPLEMENTATION PHASES

### Phase 1: Dependency Analysis & Breaking
**Goal**: Map and eliminate all dependencies between core and nodes

#### Step 1.1: Dependency Audit
- [ ] **Map current includes**: `grep -r "#include.*core/" .akao/nodes/`
- [ ] **Map reverse includes**: `grep -r "#include.*\.akao/" core/`
- [ ] **Document coupling points**: All interfaces, data types, utilities
- [ ] **Identify critical dependencies**: NodeValue, INode, ExecutionResult

#### Step 1.2: Design Independence Interfaces
- [ ] **Define YAML-RPC protocol**: Message format, error handling
- [ ] **Design node manifest format**: `_.yaml` specification
- [ ] **Create communication contracts**: Input/output schemas
- [ ] **Plan data serialization**: NodeValue → YAML conversion

### Phase 2: YAML Infrastructure Extraction
**Goal**: Move core YAML functionality, maintain business logic in nodes

#### Step 2.1: Split YAML Responsibilities
- [ ] **Extract core YAML**: Move to `core/foundation/formats/yaml/`
- [ ] **Minimal feature set**: Config parsing, communication protocol only
- [ ] **Zero external deps**: C++ stdlib implementation only
- [ ] **Create YAML node**: Advanced processing in `.akao/nodes/yaml/`

#### Step 2.2: YAML-RPC Protocol Implementation
- [ ] **Define protocol spec**:
  ```yaml
  yamlrpc: "1.0"
  method: "node.execute"
  params:
    input: {...}
    config: {...}
  id: "req-123"
  ```
- [ ] **Implement protocol layer**: `core/engine/communication/protocol/`
- [ ] **Create transport layer**: Unix sockets in `core/engine/communication/transport/`
- [ ] **Error handling**: Timeout, retry, failure recovery

### Phase 3: Process Management System
**Goal**: External node lifecycle management

#### Step 3.1: Node Discovery
- [ ] **Directory scanning**: Scan `.akao/nodes/` for node manifests
- [ ] **Metadata parsing**: Read `_.yaml` node specifications
- [ ] **Registry update**: Dynamic node registration/deregistration
- [ ] **Health monitoring**: Node status tracking

#### Step 3.2: Process Lifecycle
- [ ] **Node launching**: `core/engine/orchestrator/process/launcher/`
- [ ] **Process management**: `core/engine/orchestrator/process/manager/`
- [ ] **Communication setup**: Socket creation, connection management
- [ ] **Graceful shutdown**: Clean process termination

### Phase 4: Node Independence Implementation
**Goal**: Make all nodes completely self-contained

#### Step 4.1: Remove Core Dependencies
- [ ] **File node**: Remove `#include` from core, implement YAML-RPC client
- [ ] **Logic node**: Self-contained with own data types
- [ ] **Reporter node**: Independent implementation
- [ ] **YAML node**: Business logic processing only

#### Step 4.2: Node Communication
- [ ] **YAML-RPC client**: Each node implements protocol client
- [ ] **Message handling**: Request parsing, response generation
- [ ] **Error reporting**: Standard error codes and messages
- [ ] **Health checks**: Node status reporting

### Phase 5: Testing & Validation
**Goal**: Verify complete independence and functionality

#### Step 5.1: Independence Testing
- [ ] **Standalone builds**: Each node builds without core
- [ ] **Core isolation**: Core builds without nodes
- [ ] **Communication tests**: YAML-RPC protocol validation
- [ ] **Process tests**: Node lifecycle management

#### Step 5.2: Functionality Testing
- [ ] **Workflow execution**: End-to-end workflow testing
- [ ] **Node operations**: All existing functionality preserved
- [ ] **Performance testing**: Communication overhead measurement
- [ ] **Error handling**: Failure scenarios and recovery

## 🛠️ TECHNICAL SPECIFICATIONS

### YAML-RPC Protocol

#### Message Format
```yaml
# Request
yamlrpc: "1.0"
method: "node.execute"
params:
  input:
    data: "example input"
    metadata:
      source: "workflow-123"
  config:
    mode: "production"
    timeout: 30
id: "req-456"

# Response
yamlrpc: "1.0"
result:
  output:
    data: "processed result"
    metadata:
      execution_time: 1234
      memory_used: 5678
id: "req-456"

# Error
yamlrpc: "1.0"
error:
  code: -32603
  message: "Internal error"
  data:
    details: "Node execution failed"
id: "req-456"
```

#### Transport Layer
- **Unix Domain Sockets**: `/tmp/akao-node-{node-id}.sock`
- **Connection pooling**: Reuse connections for multiple requests
- **Timeout handling**: Configurable request timeout
- **Retry mechanism**: Automatic retry for transient failures

### Node Independence Requirements

#### Node Manifest (`_.yaml`)
```yaml
id: "akao:node:filesystem:scanner:v1"
name: "Filesystem Scanner"
version: "1.0.0"
description: "Scans and analyzes filesystem structures"

runtime:
  type: "executable"
  command: "./scanner"
  args: ["--mode", "server"]

communication:
  protocol: "yamlrpc"
  version: "1.0"
  socket: "/tmp/akao-node-file.sock"

inputs:
  - name: "path"
    type: "string"
    required: true
  - name: "recursive"
    type: "boolean"
    default: false

outputs:
  - name: "files"
    type: "array"
  - name: "statistics"
    type: "object"

resources:
  memory: "128MB"
  cpu: "100m"
  timeout: "30s"
```

#### Node Implementation Template
```cpp
// Node implementation (completely independent)
#include <iostream>
#include <string>
// NO core includes allowed

class NodeServer {
public:
    void start(const std::string& socket_path);
    void handle_request(const std::string& yaml_request);
    std::string process_execute(const std::map<std::string, std::string>& params);
};

int main(int argc, char* argv[]) {
    NodeServer server;
    server.start("/tmp/akao-node-file.sock");
    return 0;
}
```

## 📊 SUCCESS CRITERIA

### Technical Metrics
- [ ] **Zero includes**: No `#include` between core and nodes
- [ ] **Independent builds**: Nodes compile without core
- [ ] **Protocol compliance**: All communication via YAML-RPC
- [ ] **Process isolation**: Nodes run as separate processes

### Functional Metrics
- [ ] **Feature parity**: All existing functionality preserved
- [ ] **Performance**: Communication overhead < 10ms per operation
- [ ] **Reliability**: Node failures don't crash core
- [ ] **Extensibility**: Easy to add new nodes

### Quality Metrics
- [ ] **Test coverage**: Comprehensive testing of separation
- [ ] **Documentation**: Clear node development guidelines
- [ ] **Examples**: Reference node implementations
- [ ] **Migration guide**: Step-by-step transition plan

## 🚨 CRITICAL CONSTRAINTS

### Core Rules
1. **No external dependencies**: Core uses only C++ stdlib
2. **No business logic**: Core is pure orchestrator
3. **Infrastructure only**: Configs, communication, process management
4. **Standard interfaces**: Well-defined YAML-RPC protocol

### Node Rules
1. **No core includes**: Completely independent compilation
2. **YAML-RPC only**: Standard communication protocol
3. **Self-contained**: Own dependencies, own build system
4. **Standard manifest**: Required `_.yaml` metadata

### Communication Rules
1. **Protocol versioning**: Backward compatibility support
2. **Error handling**: Standard error codes and recovery
3. **Performance**: Minimal serialization overhead
4. **Security**: Input validation and sanitization

## 📈 DEVELOPMENT WORKFLOW

### Implementation Order
1. **Dependency analysis** → Understand current coupling
2. **YAML extraction** → Create core infrastructure
3. **Protocol design** → Define communication standard
4. **Process management** → Node lifecycle system
5. **Node conversion** → Eliminate dependencies one by one
6. **Testing & validation** → Verify complete separation

### Quality Gates
- [ ] **Each phase**: Must compile and pass existing tests
- [ ] **No regressions**: All existing functionality preserved
- [ ] **Progressive isolation**: Incremental dependency removal
- [ ] **Documentation**: Updated guides and examples

This hardcore plan transforms the current coupled architecture into a truly independent, scalable system where core and nodes operate as separate, communicating processes.