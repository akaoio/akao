---
name: tool-kit-attack
description: "Hunt vulnerabilities on ONE feature. Spawns applicable security personas (Alex/Rex/Vera/Cora/Pax) as parallel subagents (party-mode pattern) for independent threat-model perspectives, generates attack specs (passing test = exploit confirmed), runs them, then emits a plan-mode report — diff snippet + decision prompt — and waits for user approval before touching source. Args: <feature-name> [--solo]"
---

# tool-kit-attack — Feature-scoped vuln hunt with plan-mode reporting

This skill takes a single feature name, picks attack classes by reading the feature's source, writes/refreshes attack specs against the built artifact, runs them, and presents findings in a plan-mode block. **Source files are never modified before the user approves a fix.**

## Arguments

- `$ARGUMENTS` — must include exactly one feature name. Optional flag:
  - `--solo` — skip the persona-consultation step. The orchestrator alone reads persona files and infers vectors directly. Default is consult mode (parallel subagents). Use `--solo` when subagents are unavailable, when speed beats perspective diversity, or when re-running on a feature that's already been broadly characterized.

## On invocation

1. **Resolve the feature** (same rules as `tool-kit-test`): route under `src/UI/routes/<name>/` or core module under `src/core/<name>(/|.js)`. Stop with a clear message if neither exists.

2. **Read project context**: `README.md`, any relevant `docs/**/*.md` you find by grepping for the feature name, the feature source. Newer docs win over stale ones.

3. **Pick applicable personas** using the signal table below. Alex and Rex always activate (generalist + adversarial baseline); the others are conditional on feature surface.

4. **Persona consultation** (default — skip with `--solo`). Spawn each picked persona as an independent parallel subagent via the Agent tool. Each subagent thinks independently from its persona checklist; the orchestrator aggregates their findings. Pattern lifted from `bmad-party-mode` so opinions don't converge under a single LLM. Spec below in **Persona spawn protocol**.

5. **Write or refresh attack specs** at `e2e/attacks/<feature>-<vector>.spec.ts` based on the aggregated vectors. Each spec is a Playwright test where **passing means the exploit succeeded** (vulnerability confirmed) and **failing means the defense held**. Spec header includes CWE, MITRE ATT&CK ID, severity, threat model. Tag the describe block `@flow-attack @<feature>`.

