import Test from "../Test.js"
import { hash, TorrentError } from "../Torrent.js"

// ─── Helpers ─────────────────────────────────────────────────────────────────

function makeBytes(length, fill = 0xAB) {
    const buf = new Uint8Array(length)
    buf.fill(fill)
    return buf
}

function makeStreamFactory(bytes) {
    return () => new ReadableStream({ start(c) { c.enqueue(bytes); c.close() } })
}

async function* asyncChunks(data, chunkSize = 64) {
    for (let offset = 0; offset < data.length; offset += chunkSize)
        yield data.subarray(offset, offset + chunkSize)
}

// ─── TorrentError ─────────────────────────────────────────────────────────────

Test.describe("TorrentError", () => {

    Test.it("has correct name, code, and message", () => {
        const err = new TorrentError("INVALID_INPUT", "bad input")
        Test.assert.equal(err.name, "TorrentError")
        Test.assert.equal(err.code, "INVALID_INPUT")
        Test.assert.equal(err.message, "bad input")
    })

    Test.it("is an instance of Error and TorrentError", () => {
        const err = new TorrentError("SOME_CODE", "msg")
        Test.assert.truthy(err instanceof Error)
        Test.assert.truthy(err instanceof TorrentError)
    })

    Test.it("supports different error codes", () => {
        const codes = ["INVALID_INPUT", "CRYPTO_UNAVAILABLE", "INVALID_PATH", "STREAM_FAILED"]
        for (const code of codes) {
            const err = new TorrentError(code, "msg")
            Test.assert.equal(err.code, code)
        }
    })

})

// ─── Uint8Array input ─────────────────────────────────────────────────────────

Test.describe("Torrent.hash — Uint8Array input", () => {

    Test.it("returns v1 as 40-char hex string", async () => {
        const result = await hash(makeBytes(1024), "file.bin")
        Test.assert.equal(typeof result.v1, "string")
        Test.assert.equal(result.v1.length, 40)
        Test.assert.truthy(/^[0-9a-f]+$/.test(result.v1))
    })

    Test.it("returns null v2 by default", async () => {
        const result = await hash(makeBytes(512), "file.bin")
        Test.assert.equal(result.v2, null)
    })

    Test.it("is deterministic — same bytes produce same v1", async () => {
        const bytes = makeBytes(512, 0x42)
        const r1 = await hash(bytes, "file.bin")
        const r2 = await hash(bytes, "file.bin")
        Test.assert.equal(r1.v1, r2.v1)
    })

    Test.it("different content produces different v1", async () => {
        const r1 = await hash(makeBytes(512, 0x01), "file.bin")
        const r2 = await hash(makeBytes(512, 0x02), "file.bin")
        Test.assert.notEqual(r1.v1, r2.v1)
    })

    Test.it("single byte input is accepted", async () => {
        const result = await hash(new Uint8Array([0xFF]), "tiny.bin")
        Test.assert.equal(result.v1.length, 40)
    })

})

// ─── ArrayBuffer input ────────────────────────────────────────────────────────

Test.describe("Torrent.hash — ArrayBuffer input", () => {

    Test.it("returns v1 for ArrayBuffer", async () => {
        const buf = makeBytes(256, 0x55).buffer
        const result = await hash(buf, "data.bin")
        Test.assert.equal(result.v1.length, 40)
        Test.assert.equal(result.v2, null)
    })

    Test.it("produces same v1 as equivalent Uint8Array", async () => {
        const bytes = makeBytes(256, 0x77)
        const r1 = await hash(bytes, "f.bin")
        const r2 = await hash(bytes.buffer, "f.bin")
        Test.assert.equal(r1.v1, r2.v1)
    })

})

// ─── AsyncIterable input ─────────────────────────────────────────────────────

Test.describe("Torrent.hash — AsyncIterable input", () => {

    Test.it("accepts async generator and returns v1", async () => {
        const data = makeBytes(2048, 0x33)
        const result = await hash(asyncChunks(data), "stream.bin", { length: data.length })
        Test.assert.equal(result.v1.length, 40)
        Test.assert.equal(result.v2, null)
    })

    Test.it("async iterable matches Uint8Array hash for same data", async () => {
        const data = makeBytes(512, 0x22)
        const r1 = await hash(data, "file.bin")
        const r2 = await hash(asyncChunks(data, 64), "file.bin", { length: data.length })
        Test.assert.equal(r1.v1, r2.v1)
    })

    Test.it("chunk size does not affect the hash", async () => {
        const data = makeBytes(1024, 0x55)
        const r1 = await hash(asyncChunks(data, 32), "file.bin", { length: data.length })
        const r2 = await hash(asyncChunks(data, 256), "file.bin", { length: data.length })
        Test.assert.equal(r1.v1, r2.v1)
    })

})

