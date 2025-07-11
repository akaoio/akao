# 📡 YAML-RPC PROTOCOL SPECIFICATION v1.0

**⚠️ PROVISIONAL**: Protocol design for core-node independence

## Protocol Overview

### Purpose
Replace direct C++ interface calls with standardized YAML-based RPC communication to achieve complete core-node separation.

### Transport
- **Medium**: Unix Domain Sockets
- **Format**: YAML messages
- **Pattern**: Request-Response
- **Connection**: Persistent per node process

## Message Format

### Request Structure
```yaml
yamlrpc: "1.0"
method: "node.execute"
params:
  input:
    # Input data (equivalent to current NodeParameters)
    key: "value"
    data: {...}
  context:
    # Execution context (equivalent to current NodeContext)  
    node_id: "file-scanner"
    workflow_id: "validation-workflow"
    execution_id: "exec-12345"
    timestamp: "2025-07-11T23:15:30Z"
  config:
    # Node-specific configuration
    timeout: 30
    mode: "production"
id: "req-789"
```

### Response Structure
```yaml
yamlrpc: "1.0"
result:
  output:
    # Output data (equivalent to current ExecutionResult)
    data: {...}
    files: [...]
    statistics: {...}
  metadata:
    # Execution metadata
    execution_time_ms: 1234
    memory_used_mb: 45
    status: "success"
    node_version: "1.0.0"
id: "req-789"
```

### Error Structure
```yaml
yamlrpc: "1.0"
error:
  code: -32603
  message: "Internal error"
  data:
    details: "Node execution failed"
    error_type: "validation_error"
    stack_trace: "..."
id: "req-789"
```

## Standard Methods

### 1. node.info
**Purpose**: Get node metadata and capabilities
```yaml
# Request
yamlrpc: "1.0"
method: "node.info"
params: {}
id: "info-123"

# Response
yamlrpc: "1.0"
result:
  node_id: "akao:node:filesystem:scanner:v1"
  name: "Filesystem Scanner"
  version: "1.0.0"
  description: "Scans and analyzes filesystem structures"
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
id: "info-123"
```

### 2. node.validate
**Purpose**: Validate input parameters before execution
```yaml
# Request
yamlrpc: "1.0"
method: "node.validate"
params:
  input:
    path: "/home/user/project"
    recursive: true
id: "validate-456"

# Response
yamlrpc: "1.0"
result:
  valid: true
  errors: []
  warnings: 
    - "Large directory, execution may take time"
id: "validate-456"
```

### 3. node.execute
**Purpose**: Execute node with given parameters
```yaml
# Request
yamlrpc: "1.0"
method: "node.execute"
params:
  input:
    path: "/home/user/project"
    recursive: true
    extensions: [".cpp", ".hpp"]
  context:
    node_id: "file-scanner"
    workflow_id: "cpp-validation"
    execution_id: "exec-789"
  config:
    timeout: 60
    max_files: 10000
id: "exec-789"

# Response
yamlrpc: "1.0"
result:
  output:
    total_files: 245
    files:
      - path: "src/main.cpp"
        size: 1024
        modified: "2025-07-11T10:30:00Z"
      - path: "include/header.hpp"
        size: 512
        modified: "2025-07-11T09:15:00Z"
    statistics:
      cpp_files: 180
      hpp_files: 65
      total_size_mb: 12.5
  metadata:
    execution_time_ms: 2340
    memory_used_mb: 25
    status: "success"
id: "exec-789"
```

### 4. node.health
**Purpose**: Check node health and status
```yaml
# Request
yamlrpc: "1.0"
method: "node.health"
params: {}
id: "health-999"

# Response
yamlrpc: "1.0"
result:
  status: "healthy"
  uptime_seconds: 3600
  memory_usage_mb: 12
  cpu_usage_percent: 2.5
  requests_processed: 45
  last_activity: "2025-07-11T23:10:00Z"
id: "health-999"
```

### 5. node.shutdown
**Purpose**: Graceful node shutdown
```yaml
# Request
yamlrpc: "1.0"
method: "node.shutdown"
params:
  timeout_seconds: 10
id: "shutdown-000"

# Response
yamlrpc: "1.0"
result:
  status: "shutting_down"
  estimated_shutdown_time: 5
id: "shutdown-000"
```

