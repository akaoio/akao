import { readFileSync, readdirSync, statSync, existsSync, createReadStream } from "fs"
import { join } from "path"
import { createInterface } from "readline"
import { COUNTRY_FEATURE_REGEX } from "./shared.js"

// === Shared utilities ===

function walkDir(dir, callback) {
    if (!existsSync(dir)) return
    const entries = readdirSync(dir)
    for (const entry of entries) {
        const fullPath = join(dir, entry)
        const stat = statSync(fullPath)
        if (stat.isDirectory()) {
            walkDir(fullPath, callback)
        } else if (entry.endsWith(".json") && entry !== "countries.json" && entry !== "features.json") {
            callback(fullPath)
        }
    }
}

function loadAllRecordsFromDir(baseDir) {
    const records = new Map()
    walkDir(baseDir, (filePath) => {
        try {
            const content = readFileSync(filePath, "utf-8")
            const data = JSON.parse(content)
            records.set(data.id, data)
        } catch (error) {
            // Skip invalid files
        }
    })
    return records
}

// Load parent info from raw data to check parent country and feature code
async function loadParentInfo(parentIds) {
    const parentInfo = new Map()
    const geoPath = "geo/allCountries.txt"
    
    if (!existsSync(geoPath)) {
        console.warn("⚠ allCountries.txt not found - parent classification unavailable")
        return parentInfo
    }
    
    if (parentIds.size === 0) return parentInfo
    
    console.log(`  Loading info for ${parentIds.size} missing parents from allCountries.txt...`)
    
    return new Promise((resolve, reject) => {
        const stream = createReadStream(geoPath, { encoding: "utf-8" })
        const rl = createInterface({ input: stream, crlfDelay: Infinity })
        
        let found = 0
        
        rl.on("line", (line) => {
            if (!line.trim()) return
            
            const fields = line.split("\t")
            const id = parseInt(fields[0])
            
            if (parentIds.has(id)) {
                parentInfo.set(id, {
                    id,
                    name: fields[1],
                    featureCode: fields[7],
                    countryCode: fields[8]
                })
                found++
                
                if (found === parentIds.size) {
                    rl.close()
                    stream.destroy()
                }
            }
        })
        
        rl.on("close", () => {
            console.log(`  ✓ Found ${found}/${parentIds.size} parent records`)
            resolve(parentInfo)
        })
        
        rl.on("error", (error) => {
            console.warn(`  ⚠ Error reading allCountries.txt: ${error.message}`)
            resolve(parentInfo)
        })
    })
}

