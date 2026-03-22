import { init } from "./Wave/init.js"
import { check } from "./Wave/check.js"
import { request } from "./Wave/request.js"

export class Wave {
    constructor() {
        this.wave = null
        this.ready = null
        this.init()
    }

    init = init
    check = check
    request = request
}