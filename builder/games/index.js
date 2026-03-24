import path from "node:path"
import fsNative from "node:fs"
import { log } from "../core/logger.js"
import { syncGameItems, hashGameItems, pruneGameItems } from "./sync.js"
import { gameArcRaiders } from "./arc-raiders/index.js"
import { gameDiablo4 } from "./diablo-4/index.js"

const gameRegistry = [gameArcRaiders, gameDiablo4]

function parseArrayArg(token) {
    return token
        .split(",")
        .map((part) => part.trim())
        .filter(Boolean)
}

export function parseGamesBuildArgs(argv) {
    const options = {
        games: [],
        dryRun: false,
        force: false,
        prune: false,
    }

    for (let i = 0; i < argv.length; i += 1) {
        const token = argv[i]
        const next = argv[i + 1]

        if (token === "--game" && next) {
            options.games.push(...parseArrayArg(next))
            i += 1
            continue
        }

        if (token.startsWith("--game=")) {
            options.games.push(...parseArrayArg(token.slice("--game=".length)))
            continue
        }

        if (token === "--dry-run") {
            options.dryRun = true
            continue
        }

        if (token === "--force") {
            options.force = true
            continue
        }

        if (token === "--prune") {
            options.prune = true
        }
    }

    return options
}

function resolveGames(gameIds) {
    if (!gameIds || gameIds.length === 0) {
        return gameRegistry
    }

    const selected = gameRegistry.filter((game) => gameIds.includes(game.id))
    const unknown = gameIds.filter((id) => !gameRegistry.some((game) => game.id === id))

    if (unknown.length > 0) {
        throw new Error(`Unknown game id(s): ${unknown.join(", ")}`)
    }

    return selected
}

export async function buildGames(options = {}) {
    const targets = resolveGames(options.games)
    log.start(`Starting games build for ${targets.length} game(s)...`)

    for (const game of targets) {
        log.section(`Game: ${game.id}`)

        // Crawl gate: skip if items.json already has data (unless --force)
        let shouldCrawl = true
        if (!options.force && !options.dryRun) {
            try {
                const rawItemsPath = path.join("games", game.id, "items.json")
                const existing = JSON.parse(fsNative.readFileSync(rawItemsPath, "utf8"))
                if (Array.isArray(existing) && existing.length > 0) {
                    log.info(`  ${game.id}: items.json exists (${existing.length} items) — skipping crawl. Use --force to re-crawl.`)
                    shouldCrawl = false
                }
            } catch {
                // File missing or invalid — proceed with crawl
            }
        }

        // Step 1-2: Crawl raw data → games/<game-id>/items.json
        if (shouldCrawl) {
            await game.build({
                id: game.id,
                output: path.resolve("games", game.id),
                dryRun: !!options.dryRun,
            })
        }

        // Step 3-7: Sync to src/statics/items/<game-id>/<item-id>/ (non-destructive YAML + JSON)
        if (!options.dryRun) {
            log.info(`  Syncing items to src/ and build/...`)
            const syncResult = await syncGameItems(game.id, game.detailMapper ?? null, game.imageResolver ?? null)

            if (options.prune) {
                log.info(`  Pruning orphan item folders...`)
                await pruneGameItems(game.id, syncResult.itemIds)
            }

            // Step 8: Hash game item build output
            log.info(`  Hashing game item build output...`)
            await hashGameItems(game.id)
        }
    }

    log.start("Games build completed successfully!")
}

export { gameRegistry }