// ─── v2 option (BEP 52 hybrid) ───────────────────────────────────────────────

Test.describe("Torrent.hash — v2 option", () => {

    Test.it("returns v2 as 64-char hex string when v2:true", async () => {
        const result = await hash(makeBytes(512, 0xAA), "file.bin", { v2: true })
        Test.assert.equal(typeof result.v2, "string")
        Test.assert.equal(result.v2.length, 64)
        Test.assert.truthy(/^[0-9a-f]+$/.test(result.v2))
    })

    Test.it("v1 in hybrid mode differs from v1-only for same content", async () => {
        const bytes = makeBytes(512, 0xBB)
        const v1only = await hash(bytes, "file.bin")
        const hybrid = await hash(bytes, "file.bin", { v2: true })
        Test.assert.notEqual(v1only.v1, hybrid.v1)
    })

    Test.it("v2 hash is deterministic", async () => {
        const bytes = makeBytes(1024, 0xCC)
        const r1 = await hash(bytes, "file.bin", { v2: true })
        const r2 = await hash(bytes, "file.bin", { v2: true })
        Test.assert.equal(r1.v2, r2.v2)
    })

    Test.it("different content produces different v2", async () => {
        const r1 = await hash(makeBytes(512, 0x01), "f.bin", { v2: true })
        const r2 = await hash(makeBytes(512, 0x02), "f.bin", { v2: true })
        Test.assert.notEqual(r1.v2, r2.v2)
    })

})

// ─── Multi-file array ─────────────────────────────────────────────────────────

Test.describe("Torrent.hash — multi-file array", () => {

    Test.it("accepts array of file descriptors and returns v1", async () => {
        const a = makeBytes(256, 0x01)
        const b = makeBytes(256, 0x02)
        const files = [
            { path: "dir/a.bin", length: a.length, stream: makeStreamFactory(a) },
            { path: "dir/b.bin", length: b.length, stream: makeStreamFactory(b) },
        ]
        const result = await hash(files, "myTorrent")
        Test.assert.equal(result.v1.length, 40)
        Test.assert.equal(result.v2, null)
    })

    Test.it("multi-file v1 is deterministic regardless of input order", async () => {
        const a = makeBytes(256, 0x01)
        const b = makeBytes(256, 0x02)
        const filesAB = [
            { path: "dir/a.bin", length: a.length, stream: makeStreamFactory(a) },
            { path: "dir/b.bin", length: b.length, stream: makeStreamFactory(b) },
        ]
        const filesBA = [
            { path: "dir/b.bin", length: b.length, stream: makeStreamFactory(b) },
            { path: "dir/a.bin", length: a.length, stream: makeStreamFactory(a) },
        ]
        const r1 = await hash(filesAB, "myTorrent")
        const r2 = await hash(filesBA, "myTorrent")
        Test.assert.equal(r1.v1, r2.v1)
    })

    Test.it("zero-byte files are included without being streamed", async () => {
        const data = makeBytes(256, 0x05)
        const files = [
            { path: "empty.txt", length: 0, stream: () => { throw new Error("must not stream zero-byte file") } },
            { path: "data.bin", length: data.length, stream: makeStreamFactory(data) },
        ]
        const result = await hash(files, "mixed")
        Test.assert.equal(result.v1.length, 40)
    })

    Test.it("multi-file v2 returns both v1 and v2", async () => {
        const a = makeBytes(256, 0x11)
        const b = makeBytes(256, 0x22)
        const files = [
            { path: "a.bin", length: a.length, stream: makeStreamFactory(a) },
            { path: "b.bin", length: b.length, stream: makeStreamFactory(b) },
        ]
        const result = await hash(files, "hybrid", { v2: true })
        Test.assert.equal(result.v1.length, 40)
        Test.assert.equal(result.v2.length, 64)
    })

})

// ─── options ──────────────────────────────────────────────────────────────────

