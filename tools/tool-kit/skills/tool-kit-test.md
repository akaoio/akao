---
name: tool-kit-test
description: "Test ONE feature through 5 coverage flows, one flow at a time, with a user-decision gate after each. Spawns review lenses as parallel subagents (party-mode pattern) for independent perspective on coverage gaps + scaffold drafts. Surfaces sibling features that would benefit from the same testing. Self-contained. Args: <feature-name> [--solo] [--start <N>]"
---

# tool-kit-test — Sequential 5-flow test skill with per-flow gates

Test ONE feature through 5 coverage flows. Walk the flows one at a time. After each flow, **stop and ask the user** whether to continue, end, jump ahead, or repeat — never silently skip a flow because the skill judged it "not applicable".

## Why This Matters

Tests aren't all-or-nothing. A feature might need flow 2 (logic) urgently, flow 3 (DOM) later, and flow 5 (e2e) only after some external dependency lands (e.g. live torrent peers, real wallet, deployed contract). When a single batch run silently rationalizes "flow 5 doesn't apply because X", the user loses visibility and control.

Sequential per-flow gating restores that control: the skill proposes, the user decides. Same shape as `bmad-party-mode` — show the substance, hand the wheel back, wait for the next instruction.

A second invariant: testing one feature often surfaces siblings that share its contract (leech ↔ seed both ride peer-discovery + piece selection; deposit ↔ withdraw both touch wallet + chain). Whenever the audit or lenses identify a sibling, the skill names it explicitly so the user can queue another `/tool-kit-test <sibling>`.

## Arguments

- `$ARGUMENTS` — must include exactly one feature name (e.g. `swap`, `withdraw`, `Wallet`, `leech-seed`).
- `--solo` — skip the lens-consultation step. The orchestrator does the audit and scaffold drafts in a single pass. Default is consult mode (parallel subagents). Use `--solo` when subagents are unavailable, when speed beats perspective diversity, or for re-runs on a feature you've already characterized.
- `--start <N>` — start at flow `N` (default `1`). Useful when resuming after a previous session ended at flow `N-1`.

## On Activation

1. **Parse arguments** — feature name, `--solo`, `--start`.

2. **Resolve the feature.**
   - Route: `src/UI/routes/<name>/` exists. Get the custom element tag from `build/UI/routes/<name>/index.js` (`customElements.define("route-<x>", ...)`).
   - Core module: `src/core/<name>/` (directory) or `src/core/<name>.js` or `src/core/<sub>/<name>.js`.
   - If neither, stop with a clear message naming the paths checked.

3. **Read project context**: `README.md`, any relevant `docs/**/*.md` you find by grepping for the feature name, the feature source. Newer docs win over stale ones.

4. **Audit all 5 flows (no scaffolding yet).** For each flow, record `present` / `missing` / `not-applicable` with a one-line reason. **Treat `not-applicable` as a hypothesis to verify with the user — not as a final decision.**

5. **Lens consultation** (default — skip with `--solo`). Spawn 2-3 review lenses as parallel subagents via the Agent tool. Each lens thinks independently and returns its take; the orchestrator aggregates. Spec below in **Lens spawn protocol**.

6. **Surface sibling-feature suggestions.** From the lens output and your own audit, name any sibling feature(s) that share a contract with this one and would benefit from the same testing pass. Two or three names max. Format: `→ Suggested next: /tool-kit-test <sibling> — reason`.

7. **Print the full audit + plan + sibling list, then enter the Core Loop at flow `--start` (default 1).**

## The Core Loop

For each flow `N` from the start position to 5, **in order**:

### 1. Announce

State which flow is up, its status from the audit, and the planned action:

```
── Flow <N>: <Build artifacts | Isomorphic core | Web components | Local-first | User journeys> ──
audit-status: <present | missing | not-applicable>
planned-action: <skip | scaffold | run-existing | scaffold+run>
reason: <one-line>
```

### 2. Execute (or skip)

Per `planned-action`:

- `skip` (when `not-applicable`): do nothing this flow. Note the reason in the report.
- `scaffold`: write the spec following **Scaffolding rules**, tagged `@flow<N> @<feature>`.
- `run-existing`: invoke the run command for this flow (see **Run commands**).
- `scaffold+run`: scaffold first, then run.

### 3. Report this flow's outcome

```
flow <N> verdict: <pass | fail | scaffolded | no-specs | n/a>
files touched: <list>
[run output if applicable, last 20 lines]
```

### 4. Decision gate — wait for the user

Emit **exactly** this prompt and stop:

```
── flow <N> done. ──

Reply one of:
- continue       → run flow <N+1>
- end            → stop now, emit final summary across flows 1..<N>
- skip to <M>    → jump straight to flow M (where M > N), record N+1..M-1 as skipped
- redo           → re-audit, re-scaffold and re-run flow <N>
- force          → if flow <N> was skipped as n/a, override and scaffold/run it anyway
```

