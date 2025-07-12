# ✅ AKAO C++ ORCHESTRATOR CLI DESIGN

**⚠️ PROVISIONAL**: This project is in active design phase using "vibe coding" methodology.

## CURRENT CLI ARCHITECTURE

The akao CLI is designed for a C++ workflow orchestrator platform with node-based processing.

```bash
akao <command> [--node|--workflow] <targets...> [--options] [--] [params...]
```

---

## 🔧 CORE COMMANDS

### **Node Management**
| Command                                   | Description                                                       |
| ----------------------------------------- | ----------------------------------------------------------------- |
| `akao list --node`                        | List all installed nodes (builtin + external)                    |
| `akao install --node <src1> <src2>`       | Install external nodes from local paths or git URLs              |
| `akao uninstall --node <name>`            | Uninstall external node (ask user: delete or archive)            |
| `akao enable --node <name>`               | Enable a previously installed node                                |
| `akao disable --node <name>`              | Disable a node without uninstalling it                           |
| `akao run --node <name> -- [args...]`     | Run a specific node with arguments                                |
| `akao search --node <keyword>`            | Search for nodes by name, ID, or keyword                         |

### **Workflow Management**
| Command                                   | Description                                                       |
| ----------------------------------------- | ----------------------------------------------------------------- |
| `akao list --workflow`                    | List all installed workflows                                      |
| `akao install --workflow <src1> ...`      | Install one or more workflows                                     |
| `akao uninstall --workflow <name>`        | Uninstall a workflow                                              |
| `akao enable --workflow <name>`           | Enable a workflow                                                 |
| `akao disable --workflow <name>`          | Disable a workflow                                                |
| `akao run --workflow <name> -- [args...]` | Run a workflow with optional parameters                           |
| `akao search --workflow <keyword>`        | Search for workflows by name, ID, or keyword                     |

### **System Commands**
| Command                                   | Description                                                       |
| ----------------------------------------- | ----------------------------------------------------------------- |
| `akao version`                            | Show version information                                          |
| `akao status`                             | Show system status and health                                     |
| `akao validate`                           | Validate system configuration                                     |
| `akao test`                               | Run system tests                                                  |

---

## 💡 CURRENT USAGE EXAMPLES

### **Working with Builtin Nodes**
```bash
# List all nodes (builtin + external)
akao list --node

# Run specific builtin nodes
akao run --node file -- --path ./core --recursive
akao run --node yaml -- --parse config.yaml
akao run --node logic -- --expression "result = 1 + 2"
akao run --node reporter -- --format json --output results.json
```

### **Working with Workflows**
```bash
# List available workflows
akao list --workflow

# Run workflow
akao run --workflow validation -- --target ./core --verbose
akao run --workflow build -- --configuration release
```

### **System Operations**
```bash
# Check system status
akao status
akao validate
akao test

# Show version
akao version
```

---

## 🌍 GLOBAL FLAGS

| Flag        | Description                                     |
| ----------- | ----------------------------------------------- |
| `--verbose` | Print detailed logs to the terminal            |
| `--quiet`   | Suppress all output (useful in CI/CD)          |
| `--yaml`    | Output results in YAML format                  |
| `--json`    | Output results in JSON format                  |
| `--debug`   | Enable debug mode with detailed logging        |

---

## 🏗️ CURRENT IMPLEMENTATION STATUS

### **Working Commands** ✅
- Basic executable: `akao --version`
- Node execution via direct workflow calls
- Build system integration

### **Planned Commands** ⏳
- Comprehensive node management
- Workflow management system
- External node installation
- System status and validation

---

## 📋 ARCHITECTURE NOTES

### **Node Types**
- **Builtin Nodes**: C++ nodes in `.akao/nodes/` (file, logic, yaml, reporter)
- **External Nodes**: Future support for multi-language nodes
- **Workflow Definitions**: YAML-based workflow configurations

### **Command Structure**
- **Clear separation**: Commands vs targets vs parameters
- **Consistent patterns**: All commands follow same structure
- **Future-proof**: Easy to add new node types and commands
- **Familiar syntax**: Similar to docker, kubectl, npm

### **Integration Points**
- **Build system**: Makefile integration
- **Testing**: Comprehensive test suite
- **Workflows**: YAML-based execution
- **Node registry**: Automatic node discovery

---

## ✅ KEY ADVANTAGES

* **No name collisions**: Clear command/target separation
* **Flexible**: Clean parameter passing with `--` separator
* **Extensible**: Easy to add new node types and commands
* **Consistent**: Familiar patterns from popular tools
* **Current**: Reflects actual C++ orchestrator architecture

This CLI design supports the current working C++ workflow orchestrator while providing a foundation for future external node support.