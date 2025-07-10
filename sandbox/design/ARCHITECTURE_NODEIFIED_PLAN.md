# 🏗️ AKAO NODE-BASED ARCHITECTURE TRANSFORMATION PLAN

## 📋 EXECUTIVE SUMMARY

Kế hoạch chuyển đổi hệ thống Akao từ kiến trúc hardcoded sang **fully configurable node-based workflow system** giống N8N nhưng được viết bằng C++ với khả năng hỗ trợ multi-language nodes.

## 🔍 CURRENT STATE ANALYSIS

### Vấn đề hiện tại:
- **Hardcoded workflows** trong main.cpp
- **Direct component coupling** - các thành phần gọi thẳng vào nhau
- **Embedded business logic** - logic nghiệp vụ được nhúng sâu trong code
- **Single workflow limitation** - chỉ hỗ trợ 1 workflow duy nhất
- **No external configuration** - không thể config từ bên ngoài

### Kiến trúc hiện tại:
```
CLI → Command Router → Direct Function Call → Component → Hardcoded Logic → Output
```

## 🎯 TARGET ARCHITECTURE

### Kiến trúc mục tiêu:
```
YAML Workflow Definition → Node Orchestrator → Node Registry → Execution Engine → Multi-language Node Support
```

## 📐 STANDARD NODE INTERFACE SPECIFICATION

### 1. Core Node Interface (INode)

```cpp
/**
 * @brief Universal Node Interface - Tiêu chuẩn cho tất cả nodes
 * 
 * Inspired by YAML parser node architecture
 * Supports multi-language implementation (C++, Python, JavaScript, Rust, Java)
 */
class INode {
public:
    // Node Identity
    virtual std::string getNodeId() const = 0;
    virtual std::string getNodeType() const = 0;
    virtual std::string getVersion() const = 0;
    
    // Node Metadata
    virtual NodeMetadata getMetadata() const = 0;
    virtual std::vector<PortDefinition> getInputPorts() const = 0;
    virtual std::vector<PortDefinition> getOutputPorts() const = 0;
    
    // Node Configuration
    virtual void configure(const YAML::Node& config) = 0;
    virtual YAML::Node getConfiguration() const = 0;
    virtual bool validateConfiguration() const = 0;
    
    // Node Execution
    virtual ExecutionResult execute(const NodeContext& context) = 0;
    virtual bool canExecute(const NodeContext& context) const = 0;
    
    // Node Lifecycle
    virtual void initialize() = 0;
    virtual void cleanup() = 0;
    virtual NodeState getState() const = 0;
    
    // Multi-language Support
    virtual NodeLanguage getLanguage() const = 0;
    virtual std::string getExecutablePath() const = 0;
    
    virtual ~INode() = default;
};
```

### 2. Node Data Types

```cpp
// Standard data exchange format inspired by YAML node values
enum class DataType {
    STRING,
    INTEGER,
    FLOAT,
    BOOLEAN,
    NULL_VALUE,
    ARRAY,
    OBJECT,
    BINARY,
    FILE_REFERENCE
};

class NodeValue {
private:
    DataType type_;
    std::variant<
        std::string,                           // STRING
        int64_t,                               // INTEGER
        double,                                // FLOAT
        bool,                                  // BOOLEAN
        std::nullptr_t,                        // NULL_VALUE
        std::vector<NodeValue>,                // ARRAY
        std::map<std::string, NodeValue>,      // OBJECT
        std::vector<uint8_t>,                  // BINARY
        std::string                            // FILE_REFERENCE (path)
    > value_;
    
public:
    // Type-safe constructors and accessors (similar to YamlNode)
    explicit NodeValue(const std::string& value);
    explicit NodeValue(int64_t value);
    explicit NodeValue(double value);
    explicit NodeValue(bool value);
    explicit NodeValue(std::nullptr_t);
    explicit NodeValue(const std::vector<NodeValue>& array);
    explicit NodeValue(const std::map<std::string, NodeValue>& object);
    
    // Type checking and conversion methods
    DataType getType() const;
    bool isString() const;
    bool isInteger() const;
    bool isFloat() const;
    bool isBoolean() const;
    bool isNull() const;
    bool isArray() const;
    bool isObject() const;
    
    // Safe accessors with defaults
    std::string asString() const;
    int64_t asInteger() const;
    double asFloat() const;
    bool asBoolean() const;
    std::vector<NodeValue> asArray() const;
    std::map<std::string, NodeValue> asObject() const;
    
    // Serialization
    std::string toJson() const;
    YAML::Node toYaml() const;
    static NodeValue fromJson(const std::string& json);
    static NodeValue fromYaml(const YAML::Node& yaml);
};
```

