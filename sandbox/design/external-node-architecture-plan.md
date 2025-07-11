# External Node Architecture: Comprehensive Implementation Plan

## Executive Summary

This document outlines a comprehensive plan to transform the Akao workflow orchestration system from a compile-time static node system to a dynamic, sandboxed external node ecosystem. The plan addresses the fundamental architectural limitations while preserving the excellent existing foundation.

## Current System Assessment

### Strengths to Preserve
- **Solid Registry System**: Thread-safe, metadata-rich node registry with callback support
- **Robust Workflow Orchestrator**: Comprehensive YAML-based workflow engine with dependency management
- **Clean Node Interface**: Well-designed INode interface with proper abstraction
- **Flexible Type System**: Universal NodeValue system for data exchange
- **Self-Registration Pattern**: Elegant RAII-based registration for built-in nodes

### Critical Limitations Identified
- **Static Compilation Only**: All nodes must be compiled into main executable
- **No External Node Support**: Missing dynamic loading, package management, and runtime discovery
- **Tight Core Coupling**: Nodes directly depend on core libraries, violating sandboxing principles
- **Primitive CLI**: Missing node management commands (install, uninstall, enable, disable)
- **No Security Framework**: Missing sandboxing, trust verification, and permission system
- **Missing Package Management**: No node packages, dependency resolution, or repository support

## Strategic Architecture Overview

### Design Philosophy
1. **Maintain Existing Foundation**: Preserve working registry and workflow systems
2. **Dual Node Support**: Support both built-in and external nodes equally
3. **Progressive Enhancement**: Enable external nodes without breaking existing functionality
4. **Security First**: Implement robust sandboxing and security from the start
5. **Language Agnostic**: Support any language through process-based execution
6. **Zero Core Dependencies**: External nodes must be completely sandboxed

