import Thread from "/core/Thread.js"
import { Wave } from "/core/Wave.js"

const thread = new Thread()

let wave = null
const state = {
    module: null,
    receiver: null,
    sender: null,
    params: null,
    decoder: typeof TextDecoder !== "undefined" ? new TextDecoder() : null,
    protocolId: null,
    volume: 48,
    sampleRate: 48000
}

function bytesToInt8(input) {
    if (!input) return null
    if (input instanceof Int8Array) return input
    if (input instanceof Uint8Array) return new Int8Array(input.buffer, input.byteOffset, input.byteLength)
    if (input instanceof ArrayBuffer) return new Int8Array(input)
    return null
}

function freeInstance(instance) {
    if (!instance || !state.module?.free) return
    try {
        state.module.free(instance)
    } catch {}
}

function ensureParams(overrides = {}) {
    const defaults = state.module.getDefaultParameters()
    return {
        ...defaults,
        ...overrides,
        sampleRateInp: overrides?.sampleRateInp || overrides?.sampleRate || defaults.sampleRateInp,
        sampleRateOut: overrides?.sampleRateOut || overrides?.sampleRate || defaults.sampleRateOut,
        sampleRate: overrides?.sampleRate || defaults.sampleRate,
        sampleFormatInp: state.module.SampleFormat.GGWAVE_SAMPLE_FORMAT_I16,
        sampleFormatOut: state.module.SampleFormat.GGWAVE_SAMPLE_FORMAT_I16,
        operatingMode: overrides?.operatingMode ?? defaults.operatingMode
    }
}

function normalizePayload(payload = "") {
    if (typeof payload === "string") return payload
    try {
        return JSON.stringify(payload)
    } catch {
        return String(payload)
    }
}

async function ensureModule() {
    if (!wave) wave = new Wave()
    if (!state.module) state.module = await wave.init()
    return state.module
}

function ensureInstances(overrides = {}) {
    if (!state.module) throw new Error("Wave module is not initialized")
    const params = ensureParams({ ...state.params, ...overrides })
    state.params = params
    state.sampleRate = params.sampleRateOut || params.sampleRate || state.sampleRate
    if (state.protocolId == null) state.protocolId = state.module.ProtocolId.GGWAVE_PROTOCOL_AUDIBLE_NORMAL

    if (!state.receiver) state.receiver = state.module.init(params)
    if (!state.sender) state.sender = state.module.init(params)
    return params
}

thread.init = async function () {
    await ensureModule()
    ensureInstances()
}

thread.configure = async function (params = {}) {
    await ensureModule()
    if (params?.reset) {
        freeInstance(state.receiver)
        freeInstance(state.sender)
        state.receiver = null
        state.sender = null
    }
    if (params?.protocolId !== undefined) state.protocolId = params.protocolId
    if (params?.volume !== undefined) state.volume = params.volume
    state.params = { ...(state.params || {}), ...params }
    const current = ensureInstances(state.params)
    return {
        ok: true,
        params: {
            sampleRate: current.sampleRate,
            sampleRateInp: current.sampleRateInp,
            sampleRateOut: current.sampleRateOut,
            samplesPerFrame: current.samplesPerFrame,
            operatingMode: current.operatingMode,
            protocolId: state.protocolId,
            volume: state.volume
        }
    }
}

thread.encode = async function ({ message, protocolId, volume } = {}) {
    await ensureModule()
    ensureInstances()
    const payload = normalizePayload(message)
    if (!payload) return { ok: false, error: "No message provided" }
    const output = state.module.encode(state.sender, payload, protocolId ?? state.protocolId, volume ?? state.volume)
    if (!output?.length) return { ok: false, error: "Unable to encode wave payload" }
    return {
        ok: true,
        bytes: Array.from(output),
        sampleRate: state.sampleRate,
        payload
    }
}

thread.decode = async function ({ bytes } = {}) {
    await ensureModule()
    ensureInstances()
    const chunk = bytesToInt8(bytes)
    if (!chunk?.length) return { ok: false, found: false, error: "No audio bytes provided" }
    try {
        const output = state.module.decode(state.receiver, chunk)
        if (!output?.length) return { ok: true, found: false }
        const message = state.decoder ? state.decoder.decode(output) : String.fromCharCode(...output)
        return { ok: true, found: true, message }
    } catch (error) {
        return { ok: false, found: false, error: error?.message || String(error) }
    }
}

thread.reset = async function () {
    await ensureModule()
    freeInstance(state.receiver)
    freeInstance(state.sender)
    state.receiver = null
    state.sender = null
    ensureInstances(state.params || {})
    return { ok: true }
}

thread.destroy = async function () {
    freeInstance(state.receiver)
    freeInstance(state.sender)
    state.receiver = null
    state.sender = null
    state.params = null
    state.module = null
    return { ok: true }
}