# 🏗️ FUTURE DIRECTORY STRUCTURE - CẤU TRÚC THƯ MỤC HỆ THỐNG NODE-BASED

## 📁 TỔNG QUAN CẤU TRÚC

```
akao/                                    # Root project directory
├── 🏛️ core/                            # CORE ENGINE (Cannot be nodified)
│   ├── engine/                         # Core execution engines  
│   │   ├── orchestrator/               # 🎯 NEW: Workflow orchestration
│   │   │   ├── workflow_engine/        # Workflow execution engine
│   │   │   │   ├── v1.hpp/.cpp        # Main workflow executor
│   │   │   │   └── _.yaml             # Component metadata
│   │   │   ├── node_registry/          # Node discovery & management
│   │   │   │   ├── v1.hpp/.cpp        # Node registration system
│   │   │   │   └── _.yaml
│   │   │   ├── execution_context/      # Execution state management
│   │   │   │   ├── v1.hpp/.cpp        # Context and state tracking
│   │   │   │   └── _.yaml
│   │   │   └── _.yaml                  # Orchestrator metadata
│   │   ├── parser/                     # Language parsers (KEEP)
│   │   │   ├── a/                      # .a format parser
│   │   │   │   ├── v1.hpp/.cpp
│   │   │   │   └── _.yaml
│   │   │   ├── yaml/                   # YAML parser  
│   │   │   │   ├── node/v1.hpp/.cpp
│   │   │   │   ├── parser/v1.hpp/.cpp
│   │   │   │   ├── validator/v1.hpp/.cpp
│   │   │   │   └── _.yaml
│   │   │   └── _.yaml
│   │   ├── logic/                      # Logic processing (KEEP)
│   │   │   ├── runtime/                # Pure logic engine
│   │   │   │   ├── v1.hpp/.cpp
│   │   │   │   └── _.yaml
│   │   │   └── _.yaml
│   │   ├── runtime/                    # 🎯 NEW: Node execution runtime
│   │   │   ├── node_runtime/           # Node execution engine
│   │   │   │   ├── executor/           # Node executor implementations
│   │   │   │   │   ├── cpp_executor/   # C++ node executor
│   │   │   │   │   ├── python_executor/ # Python node executor
│   │   │   │   │   ├── js_executor/    # JavaScript node executor
│   │   │   │   │   └── rust_executor/  # Rust node executor
│   │   │   │   ├── v1.hpp/.cpp        # Main runtime interface
│   │   │   │   └── _.yaml
│   │   │   ├── multi_language/         # Multi-language support
│   │   │   │   ├── bridge/             # Language bridge implementations
│   │   │   │   ├── process_manager/    # Process management
│   │   │   │   ├── v1.hpp/.cpp
│   │   │   │   └── _.yaml
│   │   │   ├── communication/          # Inter-node communication
│   │   │   │   ├── ipc/                # Inter-process communication
│   │   │   │   ├── data_exchange/      # Data serialization/deserialization
│   │   │   │   ├── v1.hpp/.cpp
│   │   │   │   └── _.yaml
│   │   │   └── _.yaml
│   │   └── _.yaml
│   ├── foundation/                     # 🎯 NEW: Core primitives
│   │   ├── interfaces/                 # Standard interfaces
│   │   │   ├── node_interface/         # INode definition
│   │   │   │   ├── v1.hpp/.cpp
│   │   │   │   └── _.yaml
│   │   │   ├── workflow_interface/     # Workflow interfaces
│   │   │   │   ├── v1.hpp/.cpp  
│   │   │   │   └── _.yaml
│   │   │   └── _.yaml
│   │   ├── data_types/                 # Core data types
│   │   │   ├── node_value/             # NodeValue implementation
│   │   │   │   ├── v1.hpp/.cpp
│   │   │   │   └── _.yaml
│   │   │   ├── execution_result/       # Execution results
│   │   │   │   ├── v1.hpp/.cpp
│   │   │   │   └── _.yaml
│   │   │   ├── node_metadata/          # Node metadata types
│   │   │   │   ├── v1.hpp/.cpp
│   │   │   │   └── _.yaml
│   │   │   └── _.yaml
│   │   ├── utilities/                  # Core utilities
│   │   │   ├── serialization/          # JSON/YAML serialization
│   │   │   ├── logging/                # Structured logging
│   │   │   ├── error_handling/         # Error management
│   │   │   └── _.yaml
│   │   └── _.yaml
│   └── _.yaml
├── 🧩 nodes/                           # NODE ECOSYSTEM (Nodified components)
│   ├── input/                          # Input data acquisition nodes
│   │   ├── filesystem_scanner/         # File system scanning
│   │   │   ├── cpp/                    # C++ implementation
│   │   │   │   ├── v1.hpp/.cpp
│   │   │   │   └── _.yaml
│   │   │   ├── config/                 # Node configuration schemas
│   │   │   │   └── schema.yaml
│   │   │   ├── examples/               # Usage examples
│   │   │   │   ├── basic_scan.yaml
│   │   │   │   └── recursive_scan.yaml
│   │   │   ├── tests/                  # Node-specific tests
│   │   │   │   └── scanner_test.cpp
│   │   │   └── node.yaml               # Node definition
│   │   ├── git_scanner/               # Git repository scanning
│   │   │   ├── cpp/v1.hpp/.cpp
│   │   │   ├── python/scanner.py      # Alternative Python impl
│   │   │   ├── config/schema.yaml
│   │   │   ├── examples/
│   │   │   ├── tests/
│   │   │   └── node.yaml
│   │   ├── network_scanner/           # Network resource scanning
│   │   │   ├── rust/                  # Rust implementation for performance
│   │   │   │   ├── src/lib.rs
│   │   │   │   └── Cargo.toml
│   │   │   ├── config/schema.yaml
│   │   │   ├── examples/
│   │   │   ├── tests/
│   │   │   └── node.yaml
│   │   ├── database_reader/           # Database input
│   │   └── api_reader/                # REST API input
│   ├── processing/                    # Data processing nodes
│   │   ├── cpp_analyzer/              # C++ code analysis
│   │   │   ├── cpp/v1.hpp/.cpp
│   │   │   ├── config/schema.yaml
│   │   │   ├── examples/
│   │   │   ├── tests/
│   │   │   └── node.yaml
│   │   ├── rule_executor/             # Rule execution engine
│   │   │   ├── cpp/v1.hpp/.cpp
│   │   │   ├── config/schema.yaml
│   │   │   ├── examples/
│   │   │   ├── tests/
│   │   │   └── node.yaml
│   │   ├── validator/                 # General validation
│   │   ├── transformer/               # Data transformation
│   │   ├── analyzer/                  # Generic analysis
│   │   ├── filter/                    # Data filtering
│   │   ├── aggregator/                # Data aggregation
│   │   └── security_scanner/          # Security analysis
│   ├── output/                        # Output generation nodes
│   │   ├── formatter/                 # Data formatting
│   │   │   ├── cpp/v1.hpp/.cpp
│   │   │   ├── javascript/            # JS template engine
│   │   │   │   ├── formatter.js
│   │   │   │   └── package.json
│   │   │   ├── config/schema.yaml
│   │   │   ├── templates/             # Output templates
│   │   │   │   ├── json.tmpl
│   │   │   │   ├── html.tmpl
│   │   │   │   └── markdown.tmpl
│   │   │   ├── examples/
│   │   │   ├── tests/
│   │   │   └── node.yaml
│   │   ├── reporter/                  # Report generation
│   │   │   ├── python/                # Python reporting with rich libraries
│   │   │   │   ├── reporter.py
│   │   │   │   └── requirements.txt
│   │   │   ├── config/schema.yaml
│   │   │   ├── examples/
│   │   │   ├── tests/
│   │   │   └── node.yaml
│   │   ├── notifier/                  # Notification sending
│   │   ├── file_writer/               # File output
│   │   ├── database_writer/           # Database output
│   │   └── api_sender/                # REST API output
│   ├── utility/                       # Utility nodes
│   │   ├── data_mapper/               # Data mapping/transformation
│   │   ├── cache/                     # Caching nodes
│   │   ├── logger/                    # Logging nodes
│   │   ├── metrics/                   # Metrics collection
│   │   └── debugger/                  # Debugging utilities
│   ├── external/                      # Multi-language node examples
│   │   ├── python/                    # Python nodes
│   │   │   ├── data_science/          # Data analysis nodes
│   │   │   │   ├── pandas_processor/
│   │   │   │   ├── ml_analyzer/
│   │   │   │   └── statistical_analysis/
│   │   │   ├── web_scraping/          # Web scraping nodes
│   │   │   ├── image_processing/      # Image processing
│   │   │   └── ai_integration/        # AI/ML integration
│   │   ├── javascript/                # JavaScript nodes  
│   │   │   ├── frontend_testing/      # Frontend testing
│   │   │   ├── api_testing/           # API testing
│   │   │   ├── package_analysis/      # NPM package analysis
│   │   │   └── browser_automation/    # Browser automation
│   │   ├── rust/                      # Rust nodes
│   │   │   ├── performance_analysis/  # Performance-critical analysis
│   │   │   ├── system_monitoring/     # System monitoring
│   │   │   └── crypto_operations/     # Cryptographic operations
│   │   ├── java/                      # Java nodes
│   │   │   ├── enterprise_integration/ # Enterprise system integration
│   │   │   ├── spring_analysis/       # Spring framework analysis
│   │   │   └── maven_processor/       # Maven project processing
│   │   └── go/                        # Go nodes
│   │       ├── microservice_analysis/ # Microservice analysis
│   │       ├── docker_integration/    # Docker integration
│   │       └── kubernetes_deployment/ # Kubernetes deployment
│   └── _.yaml
├── 📋 workflows/                       # WORKFLOW DEFINITIONS
│   ├── validation/                     # Validation workflows
│   │   ├── cpp_validation.yaml         # C++ code validation
│   │   ├── universal_validation.yaml   # Universal validation
│   │   ├── custom_rules.yaml          # Custom rule validation
│   │   ├── security_validation.yaml   # Security validation
│   │   └── performance_validation.yaml # Performance validation
│   ├── analysis/                      # Analysis workflows
│   │   ├── code_metrics.yaml          # Code metrics analysis
│   │   ├── dependency_analysis.yaml   # Dependency analysis
│   │   ├── architecture_analysis.yaml # Architecture analysis
│   │   ├── technical_debt.yaml        # Technical debt analysis
│   │   └── quality_assessment.yaml    # Quality assessment
│   ├── automation/                    # Automation workflows
│   │   ├── ci_pipeline.yaml           # CI/CD pipeline
│   │   ├── deployment.yaml            # Deployment automation
│   │   ├── testing.yaml               # Automated testing
│   │   ├── documentation.yaml         # Documentation generation
│   │   └── release_management.yaml    # Release management
│   ├── integration/                   # Integration workflows
│   │   ├── github_integration.yaml    # GitHub integration
│   │   ├── jira_integration.yaml      # JIRA integration
│   │   ├── slack_notifications.yaml   # Slack notifications
│   │   └── email_reporting.yaml       # Email reporting
│   ├── examples/                      # Example workflows
│   │   ├── hello_world.yaml           # Simple example
│   │   ├── data_pipeline.yaml         # Data processing pipeline
│   │   ├── multi_language.yaml        # Multi-language processing
│   │   └── parallel_execution.yaml    # Parallel execution example
│   └── templates/                     # Workflow templates
│       ├── basic_validation.template.yaml
│       ├── analysis_pipeline.template.yaml
│       └── ci_cd_pipeline.template.yaml
├── 🧪 tests/                          # TESTING FRAMEWORK
│   ├── unit/                          # Unit tests
│   │   ├── core/                      # Core engine tests
│   │   ├── nodes/                     # Node tests
│   │   └── workflows/                 # Workflow tests
│   ├── integration/                   # Integration tests
│   │   ├── workflow_execution/        # Workflow execution tests
│   │   ├── multi_language/            # Multi-language tests
│   │   └── performance/               # Performance tests
│   ├── e2e/                          # End-to-end tests
│   │   ├── complete_workflows/        # Complete workflow tests
│   │   └── user_scenarios/            # User scenario tests
│   └── benchmarks/                    # Performance benchmarks
├── 📚 docs/                           # DOCUMENTATION
│   ├── architecture/                  # Architecture documentation
│   │   ├── overview.md
│   │   ├── node_system.md
│   │   ├── multi_language.md
│   │   └── performance.md
│   ├── user_guide/                    # User documentation
│   │   ├── getting_started.md
│   │   ├── creating_workflows.md
│   │   ├── custom_nodes.md
│   │   └── troubleshooting.md
│   ├── developer_guide/               # Developer documentation
│   │   ├── contributing.md
│   │   ├── node_development.md
│   │   ├── api_reference.md
│   │   └── testing.md
│   ├── examples/                      # Example documentation
│   └── api/                          # API documentation
├── 🔧 tools/                          # DEVELOPMENT TOOLS
│   ├── workflow_editor/               # Visual workflow editor (future)
│   ├── node_generator/                # Node scaffolding tool
│   ├── performance_profiler/          # Performance profiling
│   ├── workflow_validator/            # Workflow validation
│   └── migration_tools/               # Migration utilities
├── 📦 packages/                       # DISTRIBUTION PACKAGES
│   ├── core/                          # Core engine package
│   ├── standard_nodes/                # Standard node library
│   ├── python_nodes/                  # Python node package
│   ├── javascript_nodes/              # JavaScript node package
│   └── examples/                      # Example packages
├── 🌐 web/                           # WEB INTERFACE (future)
│   ├── workflow_editor/               # Web-based workflow editor
│   ├── monitoring_dashboard/          # Monitoring dashboard
│   └── node_marketplace/              # Node marketplace
├── 🗃️ data/                          # DATA AND CONFIGURATION
│   ├── templates/                     # System templates
│   ├── schemas/                       # JSON schemas for validation
│   └── sample_data/                   # Sample data for testing
├── .akao/                             # PROJECT CONFIGURATION
│   ├── settings.yaml                  # System settings
│   ├── rules/                         # Rules directory
│   │   ├── enabled/
│   │   ├── disabled/
│   │   └── examples/
│   └── cache/                         # System cache
├── main.cpp                           # 🎯 UPDATED: Simplified main (workflow dispatcher)
├── Makefile                           # Build system
├── CMakeLists.txt                     # Alternative build system
├── package.json                       # Node.js dependencies (for JS nodes)
├── requirements.txt                   # Python dependencies (for Python nodes)
├── Cargo.toml                         # Rust dependencies (for Rust nodes)
├── README.md                          # Project documentation
├── CHANGELOG.md                       # Change log
├── LICENSE                            # License information
├── ARCHITECTURE_NODEIFIED_PLAN.md     # Architecture plan
├── CORE_COMPONENTS_ANALYSIS.md        # Core components analysis
└── FUTURE_DIRECTORY_STRUCTURE.md      # This document
```

