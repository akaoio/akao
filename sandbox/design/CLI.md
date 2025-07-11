## ✅ Standardized CLI Design for `akao`

```bash
akao <command> [--node|--workflow] <targets...> [--options] [--] [params...]
```

---

## 🔧 Core Commands

| Command                                   | Description                                                       |
| ----------------------------------------- | ----------------------------------------------------------------- |
| `akao list --node`                        | List all installed external nodes (enabled/disabled/archived)     |
| `akao list --workflow`                    | List all installed workflows                                      |
| `akao install --node <src1> <src2>`       | Install one or more nodes from local paths or git URLs            |
| `akao uninstall --node <name>`            | Uninstall a node (ask user: delete or archive)                    |
| `akao enable --node <name>`               | Enable a previously installed node                                |
| `akao disable --node <name>`              | Disable a node without uninstalling it                            |
| `akao run --node <name> -- [args...]`     | Run a specific node with arguments (e.g., CLI flags, file inputs) |
| `akao install --workflow <src1> ...`      | Install one or more workflows                                     |
| `akao uninstall --workflow <name>`        | Uninstall a workflow                                              |
| `akao enable --workflow <name>`           | Enable a workflow                                                 |
| `akao disable --workflow <name>`          | Disable a workflow                                                |
| `akao run --workflow <name> -- [args...]` | Run a workflow with optional parameters                           |
| `akao search --node <keyword>`            | Search for nodes by name, ID, or keyword                          |
| `akao search --workflow <keyword>`        | Search for workflows by name, ID, or keyword                      |

---

## 💡 Usage Examples

```bash
akao list --node
akao install --node ./my-node https://github.com/user/some-node.git
akao uninstall --node logic-core
akao run --node logic-core -- ./examples/godel-exhibition.a --debug

akao install --workflow ./wf1 ./wf2
akao run --workflow validation -- --metadata --orphan
akao search --node godel
```

---

## 🌍 Global Flags (Available in all commands)

| Flag        | Description                                     |
| ----------- | ----------------------------------------------- |
| `--verbose` | Print detailed logs to the terminal or log file |
| `--quiet`   | Suppress all output (useful in CI/CD)           |
| `--yaml`    | Output results in YAML format                   |
| `--json`    | Output results in JSON format                   |

---

## ✅ Key Advantages of This Model

* **No name collisions**: `install`, `run`, etc. are top-level commands, not node/workflow names
* **Flexible**: Clean separation of command/target/params using `--`
* **Future-proof**: Easy to add support for `--plugin`, `--template`, etc.
* **Familiar**: Similar to tools like `git`, `docker`, `kubectl`, and `npm`