## Error Codes

### Standard JSON-RPC Errors
- `-32700`: Parse error
- `-32600`: Invalid Request
- `-32601`: Method not found
- `-32602`: Invalid params
- `-32603`: Internal error

### Node-Specific Errors
- `-1000`: Node initialization error
- `-1001`: Configuration error
- `-1002`: Input validation error
- `-1003`: Execution timeout
- `-1004`: Resource limit exceeded
- `-1005`: Node internal error

## Data Type Mapping

### Current Core Types → YAML-RPC
```yaml
# NodeValue → YAML native types
string: "text value"
integer: 123
float: 45.67
boolean: true
array: [1, 2, 3]
object:
  key: "value"
  nested:
    data: "example"

# ExecutionResult → result object
result:
  output: {...}          # Main output data
  metadata: {...}        # Execution metadata
  
# ValidationResult → validate response
result:
  valid: true/false
  errors: [...]          # Validation errors
  warnings: [...]        # Validation warnings
```

## Transport Layer

### Socket Configuration
```yaml
# Core manages sockets
socket_path: "/tmp/akao-node-{node-id}.sock"
connection_type: "unix_domain_socket"
mode: "stream"
permissions: "0600"  # Owner read/write only
```

### Connection Management
```yaml
# Node startup
1. Node creates socket at specified path
2. Node listens for connections
3. Core connects when node needed
4. Persistent connection maintained
5. Reconnection on failure

# Communication pattern
Core → Node: Request (YAML)
Node → Core: Response (YAML)
```

### Message Framing
```yaml
# Each message prefixed with length
message_format: |
  [4-byte length][YAML content]
  
# Example
length: 156
content: |
  yamlrpc: "1.0"
  method: "node.execute"
  params: {...}
  id: "req-123"
```

## Node Implementation Template

### Node Server Structure
```cpp
// Node completely independent - NO core includes
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>

class YamlRpcServer {
public:
    void start(const std::string& socket_path);
    void handle_request(const std::string& yaml_request);
    std::string process_info();
    std::string process_validate(const std::string& input);
    std::string process_execute(const std::string& input, const std::string& context);
    std::string process_health();
    void shutdown();
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <socket_path>" << std::endl;
        return 1;
    }
    
    YamlRpcServer server;
    server.start(argv[1]);  // e.g., "/tmp/akao-node-file.sock"
    return 0;
}
```

### Node Manifest (`_.yaml`)
```yaml
id: "akao:node:filesystem:scanner:v1"
name: "Filesystem Scanner"
version: "1.0.0"
description: "Scans and analyzes filesystem structures"

runtime:
  type: "executable"
  command: "./file-scanner"
  args: ["/tmp/akao-node-file.sock"]

communication:
  protocol: "yamlrpc"
  version: "1.0"
  methods: ["info", "validate", "execute", "health", "shutdown"]

inputs:
  - name: "path"
    type: "string"
    required: true
    description: "Directory path to scan"
  - name: "recursive"
    type: "boolean"
    default: false
    description: "Scan subdirectories recursively"
  - name: "extensions"
    type: "array"
    description: "File extensions to include"

outputs:
  - name: "files"
    type: "array"
    description: "List of discovered files"
  - name: "statistics"
    type: "object"
    description: "Scan statistics and metrics"

resources:
  memory_mb: 128
  cpu_percent: 10
  timeout_seconds: 60
```

## Core Implementation

### Communication Manager
```cpp
// core/engine/communication/yamlrpc/
class YamlRpcClient {
public:
    bool connect(const std::string& socket_path);
    std::string send_request(const std::string& yaml_request);
    void disconnect();
};

class NodeCommunicator {
public:
    std::string node_info(const std::string& node_id);
    std::string node_validate(const std::string& node_id, const std::string& input);
    std::string node_execute(const std::string& node_id, const std::string& input, const std::string& context);
    std::string node_health(const std::string& node_id);
    void node_shutdown(const std::string& node_id);
};
```

This protocol completely eliminates the need for C++ interface dependencies and enables true core-node independence.