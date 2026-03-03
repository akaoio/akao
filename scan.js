import { scanTokenDecimals } from "./scan/currencies.js"
import { scanPools } from "./scan/pools.js"
import { icons, color } from "./src/core/Colors.js"

const parseOptions = (args = []) => {
    const options = { chain: null, force: false }

    for (const arg of args) {
        if (arg.startsWith("--chain=")) options.chain = arg.split("=").slice(1).join("=")
        if (arg === "--force") options.force = true
    }

    return options
}

const runFullScan = async (options) => {
    await scanTokenDecimals(options)
    await scanPools(options)
}

const runCurrencies = async (options) => {
    await scanTokenDecimals(options)
}

const runPools = async (options) => {
    await scanPools(options)
}

const showUsage = () => {
    console.error(color.error("Usage: node scan.js [currencies|pools] [--chain=CHAIN] [--force]"))
}

const args = process.argv.slice(2)
const command = args.find((arg) => !arg.startsWith("--"))
const options = parseOptions(args)

try {
    if (!command) {
        console.log(color.header(`${icons.start} Running full crypto scan...`))
        await runFullScan(options)
        process.exit(0)
    }

    switch (command.toLowerCase()) {
        case "currencies":
            console.log(color.header(`${icons.start} Running currency scan...`))
            await runCurrencies(options)
            process.exit(0)
            break

        case "pools":
            console.log(color.header(`${icons.start} Running pool scan...`))
            await runPools(options)
            process.exit(0)
            break

        default:
            console.error(color.error(`Unknown command: ${command}`))
            showUsage()
            process.exit(1)
    }
} catch (error) {
    console.error(color.error(`Crypto scan failed: ${error.message}`))
    process.exit(1)
}
