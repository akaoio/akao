# 🚀 AKAO NODE-BASED EVOLUTION PLAN

## 📋 EXECUTION OVERVIEW

This plan transforms Akao from hardcoded workflows to a fully configurable node-based system inspired by N8N but built in C++ with unique Akao capabilities.

### 🎯 **Core Objectives:**
1. **Maintain Compatibility**: All existing functionality must continue working
2. **Incremental Evolution**: Each phase builds on the previous with working tests
3. **Testable Progress**: Every step produces executable artifacts with verifiable results
4. **Performance First**: C++ core with multi-language node support
5. **Philosophy Integration**: .a format and self-validation remain central

---

## 📊 PHASE BREAKDOWN

### **Phase 1: Foundation Infrastructure (Weeks 1-4)**
Build core node infrastructure and data exchange system

### **Phase 2: Node Implementation (Weeks 5-8)** 
Convert existing components to node-based architecture

### **Phase 3: Workflow Engine (Weeks 9-12)**
Implement advanced workflow execution and orchestration

### **Phase 4: Integration & Ecosystem (Weeks 13-16)**
Package management, N8N compatibility, and marketplace integration

---

## 🏗️ PHASE 1: FOUNDATION INFRASTRUCTURE

### **Phase 1.1: Core Data Exchange System**

#### **Step 1.1.1: NodeValue and NodeItem Implementation**
```cpp
// Target: Universal data exchange system
class NodeValue {
    ValueType type_;
    std::variant<std::string, int64_t, double, bool, Object, Array> value_;
public:
    template<typename T> T as() const;
    bool isString() const;
    bool isNumber() const;
    // ... type checking methods
};

class NodeItem {
public:
    NodeValue json;                           // Main data payload
    std::map<std::string, BinaryData> binary; // Binary attachments
    std::map<std::string, NodeValue> metadata; // Item metadata
    std::optional<ExecutionError> error;      // Item-level error
};
```

**Expected Artifacts:**
- `artifacts/evolve/phase1/step1-1-1/nodevalue.hpp`
- `artifacts/evolve/phase1/step1-1-1/nodevalue.cpp`
- `artifacts/evolve/phase1/step1-1-1/test_nodevalue.cpp`

**Test Requirements:**
```bash
# Must compile and pass all tests
cd artifacts/evolve/phase1/step1-1-1
make test_nodevalue
./test_nodevalue
# Expected output: All 20+ type conversion tests PASS
```

#### **Step 1.1.2: NodeContext and ExecutionResult**
```cpp
// Target: Node execution context and result handling
class NodeContext {
    std::vector<NodeItem> input_items_;
    std::map<std::string, NodeValue> parameters_;
    AkaoLogicEngine& logic_engine_;
public:
    std::vector<NodeItem> getInputItems() const;
    NodeValue getParameter(const std::string& name) const;
    NodeValue executeAkaoLogic(const std::string& expression);
};

class ExecutionResult {
public:
    std::vector<NodeItem> items;
    ExecutionStatus status;
    std::vector<ExecutionError> errors;
    void addItem(const NodeValue& data);
    void addError(const std::string& message);
};
```

**Expected Artifacts:**
- `artifacts/evolve/phase1/step1-1-2/nodecontext.hpp`
- `artifacts/evolve/phase1/step1-1-2/nodecontext.cpp`
- `artifacts/evolve/phase1/step1-1-2/test_nodecontext.cpp`

**Test Requirements:**
```bash
# Must demonstrate batch processing and .a format integration
cd artifacts/evolve/phase1/step1-1-2
make test_nodecontext
./test_nodecontext
# Expected: Process 100 items in batch, execute .a format expression, return results
```

### **Phase 1.2: Node Interface System**

