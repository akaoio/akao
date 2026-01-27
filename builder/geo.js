import { buildGeo, downloadGeoData, fixChildrenRelationships } from "./geo/index.js"

const args = process.argv.slice(2)
const action = args.includes("--build")
    ? "build"
    : args.includes("--fix")
    ? "fix"
    : null

const testCountryArg = args.find((arg) => arg.startsWith("--country="))
const testCountry = testCountryArg ? testCountryArg.split("=")[1].toUpperCase() : null

if (!action) {
    console.error("Usage:")
    console.error("  node builder/geo.js --build [--country=XX]  - Build geo data (downloads if needed)")
    console.error("  node builder/geo.js --fix                   - Fix children relationships")
    process.exit(1)
}

async function main() {
    // Always ensure data is downloaded before building
    if (action === "build") {
        await downloadGeoData()
        
        const context = {
            isTestMode: !!testCountry,
            testCountry: testCountry || "US",
            outputBase: "build"
        }
        
        await buildGeo(context)
        return
    }

    if (action === "fix") {
        await fixChildrenRelationships({
            isTestMode: false,
            testCountry: null,
            outputBase: "build"
        })
    }
}

main().catch((error) => {
    console.error(error)
    process.exit(1)
})
