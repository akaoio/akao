let NODE = false
let BROWSER = false
let WIN = false

if (globalThis?.process?.versions?.node) {
    NODE = true
    BROWSER = !NODE
}

if (globalThis?.location?.origin) {
    BROWSER = true
    NODE = !BROWSER
}

if (globalThis?.process?.platform === "win32") WIN = true

const DEV = BROWSER && globalThis?.location?.hostname === "localhost"

export { NODE, BROWSER, WIN, DEV }