### High-Level Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                    Akao Workflow Orchestrator                   │
├─────────────────────────────────────────────────────────────────┤
│                      Node Registry                              │
│  ┌─────────────────┐  │  ┌─────────────────────────────────────┐│
│  │   Built-in      │  │  │         External Node               ││
│  │    Nodes        │  │  │         Management                  ││
│  │                 │  │  │                                     ││
│  │ • File Scanner  │  │  │ • Package Manager                   ││
│  │ • YAML Proc     │  │  │ • Dynamic Loader                    ││
│  │ • Logic Exec    │  │  │ • Security Sandbox                  ││
│  │ • Reporter      │  │  │ • Process Manager                   ││
│  └─────────────────┘  │  └─────────────────────────────────────┘│
├─────────────────────────────────────────────────────────────────┤
│                     Communication Layer                         │
│  ┌─────────────────┐  │  ┌─────────────────────────────────────┐│
│  │   In-Process    │  │  │      Inter-Process                  ││
│  │ Communication   │  │  │     Communication                   ││
│  │                 │  │  │                                     ││
│  │ • Direct Calls  │  │  │ • JSON/MessagePack Protocol         ││
│  │ • Shared Memory │  │  │ • Named Pipes / Unix Sockets        ││
│  │ • C++ Interface │  │  │ • Process Lifecycle Management      ││
│  └─────────────────┘  │  └─────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────────┘
```

## Implementation Plan

### Phase 1: Foundation and CLI Enhancement
**Duration**: 2-3 weeks
**Goal**: Create CLI infrastructure for node management

#### 1.1 CLI Framework Enhancement
- **File**: `main.cpp` - Complete overhaul with proper command framework
- **New Commands**:
  - `akao install <source>` - Install from local path, git repo, or registry
  - `akao uninstall <node-id>` - Remove external node with user confirmation
  - `akao enable <node-id>` - Enable disabled node
  - `akao disable <node-id>` - Disable active node
  - `akao list-nodes` - List all available nodes with status
  - `akao node-info <node-id>` - Show detailed node information
  - `akao search <query>` - Search available nodes
  - `akao update <node-id>` - Update external node to latest version

#### 1.2 Directory Structure Creation
- **Create**: `.akao/nodes/external/` - External node storage
- **Create**: `.akao/nodes/packages/` - Node package cache
- **Create**: `.akao/nodes/disabled/` - Disabled node storage
- **Create**: `.akao/config/nodes/` - Node configuration files
- **Create**: `.akao/cache/` - Runtime caches and temporary files

#### 1.3 Configuration System
- **File**: `core/config/nodes/manager.hpp` - Node configuration management
- **Features**:
  - Node installation paths
  - Registry URLs
  - Trust settings
  - Security policies
  - Resource limits

### Phase 2: External Node Communication Protocol
**Duration**: 3-4 weeks
**Goal**: Implement process-based node communication

#### 2.1 Communication Protocol Design
- **Protocol**: JSON-RPC 2.0 over Unix Domain Sockets
- **Message Types**:
  - `node.info` - Get node metadata
  - `node.validate` - Validate parameters
  - `node.execute` - Execute node with parameters
  - `node.health` - Health check
  - `node.shutdown` - Graceful shutdown

#### 2.2 Node Process Manager
- **File**: `core/engine/orchestrator/process/manager.hpp`
- **Features**:
  - Process lifecycle management
  - Resource monitoring and limits
  - Health checking and restart
  - Graceful shutdown handling
  - Process isolation and sandboxing

#### 2.3 External Node Bridge
- **File**: `core/engine/orchestrator/bridge/external.hpp`
- **Purpose**: Translate between internal INode interface and external process communication
- **Features**:
  - Protocol translation
  - Error handling and timeout management
  - Async communication with callbacks
  - Process failure recovery

### Phase 3: Package Management System
**Duration**: 3-4 weeks
**Goal**: Implement node package installation and management

#### 3.1 Package Format Specification
- **Format**: Node packages as `.akao` archives (ZIP-based)
- **Structure**:
  ```
  my-node.akao
  ├── _.yaml              # Node manifest
  ├── executable          # Node executable
  ├── README.md           # Documentation
  ├── LICENSE             # License file
  └── resources/          # Node resources
  ```

#### 3.2 Package Manager
- **File**: `core/engine/orchestrator/package/manager.hpp`
- **Features**:
  - Package installation/uninstallation
  - Dependency resolution
  - Version compatibility checking
  - Package verification and signing
  - Repository management

#### 3.3 Node Manifest Format
- **File**: `_.yaml` in node package
- **Structure**:
  ```yaml
  id: "com.example.my-node"
  name: "My Custom Node"
  version: "1.0.0"
  description: "Node description"
  
  runtime:
    type: "executable"  # or "python", "node", "java", etc.
    command: "./executable"
    args: ["--mode", "server"]
    
  dependencies:
    - id: "com.example.base-lib"
      version: ">=1.0.0"
      
  permissions:
    - filesystem.read
    - network.http
    
  resources:
    memory: "128MB"
    cpu: "100m"
    
  inputs:
    - name: "input_data"
      type: "object"
      required: true
      
  outputs:
    - name: "result"
      type: "object"
  ```

### Phase 4: Security and Sandboxing
**Duration**: 2-3 weeks
**Goal**: Implement comprehensive security framework

#### 4.1 Process Sandboxing
- **Technology**: Linux namespaces, cgroups, seccomp-bpf
- **Isolation**:
  - Filesystem isolation (chroot/containers)
  - Network isolation
  - Process isolation
  - Resource limits (CPU, memory, file descriptors)

#### 4.2 Permission System
- **File**: `core/security/permissions/manager.hpp`
- **Capabilities**:
  - `filesystem.read` - Read file system
  - `filesystem.write` - Write file system
  - `network.http` - HTTP requests
  - `network.tcp` - TCP connections
  - `system.exec` - Execute processes

#### 4.3 Trust and Verification
- **Features**:
  - Package signature verification
  - Trusted source management
  - Runtime integrity checking
  - Security audit logging

### Phase 5: Dynamic Loading and Registry Enhancement
**Duration**: 2-3 weeks
**Goal**: Implement runtime node loading and management

#### 5.1 Enhanced Registry System
- **File**: `core/engine/orchestrator/registry/v1.hpp`
- **New Features**:
  - Runtime node loading/unloading
  - Node state persistence
  - Node metadata caching
  - Node dependency tracking
  - Node lifecycle management

#### 5.2 Dynamic Node Loader
- **File**: `core/engine/orchestrator/loader/dynamic.hpp`
- **Features**:
  - Node discovery and scanning
  - Package validation and loading
  - Node instance creation
  - Node health monitoring

#### 5.3 Node State Management
- **File**: `core/engine/orchestrator/state/manager.hpp`
- **Features**:
  - Node enable/disable state
  - Node installation tracking
  - Node update management
  - Configuration persistence

### Phase 6: Testing and Integration
**Duration**: 2-3 weeks
**Goal**: Comprehensive testing and integration

#### 6.1 Test Framework Enhancement
- **External Node Testing**: Framework for testing external nodes
- **Integration Tests**: End-to-end workflow testing with external nodes
- **Security Tests**: Sandboxing and security verification
- **Performance Tests**: External node performance benchmarking

#### 6.2 Example External Nodes
- **Python Node**: Simple Python-based node example
- **Node.js Node**: JavaScript-based node example
- **Go Node**: Compiled Go-based node example
- **Shell Script Node**: Bash-based node example

#### 6.3 Documentation and Guides
- **External Node Development Guide**
- **Package Creation Tutorial**
- **Security Best Practices**
- **Troubleshooting Guide**

## Technical Specifications

### Node Communication Protocol

#### Message Format
```json
{
  "jsonrpc": "2.0",
  "method": "node.execute",
  "params": {
    "context": {
      "node_id": "my-node",
      "workflow_id": "test-workflow",
      "execution_id": "exec-123"
    },
    "parameters": {
      "input_data": {...}
    }
  },
  "id": "req-456"
}
```

#### Response Format
```json
{
  "jsonrpc": "2.0",
  "result": {
    "success": true,
    "outputs": {
      "result": {...}
    },
    "metadata": {
      "execution_time": 1234,
      "memory_used": 5678
    }
  },
  "id": "req-456"
}
```

### Security Architecture

#### Resource Limits
- **Memory**: Configurable per node (default 256MB)
- **CPU**: Configurable CPU quota (default 100m)
- **File Descriptors**: Limited to prevent resource exhaustion
- **Network**: Restricted to allowed endpoints only

#### Filesystem Isolation
- **Chroot Environment**: Each node runs in isolated filesystem
- **Read-Only System**: System files are read-only
- **Temporary Space**: Writable temporary directory per node
- **Volume Mounts**: Explicit volume mounts for data access

### Performance Considerations

#### Node Startup Optimization
- **Lazy Loading**: Nodes loaded only when needed
- **Startup Caching**: Cache node metadata and capabilities
- **Connection Pooling**: Reuse node processes for multiple executions
- **Preloading**: Preload frequently used nodes

#### Communication Optimization
- **Message Compression**: Compress large messages
- **Batch Operations**: Batch multiple operations
- **Streaming**: Support streaming for large data transfers
- **Connection Multiplexing**: Multiple requests per connection

## Migration Strategy

### Backward Compatibility
- **Existing Workflows**: All existing workflows continue to work
- **Built-in Nodes**: Existing built-in nodes unchanged
- **API Compatibility**: Maintain existing API surface
- **Configuration**: Graceful configuration migration

### Gradual Rollout
1. **Phase 1**: CLI enhancement (no functional changes)
2. **Phase 2**: External node support (opt-in)
3. **Phase 3**: Package management (new feature)
4. **Phase 4**: Security hardening (transparent)
5. **Phase 5**: Performance optimization (transparent)
6. **Phase 6**: Full external node ecosystem

## Success Metrics

### Functionality Metrics
- **Node Installation**: Time to install external node < 30 seconds
- **Node Execution**: External node execution overhead < 10ms
- **Package Management**: Support for 100+ external nodes
- **Security**: Zero security vulnerabilities in external nodes

### Performance Metrics
- **Startup Time**: System startup < 500ms with 50 nodes
- **Memory Usage**: <10MB baseline + 50MB per active external node
- **CPU Usage**: <5% baseline + configurable per external node
- **Network Overhead**: <1KB per node execution

### User Experience Metrics
- **Easy Installation**: One-command node installation
- **Clear Documentation**: Complete external node development guide
- **Error Handling**: Clear error messages and recovery guidance
- **Node Discovery**: Searchable node registry

## Risk Mitigation

### Technical Risks
- **Performance Degradation**: Comprehensive benchmarking and optimization
- **Security Vulnerabilities**: Security-first design with sandboxing
- **Complexity Increase**: Incremental rollout with backward compatibility
- **Node Compatibility**: Standardized node interface and testing

### Operational Risks
- **Node Quality**: Community guidelines and review process
- **Support Burden**: Comprehensive documentation and examples
- **Ecosystem Fragmentation**: Standardized node APIs and best practices
- **Maintenance Overhead**: Automated testing and CI/CD integration

## Conclusion

This comprehensive plan transforms Akao from a static node system to a dynamic, secure, and extensible external node ecosystem. The implementation preserves all existing functionality while adding powerful new capabilities that position Akao as a competitive alternative to systems like N8N, with superior performance through native C++ implementation and comprehensive security through process-based sandboxing.

The phased approach ensures minimal disruption to existing users while enabling rapid adoption of external node capabilities. The security-first design ensures that external nodes cannot compromise system integrity, while the performance optimizations maintain Akao's competitive advantage in execution speed.

The end result will be a robust, secure, and highly extensible workflow orchestration platform that can compete with the best-in-class solutions while maintaining its philosophical foundations and architectural integrity.