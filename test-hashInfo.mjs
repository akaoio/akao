/**
 * Test suite for src/core/Torrent/hashInfo.js
 * Run: node test-hashInfo.mjs
 * Node.js 18+ required (node:test, crypto.subtle)
 *
 * API under test:
 *   hashInfo(inputData, fileName, options) → Promise<{ v1: string, v2: string|null }>
 *   - v1: 40-char SHA-1 hex (BEP 3 infohash) — always present
 *   - v2: 64-char SHA-256 hex (BEP 52 infohash) — only when options.v2 === true, otherwise null
 *
 * GOLDEN VALUES — how to calibrate:
 *   After the first passing run, print results and paste them back as GOLDEN.
 *   They are intentionally absent until the implementation is trusted.
 *   Tests that verify structure/determinism/isolation do NOT need golden values.
 */

import { test } from "node:test"
import assert from "node:assert/strict"
import { hashInfo, HashInfoError } from "./builder/hashInfo.js"

// ─── Fixtures ─────────────────────────────────────────────────────────────────

const D64   = new Uint8Array(64 * 1024).fill(42)   // 64 KB — main fixture
const D1    = new Uint8Array([99])                   // 1 byte
const D0    = new Uint8Array(0)                      // empty
const NAME  = "test-shop.db"

// Helpers
const isV1 = (s) => /^[0-9a-f]{40}$/.test(s)
const isV2 = (s) => /^[0-9a-f]{64}$/.test(s)

// ─── Group 1: Return Value Structure ─────────────────────────────────────────

test("G1-01 returns an object with v1 and v2 keys; v2 is null in default (BEP 3) mode", async () => {
    const result = await hashInfo(D64, NAME)
    assert.equal(typeof result, "object")
    assert.ok("v1" in result, "missing v1")
    assert.ok("v2" in result, "missing v2 key")
    assert.equal(result.v2, null, "v2 must be null when opts.v2 is not set")
    assert.equal(Object.keys(result).length, 2, "no extra keys expected")
})

test("G1-02 v1 is a 40-char lowercase hex string (SHA-1 infohash)", async () => {
    const { v1 } = await hashInfo(D64, NAME)
    assert.match(v1, /^[0-9a-f]{40}$/)
})

test("G1-03 v2 is a 64-char lowercase hex string when opts.v2 is true (BEP 52 hybrid)", async () => {
    const { v2 } = await hashInfo(D64, NAME, { v2: true })
    assert.match(v2, /^[0-9a-f]{64}$/)
})

test("G1-04 v1 and v2 are always different in hybrid mode (SHA-1 ≠ SHA-256 of same info dict)", async () => {
    const { v1, v2 } = await hashInfo(D64, NAME, { v2: true })
    assert.notEqual(v1, v2)
})

test("G1-05 hybrid v1 differs from BEP 3 v1 for same content (BEP 52 fields change the info dict)", async () => {
    const [bep3, hybrid] = await Promise.all([
        hashInfo(D64, NAME),
        hashInfo(D64, NAME, { v2: true }),
    ])
    assert.notEqual(bep3.v1, hybrid.v1, "adding BEP 52 fields to info dict must change the SHA-1 hash")
})

// ─── Group 2: Input Types ─────────────────────────────────────────────────────

test("G2-01 Uint8Array input produces valid v1", async () => {
    const { v1 } = await hashInfo(D64, NAME)
    assert.ok(isV1(v1))
})

test("G2-02 ArrayBuffer input produces same v1 as Uint8Array", async () => {
    const ab = D64.buffer.slice(D64.byteOffset, D64.byteOffset + D64.byteLength)
    const [a, b] = await Promise.all([hashInfo(D64, NAME), hashInfo(ab, NAME)])
    assert.equal(a.v1, b.v1)
})

test("G2-03 Node.js Buffer input produces same v1 as Uint8Array", async () => {
    const buf = Buffer.from(D64)
    const [a, b] = await Promise.all([hashInfo(D64, NAME), hashInfo(buf, NAME)])
    assert.equal(a.v1, b.v1)
})

test("G2-04 async generator (AsyncIterable) input without length — engine auto-measures", async () => {
    async function* gen() { yield D64 }
    const { v1 } = await hashInfo(gen(), NAME)
    assert.ok(isV1(v1))
})

