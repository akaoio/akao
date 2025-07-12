# Contributing to Akao C++ Workflow Orchestrator

**🎯 Current System**: C++ node-based workflow orchestrator with hardcore separation architecture

## 📋 **System Overview**

**Akao** is a C++ workflow orchestrator platform with complete core-node independence:

- **Core**: Pure orchestrator managing process lifecycle (no business logic)
- **Nodes**: Independent processes communicating via YAML-RPC protocol  
- **Architecture**: Hardcore separation with Unix Domain Socket communication
- **Discovery**: Manifest-based automatic node registration

## 🏗️ **Architecture Principles**

### **1. Hardcore Separation**
- **Core ≠ Nodes**: Complete independence, no shared dependencies
- **Communication**: YAML-RPC protocol over Unix Domain Sockets only
- **Processes**: Each node runs as separate process managed by core
- **Manifests**: YAML-based node discovery and configuration

### **2. Node Independence**
- **Self-contained**: Each node builds and runs independently
- **No core includes**: Zero `#include` statements referencing core
- **Protocol-based**: All communication via YAML-RPC messages
- **Lifecycle**: Managed by core process management system

### **3. Current Node Types**
- **File Scanner**: `.akao/nodes/file/` - Filesystem analysis and scanning
- **Logic Executor**: `.akao/nodes/logic/` - .a format expression evaluation
- **Report Generator**: `.akao/nodes/reporter/` - Multi-format report generation
- **YAML Processor**: `.akao/nodes/yaml/` - YAML parsing and generation

## 🔧 **Development Guidelines**

### **Component Identification**
```
Pattern: akao:[type]:[category]:[name]:[version]
Types: core, node, workflow, orchestrator, cli
Examples: 
- akao:node:filesystem:scanner:v1
- akao:core:orchestrator:process:v1
- akao:workflow:build:cpp:v1
```

### **File Structure**
```
akao/
├── core/                           # Core orchestrator (no business logic)
│   ├── engine/orchestrator/        # Process & workflow management
│   ├── engine/communication/       # YAML-RPC protocol implementation
│   └── foundation/                 # Core infrastructure only
├── .akao/nodes/                    # Independent node processes
│   ├── file/                       # File scanner node
│   ├── logic/                      # Logic executor node
│   ├── reporter/                   # Report generator node
│   └── yaml/                       # YAML processor node
├── cli/                            # CLI implementation
└── test/                           # System integration tests
```

### **Node Development**
```
{node}/
├── _{manifest}.yaml               # Node manifest (required)
├── independent/                   # Independent implementation
│   ├── independent.hpp            # Self-contained headers
│   ├── independent.cpp            # Implementation with YAML-RPC server
│   └── Makefile                   # Independent build system
├── tests/                         # Node-specific tests
└── README.md                      # Node documentation
```

## 📝 **File Naming Standards**

### **Directory Naming**
- **Characters**: Only `a-z`, `0-9`, `-` (NO underscores, NO versions)
- **Grammar**: Singular for functions, plural for collections
- **Examples**: `orchestrator/`, `nodes/`, `communication/`

### **File Naming**  
- **Pattern**: `{function}/{version}.{ext}`
- **Characters**: Only `a-z`, `0-9`, `-`, `.`
- **Examples**: `v1.cpp`, `v1.hpp`

## 🎯 **Development Workflow**

### **1. Node Development**
```bash
# Create new node
mkdir .akao/nodes/{node-name}
cd .akao/nodes/{node-name}

# Create manifest
cat > _.yaml << EOF
id: "akao:node:{category}:{name}:v1"
name: "{Node Name}"
description: "{Node description}"
communication:
  protocol: "yamlrpc"
  version: "1.0"
EOF

# Implement independent version
mkdir independent
# Create independent.hpp, independent.cpp with YAML-RPC server
# Create Makefile for independent build

# Test node
make && make test
```

### **2. Core Development**
```bash
# Core changes (infrastructure only)
cd core/
# Implement in appropriate subdirectory
# NO business logic in core - only orchestration
```

### **3. CLI Development**
```bash
# CLI implementation
cd cli/
# Implement CLI that communicates with core
# CLI → Core → Nodes architecture
```

## 📋 **Documentation Requirements**

### **Component Headers**
```cpp
/**
 * @id: akao:{type}:{category}:{name}:{version}
 * @doc: {20-200 word description}
 * @specification: {Technical specification}
 * @scope: {Architectural location}
 * @timeline: {YYYY-MM-DD}
 * @rationale: {Why this component exists}
 * @methodology: {Implementation approach}
 * @references: {Related components or specs}
 */
```

### **Manifest Requirements**
```yaml
id: "akao:node:{category}:{name}:v1"
name: "{Human-readable name}"
version: "1.0.0"
description: "{Functionality description}"

runtime:
  type: "executable" 
  command: "./independent/{executable}"

communication:
  protocol: "yamlrpc"
  version: "1.0"
  socket_path: "/tmp/akao-node-{name}.sock"
  methods: ["info", "validate", "execute", "health", "shutdown"]

inputs: [{input specifications}]
outputs: [{output specifications}]
resources: {resource limits}
dependencies: {dependencies}
metadata: {additional metadata}
```

## 🧪 **Testing Requirements**

### **Node Testing**
- **Unit tests**: Test node functionality independently
- **Integration tests**: Test YAML-RPC communication
- **Process tests**: Test node lifecycle management

### **System Testing**
- **Build tests**: Verify all components build successfully
- **Communication tests**: Test YAML-RPC protocol
- **Integration tests**: Test end-to-end workflows

## 🔍 **System Commands**

### **Build System**
```bash
# Build all nodes
make build-nodes

# Build core
make build-core  

# Build CLI
make build-cli

# Run system tests
make test
```

### **CLI Commands**
```bash
# List nodes
akao list --node

# Run node
akao run --node {name} -- {args}

# System status (health check, NOT legacy validation)
akao status

# System health check (NOT legacy validation)  
akao validate

# Run system tests
akao test
```

## ⚠️ **Critical Guidelines**

### **DO NOT:**
- ❌ Add business logic to core (hardcore separation violation)
- ❌ Include core headers in nodes (independence violation)
- ❌ Use direct C++ interfaces between core and nodes
- ❌ Create shared dependencies between components

### **DO:**
- ✅ Implement nodes as independent processes
- ✅ Use YAML-RPC protocol for all communication
- ✅ Build each node independently
- ✅ Maintain manifest-based configuration
- ✅ Test components in isolation

## 🎯 **Quality Standards**

### **Architecture Compliance**
- **Independence**: No core dependencies in nodes
- **Communication**: YAML-RPC protocol only
- **Separation**: Clear core/node boundaries
- **Testing**: Comprehensive test coverage

### **Code Quality**
- **Headers**: Complete metadata for all components
- **Documentation**: Clear, concise descriptions
- **Testing**: Unit and integration tests
- **Build**: Independent compilation

## 🚀 **Current Status**

### **Completed ✅**
- Hardcore separation architecture
- YAML-RPC protocol implementation
- Node discovery and registry system
- Process management system
- Independent node implementations
- System integration tests

### **In Progress ⏳**
- CLI implementation with core integration
- Advanced workflow management
- External node support

## 📞 **Support**

For questions about:
- **Architecture**: Refer to `sandbox/hardcore/plan.md`
- **CLI Design**: See `sandbox/cli/` documentation  
- **Testing**: Check `test/` directory
- **Node Development**: Examine existing nodes in `.akao/nodes/`

**🎯 Focus: C++ workflow orchestrator with hardcore core-node separation architecture!**