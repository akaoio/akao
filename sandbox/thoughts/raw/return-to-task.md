We were in the middle of refactoring some components:
@core/engine/validator (being hardcoded, wrong)
@core/philosophy
@core/rule
@core/trace
Into one (but new tech) component:
@core/engine/validator

How the new workflow works:
- User can create configurable rules in .akao/rules of there repo (including this repo)
- All rules must be written in .yaml format or .a (future builtin Akao Pure Logic format)
- All rules must not use datalog, but use advanced math
- When the user type "akao validate" -> validator lazy-load all enabled rules and validate the current working folder recursively
- We have a lazy loader in here core/engine/runtime but the core/engine/runtime folder it self violates the rules. (no files allowed to stand besides folder, files can only stand together, excerpt _.yaml and environment config files at the root folder)

What this refactoring requires?
- core/engine/logic is a fucking mess. it has godel, incompleteness blah blah. its core engine is at core/engine/logic/core , but many of its children stand at the same folder level with it.
- these ones are confusing:
core/engine/logic/cpp
core/engine/logic/datalog
core/engine/logic/filesystem
core/engine/logic/builtin
- these ones are also confusing in another way:
core/engine/logic/cpp
core/engine/logic/datalog
core/engine/logic/filesystem
core/engine/logic/builtin
- I think we should move core/engine/logic/core -> core/engine/logic/core
- And this is how the logic/ folder should look like:
logic/
  - core/
  - components/
- There are many placeholders in this pure logic core:
core/engine/logic/core/v1.cpp
core/engine/logic/core/v1.hpp
we need to pay all tech debts before moving on