## 🔍 CẤU TRÚC CHI TIẾT CÁC THÀNH PHẦN QUAN TRỌNG

### 🏛️ Core Engine Structure
```cpp
// core/foundation/interfaces/node_interface/v1.hpp
class INode {
    virtual std::string getNodeId() const = 0;
    virtual ExecutionResult execute(const NodeContext& context) = 0;
    // ... full interface
};

// core/engine/orchestrator/workflow_engine/v1.hpp  
class WorkflowEngine {
    void executeWorkflow(const std::string& workflow_path);
    void executeNode(const std::string& node_id, const NodeContext& context);
    // ... orchestration logic
};
```

### 🧩 Node Package Structure
```yaml
# nodes/processing/cpp_analyzer/node.yaml
node:
  id: "cpp.analyzer.v1"
  name: "C++ Code Analyzer"
  version: "1.0.0"
  description: "Analyzes C++ code for quality metrics"
  
implementation:
  primary: "cpp"
  alternatives: ["python", "rust"]
  
interfaces:
  inputs:
    - name: "source_files"
      type: "array<file_reference>"
      description: "C++ source files to analyze"
    - name: "config"
      type: "object"
      description: "Analysis configuration"
      
  outputs:
    - name: "metrics"
      type: "object"
      description: "Code quality metrics"
    - name: "violations" 
      type: "array<object>"
      description: "Rule violations found"

configuration:
  schema_file: "config/schema.yaml"
  examples_dir: "examples/"
  
dependencies:
  system: ["clang", "libclang"]
  language_specific:
    cpp: []
    python: ["clang-python", "pyyaml"]
```

