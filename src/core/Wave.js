import { init } from "./Wave/init.js"
import { check } from "./Wave/check.js"
import { request } from "./Wave/request.js"

export class Wave {
    constructor() {
        this.wave = null
        this.ready = null
    }

    init = init
    check = check
    request = request

    static async setup(params = {}) {
        const worker = await import("./Wave/worker.js")
        return worker.setup(params)
    }

    static async encode(params = {}, options = {}) {
        const worker = await import("./Wave/worker.js")
        return worker.encode(params, options)
    }

    static async decode(params = {}, options = {}) {
        const worker = await import("./Wave/worker.js")
        return worker.decode(params, options)
    }

    static async reset() {
        const worker = await import("./Wave/worker.js")
        return worker.reset()
    }

    static async dispose() {
        const worker = await import("./Wave/worker.js")
        return worker.dispose()
    }
}