import { readFileSync, createReadStream, existsSync, statSync, unlinkSync } from "fs"
import { createInterface } from "readline"
import AdmZip from "adm-zip"
import DB from "../../src/core/DB.js"
import { parseCSV, sha256 } from "../../src/core/Utils.js"
import { FS } from "../../src/core/FS.js"
import { generateGeoDirectoryHashes, resetGeoHashCounter } from "./hash.js"
import { validateHierarchy } from "./validate.js"
import { ADMIN_FEATURE_REGEX, COUNTRY_FEATURE_REGEX, resolveParent, normalizeAdminCodes } from "./shared.js"
import { loadHierarchyMap, loadAdminCode5 } from "./hierarchy.js"

const GEO_BUILD_CACHE_VERSION = 1

function buildGeoFingerprint({ isTestMode, testCountry, outputBase }) {
    const inputs = [
        "./geo/countryInfo.txt",
        "./geo/featureCodes_en.txt",
        "./geo/allCountries.txt",
        "./geo/hierarchy.txt",
        "./geo/adminCode5.txt",
    ]

    return {
        version: GEO_BUILD_CACHE_VERSION,
        isTestMode,
        testCountry,
        outputBase,
        inputs: inputs.map((filePath) => {
            if (!existsSync(filePath)) return { filePath, exists: false }
            const stats = statSync(filePath)
            return {
                filePath,
                exists: true,
                size: stats.size,
                mtimeMs: stats.mtimeMs,
            }
        }),
    }
}

function hasGeoOutputs(outputBase) {
    return [
        `./${outputBase}/geo/countries.json`,
        `./${outputBase}/geo/features.json`,
        `./${outputBase}/geo/_.hash`,
    ].every((filePath) => existsSync(filePath))
}