### 📋 Workflow Definition Structure
```yaml
# workflows/validation/cpp_validation.yaml
workflow:
  id: "cpp_validation_v1"
  name: "C++ Code Validation Pipeline"
  version: "1.0.0"
  
parameters:
  - name: "source_path"
    type: "string"
    required: true
    description: "Path to C++ source code"
  - name: "output_format"
    type: "string"
    default: "json"
    options: ["json", "yaml", "xml", "html"]

nodes:
  - id: "scanner"
    type: "input.filesystem_scanner"
    config:
      path: "${parameters.source_path}"
      patterns: ["*.cpp", "*.hpp", "*.h"]
      recursive: true
      
  - id: "cpp_analyzer"
    type: "processing.cpp_analyzer"
    config:
      rules: ["one_class_per_file", "modern_cpp"]
      metrics: ["complexity", "maintainability"]
      
  - id: "rule_engine"
    type: "processing.rule_executor"
    config:
      rules_directory: ".akao/rules/enabled"
      engine: "pure_logic"
      
  - id: "formatter"
    type: "output.formatter"
    config:
      format: "${parameters.output_format}"
      template: "validation_report"

connections:
  - from: "scanner.files"
    to: "cpp_analyzer.source_files"
  - from: "scanner.files" 
    to: "rule_engine.files"
  - from: "cpp_analyzer.results"
    to: "formatter.cpp_analysis"
  - from: "rule_engine.results"
    to: "formatter.rule_results"

execution:
  mode: "sequential"
  timeout: 300
  error_handling: "continue_on_error"
```