### 3. Workflow Definition Format

```yaml
# Example: validation_workflow.yaml
workflow:
  id: "akao_validation_workflow_v1"
  name: "Akao Universal Validation"
  version: "1.0.0"
  description: "Configurable validation workflow"
  
nodes:
  - id: "input_scanner"
    type: "filesystem.scanner"
    language: "cpp"
    config:
      path: "${workflow.input.path}"
      recursive: true
      patterns: ["*.cpp", "*.hpp", "*.yaml"]
    
  - id: "cpp_analyzer"
    type: "cpp.analyzer"
    language: "cpp"
    config:
      rules: ["one_class_per_file", "modern_cpp_features"]
    
  - id: "rule_executor"
    type: "logic.rule_executor"
    language: "cpp"
    config:
      engine: "pure_logic"
      rules_directory: ".akao/rules/enabled"
    
  - id: "python_checker"
    type: "python.style_checker"
    language: "python"
    executable: "/usr/bin/python3"
    script: "scripts/python_validator.py"
    
  - id: "output_formatter"
    type: "output.formatter"
    language: "cpp"
    config:
      format: "${workflow.output.format}"
      template: "validation_report.tmpl"

connections:
  - from: "input_scanner.files"
    to: "cpp_analyzer.input"
    
  - from: "input_scanner.files"
    to: "rule_executor.files"
    
  - from: "cpp_analyzer.results"
    to: "output_formatter.cpp_results"
    
  - from: "rule_executor.results"
    to: "output_formatter.rule_results"

execution:
  mode: "parallel"
  max_concurrency: 4
  timeout: 300
  retry_policy:
    max_retries: 3
    backoff: "exponential"
```

## 🏛️ SYSTEM ARCHITECTURE COMPONENTS

### 1. Core System (Cannot be Nodified)

```
core/
├── engine/
│   ├── orchestrator/           # NEW: Workflow orchestrator
│   │   ├── workflow_engine/    # Workflow execution engine
│   │   ├── node_registry/      # Node discovery and management
│   │   └── execution_context/  # Execution state management
│   ├── parser/
│   │   ├── a/                  # .a format parser (KEEP)
│   │   └── yaml/               # YAML parser (KEEP)
│   └── runtime/
│       ├── node_runtime/       # NEW: Node execution runtime
│       ├── multi_language/     # NEW: Multi-language support
│       └── communication/      # NEW: Inter-node communication
└── foundation/                 # NEW: Core primitives
    ├── data_types/            # NodeValue and data exchange
    ├── interfaces/            # Standard node interfaces
    └── utilities/             # Core utilities
```

### 2. Node Library (Nodified Components)

```
nodes/
├── input/
│   ├── filesystem_scanner/     # File system scanning
│   ├── git_scanner/           # Git repository scanning
│   └── network_scanner/       # Network resource scanning
├── processing/
│   ├── cpp_analyzer/          # C++ code analysis
│   ├── rule_executor/         # Rule execution
│   ├── validator/             # Validation logic
│   └── transformer/           # Data transformation
├── output/
│   ├── formatter/             # Output formatting
│   ├── reporter/              # Report generation
│   └── notifier/              # Notification sending
└── external/
    ├── python/                # Python-based nodes
    ├── javascript/            # JavaScript-based nodes
    ├── rust/                  # Rust-based nodes
    └── java/                  # Java-based nodes
```

### 3. Workflow Definitions

```
workflows/
├── validation/
│   ├── cpp_validation.yaml
│   ├── universal_validation.yaml
│   └── custom_rules.yaml
├── analysis/
│   ├── code_metrics.yaml
│   ├── dependency_analysis.yaml
│   └── security_scan.yaml
└── automation/
    ├── ci_pipeline.yaml
    ├── deployment.yaml
    └── testing.yaml
```

## 🔄 EXECUTION FLOW

