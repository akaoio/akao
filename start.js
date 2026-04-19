import { spawn } from "child_process"

const args = process.argv.slice(2)
const headless = args.includes("--headless") || ["1", "true", "yes", "on"].includes((process.env.npm_config_headless || "").toLowerCase())
const npm = process.platform === "win32" ? "npm.cmd" : "npm"

function run(cmd, argv, options = {}) {
    return new Promise((resolve, reject) => {
        const child = spawn(cmd, argv, {
            stdio: "inherit",
            shell: process.platform === "win32",
            ...options
        })

        child.on("close", (code) => {
            if (code === 0) return resolve()
            reject(new Error(`${cmd} ${argv.join(" ")} exited with code ${code}`))
        })

        child.on("error", reject)
    })
}

function capture(cmd, argv) {
    return new Promise((resolve, reject) => {
        const child = spawn(cmd, argv, {
            stdio: ["ignore", "pipe", "pipe"],
            shell: false
        })

        let stdout = ""
        let stderr = ""
        child.stdout.on("data", (chunk) => stdout += chunk)
        child.stderr.on("data", (chunk) => stderr += chunk)

        child.on("close", (code) => {
            if (code === 0) return resolve(stdout)
            reject(new Error(`${cmd} ${argv.join(" ")} exited with code ${code}\n${stderr}`))
        })

        child.on("error", reject)
    })
}

async function listPortPids(port) {
    if (process.platform !== "linux") return []

    try {
        const output = await capture("ss", ["-ltnpH"])
        const lines = output.split("\n").filter((line) => new RegExp(`:${port}\\b`).test(line))
        const pids = new Set()

        for (const line of lines) {
            for (const match of line.matchAll(/pid=(\d+)/g)) pids.add(Number(match[1]))
        }

        return Array.from(pids)
    } catch {
        return []
    }
}

async function readArgs(pid) {
    if (process.platform !== "linux") return ""

    try {
        return (await capture("ps", ["-p", String(pid), "-o", "args="])).trim()
    } catch {
        return ""
    }
}

async function killMatchingPortProcesses(port, pattern) {
    const pids = await listPortPids(port)
    const killed = []

    for (const pid of pids) {
        const command = await readArgs(pid)
        if (!pattern.test(command)) continue

        process.kill(pid, "SIGTERM")
        killed.push(pid)
    }

    return killed
}

async function waitForPortsToClose(ports, timeout = 10000) {
    const start = Date.now()
    while (Date.now() - start < timeout) {
        const open = await Promise.all(ports.map((port) => listPortPids(port)))
        if (open.every((pids) => pids.length === 0)) return
        await new Promise((resolve) => setTimeout(resolve, 200))
    }

    throw new Error(`Timed out waiting for ports ${ports.join(", ")} to close`)
}

async function restartDevStack() {
    const devPids = await killMatchingPortProcesses(8080, /(?:^|\s)(?:node|bun)\s+.*\bdev\.js(?:\s|$)/)
    const marketPids = await killMatchingPortProcesses(8420, /(?:^|\s)(?:node|bun)\s+.*\bmarket\.js(?:\s|$)/)

    const stopped = [...new Set([...devPids, ...marketPids])]
    if (!stopped.length) return

    console.log(`Stopped existing dev processes: ${stopped.join(", ")}`)
    await waitForPortsToClose([8080, 8420])
}

function hold() {
    const timer = setInterval(() => {}, 1 << 30)
    const stop = () => {
        clearInterval(timer)
        process.exit(0)
    }

    process.on("SIGINT", stop)
    process.on("SIGTERM", stop)
}

async function start() {
    if (!headless) {
        await restartDevStack()
        await run(npm, ["run", "start:dev"])
        return
    }

    await run("node", ["build.js", "crypto"])
    await run("node", ["build.js", "core", "--dev"])

    globalThis._autolaunch = false
    const { launch } = await import(new URL("./build/core/Launcher.js", import.meta.url).href)
    const state = await launch({ mode: "headless" })

    console.log(`Headless runtime ready (${state.mode})`)
    hold()
}

start().catch((error) => {
    console.error(error?.stack || error)
    process.exit(1)
})
