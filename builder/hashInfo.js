/**
 * BitTorrent Info-Hash Engine — BEP 3 (v1) and BEP 52 hybrid (v1 + v2)
 *
 * Why pure Web Standards (no npm deps)?
 * All target runtimes — browser, Node.js 15+, Deno, Bun, Cloudflare Workers —
 * implement crypto.subtle and TextEncoder. Zero external dependencies = zero supply-chain risk.
 */

// ─── Error ────────────────────────────────────────────────────────────────────

/**
 * Typed error for all hashing failures.
 * Consumers can switch on `err.code` instead of parsing error message strings.
 */
export class HashInfoError extends Error {
    constructor(code, message, options) {
        super(message, options)
        this.name = "HashInfoError"
        this.code = code
    }
}

// ─── Module-Level Constants & Utilities ───────────────────────────────────────

// BEP 52: each file is split into 16 KiB blocks for SHA-256 Merkle hashing.
const V2_BLOCK_SIZE = 16384
const ZERO_HASH = new Uint8Array(32)

const utf8Encoder = new TextEncoder()
const stringToBytes = (s) => utf8Encoder.encode(s)
const toHex = (bytes) => Array.from(bytes, (b) => b.toString(16).padStart(2, "0")).join("")

// Releases the event loop between hash batches to keep the UI/other tasks responsive.
const yieldToEventLoop = () => new Promise((r) => setTimeout(r, 0))

function concatUint8Arrays(chunks) {
    const result = new Uint8Array(chunks.reduce((total, chunk) => total + chunk.length, 0))
    let offset = 0
    for (const chunk of chunks) {
        result.set(chunk, offset)
        offset += chunk.length
    }
    return result
}

function normalizePathParts(path) {
    const parts = Array.isArray(path)
        ? path
        : String(path || "")
              .replace(/\\/g, "/")
              .split("/")
              .filter(Boolean)
    const result = []
    for (const part of parts) {
        const segment = String(part).normalize("NFC")
        if (!segment || segment === ".") continue
        // Reject ".." because it can escape the torrent root directory on extraction.
        if (segment === "..") throw new HashInfoError("INVALID_PATH", `Path segment '..' is not allowed`)
        result.push(segment)
    }
    return result
}

// ─── Zero-Copy Bencode Serializer ─────────────────────────────────────────────
//
// Why return an array of chunks instead of one concatenated buffer?
// A torrent with many files has thousands of 20-byte SHA-1 hashes in "pieces".
// Concatenating on every recursive call is O(n²) allocations.
// The single concat is deferred to the final step in concatUint8Arrays.
//
// Why for..of instead of push(...subArray)?
// V8 implements spread/apply via the call stack — push(...65k_items) overflows
// at ~65k arguments and throws "Maximum call stack size exceeded".

function encodeToBencodeChunks(value) {
    if (value instanceof Uint8Array) return [stringToBytes(`${value.length}:`), value]
    if (typeof value === "string") return encodeToBencodeChunks(stringToBytes(value))
    if (typeof value === "number") return [stringToBytes(`i${Math.trunc(value)}e`)]
    if (typeof value === "bigint") return [stringToBytes(`i${value}e`)]

    if (Array.isArray(value)) {
        const chunks = [stringToBytes("l")]
        for (const item of value) for (const c of encodeToBencodeChunks(item)) chunks.push(c)
        chunks.push(stringToBytes("e"))
        return chunks
    }

    if (value !== null && typeof value === "object") {
        // BEP 3: dict keys must be sorted by raw byte value, not locale-sensitive collation.
        // localeCompare produces different orderings on different OSes → infohash mismatch.
        const chunks = [stringToBytes("d")]
        for (const key of Object.keys(value).sort()) {
            for (const c of encodeToBencodeChunks(key)) chunks.push(c)

            if (key === "pieces" && Array.isArray(value[key])) {
                // "pieces" is a flat byte-string of concatenated 20-byte SHA-1 hashes, not a list.
                // Write it with a single length prefix to match the BitTorrent wire format.
                chunks.push(stringToBytes(`${value[key].length * 20}:`))
                for (const piece of value[key]) chunks.push(piece)
            } else for (const c of encodeToBencodeChunks(value[key])) chunks.push(c)
        }
        chunks.push(stringToBytes("e"))
        return chunks
    }

    throw new HashInfoError("INVALID_INPUT", `Bencode: unsupported type "${typeof value}"`)
}

