import { init } from "./Wave/init.js"
import { request } from "./Wave/request.js"
import { check } from "./Wave/check.js"
import { permission } from "./Wave/permission.js"
import { encode } from "./Wave/encode.js"
import { decode } from "./Wave/decode.js"

export class Wave {
    constructor() {
        this.wave = null
        this.ready = null
    }

    init = init
    static check = check
    static permission = permission
    static request = request
    static encode = encode
    static decode = decode
}