#### **Step 1.2.1: INode Interface Definition**
```cpp
// Target: Standard node interface for all implementations
class INode {
public:
    virtual ~INode() = default;
    virtual std::string getNodeId() const = 0;
    virtual std::string getNodeType() const = 0;
    virtual NodeDefinition getDefinition() const = 0;
    virtual ExecutionResult execute(const NodeContext& context) = 0;
    virtual ValidationResult validate(const std::map<std::string, NodeValue>& parameters) = 0;
};
```

**Expected Artifacts:**
- `artifacts/evolve/phase1/step1-2-1/inode.hpp`
- `artifacts/evolve/phase1/step1-2-1/node_definition.hpp`
- `artifacts/evolve/phase1/step1-2-1/test_inode_interface.cpp`

**Test Requirements:**
```bash
# Must compile interface and test with mock implementation
cd artifacts/evolve/phase1/step1-2-1
make test_inode_interface
./test_inode_interface
# Expected: Mock node executes successfully, validates parameters correctly
```

#### **Step 1.2.2: NodeParameter System**
```cpp
// Target: Rich parameter system inspired by N8N
enum class ParameterType {
    STRING, NUMBER, BOOLEAN, OPTIONS, COLLECTION,
    AKAO_EXPRESSION, MULTI_LANGUAGE_CODE, FILE_PATH
};

class NodeParameter {
public:
    std::string name, display_name, description;
    ParameterType type;
    NodeValue default_value;
    bool required;
    std::vector<ParameterOption> options;
    std::string akao_validation_rule;
};
```

**Expected Artifacts:**
- `artifacts/evolve/phase1/step1-2-2/node_parameter.hpp`
- `artifacts/evolve/phase1/step1-2-2/node_parameter.cpp`
- `artifacts/evolve/phase1/step1-2-2/test_parameters.cpp`

**Test Requirements:**
```bash
# Must validate parameters and handle all types
cd artifacts/evolve/phase1/step1-2-2
make test_parameters
./test_parameters
# Expected: All parameter types validate correctly, .a format validation works
```

### **Phase 1.3: Node Registry System**

#### **Step 1.3.1: NodeRegistry Implementation**
```cpp
// Target: Central registry for all available nodes
class NodeRegistry {
    std::map<std::string, std::unique_ptr<INode>> nodes_;
    std::map<std::string, NodeDefinition> definitions_;
public:
    bool registerNode(std::unique_ptr<INode> node);
    bool hasNode(const std::string& node_type) const;
    INode* getNode(const std::string& node_type) const;
    std::vector<NodeDefinition> getAvailableNodes() const;
};
```

**Expected Artifacts:**
- `artifacts/evolve/phase1/step1-3-1/node_registry.hpp`
- `artifacts/evolve/phase1/step1-3-1/node_registry.cpp`
- `artifacts/evolve/phase1/step1-3-1/test_registry.cpp`

**Test Requirements:**
```bash
# Must register, discover, and retrieve nodes
cd artifacts/evolve/phase1/step1-3-1
make test_registry
./test_registry
# Expected: Register 10 mock nodes, retrieve all, verify definitions match
```

#### **Step 1.3.2: Built-in Core Nodes**
```cpp
// Target: Essential system nodes that cannot be removed
namespace builtin {
    class LoggerNode : public INode;      // system.logger.v1
    class DataMapperNode : public INode;  // data.mapper.v1  
    class ConditionalNode : public INode; // control.conditional.v1
    class AkaoLogicNode : public INode;   // akao.logic_executor.v1
    class FileReaderNode : public INode;  // io.file_reader.v1
}
```

**Expected Artifacts:**
- `artifacts/evolve/phase1/step1-3-2/builtin_nodes.hpp`
- `artifacts/evolve/phase1/step1-3-2/builtin_nodes.cpp`
- `artifacts/evolve/phase1/step1-3-2/test_builtin_nodes.cpp`

**Test Requirements:**
```bash
# Must implement and test all 5 core nodes
cd artifacts/evolve/phase1/step1-3-2
make test_builtin_nodes
./test_builtin_nodes
# Expected: All core nodes execute, process data, log correctly
```