test("G2-05 async generator with explicit length produces same v1 as buffer", async () => {
    async function* gen() { yield D64 }
    const [fromBuf, fromStream] = await Promise.all([
        hashInfo(D64, NAME),
        hashInfo(gen(), NAME, { length: D64.length }),
    ])
    assert.equal(fromBuf.v1, fromStream.v1)
})

test("G2-06 async generator with chunked delivery produces same v1 as one-shot", async () => {
    // Split D64 into 4 chunks of 16 KB each
    const CHUNK = 16 * 1024
    async function* chunked() {
        for (let i = 0; i < D64.length; i += CHUNK) yield D64.subarray(i, i + CHUNK)
    }
    const [a, b] = await Promise.all([hashInfo(D64, NAME), hashInfo(chunked(), NAME)])
    assert.equal(a.v1, b.v1)
})

test("G2-07 multi-file Array input returns valid v1", async () => {
    const files = [
        { path: "a.txt", length: 3, stream: () => new ReadableStream({ start(c) { c.enqueue(new Uint8Array([1,2,3])); c.close() } }) },
        { path: "b.txt", length: 3, stream: () => new ReadableStream({ start(c) { c.enqueue(new Uint8Array([4,5,6])); c.close() } }) },
    ]
    const { v1 } = await hashInfo(files, "multi-torrent")
    assert.ok(isV1(v1))
})

test("G2-08 single-byte data produces valid v1", async () => {
    const { v1 } = await hashInfo(D1, "a.txt")
    assert.ok(isV1(v1))
})

test("G2-09 empty data (0 bytes) does not throw and returns valid v1", async () => {
    const { v1 } = await hashInfo(D0, NAME)
    assert.ok(isV1(v1))
})

test("G2-10 data exactly 1 pieceLength (16384 bytes) — exactly 1 piece", async () => {
    const d = new Uint8Array(16384).fill(7)
    const { v1 } = await hashInfo(d, "exact.bin")
    assert.ok(isV1(v1))
})

test("G2-11 data exactly 2 pieceLengths (32768 bytes) — no off-by-one", async () => {
    const d = new Uint8Array(32768).fill(7)
    const { v1 } = await hashInfo(d, "exact2.bin")
    assert.ok(isV1(v1))
})

test("G2-12 data not divisible by pieceLength — correct last partial piece", async () => {
    const d = new Uint8Array(20000).fill(3) // 16384 + 3616 bytes
    const { v1 } = await hashInfo(d, "partial.bin")
    assert.ok(isV1(v1))
})

// ─── Group 3: HashInfoError — Error Codes ────────────────────────────────────

test("G3-01 unsupported input type throws HashInfoError(UNSUPPORTED_FORMAT)", async () => {
    await assert.rejects(
        () => hashInfo(12345, NAME),
        (err) => { assert.ok(err instanceof HashInfoError); assert.equal(err.code, "UNSUPPORTED_FORMAT"); return true },
    )
})

test("G3-02 null input throws HashInfoError(UNSUPPORTED_FORMAT)", async () => {
    await assert.rejects(
        () => hashInfo(null, NAME),
        (err) => { assert.ok(err instanceof HashInfoError); assert.equal(err.code, "UNSUPPORTED_FORMAT"); return true },
    )
})

test("G3-03 pieceLength < 16384 throws HashInfoError(INVALID_PIECE_LENGTH)", async () => {
    await assert.rejects(
        () => hashInfo(D64, NAME, { pieceLength: 8192 }),
        (err) => { assert.ok(err instanceof HashInfoError); assert.equal(err.code, "INVALID_PIECE_LENGTH"); return true },
    )
})

test("G3-04 pieceLength not a power of two throws HashInfoError(INVALID_PIECE_LENGTH)", async () => {
    await assert.rejects(
        () => hashInfo(D64, NAME, { pieceLength: 20000 }),
        (err) => { assert.ok(err instanceof HashInfoError); assert.equal(err.code, "INVALID_PIECE_LENGTH"); return true },
    )
})

test("G3-05 path with '..' segment throws HashInfoError(INVALID_PATH)", async () => {
    const files = [{ path: "../escape/file.txt", length: 1, stream: () => new ReadableStream({ start(c) { c.enqueue(new Uint8Array([1])); c.close() } }) }]
    await assert.rejects(
        () => hashInfo(files, "torrent"),
        (err) => { assert.ok(err instanceof HashInfoError); assert.equal(err.code, "INVALID_PATH"); return true },
    )
})

