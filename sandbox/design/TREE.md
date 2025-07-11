# 🏗️ CURRENT AKAO C++ ORCHESTRATOR DIRECTORY STRUCTURE

**⚠️ PROVISIONAL**: This project is in active design phase using "vibe coding" methodology.

## 📁 ACTUAL CURRENT STRUCTURE

```
akao/
├── 🏛️ core/                         # PURE C++ ORCHESTRATOR (Zero Dependencies)
│   ├── foundation/                  # Core primitives and interfaces
│   │   ├── interfaces/              # Standard node interfaces
│   │   │   ├── inode/               # INode definition
│   │   │   │   ├── v1.hpp/.cpp    
│   │   │   │   └── _.yaml
│   │   │   └── _.yaml
│   │   ├── types/                   # Core data structures
│   │   │   ├── value/               # Universal data exchange
│   │   │   │   ├── v1.hpp/.cpp    
│   │   │   │   └── _.yaml
│   │   │   ├── result/              # Execution results
│   │   │   │   ├── v1.hpp/.cpp    
│   │   │   │   └── _.yaml
│   │   │   └── _.yaml
│   │   └── _.yaml
│   ├── engine/                      # Core execution engines
│   │   ├── orchestrator/            # Workflow orchestration
│   │   │   ├── workflow/            # Main workflow executor
│   │   │   │   ├── v1.hpp/.cpp    
│   │   │   │   └── _.yaml
│   │   │   ├── registry/            # Node discovery system
│   │   │   │   ├── v1.hpp/.cpp    
│   │   │   │   └── _.yaml
│   │   │   └── _.yaml
│   │   └── _.yaml
│   └── _.yaml
├── 🔧 .akao/                        # PROJECT CONFIGURATION
│   ├── nodes/                       # SELF-CONTAINED PROCESSING NODES
│   │   ├── file/                    # Filesystem operations
│   │   │   ├── tests/               # Self-contained tests
│   │   │   │   ├── v1.cpp
│   │   │   │   └── test_ext_dir/
│   │   │   ├── v1.hpp/.cpp
│   │   │   └── _.yaml
│   │   ├── logic/                   # Logic execution
│   │   │   ├── tests/               # Self-contained tests
│   │   │   │   └── v1.cpp
│   │   │   ├── v1.hpp/.cpp
│   │   │   └── _.yaml
│   │   ├── yaml/                    # YAML processing
│   │   │   ├── tests/               # Self-contained tests
│   │   │   │   └── v1.cpp
│   │   │   ├── v1.hpp/.cpp
│   │   │   └── _.yaml
│   │   ├── reporter/                # Output generation
│   │   │   ├── tests/               # Self-contained tests
│   │   │   │   └── v1.cpp
│   │   │   ├── v1.hpp/.cpp
│   │   │   └── _.yaml
│   │   └── _.yaml
│   ├── workflows/                   # YAML-BASED WORKFLOWS
│   │   └── examples/                # Example workflows
│   │       ├── simple.yaml          # Simple example
│   │       └── validation.yaml      # Validation workflow
│   └── settings.yaml                # System configuration
├── 🧪 tests/                        # PLATFORM TESTS
│   └── unit/                        # Unit test suite
│       ├── core/                    # Core platform tests
│       │   ├── engine/              # Engine tests
│       │   └── foundation/          # Foundation tests
│       ├── nodes/                   # Legacy test location
│       │   └── builtin/             # Old test structure
│       └── test-runner.cpp          # Main test runner
├── 📁 examples/                     # EXHIBITION FILES
│   ├── akao-format/                 # Format examples
│   └── godel-exhibition.a           # Logic exhibition
├── 📋 sandbox/                      # DEVELOPMENT WORKSPACE
│   ├── design/                      # Design documentation
│   ├── prompts/                     # Development prompts
│   └── thoughts/                    # Development thoughts
├── 🔧 build/                        # BUILD ARTIFACTS
│   ├── bin/                         # Executables
│   └── release/                     # Release builds
├── main.cpp                         # CLI ENTRY POINT
├── Makefile                         # BUILD SYSTEM
├── CONTRIBUTING.md                  # CONTRIBUTION GUIDELINES
└── LICENSE                          # LICENSE FILE
```

## 🏗️ ARCHITECTURE PRINCIPLES

### **Core Orchestrator** ✅
- **Pure C++ Implementation**: Zero external dependencies
- **Foundation Layer**: Universal types and interfaces
- **Engine Layer**: Registry and workflow execution
- **Orchestration Only**: No processing logic in core

### **Node System** ✅
- **Self-Contained**: Each node independent with own tests
- **Standard Interface**: All nodes implement INode
- **Automatic Discovery**: Registry scans `.akao/nodes/`
- **Processing Units**: Specialized functionality per node

### **Workflow System** ✅
- **YAML-Based**: Human-readable workflow definitions
- **Data Flow**: Connections between nodes
- **Execution Engine**: Sequential and parallel execution
- **Configuration**: Stored in `.akao/workflows/`

### **Testing Architecture** ✅
- **Platform Tests**: Core orchestrator testing in `tests/`
- **Node Tests**: Self-contained tests in `.akao/nodes/{node}/tests/`
- **Comprehensive Coverage**: Unit and integration tests
- **Build Integration**: Makefile test execution

## 🔧 CURRENT OPERATIONAL STATUS

### **Working Components** ✅
- **Core Orchestrator**: Fully operational
- **Node Registry**: Automatic discovery and registration
- **Workflow Engine**: YAML-based execution
- **Build System**: Makefile with comprehensive testing
- **4 Active Nodes**: file, logic, yaml, reporter

### **CLI Support** ✅
- **Basic Commands**: version, help, basic operations
- **Node Execution**: Direct node invocation
- **Workflow Execution**: YAML workflow processing
- **Build Integration**: Test and validation commands

### **Development Environment** ✅
- **Zero Dependencies**: C++ standard library only
- **Clean Build**: No external dependencies or tools
- **Comprehensive Testing**: All components tested
- **Documentation**: Design docs and guides

## 🚀 FUTURE EXPANSION AREAS

### **Enhanced CLI** (Future)
- **Node Management**: list, run, install, uninstall
- **Workflow Management**: comprehensive workflow operations
- **System Operations**: status, validate, test commands
- **Global Flags**: verbose, quiet, output format options

### **External Node Support** (Future)
- **Communication Protocol**: JSON-RPC 2.0 over Unix sockets
- **Process Management**: External node lifecycle management
- **Package System**: Installation and management
- **Security Framework**: Sandboxing and permissions

### **Advanced Features** (Future)
- **Conditional Workflows**: If/else and loop constructs
- **Parallel Execution**: Concurrent node processing
- **Performance Optimization**: Caching and optimization
- **Monitoring**: Logging and performance metrics

This directory structure represents the current working C++ workflow orchestrator with a clear foundation for future enhancements while maintaining the core architectural principles.