// ─── BEP 52 Merkle Tree ───────────────────────────────────────────────────────
//
// Why a Merkle tree per file?
// BEP 52 defines the v2 infohash as SHA-256(bencoded info dict with per-file Merkle roots).
// This lets peers verify individual pieces without downloading the full file.
//
// Why a stack-based O(log n) algorithm?
// We don't know how many blocks a file has before reading it.
// The stack holds at most one hash per tree level — memory bounded by log₂(block count).

const nextPowerOfTwo = (n) => {
    let p = 1
    while (p < n) p *= 2
    return p
}

async function hashPair(left, right, crypto) {
    const buf = new Uint8Array(64)
    buf.set(left, 0)
    buf.set(right, 32)
    return new Uint8Array(await crypto.subtle.digest("SHA-256", buf))
}

async function pushMerkleLeaf(leaf, stack, crypto) {
    let node = leaf,
        level = 0
    while (stack[level]) {
        node = await hashPair(stack[level], node, crypto)
        stack[level++] = null
    }
    stack[level] = node
}

async function computeMerkleRoot(leavesCount, stack, crypto) {
    if (leavesCount === 0) return ZERO_HASH
    // Pad to the next power-of-two using ZERO_HASH, as required by BEP 52 §4.
    const target = nextPowerOfTwo(leavesCount)
    for (let i = leavesCount; i < target; i++) await pushMerkleLeaf(ZERO_HASH, stack, crypto)
    for (let l = stack.length - 1; l >= 0; l--) if (stack[l]) return stack[l]
    return ZERO_HASH
}

function addToV2FileTree(tree, pathParts, length, piecesRoot) {
    let dir = tree
    for (let i = 0; i < pathParts.length - 1; i++) {
        const part = pathParts[i]
        if (!dir[part]) dir[part] = {}
        // A path cannot be both a file (has "") and a directory (has children) simultaneously.
        if (dir[part][""] && Object.keys(dir[part]).length > 1)
            throw new HashInfoError("INVALID_INPUT", `Path conflict: "${pathParts.join("/")}" is both a file and a directory`)
        dir = dir[part]
    }
    dir[pathParts[pathParts.length - 1]] = { "": { length, "pieces root": piecesRoot } }
}

// ─── Sliding-Window Block Buffer ──────────────────────────────────────────────
//
// Why a carry buffer?
// Stream chunks arrive in arbitrary sizes. The carry buffer absorbs boundary
// misalignment between incoming chunk sizes and hashing block sizes (piece length
// for V1, 16 KiB for V2) without extra allocations.

/**
 * Feeds one stream chunk into a carry buffer, calling onBlock for each complete block.
 * Returns the updated carry length.
 *
 * @param {Uint8Array}                           chunk     — incoming stream chunk
 * @param {Uint8Array}                           carry     — persistent carry buffer (mutated in-place)
 * @param {number}                               carryLen  — bytes currently in carry
 * @param {number}                               blockSize — target block size
 * @param {(block: Uint8Array) => Promise<void>} onBlock   — called with each complete block
 */
async function feedBlockBuffer(chunk, carry, carryLen, blockSize, onBlock) {
    let offset = 0

    // Finish filling any partial block in the carry buffer first.
    if (carryLen > 0) {
        const take = Math.min(blockSize - carryLen, chunk.length)
        carry.set(chunk.subarray(0, take), carryLen)
        carryLen += take
        offset = take
        if (carryLen === blockSize) {
            await onBlock(carry)
            carryLen = 0
        }
    }

    // Consume full blocks directly from the chunk (zero-copy).
    while (offset + blockSize <= chunk.length) {
        await onBlock(chunk.subarray(offset, offset + blockSize))
        offset += blockSize
    }

    // Stash remaining tail bytes into the carry buffer for the next chunk.
    if (offset < chunk.length) {
        carry.set(chunk.subarray(offset), 0)
        carryLen = chunk.length - offset
    }

    return carryLen
}

