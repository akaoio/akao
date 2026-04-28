#!/usr/bin/env node
// tool-kit installer.
// Copies tools/tool-kit/skills/tool-kit-*.md → ~/.claude/skills/tool-kit-*/SKILL.md
// (and equivalent locations for other CLIs). Idempotent.
//
// Usage:
//   node tools/tool-kit/install.js                        # interactive picker
//   node tools/tool-kit/install.js --targets claude       # non-interactive
//   node tools/tool-kit/install.js --targets all

import { existsSync, mkdirSync, readdirSync, readFileSync, writeFileSync, rmSync } from "node:fs"
import { homedir } from "node:os"
import { join, dirname } from "node:path"
import { fileURLToPath } from "node:url"
import readline from "node:readline"

const HERE = dirname(fileURLToPath(import.meta.url))
const SKILLS_DIR = join(HERE, "skills")
const HOME = homedir()
const CWD = process.cwd()

// ── Targets ───────────────────────────────────────────────────────────────────

const CLI_TARGETS = [
    { id: "claude",   name: "Claude Code",    dir: join(HOME, ".claude", "skills"), mode: "claude" },
    { id: "cursor",   name: "Cursor",         dir: join(HOME, ".cursor", "skills"), mode: "flat" },
    { id: "codex",    name: "Codex CLI",      dir: join(HOME, ".codex",  "skills"), mode: "claude" },
    { id: "windsurf", name: "Windsurf",       dir: join(CWD,  ".windsurf", "skills"), mode: "flat" },
    { id: "copilot",  name: "GitHub Copilot", dir: join(CWD,  ".github", "skills"), mode: "flat" }
]

const SKILLS = ["tool-kit-test", "tool-kit-attack"]

// ── Install logic ─────────────────────────────────────────────────────────────

function ensureDir(d) { mkdirSync(d, { recursive: true }) }

function installToClaude(targetRoot) {
    // Claude Code convention: each skill lives at ~/.claude/skills/<name>/SKILL.md
    for (const skill of SKILLS) {
        const src = join(SKILLS_DIR, `${skill}.md`)
        if (!existsSync(src)) throw new Error(`source missing: ${src}`)
        const targetDir = join(targetRoot, skill)
        if (existsSync(targetDir)) rmSync(targetDir, { recursive: true, force: true })
        ensureDir(targetDir)
        writeFileSync(join(targetDir, "SKILL.md"), readFileSync(src, "utf8"), "utf8")
    }
}

function installFlat(targetRoot) {
    // Single flat dir for CLIs without per-skill convention.
    const dir = join(targetRoot, "tool-kit")
    if (existsSync(dir)) rmSync(dir, { recursive: true, force: true })
    ensureDir(dir)
    for (const skill of SKILLS) {
        const src = join(SKILLS_DIR, `${skill}.md`)
        writeFileSync(join(dir, `${skill}.md`), readFileSync(src, "utf8"), "utf8")
    }
    // Bundle personas so skills have checklists on-disk. (No features.json — tool-kit is stateless.)
    ensureDir(join(dir, "personas"))
    for (const f of readdirSync(join(HERE, "personas"))) {
        writeFileSync(join(dir, "personas", f), readFileSync(join(HERE, "personas", f), "utf8"), "utf8")
    }
}

function runInstall(targets) {
    let ok = 0, fail = 0
    process.stdout.write("\nInstalling tool-kit skills...\n\n")
    for (const t of targets) {
        try {
            if (t.mode === "claude") installToClaude(t.dir)
            else installFlat(t.dir)
            const path = t.dir.replace(HOME, "~").replace(CWD, ".")
            process.stdout.write(`  ✓  ${t.name.padEnd(16)} → ${path}\n`)
            ok++
        } catch (e) {
            process.stdout.write(`  ✗  ${t.name.padEnd(16)} (${e.message})\n`)
            fail++
        }
    }
    process.stdout.write(`\n${ok}/${ok + fail} installed.${fail ? ` ${fail} failed.` : ""}\n\n`)
    process.exit(fail ? 1 : 0)
}

// ── Args ──────────────────────────────────────────────────────────────────────

function parseFlagTargets() {
    const args = process.argv.slice(2)
    const i = args.indexOf("--targets")
    if (i === -1) return null
    const csv = args[i + 1]
    if (!csv) return null
    if (csv === "all") return CLI_TARGETS
    return csv.split(",").map(s => CLI_TARGETS.find(t => t.id === s.trim())).filter(Boolean)
}

function ask(rl, q) { return new Promise(r => rl.question(q, r)) }

function parseSelection(input, max) {
    const t = input.trim().toLowerCase()
    if (t === "a" || t === "all") return Array.from({ length: max }, (_, i) => i)
    return t.split(/[,\s]+/).map(s => parseInt(s, 10) - 1).filter(i => Number.isFinite(i) && i >= 0 && i < max)
}

// ── Main ──────────────────────────────────────────────────────────────────────

async function main() {
    if (!existsSync(SKILLS_DIR)) {
        process.stderr.write(`install.js: skills/ not found at ${SKILLS_DIR}\n`)
        process.exit(1)
    }

    const flagTargets = parseFlagTargets()
    if (flagTargets) return runInstall(flagTargets)

    if (!process.stdin.isTTY) {
        process.stderr.write("Not a TTY. Use --targets <ids,...> or --targets all\n")
        process.stderr.write(`Available ids: ${CLI_TARGETS.map(t => t.id).join(", ")}\n`)
        process.exit(1)
    }

    const rl = readline.createInterface({ input: process.stdin, output: process.stdout })

    process.stdout.write("\n╔════════════════════════════════════════╗\n")
    process.stdout.write("║   tool-kit installer  v1.0             ║\n")
    process.stdout.write("╚════════════════════════════════════════╝\n\n")
    process.stdout.write("Select CLI targets to install tool-kit skills:\n\n")
    CLI_TARGETS.forEach((t, i) => {
        const path = t.dir.replace(HOME, "~").replace(CWD, ".")
        process.stdout.write(`  [${i + 1}] ${t.name.padEnd(16)} ${path}\n`)
    })
    process.stdout.write("  [a] All targets\n\n")

    const selection = parseSelection(await ask(rl, "Select targets: "), CLI_TARGETS.length)
    rl.close()
    if (!selection.length) {
        process.stderr.write("\nNo valid selection. Aborting.\n\n")
        process.exit(1)
    }
    runInstall(selection.map(i => CLI_TARGETS[i]))
}

main()