Test.describe("Torrent.hash — options", () => {

    Test.it("private option changes the v1 hash", async () => {
        const bytes = makeBytes(256, 0x11)
        const r1 = await hash(bytes, "file.bin")
        const r2 = await hash(bytes, "file.bin", { private: true })
        Test.assert.notEqual(r1.v1, r2.v1)
    })

    Test.it("source option changes the v1 hash", async () => {
        const bytes = makeBytes(256, 0x22)
        const r1 = await hash(bytes, "file.bin")
        const r2 = await hash(bytes, "file.bin", { source: "mytracker" })
        Test.assert.notEqual(r1.v1, r2.v1)
    })

    Test.it("different pieceLength produces different v1", async () => {
        const bytes = makeBytes(16384, 0x33)
        const r1 = await hash(bytes, "file.bin", { pieceLength: 16384 })
        const r2 = await hash(bytes, "file.bin", { pieceLength: 32768 })
        Test.assert.notEqual(r1.v1, r2.v1)
    })

    Test.it("same options always produce same hash", async () => {
        const bytes = makeBytes(512, 0x44)
        const opts = { private: true, source: "tracker", pieceLength: 16384 }
        const r1 = await hash(bytes, "file.bin", opts)
        const r2 = await hash(bytes, "file.bin", opts)
        Test.assert.equal(r1.v1, r2.v1)
    })

})

// ─── error handling ───────────────────────────────────────────────────────────

Test.describe("Torrent.hash — error handling", () => {

    Test.it("throws TorrentError UNSUPPORTED_FORMAT for a number", async () => {
        const err = await Test.assert.rejects(hash(12345, "file.bin"))
        Test.assert.truthy(err instanceof TorrentError)
        Test.assert.equal(err.code, "UNSUPPORTED_FORMAT")
    })

    Test.it("throws TorrentError UNSUPPORTED_FORMAT for null", async () => {
        const err = await Test.assert.rejects(hash(null, "file.bin"))
        Test.assert.truthy(err instanceof TorrentError)
        Test.assert.equal(err.code, "UNSUPPORTED_FORMAT")
    })

    Test.it("throws TorrentError INVALID_INPUT for empty files array", async () => {
        const err = await Test.assert.rejects(hash([], "name"))
        Test.assert.truthy(err instanceof TorrentError)
        Test.assert.equal(err.code, "INVALID_INPUT")
    })

    Test.it("throws TorrentError INVALID_PATH for path traversal (..)", async () => {
        const bytes = makeBytes(64)
        const files = [{ path: "dir/../etc/passwd", length: bytes.length, stream: makeStreamFactory(bytes) }]
        const err = await Test.assert.rejects(hash(files, "name"))
        Test.assert.truthy(err instanceof TorrentError)
        Test.assert.equal(err.code, "INVALID_PATH")
    })

    Test.it("throws TorrentError INVALID_PIECE_LENGTH for non-power-of-two", async () => {
        const err = await Test.assert.rejects(hash(makeBytes(256), "file.bin", { pieceLength: 20000 }))
        Test.assert.truthy(err instanceof TorrentError)
        Test.assert.equal(err.code, "INVALID_PIECE_LENGTH")
    })

    Test.it("throws TorrentError INVALID_PIECE_LENGTH for value below 16384", async () => {
        const err = await Test.assert.rejects(hash(makeBytes(256), "file.bin", { pieceLength: 8192 }))
        Test.assert.truthy(err instanceof TorrentError)
        Test.assert.equal(err.code, "INVALID_PIECE_LENGTH")
    })

    Test.it("throws TorrentError STREAM_LENGTH_MISMATCH when declared length differs from actual", async () => {
        const bytes = makeBytes(256, 0xAA)
        const files = [{ path: "file.bin", length: 999, stream: makeStreamFactory(bytes) }]
        const err = await Test.assert.rejects(hash(files, "name"))
        Test.assert.truthy(err instanceof TorrentError)
        Test.assert.equal(err.code, "STREAM_LENGTH_MISMATCH")
    })

    Test.it("throws TorrentError INVALID_INPUT for negative file length", async () => {
        const files = [{ path: "file.bin", length: -1, stream: makeStreamFactory(makeBytes(64)) }]
        const err = await Test.assert.rejects(hash(files, "name"))
        Test.assert.truthy(err instanceof TorrentError)
        Test.assert.equal(err.code, "INVALID_INPUT")
    })

})