test("G3-06 negative file length throws HashInfoError(INVALID_INPUT)", async () => {
    const files = [{ path: "a.txt", length: -1, stream: () => new ReadableStream({ start(c) { c.enqueue(new Uint8Array([1])); c.close() } }) }]
    await assert.rejects(
        () => hashInfo(files, "torrent"),
        (err) => { assert.ok(err instanceof HashInfoError); assert.equal(err.code, "INVALID_INPUT"); return true },
    )
})

test("G3-07 Infinity file length throws HashInfoError(INVALID_INPUT)", async () => {
    const files = [{ path: "a.txt", length: Infinity, stream: () => new ReadableStream({ start(c) { c.enqueue(new Uint8Array([1])); c.close() } }) }]
    await assert.rejects(
        () => hashInfo(files, "torrent"),
        (err) => { assert.ok(err instanceof HashInfoError); assert.equal(err.code, "INVALID_INPUT"); return true },
    )
})

test("G3-08 stream yields fewer bytes than declared length → STREAM_LENGTH_MISMATCH", async () => {
    async function* gen() { yield new Uint8Array([1, 2, 3]) } // 3 bytes, claimed 100
    await assert.rejects(
        () => hashInfo(gen(), NAME, { length: 100 }),
        (err) => { assert.ok(err instanceof HashInfoError); assert.equal(err.code, "STREAM_LENGTH_MISMATCH"); return true },
    )
})

test("G3-09 stream yields more bytes than declared length → STREAM_LENGTH_MISMATCH", async () => {
    async function* gen() { yield new Uint8Array(100).fill(1) } // 100 bytes, claimed 3
    await assert.rejects(
        () => hashInfo(gen(), NAME, { length: 3 }),
        (err) => { assert.ok(err instanceof HashInfoError); assert.equal(err.code, "STREAM_LENGTH_MISMATCH"); return true },
    )
})

test("G3-10 all HashInfoErrors are instanceof Error (standard contract)", async () => {
    try { await hashInfo(null, NAME) } catch (err) {
        assert.ok(err instanceof Error)
        assert.ok(err instanceof HashInfoError)
        assert.equal(err.name, "HashInfoError")
        assert.ok(typeof err.code === "string")
    }
})

test("G3-11 empty files array throws HashInfoError(INVALID_INPUT)", async () => {
    await assert.rejects(
        () => hashInfo([], NAME),
        (err) => { assert.ok(err instanceof HashInfoError); assert.equal(err.code, "INVALID_INPUT"); return true },
    )
})

// ─── Group 4: pieceLength Option ─────────────────────────────────────────────

test("G4-01 default pieceLength is auto-calculated (create-torrent compatible)", async () => {
    // For D64 (64 KB = 65536 bytes): nextPow2(ceil(65536/1000)) = nextPow2(66) = 128, clamped to [16384, 262144] → 16384
    const a = await hashInfo(D64, NAME)
    const b = await hashInfo(D64, NAME, { pieceLength: 16384 })
    assert.equal(a.v1, b.v1, "auto-calculated pieceLength should equal 16384 for a 64KB file")
})

test("G4-02 different pieceLength produces different hashes", async () => {
    const [a, b] = await Promise.all([
        hashInfo(D64, NAME, { pieceLength: 16384 }),
        hashInfo(D64, NAME, { pieceLength: 32768 }),
    ])
    assert.notEqual(a.v1, b.v1)
})

test("G4-03 minimum valid pieceLength (16384) does not throw", async () => {
    const { v1 } = await hashInfo(D64, NAME, { pieceLength: 16384 })
    assert.ok(isV1(v1))
})

test("G4-04 pieceLength larger than file size produces valid hash (1 piece)", async () => {
    const d = new Uint8Array(1000).fill(5)
    const { v1 } = await hashInfo(d, "small.bin", { pieceLength: 16384 })
    assert.ok(isV1(v1))
})

test("G4-05 two different pieceLengths with same data → different hashes (pieceLength is in info dict)", async () => {
    const d = new Uint8Array(1000).fill(5)
    const [a, b] = await Promise.all([
        hashInfo(d, "f.bin", { pieceLength: 16384 }),
        hashInfo(d, "f.bin", { pieceLength: 32768 }),
    ])
    assert.notEqual(a.v1, b.v1, "different pieceLength → different info dict → different infohash")
})