async function validateCountry(countryCode, records) {
    console.log(`\n${"=".repeat(70)}`)
    console.log(`VALIDATION REPORT: ${countryCode}`)
    console.log("=".repeat(70) + "\n")

    const countryRecords = Array.from(records.values()).filter((r) => r.countryCode === countryCode)
    const childrenMap = new Map()

    // Build children map
    for (const record of countryRecords) {
        if (record.children && record.children.length > 0) {
            childrenMap.set(record.id, record.children)
        }
    }

    // Collect all missing parent IDs
    const missingParentIds = new Set()
    for (const record of countryRecords) {
        if (record.parent && !records.has(record.parent)) {
            missingParentIds.add(record.parent)
        }
    }

    // Load parent info from raw data
    const parentInfo = await loadParentInfo(missingParentIds)

    const stats = {
        total: countryRecords.length,
        countries: 0,
        countriesWithChildren: 0,
        adm1: 0,
        adm1WithParent: 0,
        adm1Orphans: 0,
        adm2: 0,
        adm2WithParent: 0,
        adm2Orphans: 0,
        adm3: 0,
        adm4: 0,
        adm5: 0,
        orphans: [],
        missingParentsHistorical: [],
        missingParentsWrongCountry: [],
        missingParentsUnknown: []
    }

    for (const record of countryRecords) {
        const fc = record.featureCode
        const level = record.level  // Use level field from build

        if (/^(PCLI|PCLD|PCLF|PCLIS|PCLIX|PCLS)$/.test(fc)) {
            stats.countries++
            const children = childrenMap.get(record.id) || []
            if (children.length > 0) {
                stats.countriesWithChildren++
            } else {
                stats.orphans.push(`⚠ Country ${record.name} (${record.id}) has no children`)
            }
        } else if (level === 1) {
            // All level-1 administrative units (regardless of feature code)
            stats.adm1++
            if (record.parent) {
                stats.adm1WithParent++
                const parent = records.get(record.parent)
                if (!parent) {
                    const pInfo = parentInfo.get(record.parent)
                    if (pInfo) {
                        if (pInfo.featureCode.endsWith("H")) {
                            stats.missingParentsHistorical.push({
                                child: { id: record.id, name: record.name, fc },
                                parent: pInfo
                            })
                        } else if (pInfo.countryCode !== countryCode) {
                            stats.missingParentsWrongCountry.push({
                                child: { id: record.id, name: record.name, fc },
                                parent: pInfo
                            })
                        } else {
                            stats.missingParentsUnknown.push({
                                child: { id: record.id, name: record.name, fc },
                                parentId: record.parent
                            })
                        }
                    } else {
                        stats.missingParentsUnknown.push({
                            child: { id: record.id, name: record.name, fc },
                            parentId: record.parent
                        })
                    }
                } else if (!parent.children || !parent.children.includes(record.id)) {
                    stats.orphans.push(`✗ Level-1 ${record.name} (${record.id}) [${fc}] not listed in parent's children`)
                }
            } else {
                stats.adm1Orphans++
                stats.orphans.push(`✗ Level-1 ${record.name} (${record.id}) [${fc}] has no parent`)
            }
        } else if (level === 2) {
            // All level-2 administrative units
            stats.adm2++
            if (record.parent) {
                stats.adm2WithParent++
                const parent = records.get(record.parent)
                if (!parent) {
                    const pInfo = parentInfo.get(record.parent)
                    if (pInfo) {
                        if (pInfo.featureCode.endsWith("H")) {
                            stats.missingParentsHistorical.push({
                                child: { id: record.id, name: record.name, fc },
                                parent: pInfo
                            })
                        } else if (pInfo.countryCode !== countryCode) {
                            stats.missingParentsWrongCountry.push({
                                child: { id: record.id, name: record.name, fc },
                                parent: pInfo
                            })
                        } else {
                            stats.missingParentsUnknown.push({
                                child: { id: record.id, name: record.name, fc },
                                parentId: record.parent
                            })
                        }
                    } else {
                        stats.missingParentsUnknown.push({
                            child: { id: record.id, name: record.name, fc },
                            parentId: record.parent
                        })
                    }
                } else if (!parent.children || !parent.children.includes(record.id)) {
                    stats.orphans.push(`✗ Level-2 ${record.name} (${record.id}) [${fc}] not listed in parent's children`)
                }
            } else {
                stats.adm2Orphans++
            }
        } else if (level === 3) {
            // All level-3 administrative units
            stats.adm3++
            if (record.parent) {
                const parent = records.get(record.parent)
                if (!parent) {
                    const pInfo = parentInfo.get(record.parent)
                    if (pInfo) {
                        if (pInfo.featureCode.endsWith("H")) {
                            stats.missingParentsHistorical.push({
                                child: { id: record.id, name: record.name, fc },
                                parent: pInfo
                            })
                        } else if (pInfo.countryCode !== countryCode) {
                            stats.missingParentsWrongCountry.push({
                                child: { id: record.id, name: record.name, fc },
                                parent: pInfo
                            })
                        } else {
                            stats.missingParentsUnknown.push({
                                child: { id: record.id, name: record.name, fc },
                                parentId: record.parent
                            })
                        }
                    } else {
                        stats.missingParentsUnknown.push({
                            child: { id: record.id, name: record.name, fc },
                            parentId: record.parent
                        })
                    }
                } else if (!parent.children || !parent.children.includes(record.id)) {
                    stats.orphans.push(`✗ Level-3 ${record.name} (${record.id}) [${fc}] not listed in parent's children`)
                }
            }
        } else if (level === 4) {
            // All level-4 administrative units
            stats.adm4++
            if (record.parent) {
                const parent = records.get(record.parent)
                if (!parent) {
                    const pInfo = parentInfo.get(record.parent)
                    if (pInfo) {
                        if (pInfo.featureCode.endsWith("H")) {
                            stats.missingParentsHistorical.push({
                                child: { id: record.id, name: record.name, fc },
                                parent: pInfo
                            })
                        } else if (pInfo.countryCode !== countryCode) {
                            stats.missingParentsWrongCountry.push({
                                child: { id: record.id, name: record.name, fc },
                                parent: pInfo
                            })
                        } else {
                            stats.missingParentsUnknown.push({
                                child: { id: record.id, name: record.name, fc },
                                parentId: record.parent
                            })
                        }
                    } else {
                        stats.missingParentsUnknown.push({
                            child: { id: record.id, name: record.name, fc },
                            parentId: record.parent
                        })
                    }
                } else if (!parent.children || !parent.children.includes(record.id)) {
                    stats.orphans.push(`✗ Level-4 ${record.name} (${record.id}) [${fc}] not listed in parent's children`)
                }
            }
        } else if (level === 5) {
            // All level-5 administrative units
            stats.adm5++
        }
    }

    // Print statistics
    console.log("Administrative Hierarchy Statistics:")
    console.log("-".repeat(70))
    console.log(`Total records:                 ${stats.total.toLocaleString()}`)
    console.log(``)
    console.log(`Countries (PCLI/PCL*):         ${stats.countries.toLocaleString()}`)
    console.log(`  └─ With children:             ${stats.countriesWithChildren.toLocaleString()}`)
    console.log(`  └─ Without children:          ${stats.countries - stats.countriesWithChildren}`)
    console.log(``)
    console.log(`ADM1 divisions:                 ${stats.adm1.toLocaleString()}`)
    console.log(`  └─ With parent:               ${stats.adm1WithParent.toLocaleString()}`)
    console.log(`  └─ Orphans (no parent):       ${stats.adm1Orphans.toLocaleString()}`)
    console.log(``)
    console.log(`ADM2 divisions:                 ${stats.adm2.toLocaleString()}`)
    console.log(`  └─ With parent:               ${stats.adm2WithParent.toLocaleString()}`)
    console.log(`  └─ Orphans (no parent):       ${stats.adm2Orphans.toLocaleString()}`)
    console.log(``)
    console.log(`ADM3 divisions:                 ${stats.adm3.toLocaleString()}`)
    console.log(`ADM4 divisions:                 ${stats.adm4.toLocaleString()}`)
    console.log(`ADM5 divisions:                 ${stats.adm5.toLocaleString()}`)
    console.log(``)

    const totalIssues =
        stats.orphans.length +
        stats.missingParentsHistorical.length +
        stats.missingParentsWrongCountry.length +
        stats.missingParentsUnknown.length

    if (stats.missingParentsHistorical.length > 0) {
        console.log("\n⚠️  Parents are Historical (feature code ends with H):")
        console.log("-".repeat(70))
        const display = stats.missingParentsHistorical.slice(0, 10)
        display.forEach((m) =>
            console.log(
                `  ℹ ${m.child.name} (${m.child.id}) [${m.child.fc}] → parent: ${m.parent.name} (${m.parent.id}) [${m.parent.featureCode}]`
            )
        )
        if (stats.missingParentsHistorical.length > 10) {
            console.log(`  ... and ${stats.missingParentsHistorical.length - 10} more historical parent references`)
        }
    }

    if (stats.missingParentsWrongCountry.length > 0) {
        console.log("\n⚠️  Parents belong to different country:")
        console.log("-".repeat(70))
        const display = stats.missingParentsWrongCountry.slice(0, 10)
        display.forEach((m) =>
            console.log(
                `  ℹ ${m.child.name} (${m.child.id}) [${m.child.fc}] → parent: ${m.parent.name} (${m.parent.id}) [${m.parent.countryCode}]`
            )
        )
        if (stats.missingParentsWrongCountry.length > 10) {
            console.log(`  ... and ${stats.missingParentsWrongCountry.length - 10} more cross-country parent references`)
        }
    }

    if (stats.missingParentsUnknown.length > 0) {
        console.log("\n❌ Unknown Missing Parents:")
        console.log("-".repeat(70))
        const display = stats.missingParentsUnknown.slice(0, 10)
        display.forEach((m) =>
            console.log(`  ✗ ${m.child.name} (${m.child.id}) [${m.child.fc}] parent=${m.parentId} NOT FOUND`)
        )
        if (stats.missingParentsUnknown.length > 10) {
            console.log(`  ... and ${stats.missingParentsUnknown.length - 10} more unknown missing parents`)
        }
    }

    if (stats.orphans.length > 0) {
        console.log("\n⚠️  Children Relationship Issues:")
        console.log("-".repeat(70))
        const display = stats.orphans.slice(0, 10)
        display.forEach((o) => console.log(`  ${o}`))
        if (stats.orphans.length > 10) {
            console.log(`  ... and ${stats.orphans.length - 10} more children relationship issues`)
        }
    }

    if (totalIssues === 0) {
        console.log("✅ No issues found!")
    }

    console.log("\n" + "=".repeat(70) + "\n")

    return { stats, issues: totalIssues }
}