### 1. Workflow Loading
```
YAML File → Workflow Parser → Dependency Resolution → Node Registry Lookup → Execution Plan
```

### 2. Node Execution
```
Input Data → Node Validation → Configuration → Execution → Output Collection → Next Node
```

### 3. Multi-language Node Support
```cpp
class MultiLanguageNodeExecutor {
public:
    ExecutionResult executeNode(const INode& node, const NodeContext& context) {
        switch (node.getLanguage()) {
            case NodeLanguage::CPP:
                return executeCppNode(node, context);
            case NodeLanguage::PYTHON:
                return executePythonNode(node, context);
            case NodeLanguage::JAVASCRIPT:
                return executeJavaScriptNode(node, context);
            case NodeLanguage::RUST:
                return executeRustNode(node, context);
            case NodeLanguage::JAVA:
                return executeJavaNode(node, context);
        }
    }
    
private:
    ExecutionResult executePythonNode(const INode& node, const NodeContext& context) {
        // Execute Python script via subprocess
        // Serialize input data as JSON
        // Parse output as JSON
        // Return ExecutionResult
    }
    
    ExecutionResult executeJavaScriptNode(const INode& node, const NodeContext& context) {
        // Execute via Node.js runtime
    }
    
    // ... other language executors
};
```

## 📊 IMPLEMENTATION PHASES

### Phase 1: Foundation (2-3 weeks)
- [ ] Design and implement core node interfaces
- [ ] Create NodeValue data exchange system
- [ ] Implement basic workflow parser
- [ ] Create node registry system

### Phase 2: Core Nodes (3-4 weeks)
- [ ] Convert existing components to nodes
- [ ] Implement C++ node execution runtime
- [ ] Create basic workflow orchestrator
- [ ] Test with simple workflows

### Phase 3: Multi-language Support (4-5 weeks)
- [ ] Implement Python node support
- [ ] Add JavaScript node support
- [ ] Create inter-process communication
- [ ] Add serialization/deserialization

### Phase 4: Advanced Features (3-4 weeks)
- [ ] Parallel execution support
- [ ] Error handling and retry mechanisms
- [ ] Performance monitoring
- [ ] Workflow debugging tools

### Phase 5: Full System Integration (2-3 weeks)
- [ ] Convert all existing workflows to YAML
- [ ] Performance optimization
- [ ] Documentation and examples
- [ ] Production testing

## 🛠️ REQUIRED TECHNOLOGIES

### Core Technologies:
- **C++17/20** - Core system implementation
- **YAML-cpp** - Workflow definition parsing
- **JSON** - Inter-node data exchange
- **Process management** - Multi-language support
- **Threading** - Parallel execution

### Multi-language Support:
- **Python 3.8+** - Python node execution
- **Node.js 16+** - JavaScript node execution
- **Rust 1.60+** - Rust node compilation/execution
- **OpenJDK 11+** - Java node execution

### Communication:
- **Named pipes** - Inter-process communication
- **Shared memory** - High-performance data exchange
- **JSON-RPC** - Standardized node communication

## 🎯 SUCCESS METRICS

### Performance Targets:
- **Workflow startup time**: < 100ms
- **Node execution overhead**: < 10ms per node
- **Memory usage**: < 50MB for orchestrator
- **Concurrent workflows**: Support 10+ parallel workflows

### Functionality Targets:
- **Multi-language support**: C++, Python, JavaScript, Rust, Java
- **Node library**: 50+ built-in nodes
- **Workflow examples**: 20+ sample workflows
- **Documentation**: Complete API documentation

## 🚀 FUTURE VISION

### N8N-like Capabilities:
- **Visual workflow editor** (web-based GUI)
- **Real-time workflow monitoring**
- **Workflow versioning and rollback**
- **Distributed execution** across multiple machines
- **Plugin marketplace** for community nodes
- **Enterprise features**: RBAC, audit logs, SLA monitoring

### Advanced Features:
- **AI-powered workflow optimization**
- **Auto-generated workflows** from code analysis
- **Integration with external systems** (REST APIs, databases)
- **Event-driven workflows** with triggers
- **Workflow templates** and libraries

Kiến trúc này sẽ biến Akao thành một **universal workflow orchestration platform** với khả năng mở rộng vô hạn và hỗ trợ đa ngôn ngữ.