**Do NOT proceed to flow `N+1` without a user reply.** Treat any other natural-language reply as instruction to interpret (e.g. "actually run flow 5 anyway and ignore torrent" → behave as `force`).

### 5. Loop or exit

- On `continue` or empty reply → set N := N+1; if N > 5, exit; else loop.
- On `end` → exit immediately, emit final summary.
- On `skip to <M>` → set N := M; loop.
- On `redo` → re-execute step 2-4 for the same N.
- On `force` → flip planned-action from `skip` to `scaffold+run`, re-execute step 2-4.

## Lens Spawn Protocol (consult mode)

Pick the relevant lenses based on the audit:

| Lens | Focus | Activate when |
|---|---|---|
| 🔍 Coverage Auditor | Walks the 5 flows, judges `present` / `missing` / `not-applicable`, flags gaps the mechanical audit might have rationalized away | always |
| ✏️ Spec Drafter | For each missing applicable flow, drafts the spec shape — pierce path, test cases, tags, what to assert — without writing it to disk | when ≥1 flow is `missing` |
| 🚦 TDD Enforcer | Critiques scaffolded specs: are they correctly red on broken behavior, green on working? Is `test.fail` placed where contract is genuinely unmet? Are assertions semantic (test the contract) or accidental (test the implementation)? | when ≥1 spec was scaffolded in a prior turn |

For each picked lens, spawn one subagent in parallel (all Agent calls in a single response). Each gets:

```
You are <lens-name>, a review lens in a feature-coverage audit.

## Your Lens
<lens focus from the table above + 2-3 sentence elaboration>

## Feature Under Review
- Name: <feature>
- Source: <path/to/source/files>
- Resolved type: route | core-module
- Audit so far: <the 5-flow status table>

## Your Task
<lens-specific question>:
- Coverage Auditor: confirm or challenge the audit. Each flow with status should be cited; each `not-applicable` needs a one-line justification you'd defend. ALSO: list up to 3 sibling features that share a contract with this one and would benefit from the same testing.
- Spec Drafter: for each `missing` applicable flow, write a 5-15 line draft (pseudocode is fine) showing test name(s), key assertions, and the pierce path / import shape. No prose preamble.
- TDD Enforcer: read each scaffolded spec and verdict each: `red-correctly` / `green-correctly` / `wrong-direction` / `tests-implementation-not-contract`. Cite the assertion line.

Output Markdown. No prose preamble.

## Guidelines
- Stay scoped to your lens. Don't audit security; that's `tool-kit-attack`.
- Do NOT use tools — you are advising, not executing.
- If you have nothing substantive to add, say so in one line.
```

After all lenses return, aggregate. Where lenses agree → that's the plan. Where they disagree → present both takes in the audit print-out and pick the safer one (more coverage, redder TDD direction).

`--solo` mode: skip the spawn. The orchestrator drafts coverage gaps + spec shapes + sibling suggestions itself in one pass. Faster, less diverse.

## The 5 flows

| # | Flow | Where the spec lives | "Has it" means |
|---|---|---|---|
| 1 | Build artifacts | inline assertion in skill | The feature's emitted files exist in `build/` AND have the expected shape. Routes: `build/UI/routes/<name>/index.html` exists, `build/UI/routes/<name>/index.js` exists and contains `customElements.define("route-<name>"`. Core modules: `build/core/<path>/<name>.js` exists. **Each feature gets a concrete, named assertion — not "bundle exists".** |
| 2 | Isomorphic core / logic | `src/core/tests/<name>.test.js`, registered in `src/core/tests/manifest.js` | A test exercising non-DOM behavior (state, router, transforms, pure functions). If the feature has no isomorphic surface, mark `not-applicable` with the reason — DO NOT scaffold a fake-passing stub. The user can `force` to scaffold a SKIP-stub if they want the gap visible. |
| 3 | Web components / Shadow DOM | `e2e/components/<name>.spec.ts` | Render, reactivity, custom events, lifecycle. Shadow-DOM pierce: `route-<name>.shadowRoot → layout-main.shadowRoot → target` for depth-2 routes; `route-<name>.shadowRoot → target` for depth-1 (`game`, `tag`, `item`, `test`). Optional helper (not a dependency): `node tools/tool-kit/tools/scan-components.js <name>` prints the pierce depth. Tag the describe block `@flow3 @<name>`. |
| 4 | Local-first / cache | `e2e/local-first/<name>.spec.ts` | `page.context().setOffline(true)` round-trip + hash-based invalidation. Mark `not-applicable` if the feature reads no IndexedDB-backed data. Tag `@flow4 @<name>`. |
| 5 | User journeys | `e2e/<name>.spec.ts` | End-to-end real-user flow against the built artifact. Tag `@flow5 @<name>`. |