// === Export for build pipeline ===
export function validateHierarchy(recordsMap) {
    console.log("\n" + "=".repeat(70))
    console.log("HIERARCHY VALIDATION")
    console.log("=".repeat(70) + "\n")

    const childrenMap = new Map()
    const stats = {
        countries: 0,
        countriesWithChildren: 0,
        adm1: 0,
        adm1WithParent: 0,
        adm1Orphans: 0,
        adm2: 0,
        adm2WithParent: 0,
        adm2Orphans: 0,
        adm3: 0,
        adm4: 0,
        adm5: 0
    }

    // Build children map
    for (const record of recordsMap.values()) {
        if (record.children && record.children.length > 0) {
            childrenMap.set(record.id, record.children)
        }
    }

    // Collect stats
    for (const record of recordsMap.values()) {
        const fc = record.featureCode
        const level = record.level

        if (COUNTRY_FEATURE_REGEX.test(fc)) {
            stats.countries++
            const children = childrenMap.get(record.id) || []
            if (children.length > 0) stats.countriesWithChildren++
        } else if (level === 1) {
            // All level-1 administrative units
            stats.adm1++
            if (record.parent) stats.adm1WithParent++
            else stats.adm1Orphans++
        } else if (level === 2) {
            // All level-2 administrative units
            stats.adm2++
            if (record.parent) stats.adm2WithParent++
            else stats.adm2Orphans++
        } else if (level === 3) {
            stats.adm3++
        } else if (level === 4) {
            stats.adm4++
        } else if (level === 5) {
            stats.adm5++
        }
    }

    console.log(`Countries (PCLI/PCL*):         ${stats.countries.toLocaleString()}`)
    console.log(`  └─ With children:             ${stats.countriesWithChildren.toLocaleString()} (${Math.round((stats.countriesWithChildren / stats.countries) * 100)}%)`)
    console.log(``)
    console.log(`ADM1 divisions:                 ${stats.adm1.toLocaleString()}`)
    console.log(`  └─ With parent:               ${stats.adm1WithParent.toLocaleString()} (${Math.round((stats.adm1WithParent / stats.adm1) * 100)}%)`)
    if (stats.adm1Orphans > 0) {
        console.log(`  └─ ⚠ Orphans (no parent):     ${stats.adm1Orphans.toLocaleString()}`)
    }
    console.log(``)
    console.log(`ADM2 divisions:                 ${stats.adm2.toLocaleString()}`)
    console.log(`  └─ With parent:               ${stats.adm2WithParent.toLocaleString()} (${Math.round((stats.adm2WithParent / stats.adm2) * 100)}%)`)
    if (stats.adm2Orphans > 0) {
        console.log(`  └─ ⚠ Orphans (no parent):     ${stats.adm2Orphans.toLocaleString()}`)
    }
    console.log(``)
    console.log(`ADM3 divisions:                 ${stats.adm3.toLocaleString()}`)
    console.log(`ADM4 divisions:                 ${stats.adm4.toLocaleString()}`)
    console.log(`ADM5 divisions:                 ${stats.adm5.toLocaleString()}`)

    if (stats.adm1Orphans === 0 && stats.adm2Orphans === 0) {
        console.log(`\n✓ No issues found!`)
    }

    console.log("\n" + "=".repeat(70) + "\n")
}