6. **Run the attacks** (only this feature's specs):

   ```
   npx playwright test --project=chromium --grep "@<feature>" e2e/attacks/
   ```

   Zero-match guard: if no spec matches, report `no-vectors-applicable` cleanly — never silent-pass.

7. **Emit the plan-mode block** (format below), grouping findings by persona when consultation ran. **Do NOT edit any source file at this stage.**

8. **Wait for the user's reply.** Then act per the decision protocol.

## Persona spawn protocol (consult mode)

For each persona picked in step 3, spawn one subagent in parallel (all Agent calls in a single response). Each subagent gets:

```
You are <persona-displayName> (<persona-title>), an adversarial reviewer in a vulnerability hunt.

## Your Persona
{full contents of tools/tool-kit/personas/<persona>.md}

## Feature Under Attack
- Name: <feature>
- Source: <path/to/source/files>
- Surface signals detected: <which signals matched>

## Your Task
Walk your checklist against this feature only. For each vector that applies:
- Vector name (kebab-case, e.g. slippage-unclamped)
- One-sentence threat model
- Severity (critical/high/medium/low)
- CWE id
- MITRE ATT&CK id
- Source file:line of the suspect code (cite the exact location)

Output a Markdown bullet list — one bullet per vector. No prose preamble, no caveats.

## Guidelines
- Stay scoped to vectors your persona owns. If a vector belongs to another persona, skip it.
- Cite the line in your persona MD that maps to each vector you raise.
- Do NOT use tools — you are advising, not executing.
- If nothing applies from your checklist, say "no applicable vectors" in one line.
```

After all personas return, **deduplicate** across responses (same vector raised by multiple personas → keep one entry, list both as sources). Pass the merged set to step 5.

`--solo` mode: skip the spawn. The orchestrator reads each picked persona MD and infers vectors directly in a single pass. Faster, less diverse.

## Persona selection (signal table)

| Persona | File | Vector domain | Activate when |
|---|---|---|---|
| 🔐 Alex (Security Auditor) | `tools/tool-kit/personas/alex.md` | OWASP Top 10, CWE Top 25 — generalist baseline | always |
| ⚔️ Rex (Pentester) | `tools/tool-kit/personas/rex.md` | Adversarial: input abuse, state races, auth bypass | always |
| ⛓️ Vera (Web3 Auditor) | `tools/tool-kit/personas/vera.md` | DeFi: slippage, deadline, quote, encoding, chain, approval | source contains `estimateGas`, `Wallets[`, `slippage`, `SendLogic`, `swap`, `withdraw`, `deposit` |
| 😵 Cora (Confused User) | `tools/tool-kit/personas/cora.md` | Usability-induced loss: paste, double-tap, locale, back-button | money flow + user input (address, amount, currency) |
| ⚡ Pax (Performance Hawk) | `tools/tool-kit/personas/pax.md` | Resource exhaustion, DoS vectors | heavy rendering, async work, large dependencies |

A feature can match multiple personas — that's expected. Each persona contributes from its own domain; the consultation step (step 4) is what makes their thinking independent rather than a single LLM converging on consensus.

## Spec shape (each `e2e/attacks/<feature>-<vector>.spec.ts`)

Header (mandatory):

```ts
// Vector: <vector-name>
//
// Feature: <feature>
// Target:  /en/<feature> → <component-tag>
//
// Threat model:
//   Actor:      <who>
//   Capability: <what they can do>
//   Goal:       <what they're trying to break>
//
// CWE:    CWE-<NN>
// MITRE:  T<NNNN>
// Severity: <critical | high | medium | low>
//
// Status semantics:
//   Test passes → exploit succeeded → vulnerability confirmed → fix needed
//   Test fails  → defense held
```

Then `test.describe("@flow-attack @<feature> <vector-name>", () => { … })` with assertions that **pass when exploited**.

## Assertion patterns per vector class

"Passing test = exploit succeeded" is the contract. To stay reproducible across personas, each spec MUST use one of the canonical assertion shapes below for its vector class. If a vector does not match any class, name a new one in the spec header and reference it in this table on the next edit.

| Vector class | Canonical "exploit succeeded" assertion | Notes |
|---|---|---|
| **XSS (reflected/DOM/stored)** | `await expect.poll(() => alertFired).toBe(true)` after registering `page.on("dialog", d => { alertFired = true; d.dismiss() })` | Inject the canary `<img src=x onerror=alert(1)>` (or equivalent) via the user-controlled surface; `alertFired === true` proves arbitrary script ran |
| **CSRF / state-changing GET** | `expect(response.status()).toBe(200)` after a cross-origin form-POST/`fetch` from `evil.local`; assert the side-effect (e.g. balance changed, route navigated) | The fact that the request succeeded *without a token check* is the exploit — assert on the persisted side-effect, not the network call alone |
| **Open redirect / URL injection** | `expect(page.url()).toMatch(/^https?:\/\/(?!shop-domain\.local)/)` after triggering the redirect surface | Domain allowlist must be observable in test |
| **IDOR / authz bypass** | `expect(response.status()).not.toBe(403); expect(await response.json()).toMatchObject({ owner: "victim" })` | Logging in as attacker, fetching victim's resource by id |
| **Prototype pollution** | `expect((Object.prototype as any).polluted).toBe("yes")` after the input flows through merge/clone | Pollution must be observable on a fresh `{}` or via a route that reads the polluted key |
| **Slippage / amount unclamped (web3)** | `expect(quotedOut - actualOut).toBeGreaterThan(quotedOut * 0.05)` after a sandwich-style fill at the contract surface | Threshold = the slippage cap the UI claims; attack is "did the user receive less than promised cap allows?" |
| **Deadline / expiry unenforced (web3)** | `expect(txReceipt.status).toBe(1)` after submitting with `deadline = 1` (epoch in the past) | A successful tx with stale deadline = no enforcement |
| **Approval over-grant (web3)** | `expect(allowance).toBe(MAX_UINT256)` after the UI's "approve" CTA fires | UI silently approving infinite when user expected a per-tx amount |
| **Locale / number-format confusion (Cora)** | `expect(submittedAmount).not.toBe(typedAmount)` where `typedAmount = "1,5"` (EU comma) and `submittedAmount` is what hit the wire | Locale parsing mismatch confirmed |
| **Double-submit / replay (Cora)** | `expect(receipts.length).toBeGreaterThan(1)` after rapid double-tap or back-button → resubmit | Two charges from one user intent |
| **Resource exhaustion / unbounded loop (Pax)** | `expect(durationMs).toBeGreaterThan(10_000)` against a small malicious payload (e.g. nested JSON of depth 20, regex with catastrophic backtracking) | Time-based, cap matches realistic SLO |
| **Bundle/dep DoS (Pax)** | `expect(transferredBytes).toBeGreaterThan(BUDGET_BYTES)` for a single route load with adversarial input | Budget = the limit the team committed to |

Spec authors: copy the assertion shape verbatim where possible. Personas (Alex/Rex/Vera/Cora/Pax) propose the vector + threat model; this table is what turns the proposal into a test that anyone can re-run and get the same verdict.

## Plan-mode report block (the gate)

After the attack run, emit exactly this shape — nothing else, no source edits:

```markdown
## Attack Plan: <feature>

**Surface scanned:** <classes>
**Vectors run:** <N>   **Confirmed vulnerabilities:** <K>   **Defenses held:** <N − K>

---

### Finding 1 — <vector-name> (severity: HIGH)

- **Surfaced by:** <persona-name> (or `<persona-A> + <persona-B>` if multiple converged)
- **CWE:** CWE-<NN>  •  **MITRE:** T<NNNN>
- **Spec:** `e2e/attacks/<feature>-<vector>.spec.ts:<LINE>` (passed → exploit succeeded)
- **Source:** `src/path/to/file.js:<LINE>`
- **Threat:** <one-sentence threat model>

**Proposed fix:**
\`\`\`diff
- <current code>
+ <patched code>
\`\`\`

**Why this fix:** <one short paragraph — what it prevents, why these bounds, which assertions in the attack spec will flip from pass→fail after the fix>.

---

### Finding 2 — …

---

## Awaiting decision

Reply one of:
- **`apply all`** — I write all proposed fixes.
- **`apply 1, 3`** — I write only the listed findings.
- **`skip`** — I write nothing; you'll handle it manually.
- **`explain N`** — I expand on finding N before you decide.

I will not modify source files until you reply.
```

## After the user replies

- **`apply all`** or **`apply <list>`** → for each approved finding:
  1. **Freeze the spec.** Record the exact bytes / sha256 of `e2e/attacks/<feature>-<vector>.spec.ts` *before* applying the diff. The spec is the test of record for this finding — it MUST NOT be modified during the apply step. If the spec needs to change after a finding is approved, that is a separate, explicit "amend spec" decision the user must make first.
  2. Apply the diff to the source file(s) exactly as shown in the plan. Source edits only — never co-edit the spec in the same step.
  3. Re-verify the spec is byte-identical to the frozen capture. If it changed (e.g. by autoformatter, hook, or accidental edit), abort the apply for this finding, restore the source file, and report a `spec-mutated` error. The user must re-approve before continuing.
  4. Re-run the corresponding attack spec: `npx playwright test --project=chromium e2e/attacks/<feature>-<vector>.spec.ts`.
  5. The spec MUST flip from pass → fail (defense now holds). If it doesn't flip, revert the diff and report the unexpected result — do not proceed to the next finding.
- **`skip`** → exit. No source edits.
- **`explain <N>`** → expand finding N (mechanism, impact estimate, alternative fixes), then re-prompt for decision.

## Constraints (hard)

- Act on **one feature only** — the one from `$ARGUMENTS`.
- NEVER auto-fix without an explicit `apply …` reply from the user.
- NEVER call `npm test`, `npm run test:isomorphic`, or any unfiltered Playwright invocation.
- NEVER edit `src/`, `playwright.config.ts`, `package.json`, or `.github/workflows/` before approval.
- NEVER edit `e2e/attacks/<feature>-<vector>.spec.ts` during the apply step. The spec is frozen between user approval and re-run; mutating it would invalidate the pass→fail flip as evidence that the source fix worked.
- Findings reference this codebase only — no generic OWASP pasta.
- Specs target the built artifact via `node preview.js` (`webServer` boots it via `playwright.config.ts`).

## Done when

- Every applicable vector has a spec at `e2e/attacks/<feature>-<vector>.spec.ts`.
- The plan-mode block was emitted.
- Either: (a) user replied `skip` and the skill exited, or (b) approved findings were applied + re-run results reported (each spec flipped pass → fail), or (c) `explain N` cycle resolved before exit.
