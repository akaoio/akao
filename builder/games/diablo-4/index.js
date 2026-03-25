import { log } from "../../core/logger.js"
import { crawlDiablo4Items } from "./crawler.js"

// Wowhead quality tiers mapped to D4 rarity labels
const QUALITY_MAP = {
    0: "Normal",
    1: "Common",
    2: "Uncommon",
    3: "Rare",
    4: "Epic",
    5: "Unique",
    6: "Legendary",
    8: "Mythic",
}

// Wowhead icon CDN URL: prefix = iconId % 256
function buildIconUrl(iconId) {
    const prefix = iconId % 256
    return `https://wow.zamimg.com/d4/d4/texture/hash/${prefix}/${iconId}.webp`
}

function catalogMapper(raw) {
    return {
        id: String(raw.id),
        name: raw.name,
        icon: raw.icon != null ? buildIconUrl(raw.icon) : null,
        rarity: QUALITY_MAP[raw.quality] ?? "Special",
        type: raw.itemTypeName || "Misc",
        classes: raw.playerClassNames?.length ? raw.playerClassNames : null,
        slots: raw.slotNames?.length ? raw.slotNames : null,
        value: raw.popularity ?? 0,
    }
}

function detailMapper(raw) {
    const normalized = catalogMapper(raw)
    return {
        meta: {
            game: "diablo-4",
            type: normalized.type || "Misc",
            tags: ["diablo-4", normalized.rarity?.toLowerCase(), (normalized.type || "").toLowerCase()].filter(Boolean),
            rarity: normalized.rarity,
            itemTypeName: raw.itemTypeName || null,
            classes: normalized.classes,
            slots: normalized.slots,
            value: normalized.value,
        },
        locale: {
            name: raw.name,
            description: "",
        },
    }
}

export const gameDiablo4 = {
    id: "diablo-4",
    catalogMapper,
    detailMapper,
    /** Maps a raw item to its source image filename(s) in games/diablo-4/images/. */
    imageResolver(raw) {
        return raw.icon != null ? [`${raw.icon}.webp`] : []
    },
    async build(context) {
        const summary = await crawlDiablo4Items({
            output: context.output,
            dryRun: context.dryRun,
        })

        log.ok(
            `Diablo 4: ${summary.totalItems} items, ${summary.pagesFetched} pages, ${summary.downloadedImages} new images`,
        )
    },
}
