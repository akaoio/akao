#!/usr/bin/env node
// Master skill installer — interactive picker for skills and CLI targets.
// Discovers skill packages automatically from tools/*/*.config.json.
// Zero external dependencies; Node built-ins only.

import fs from "node:fs"
import path from "node:path"
import os from "node:os"
import readline from "node:readline"
import cp from "node:child_process"
import { fileURLToPath } from "node:url"

const __dirname = path.dirname(fileURLToPath(import.meta.url))
const HOME = os.homedir()
const CWD = process.cwd()

// ── CLI targets (always shown — user picks explicitly) ────────────────────────
const CLI_TARGETS = [
    { name: "Claude Code",    dir: path.join(HOME, ".claude", "skills") },
    { name: "Cursor",         dir: path.join(HOME, ".cursor", "skills") },
    { name: "Windsurf",       dir: path.join(CWD, ".windsurf", "skills") },
    { name: "Codex CLI",      dir: path.join(HOME, ".codex", "skills") },
    { name: "GitHub Copilot", dir: path.join(CWD, ".github", "skills") },
]

// ── Auto-discover skill packages from tools/*/*.config.json ──────────────────
function discoverSkills() {
    const skills = []
    for (const entry of fs.readdirSync(__dirname, { withFileTypes: true })) {
        if (!entry.isDirectory()) continue
        const pkgDir = path.join(__dirname, entry.name)
        const cfgFile = fs.readdirSync(pkgDir).find(f => f.endsWith(".config.json"))
        if (!cfgFile) continue
        let cfg
        try {
            cfg = JSON.parse(fs.readFileSync(path.join(pkgDir, cfgFile), "utf8"))
        } catch {
            continue
        }
        const skillDir = cfg.skillDir ?? "skill"
        const skillSrcDir = path.join(pkgDir, skillDir)
        if (!fs.existsSync(skillSrcDir)) {
            console.warn(`  ⚠  ${entry.name}: skillDir '${skillDir}' not found — skipped`)
            continue
        }
        skills.push({
            name: cfg.name ?? entry.name,
            description: cfg.description ?? "",
            version: cfg.version ?? "",
            skillSrcDir,
            hasCustomInstaller: cfg.hasCustomInstaller ?? false,
            pkgDir,
        })
    }
    return skills
}

// ── File utilities ────────────────────────────────────────────────────────────
function copyDir(src, dest) {
    fs.mkdirSync(dest, { recursive: true })
    for (const entry of fs.readdirSync(src, { withFileTypes: true })) {
        const s = path.join(src, entry.name)
        const d = path.join(dest, entry.name)
        if (entry.isDirectory()) copyDir(s, d)
        else fs.copyFileSync(s, d)
    }
}

// ── Selection parser: "1,3" | "a" | "2" → index array ───────────────────────
function parseSelection(input, max) {
    const t = input.trim().toLowerCase()
    if (t === "a" || t === "all") return Array.from({ length: max }, (_, i) => i)
    return t
        .split(/[,\s]+/)
        .map(s => parseInt(s, 10) - 1)
        .filter(i => Number.isFinite(i) && i >= 0 && i < max)
}

// ── Prompt helper ─────────────────────────────────────────────────────────────
function ask(rl, question) {
    return new Promise(resolve => rl.question(question, resolve))
}

// ── Non-interactive flag mode: --skills s1,s2 --targets t1,t2 ────────────────
function parseFlags(skills, targets) {
    const args = process.argv.slice(2)
    const get = flag => {
        const i = args.indexOf(flag)
        return i !== -1 ? args[i + 1] : null
    }
    const sf = get("--skills")
    const tf = get("--targets")
    if (!sf && !tf) return null

    const selectedSkills = sf
        ? sf.split(",").map(n => skills.find(s => s.name === n.trim())).filter(Boolean)
        : skills
    const selectedTargets = tf
        ? tf.split(",").map(n => targets.find(t => t.name.toLowerCase().replace(/\s+/g, "") === n.trim().toLowerCase())).filter(Boolean)
        : null

    if (!selectedTargets) return null // Always show CLI picker

    return { selectedSkills, selectedTargets }
}

