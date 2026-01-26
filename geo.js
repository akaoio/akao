import { buildGeo, downloadGeoData, fixChildrenRelationships } from "./builder/geo/index.js"

const args = process.argv.slice(2)
const action = args.includes("--build")
    ? "build"
    : args.includes("--download")
    ? "download"
    : args.includes("--fix")
    ? "fix"
    : args.includes("--test")
    ? "test"
    : null

const testCountryArg = args.find((arg) => arg.startsWith("--country="))
const testCountry = testCountryArg ? testCountryArg.split("=")[1].toUpperCase() : "US"
const isTestMode = action === "test"
const outputBase = isTestMode ? "temp" : "build"

if (!action) {
    console.error("Usage: node geo.js --download | node geo.js --build | node geo.js --fix | node geo.js --test [--country=XX]")
    process.exit(1)
}

async function main() {
    if (action === "download") {
        await downloadGeoData()
        return
    }

    const context = { isTestMode, testCountry, outputBase }

    if (action === "build") {
        await buildGeo({ ...context, isTestMode: false, outputBase: "build" })
        return
    }

    if (action === "test") {
        await buildGeo(context)
        return
    }

    if (action === "fix") {
        await fixChildrenRelationships({ ...context, isTestMode: false, outputBase: "build" })
    }
}

main().catch((error) => {
    console.error(error)
    process.exit(1)
})
