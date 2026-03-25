import { exist } from "../src/core/FS.js"
import { paths } from "./core/config.js"
import { log } from "./core/logger.js"
import { generateHashFiles } from "./core/hash.js"

log.start("Starting hash generation...")

const buildRoot = paths.build.root

if (!(await exist([buildRoot]))) {
    log.info("Build directory not found — run build:core first")
    process.exit(1)
}

// Hash entire build directory, excluding geo (geo has its own hash structure)
log.info("Generating hash files for build directory...")
const result = await generateHashFiles(buildRoot, ["geo"])
log.ok(`Generated ${result.hashFiles} hash files`)

log.start("Hash generation completed!")