// Export individual functions for test.js
export { validateCountry, loadAllRecordsFromDir as loadAllRecords }

// === CLI execution (when run directly) ===
if (import.meta.url === `file://${process.argv[1]}`) {
    ;(async () => {
        const args = process.argv.slice(2)
        const countryArg = args.find((arg) => arg.startsWith("--country="))
        const targetCountry = countryArg ? countryArg.split("=")[1].toUpperCase() : null

        console.log("Loading records from temp/geo/...")
        const records = loadAllRecordsFromDir("temp/geo")
        console.log(`✓ Loaded ${records.size.toLocaleString()} records\n`)

        if (targetCountry) {
            // Validate single country
            await validateCountry(targetCountry, records)
        } else {
            // Find all countries in temp/geo and validate each
            const countries = new Set()
            for (const record of records.values()) {
                if (record.countryCode) {
                    countries.add(record.countryCode)
                }
            }

            const sortedCountries = Array.from(countries).sort()
            console.log(`Found ${sortedCountries.length} countries in temp/geo/`)
            console.log(`Countries: ${sortedCountries.join(", ")}\n`)

            let totalIssues = 0
            for (const countryCode of sortedCountries) {
                const result = await validateCountry(countryCode, records)
                totalIssues += result.issues
            }

            console.log("\n" + "=".repeat(70))
            console.log(`SUMMARY: ${sortedCountries.length} countries validated, ${totalIssues} total issues`)
            console.log("=".repeat(70) + "\n")
        }
    })()
}