export async function buildGeo({ isTestMode = false, testCountry = "US", outputBase = "build" }) {
    if (isTestMode) {
        console.log(`Starting GeoNames test build for country: ${testCountry}\n`)
        console.log("Test data will be written to: temp/geo/\n")
    } else {
        console.log("Starting GeoNames data processing...\n")
    }

    console.log("Step 1: Extracting allCountries.zip...")
    const zipPath = "./geo/allCountries.zip"
    const extractPath = "./geo/"
    const extractedFile = "./geo/allCountries.txt"

    if (!existsSync(zipPath)) {
        console.error("Error: allCountries.zip not found. Run 'npm run geo:download' first.")
        process.exit(1)
    }

    let needsExtraction = true
    if (existsSync(extractedFile)) {
        const stats = statSync(extractedFile)
        if (stats.size > 0) {
            const ageMs = Date.now() - stats.mtimeMs
            const hoursAgo = Math.floor(ageMs / (60 * 60 * 1000))
            const daysAgo = Math.floor(ageMs / (24 * 60 * 60 * 1000))
            if (daysAgo > 0) {
                console.log(`⊘ Skipping extraction (extracted ${daysAgo}d ago)\n`)
            } else {
                console.log(`⊘ Skipping extraction (extracted ${hoursAgo}h ago)\n`)
            }
            needsExtraction = false
        }
    }

    if (needsExtraction) {
        try {
            const zip = new AdmZip(zipPath)
            zip.extractAllTo(extractPath, true)
            console.log("✓ Extracted allCountries.txt\n")
        } catch (error) {
            console.error("Error extracting zip:", error)
            process.exit(1)
        }
    }

    const buildCachePath = `./${outputBase}/geo/.build-cache.json`
    const currentFingerprint = buildGeoFingerprint({ isTestMode, testCountry, outputBase })
    if (!isTestMode && existsSync(buildCachePath) && hasGeoOutputs(outputBase)) {
        try {
            const cachedFingerprint = JSON.parse(readFileSync(buildCachePath, "utf8"))
            if (JSON.stringify(cachedFingerprint) === JSON.stringify(currentFingerprint)) {
                console.log("⊘ Geo inputs unchanged; skipping rebuild.\n")
                return
            }
        } catch {
            // Ignore malformed cache and rebuild.
        }
    }

    console.log("Step 2: Processing countryInfo.txt...")
    const countryInfoPath = "./geo/countryInfo.txt"
    if (!existsSync(countryInfoPath)) {
        console.error("Error: countryInfo.txt not found.")
        process.exit(1)
    }

    try {
        const countryText = readFileSync(countryInfoPath, "utf8")
        const countryData = parseCSV(countryText, { delimiter: "\t", headers: true, commentPrefix: "#" })
        console.log(`  Parsed ${countryData.length} rows`)

        const countries = countryData.map((row) => ({
            id: parseInt(row.geonameid) || 0,
            code: row.ISO || "",
            iso3: row.ISO3 || "",
            isoNumeric: row["ISO-Numeric"] || "",
            fips: row.fips || "",
            name: row.Country || "",
            capital: row.Capital || "",
            area: parseFloat(row["Area(in sq km)"]) || 0,
            population: parseInt(row.Population) || 0,
            continent: row.Continent || "",
            tld: row.tld || "",
            currency: row.CurrencyCode || "",
            currencyName: row.CurrencyName || "",
            phone: row.Phone || "",
            postalCodeFormat: row["Postal Code Format"] || "",
            postalCodeRegex: row["Postal Code Regex"] || "",
            languages: row.Languages || "",
            neighbours: row.neighbours || "",
            equivalentFipsCode: row.EquivalentFipsCode || ""
        }))

        await FS.write([outputBase, "geo", "countries.json"], countries)
        const countriesHash = sha256(JSON.stringify(countries))
        await FS.write([outputBase, "geo", "countries.hash"], countriesHash)
        console.log(`✓ Created ${outputBase}/geo/countries.json with ${countries.length} countries\n`)
    } catch (error) {
        console.error("Error processing countries:", error)
        process.exit(1)
    }

    console.log("Step 3: Processing featureCodes_en.txt...")
    const featureCodesPath = "./geo/featureCodes_en.txt"
    if (!existsSync(featureCodesPath)) {
        console.error("Error: featureCodes.txt not found.")
        process.exit(1)
    }

    try {
        const featureText = readFileSync(featureCodesPath, "utf8")
        const featureData = parseCSV(featureText, { delimiter: "\t", headers: false })

        const features = featureData.map((row) => ({
            code: row[0] || "",
            name: row[1] || "",
            description: row[2] || ""
        }))

        await FS.write([outputBase, "geo", "features.json"], features)
        const featuresHash = sha256(JSON.stringify(features))
        await FS.write([outputBase, "geo", "features.hash"], featuresHash)
        console.log(`✓ Created ${outputBase}/geo/features.json with ${features.length} feature codes\n`)
    } catch (error) {
        console.error("Error processing feature codes:", error)
        process.exit(1)
    }

    console.log("Step 4: Processing allCountries.txt...")
    const allCountriesPath = "./geo/allCountries.txt"
    if (!existsSync(allCountriesPath)) {
        console.error("Error: allCountries.txt not found.")
        process.exit(1)
    }

    let needsProcessing = true

    const hierarchyMap = loadHierarchyMap()
    const admin5Map = loadAdminCode5()

    // adminMeta keeps lightweight info for parent-country validation
    let adminMeta = new Map()
    let adminIndex = new Map()
    let countryIds = new Map()  // Map countryCode -> country ID
    let processedCount = 0
    let writtenCount = 0
    let skippedHistorical = 0
    let skippedCrossCountry = 0

    if (needsProcessing) {
        if (isTestMode) console.log(`Building test data for ${testCountry}...\n`)
        else console.log("This will take a while (12M+ records)...\n")

        console.log("Pass 1/4: Building admin feature index...")
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
            if (!ADMIN_FEATURE_REGEX.test(featureCode)) continue

            // Skip historical feature codes (suffix H)
            if (featureCode.endsWith("H")) {
                skippedHistorical++
                continue
            }

            const id = parseInt(fields[0], 10)
            const countryCode = fields[8]
            const { admin1, admin2, admin3, admin4 } = normalizeAdminCodes(fields)
            const key = `${countryCode}|${admin1}|${admin2}|${admin3}|${admin4}|${featureCode}`
            adminIndex.set(key, id)
            adminMeta.set(id, { key, featureCode, countryCode, admin1, admin2, admin3, admin4 })
            
            // Store country IDs for quick lookup
            if (COUNTRY_FEATURE_REGEX.test(featureCode)) {
                countryIds.set(countryCode, id)
            }

            lineCount++
            if (lineCount % 100000 === 0) {
                console.log(`  Indexed ${lineCount.toLocaleString()} records...`)
            }
        }

        // Second pass inside Pass 1: drop records whose parent belongs to another country
        // or whose parent is missing entirely.
        const removedIds = new Set()
        for (const [id, meta] of adminMeta.entries()) {
            const parentId = resolveParent({
                featureCode: meta.featureCode,
                countryCode: meta.countryCode,
                admin1: meta.admin1,
                admin2: meta.admin2,
                admin3: meta.admin3,
                admin4: meta.admin4,
                adminIndex,
                hierarchyMap,
                countryIds
            })

            if (!parentId) continue

            const parentMeta = adminMeta.get(parentId)
            if (!parentMeta) {
                skippedCrossCountry++
                removedIds.add(id)
                continue
            }

            if (parentMeta.countryCode !== meta.countryCode) {
                skippedCrossCountry++
                removedIds.add(id)
            }
        }

        for (const id of removedIds) {
            const meta = adminMeta.get(id)
            if (!meta) continue
            adminMeta.delete(id)
            adminIndex.delete(meta.key)

            // Remove stale files if they exist from previous runs
            const pathSegments = DB.path(id)
            const filename = pathSegments[pathSegments.length - 1] + ".json"
            const hashFilename = pathSegments[pathSegments.length - 1] + ".hash"
            const filePath = [outputBase, "geo", ...pathSegments.slice(0, -1), filename]
            const hashFilePath = [outputBase, "geo", ...pathSegments.slice(0, -1), hashFilename]
            const fullPath = "./" + filePath.join("/")
            const fullHashPath = "./" + hashFilePath.join("/")

            if (existsSync(fullPath)) {
                try {
                    unlinkSync(fullPath)
                } catch (err) {
                    console.warn(`⚠ Could not delete filtered record ${id}: ${err.message}`)
                }
            }
            if (existsSync(fullHashPath)) {
                try {
                    unlinkSync(fullHashPath)
                } catch (err) {
                    console.warn(`⚠ Could not delete filtered hash ${id}: ${err.message}`)
                }
            }
        }

        console.log(`✓ Indexed ${adminIndex.size.toLocaleString()} admin features from ${lineCount.toLocaleString()} records`)
        if (skippedHistorical > 0) {
            console.log(`  ⊘ Skipped historical features: ${skippedHistorical.toLocaleString()}`)
        }
        if (skippedCrossCountry > 0) {
            console.log(`  ⊘ Skipped cross-country parents: ${skippedCrossCountry.toLocaleString()}`)
        }
        console.log("")

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

        console.log("Pass 2/4: Processing and writing records...")
        const rl2 = createInterface({
            input: createReadStream(allCountriesPath),
            crlfDelay: Infinity
        })

        for await (const line of rl2) {
            if (!line.trim()) continue
            const fields = line.split("\t")
            if (fields.length < 19) continue
            if (isTestMode && fields[8] !== testCountry) continue

            const featureCode = fields[7]
            if (!ADMIN_FEATURE_REGEX.test(featureCode)) continue

            processedCount++

            const id = parseInt(fields[0], 10)
            const name = fields[1]
            const asciiname = fields[2]
            const alternatenames = fields[3]
            const latitude = parseFloat(fields[4])
            const longitude = parseFloat(fields[5])
            const featureClass = fields[6]
            const countryCode = fields[8]
            const cc2 = fields[9]
            const admin1 = fields[10]
            const admin2 = fields[11]
            const admin3 = fields[12]
            const admin4 = fields[13]
            const population = parseInt(fields[14], 10) || 0
            const elevation = parseInt(fields[15], 10) || null
            const dem = parseInt(fields[16], 10) || null
            const timezone = fields[17]
            const modificationDate = fields[18]

            let level = 0
            if (COUNTRY_FEATURE_REGEX.test(featureCode)) level = 0
            else if (!admin1) level = 1  // Level 1: directly under country (no admin1)
            else if (!admin2) level = 2  // Level 2: has admin1 but no admin2
            else if (!admin3) level = 3  // Level 3: has admin2 but no admin3
            else if (!admin4) level = 4  // Level 4: has admin3 but no admin4
            else level = 5               // Level 5: has all admin fields

            let parent = resolveParent({ featureCode, countryCode, admin1, admin2, admin3, admin4, adminIndex, hierarchyMap, countryIds })
            if (parent === id) parent = null

            // If parent was filtered out (historical or cross-country), skip this record
            if (parent && !adminMeta.has(parent)) {
                continue
            }

            const adm5code = admin5Map.get(id) || null

            const record = {
                id,
                name,
                asciiname,
                alternatenames,
                latitude,
                longitude,
                featureClass,
                featureCode,
                countryCode,
                cc2,
                admin1,
                admin2,
                admin3,
                admin4,
                adm5code,
                population,
                elevation,
                dem,
                timezone,
                modificationDate,
                level,
                parent,
                children: []
            }

            const pathSegments = DB.path(id)
            const filename = pathSegments[pathSegments.length - 1] + ".json"
            const hashFilename = pathSegments[pathSegments.length - 1] + ".hash"
            const filePath = [outputBase, "geo", ...pathSegments.slice(0, -1), filename]
            const hashFilePath = [outputBase, "geo", ...pathSegments.slice(0, -1), hashFilename]
            const fullPath = "./" + filePath.join("/")
            const fullHashPath = "./" + hashFilePath.join("/")

            let needsWrite = true
            if (!isTestMode && existsSync(fullPath) && existsSync(fullHashPath)) {
                const stats = statSync(fullPath)
                const hashStats = statSync(fullHashPath)
                if (stats.size > 10 && hashStats.size > 0) {
                    needsWrite = false
                }
            }

            if (needsWrite) {
                try {
                    await FS.write(filePath, record)
                    const fileHash = sha256(JSON.stringify(record))
                    await FS.write(hashFilePath, fileHash)
                    writtenCount++
                } catch (error) {
                    console.error(`Error writing record ${id}:`, error.message)
                }
            }

            if (processedCount % 100000 === 0) {
                console.log(`  Processed ${processedCount.toLocaleString()} | Skipped ${processedCount - writtenCount} | Written ${writtenCount.toLocaleString()} records...`)
            }
        }

        console.log(`\n✓ Processing complete!`)
        console.log(`  Total records processed: ${processedCount.toLocaleString()}`)
        console.log(`  Total records skipped (cached): ${(processedCount - writtenCount).toLocaleString()}`)
        console.log(`  Total records written: ${writtenCount.toLocaleString()}`)
        console.log(`  Admin features indexed: ${adminIndex.size.toLocaleString()}`)

        console.log("\nPass 3/4: Building children relationships...")
        const childrenMap = new Map()
        const recordsMap = new Map()
        let relationCount = 0
        let processedRelations = 0
        let skippedNonAdmin = 0
        let skippedCrossCountryChildren = 0

        const rl3 = createInterface({
            input: createReadStream(allCountriesPath),
            crlfDelay: Infinity
        })

        for await (const line of rl3) {
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
                // Verify parent exists in adminMeta and belongs to same country
                const parentMeta = adminMeta.get(parent)
                if (!parentMeta) {
                    skippedCrossCountryChildren++
                    continue
                }
                
                if (parentMeta.countryCode !== countryCode) {
                    skippedCrossCountryChildren++
                    continue
                }

                if (!childrenMap.has(parent)) childrenMap.set(parent, [])
                childrenMap.get(parent).push(id)
                relationCount++
            }

            if (isTestMode && (COUNTRY_FEATURE_REGEX.test(featureCode) || ADMIN_FEATURE_REGEX.test(featureCode))) {
                recordsMap.set(id, { id, name: fields[1], featureCode, countryCode, parent })
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

        if (isTestMode) {
            validateHierarchy(recordsMap, childrenMap)
        }
    }

    console.log("Pass 4/4: Generating directory hash files (_.hash)...")
    resetGeoHashCounter()
    const dirHashCount = await generateGeoDirectoryHashes([outputBase, "geo"])
    console.log(`✓ Generated ${dirHashCount.toLocaleString()} directory hash files\n`)

    if (isTestMode) {
        console.log(`\n✅ Geo test complete! Test data for ${testCountry} written to temp/geo/`)
    } else {
        await FS.write([outputBase, "geo", ".build-cache.json"], currentFingerprint)
        console.log(`\n✅ Geo build complete! Data written to ${outputBase}/geo/`)
    }
}
