import { $root } from "./OPFS/root.js"
import { $dir } from "./OPFS/dir.js"
import { $handle } from "./OPFS/handle.js"
import { load } from "./OPFS/load.js"
import { write } from "./OPFS/write.js"
import { remove } from "./OPFS/remove.js"
import { move } from "./OPFS/move.js"
import { mkdir } from "./OPFS/mkdir.js"
import { dir } from "./OPFS/dir.js"
import { exist } from "./OPFS/exist.js"

export class OPFS {
    constructor({ root = "" } = {}) {
        // Optional subdirectory prefix — all paths are relative to this
        this.root = root
        // Per-path write queue — instance-level so multiple OPFS instances don't share locks
        this._locks = new Map()
    }

    // Prepend instance root to a user-supplied path array
    _path(path = []) {
        return this.root ? [this.root, ...path] : [...path]
    }

    // Public API
    load = load
    write = write
    remove = remove
    move = move
    mkdir = mkdir
    dir = dir
    exist = exist

    // Internal helpers
    $root = $root
    $dir = $dir
    $handle = $handle
}

export default OPFS
