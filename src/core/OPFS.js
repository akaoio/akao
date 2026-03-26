import { $root } from "./OPFS/root.js"
import { $dir } from "./OPFS/dir.js"
import { $handle } from "./OPFS/handle.js"
import { read } from "./OPFS/read.js"
import { write } from "./OPFS/write.js"
import { del } from "./OPFS/del.js"
import { move } from "./OPFS/move.js"
import { mkdir } from "./OPFS/mkdir.js"
import { list } from "./OPFS/list.js"
import { exist } from "./OPFS/exist.js"

export class OPFS {
    constructor({ root = "" } = {}) {
        // Optional subdirectory prefix — all paths are relative to this
        this.root = root
    }

    // Prepend instance root to a user-supplied path array
    _path(path = []) {
        return this.root ? [this.root, ...path] : [...path]
    }

    // Public API
    read = read
    write = write
    del = del
    move = move
    mkdir = mkdir
    list = list
    exist = exist

    // Internal helpers
    $root = $root
    $dir = $dir
    $handle = $handle
}

export default OPFS
