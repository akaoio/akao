import { existsSync, statSync } from "fs"
import AdmZip from "adm-zip"
import { FS } from "../../src/core/FS.js"

const base = "http://download.geonames.org/export/dump/"
const files = ["readme.txt", "countryInfo.txt", "allCountries.zip", "featureCodes_en.txt", "hierarchy.zip", "adminCode5.zip"]
const zipTargets = {
    "hierarchy.zip": "hierarchy.txt",
    "adminCode5.zip": "adminCode5.txt"
}

function extractZipIfNeeded(zipPath, outputDir, expectedFile) {
    const outPath = `${outputDir}/${expectedFile}`
    if (existsSync(outPath)) return
    try {
        const zip = new AdmZip(zipPath)
        zip.extractAllTo(outputDir, true)
        console.log(`✓ Extracted ${expectedFile} from ${zipPath}`)
    } catch (error) {
        console.error(`✗ Failed to extract ${zipPath}:`, error.message)
    }
}

export async function downloadGeoData() {
    console.log("Starting GeoNames data download...\n")

    const now = Date.now()
    const oneDayMs = 24 * 60 * 60 * 1000

    for (const file of files) {
        const filePath = `./geo/${file}`
        const isZip = file.endsWith(".zip")
        const expected = zipTargets[file]

        let needsDownload = true
        if (existsSync(filePath)) {
            const stats = statSync(filePath)
            const ageMs = now - stats.mtimeMs
            if (ageMs < oneDayMs) {
                const hoursAgo = Math.floor(ageMs / (60 * 60 * 1000))
                console.log(`⊘ Skipping ${file} (downloaded ${hoursAgo}h ago)\n`)
                needsDownload = false
            }
        }

        if (needsDownload) {
            const url = base + file
            console.log(`Downloading ${file}...`)
            const result = await FS.download(url, ["geo", file])
            if (result?.success) {
                console.log(`✓ Downloaded: ${result.path}\n`)
            } else {
                console.log(`✗ Failed to download: ${file}\n`)
            }
        }

        if (isZip && expected) {
            extractZipIfNeeded(filePath, "./geo", expected)
        }
    }

    console.log("Download complete!")
}
