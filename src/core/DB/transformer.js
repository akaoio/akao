/**
 * transformer — maps IDB path + data to a SQL upsert operation.
 *
 * Returns { schema, upsert, values } for known paths, or null to skip sync.
 *
 * Path conventions:
 *   ["statics","items", gameId, itemId, locale.json]  rest.length=3 → game_items
 *   ["statics","items", itemId, locale.json]           rest.length=2 → shop_items
 *
 * Paths that return null (no SQL sync):
 *   - hash files          (path ends with ".hash")
 *   - page index files    (data is an Array)
 *   - non-statics paths
 *   - unknown structures
 */
import { SCHEMA_GAME_ITEMS } from "./schemas/game-items.js"
import { SCHEMA_SHOP_ITEMS } from "./schemas/shop-items.js"

const GAME_ITEMS_DELETE_SQL = "DELETE FROM game_items WHERE id=? AND game_id=? AND locale=?"
const SHOP_ITEMS_DELETE_SQL = "DELETE FROM shop_items WHERE id=? AND locale=?"

export function getLocaleFromFilename(filename) {
    if (typeof filename !== "string" || !filename.endsWith(".json")) return null
    if (filename === "meta.json") return null
    const locale = filename.slice(0, -".json".length)
    if (!/^[a-z]{2}(?:-[A-Z]{2})?$/.test(locale)) return null
    return locale
}

export function transform(path, data) {
    if (!Array.isArray(path) || path.length === 0) return null
    if (path.at(-1)?.endsWith(".hash")) return null
    if (path[0] !== "statics") return null

    const [, domain, ...rest] = path

    // Game item: ["statics","items","gameId","itemId","vi.json"]
    if (domain === "items" && rest.length === 3) {
        const [gameId, itemId, filename] = rest
        const locale = getLocaleFromFilename(filename)
        if (!locale) return null
        if (data === null)
            return {
                schema: SCHEMA_GAME_ITEMS,
                delete: GAME_ITEMS_DELETE_SQL,
                values: [itemId, gameId, locale],
            }
        if (!data || typeof data !== "object" || Array.isArray(data)) return null
        const values = [
            itemId,
            gameId,
            locale,
            data.name   ?? null,
            data.rarity ?? null,
            data.type   ?? null,
            data.value  ?? null,
            data.images?.[0] ? `/statics/items/${gameId}/${itemId}/images/${data.images[0]}` : null,
            JSON.stringify(data),
            JSON.stringify(path),
            Math.floor(Date.now() / 1000)
        ]
        const cols = "(id,game_id,locale,name,rarity,type,value,icon,data,path,synced_at)"
        const vals = "VALUES (?,?,?,?,?,?,?,?,?,?,?)"
        return {
            schema: SCHEMA_GAME_ITEMS,
            upsert: `INSERT OR REPLACE INTO game_items ${cols} ${vals}`,
            insert: `INSERT OR IGNORE  INTO game_items ${cols} ${vals}`,
            delete: GAME_ITEMS_DELETE_SQL,
            values,
        }
    }

    // Shop item: ["statics","items","itemId","vi.json"]
    if (domain === "items" && rest.length === 2) {
        const [itemId, filename] = rest
        const locale = getLocaleFromFilename(filename)
        if (!locale) return null
        if (data === null)
            return {
                schema: SCHEMA_SHOP_ITEMS,
                delete: SHOP_ITEMS_DELETE_SQL,
                values: [itemId, locale],
            }
        if (!data || typeof data !== "object" || Array.isArray(data)) return null
        const values = [
            data.id ?? itemId,
            locale,
            data.name     ?? null,
            data.price    ?? null,
            data.currency ?? null,
            data.category ?? null,
            JSON.stringify(data),
            JSON.stringify(path),
            Math.floor(Date.now() / 1000)
        ]
        const cols = "(id,locale,name,price,currency,category,data,path,synced_at)"
        const vals = "VALUES (?,?,?,?,?,?,?,?,?)"
        return {
            schema: SCHEMA_SHOP_ITEMS,
            upsert: `INSERT OR REPLACE INTO shop_items ${cols} ${vals}`,
            insert: `INSERT OR IGNORE  INTO shop_items ${cols} ${vals}`,
            delete: SHOP_ITEMS_DELETE_SQL,
            values,
        }
    }

    return null
}
