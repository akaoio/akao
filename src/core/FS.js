import { root } from "./FS/root.js"
import { join } from "./FS/join.js"
import { ensure } from "./FS/ensure.js"
import { remove } from "./FS/remove.js"
import { write } from "./FS/write.js"
import { load } from "./FS/load.js"
import { download } from "./FS/download.js"
import { find } from "./FS/find.js"
import { copy } from "./FS/copy.js"
import { dir } from "./FS/dir.js"
import { exist } from "./FS/exist.js"
import { isDirectory } from "./FS/isDirectory.js"
import { hash } from "./FS/hash.js"
import { move } from "./FS/move.js"

export class FS {
    static root = root
    static join = join
    static ensure = ensure
    static remove = remove
    static write = write
    static load = load
    static download = download
    static find = find
    static copy = copy
    static dir = dir
    static exist = exist
    static isDirectory = isDirectory
    static hash = hash
    static move = move
}

export default FS
