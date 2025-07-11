# 🚀 AKAO EVOLUTION PLAN - NODE-BASED ARCHITECTURE

## 🎯 CORE VISION

Completely rebuild akao as a revolutionary node-based workflow system with:
- **Zero External Dependencies**: C++ standard library only for core
- **Next-Gen Philosophy Architecture**: .a format v2 with enhanced capabilities
- **Universal Workflow System**: Everything is a workflow (validation, analysis, automation)
- **Extreme Performance**: 10x faster than legacy systems
- **Multi-Language Ecosystem**: Community-driven node marketplace with package management
- **Complete Sandboxing**: Process isolation and security framework
- **Runtime Management**: Dynamic node installation and lifecycle management

## 📊 STRATEGIC APPROACH

### **Key Principles**
1. **Revolutionary Rebuild**: Complete system rewrite for optimal architecture
2. **No Legacy Constraints**: Freedom to design perfect system
3. **Performance First**: 10x faster than any legacy system
4. **Extensibility**: Community-driven node ecosystem with external node support
5. **Security First**: Complete sandboxing and security framework
6. **Step-by-Step Validation**: Each step must compile and pass tests
7. **Clean Development**: All artifacts in artifacts/{mission-name}/{phase}/{step}/
8. **Language Agnostic**: Support any programming language for external nodes
9. **Zero Core Dependencies**: External nodes completely isolated from core

