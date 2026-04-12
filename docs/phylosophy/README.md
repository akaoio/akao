# Philosophy Index

This folder extracts durable ideas from `README.md` and `docs/thoughts/` into a smaller set of philosophical documents.

These are not implementation guides. They are attempts to identify the beliefs behind the system: what it optimizes for, what it distrusts, and what it treats as foundational.

The source of code in this project is not a framework, not a library, and not a stack of fashionable tools. The source is a belief about what the web already is.

The web is already a runtime. The browser is already a computer. The open platform is not a broken beginning waiting for completion by a private abstraction. It is already substantial enough to host serious systems.

From that belief follows the rest:

- if the browser is real infrastructure, then the app can be static and still be powerful
- if native standards are enough, then frameworks are optional, not foundational
- if users can compute, store, verify, and communicate locally, then the server becomes less of a ruler and more of a helper
- if identity can be cryptographic, then the platform does not need to possess the user

This is why the project values serverlessness, directness, local capability, and decentralized coordination. These are not separate features. They are consequences of one worldview: push capability toward the edge, and trust the platform that already exists.

In this sense, code is only the visible layer. Underneath it is a set of commitments:

- native over ornamental abstraction
- static over unnecessarily dynamic
- local truth before remote dependence
- verifiability over trust theater
- ownership over custody

So the source of code here is a specific spirit: that the open web is sufficient, that the browser deserves respect as a first-class machine, and that software should return power to the user whenever possible.

Code is what that spirit looks like when it becomes exact.

## Documents

1. [Frameworkless Native Web](./01-frameworkless-native-web.md)
2. [TDD as Specification](./02-tdd-as-specification.md)
3. [Decentralized-First Design](./03-decentralized-first-design.md)
4. [Temporal Validation Systems](./04-temporal-validation-systems.md)
5. [Local-First Storage Architecture](./05-local-first-storage-architecture.md)
6. [Isomorphic Runtime Architecture](./06-isomorphic-runtime-architecture.md)
7. [Cryptographic Sovereignty](./07-cryptographic-sovereignty.md)
8. [DSLs as Decentralized Authority](./08-dsls-as-decentralized-authority.md)
9. [People, Culture, and Historical Direction](./09-people-culture-and-historical-direction.md)

## Recurring Principles

- Determinism is preferred to hidden coordination.
- Native standards are preferred to framework abstractions.
- Validation should happen as early as possible.
- Local state and local storage are first-class, not fallback.
- Tests are specification, not decoration.
- Users should control keys, identity, and data whenever possible.
- Naming should be constrained enough to keep the conceptual space manageable.
- A system must know which people it serves and which civilizational memory gives it direction.

## Sources

The main source material is:

- `README.md`
- `docs/thoughts/the-philosophy-of-framework-less.md`
- `docs/thoughts/TDD-MINDSET.md`
- `docs/thoughts/white-paper.md`
- `docs/thoughts/temporal-proof-of-work-gossip-protocol.md`
- `docs/thoughts/sqlite-wasm-opfs-worker.md`
- `docs/thoughts/indexing-discovery-guide.md`
- `docs/thoughts/webauthn-prf-extension.md`
- `docs/thoughts/pen-dsl-guide.md`
- plus the remaining notes in `docs/thoughts/`
