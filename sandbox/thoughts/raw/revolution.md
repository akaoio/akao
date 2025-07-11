Currently, the system has undergone several significant transformations.
From being a system with a fixed workflow, we have now turned it into a node-based system with a workflow orchestrator.

However, there are some weaknesses:

Our system cannot load nodes dynamically during the execution of a run chain; instead, all nodes are loaded at compile time.

Additionally, nodes are currently required to call libraries from the system's core in order to register themselves. This violates the design logic and architectural philosophy of the system.

The roadmap indicates that the designer is intending to incorporate both Python and JavaScript into the system, which will make codebase maintenance burdensome in the future.

Here are several things we should do before continuing with project implementation:

1. Treat both internal and external nodes equally, although they should still be separated into different folders.
2. When the user types a command like akao install ~/abcdef, the system should automatically scan the directory for a _.yaml file.
3. When the user types akao install https://github.com/abcde/asdf or https://github.com/abcde/asdf.git, the system must check if the git repo contains a _.yaml file, and if so, clone the repo into the local .akao/nodes folder.
4. Users must be able to install, uninstall, enable, or disable any external node at runtime, with lazy loading.
5. When a node is uninstalled, the system must ask the user whether they want to move the node elsewhere to keep it, or delete it completely from the computer.
6. No node is allowed to call any core libraries. Every node must be sandboxed, standalone, and able to work independently of the orchestrator—including built-in nodes and external nodes.
7. We should not implement our own built-in JavaScript or Python runtimes. Instead, if the system detects that such software is not installed, it must ask the user for permission to install external tools (like npm, node, python, python3, pip, etc.).
8. The system must clearly display a message asking the user to install external dependencies before proceeding.
9. The system must be designed to support any kind of node written in any language, without having to maintain or implement external runtimes (like Node.js, npm, Cargo, Python, etc.) inside its core.
10. Users are allowed two install/uninstall/enable/disable any external node. Users cannot disable or uninstall any nodes that are builtins. the builtin nodes must always be installed and always be enabled system wide.
11. Nodes should be stored in .akao/nodes, instead of nodes (but I'm not sure, need further thingking)
12. In the future, our system should be an alternative to N8N, but much faster and native C++ for speed.
13. The system should not only be a codebase validator. It should be a real and practical and no-bullshit workflow orchestrator, with 100% externalized/configurable business logics.

REASEARCH AND WRITE PLAN:

We must do a deep and exhaustive research about the system architecture beef on making plan.
We don't want to duplicate our architecture, or code, or rewrite what is written already. We do not want to create more technical debts.
The plan must be technocratic and insightful.

Plan should be written in sandbox/thoughts/refined/