# 🚀 AKAO EVOLUTION PLAN - NODE-BASED ARCHITECTURE

## 🎯 CORE VISION

Transform akao from a monolithic validation framework into a modular, node-based workflow system while maintaining:
- **Zero External Dependencies**: C++ standard library only for core
- **Philosophy-Driven Architecture**: .a format and validation central
- **Universal Validation**: Language-agnostic validation capability
- **Native C++ Performance**: Core nodes implemented in C++
- **Multi-Language Extensibility**: Community nodes in any language

## 📊 STRATEGIC APPROACH

### **Key Principles**
1. **Incremental Evolution**: Each phase builds working system
2. **Backward Compatibility**: Existing .akao rules continue working
3. **Performance First**: Core remains C++ for maximum performance
4. **Extensibility**: Community can add nodes in any language
5. **Step-by-Step Validation**: Each step must compile and pass tests

### **Development Method**
- **Direct Codebase Development**: Code directly into main codebase
- **Branch-Based Evolution**: Each phase uses separate branch
- **Immediate Commits**: Commit after each working step
- **Minimal Artifacts**: Only thinking notes, not code implementations
- **120-Line Artifact Limit**: Keep artifacts concise and focused

## 🏗️ FUTURE DIRECTORY STRUCTURE

```
akao/
├── 🏛️ core/                     # CORE ENGINE (C++ Only, Zero Dependencies)
│   ├── foundation/               # Core primitives and interfaces
│   │   ├── interfaces/           # Standard node interfaces
│   │   │   ├── node-interface/   # INode definition
│   │   │   │   ├── v1.hpp/.cpp
│   │   │   │   └── _.yaml
│   │   │   └── _.yaml
│   │   ├── data-types/          # Core data structures
│   │   │   ├── node-value/      # Universal data exchange
│   │   │   │   ├── v1.hpp/.cpp
│   │   │   │   └── _.yaml
│   │   │   ├── execution-result/ # Execution results
│   │   │   │   ├── v1.hpp/.cpp
│   │   │   │   └── _.yaml
│   │   │   └── _.yaml
│   │   └── _.yaml
│   ├── engine/                  # Core execution engines
│   │   ├── orchestrator/        # Workflow orchestration
│   │   │   ├── workflow-engine/ # Main workflow executor
│   │   │   │   ├── v1.hpp/.cpp
│   │   │   │   └── _.yaml
│   │   │   ├── node-registry/   # Node discovery system
│   │   │   │   ├── v1.hpp/.cpp
│   │   │   │   └── _.yaml
│   │   │   └── _.yaml
│   │   ├── parser/              # Language parsers (KEEP EXISTING)
│   │   │   ├── a/               # .a format parser
│   │   │   ├── yaml/            # YAML parser
│   │   │   └── _.yaml
│   │   ├── logic/               # Logic processing (KEEP EXISTING)
│   │   │   ├── runtime/         # Pure logic engine
│   │   │   └── _.yaml
│   │   ├── runtime/             # Node execution runtime
│   │   │   ├── cpp-executor/    # C++ node executor
│   │   │   │   ├── v1.hpp/.cpp
│   │   │   │   └── _.yaml
│   │   │   ├── process-manager/ # Multi-language process management
│   │   │   │   ├── v1.hpp/.cpp
│   │   │   │   └── _.yaml
│   │   │   └── _.yaml
│   │   └── _.yaml
│   └── _.yaml
├── 🧩 nodes/                    # NODE ECOSYSTEM (Modular Components)
│   ├── builtin/                 # Built-in C++ nodes (Zero Dependencies)
│   │   ├── input/               # Input acquisition nodes
│   │   │   ├── filesystem-scanner/
│   │   │   │   ├── v1.hpp/.cpp
│   │   │   │   └── _.yaml
│   │   │   ├── git-scanner/
│   │   │   │   ├── v1.hpp/.cpp
│   │   │   │   └── _.yaml
│   │   │   └── _.yaml
│   │   ├── processing/          # Core processing nodes
│   │   │   ├── cpp-analyzer/    # C++ code analysis
│   │   │   │   ├── v1.hpp/.cpp
│   │   │   │   └── _.yaml
│   │   │   ├── rule-executor/   # Rule execution engine
│   │   │   │   ├── v1.hpp/.cpp
│   │   │   │   └── _.yaml
│   │   │   ├── validator/       # Universal validation
│   │   │   │   ├── v1.hpp/.cpp
│   │   │   │   └── _.yaml
│   │   │   └── _.yaml
│   │   ├── output/              # Output generation nodes
│   │   │   ├── formatter/       # Data formatting
│   │   │   │   ├── v1.hpp/.cpp
│   │   │   │   └── _.yaml
│   │   │   ├── reporter/        # Report generation
│   │   │   │   ├── v1.hpp/.cpp
│   │   │   │   └── _.yaml
│   │   │   └── _.yaml
│   │   └── _.yaml
│   ├── external/                # Multi-language community nodes
│   │   ├── python/              # Python-based nodes
│   │   │   ├── data-science/    # Data analysis, ML
│   │   │   ├── web-scraping/    # Web scraping tools
│   │   │   └── _.yaml
│   │   ├── javascript/          # JavaScript-based nodes
│   │   │   ├── frontend-testing/ # Frontend testing tools
│   │   │   ├── api-testing/     # API testing
│   │   │   └── _.yaml
│   │   ├── rust/                # Rust-based nodes
│   │   │   ├── performance-analysis/ # Performance-critical
│   │   │   ├── crypto-operations/ # Cryptographic ops
│   │   │   └── _.yaml
│   │   └── _.yaml
│   └── _.yaml
├── 📋 workflows/                # WORKFLOW DEFINITIONS
│   ├── validation/              # Validation workflows
│   │   ├── cpp-validation.yaml  # C++ validation pipeline
│   │   ├── universal-validation.yaml # Universal validation
│   │   └── security-validation.yaml # Security checks
│   ├── analysis/                # Analysis workflows
│   │   ├── code-metrics.yaml    # Code quality metrics
│   │   ├── architecture-analysis.yaml # Architecture analysis
│   │   └── technical-debt.yaml  # Technical debt analysis
│   ├── automation/              # Automation workflows
│   │   ├── ci-pipeline.yaml     # CI/CD automation
│   │   ├── testing.yaml         # Automated testing
│   │   └── documentation.yaml   # Doc generation
│   └── examples/                # Example workflows
│       ├── hello-world.yaml     # Simple example
│       ├── data-pipeline.yaml   # Data processing
│       └── multi-language.yaml  # Multi-language processing
├── 🔧 tools/                    # DEVELOPMENT TOOLS
│   ├── node-generator/          # Node scaffolding
│   ├── workflow-validator/      # Workflow validation
│   └── migration-tools/         # Migration utilities
├── 📦 packages/                 # DISTRIBUTION PACKAGES
│   ├── core/                    # Core engine package
│   ├── standard-nodes/          # Standard node library
│   └── community-nodes/         # Community contributions
├── .akao/                       # PROJECT CONFIGURATION (KEEP EXISTING)
│   ├── settings.yaml            # System settings
│   ├── rules/                   # Rules directory
│   │   ├── enabled/
│   │   └── disabled/
│   └── cache/                   # System cache
├── main.cpp                     # SIMPLIFIED MAIN (workflow dispatcher)
└── Makefile                     # Build system
```

