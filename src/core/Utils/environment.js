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

const DEV = BROWSER && (
    globalThis?._dev === true
    || globalThis?._dev?.enabled === true
    || globalThis?.location?.hostname === "localhost"
    || globalThis?.location?.hostname === "127.0.0.1"
)

export { NODE, BROWSER, WIN, DEV }
