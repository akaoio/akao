import { $call } from "./SQL/call.js"
import { exec } from "./SQL/exec.js"
import { run } from "./SQL/run.js"
import { get } from "./SQL/get.js"
import { all } from "./SQL/all.js"
import { batch } from "./SQL/batch.js"

export class SQL {
    constructor({ name = "system" } = {}) {
        this.name = name
        // this.ready resolves once the worker has opened the database file.
        // Await it before using any query method, or let the methods await it internally.
        this.ready = this.$call("open", { name })
    }

    // Internal dispatch
    $call = $call

    // Public API
    exec = exec
    run = run
    get = get
    all = all
    batch = batch
}

export default SQL
