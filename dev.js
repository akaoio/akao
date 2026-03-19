import server from "live-server"
import chokidar from "chokidar"
import { spawn } from "child_process"

// Watch for changes and rebuild
let buildInProgress = false

const watcher = chokidar.watch("src/**/*", {
    ignored: /(^|[\/\\])\../,
    persistent: true
})

// Patterns that require build:crypto in addition to build:core
const cryptoPaths = /src[/\\]statics[/\\](chains|ABIs)[/\\]/

watcher.on("change", (path) => {
    if (buildInProgress) {
        console.log(`⏳ Build in progress, queuing change: ${path}`)
        return
    }

    const needsCrypto = cryptoPaths.test(path)
    console.log(`🔨 File changed: ${path}, rebuilding${needsCrypto ? " (core + crypto)" : ""}...`)
    buildInProgress = true

    const runBuild = (script, next) => {
        const build = spawn("npm", ["run", script], { stdio: "inherit", shell: true })
        build.on("close", (code) => {
            if (code !== 0) {
                console.error(`❌ ${script} failed with code ${code}`)
                buildInProgress = false
                return
            }
            if (next) next()
            else {
                buildInProgress = false
                console.log("✅ Build completed")
            }
        })
    }

    if (needsCrypto) {
        runBuild("build:crypto", () => runBuild("build:core", null))
    } else {
        runBuild("build:core", null)
    }
})

// Start live-server with file watcher
console.log("🚀 Starting development server with auto-rebuild...")

server.start({
    port: 8080,
    host: "localhost",
    root: "build",
    open: true,
    wait: 1000,
    logLevel: 2,
    ignore: "build/geo" // Ignore geo folder to prevent file watcher limit
})

console.log("✅ Server running on http://localhost:8080")
console.log("👀 Watching src/ for changes...")