---

## 🔧 PHASE 2: NODE IMPLEMENTATION

### **Phase 2.1: Component Conversion**

#### **Step 2.1.1: Convert Existing Validator to Node**
```cpp
// Target: Transform core/engine/validator into ValidatorNode
class ValidatorNode : public INode {
public:
    std::string getNodeId() const override { return "akao.validator.v1"; }
    ExecutionResult execute(const NodeContext& context) override;
    // Convert existing validator logic to node format
};
```

**Expected Artifacts:**
- `artifacts/evolve/phase2/step2-1-1/validator_node.hpp`
- `artifacts/evolve/phase2/step2-1-1/validator_node.cpp`
- `artifacts/evolve/phase2/step2-1-1/test_validator_node.cpp`

**Test Requirements:**
```bash
# Must validate same .akao rules as current system
cd artifacts/evolve/phase2/step2-1-1
make test_validator_node
./test_validator_node
# Expected: Validate current .akao project, return same results as existing validator
```

#### **Step 2.1.2: Convert YAML Parser to Node**
```cpp
// Target: Transform core/engine/parser/yaml into YamlParserNode
class YamlParserNode : public INode {
public:
    std::string getNodeId() const override { return "parser.yaml.v1"; }
    ExecutionResult execute(const NodeContext& context) override;
    // Parse YAML using existing parser, return as NodeValue
};
```

**Expected Artifacts:**
- `artifacts/evolve/phase2/step2-1-2/yaml_parser_node.hpp`
- `artifacts/evolve/phase2/step2-1-2/yaml_parser_node.cpp`
- `artifacts/evolve/phase2/step2-1-2/test_yaml_parser_node.cpp`

**Test Requirements:**
```bash
# Must parse YAML files correctly
cd artifacts/evolve/phase2/step2-1-2
make test_yaml_parser_node
./test_yaml_parser_node
# Expected: Parse complex YAML, return structured NodeValue, handle errors
```

#### **Step 2.1.3: Convert Akao Parser to Node**
```cpp
// Target: Transform core/engine/parser/a into AkaoParserNode
class AkaoParserNode : public INode {
public:
    std::string getNodeId() const override { return "parser.akao.v1"; }
    ExecutionResult execute(const NodeContext& context) override;
    // Parse .a format using existing parser
};
```

**Expected Artifacts:**
- `artifacts/evolve/phase2/step2-1-3/akao_parser_node.hpp`
- `artifacts/evolve/phase2/step2-1-3/akao_parser_node.cpp`
- `artifacts/evolve/phase2/step2-1-3/test_akao_parser_node.cpp`

**Test Requirements:**
```bash
# Must parse .a format correctly
cd artifacts/evolve/phase2/step2-1-3
make test_akao_parser_node
./test_akao_parser_node
# Expected: Parse .a expressions, return AST as NodeValue, validate syntax
```

### **Phase 2.2: Filesystem and Analysis Nodes**

#### **Step 2.2.1: Filesystem Scanner Node**
```cpp
// Target: Scan filesystem for files matching patterns
class FilesystemScannerNode : public INode {
public:
    std::string getNodeId() const override { return "filesystem.scanner.v1"; }
    ExecutionResult execute(const NodeContext& context) override;
    // Scan directories, return file lists as NodeItems
};
```

**Expected Artifacts:**
- `artifacts/evolve/phase2/step2-2-1/filesystem_scanner_node.hpp`
- `artifacts/evolve/phase2/step2-2-1/filesystem_scanner_node.cpp`
- `artifacts/evolve/phase2/step2-2-1/test_filesystem_scanner.cpp`

**Test Requirements:**
```bash
# Must scan current project and find all files
cd artifacts/evolve/phase2/step2-2-1
make test_filesystem_scanner
./test_filesystem_scanner
# Expected: Find all .cpp/.hpp files in akao project, return as NodeItems
```

