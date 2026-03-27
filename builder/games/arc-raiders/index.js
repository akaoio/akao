import { log } from "../../core/logger.js"
import { crawlArcRaidersItems } from "./crawler.js"

function normalizeString(str) {
    if (!str) return null
    return str.replace(/\b\w/g, (c) => c.toUpperCase()).trim() || null
}

function sparseStat(stat_block) {
    if (!stat_block) return null
    const entries = Object.entries(stat_block).filter(([, v]) => v !== 0 && v !== "" && v !== null)
    return entries.length > 0 ? Object.fromEntries(entries) : null
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

function detailMapper(raw) {
    return {
        meta: {
            catalog: "game",
            game: "arc-raiders",
            tags: [raw.rarity?.toLowerCase(), normalizeString(raw.item_type)?.toLowerCase()].filter(Boolean),
            rarity: raw.rarity || "Common",
            type: normalizeString(raw.item_type) || "Misc",
            subtype: normalizeString(raw.subcategory),
            value: raw.value ?? 0,
            stat_block: sparseStat(raw.stat_block),
            loadout_slots: raw.loadout_slots || [],
            workbench: raw.workbench || null,
            flavor_text: raw.flavor_text || null,
            ammo_type: raw.ammo_type || null,
            shield_type: raw.shield_type || null,
            loot_area: raw.loot_area || null,
            sources: raw.sources || null,
            locations: raw.locations || [],
        },
        locale: {
            name: raw.name,
            description: raw.description || "",
        },
    }
}

export const gameArcRaiders = {
    id: "arc-raiders",
    catalogMapper,
    detailMapper,
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