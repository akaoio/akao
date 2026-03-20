import { log } from "../../core/logger.js"
import { crawlArcRaidersItems } from "./crawler.js"

function normalizeString(str) {
    if (!str) return null
    return str.replace(/\b\w/g, (c) => c.toUpperCase()).trim() || null
}

function catalogMapper(raw) {
    return {
        id: raw.id,
        name: raw.name,
        icon: raw.icon || null,
        rarity: raw.rarity || "Common",
        type: normalizeString(raw.item_type) || "Misc",
        subtype: normalizeString(raw.subcategory),
        value: raw.value ?? 0,
    }
}

export const gameArcRaiders = {
    id: "arc-raiders",
    catalogMapper,
    async build(context) {
        const summary = await crawlArcRaidersItems({
            output: context.output,
            dryRun: context.dryRun,
        })

        log.ok(
            `Arc Raiders: ${summary.totalItems} items, ${summary.pagesFetched} pages, ${summary.downloadedImages} new images`,
        )
    },
}