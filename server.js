import server from "live-server"
import chokidar from "chokidar"
import { spawn } from "child_process"

// Watch for changes and rebuild
let buildInProgress = false

const watcher = chokidar.watch("src/**/*", {
    ignored: /(^|[\/\\])\../,
    persistent: true
})

watcher.on("change", (path) => {
    if (buildInProgress) {
        console.log(`⏳ Build in progress, queuing change: ${path}`)
        return
    }
    
    console.log(`🔨 File changed: ${path}, rebuilding...`)
    buildInProgress = true
    
    const build = spawn("npm", ["run", "build"], { stdio: "inherit" })
    build.on("close", (code) => {
        buildInProgress = false
        if (code === 0) {
            console.log("✅ Build completed")
        } else {
            console.error(`❌ Build failed with code ${code}`)
        }
    })
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