## Scaffolding rules

- All Playwright specs target the built artifact via `node preview.js` on `127.0.0.1:8080` — the `webServer` block in `playwright.config.ts` boots it. Never test against the dev server.
- Every spec's outermost `test.describe(...)` MUST start with `@flowN @<feature>` so `--grep "@<feature>"` finds it.
- Known-broken behavior: wrap with `test.fail("…")` so CI stays green while the bug is documented.
- `forbidOnly: !!process.env.CI` is set — never commit `test.only(…)`.
- Look at any existing spec in the matching directory as a shape reference. Honor the convention you observe — tag, pierce-depth pattern, threat-model header — don't copy paths verbatim.

## Run commands

For each flow with `planned-action` of `run-existing` or `scaffold+run`:

| Flow | Command (developer loop) |
|---|---|
| 1 | inline `existsSync` + `readFileSync` regex check in the skill itself |
| 2 | `node --input-type=module -e "import('./src/core/tests/<name>.test.js').then(m => m.default ? m.default() : null)"` — runs **just this single registered test** |
| 3 | `npx playwright test --project=chromium --grep "@<name>" e2e/components/` |
| 4 | `npx playwright test --project=chromium --grep "@<name>" e2e/local-first/` |
| 5 | `npx playwright test --project=chromium --grep "@<name>" e2e/<name>.spec.ts` |

CI runs the full 3-browser matrix via the existing `.github/workflows/playwright.yml` (orthogonal to this skill — leave it alone).

### Zero-match handling — never silent-pass

**Zero matched tests is NOT a pass.** Both "spec file missing" and "spec file exists but no `@<feature>` tag inside" must be treated as "not done", never as success.

Protocol per flow:

1. Before running, check the expected spec file path exists (e.g. `e2e/components/<name>.spec.ts`). If missing → mark planned-action `scaffold+run` and proceed.
2. After scaffolding (or if the file already existed), run the actual command. If it exits 1 with "No tests found" / "0 passed", that means the spec file exists but has no `@<feature>` tag. Status is `no-specs`. STOP and surface the issue at the gate — do not silently mark as `pass`.
3. Any other non-zero exit is a real `fail`.

`--list` exits 0 even on zero matches, so don't gate on `--list`'s exit code.

## Final report (when the loop exits)

Emit on `end` or after flow 5 completes:

```
feature=<name>  flows-walked=<list>  flows-skipped=<list>
  flow1=pass
  flow2=not-applicable (skipped by user)
  flow3=pass
  flow4=force-scaffolded  (was n/a, user overrode)
  flow5=fail              (see <path>:<line>)
verified = passed / (passed + failed + scaffolded + no-specs)

→ Suggested next:
  /tool-kit-test seed         — sibling: shares peer-discovery + piece selection
  /tool-kit-test piece-select — sibling: tested implicitly through leech, deserves direct coverage
```

A feature with zero tagged specs across all applicable flows reports `verified = 0/N` — visible, not silently green.

## When Things Go Sideways

- **Lens disagrees with audit**: print both views in the audit table, pick the safer one (more coverage / redder TDD), and let the user override at the first gate.
- **Scaffold fails (write error, format conflict)**: report `flow <N> verdict = scaffold-error`, file path + error message, then go to the gate. User can `redo` after fixing, or `skip to`.
- **Test flakes (passes once, fails once)**: report `flow <N> verdict = flaky`, attach last 2 run outputs, gate. Do not auto-retry — that's a decision for the user.
- **Sibling list is empty or speculative**: it is OK to say `→ Suggested next: (none — this feature has no obvious sibling under the same contract)`. Don't invent.
- **User replies with a natural-language instruction at the gate**: interpret it. `"actually let's run flow 5 anyway"` ≈ `force`. `"jump to 4"` ≈ `skip to 4`. When ambiguous, ask back in one line, then wait again.

## Constraints (hard)

- Act on **one feature only** — the one from `$ARGUMENTS`.
- NEVER call `npm test`, `npm run test:isomorphic`, `npm run test:browser`, `npm run test:runtime`, `npm run test:playwright`, or `npx playwright test` without a `--grep` or path filter that scopes to this feature.
- NEVER mark `pass` for a flow that was scaffolded but not yet run + verified.
- NEVER edit `playwright.config.ts`, `package.json`, or `.github/workflows/`.
- NEVER proceed past a decision gate without a user reply.
- If the feature can't be resolved, exit early — do not guess.

## Exit

Exit when:
- The user replies `end` at any gate, or
- Flow 5's gate is passed with `continue`, or
- A non-recoverable error occurs (e.g. feature unresolvable, write permission denied).

Always emit the final report on exit. The report includes which flows were walked, which were skipped (with reasons), the verified ratio, and the sibling-feature suggestion list.