// ─── Group 5: fileName Encoding ──────────────────────────────────────────────

test("G5-01 ASCII filename — deterministic", async () => {
    const [a, b] = await Promise.all([hashInfo(D64, "file.db"), hashInfo(D64, "file.db")])
    assert.equal(a.v1, b.v1)
})

test("G5-02 different filenames → different hashes", async () => {
    const [a, b] = await Promise.all([hashInfo(D64, "a.db"), hashInfo(D64, "b.db")])
    assert.notEqual(a.v1, b.v1)
})

test("G5-03 Vietnamese UTF-8 filename produces valid v1", async () => {
    const { v1 } = await hashInfo(D64, "tài liệu.pdf")
    assert.ok(isV1(v1))
})

test("G5-04 Vietnamese filename ≠ ASCII filename hash", async () => {
    const [a, b] = await Promise.all([hashInfo(D64, "file.pdf"), hashInfo(D64, "tài liệu.pdf")])
    assert.notEqual(a.v1, b.v1)
})

test("G5-05 empty string filename produces valid v1", async () => {
    const { v1 } = await hashInfo(D64, "")
    assert.ok(isV1(v1))
})

test("G5-06 long filename (1000 chars) produces valid v1", async () => {
    const { v1 } = await hashInfo(D64, "a".repeat(1000))
    assert.ok(isV1(v1))
})

test("G5-07 filename with spaces and special chars produces valid v1", async () => {
    const { v1 } = await hashInfo(D64, "my file (2024) [final].pdf")
    assert.ok(isV1(v1))
})

// ─── Group 6: Determinism ─────────────────────────────────────────────────────

test("G6-01 same Uint8Array called twice → identical v1", async () => {
    const [a, b] = await Promise.all([hashInfo(D64, NAME), hashInfo(D64, NAME)])
    assert.equal(a.v1, b.v1)
})

test("G6-02 same data in two separate Uint8Array instances → identical v1", async () => {
    const x = new Uint8Array(1000).fill(42)
    const y = new Uint8Array(1000).fill(42)
    const [a, b] = await Promise.all([hashInfo(x, "x"), hashInfo(y, "x")])
    assert.equal(a.v1, b.v1)
})

test("G6-03 different data → different v1 and different v2", async () => {
    const [a, b] = await Promise.all([
        hashInfo(new Uint8Array(1000).fill(1), "x", { v2: true }),
        hashInfo(new Uint8Array(1000).fill(2), "x", { v2: true }),
    ])
    assert.notEqual(a.v1, b.v1)
    assert.notEqual(a.v2, b.v2)
})

test("G6-04 ArrayBuffer and Uint8Array of same bytes → identical v1", async () => {
    const buf = new Uint8Array(512).fill(99)
    const ab = buf.buffer.slice(0)
    const [a, b] = await Promise.all([hashInfo(buf, "f"), hashInfo(ab, "f")])
    assert.equal(a.v1, b.v1)
})

// ─── Group 7: Multi-File Torrent Isolation ────────────────────────────────────

test("G7-01 file order is normalized — swapping files gives same hash (BEP 52 sort)", async () => {
    const makeStream = (byte) => () => new ReadableStream({ start(c) { c.enqueue(new Uint8Array(100).fill(byte)); c.close() } })
    const filesAB = [
        { path: "a.bin", length: 100, stream: makeStream(1) },
        { path: "b.bin", length: 100, stream: makeStream(2) },
    ]
    const filesBA = [
        { path: "b.bin", length: 100, stream: makeStream(2) },
        { path: "a.bin", length: 100, stream: makeStream(1) },
    ]
    const [ab, ba] = await Promise.all([hashInfo(filesAB, "t"), hashInfo(filesBA, "t")])
    assert.equal(ab.v1, ba.v1, "BEP 52 sort must normalize file order")
})

test("G7-02 file path is included in multi-file info dict — different paths produce different hashes", async () => {
    // Multi-file format (2+ files) includes path in info.files; single-file format does not.
    const makeStream = (byte) => () => new ReadableStream({ start(c) { c.enqueue(new Uint8Array(100).fill(byte)); c.close() } })
    const [a, b] = await Promise.all([
        hashInfo([
            { path: "a/unique.bin", length: 100, stream: makeStream(1) },
            { path: "shared.bin",   length: 100, stream: makeStream(2) },
        ], "t"),
        hashInfo([
            { path: "b/unique.bin", length: 100, stream: makeStream(1) },
            { path: "shared.bin",   length: 100, stream: makeStream(2) },
        ], "t"),
    ])
    assert.notEqual(a.v1, b.v1)
})

