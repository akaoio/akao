#!/usr/bin/env node
// tool-kit uninstaller. Removes every artifact install.js writes.

import { existsSync, rmSync } from "node:fs"
import { homedir } from "node:os"
import { join } from "node:path"

const HOME = homedir()
const CWD = process.cwd()

const SKILLS = ["tool-kit-test", "tool-kit-attack"]
const LEGACY_SKILLS = ["duke-test", "duke-attack"]

const TARGETS = [
    // Claude Code: per-skill dirs
    ...SKILLS.map(s => ({ tool: `Claude Code/${s}`,  path: join(HOME, ".claude", "skills", s) })),
    // Codex also uses per-skill dirs with SKILL.md
    ...SKILLS.map(s => ({ tool: `Codex CLI/${s}`, path: join(HOME, ".codex", "skills", s) })),
    // Legacy cleanup: prior duke-test / duke-attack per-skill dirs (renamed to tool-kit-*)
    ...LEGACY_SKILLS.flatMap(s => [
        { tool: `Claude Code/${s} (legacy)`, path: join(HOME, ".claude", "skills", s) },
        { tool: `Codex CLI/${s} (legacy)`,   path: join(HOME, ".codex",  "skills", s) }
    ]),
    // Flat-mode CLIs: single tool-kit dir
    { tool: "Cursor",         path: join(HOME, ".cursor",   "skills", "tool-kit") },
    { tool: "Windsurf",       path: join(CWD,  ".windsurf", "skills", "tool-kit") },
    { tool: "GitHub Copilot", path: join(CWD,  ".github",   "skills", "tool-kit") },
    // Legacy cleanup: older Codex flat bundle
    { tool: "Codex CLI (legacy flat)", path: join(HOME, ".codex", "skills", "tool-kit") },
    // Legacy cleanup: prior duke-kit flat bundles
    { tool: "Cursor (legacy duke-kit)",         path: join(HOME, ".cursor",   "skills", "duke-kit") },
    { tool: "Windsurf (legacy duke-kit)",       path: join(CWD,  ".windsurf", "skills", "duke-kit") },
    { tool: "GitHub Copilot (legacy duke-kit)", path: join(CWD,  ".github",   "skills", "duke-kit") },
    { tool: "Codex CLI (legacy duke-kit flat)", path: join(HOME, ".codex",    "skills", "duke-kit") },
    // Legacy cleanup: prior duke-pipeline flat bundles
    { tool: "Cursor (legacy duke-pipeline)",         path: join(HOME, ".cursor",   "skills", "duke-pipeline") },
    { tool: "Windsurf (legacy duke-pipeline)",       path: join(CWD,  ".windsurf", "skills", "duke-pipeline") },
    { tool: "GitHub Copilot (legacy duke-pipeline)", path: join(CWD,  ".github",   "skills", "duke-pipeline") },
    { tool: "Codex CLI (legacy duke-pipeline flat)", path: join(HOME, ".codex",    "skills", "duke-pipeline") },
    // Legacy cleanup: remove old `pipeline-*` skill dirs from previous version
    ...["pipeline", "pipeline-build", "pipeline-test", "pipeline-review", "pipeline-attack", "pipeline-ship", "pipeline-docs"]
        .map(s => ({ tool: `Claude Code/${s} (legacy)`, path: join(HOME, ".claude", "skills", s) })),
    // Legacy cleanup: remove old duke-preview / duke-qa / duke-review (superseded by 2-skill model)
    ...["duke-preview", "duke-qa", "duke-review"].flatMap(s => [
        { tool: `Claude Code/${s} (legacy)`, path: join(HOME, ".claude", "skills", s) },
        { tool: `Codex CLI/${s} (legacy)`,   path: join(HOME, ".codex",  "skills", s) }
    ])
]

let ok = 0, skipped = 0
process.stdout.write("\ntool-kit uninstall:\n\n")
for (const t of TARGETS) {
    if (!existsSync(t.path)) {
        process.stdout.write(`  –  ${t.tool.padEnd(36)} (not present)\n`)
        skipped++
        continue
    }
    rmSync(t.path, { recursive: true, force: true })
    process.stdout.write(`  ✓  ${t.tool.padEnd(36)} → ${t.path.replace(HOME, "~").replace(CWD, ".")}\n`)
    ok++
}
process.stdout.write(`\n${ok} removed, ${skipped} not present.\n\n`)