### **Development Method**
- **Direct Codebase Development**: Code directly into main codebase
- **Branch-Based Evolution**: Each phase uses separate artifact folder for taking notes (artifacts/{mission-name}/{phase}/{step}/*.md)
- **Immediate Commits**: Commit after each working step
- **Minimal Artifacts**: Only thinking notes, not code implementations
- **120-Line Artifact Limit**: Keep artifacts concise and focused

## FINAL FOLDER STRUCTURE
Please read [THE FINAL TREE](./FINAL_TREE.md) - Updated for external node ecosystem

## 🔄 EVOLUTION PHASES

### **PHASE 1: FOUNDATION (Weeks 1-2)**
**Goal**: Establish core node infrastructure

#### **Step 1.1: Core Data Types**
- **Target**: `core/foundation/types/value/v1.hpp`
- **Implementation**: Universal data exchange system (inspired by N8N's item-based processing)
- **Features**: Batch processing support, binary data handling, metadata support
- **Test**: Basic type conversion and validation, batch data processing
- **Commit**: "Add NodeValue universal data type"

#### **Step 1.2: Node Interface**
- **Target**: `core/foundation/interfaces/inode/v1.hpp`
- **Implementation**: Standard INode interface
- **Test**: Mock node implementation
- **Commit**: "Add INode interface definition"

#### **Step 1.3: Node Registry**
- **Target**: `core/engine/orchestrator/registry/v1.hpp`
- **Implementation**: Node discovery and registration
- **Test**: Register and retrieve nodes
- **Commit**: "Add node registry system"

### **PHASE 2: CORE NODES (Weeks 3-4)**
**Goal**: Convert existing components to nodes

#### **Step 2.1: Filesystem Scanner Node**
- **Target**: `nodes/builtin/file/v1.hpp`
- **Implementation**: Convert existing filesystem scanning
- **Test**: Scan akao project files
- **Commit**: "Add filesystem scanner node"

#### **Step 2.2: C++ Analyzer Node**
- **Target**: `nodes/external/cpp/v1.hpp`
- **Implementation**: Convert existing C++ analysis
- **Test**: Analyze real C++ files
- **Commit**: "Add C++ analyzer node"

#### **Step 2.3: Logic Executor Node**
- **Target**: `nodes/builtin/logic/v1.hpp`
- **Implementation**: Convert existing .a format execution
- **Test**: Execute .a format expressions
- **Commit**: "Add logic executor node"

#### **Step 2.4: Validator Node**
- **Target**: `nodes/builtin/validator/v1.hpp`
- **Implementation**: Convert existing validator
- **Test**: Validate akao project
- **Commit**: "Add universal validator node"

#### **Step 2.5: Git Analysis Node**
- **Target**: `nodes/builtin/git/v1.hpp`
- **Implementation**: Git repository analysis and operations
- **Test**: Analyze git repository
- **Commit**: "Add git analysis node"

#### **Step 2.6: YAML Parser Node**
- **Target**: `nodes/builtin/yaml/v1.hpp`
- **Implementation**: YAML parsing and stringification
- **Test**: Parse and generate YAML files
- **Commit**: "Add YAML parser node"

#### **Step 2.7: JSON Parser Node**
- **Target**: `nodes/builtin/json/v1.hpp`
- **Implementation**: JSON parsing and stringification
- **Test**: Parse and generate JSON files
- **Commit**: "Add JSON parser node"

#### **Step 2.8: Formatter Node**
- **Target**: `nodes/builtin/formatter/v1.hpp`
- **Implementation**: Data formatting and transformation
- **Test**: Format various data types
- **Commit**: "Add data formatter node"

#### **Step 2.9: Reporter Node**
- **Target**: `nodes/builtin/reporter/v1.hpp`
- **Implementation**: Report generation and aggregation
- **Test**: Generate validation reports
- **Commit**: "Add report generator node"

### **PHASE 3: WORKFLOW ENGINE (Weeks 5-6)**
**Goal**: Implement workflow execution system

#### **Step 3.1: Workflow Parser**
- **Target**: `core/engine/orchestrator/workflow/v1.hpp`
- **Implementation**: Parse YAML workflow definitions
- **Test**: Parse sample workflows
- **Commit**: "Add workflow definition parser"

#### **Step 3.2: Workflow Executor**
- **Target**: Extend workflow engine
- **Implementation**: Execute workflows with data flow
- **Test**: Execute multi-node workflow
- **Commit**: "Add workflow execution engine"

#### **Step 3.3: Built-in Workflows**
- **Target**: `.akao/workflows/validation/cpp.yaml`
- **Implementation**: Create essential workflows
- **Test**: Execute C++ validation workflow
- **Commit**: "Add built-in validation workflows"

### **PHASE 4: EXTERNAL NODE FOUNDATION (Weeks 7-10)**
**Goal**: Enable external node ecosystem with complete sandboxing

#### **Step 4.1: Complete Missing Built-in Nodes**
- **Target**: `nodes/builtin/validator/v1.hpp`, `nodes/builtin/json/v1.hpp`
- **Implementation**: Finish Universal Validator and JSON Parser nodes
- **Test**: Validate akao project, parse JSON data
- **Commit**: "Complete critical built-in nodes for external communication"

#### **Step 4.2: CLI Framework Enhancement**
- **Target**: `main.cpp` with comprehensive command framework
- **Implementation**: Node management commands (install, uninstall, enable, disable, list, info, search)
- **Test**: CLI command parsing and execution
- **Commit**: "Add comprehensive CLI for node management"

#### **Step 4.3: External Node Communication Protocol**
- **Target**: `core/engine/orchestrator/bridge/external.hpp`
- **Implementation**: JSON-RPC 2.0 over Unix sockets, external node bridge
- **Test**: Communication with mock external node
- **Commit**: "Implement external node communication protocol"

#### **Step 4.4: Process Management System**
- **Target**: `core/engine/orchestrator/process/manager.hpp`
- **Implementation**: Process lifecycle management, resource limits, health monitoring
- **Test**: Spawn, monitor, and cleanup external processes
- **Commit**: "Add process management system for external nodes"

### **PHASE 5: EXTERNAL NODE ECOSYSTEM (Weeks 11-15)**
**Goal**: Complete external node ecosystem with security and package management

#### **Step 5.1: Package Management System**
- **Target**: `core/engine/orchestrator/package/manager.hpp`
- **Implementation**: Package format (.akao archives), installation/management, dependency resolution
- **Test**: Install, uninstall, and manage external node packages
- **Commit**: "Implement comprehensive package management system"

#### **Step 5.2: Security and Sandboxing Framework**
- **Target**: `core/security/sandbox/isolator.hpp`
- **Implementation**: Process isolation, permission system, resource limits, trust verification
- **Test**: Secure external node execution with resource constraints
- **Commit**: "Add security framework with process sandboxing"

#### **Step 5.3: Dynamic Loading and Registry Enhancement**
- **Target**: Enhanced `core/engine/orchestrator/registry/v2.hpp`
- **Implementation**: Runtime node loading, external node discovery, state management
- **Test**: Dynamic loading and unloading of external nodes
- **Commit**: "Enhance registry for external node lifecycle management"

#### **Step 5.4: Example External Nodes**
- **Target**: `.akao/nodes/external/examples/`
- **Implementation**: Python, JavaScript, Go, Shell script example nodes
- **Test**: Execute multi-language external node workflow
- **Commit**: "Add example external nodes in multiple languages"

#### **Step 5.5: Integration and Testing**
- **Target**: Complete integration test suite
- **Implementation**: External node integration tests, security tests, performance benchmarks
- **Test**: End-to-end external node ecosystem validation
- **Commit**: "Complete external node ecosystem integration"

### **PHASE 6: PRODUCTION READINESS (Weeks 16)**
**Goal**: Finalize system for production deployment

#### **Step 6.1: Performance Optimization**
- **Target**: Communication protocol optimization
- **Implementation**: Connection pooling, caching, preloading, streaming optimizations
- **Test**: Benchmark external node performance overhead
- **Commit**: "Optimize external node communication performance"

#### **Step 6.2: Documentation and Polish**
- **Target**: Complete documentation suite
- **Implementation**: External node development guide, security documentation, user guides
- **Test**: Documentation validation and user testing
- **Commit**: "Add comprehensive external node documentation"

## ✅ SUCCESS CRITERIA

### **Each Step Must:**
1. **Compile Successfully**: Zero compilation errors
2. **Pass All Tests**: 100% test success rate
3. **Real Data Processing**: No mock/fake data in tests or code
4. **Immediate Commit**: Commit working code immediately
5. **Security Compliant**: All external node features must be secure by default

### **Each Phase Must:**
1. **Build on Previous**: Use components from previous phases
2. **Demonstrate Value**: Show measurable improvement
3. **Working System**: Fully functional at phase end
4. **Documentation**: Clear usage examples
5. **Backward Compatible**: Existing functionality preserved

### **Final System Must:**
- ✅ Execute workflows 10x faster than legacy systems
- ✅ Support 5+ programming languages for external nodes
- ✅ Enable thriving community node marketplace with package management
- ✅ Replace all legacy validation with workflows
- ✅ Provide revolutionary user experience
- ✅ Support massive-scale data processing
- ✅ Zero legacy technical debt
- ✅ **NEW**: Complete external node sandboxing and security
- ✅ **NEW**: Runtime node installation and management
- ✅ **NEW**: Package management system with dependency resolution
- ✅ **NEW**: External node execution overhead < 10ms
- ✅ **NEW**: Support for 100+ external nodes simultaneously

## 🛠️ DEVELOPMENT GUIDELINES

### **Artifact Management**
- **Keep artifacts minimal**: Only thinking notes, max 120 lines
- **No code in artifacts**: All code goes directly into main codebase
- **Branch-based development**: Each phase uses separate branch
- **Immediate commits**: Commit after each working step

### **Code Quality**
- **Zero external dependencies**: Core uses only C++ standard library
- **Philosophy compliance**: Follow akao design principles
- **English grammar**: Correct naming conventions
- **Metadata headers**: Complete documentation for all components

### **Testing Strategy**
- **Unit tests**: Test individual components
- **Integration tests**: Test component interactions
- **End-to-end tests**: Test complete workflows
- **Performance tests**: Benchmark against existing system

This evolution plan provides a clear, step-by-step approach to transforming akao into a powerful node-based workflow system while maintaining its core philosophy and performance characteristics.