// ─── Per-File Stream Hashing ──────────────────────────────────────────────────
//
// Why checkpoint + rollback?
// V1 piece hashes span file boundaries — the carry buffer is shared across all files.
// If a stream fails mid-file, the partial V1 pieces from the failed attempt must be
// undone before retry begins, or the infohash will be corrupted.
//
// Why isStreamFactory determines maxAttempts?
// A stream factory (typeof stream === "function") produces a fresh stream on each call.
// A raw stream object is already open and cannot be rewound — retry would read garbage.

/**
 * Streams one file through the V1 SHA-1 pieces pipeline only (BEP 3 mode).
 * pieceState is mutated in-place.
 */
async function hashFileV1(file, pathParts, pieceState, opts, crypto) {
    const isStreamFactory = typeof file.stream === "function"
    const maxAttempts = isStreamFactory ? (opts.retries ?? 3) : 1
    const breatherInterval = opts.breatherInterval ?? 500

    // Save a snapshot so we can roll back V1 state if a stream attempt fails.
    const checkpoint = {
        count: pieceState.pieces.length,
        carry: new Uint8Array(pieceState.carry),
        carryLen: pieceState.carryLen,
    }

    for (let attempt = 1; attempt <= maxAttempts; attempt++) {
        let fileBytesRead = 0
        try {
            const stream = isStreamFactory ? file.stream() : file.stream
            for await (const raw of stream) {
                const chunk = raw instanceof Uint8Array ? raw : new Uint8Array(raw)
                fileBytesRead += chunk.length
                pieceState.carryLen = await feedBlockBuffer(chunk, pieceState.carry, pieceState.carryLen, pieceState.pieceLen, async (block) => {
                    pieceState.pieces.push(new Uint8Array(await crypto.subtle.digest("SHA-1", block)))
                    if (++pieceState.yieldCounter % breatherInterval === 0) await yieldToEventLoop()
                })
            }

            // Dynamic length: backfill if caller didn't know the file size upfront (e.g. piped stream).
            if (file.length == null) file.length = fileBytesRead
            else if (fileBytesRead !== file.length)
                throw new HashInfoError("STREAM_LENGTH_MISMATCH", `"${pathParts.join("/")}" declared ${file.length} bytes but stream yielded ${fileBytesRead}`)

            return // success — exit retry loop
        } catch (err) {
            // Rollback: undo V1 pieces written during this failed attempt.
            pieceState.pieces.length = checkpoint.count
            pieceState.carry.set(checkpoint.carry)
            pieceState.carryLen = checkpoint.carryLen

            if (attempt === maxAttempts) {
                if (err instanceof HashInfoError) throw err
                throw new HashInfoError("STREAM_FAILED", `Failed to hash "${pathParts.join("/")}" after ${maxAttempts} attempt(s)`, { cause: err })
            }

            opts.onRetry?.(pathParts.join("/"), attempt, err)
            await new Promise((r) => setTimeout(r, 1000))
        }
    }
}

/**
 * Streams one file through V1 (SHA-1 pieces) and V2 (SHA-256 Merkle) pipelines simultaneously.
 * pieceState is mutated in-place. Returns the file's Merkle root for BEP 52.
 *
 * Why feed both pipelines in a single pass?
 * Reading a file twice (once for SHA-1, once for SHA-256) doubles I/O cost.
 * A single pass feeds both pipelines simultaneously.
 */
