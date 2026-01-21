import { readFileSync, createReadStream, existsSync, statSync } from "fs"
import { createInterface } from "readline"
import AdmZip from "adm-zip"
import { download, write, parseCSV, sha256 } from "./src/core/Utils.js"
import { load, dir, exist, isDirectory } from "./src/core/FS.js"
import Geo from "./src/core/Geo.js"

const args = process.argv.slice(2)
const action = args.includes("--build") ? "build" : args.includes("--download") ? "download" : null

if (!action) {
    console.error("Usage: node geo.js --download | node geo.js --build")
    process.exit(1)
}

const base = "http://download.geonames.org/export/dump/"
const files = ["readme.txt", "countryInfo.txt", "allCountries.zip", "featureCodes_en.txt"]

// ====================== HASH GENERATION ======================
let totalDirHashCount = 0

/**
 * Generate directory hash files recursively for geo data
 * - Reads existing .hash files for JSON files (already created during Pass 2)
 * - Only creates _.hash files for directories
 * - Optimized for large datasets to avoid memory issues
 */
async function generateGeoDirectoryHashes(path = [], depth = 0) {
    const entries = await dir(path)
    if (!entries || entries.length === 0) return 0

    const hashFiles = []
    const subDirs = []
    let hashCount = 0

    // Separate hash files and directories
    for (const entry of entries) {
        const entryPath = [...path, entry]

        if (await isDirectory(entryPath)) {
            subDirs.push(entry)
        } else if (entry.endsWith(".hash") && entry !== "_.hash") {
            hashFiles.push(entry)
        }
    }

    // First, process subdirectories recursively (deepest first)
    for (const subDir of subDirs) {
        const count = await generateGeoDirectoryHashes([...path, subDir], depth + 1)
        hashCount += count
    }

    // Collect all child hashes (from .hash files in this directory)
    const childHashes = []

    for (const hashFile of hashFiles) {
        const hashContent = await load([...path, hashFile])
        if (hashContent) {
            childHashes.push(hashContent)
        }
    }

    // Add subdirectory hashes
    for (const subDir of subDirs) {
        const subDirHashPath = [...path, subDir, "_.hash"]
        if (await exist(subDirHashPath)) {
            const hashContent = await load(subDirHashPath)
            if (hashContent) {
                childHashes.push(hashContent)
            }
        }
    }

    // Generate directory _.hash if there are any hashes to combine
    if (childHashes.length > 0) {
        const combinedHash = sha256(childHashes.sort().join(""))
        await write([...path, "_.hash"], combinedHash)
        hashCount++
        totalDirHashCount++
        
        // Progress logging at top level only
        if (depth === 0 && totalDirHashCount % 100 === 0) {
            console.log(`  Generated ${totalDirHashCount.toLocaleString()} directory hash files...`)
        }
    }

    // Clear childHashes array to free memory
    childHashes.length = 0

    return hashCount
}

// ====================== DOWNLOAD ACTION ======================
if (action === "download") {
    console.log("Starting GeoNames data download...\n")
    
    const now = Date.now()
    const oneDayMs = 24 * 60 * 60 * 1000
    
    for (const file of files) {
        const filePath = `./geo/${file}`
        
        // Check if file exists and was downloaded within 24h
        if (existsSync(filePath)) {
            const stats = statSync(filePath)
            const ageMs = now - stats.mtimeMs
            
            if (ageMs < oneDayMs) {
                const hoursAgo = Math.floor(ageMs / (60 * 60 * 1000))
                console.log(`⊘ Skipping ${file} (downloaded ${hoursAgo}h ago)\n`)
                continue
            }
        }
        
        const url = base + file
        console.log(`Downloading ${file}...`)
        const result = await download(url, ["geo", file])
        if (result?.success) {
            console.log(`✓ Downloaded: ${result.path}\n`)
        } else {
            console.log(`✗ Failed to download: ${file}\n`)
        }
    }
    
    console.log("Download complete!")
}