## 🔄 EVOLUTION PHASES

### **PHASE 1: FOUNDATION (Weeks 1-2)**
**Goal**: Establish core node infrastructure

#### **Step 1.1: Core Data Types**
- **Target**: `core/foundation/data-types/node-value/v1.hpp`
- **Implementation**: Universal data exchange system (inspired by N8N's item-based processing)
- **Features**: Batch processing support, binary data handling, metadata support
- **Test**: Basic type conversion and validation, batch data processing
- **Commit**: "Add NodeValue universal data type"

#### **Step 1.2: Node Interface**
- **Target**: `core/foundation/interfaces/node-interface/v1.hpp`
- **Implementation**: Standard INode interface
- **Test**: Mock node implementation
- **Commit**: "Add INode interface definition"

#### **Step 1.3: Node Registry**
- **Target**: `core/engine/orchestrator/node-registry/v1.hpp`
- **Implementation**: Node discovery and registration
- **Test**: Register and retrieve nodes
- **Commit**: "Add node registry system"

### **PHASE 2: CORE NODES (Weeks 3-4)**
**Goal**: Convert existing components to nodes

#### **Step 2.1: Filesystem Scanner Node**
- **Target**: `nodes/builtin/input/filesystem-scanner/v1.hpp`
- **Implementation**: Convert existing filesystem scanning
- **Test**: Scan akao project files
- **Commit**: "Add filesystem scanner node"

#### **Step 2.2: C++ Analyzer Node**
- **Target**: `nodes/builtin/processing/cpp-analyzer/v1.hpp`
- **Implementation**: Convert existing C++ analysis
- **Test**: Analyze real C++ files
- **Commit**: "Add C++ analyzer node"

#### **Step 2.3: Rule Executor Node**
- **Target**: `nodes/builtin/processing/rule-executor/v1.hpp`
- **Implementation**: Convert existing rule execution
- **Test**: Execute .a format rules
- **Commit**: "Add rule executor node"

#### **Step 2.4: Validator Node**
- **Target**: `nodes/builtin/processing/validator/v1.hpp`
- **Implementation**: Convert existing validator
- **Test**: Validate akao project
- **Commit**: "Add universal validator node"

### **PHASE 3: WORKFLOW ENGINE (Weeks 5-6)**
**Goal**: Implement workflow execution system

#### **Step 3.1: Workflow Parser**
- **Target**: `core/engine/orchestrator/workflow-engine/v1.hpp`
- **Implementation**: Parse YAML workflow definitions
- **Test**: Parse sample workflows
- **Commit**: "Add workflow definition parser"

#### **Step 3.2: Workflow Executor**
- **Target**: Extend workflow engine
- **Implementation**: Execute workflows with data flow
- **Test**: Execute multi-node workflow
- **Commit**: "Add workflow execution engine"

#### **Step 3.3: Built-in Workflows**
- **Target**: `workflows/validation/cpp-validation.yaml`
- **Implementation**: Create essential workflows
- **Test**: Execute C++ validation workflow
- **Commit**: "Add built-in validation workflows"

### **PHASE 4: MULTI-LANGUAGE SUPPORT (Weeks 7-8)**
**Goal**: Enable multi-language node development

#### **Step 4.1: Process Manager**
- **Target**: `core/engine/runtime/process-manager/v1.hpp`
- **Implementation**: Multi-language process management
- **Test**: Execute external process
- **Commit**: "Add multi-language process manager"

#### **Step 4.2: Python Node Support**
- **Target**: `nodes/external/python/` + runtime support
- **Implementation**: Python node executor
- **Test**: Execute Python node
- **Commit**: "Add Python node support"

#### **Step 4.3: JavaScript Node Support**
- **Target**: `nodes/external/javascript/` + runtime support
- **Implementation**: JavaScript node executor
- **Test**: Execute JavaScript node
- **Commit**: "Add JavaScript node support"

### **PHASE 5: INTEGRATION & POLISH (Weeks 9-10)**
**Goal**: Complete system integration and optimization

#### **Step 5.1: End-to-End Validation**
- **Target**: Complete system test
- **Implementation**: Multi-language workflow
- **Test**: Execute complex validation workflow
- **Commit**: "Complete end-to-end validation"

#### **Step 5.2: Performance Optimization**
- **Target**: Core engine optimization
- **Implementation**: Performance improvements
- **Test**: Benchmark against original system
- **Commit**: "Optimize workflow execution performance"

#### **Step 5.3: Documentation & Examples**
- **Target**: Complete documentation
- **Implementation**: User guides and examples
- **Test**: Documentation validation
- **Commit**: "Add comprehensive documentation"

## ✅ SUCCESS CRITERIA

### **Each Step Must:**
1. **Compile Successfully**: Zero compilation errors
2. **Pass All Tests**: 100% test success rate
3. **Maintain Compatibility**: Existing .akao rules work
4. **Real Data Processing**: No mock/fake data in tests
5. **Immediate Commit**: Commit working code immediately

### **Each Phase Must:**
1. **Build on Previous**: Use components from previous phases
2. **Demonstrate Value**: Show measurable improvement
3. **Working System**: Fully functional at phase end
4. **Documentation**: Clear usage examples

### **Final System Must:**
- ✅ Execute workflows 2x faster than sequential processing
- ✅ Support 3+ programming languages for nodes
- ✅ Maintain 100% backward compatibility
- ✅ Enable community node development
- ✅ Validate using node-based workflows
- ✅ Provide N8N-inspired usability while maintaining C++ core performance
- ✅ Support batch data processing for efficient large-scale operations

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