async function hashFileV1V2(file, pathParts, pieceState, opts, crypto) {
    const isStreamFactory = typeof file.stream === "function"
    const maxAttempts = isStreamFactory ? (opts.retries ?? 3) : 1
    const breatherInterval = opts.breatherInterval ?? 500

    const checkpoint = {
        count: pieceState.pieces.length,
        carry: new Uint8Array(pieceState.carry),
        carryLen: pieceState.carryLen,
    }

    for (let attempt = 1; attempt <= maxAttempts; attempt++) {
        // V2 state is per-attempt — a failed attempt's Merkle tree is discarded on rollback.
        const v2Carry = new Uint8Array(V2_BLOCK_SIZE)
        let v2CarryLen = 0,
            v2Stack = [],
            v2LeavesCount = 0,
            fileBytesRead = 0

        try {
            const stream = isStreamFactory ? file.stream() : file.stream
            for await (const raw of stream) {
                const chunk = raw instanceof Uint8Array ? raw : new Uint8Array(raw)
                fileBytesRead += chunk.length

                // V1: SHA-1 pieces — carry is shared across files (pieces span file boundaries).
                pieceState.carryLen = await feedBlockBuffer(chunk, pieceState.carry, pieceState.carryLen, pieceState.pieceLen, async (block) => {
                    pieceState.pieces.push(new Uint8Array(await crypto.subtle.digest("SHA-1", block)))
                    if (++pieceState.yieldCounter % breatherInterval === 0) await yieldToEventLoop()
                })

                // V2: SHA-256 Merkle leaves — per-file, each file has its own carry and stack.
                v2CarryLen = await feedBlockBuffer(chunk, v2Carry, v2CarryLen, V2_BLOCK_SIZE, async (block) => {
                    await pushMerkleLeaf(new Uint8Array(await crypto.subtle.digest("SHA-256", block)), v2Stack, crypto)
                    v2LeavesCount++
                    if (++pieceState.yieldCounter % breatherInterval === 0) await yieldToEventLoop()
                })
            }

            // Flush the final partial V2 block.
            // BEP 52 §4.2: the last block of every file is zero-padded before hashing.
            if (v2CarryLen > 0) {
                v2Carry.fill(0, v2CarryLen)
                await pushMerkleLeaf(new Uint8Array(await crypto.subtle.digest("SHA-256", v2Carry)), v2Stack, crypto)
                v2LeavesCount++
            }

            if (file.length == null) file.length = fileBytesRead
            else if (fileBytesRead !== file.length)
                throw new HashInfoError("STREAM_LENGTH_MISMATCH", `"${pathParts.join("/")}" declared ${file.length} bytes but stream yielded ${fileBytesRead}`)

            return computeMerkleRoot(v2LeavesCount, v2Stack, crypto)
        } catch (err) {
            pieceState.pieces.length = checkpoint.count
            pieceState.carry.set(checkpoint.carry)
            pieceState.carryLen = checkpoint.carryLen

            if (attempt === maxAttempts) {
                if (err instanceof HashInfoError) throw err
                throw new HashInfoError("STREAM_FAILED", `Failed to hash "${pathParts.join("/")}" after ${maxAttempts} attempt(s)`, { cause: err })
            }

            opts.onRetry?.(pathParts.join("/"), attempt, err)
            await new Promise((r) => setTimeout(r, 1000))
        }
    }
}

// ─── Info Dict Assembly ───────────────────────────────────────────────────────
//
// Why two layouts (single-file vs multi-file)?
// BEP 3 defines them as separate wire formats — not a convention, a protocol requirement.
//   Single-file: info dict has "length" (integer), no "files" key.
//   Multi-file:  info dict has "files" (list), no "length" key.
// A client receiving the wrong layout cannot reconstruct the files.

function buildInfoDict(name, pieceLen, pieces, v2FileTree, normalizedFiles, opts) {
    const info = {
        name,
        "piece length": pieceLen,
        pieces,
    }

    // BEP 52 hybrid fields — only included when opts.v2 === true.
    //
    // Why omit by default? The v1 infohash is SHA-1 of the entire bencoded info dict.
    // Adding BEP 52 fields (meta version, file tree) changes the dict → changes the v1
    // hash → incompatible with webtorrent/qBittorrent plain BEP 3 torrents.
    //
    // Why keep them for hybrid? With both fields present, clients that only know v1 use
    // the btih= part of the magnet link; BEP 52 clients use btih2=. Same torrent, both
    // sides can connect. No downgrade needed.
    if (opts.v2) {
        info["meta version"] = 2
        info["file tree"] = v2FileTree
    }

    if (normalizedFiles.length === 1) info.length = normalizedFiles[0].file.length
    else info.files = normalizedFiles.map(({ file, pathParts }) => ({ length: file.length, path: pathParts }))

    if (opts.private) info.private = 1
    if (opts.source) info.source = opts.source

    return info
}

