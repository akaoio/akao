/**
 * sync.js — Non-destructive sync of game-derived items to src/statics/items/<game-id>/<item-id>/
 * and their compiled JSON to build/statics/items/<game-id>/<item-id>/.
 *
 * Responsibilities:
 *   - write raw.yaml per item (crawler data, icon field stripped)
 *   - sync YAML sources to src/ (non-destructive: patch meta.yaml, create locale only if missing)
 *   - compile JSON to build/ (non-destructive: patch meta.json, create locale only if missing)
 *   - hash build/statics/items/<game-id>/ after writing
 *
 * NOT responsible for:
 *   - pagination (owned by build:core)
 *   - core-managed items (owned by build:core)
 */

import crypto from "node:crypto"
import fsNative from "node:fs"
import pathNative from "node:path"
import { write, load, exist, dir, copy, isDirectory } from "../../src/core/FS.js"
import { sha256 } from "../../src/core/Utils/crypto.js"
import { generateHashFiles } from "../core/hash.js"
import { paths } from "../core/config.js"
import { log } from "../core/logger.js"

const LOCALES_PATH = [...paths.src.statics, "locales.yaml"]

const VALID_TYPES = new Set(["item", "service", "account", "currency"])
const FORBIDDEN_META_FIELDS = ["price", "sale", "currency"]
const WARNED_UNKNOWN_TYPES = new Set()

const DEFAULT_CONFIGS = Object.freeze({
    min: { quantity: null, value: null },
    max: { quantity: null, value: null },
    step: null,
})

function stableStringify(value) {
    if (value === null || typeof value !== "object") return JSON.stringify(value)
    if (Array.isArray(value)) return `[${value.map(stableStringify).join(",")}]`
    const keys = Object.keys(value).sort()
    return `{${keys.map((k) => `${JSON.stringify(k)}:${stableStringify(value[k])}`).join(",")}}`
}

function slugify(value) {
    return String(value || "")
        .normalize("NFKD")
        .replace(/[\u0300-\u036f]/g, "")
        .replace(/[^\p{L}\p{N}]+/gu, "-")
        .replace(/^-+|-+$/g, "")
        .replace(/-+/g, "-")
        .toLowerCase()
}

function buildCanonicalItemId(raw, localeData) {
    const preferredName = localeData?.name || raw?.name || raw?.id || "item"
    const namedSlug = slugify(preferredName) || "item"
    const rawBlock = stableStringify(raw)
    const hash = sha256(rawBlock)
    const suffix = hash.slice(-5)
    return `${namedSlug}-${suffix}`
}

function ensureConfigs(meta = {}) {
    const current = meta.configs || {}
    const currentMin = current.min || {}
    const currentMax = current.max || {}

    return {
        ...meta,
        configs: {
            min: {
                quantity: currentMin.quantity ?? DEFAULT_CONFIGS.min.quantity,
                value: currentMin.value ?? DEFAULT_CONFIGS.min.value,
            },
            max: {
                quantity: currentMax.quantity ?? DEFAULT_CONFIGS.max.quantity,
                value: currentMax.value ?? DEFAULT_CONFIGS.max.value,
            },
            step: current.step ?? DEFAULT_CONFIGS.step,
        },
    }
}

function normalizeLocaleData(locale = {}, fallback = {}) {
    let source = locale

    if (source && typeof source === "object" && !Array.isArray(source) && source.en && typeof source.en === "object") {
        source = source.en
    }

    const name = source?.name ?? fallback?.name ?? ""
    const description = source?.description ?? fallback?.description ?? ""

    return { name, description }
}

function enforceType(type, itemId) {
    if (VALID_TYPES.has(type)) return type
    const unknownType = String(type)
    if (!WARNED_UNKNOWN_TYPES.has(unknownType)) {
        WARNED_UNKNOWN_TYPES.add(unknownType)
        log.info(`    warn: unknown type "${unknownType}" (first seen at ${itemId}), falling back to "item"`)
    }
    return "item"
}

async function getLocaleCodes() {
    const config = await load(LOCALES_PATH)
    return config ? config.map((l) => l.code) : []
}

/**
 * Merge meta: preserve dev-owned fields, upsert system fields.
 * @param {object} existing - parsed existing meta.yaml (or null)
 * @param {object} incoming - normalized meta from detailMapper
 */
function mergeMeta(existing, incoming) {
    if (!existing) return incoming

    // Fields that belong to the dev — never overwrite
    const preserved = {}
    if (existing.tags !== undefined) preserved.tags = existing.tags
    if (existing.configs !== undefined) preserved.configs = existing.configs

    // Custom fields not in the system set are also preserved
    const systemFields = new Set(["id", "game", "type", "images", "icon"])
    for (const [k, v] of Object.entries(existing)) {
        if (!systemFields.has(k) && !(k in preserved)) preserved[k] = v
    }

    return { ...incoming, ...preserved }
}

