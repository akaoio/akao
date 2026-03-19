import path from "node:path"
import { log } from "../core/logger.js"
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
        await game.build({
            id: game.id,
            output: path.resolve("games", game.id),
            dryRun: !!options.dryRun,
        })
    }

    log.start("Games build completed successfully!")
}

export { gameRegistry }