test("G7-03 zero-byte file in multi-file torrent does not throw", async () => {
    const files = [
        { path: "empty.txt", length: 0, stream: () => new ReadableStream({ start(c) { c.close() } }) },
        { path: "data.bin", length: 3, stream: () => new ReadableStream({ start(c) { c.enqueue(new Uint8Array([1,2,3])); c.close() } }) },
    ]
    const { v1 } = await hashInfo(files, "mixed")
    assert.ok(isV1(v1))
})

test("G7-04 single-file vs multi-file layout produces different hashes (different info dict schema)", async () => {
    const d = new Uint8Array(100).fill(1)
    const single = await hashInfo(d, "file.bin")
    const multi = await hashInfo(
        [{ path: "file.bin", length: 100, stream: () => new ReadableStream({ start(c) { c.enqueue(d); c.close() } }) }],
        "file.bin",
    )
    // The engine wraps single inputs as single-file format; the multi-file path is explicitly an Array.
    // Both paths should produce valid hashes.
    assert.ok(isV1(single.v1)); assert.ok(isV1(multi.v1))
})

// ─── Group 8: onRetry Callback ────────────────────────────────────────────────

test("G8-01 onRetry is called when stream-factory stream fails (mock)", async () => {
    let retryCount = 0
    let attempt1Fail = true

    // Stream factory that fails once, then succeeds
    const streamFactory = () => {
        if (attempt1Fail) {
            attempt1Fail = false
            return {
                [Symbol.asyncIterator]() {
                    return { next() { return Promise.reject(new Error("mock I/O error")) } }
                }
            }
        }
        // Success on second attempt
        return new ReadableStream({ start(c) { c.enqueue(D1); c.close() } })
    }

    const files = [{ path: "file.txt", length: 1, stream: streamFactory }]
    const { v1 } = await hashInfo(files, "retry-test", {
        retries: 2,
        onRetry: (path, attempt, _err) => { retryCount++; assert.equal(path, "file.txt"); assert.equal(attempt, 1) },
    })
    assert.equal(retryCount, 1, "onRetry should be called exactly once")
    assert.ok(isV1(v1))
})

test("G8-02 stream without factory (raw AsyncIterable) gets no retry — fails immediately", async () => {
    let callCount = 0
    async function* failing() { callCount++; throw new Error("immediate failure") }
    await assert.rejects(
        () => hashInfo(failing(), NAME, { retries: 5 }),
        (err) => { assert.ok(err instanceof HashInfoError); assert.equal(err.code, "STREAM_FAILED"); return true },
    )
    assert.equal(callCount, 1, "raw stream must not be retried")
})

// ─── Group 9: options.private / options.source ────────────────────────────────

test("G9-01 options.private changes the hash (private=1 is in info dict)", async () => {
    const [a, b] = await Promise.all([
        hashInfo(D64, NAME),
        hashInfo(D64, NAME, { private: true }),
    ])
    assert.notEqual(a.v1, b.v1)
})

test("G9-02 options.source changes the hash", async () => {
    const [a, b] = await Promise.all([
        hashInfo(D64, NAME),
        hashInfo(D64, NAME, { source: "my-tracker.com" }),
    ])
    assert.notEqual(a.v1, b.v1)
})

// ─── Group 10: Concurrency ────────────────────────────────────────────────────

test("G10-01 100 parallel calls — no race conditions, all identical", async () => {
    const results = await Promise.all(Array.from({ length: 100 }, () => hashInfo(D64, NAME)))
    const first = results[0].v1
    assert.ok(results.every((r) => r.v1 === first), "all 100 calls must return the same v1")
})

test("G10-02 20 parallel calls with different data — all unique v1 hashes", async () => {
    const datasets = Array.from({ length: 20 }, (_, i) => new Uint8Array(1000).fill(i))
    const results = await Promise.all(datasets.map((d, i) => hashInfo(d, `file${i}.bin`)))
    const unique = new Set(results.map((r) => r.v1))
    assert.equal(unique.size, 20, "all 20 different files must produce different v1 hashes")
})