function isImageFile(name) {
    return /\.(webp|png|jpg|jpeg|gif|avif|svg)$/i.test(name)
}

function sortImageNames(names = []) {
    return [...names].sort((a, b) => {
        const aStem = a.replace(/\.[^.]+$/, "")
        const bStem = b.replace(/\.[^.]+$/, "")
        const aNum = Number(aStem)
        const bNum = Number(bStem)
        const aIsNum = Number.isFinite(aNum)
        const bIsNum = Number.isFinite(bNum)

        if (aIsNum && bIsNum && aNum !== bNum) return aNum - bNum
        return a.localeCompare(b)
    })
}

async function buildSourceImageMap(gameId) {
    const imageDir = ["games", gameId, "images"]
    if (!(await exist(imageDir))) return new Map()

    const files = await dir(imageDir)
    const map = new Map()

    for (const file of files) {
        if (!isImageFile(file)) continue
        const stem = file.replace(/\.[^.]+$/, "")
        if (!map.has(stem)) map.set(stem, [])
        map.get(stem).push(file)
    }

    for (const [key, values] of map.entries()) {
        map.set(key, values.sort((a, b) => a.localeCompare(b)))
    }

    return map
}

async function listItemImages(itemDir) {
    const imagesDir = [...itemDir, "images"]
    if (!(await exist(imagesDir))) return []
    const files = await dir(imagesDir)
    return sortImageNames(files.filter((name) => isImageFile(name)))
}

function hashImageBytes(absPath) {
    try {
        const bytes = fsNative.readFileSync(absPath)
        return crypto.createHash("sha256").update(bytes).digest("hex")
    } catch {
        return null
    }
}

/**
 * Sync images from games/<gameId>/images/<itemId>.* into src item dir.
 * Uses content hash to dedupe: skips source images whose hash already exists in the dest dir.
 * New images are appended as next integer index (0.webp, 1.webp, ...).
 */
async function syncItemImages(gameId, srcDir, sourceFiles = []) {
    if (sourceFiles.length === 0) return 0

    const existingImages = await listItemImages(srcDir)
    const existingHashes = new Set()
    for (const name of existingImages) {
        const h = hashImageBytes(pathNative.join(...srcDir, "images", name))
        if (h) existingHashes.add(h)
    }

    let nextIndex = existingImages.length
    let copied = 0
    for (const sourceFile of sourceFiles) {
        const sourceAbsPath = pathNative.join("games", gameId, "images", sourceFile)
        const sourceHash = hashImageBytes(sourceAbsPath)
        if (!sourceHash || existingHashes.has(sourceHash)) continue

        const ext = sourceFile.includes(".") ? sourceFile.slice(sourceFile.lastIndexOf(".")) : ".webp"
        const targetName = `${nextIndex}${ext.toLowerCase()}`
        await copy(["games", gameId, "images", sourceFile], [...srcDir, "images", targetName])
        existingHashes.add(sourceHash)
        copied++
        nextIndex++
    }
    return copied
}

async function copyItemImagesToBuild(srcDir, buildDir, imageNames) {
    for (const name of imageNames) {
        await copy([...srcDir, "images", name], [...buildDir, "images", name])
    }
}

/**
 * Sync a single game item to src/ (YAML) and build/ (JSON), non-destructive.
 *
 * @param {string}   gameId         - game namespace id
 * @param {string}   itemId         - item id (slug)
 * @param {object}   raw            - original crawler output for this item
 * @param {object}   meta           - normalized meta object { id, game, type, ... }
 * @param {object}   localeData     - { name, description, ... } for locale seed (en)
 * @param {string[]} localeCodes    - all locale codes
 * @param {Map<string, string[]>} sourceImageMap - source game images keyed by item id
 * @param {object}   stats          - mutable counters
 */