#### **Step 2.2.2: C++ Analyzer Node**
```cpp
// Target: Analyze C++ code for quality metrics
class CppAnalyzerNode : public INode {
public:
    std::string getNodeId() const override { return "analysis.cpp.v1"; }
    ExecutionResult execute(const NodeContext& context) override;
    // Analyze C++ files for classes, functions, complexity
};
```

**Expected Artifacts:**
- `artifacts/evolve/phase2/step2-2-2/cpp_analyzer_node.hpp`
- `artifacts/evolve/phase2/step2-2-2/cpp_analyzer_node.cpp`
- `artifacts/evolve/phase2/step2-2-2/test_cpp_analyzer.cpp`

**Test Requirements:**
```bash
# Must analyze real C++ files and return metrics
cd artifacts/evolve/phase2/step2-2-2
make test_cpp_analyzer
./test_cpp_analyzer
# Expected: Analyze main.cpp, count classes=0, functions>1, return detailed metrics
```

---

## ⚡ PHASE 3: WORKFLOW ENGINE

### **Phase 3.1: Workflow Definition System**

#### **Step 3.1.1: Workflow Definition Parser**
```yaml
# Target: Parse YAML workflow definitions
workflow:
  id: "sample_analysis"
  nodes:
    - id: "scanner"
      type: "filesystem.scanner.v1"
      config:
        pattern: "**/*.cpp"
    - id: "analyzer"  
      type: "analysis.cpp.v1"
  connections:
    - from: "scanner.output"
      to: "analyzer.input"
```

**Expected Artifacts:**
- `artifacts/evolve/phase3/step3-1-1/workflow_definition.hpp`
- `artifacts/evolve/phase3/step3-1-1/workflow_parser.cpp`
- `artifacts/evolve/phase3/step3-1-1/test_workflow_parser.cpp`

**Test Requirements:**
```bash
# Must parse workflow YAML and create execution plan
cd artifacts/evolve/phase3/step3-1-1
make test_workflow_parser
./test_workflow_parser
# Expected: Parse sample workflow, validate nodes exist, create execution graph
```

#### **Step 3.1.2: Workflow Execution Engine**
```cpp
// Target: Execute workflows with proper data flow
class WorkflowEngine {
public:
    ExecutionResult executeWorkflow(const WorkflowDefinition& workflow);
    ExecutionResult executeFromNode(const WorkflowDefinition& workflow, 
                                    const std::string& start_node);
private:
    void executeNode(const NodeInstance& node, const NodeContext& context);
    std::vector<NodeItem> getNodeInput(const NodeInstance& node);
};
```

**Expected Artifacts:**
- `artifacts/evolve/phase3/step3-1-2/workflow_engine.hpp`
- `artifacts/evolve/phase3/step3-1-2/workflow_engine.cpp`
- `artifacts/evolve/phase3/step3-1-2/test_workflow_execution.cpp`

**Test Requirements:**
```bash
# Must execute real workflow end-to-end
cd artifacts/evolve/phase3/step3-1-2
make test_workflow_execution
./test_workflow_execution
# Expected: Execute scanner->analyzer workflow, return final analysis results
```

### **Phase 3.2: Advanced Execution Features**

#### **Step 3.2.1: Conditional Execution with .a Format**
```yaml
# Target: Support conditional workflow paths
nodes:
  - id: "conditional"
    type: "control.conditional.v1"
    config:
      condition: |
        exists $file in input.files:
          cpp.count_classes(filesystem.read_file($file)) > 1
      then_path: "fix_violations"
      else_path: "generate_report"
```

**Expected Artifacts:**
- `artifacts/evolve/phase3/step3-2-1/conditional_execution.hpp`
- `artifacts/evolve/phase3/step3-2-1/conditional_execution.cpp`
- `artifacts/evolve/phase3/step3-2-1/test_conditional_workflow.cpp`

