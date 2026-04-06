import { init, $client } from "./Torrent/init.js"
import { restore } from "./Torrent/restore.js"
import { $save, $load, $delete } from "./Torrent/persist.js"
import { on } from "./Torrent/on.js"
import { off } from "./Torrent/off.js"
import { add } from "./Torrent/add.js"
import { seed } from "./Torrent/seed.js"
import { get } from "./Torrent/get.js"
import { list } from "./Torrent/list.js"
import { remove } from "./Torrent/remove.js"
import { destroy } from "./Torrent/destroy.js"
import { tracker } from "./Torrent/tracker.js"
import { rotate } from "./Torrent/rotate.js"
import { reconnect } from "./Torrent/reconnect.js"
import { $emit } from "./Torrent/emit.js"
import { $track } from "./Torrent/track.js"
import { $snapshot } from "./Torrent/snapshot.js"
import { hash } from "./Torrent/hash.js"

export { TorrentError } from "./Torrent/error.js"
export { hash }

export class Torrent {
    constructor(options = {}) {
        this.options = options
        this.client = null
        this.ready = null
        this.torrents = new Map()
        this.callbacks = new Map()
        this.bound = new WeakSet()
        this.pool = []
        this.trackers = { http: [], https: [], ws: [], wss: [] }
        this._active = null
        this._scheme = null
        this._reconnect = 0
    }

    init = init
    on = on
    off = off
    add = add
    seed = seed
    get = get
    list = list
    remove = remove
    destroy = destroy
    tracker = tracker
    rotate = rotate
    reconnect = reconnect

    $client = $client
    $emit = $emit
    $track = $track
    $snapshot = $snapshot
    $save = $save
    $load = $load
    $delete = $delete

    restore = restore
    hash = hash
}

export default Torrent