// ─── Core Engine ──────────────────────────────────────────────────────────────

async function computeInfohash(files, name, opts = {}) {
    // Step 1 — Validate environment.
    const crypto = opts.customCrypto ?? globalThis.crypto
    if (!crypto?.subtle) throw new HashInfoError("CRYPTO_UNAVAILABLE", "crypto.subtle is not available — requires Node.js 15+, a modern browser, or a WinterCG-compatible runtime")
    if (!Array.isArray(files) || files.length === 0) throw new HashInfoError("INVALID_INPUT", "files must be a non-empty array")

    // Step 2 — Calculate piece length.
    // Auto-calculate to match create-torrent's algorithm — so our v1 infohash
    // matches webtorrent's torrent.infoHash without the caller having to guess.
    // Formula: smallest power-of-2 ≥ totalSize/1000, clamped to [16384, 262144].
    const totalKnownSize = files.reduce((total, file) => total + (file.length ?? 0), 0)
    const pieceLen = opts.pieceLength ?? (() => {
        const raw = nextPowerOfTwo(Math.ceil(totalKnownSize / 1000) || 1)
        return Math.max(V2_BLOCK_SIZE, Math.min(262144, raw))
    })()

    if (pieceLen < V2_BLOCK_SIZE) throw new HashInfoError("INVALID_PIECE_LENGTH", `pieceLength must be ≥ ${V2_BLOCK_SIZE} bytes (BEP 52)`)
    if ((pieceLen & (pieceLen - 1)) !== 0) throw new HashInfoError("INVALID_PIECE_LENGTH", "pieceLength must be a power of two (e.g. 16384, 32768, 262144)")

    // Step 3 — Normalize and sort file paths.
    // Why NOT localeCompare? Locale-sensitive collation differs across OSes ("ä" sorts
    // differently on macOS vs Linux), making the infohash non-deterministic.
    const normalizedFiles = files
        .map((file, index) => {
            if (file.length != null && (!Number.isFinite(file.length) || file.length < 0))
                throw new HashInfoError("INVALID_INPUT", `Invalid length for "${file.path}": must be a non-negative finite number`)
            const pathParts = normalizePathParts(file.path)
            const pathKeyBytes = stringToBytes(pathParts.join("/"))
            return { file, pathParts, pathKeyBytes, index }
        })
        .sort((a, b) => {
            const len = Math.min(a.pathKeyBytes.length, b.pathKeyBytes.length)
            for (let i = 0; i < len; i++) if (a.pathKeyBytes[i] !== b.pathKeyBytes[i]) return a.pathKeyBytes[i] - b.pathKeyBytes[i]
            return a.pathKeyBytes.length - b.pathKeyBytes.length || a.index - b.index
        })

    // Step 4 — Stream all files through the hashing pipeline(s).
    // V1 carry is shared across files because BEP 3 pieces span file boundaries.
    // V2 state is per-file because each file has its own independent Merkle tree.
    const pieceState = {
        pieces: [],
        carry: new Uint8Array(pieceLen),
        carryLen: 0,
        pieceLen,
        yieldCounter: 0, // throttles yieldToEventLoop — counts hashes across both V1 and V2 pipelines
    }
    const v2FileTree = {}

    for (const { file, pathParts } of normalizedFiles) {
        // Zero-byte files: skip streaming; BEP 52 §4.1 defines their Merkle root as ZERO_HASH.
        if (file.length === 0) {
            if (opts.v2) addToV2FileTree(v2FileTree, pathParts, 0, ZERO_HASH)
            continue
        }

        if (opts.v2) {
            const piecesRoot = await hashFileV1V2(file, pathParts, pieceState, opts, crypto)
            addToV2FileTree(v2FileTree, pathParts, file.length, piecesRoot)
        } else await hashFileV1(file, pathParts, pieceState, opts, crypto)
    }

    // Flush the final partial V1 piece (last bytes across all files).
    if (pieceState.carryLen > 0)
        pieceState.pieces.push(new Uint8Array(await crypto.subtle.digest("SHA-1", pieceState.carry.subarray(0, pieceState.carryLen))))

    // Step 5 — Assemble info dict, bencode it, compute infohash(es).
    const info = buildInfoDict(String(name || "").normalize("NFC"), pieceLen, pieceState.pieces, v2FileTree, normalizedFiles, opts)
    const bencodedInfo = concatUint8Arrays(encodeToBencodeChunks(info))

    return {
        v1: toHex(new Uint8Array(await crypto.subtle.digest("SHA-1", bencodedInfo))),
        v2: opts.v2 ? toHex(new Uint8Array(await crypto.subtle.digest("SHA-256", bencodedInfo))) : null,
    }
}

