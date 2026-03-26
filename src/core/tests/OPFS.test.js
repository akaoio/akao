import Test from "../Test.js"
import OPFS from "../OPFS.js"

let _counter = 0
function fsRoot() { return `__test_opfs_${_counter++}` }

function enc(str) { return new TextEncoder().encode(str) }
function dec(buf) { return new TextDecoder().decode(buf) }

// ─── write + read ─────────────────────────────────────────────────────────────

Test.describe("OPFS — write + read", () => {

    Test.it("round-trips a string value", async () => {
        const fs = new OPFS({ root: fsRoot() })
        await fs.write(["hello.txt"], enc("hello world"))
        const buf = await fs.read(["hello.txt"])
        Test.assert.equal(dec(buf), "hello world")
    }, { browser: true })

    Test.it("overwrites an existing file", async () => {
        const fs = new OPFS({ root: fsRoot() })
        await fs.write(["f.txt"], enc("first"))
        await fs.write(["f.txt"], enc("second"))
        const buf = await fs.read(["f.txt"])
        Test.assert.equal(dec(buf), "second")
    }, { browser: true })

    Test.it("writes into a subdirectory", async () => {
        const fs = new OPFS({ root: fsRoot() })
        await fs.write(["sub", "data.txt"], enc("nested"))
        const buf = await fs.read(["sub", "data.txt"])
        Test.assert.equal(dec(buf), "nested")
    }, { browser: true })

})

// ─── exist ────────────────────────────────────────────────────────────────────

Test.describe("OPFS — exist", () => {

    Test.it("returns true for a file that has been written", async () => {
        const fs = new OPFS({ root: fsRoot() })
        await fs.write(["file.txt"], enc("x"))
        const yes = await fs.exist(["file.txt"])
        Test.assert.truthy(yes)
    }, { browser: true })

    Test.it("returns false for a file that does not exist", async () => {
        const fs = new OPFS({ root: fsRoot() })
        const no = await fs.exist(["nonexistent.txt"])
        Test.assert.falsy(no)
    }, { browser: true })

    Test.it("returns true for a created directory", async () => {
        const fs = new OPFS({ root: fsRoot() })
        await fs.mkdir(["mydir"])
        const yes = await fs.exist(["mydir"])
        Test.assert.truthy(yes)
    }, { browser: true })

})

// ─── list ─────────────────────────────────────────────────────────────────────

Test.describe("OPFS — list", () => {

    Test.it("lists files in a directory", async () => {
        const fs = new OPFS({ root: fsRoot() })
        await fs.write(["a.txt"], enc("a"))
        await fs.write(["b.txt"], enc("b"))
        const names = await fs.list([])
        Test.assert.truthy(names.includes("a.txt"))
        Test.assert.truthy(names.includes("b.txt"))
    }, { browser: true })

    Test.it("lists files in a subdirectory", async () => {
        const fs = new OPFS({ root: fsRoot() })
        await fs.write(["sub", "x.txt"], enc("x"))
        await fs.write(["sub", "y.txt"], enc("y"))
        const names = await fs.list(["sub"])
        Test.assert.truthy(names.includes("x.txt"))
        Test.assert.truthy(names.includes("y.txt"))
        Test.assert.equal(names.length, 2)
    }, { browser: true })

    Test.it("returns empty array for an empty directory", async () => {
        const fs = new OPFS({ root: fsRoot() })
        await fs.mkdir(["empty"])
        const names = await fs.list(["empty"])
        Test.assert.deepEqual(names, [])
    }, { browser: true })

})

// ─── del ──────────────────────────────────────────────────────────────────────

Test.describe("OPFS — del", () => {

    Test.it("removes a file", async () => {
        const fs = new OPFS({ root: fsRoot() })
        await fs.write(["bye.txt"], enc("bye"))
        await fs.del(["bye.txt"])
        const exists = await fs.exist(["bye.txt"])
        Test.assert.falsy(exists)
    }, { browser: true })

    Test.it("removes a directory recursively", async () => {
        const fs = new OPFS({ root: fsRoot() })
        await fs.write(["dir", "child.txt"], enc("c"))
        await fs.del(["dir"])
        const exists = await fs.exist(["dir"])
        Test.assert.falsy(exists)
    }, { browser: true })

})

// ─── mkdir ────────────────────────────────────────────────────────────────────

Test.describe("OPFS — mkdir", () => {

    Test.it("creates a directory that can be listed", async () => {
        const fs = new OPFS({ root: fsRoot() })
        await fs.mkdir(["newdir"])
        const exists = await fs.exist(["newdir"])
        Test.assert.truthy(exists)
        const names = await fs.list(["newdir"])
        Test.assert.deepEqual(names, [])
    }, { browser: true })

    Test.it("is idempotent — creating twice does not throw", async () => {
        const fs = new OPFS({ root: fsRoot() })
        await fs.mkdir(["d"])
        await fs.mkdir(["d"])
        Test.assert.truthy(await fs.exist(["d"]))
    }, { browser: true })

})

// ─── move ─────────────────────────────────────────────────────────────────────

Test.describe("OPFS — move", () => {

    Test.it("file is accessible at new path after move", async () => {
        const fs = new OPFS({ root: fsRoot() })
        await fs.write(["src.txt"], enc("moved"))
        await fs.move(["src.txt"], ["dst.txt"])
        const buf = await fs.read(["dst.txt"])
        Test.assert.equal(dec(buf), "moved")
    }, { browser: true })

    Test.it("file no longer exists at old path after move", async () => {
        const fs = new OPFS({ root: fsRoot() })
        await fs.write(["old.txt"], enc("data"))
        await fs.move(["old.txt"], ["new.txt"])
        const exists = await fs.exist(["old.txt"])
        Test.assert.falsy(exists)
    }, { browser: true })

    Test.it("can move a file into a subdirectory", async () => {
        const fs = new OPFS({ root: fsRoot() })
        await fs.mkdir(["target"])
        await fs.write(["file.txt"], enc("content"))
        await fs.move(["file.txt"], ["target", "file.txt"])
        const buf = await fs.read(["target", "file.txt"])
        Test.assert.equal(dec(buf), "content")
    }, { browser: true })

})
