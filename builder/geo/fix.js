import { readFileSync, createReadStream, existsSync } from "fs"
import { createInterface } from "readline"
import DB from "../../src/core/DB.js"
import { write } from "../../src/core/Utils.js"
import { ADMIN_FEATURE_REGEX, COUNTRY_FEATURE_REGEX, normalizeAdminCodes, resolveParent } from "./shared.js"
import { loadHierarchyMap } from "./hierarchy.js"

export async function fixChildrenRelationships({ isTestMode = false, testCountry = "US", outputBase = "build" }) {
    console.log("Fixing children relationships from allCountries.txt...\n")

    const allCountriesPath = "./geo/allCountries.txt"
    if (!existsSync(allCountriesPath)) {
        console.error("Error: allCountries.txt not found.")
        process.exit(1)
    }

    console.log("Pass 1/2: Building admin feature index...")
    const hierarchyMap = loadHierarchyMap()
    let adminIndex = new Map()
    let lineCount = 0

    const rl1 = createInterface({
        input: createReadStream(allCountriesPath),
        crlfDelay: Infinity
    })

    for await (const line of rl1) {
        if (!line.trim()) continue

        const fields = line.split("\t")
        if (fields.length < 19) continue

        if (isTestMode && fields[8] !== testCountry) continue

        const featureCode = fields[7]
        const isAdmin = ADMIN_FEATURE_REGEX.test(featureCode)
        if (!isAdmin) continue

        const id = parseInt(fields[0], 10)
        const countryCode = fields[8]
        const { admin1, admin2, admin3, admin4 } = normalizeAdminCodes(fields)
        const key = `${countryCode}|${admin1}|${admin2}|${admin3}|${admin4}|${featureCode}`
        adminIndex.set(key, id)

        lineCount++
        if (lineCount % 100000 === 0) {
            console.log(`  Indexed ${lineCount.toLocaleString()} records...`)
        }
    }

    console.log(`✓ Indexed ${adminIndex.size.toLocaleString()} admin features from ${lineCount.toLocaleString()} records\n`)

    console.log("Pass 2/2: Mapping parent-child relationships...")
    const childrenMap = new Map()
    let relationCount = 0
    let processedRelations = 0

    const rl2 = createInterface({
        input: createReadStream(allCountriesPath),
        crlfDelay: Infinity
    })

    for await (const line of rl2) {
        if (!line.trim()) continue

        const fields = line.split("\t")
        if (fields.length < 19) continue

        if (isTestMode && fields[8] !== testCountry) continue

        processedRelations++
        const id = parseInt(fields[0], 10)
        const featureCode = fields[7]
        const countryCode = fields[8]
        const admin1 = fields[10]
        const admin2 = fields[11]
        const admin3 = fields[12]
        const admin4 = fields[13]

        const parent = resolveParent({ featureCode, countryCode, admin1, admin2, admin3, admin4, adminIndex, hierarchyMap })

        if (COUNTRY_FEATURE_REGEX.test(featureCode)) {
            if (!childrenMap.has(id)) childrenMap.set(id, [])
        }

        if (parent) {
            if (!childrenMap.has(parent)) childrenMap.set(parent, [])
            childrenMap.get(parent).push(id)
            relationCount++
        }

        if (processedRelations % 100000 === 0) {
            console.log(`  Processed ${processedRelations.toLocaleString()} records | Mapped ${relationCount.toLocaleString()} relationships...`)
        }
    }

    console.log(`✓ Mapped ${relationCount.toLocaleString()} parent-child relationships for ${childrenMap.size.toLocaleString()} parents\n`)

    console.log("Updating parent files with children...")
    let updatedCount = 0

    for (const [parentId, children] of childrenMap.entries()) {
        const pathSegments = DB.path(parentId)
        const filename = pathSegments[pathSegments.length - 1] + ".json"
        const filePath = [outputBase, "geo", ...pathSegments.slice(0, -1), filename]
        const fullPath = "./" + filePath.join("/")

        if (existsSync(fullPath)) {
            try {
                const content = readFileSync(fullPath, "utf8")
                const record = JSON.parse(content)
                record.children = children
                await write(filePath, record)
                updatedCount++

                if (updatedCount % 10000 === 0) {
                    console.log(`  Updated ${updatedCount.toLocaleString()} parent files...`)
                }
            } catch (error) {
                console.error(`Error updating parent ${parentId}:`, error.message)
            }
        }
    }

    console.log(`✓ Updated ${updatedCount.toLocaleString()} parent files with children\n`)
    if (isTestMode) {
        console.log(`✅ Children relationships fixed for ${testCountry}!`)
    } else {
        console.log("✅ Children relationships fixed!")
    }
}