async function syncItem(gameId, itemId, raw, meta, localeData, localeCodes, sourceImageMap, stats) {
    const srcDir = [...paths.src.items, gameId, itemId]
    const buildDir = [...paths.build.statics, "items", gameId, itemId]

    // ── raw.yaml (always overwrite: tracks latest crawler output, verbatim) ──
    await write([...srcDir, "raw.yaml"], raw)

    // ── images: hash-based dedupe, append new, mirror to build ──
    stats.imagesSeeded += await syncItemImages(gameId, srcDir, sourceImageMap)
    const imageNames = await listItemImages(srcDir)
    await copyItemImagesToBuild(srcDir, buildDir, imageNames)

    // ── build canonical meta ──
    const normalizedMeta = {
        ...meta,
        type: enforceType(meta.type, itemId),
        images: imageNames,
    }
    delete normalizedMeta.icon
    for (const field of FORBIDDEN_META_FIELDS) {
        if (field in normalizedMeta) {
            log.info(`    warn: removed forbidden field "${field}" from meta for ${itemId}`)
            delete normalizedMeta[field]
        }
    }

    // ── meta.yaml (patch/create) ──
    const srcMetaPath = [...srcDir, "meta.yaml"]
    const existingMeta = (await exist(srcMetaPath)) ? await load(srcMetaPath) : null
    const mergedMeta = ensureConfigs(mergeMeta(existingMeta, normalizedMeta))
    for (const field of FORBIDDEN_META_FIELDS) delete mergedMeta[field]
    await write(srcMetaPath, mergedMeta)

    // ── meta.json (patch/create) ──
    const buildMetaPath = [...buildDir, "meta.json"]
    const existingBuildMeta = (await exist(buildMetaPath)) ? await load(buildMetaPath) : null
    const mergedBuildMeta = ensureConfigs(mergeMeta(existingBuildMeta, normalizedMeta))
    for (const field of FORBIDDEN_META_FIELDS) delete mergedBuildMeta[field]
    await write(buildMetaPath, mergedBuildMeta)

    if (!existingMeta) stats.items++
    else stats.metaPatched++

    // ── locale files: en first, then seed others from en.yaml ──
    const localeSeed = normalizeLocaleData(localeData)
    const enSrcPath = [...srcDir, "en.yaml"]
    const enBuildPath = [...buildDir, "en.json"]

    const existingEn = (await exist(enSrcPath)) ? await load(enSrcPath) : null
    const enSeed = normalizeLocaleData(existingEn, localeSeed)

    if (!(await exist(enSrcPath))) stats.locales++
    await write(enSrcPath, enSeed)
    await write(enBuildPath, enSeed)

    // Seed and sanitize non-en locales from canonical locale shape
    for (const code of localeCodes) {
        if (code === "en") continue
        const srcLocalePath = [...srcDir, `${code}.yaml`]
        const existingLocale = (await exist(srcLocalePath)) ? await load(srcLocalePath) : null
        const nextLocale = normalizeLocaleData(existingLocale, enSeed)
        if (!(await exist(srcLocalePath))) stats.locales++
        await write(srcLocalePath, nextLocale)

        const buildLocalePath = [...buildDir, `${code}.json`]
        await write(buildLocalePath, nextLocale)
    }
}

async function sanitizeExistingGameItems(gameId, localeCodes) {
    const gameSrcDir = [...paths.src.items, gameId]
    if (!(await exist(gameSrcDir))) return { items: 0, locales: 0, metaPatched: 0, imagesSeeded: 0, itemIds: new Set() }

    const entries = await dir(gameSrcDir)
    const itemIds = new Set()
    let metaPatched = 0

    for (const entry of entries) {
        const itemDir = [...gameSrcDir, entry]
        if (!(await isDirectory(itemDir))) continue

        itemIds.add(entry)

        const srcMetaPath = [...itemDir, "meta.yaml"]
        if (await exist(srcMetaPath)) {
            const srcMeta = await load(srcMetaPath)
            if (srcMeta && typeof srcMeta === "object") {
                const patched = ensureConfigs({ ...srcMeta })
                delete patched.icon
                for (const field of FORBIDDEN_META_FIELDS) delete patched[field]
                await write(srcMetaPath, patched)
                metaPatched++

                const buildMetaPath = [...paths.build.statics, "items", gameId, entry, "meta.json"]
                if (await exist(buildMetaPath)) {
                    const buildMeta = await load(buildMetaPath)
                    if (buildMeta && typeof buildMeta === "object") {
                        const patchedBuildMeta = ensureConfigs({ ...buildMeta })
                        delete patchedBuildMeta.icon
                        for (const field of FORBIDDEN_META_FIELDS) delete patchedBuildMeta[field]
                        await write(buildMetaPath, patchedBuildMeta)
                    }
                }
            }
        }

        const enSrcPath = [...itemDir, "en.yaml"]
        const enExisting = (await exist(enSrcPath)) ? await load(enSrcPath) : null
        const enSeed = normalizeLocaleData(enExisting, { name: "", description: "" })
        if (await exist(enSrcPath)) await write(enSrcPath, enSeed)

        const enBuildPath = [...paths.build.statics, "items", gameId, entry, "en.json"]
        if (await exist(enBuildPath)) await write(enBuildPath, enSeed)

        for (const code of localeCodes) {
            if (code === "en") continue
            const srcLocalePath = [...itemDir, `${code}.yaml`]
            if (await exist(srcLocalePath)) {
                const existingLocale = await load(srcLocalePath)
                const nextLocale = normalizeLocaleData(existingLocale, enSeed)
                await write(srcLocalePath, nextLocale)
            }

            const buildLocalePath = [...paths.build.statics, "items", gameId, entry, `${code}.json`]
            if (await exist(buildLocalePath)) {
                const existingBuildLocale = await load(buildLocalePath)
                const nextBuildLocale = normalizeLocaleData(existingBuildLocale, enSeed)
                await write(buildLocalePath, nextBuildLocale)
            }
        }
    }

    return { items: 0, locales: 0, metaPatched, imagesSeeded: 0, itemIds }
}