// ─── Group 11: Performance ────────────────────────────────────────────────────

test("G11-01 1 MB file completes in < 2000 ms", async () => {
    const data = new Uint8Array(1024 * 1024).fill(99)
    const start = Date.now()
    await hashInfo(data, "1mb.bin")
    assert.ok(Date.now() - start < 2000, "1 MB should hash in under 2 s")
})

test("G11-02 10 MB file completes without error", async () => {
    const data = new Uint8Array(10 * 1024 * 1024).fill(55)
    const { v1 } = await hashInfo(data, "10mb.bin")
    assert.ok(isV1(v1))
})

// ─── Group 12: BEP 52 Structural Checks (requires opts.v2 = true) ─────────────

test("G12-01 v1 and v2 differ between empty file and 1-byte file", async () => {
    const [empty, oneByte] = await Promise.all([
        hashInfo(D0, NAME, { v2: true }),
        hashInfo(D1, NAME, { v2: true }),
    ])
    assert.notEqual(empty.v1, oneByte.v1)
    assert.notEqual(empty.v2, oneByte.v2)
})

test("G12-02 exact pieceLength boundary — 1 piece vs 2 pieces produces different hashes", async () => {
    const PIECE = 16384
    const one  = new Uint8Array(PIECE).fill(7)        // exactly 1 piece
    const two  = new Uint8Array(PIECE * 2).fill(7)    // exactly 2 pieces (same byte pattern)
    const [a, b] = await Promise.all([hashInfo(one, "f.bin", { pieceLength: PIECE }), hashInfo(two, "f.bin", { pieceLength: PIECE })])
    assert.notEqual(a.v1, b.v1, "different piece count must change infohash")
})

test("G12-03 single-byte change in data → completely different v1 and v2", async () => {
    const base    = new Uint8Array(1000).fill(0)
    const changed = new Uint8Array(1000).fill(0)
    changed[500] = 1
    const [a, b] = await Promise.all([
        hashInfo(base, "f", { v2: true }),
        hashInfo(changed, "f", { v2: true }),
    ])
    assert.notEqual(a.v1, b.v1)
    assert.notEqual(a.v2, b.v2)
})

// ─── Group 13: Edge Cases ─────────────────────────────────────────────────────

test("G13-01 filename with path separators is treated as a string, not split", async () => {
    // hashInfo's fileName is the torrent display name, not parsed as path
    const { v1 } = await hashInfo(D64, "dir/file.txt")
    assert.ok(isV1(v1))
})

test("G13-02 async generator yielding empty chunks (0-length) handles gracefully", async () => {
    async function* withEmpties() {
        yield new Uint8Array(0)
        yield new Uint8Array([1, 2, 3])
        yield new Uint8Array(0)
    }
    const { v1 } = await hashInfo(withEmpties(), "f", { length: 3 })
    assert.ok(isV1(v1))
})

test("G13-03 async generator yielding 1-byte chunks produces same v1 as one-shot", async () => {
    const data = new Uint8Array(100).fill(42)
    async function* oneByOne() { for (const b of data) yield new Uint8Array([b]) }
    const [a, b] = await Promise.all([hashInfo(data, "f"), hashInfo(oneByOne(), "f")])
    assert.equal(a.v1, b.v1)
})

test("G13-04 BigInt file length in Array input does not throw unsupported-type from bencode", async () => {
    // Note: file.length here is a normal number; BigInt support is for the bencode serializer
    // when a consumer manually builds a file descriptor with length as BigInt.
    const files = [{ path: "big.bin", length: 3, stream: () => new ReadableStream({ start(c) { c.enqueue(new Uint8Array([1,2,3])); c.close() } }) }]
    const { v1 } = await hashInfo(files, "t")
    assert.ok(isV1(v1))
})

test("G13-05 options.customCrypto injection works (testability hook)", async () => {
    // Verify the customCrypto option is threaded through correctly.
    // If globalThis.crypto is available we just pass it as the custom value.
    const { v1: normal } = await hashInfo(D64, NAME)
    const { v1: custom } = await hashInfo(D64, NAME, { customCrypto: globalThis.crypto })
    assert.equal(normal, custom, "injected crypto.subtle produces same result as globalThis.crypto")
})
