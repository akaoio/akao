import { buildGames, parseGamesBuildArgs } from "./games/index.js"

const args = process.argv.slice(2)

async function main() {
    const options = parseGamesBuildArgs(args)
    await buildGames(options)
}

main().catch((error) => {
    console.error(error)
    process.exit(1)
})