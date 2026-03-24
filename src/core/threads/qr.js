import Thread from "/core/Thread.js"
import QR from "/core/QR.js"

const thread = new Thread()

const state = {
    engine: null,
    options: {
        alsoTryWithoutScanRegion: true,
        returnDetailedScanResult: true,
        scanRegion: null
    },
    sourceCanvas: typeof OffscreenCanvas !== "undefined" ? new OffscreenCanvas(1, 1) : null,
    sourceContext: null,
    scanCanvas: typeof OffscreenCanvas !== "undefined" ? new OffscreenCanvas(1, 1) : null
}

function ensureCompatGlobals() {
    if (typeof globalThis.window === "undefined") globalThis.window = globalThis
    if (typeof globalThis.Image === "undefined") globalThis.Image = class Image {}
    if (typeof globalThis.HTMLVideoElement === "undefined") globalThis.HTMLVideoElement = class HTMLVideoElement {}
    if (typeof globalThis.HTMLCanvasElement === "undefined") globalThis.HTMLCanvasElement = class HTMLCanvasElement {}
    if (typeof globalThis.SVGImageElement === "undefined") globalThis.SVGImageElement = class SVGImageElement {}
}

function normalizeError(error) {
    if (!error) return "Unknown error"
    if (typeof error === "string") return error
    return error.message || String(error)
}

function ensureSourceContext(width, height) {
    if (!state.sourceCanvas) throw new Error("OffscreenCanvas is not available in this worker")
    if (state.sourceCanvas.width !== width) state.sourceCanvas.width = width
    if (state.sourceCanvas.height !== height) state.sourceCanvas.height = height
    if (!state.sourceContext) state.sourceContext = state.sourceCanvas.getContext("2d", { willReadFrequently: true })
    return state.sourceContext
}

async function ensureEngine() {
    ensureCompatGlobals()
    if (!state.engine) state.engine = await QR.scan.createQrEngine()
    return state.engine
}

thread.init = async function () {
    await ensureEngine()
}

thread.configure = async function (params = {}) {
    if (params.scanRegion !== undefined) state.options.scanRegion = params.scanRegion || null
    if (params.alsoTryWithoutScanRegion !== undefined) state.options.alsoTryWithoutScanRegion = !!params.alsoTryWithoutScanRegion
    return { ok: true, options: state.options }
}

thread.scan = async function ({ bitmap, imageData, width, height, scanRegion, alsoTryWithoutScanRegion } = {}) {
    await ensureEngine()

    let source = bitmap
    if (!source && imageData) {
        width = width || imageData.width
        height = height || imageData.height
        const context = ensureSourceContext(width, height)
        context.putImageData(imageData, 0, 0)
        source = state.sourceCanvas
    }

    if (!source) return { ok: false, found: false, error: "No frame provided" }

    try {
        const result = await QR.scan.scanImage(source, {
            qrEngine: state.engine,
            canvas: state.scanCanvas,
            scanRegion: scanRegion || state.options.scanRegion,
            alsoTryWithoutScanRegion: alsoTryWithoutScanRegion ?? state.options.alsoTryWithoutScanRegion,
            returnDetailedScanResult: true
        })
        return { ok: true, found: true, data: result.data, cornerPoints: result.cornerPoints }
    } catch (error) {
        const message = normalizeError(error)
        if (message === QR.scan.NO_QR_CODE_FOUND || /no qr code found/i.test(message)) {
            return { ok: false, found: false }
        }
        return { ok: false, found: false, error: message }
    } finally {
        if (bitmap && typeof bitmap.close === "function") bitmap.close()
    }
}

thread.destroy = async function () {
    if (state.engine && typeof state.engine.terminate === "function") state.engine.terminate()
    state.engine = null
    state.sourceContext = null
    state.sourceCanvas = typeof OffscreenCanvas !== "undefined" ? new OffscreenCanvas(1, 1) : null
    state.scanCanvas = typeof OffscreenCanvas !== "undefined" ? new OffscreenCanvas(1, 1) : null
    return { ok: true }
}