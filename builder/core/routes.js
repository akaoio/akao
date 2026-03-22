import { write } from "../../src/core/FS.js"
import { paths } from "./config.js"

// ============ Routes Generation ============

function normalizeRoute(route = "") {
    return String(route || "").replace(/^\/+|\/+$/g, "")
}

function isDynamicSegment(segment = "") {
    return /^\[(?:\.\.\.)?[^\]]+\]$|^\[\[\.\.\.[^\]]+\]\]$/.test(segment)
}

function getParamName(segment = "") {
    if (segment.startsWith("[[...") && segment.endsWith("]]")) return segment.slice(5, -2)
    if (segment.startsWith("[...") && segment.endsWith("]")) return segment.slice(4, -1)
    if (segment.startsWith("[") && segment.endsWith("]")) return segment.slice(1, -1)
    return ""
}

function getParamValues(name = "", { items = [], tags = [], games = [] } = {}) {
    const values = {
        item: items,
        tag: tags,
        game: games
    }
    return values[name] || []
}

function expandRouteSegments(route = "", options = {}) {
    const segments = normalizeRoute(route).split("/").filter(Boolean)
    if (!segments.length) return [[]]

    let expanded = [[]]
    for (const segment of segments) {
        if (!isDynamicSegment(segment)) {
            expanded = expanded.map((parts) => [...parts, segment])
            continue
        }

        const name = getParamName(segment)
        const values = getParamValues(name, options)

        if (!values.length) return []

        expanded = expanded.flatMap((parts) => values.map((value) => [...parts, String(value)]))
    }

    return expanded
}

export async function generateRoutes(locales, items, tags, games, indexContent, outputBase = "build", routePatterns = []) {
    if (typeof outputBase !== "string" || !outputBase.trim()) {
        throw new TypeError(`generateRoutes expected outputBase to be a non-empty string, got: ${JSON.stringify(outputBase)}`)
    }

    if (typeof indexContent !== "string") {
        throw new TypeError(`generateRoutes expected indexContent to be a string, got: ${typeof indexContent}`)
    }

    const dynamicPaths = outputBase === "build" ? paths : {
        ...paths,
        build: {
            ...paths.build,
            root: [outputBase]
        }
    }

    const normalizedRoutes = Array.from(new Set(routePatterns.map(normalizeRoute).filter(Boolean))).sort()
    const tagList = Array.from(tags || [])
    const gameList = Array.from(games || [])
    const routeTargets = new Set()
    routeTargets.add([...dynamicPaths.build.root, "index.html"].join("/"))

    for (const locale of locales) {
        routeTargets.add([...dynamicPaths.build.root, locale, "index.html"].join("/"))
        for (const route of normalizedRoutes) {
            if (route === "home") continue

            const parts = route.split("/").filter(Boolean)
            const staticPrefix = []

            for (const segment of parts) {
                if (isDynamicSegment(segment)) break
                staticPrefix.push(segment)
            }

            if (staticPrefix.length) {
                routeTargets.add([...dynamicPaths.build.root, locale, ...staticPrefix, "index.html"].join("/"))
            }

            const expanded = expandRouteSegments(route, { items, tags: tagList, games: gameList })
            if (!expanded.length) {
                if (parts.some(isDynamicSegment)) console.warn(`Skipped dynamic route '${route}' because no values were found`)
                continue
            }

            for (const segments of expanded) {
                routeTargets.add([...dynamicPaths.build.root, locale, ...segments, "index.html"].join("/"))
            }
        }
    }

    for (const route of routeTargets) await write(route.split("/"), indexContent)

    return routeTargets.size
}