// ─── Public API ───────────────────────────────────────────────────────────────

/**
 * Compute a BitTorrent infohash from any data source.
 *
 * Default (BEP 3, v1-only): compatible with webtorrent, qBittorrent, and all standard clients.
 * With { v2: true } (BEP 52 hybrid): v1 + v2 Merkle tree — both old and new clients can find
 * the torrent via the same magnet link (btih= for v1 clients, btih2= for v2 clients).
 *
 * Important: the v1 hash in hybrid mode differs from v1-only mode for the same content,
 * because the BEP 52 info dict includes extra fields (meta version, file tree) that change
 * the SHA-1. Do not mix modes for the same torrent.
 *
 * @param {Uint8Array|ArrayBuffer|File|Blob|AsyncIterable|Array} inputData
 *   - Uint8Array / ArrayBuffer     — static in-memory buffer (retry-safe)
 *   - File / Blob                  — Web File API (browser, Deno)
 *   - AsyncIterable                — ReadableStream, Node.js Readable, async generator
 *   - Array<{path,length?,stream}> — multi-file torrent descriptor
 * @param {string}  fileName
 * @param {object}  [options]
 * @param {boolean} [options.v2=false]          — enable BEP 52 hybrid; returns v2 SHA-256, otherwise v2 is null
 * @param {number}  [options.pieceLength]       — V1 piece size in bytes (auto-calculated if omitted)
 * @param {number}  [options.length]            — declared byte length for AsyncIterable (auto-measured if omitted)
 * @param {number}  [options.retries=3]         — retry count for stream-factory inputs
 * @param {boolean} [options.private]           — set info.private = 1
 * @param {string}  [options.source]            — embed info.source (private tracker announce)
 * @param {Function}[options.onRetry]           — (path, attempt, error) => void — called before each retry
 * @param {object}  [options.customCrypto]      — inject a crypto implementation (testing / non-standard runtimes)
 * @returns {Promise<{v1: string, v2: string|null}>}
 * @throws {HashInfoError}
 */
export async function hashInfo(inputData, fileName, options = {}) {
    let files

    if (Array.isArray(inputData))
        // Multi-file: caller provides the file descriptor list directly.
        files = inputData
    else if (inputData instanceof Uint8Array || inputData instanceof ArrayBuffer) {
        // Static buffer: wrap in a stream factory so the retry logic can re-read it cheaply.
        const bytes = inputData instanceof ArrayBuffer ? new Uint8Array(inputData) : inputData
        files = [{
            path: fileName,
            length: bytes.length,
            stream: () => new ReadableStream({ start(c) { c.enqueue(bytes); c.close() } }),
        }]
    } else if (inputData != null && typeof inputData.stream === "function")
        // Web File / Blob: File.stream() always returns a fresh stream — retry is safe.
        files = [{ path: fileName, length: inputData.size ?? 0, stream: () => inputData.stream() }]
    else if (inputData != null && typeof inputData[Symbol.asyncIterator] === "function")
        // Native async iterable (ReadableStream, Node.js Readable, fetch response body…).
        // Why NOT wrap in a factory? The caller cannot recreate an exhausted stream,
        // so isStreamFactory=false → maxAttempts=1 is enforced automatically.
        // length is optional: if omitted the engine counts bytes on the fly and backfills.
        files = [{ path: fileName, length: options.length ?? null, stream: inputData }]
    else throw new HashInfoError("UNSUPPORTED_FORMAT", "Unsupported inputData type. Expected: Uint8Array, ArrayBuffer, File/Blob, AsyncIterable, or Array of file descriptors.")

    return computeInfohash(files, fileName, options)
}
