// Import environment detection flags to determine runtime context
import { NODE, BROWSER, WIN } from "../Utils/environment.js"

// Lazy-loaded modules that are only available in Node.js environment
let fs = null
let YAML = null

// Dynamically import Node.js-specific modules when running in Node.js
if (NODE) {
    fs = await import("fs")
    YAML = await import("yaml")
}

// Browser OPFS singleton — lazy-created once for all FS submodules
let opfs = null
if (BROWSER) {
    const { OPFS } = await import("../OPFS.js")
    opfs = new OPFS()
}

export { fs, YAML, NODE, BROWSER, WIN, opfs }