## 🚀 LỢI ÍCH CỦA CẤU TRÚC MỚI

### 1. **Separation of Concerns**
- **Core**: Stable, high-performance, minimal changes
- **Nodes**: Flexible, extensible, user-configurable
- **Workflows**: Business logic externalized

### 2. **Multi-language Support**
- Each node can be implemented in optimal language
- Easy integration of external tools and libraries
- Community contributions in any language

### 3. **Scalability**
- Parallel node execution
- Distributed workflow execution
- Plugin-based architecture

### 4. **Maintainability**
- Clear module boundaries
- Independent versioning for nodes
- Easy testing and debugging

### 5. **Extensibility**
- Node marketplace potential
- Custom workflow templates
- Integration with external systems

## 🔄 MIGRATION STRATEGY

### Phase 1: Foundation
```
Create core/ directory structure
Implement basic node interfaces
Create simple node examples
```

### Phase 2: Core Nodes
```
Convert existing components to nodes/
Implement C++ node execution
Create basic workflows/
```

### Phase 3: Multi-language
```
Add Python/JavaScript/Rust support
Create external/ node examples
Implement inter-process communication
```

### Phase 4: Advanced Features
```
Add workflow editor tools/
Implement web interface
Create node marketplace
```

Cấu trúc này tạo ra một **ecosystem hoàn chỉnh** cho node-based workflow system với khả năng mở rộng vô hạn và hỗ trợ đa ngôn ngữ.