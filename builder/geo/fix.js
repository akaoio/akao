import { readFileSync, createReadStream, existsSync, unlinkSync } from "fs"
import { createInterface } from "readline"
import DB from "../../src/core/DB.js"
import { FS } from "../../src/core/FS.js"
import { ADMIN_FEATURE_REGEX, COUNTRY_FEATURE_REGEX, normalizeAdminCodes, resolveParent } from "./shared.js"
import { loadHierarchyMap } from "./hierarchy.js"
import { generateGeoDirectoryHashes, resetGeoHashCounter } from "./hash.js"

export async function fixChildrenRelationships({ isTestMode = false, testCountry = "US", outputBase = "build" }) {
    console.log("Fixing parent and children relationships from allCountries.txt...\n")

    const allCountriesPath = "./geo/allCountries.txt"
    if (!existsSync(allCountriesPath)) {
        console.error("Error: allCountries.txt not found.")
        process.exit(1)
    }

    console.log("Pass 1/2: Building admin feature index...")
    const hierarchyMap = loadHierarchyMap()
    let adminIndex = new Map()
    let countryIds = new Map()  // Map countryCode -> country ID
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
        
        // Store country IDs for quick lookup
        if (COUNTRY_FEATURE_REGEX.test(featureCode)) {
            countryIds.set(countryCode, id)
        }

        lineCount++
        if (lineCount % 100000 === 0) {
            console.log(`  Indexed ${lineCount.toLocaleString()} records...`)
        }
    }

    console.log(`✓ Indexed ${adminIndex.size.toLocaleString()} admin features from ${lineCount.toLocaleString()} records\n`)

    console.log("Pass 1.5/4: Removing non-administrative features...")
    let deletedCount = 0
    let skippedDelete = 0

    const rl1b = createInterface({
        input: createReadStream(allCountriesPath),
        crlfDelay: Infinity
    })

    for await (const line of rl1b) {
        if (!line.trim()) continue

        const fields = line.split("\t")
        if (fields.length < 19) continue

        if (isTestMode && fields[8] !== testCountry) continue

        const featureCode = fields[7]
        const isAdmin = ADMIN_FEATURE_REGEX.test(featureCode)
        
        // Only delete non-admin features
        if (isAdmin) continue

        const id = parseInt(fields[0], 10)
        const pathSegments = DB.path(id)
        const filename = pathSegments[pathSegments.length - 1] + ".json"
        const hashFilename = pathSegments[pathSegments.length - 1] + ".hash"
        const filePath = [outputBase, "geo", ...pathSegments.slice(0, -1), filename]
        const hashFilePath = [outputBase, "geo", ...pathSegments.slice(0, -1), hashFilename]
        const fullPath = "./" + filePath.join("/")
        const fullHashPath = "./" + hashFilePath.join("/")

        try {
            if (existsSync(fullPath)) {
                unlinkSync(fullPath)
                deletedCount++
            }
            if (existsSync(fullHashPath)) {
                unlinkSync(fullHashPath)
            }
        } catch (err) {
            skippedDelete++
            console.warn(`⚠ Could not delete non-admin record ${id}: ${err.message}`)
        }
    }

    if (deletedCount > 0) {
        console.log(`✓ Deleted ${deletedCount.toLocaleString()} non-administrative feature files`)
        if (skippedDelete > 0) {
            console.log(`  ⚠ Failed to delete ${skippedDelete.toLocaleString()} files`)
        }
        console.log("")
    }

    console.log("Pass 2/4: Mapping parent-child relationships...")
    const childrenMap = new Map()
    let relationCount = 0
    let processedRelations = 0
    let skippedNonAdmin = 0
    let skippedCrossCountryChildren = 0

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

        // Skip non-administrative features (e.g., hotels, schools, etc.)
        if (!ADMIN_FEATURE_REGEX.test(featureCode)) {
            skippedNonAdmin++
            continue
        }

        const parent = resolveParent({ featureCode, countryCode, admin1, admin2, admin3, admin4, adminIndex, hierarchyMap, countryIds })
        if (parent === id) continue

        if (COUNTRY_FEATURE_REGEX.test(featureCode)) {
            if (!childrenMap.has(id)) childrenMap.set(id, [])
        }

        if (parent) {
            // Verify parent exists in adminIndex and belongs to same country
            const parentMeta = adminIndex.get(`${countryCode}|||||${COUNTRY_FEATURE_REGEX.test(featureCode) ? featureCode : ''}`)
            
            // For non-country features, find parent's country through adminIndex
            let parentCountryCode = countryCode
            for (const [key, id] of adminIndex.entries()) {
                if (id === parent) {
                    parentCountryCode = key.split('|')[0]
                    break
                }
            }
            
            if (parentCountryCode !== countryCode) {
                skippedCrossCountryChildren++
                continue
            }

            if (!childrenMap.has(parent)) childrenMap.set(parent, [])
            childrenMap.get(parent).push(id)
            relationCount++
        }

        if (processedRelations % 100000 === 0) {
            console.log(`  Processed ${processedRelations.toLocaleString()} records | Mapped ${relationCount.toLocaleString()} relationships...`)
        }
    }

    console.log(`✓ Mapped ${relationCount.toLocaleString()} parent-child relationships for ${childrenMap.size.toLocaleString()} parents`)
    if (skippedNonAdmin > 0) {
        console.log(`  ⊘ Skipped non-administrative features: ${skippedNonAdmin.toLocaleString()}`)
    }
    if (skippedCrossCountryChildren > 0) {
        console.log(`  ⊘ Skipped cross-country children: ${skippedCrossCountryChildren.toLocaleString()}`)
    }
    console.log("")

    console.log("Pass 3/4: Updating parent files with children...")
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
                await FS.write(filePath, record)
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

    console.log("Pass 4/4: Updating parent field for all records...")
    const parentMap = new Map()  // Map recordId -> parentId
    let parentUpdateCount = 0
    let processedParents = 0

    const rl3 = createInterface({
        input: createReadStream(allCountriesPath),
        crlfDelay: Infinity
    })

    for await (const line of rl3) {
        if (!line.trim()) continue

        const fields = line.split("\t")
        if (fields.length < 19) continue

        if (isTestMode && fields[8] !== testCountry) continue

        processedParents++
        const id = parseInt(fields[0], 10)
        const featureCode = fields[7]
        const countryCode = fields[8]
        const admin1 = fields[10]
        const admin2 = fields[11]
        const admin3 = fields[12]
        const admin4 = fields[13]

        const parent = resolveParent({ featureCode, countryCode, admin1, admin2, admin3, admin4, adminIndex, hierarchyMap, countryIds })

        if (parent === id) {
            parentMap.set(id, null)
        } else if (parent) {
            parentMap.set(id, parent)
        }

        if (processedParents % 100000 === 0) {
            console.log(`  Processed ${processedParents.toLocaleString()} records | Mapped ${parentMap.size.toLocaleString()} parents...`)
        }
    }

    console.log(`✓ Mapped ${parentMap.size.toLocaleString()} parent references\n`)

    console.log("Updating record files with parent field...")
    let parentFieldUpdatedCount = 0

    for (const [recordId, parentId] of parentMap.entries()) {
        const pathSegments = DB.path(recordId)
        const filename = pathSegments[pathSegments.length - 1] + ".json"
        const filePath = [outputBase, "geo", ...pathSegments.slice(0, -1), filename]
        const fullPath = "./" + filePath.join("/")

        if (existsSync(fullPath)) {
            try {
                const content = readFileSync(fullPath, "utf8")
                const record = JSON.parse(content)
                record.parent = parentId ?? null
                await FS.write(filePath, record)
                parentFieldUpdatedCount++

                if (parentFieldUpdatedCount % 10000 === 0) {
                    console.log(`  Updated ${parentFieldUpdatedCount.toLocaleString()} records with parent field...`)
                }
            } catch (error) {
                console.error(`Error updating record ${recordId}:`, error.message)
            }
        }
    }

    console.log(`✓ Updated ${parentFieldUpdatedCount.toLocaleString()} records with parent field\n`)
    
    console.log("Pass 5/5: Regenerating directory hash files (_.hash)...")
    resetGeoHashCounter()
    const dirHashCount = await generateGeoDirectoryHashes([outputBase, "geo"])
    console.log(`✓ Generated ${dirHashCount.toLocaleString()} directory hash files\n`)
    
    if (isTestMode) {
        console.log(`✅ Children and parent relationships fixed for ${testCountry}!`)
    } else {
        console.log("✅ Children and parent relationships fixed!")
    }
}
