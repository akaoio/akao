import { init } from "./RTC/init.js"
import { on } from "./RTC/on.js"
import { off } from "./RTC/off.js"
import { offer } from "./RTC/offer.js"
import { answer } from "./RTC/answer.js"
import { accept } from "./RTC/accept.js"
import { send } from "./RTC/send.js"
import { close } from "./RTC/close.js"
import { snapshot } from "./RTC/snapshot.js"
import { signal } from "./RTC/signal.js"
import { $emit } from "./RTC/emit.js"
import { $wait } from "./RTC/wait.js"

export class RTC {
    constructor(options = {}) {
        const channel = options.channel || "data"
        const config = options.config || {}
        const ices = options.iceservers || options.iceServers
        const dataopts = options.dataopts || options.channelOptions || {}
        const offeropts = options.offeropts || options.offerOptions || {}
        const timeout = options.timeout || 10000

        this.channel = channel
        this.config = ices?.length ? { ...config, iceServers: ices } : config
        this.dataopts = dataopts
        this.offeropts = offeropts
        this.timeout = timeout
        this.connection = null
        this.data = null
        this.callbacks = new Map()
        this.candidates = []
    }

    init = init
    on = on
    off = off
    offer = offer
    answer = answer
    accept = accept
    send = send
    close = close
    snapshot = snapshot
    signal = signal

    $emit = $emit
    $wait = $wait
}

export default RTC