**Test Requirements:**
```bash
# Must execute conditional logic using .a format
cd artifacts/evolve/phase3/step3-2-1
make test_conditional_workflow
./test_conditional_workflow
# Expected: Evaluate .a format condition, route to correct path based on result
```

#### **Step 3.2.2: Parallel Execution Support**
```cpp
// Target: Execute independent nodes in parallel
class ParallelExecutor {
public:
    ExecutionResult executeParallel(const std::vector<NodeInstance>& nodes,
                                    const NodeContext& shared_context);
private:
    std::vector<std::future<ExecutionResult>> node_futures_;
};
```

**Expected Artifacts:**
- `artifacts/evolve/phase3/step3-2-2/parallel_executor.hpp`
- `artifacts/evolve/phase3/step3-2-2/parallel_executor.cpp`
- `artifacts/evolve/phase3/step3-2-2/test_parallel_execution.cpp`

**Test Requirements:**
```bash
# Must execute multiple nodes concurrently
cd artifacts/evolve/phase3/step3-2-2
make test_parallel_execution
./test_parallel_execution
# Expected: Execute 4 independent analysis nodes in parallel, complete faster than sequential
```

---

## 🌐 PHASE 4: INTEGRATION & ECOSYSTEM

### **Phase 4.1: Package Management System**

#### **Step 4.1.1: Package Definition and Loading**
```yaml
# Target: Package manifest for distributable nodes
package:
  id: "community.advanced_cpp_analyzer.v1"
  display_name: "Advanced C++ Analyzer"
  version: "1.0.0"
  dependencies:
    akao_nodes: ["filesystem.scanner.v1"]
    system: ["clang-tools"]
  build:
    commands: ["mkdir build", "cd build && cmake ..", "make"]
    outputs: ["build/lib/libadvanced_analyzer.so"]
```

**Expected Artifacts:**
- `artifacts/evolve/phase4/step4-1-1/package_manifest.hpp`
- `artifacts/evolve/phase4/step4-1-1/package_loader.cpp`
- `artifacts/evolve/phase4/step4-1-1/test_package_loading.cpp`

**Test Requirements:**
```bash
# Must load and install packages correctly
cd artifacts/evolve/phase4/step4-1-1
make test_package_loading
./test_package_loading
# Expected: Load package manifest, validate dependencies, install successfully
```

#### **Step 4.1.2: Node Installation and Registry Integration**
```cpp
// Target: Install nodes and register them automatically
class PackageManager {
public:
    InstallResult installPackage(const std::filesystem::path& package_path);
    bool uninstallPackage(const std::string& package_id);
    std::vector<PackageInfo> listInstalled() const;
    ValidationResult validateDependencies(const PackageManifest& manifest) const;
};
```

**Expected Artifacts:**
- `artifacts/evolve/phase4/step4-1-2/package_manager.hpp`
- `artifacts/evolve/phase4/step4-1-2/package_manager.cpp`
- `artifacts/evolve/phase4/step4-1-2/test_package_manager.cpp`

**Test Requirements:**
```bash
# Must install real package and make it available
cd artifacts/evolve/phase4/step4-1-2
make test_package_manager
./test_package_manager
# Expected: Install test package, verify node appears in registry, execute successfully
```

### **Phase 4.2: Multi-Language Support**

#### **Step 4.2.1: Python Node Runtime**
```cpp
// Target: Execute Python-based nodes
class PythonNodeRuntime {
public:
    ExecutionResult executePythonNode(const std::string& script_path,
                                      const NodeContext& context);
private:
    void initializePython();
    NodeValue convertPythonToNodeValue(PyObject* py_obj);
    PyObject* convertNodeValueToPython(const NodeValue& value);
};
```

**Expected Artifacts:**
- `artifacts/evolve/phase4/step4-2-1/python_runtime.hpp`
- `artifacts/evolve/phase4/step4-2-1/python_runtime.cpp`
- `artifacts/evolve/phase4/step4-2-1/test_python_nodes.cpp`
- `artifacts/evolve/phase4/step4-2-1/sample_python_node.py`

