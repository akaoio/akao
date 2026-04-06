import { FS } from "../../src/core/FS.js"
import { paths } from "./config.js"

// ============ i18n Processing ============

export async function processI18n(locales, outputBase = "build") {
    const dynamicPaths = outputBase === "build" ? paths : {
        ...paths,
        build: {
            ...paths.build,
            locales: [outputBase]
        }
    }
    
    const i18nFiles = await FS.dir(paths.src.i18n)
    const localeData = Object.fromEntries(locales.map((l) => [l, {}]))

    for (const file of i18nFiles) {
        const keyName = file.replace(/\.(json|yaml|yml)$/, "")
        const translations = await FS.load([...paths.src.i18n, file])

        for (const locale of locales) if (translations?.[locale]) localeData[locale][keyName] = translations[locale]
    }

    for (const locale of locales) {
        const sorted = Object.fromEntries(Object.entries(localeData[locale]).sort(([a], [b]) => a.localeCompare(b)))
        await FS.write([...dynamicPaths.build.locales, `${locale}.json`], sorted)
    }

    return locales.length
}
