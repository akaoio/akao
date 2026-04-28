# tool-kit — Two QA skills, no pipeline

Exactly two self-contained skills for the `shop` project:

- **`tool-kit-test`** — test ONE feature through 5 coverage flows; scaffold missing specs; run only this feature's tests against the built artifact.
- **`tool-kit-attack`** — hunt vulnerabilities on ONE feature; emit a plan-mode report with proposed diffs; never edit source before user approval.

The two skills are independent — there is no chaining or shared state between them. They share a bundle (`tool-kit`) and a theme (quality), nothing more. Instructions live in `skills/tool-kit-{test,attack}.md`; no Node orchestrator runs between the user and Playwright. The skills call `npx playwright test --grep "@<feature>"` and per-feature Node tests directly.

## Install

Multi-agent matrix. Idempotent — safe to re-run.

```bash
node tools/tool-kit/install.js                   # interactive picker
node tools/tool-kit/install.js --targets claude  # one target
node tools/tool-kit/install.js --targets all     # all five
```

| Agent | Install path | Layout |
|---|---|---|
| Claude Code | `~/.claude/skills/<skill>/SKILL.md` | per-skill dir |
| Codex CLI | `~/.codex/skills/<skill>/SKILL.md` | per-skill dir |
| Cursor | `~/.cursor/skills/tool-kit/<skill>.md` | flat bundle (+ `personas/`) |
| Windsurf | `./.windsurf/skills/tool-kit/<skill>.md` | flat bundle (+ `personas/`) |
| GitHub Copilot | `./.github/skills/tool-kit/<skill>.md` | flat bundle (+ `personas/`) |

To remove (also purges legacy entries from earlier installs):

```bash
node tools/tool-kit/uninstall.js
```

## Use

```text
/tool-kit-test <feature>     # e.g. /tool-kit-test swap
/tool-kit-attack <feature>   # e.g. /tool-kit-attack withdraw
```

## What's in this directory

```
tools/tool-kit/
├── README.md                       ← this file
├── install.js                      ← multi-agent installer (5 targets, idempotent)
├── uninstall.js                    ← purges current + legacy skill dirs
├── skills/
│   ├── tool-kit-test.md            ← source of truth for the test skill
│   └── tool-kit-attack.md          ← source of truth for the attack skill
├── personas/                       ← reference checklists; NOT separate skills
│   ├── alex.md   (security auditor — OWASP / CWE Top 25)
│   ├── rex.md    (pentester — adversarial vector taxonomy)
│   ├── vera.md   (web3 auditor — DeFi attack classes)
│   ├── cora.md   (confused user — usability-induced loss)
│   └── pax.md    (performance hawk — bundle / latency budgets)
└── tools/
    └── scan-components.js          ← pierce-depth helper for Shadow DOM specs
```

`tool-kit-attack.md` references the personas as reference material to broaden coverage; the skills do not require them at runtime.

### `scan-components.js` — when to use it

Optional, not a dependency. Run it when you're scaffolding a flow-3 spec (`e2e/components/<feature>.spec.ts`) and want to confirm the right Shadow-DOM pierce chain before writing the query path:

```bash
node tools/tool-kit/tools/scan-components.js <feature>
# e.g. node tools/tool-kit/tools/scan-components.js withdraw
```

It prints the recommended pierce chain (depth-2 vs depth-1) by walking `build/UI/routes/<feature>/` and detecting which custom elements compose the route. If you already know the convention (`route-<x>.shadowRoot → layout-main.shadowRoot → target` for depth-2; `route-<x>.shadowRoot → target` for `game`/`tag`/`item`/`test`), you can skip it.

## Non-goals

- No `tool-kit-qa` orchestrator skill.
- No separate `tool-kit-preview` skill (the `preview.js` static server is invoked directly by `playwright.config.ts`'s `webServer` block).
- No separate `tool-kit-review` skill — code review is out of scope for this surface.
- No multi-step skill choreography. Each skill stands alone.
- No `_pipeline-out/` JSON artifacts. The artifacts are: scaffolded spec files in `e2e/` and `src/core/tests/`, and Playwright's HTML/JSON report at `playwright-report/`.