**Test Requirements:**
```bash
# Must execute Python node and return results
cd artifacts/evolve/phase4/step4-2-1
make test_python_nodes
./test_python_nodes
# Expected: Execute Python node, process data, return NodeValue results to C++
```

#### **Step 4.2.2: JavaScript Node Runtime**
```cpp
// Target: Execute JavaScript-based nodes using V8 or Node.js
class JavaScriptNodeRuntime {
public:
    ExecutionResult executeJavaScriptNode(const std::string& script_path,
                                          const NodeContext& context);
private:
    v8::Isolate* isolate_;
    NodeValue convertV8ToNodeValue(v8::Local<v8::Value> v8_value);
    v8::Local<v8::Value> convertNodeValueToV8(const NodeValue& value);
};
```

**Expected Artifacts:**
- `artifacts/evolve/phase4/step4-2-2/javascript_runtime.hpp`
- `artifacts/evolve/phase4/step4-2-2/javascript_runtime.cpp`
- `artifacts/evolve/phase4/step4-2-2/test_javascript_nodes.cpp`
- `artifacts/evolve/phase4/step4-2-2/sample_js_node.js`

**Test Requirements:**
```bash
# Must execute JavaScript node and return results
cd artifacts/evolve/phase4/step4-2-2
make test_javascript_nodes
./test_javascript_nodes
# Expected: Execute JS node, manipulate data, return results to C++ workflow
```

---

## 🧪 INTEGRATION TESTING

### **Final Integration Test: Complete Workflow**
```yaml
# Target: End-to-end workflow with multiple languages and nodes
workflow:
  id: "multi_language_analysis"
  nodes:
    - id: "scanner"
      type: "filesystem.scanner.v1"      # C++ built-in
    - id: "cpp_analyzer" 
      type: "analysis.cpp.v1"            # C++ node
    - id: "python_ml"
      type: "ml.classifier.v1"           # Python node
    - id: "js_formatter"
      type: "format.prettier.v1"         # JavaScript node
    - id: "reporter"
      type: "output.json_report.v1"      # C++ node
```

**Expected Artifacts:**
- `artifacts/evolve/integration/complete_workflow.yaml`
- `artifacts/evolve/integration/test_complete_workflow.cpp`
- `artifacts/evolve/integration/sample_nodes/` (Python, JS, C++ nodes)

**Test Requirements:**
```bash
# Must execute complete multi-language workflow
cd artifacts/evolve/integration
make test_complete_workflow
./test_complete_workflow
# Expected: 
# 1. Scan akao project files
# 2. Analyze C++ code quality
# 3. Run Python ML classification
# 4. Format results with JavaScript
# 5. Generate JSON report
# 6. All steps complete successfully with real data
```

---

## ✅ SUCCESS CRITERIA

### **Each Step Must:**
1. **Compile Successfully**: All C++ code compiles without errors
2. **Pass All Tests**: Executable tests pass with expected results
3. **Generate Real Data**: No fake/mock data in final results
4. **Maintain Performance**: No performance regression vs current system
5. **Preserve Compatibility**: All existing .akao rules still work

### **Each Phase Must:**
1. **Build on Previous**: Use artifacts from previous phases
2. **Demonstrate Progress**: Show measurable improvement in capabilities
3. **Include Documentation**: Brief README explaining artifacts
4. **Provide Examples**: Working examples of new functionality

### **Final Success Metrics:**
- ✅ Execute workflows 2x faster than sequential execution
- ✅ Support 4+ programming languages for nodes
- ✅ Maintain 100% backward compatibility with existing .akao rules
- ✅ Package system can install and manage external nodes
- ✅ Multi-language workflow executes successfully end-to-end

This plan ensures that every step produces concrete, testable results that build toward a fully functional node-based system while maintaining Akao's core philosophy and performance characteristics.