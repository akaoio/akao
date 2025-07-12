# 🎯 AKAO CURRENT SYSTEM OVERVIEW

**Current Reality**: **C++ node-based workflow orchestrator** with hardcore separation architecture

---

## 📋 **System Architecture**

### **✅ What Akao IS:**
- **C++ Workflow Orchestrator Platform** 
- **Core**: Pure orchestrator managing process lifecycle (NO business logic)
- **Nodes**: Independent processes communicating via YAML-RPC protocol
- **Architecture**: Complete core-node independence with Unix Domain Socket communication
- **Discovery**: Manifest-based automatic node registration

### **❌ What Akao is NOT:**
- ~~Validation system~~ (LEGACY - moved to `legacy-do-not-read/`)
- ~~Philosophy engine~~ (LEGACY - moved to `legacy-do-not-read/`)
- ~~Rule-based processor~~ (LEGACY - moved to `legacy-do-not-read/`)
- ~~Tracing system~~ (LEGACY - moved to `legacy-do-not-read/`)

---

## 🏗️ **Current Implementation**

### **Core Orchestrator** (`core/`)
```
core/
├── foundation/formats/yaml/     # YAML infrastructure for protocol
├── engine/communication/yamlrpc/ # YAML-RPC protocol implementation  
├── engine/orchestrator/process/ # External node process management
└── engine/orchestrator/discovery/ # Manifest-based node discovery
```

### **Independent Nodes** (`.akao/nodes/`)
```
.akao/nodes/
├── file/                       # File scanner (filesystem analysis)
├── logic/                      # Logic executor (.a format expressions)
├── reporter/                   # Report generator (multi-format)
└── yaml/                       # YAML processor (parsing/generation)
```

### **Communication Architecture**
```
User → CLI → Core Orchestrator → Node Process
              ↓ YAML-RPC ↓
         Unix Domain Socket
```

---

## 🧪 **System Status**

### **✅ COMPLETED (100% Functional)**
- **Hardcore separation architecture** - Complete core-node independence
- **YAML-RPC protocol** - Full protocol implementation with Unix Domain Sockets
- **Node discovery system** - Manifest-based automatic registration  
- **Process management** - External node lifecycle management
- **Independent nodes** - All 4 nodes (file, logic, reporter, yaml) as separate processes
- **System integration tests** - Comprehensive test suite proving system works

### **🚧 IN PROGRESS**
- **CLI implementation** - User interface for the orchestrator

---

## 📁 **Directory Structure**

### **Active Development** 
```
akao/
├── core/                       # ✅ Core orchestrator (infrastructure only)
├── .akao/nodes/                # ✅ Independent node processes  
├── test/                       # ✅ System integration tests
└── sandbox/                    # 📋 Current documentation
    ├── cli/                    # 🚧 CLI design (if recreated)
    ├── hardcore/               # ✅ Architecture documentation
    └── legacy-do-not-read/     # 🗑️ Legacy content (DO NOT READ)
```

### **Legacy Content (DO NOT READ)**
```
sandbox/legacy-do-not-read/
├── design/                     # Old validation system designs
├── prompts/                    # Legacy development prompts  
├── thoughts/                   # Legacy architectural thoughts
└── README.md                   # Legacy project overview
```

---

## 🎯 **Current Focus**

### **System Reality Check**
- **NOT** a validation system ❌
- **NOT** a philosophy engine ❌  
- **NOT** a rule processor ❌
- **IS** a C++ workflow orchestrator ✅
- **IS** a node-based processing platform ✅
- **IS** a hardcore separation architecture ✅

### **Commands Context**
```bash
# These are SYSTEM HEALTH checks, NOT legacy validation:
akao status         # Core orchestrator health status
akao validate       # System configuration health check  
akao test          # System integration tests

# These are WORKFLOW operations:
akao list --node          # List available processing nodes
akao run --node file      # Execute file scanner node
akao run --workflow build # Execute build workflow
```

---

## 📋 **Documentation Status**

### **✅ Updated for Current System**
- **CONTRIBUTING.md** - Rewritten for C++ orchestrator development
- **sandbox/hardcore/** - Architecture documentation (correct)
- **test/** - Integration tests proving system works
- **.akao/nodes/*/_.yaml** - Node manifests (correct)

### **🗑️ Moved to Legacy**
- **validation system references** → `legacy-do-not-read/`
- **philosophy content** → `legacy-do-not-read/`
- **rule/ruleset content** → `legacy-do-not-read/`
- **tracing system references** → `legacy-do-not-read/`

---

## 🚀 **Next Steps**

1. **CLI Implementation** - Create user interface for the orchestrator
2. **Workflow Management** - Advanced workflow definition and execution  
3. **External Node Support** - Installation and management of third-party nodes
4. **Performance Optimization** - Connection pooling and resource management

---

**🎯 Reality: Akao is a C++ node-based workflow orchestrator with complete core-node independence!**