// ── Main ──────────────────────────────────────────────────────────────────────
async function main() {
    const skills = discoverSkills()
    if (!skills.length) {
        console.error("\nNo skill packages found. Each tools/<name>/ needs a *.config.json.\n")
        process.exit(1)
    }

    // Non-interactive mode (for CI / scripting)
    const flags = parseFlags(skills, CLI_TARGETS)
    if (flags) {
        return runInstall(flags.selectedSkills, flags.selectedTargets)
    }

    // Interactive mode
    if (!process.stdin.isTTY) {
        console.error("\nNot a TTY. Use flags: --skills <name,...> --targets <name,...>")
        console.error("Available skills:", skills.map(s => s.name).join(", "))
        console.error("Available targets:", CLI_TARGETS.map(t => t.name.replace(/\s+/g, "").toLowerCase()).join(", "))
        process.exit(1)
    }

    const rl = readline.createInterface({ input: process.stdin, output: process.stdout })

    // ── Step 1: Pick skills ───────────────────────────────────────────────────
    console.log("\n╔══════════════════════════════════════╗")
    console.log("║     Skill Installer — Step 1/2       ║")
    console.log("╚══════════════════════════════════════╝\n")
    console.log("Available skills:\n")
    skills.forEach((s, i) => {
        const ver = s.version ? ` v${s.version}` : ""
        console.log(`  [${i + 1}] ${s.name.padEnd(16)} ${s.description}${ver}`)
    })
    console.log("  [a] All skills\n")
    const si = parseSelection(await ask(rl, "Select skills to install: "), skills.length)
    if (!si.length) { console.error("\nNo valid selection. Aborting.\n"); rl.close(); process.exit(1) }
    const selectedSkills = si.map(i => skills[i])

    // ── Step 2: Pick CLI targets ──────────────────────────────────────────────
    console.log("\n╔══════════════════════════════════════╗")
    console.log("║     Skill Installer — Step 2/2       ║")
    console.log("╚══════════════════════════════════════╝\n")
    console.log("Available CLI targets:\n")
    CLI_TARGETS.forEach((t, i) => {
        const display = t.dir.replace(HOME, "~").replace(CWD, ".")
        console.log(`  [${i + 1}] ${t.name.padEnd(16)} ${display}`)
    })
    console.log("  [a] All targets\n")
    const ci = parseSelection(await ask(rl, "Select CLI targets: "), CLI_TARGETS.length)
    if (!ci.length) { console.error("\nNo valid selection. Aborting.\n"); rl.close(); process.exit(1) }
    const selectedTargets = ci.map(i => CLI_TARGETS[i])

    rl.close()
    runInstall(selectedSkills, selectedTargets)
}

function runInstall(selectedSkills, selectedTargets) {
    console.log("\nInstalling...\n")
    let ok = 0, fail = 0
    for (const skill of selectedSkills) {
        if (skill.hasCustomInstaller) {
            // Delegate to the package's own install.js
            const customInstaller = path.join(skill.pkgDir, "install.js")
            try {
                cp.execSync(`node "${customInstaller}"`, { stdio: "inherit" })
                console.log(`  ✓  ${skill.name.padEnd(16)} (custom installer)`)
                ok++
            } catch (e) {
                console.log(`  ✗  ${skill.name.padEnd(16)} (custom installer failed)`)
                fail++
            }
            continue
        }
        for (const target of selectedTargets) {
            const dest = path.join(target.dir, skill.name)
            try {
                copyDir(skill.skillSrcDir, dest)
                console.log(`  ✓  ${skill.name.padEnd(16)} → ${target.name}`)
                ok++
            } catch (e) {
                console.log(`  ✗  ${skill.name.padEnd(16)} → ${target.name}  (${e.message})`)
                fail++
            }
        }
    }
    const total = ok + fail
    console.log(`\n${ok}/${total} installed.${fail ? ` ${fail} failed.` : ""}\n`)
    process.exit(fail ? 1 : 0)
}

main()
