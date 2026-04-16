export function detectEnvironment(scope = globalThis) {
    const NODE = !!scope?.process?.versions?.node
    const BROWSER = !NODE && !!scope?.location?.origin
    const WIN = scope?.process?.platform === "win32"
    const DEV = BROWSER && (
        scope?._dev === true
        || scope?._dev?.enabled === true
        || scope?.location?.hostname === "localhost"
        || scope?.location?.hostname === "127.0.0.1"
    )

    return { NODE, BROWSER, WIN, DEV }
}

const { NODE, BROWSER, WIN, DEV } = detectEnvironment()

export { NODE, BROWSER, WIN, DEV }