// ====================== BUILD ACTION ======================
if (action === "build") {
    console.log("Starting GeoNames data processing...\n")
    
    // ====================== STEP 1: EXTRACT ZIP ======================
    console.log("Step 1: Extracting allCountries.zip...")
    const zipPath = "./geo/allCountries.zip"
    const extractPath = "./geo/"
    const extractedFile = "./geo/allCountries.txt"
    
    if (!existsSync(zipPath)) {
        console.error("Error: allCountries.zip not found. Run 'npm run geo:download' first.")
        process.exit(1)
    }
    
    // Check if already extracted
    let needsExtraction = true
    if (existsSync(extractedFile)) {
        const stats = statSync(extractedFile)
        
        // If file exists and has content, skip extraction
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
    
    // ====================== STEP 2: PROCESS COUNTRIES ======================
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
        
        // Parse country rows
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
        
        await write(["src", "statics", "geo", "countries.yaml"], countries)
        console.log(`✓ Created countries.yaml with ${countries.length} countries\n`)
    } catch (error) {
        console.error("Error processing countries:", error)
        process.exit(1)
    }
    
    // ====================== STEP 3: PROCESS FEATURE CODES ======================
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
        
        await write(["src", "statics", "geo", "features.yaml"], features)
        console.log(`✓ Created features.yaml with ${features.length} feature codes\n`)
    } catch (error) {
        console.error("Error processing feature codes:", error)
        process.exit(1)
    }
    
    // ====================== STEP 4: PROCESS ALL COUNTRIES DATA ======================
    console.log("Step 4: Processing allCountries.txt...")
    console.log("This will take a while (12M+ records)...\n")
    
    const allCountriesPath = "./geo/allCountries.txt"
    
    if (!existsSync(allCountriesPath)) {
        console.error("Error: allCountries.txt not found.")
        process.exit(1)
    }
    
    // First pass: Build index of all admin features for parent lookup
    console.log("Pass 1/4: Building admin feature index...")
    const adminIndex = new Map()
    let lineCount = 0
    
    const rl1 = createInterface({
        input: createReadStream(allCountriesPath),
        crlfDelay: Infinity
    })
    
    for await (const line of rl1) {
        if (!line.trim()) continue
        
        const fields = line.split("\t")
        if (fields.length < 19) continue
        
        const featureCode = fields[7]
        const isAdmin = featureCode === "PCLI" || featureCode.startsWith("PCL") || /^ADM[1-5]$/.test(featureCode)
        
        if (isAdmin) {
            const id = parseInt(fields[0])
            const countryCode = fields[8]
            const admin1 = fields[10]
            const admin2 = fields[11]
            const admin3 = fields[12]
            const admin4 = fields[13]
            
            const key = `${countryCode}|${admin1}|${admin2}|${admin3}|${admin4}|${featureCode}`
            adminIndex.set(key, id)
        }
        
        lineCount++
        if (lineCount % 100000 === 0) {
            console.log(`  Indexed ${lineCount.toLocaleString()} records...`)
        }
    }
    
    console.log(`✓ Indexed ${adminIndex.size.toLocaleString()} admin features from ${lineCount.toLocaleString()} records\n`)
    
    // Second pass: Process and write records
    console.log("Pass 2/4: Processing and writing records...")
    let processedCount = 0
    let writtenCount = 0
    
    const rl2 = createInterface({
        input: createReadStream(allCountriesPath),
        crlfDelay: Infinity
    })
    
    for await (const line of rl2) {
        if (!line.trim()) continue
        
        const fields = line.split("\t")
        if (fields.length < 19) continue
        
        processedCount++
        
        // Parse record
        const id = parseInt(fields[0])
        const name = fields[1]
        const asciiname = fields[2]
        const alternatenames = fields[3]
        const latitude = parseFloat(fields[4])
        const longitude = parseFloat(fields[5])
        const featureClass = fields[6]
        const featureCode = fields[7]
        const countryCode = fields[8]
        const cc2 = fields[9]
        const admin1 = fields[10]
        const admin2 = fields[11]
        const admin3 = fields[12]
        const admin4 = fields[13]
        const population = parseInt(fields[14]) || 0
        const elevation = parseInt(fields[15]) || null
        const dem = parseInt(fields[16]) || null
        const timezone = fields[17]
        const modificationDate = fields[18]
        
        // Determine level
        let level = 0
        if (featureCode === "PCLI" || featureCode.startsWith("PCL")) level = 0
        else if (featureCode === "ADM1") level = 1
        else if (featureCode === "ADM2") level = 2
        else if (featureCode === "ADM3") level = 3
        else if (featureCode === "ADM4") level = 4
        else if (featureCode === "ADM5") level = 5
        else {
            if (admin4) level = 5
            else if (admin3) level = 4
            else if (admin2) level = 3
            else if (admin1) level = 2
            else level = 1
        }
        
        // Find parent ID
        let parent = null
        
        if (featureCode === "ADM1") {
            const key = `${countryCode}|||||PCLI`
            parent = adminIndex.get(key)
            if (!parent) {
                for (const [k, v] of adminIndex.entries()) {
                    if (k.startsWith(`${countryCode}|||||PCL`)) {
                        parent = v
                        break
                    }
                }
            }
        } else if (featureCode === "ADM2") {
            const key = `${countryCode}|${admin1}||||ADM1`
            parent = adminIndex.get(key)
        } else if (featureCode === "ADM3") {
            const key = `${countryCode}|${admin1}|${admin2}|||ADM2`
            parent = adminIndex.get(key)
        } else if (featureCode === "ADM4") {
            const key = `${countryCode}|${admin1}|${admin2}|${admin3}||ADM3`
            parent = adminIndex.get(key)
        } else if (featureCode === "ADM5") {
            const key = `${countryCode}|${admin1}|${admin2}|${admin3}|${admin4}|ADM4`
            parent = adminIndex.get(key)
        } else if (featureCode !== "PCLI" && !featureCode.startsWith("PCL")) {
            if (admin4) {
                const key = `${countryCode}|${admin1}|${admin2}|${admin3}|${admin4}|ADM4`
                parent = adminIndex.get(key)
            }
            if (!parent && admin3) {
                const key = `${countryCode}|${admin1}|${admin2}|${admin3}||ADM3`
                parent = adminIndex.get(key)
            }
            if (!parent && admin2) {
                const key = `${countryCode}|${admin1}|${admin2}|||ADM2`
                parent = adminIndex.get(key)
            }
            if (!parent && admin1) {
                const key = `${countryCode}|${admin1}||||ADM1`
                parent = adminIndex.get(key)
            }
            if (!parent) {
                const key = `${countryCode}|||||PCLI`
                parent = adminIndex.get(key)
                if (!parent) {
                    for (const [k, v] of adminIndex.entries()) {
                        if (k.startsWith(`${countryCode}|||||PCL`)) {
                            parent = v
                            break
                        }
                    }
                }
            }
        }
        
        // Build record
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
            population,
            elevation,
            dem,
            timezone,
            modificationDate,
            level,
            parent,
            children: []
        }
        
        // Write to indexed path
        const pathSegments = Geo.path(id)
        const filename = pathSegments[pathSegments.length - 1] + ".json"
        const hashFilename = pathSegments[pathSegments.length - 1] + ".hash"
        const filePath = ["geo", "data", ...pathSegments.slice(0, -1), filename]
        const hashFilePath = ["geo", "data", ...pathSegments.slice(0, -1), hashFilename]
        const fullPath = "./" + filePath.join("/")
        const fullHashPath = "./" + hashFilePath.join("/")
        
        // Check if file exists with data - skip if already exists
        let needsWrite = true
        if (existsSync(fullPath) && existsSync(fullHashPath)) {
            const stats = statSync(fullPath)
            const hashStats = statSync(fullHashPath)
            
            // If both JSON and hash files exist with content, skip writing
            if (stats.size > 10 && hashStats.size > 0) {
                needsWrite = false
            }
        }
        
        if (needsWrite) {
            try {
                // Write JSON file
                await write(filePath, record)
                
                // Generate and write hash file immediately
                const fileHash = sha256(JSON.stringify(record))
                await write(hashFilePath, fileHash)
                
                writtenCount++
            } catch (error) {
                console.error(`Error writing record ${id}:`, error.message)
            }
        }
        
        // Show progress every 100k records
        if (processedCount % 100000 === 0) {
            console.log(`  Processed ${processedCount.toLocaleString()} | Skipped ${processedCount - writtenCount} | Written ${writtenCount.toLocaleString()} records...`)
        }
    }
    
    console.log(`\n✓ Processing complete!`)
    console.log(`  Total records processed: ${processedCount.toLocaleString()}`)
    console.log(`  Total records skipped (cached): ${(processedCount - writtenCount).toLocaleString()}`)
    console.log(`  Total records written: ${writtenCount.toLocaleString()}`)
    console.log(`  Admin features indexed: ${adminIndex.size.toLocaleString()}`)
    
    // ====================== STEP 5: BUILD CHILDREN RELATIONSHIPS ======================
    console.log("\nPass 3/4: Building children relationships...")
    const childrenMap = new Map()
    let relationCount = 0
    
    const rl3 = createInterface({
        input: createReadStream(allCountriesPath),
        crlfDelay: Infinity
    })
    
    // Build children map
    let processedRelations = 0
    for await (const line of rl3) {
        if (!line.trim()) continue
        
        const fields = line.split("\t")
        if (fields.length < 19) continue
        
        processedRelations++
        const id = parseInt(fields[0])
        const featureCode = fields[7]
        const countryCode = fields[8]
        const admin1 = fields[10]
        const admin2 = fields[11]
        const admin3 = fields[12]
        const admin4 = fields[13]
        
        // Find parent ID (same logic as Pass 2)
        let parent = null
        
        if (featureCode === "ADM1") {
            const key = `${countryCode}|||||PCLI`
            parent = adminIndex.get(key)
            if (!parent) {
                for (const [k, v] of adminIndex.entries()) {
                    if (k.startsWith(`${countryCode}|||||PCL`)) {
                        parent = v
                        break
                    }
                }
            }
        } else if (featureCode === "ADM2") {
            const key = `${countryCode}|${admin1}||||ADM1`
            parent = adminIndex.get(key)
        } else if (featureCode === "ADM3") {
            const key = `${countryCode}|${admin1}|${admin2}|||ADM2`
            parent = adminIndex.get(key)
        } else if (featureCode === "ADM4") {
            const key = `${countryCode}|${admin1}|${admin2}|${admin3}||ADM3`
            parent = adminIndex.get(key)
        } else if (featureCode === "ADM5") {
            const key = `${countryCode}|${admin1}|${admin2}|${admin3}|${admin4}|ADM4`
            parent = adminIndex.get(key)
        } else if (featureCode !== "PCLI" && !featureCode.startsWith("PCL")) {
            if (admin4) {
                const key = `${countryCode}|${admin1}|${admin2}|${admin3}|${admin4}|ADM4`
                parent = adminIndex.get(key)
            }
            if (!parent && admin3) {
                const key = `${countryCode}|${admin1}|${admin2}|${admin3}||ADM3`
                parent = adminIndex.get(key)
            }
            if (!parent && admin2) {
                const key = `${countryCode}|${admin1}|${admin2}|||ADM2`
                parent = adminIndex.get(key)
            }
            if (!parent && admin1) {
                const key = `${countryCode}|${admin1}||||ADM1`
                parent = adminIndex.get(key)
            }
            if (!parent) {
                const key = `${countryCode}|||||PCLI`
                parent = adminIndex.get(key)
                if (!parent) {
                    for (const [k, v] of adminIndex.entries()) {
                        if (k.startsWith(`${countryCode}|||||PCL`)) {
                            parent = v
                            break
                        }
                    }
                }
            }
        }
        
        if (parent) {
            if (!childrenMap.has(parent)) {
                childrenMap.set(parent, [])
            }
            childrenMap.get(parent).push(id)
            relationCount++
        }
        
        // Show progress every 100k records
        if (processedRelations % 100000 === 0) {
            console.log(`  Processed ${processedRelations.toLocaleString()} records | Mapped ${relationCount.toLocaleString()} relationships...`)
        }
    }
    
    console.log(`✓ Mapped ${relationCount.toLocaleString()} parent-child relationships for ${childrenMap.size.toLocaleString()} parents\n`)
    
    // Update parent files with children
    console.log("Updating parent files with children...")
    let updatedCount = 0
    
    for (const [parentId, children] of childrenMap.entries()) {
        const pathSegments = Geo.path(parentId)
        const filename = pathSegments[pathSegments.length - 1] + ".json"
        const filePath = ["geo", "data", ...pathSegments.slice(0, -1), filename]
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
    
    // ====================== STEP 6: GENERATE DIRECTORY HASH FILES ======================
    console.log("Pass 4/4: Generating directory hash files (_.hash) for geo data...")
    totalDirHashCount = 0
    const dirHashCount = await generateGeoDirectoryHashes(["geo", "data"])
    console.log(`✓ Generated ${dirHashCount.toLocaleString()} directory hash files\n`)
    
    console.log("\nBuild complete!")
}