/**
 * Sync all items for a single game.
 *
 * @param {string}    gameId         - game id (e.g. "arc-raiders")
 * @param {Function}  detailMapper   - (rawItem) => { meta, locale }
 * @param {Function}  [imageResolver] - optional (rawItem) => string[] of source filenames
 *                                     If omitted, falls back to disk scan keyed by itemId
 * @returns {{ items: number, locales: number, metaPatched: number }}
 */
export async function syncGameItems(gameId, detailMapper, imageResolver = null) {
    if (!detailMapper) {
        log.info(`  ${gameId}: no detailMapper, skipping sync`)
        return sanitizeExistingGameItems(gameId, await getLocaleCodes())
    }

    const rawItemsPath = ["games", gameId, "items.json"]
    if (!(await exist(rawItemsPath))) {
        log.info(`  ${gameId}: games/${gameId}/items.json not found, skipping sync`)
        return sanitizeExistingGameItems(gameId, await getLocaleCodes())
    }

    const rawItems = await load(rawItemsPath)
    if (!Array.isArray(rawItems) || rawItems.length === 0) {
        log.info(`  ${gameId}: no items in crawl output, skipping sync`)
        return sanitizeExistingGameItems(gameId, await getLocaleCodes())
    }

    const localeCodes = await getLocaleCodes()
    const fallbackSourceImageMap = imageResolver ? null : await buildSourceImageMap(gameId)
    const stats = { items: 0, locales: 0, metaPatched: 0, imagesSeeded: 0 }
    const usedIds = new Set()

    for (const raw of rawItems) {
        const { meta: rawMeta, locale: localeData } = detailMapper(raw)
        const itemId = buildCanonicalItemId(raw, localeData)
        if (usedIds.has(itemId)) {
            throw new Error(`${gameId}: canonical item id collision detected for "${itemId}"`)
        }
        usedIds.add(itemId)
        const sourceFiles = imageResolver ? imageResolver(raw) : (fallbackSourceImageMap.get(String(raw.id)) || [])

        // Ensure required fields per spec §4
        const meta = {
            id: itemId,
            game: gameId,
            type: rawMeta.type || "item",
            ...rawMeta,
            // id and game are always authoritative from structure
            id: itemId,
            game: gameId,
        }

        await syncItem(gameId, itemId, raw, meta, localeData, localeCodes, sourceFiles, stats)
    }

    log.ok(`  ${gameId}: synced ${rawItems.length} items (${stats.items} new, ${stats.metaPatched} patched, ${stats.locales} locale files created, ${stats.imagesSeeded} images synced)`)
    return { ...stats, itemIds: usedIds }
}

function removeOrphanItemDirs(rootAbsPath, keepIds) {
    if (!fsNative.existsSync(rootAbsPath)) return 0

    const entries = fsNative.readdirSync(rootAbsPath, { withFileTypes: true })
    let removed = 0
    for (const entry of entries) {
        if (!entry.isDirectory()) continue
        if (keepIds.has(entry.name)) continue
        fsNative.rmSync(pathNative.join(rootAbsPath, entry.name), { recursive: true, force: true })
        removed += 1
    }

    return removed
}

export async function pruneGameItems(gameId, itemIds) {
    const keepIds = itemIds instanceof Set ? itemIds : new Set(itemIds || [])
    const srcGameDir = pathNative.join(...paths.src.items, gameId)
    const buildGameDir = pathNative.join(...paths.build.statics, "items", gameId)

    const removedSrc = removeOrphanItemDirs(srcGameDir, keepIds)
    const removedBuild = removeOrphanItemDirs(buildGameDir, keepIds)
    const removedTotal = removedSrc + removedBuild

    log.ok(`  ${gameId}: pruned ${removedTotal} orphan folders (src: ${removedSrc}, build: ${removedBuild})`)
    return { removedSrc, removedBuild, removedTotal }
}

/**
 * Hash build/statics/items/<gameId>/ after sync.
 * Called by build:games after all items are synced.
 */
export async function hashGameItems(gameId) {
    const gameBuildDir = [...paths.build.statics, "items", gameId]
    if (!(await exist(gameBuildDir))) {
        log.info(`  ${gameId}: no build output to hash, skipping`)
        return 0
    }
    const result = await generateHashFiles(gameBuildDir, [])
    log.ok(`  ${gameId}: generated ${result.hashFiles} hash files`)
    return result.hashFiles
}
