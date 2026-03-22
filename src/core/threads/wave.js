import Thread from "/core/Thread.js"
import ggwave_factory from "/core/Wave.js"

const thread = new Thread()

let ggwave = null
let instance = null
let readyPromise = null

async function ensureReady() {
    if (!readyPromise) {
        readyPromise = ggwave_factory().then((module) => {
            ggwave = module
            return module
        })
    }
    if (!ggwave) ggwave = await readyPromise
    return ggwave
}

function normalizeWaveform(waveform) {
    if (!waveform) return []
    if (Array.isArray(waveform)) return waveform
    if (ArrayBuffer.isView(waveform)) return Array.from(waveform)
    return []
}

function ensureInstance(parameters) {
    if (!ggwave) throw new Error("ggwave module is not ready")
    if (instance) return instance
    const defaults = ggwave.getDefaultParameters()
    instance = ggwave.init(parameters || defaults)
    return instance
}

thread.init = async function () {
    await ensureReady()
    ensureInstance()
}

thread.status = async function () {
    await ensureReady()
    return {
        ready: !!ggwave,
        instance: !!instance
    }
}

thread.getDefaultParameters = async function () {
    await ensureReady()
    return ggwave.getDefaultParameters()
}

thread.configure = async function (params = {}) {
    await ensureReady()
    if (instance) ggwave.free(instance)
    instance = ggwave.init(params?.parameters || ggwave.getDefaultParameters())
    return { configured: true }
}

thread.encode = async function (params = {}) {
    await ensureReady()
    const active = ensureInstance(params?.parameters)
    const payload = params?.payload || ""
    const protocolId = params?.protocolId ?? ggwave.TxProtocolId.GGWAVE_TX_PROTOCOL_AUDIBLE_FAST
    const volume = params?.volume ?? 10
    const waveform = ggwave.encode(active, payload, protocolId, volume)
    return {
        waveform: normalizeWaveform(waveform),
        length: waveform?.length || 0
    }
}

thread.decode = async function (params = {}) {
    await ensureReady()
    const active = ensureInstance(params?.parameters)
    const waveform = normalizeWaveform(params?.waveform)
    const payload = ggwave.decode(active, waveform)
    return {
        payload: payload || ""
    }
}

thread.reset = async function () {
    await ensureReady()
    if (instance) {
        ggwave.free(instance)
        instance = null
    }
    ensureInstance()
    return { reset: true }
}