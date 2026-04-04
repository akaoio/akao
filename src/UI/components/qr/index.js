import QR from "/core/QR.js"
import Events from "/core/Events.js"
import template from "./template.js"
import { html, render } from "/core/UI.js"
import { notify } from "/core/Utils/browser.js"
import logic from "./logic.js"

class $QR extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        this.events = new Events(this)
        this.render = this.render.bind(this)
        this.sync = this.sync.bind(this)
        this.ready = this.ready.bind(this)
        this.capture = this.capture.bind(this)
        this.resume = this.resume.bind(this)
        this.inspect = this.inspect.bind(this)
        this.ensure = this.ensure.bind(this)
        this.start = this.start.bind(this)
        this.stop = this.stop.bind(this)
        this.clear = this.clear.bind(this)
        this.go = this.go.bind(this)
        this.scan = this.scan.bind(this)
        this.scanning = false
        this.pending = false
        this.timer = null
        render(template, this.shadowRoot)
    }

    static get observedAttributes() {
        return ["data-value", "data-mode", "data-controls"]
    }

    attributeChangedCallback(name, last, value) {
        if (last === value) return
        if (name === "data-mode") return this.isConnected && this.sync()
        if (name === "data-value" && value && last !== value) this.render()
        if (name === "data-controls") this.shadowRoot.querySelector("#controls").style.display = value !== "false" ? "flex" : "none"
    }

    connectedCallback() {
        this.sync()
    }

    disconnectedCallback() {
        this.detach()
    }

    get mode() {
        return this.dataset.mode === "scan" ? "scan" : "encode"
    }

    sync() {
        this.$qr = this.$qr || this.shadowRoot.querySelector("#qr")
        this.$scanner = this.$scanner || this.shadowRoot.querySelector("#scanner")
        this.$qr.hidden = this.mode !== "encode"
        this.$scanner.hidden = this.mode !== "scan"
        if (this.mode === "scan") this.attach()
        else this.detach()
    }

    attach() {
        if (this._scannerAttached) return
        this._scannerAttached = true
        this.camera = this.shadowRoot.querySelector("#camera")
        this.result = this.shadowRoot.querySelector("#result")
        this.$start = this.shadowRoot.querySelector("#start")
        this.$stop = this.shadowRoot.querySelector("#stop")
        this.$clear = this.shadowRoot.querySelector("#clear")
        this.$stop.hidden = true
        this.$clear.hidden = true
        this.$start.addEventListener("click", this.start)
        this.$stop.addEventListener("click", this.stop)
        this.$clear.addEventListener("click", this.clear)
        this.$cameraReady = this.camera.events.on("ready", this.ready)
        this.$cameraCapture = this.camera.events.on("capture", this.capture)
        this.$cameraResume = this.camera.events.on("resume", this.resume)
        this.inspect().then((state) => {
            if (!state.supported) this.camera.dataset.status = "dictionary.cameraUnavailable"
            else if (!state.hasCamera) this.camera.dataset.status = "dictionary.noCameraDetected"
            else if (state.permission === "denied") this.camera.dataset.status = "dictionary.cameraPermissionDenied"
            else if (state.permission === "granted") this.camera.dataset.status = "dictionary.cameraReady"
            else this.camera.dataset.status = "dictionary.cameraPermissionRequired"
        })
    }

    detach() {
        this.stop()
        if (!this._scannerAttached) return
        this._scannerAttached = false
        this.$start?.removeEventListener("click", this.start)
        this.$stop?.removeEventListener("click", this.stop)
        this.$clear?.removeEventListener("click", this.clear)
        this.$cameraReady?.()
        this.$cameraCapture?.()
        this.$cameraResume?.()
        this.$cameraReady = null
        this.$cameraCapture = null
        this.$cameraResume = null
        this.camera?.stop?.()
    }

    ready() {
        this.camera.dataset.status = "dictionary.cameraReadyScanning"
        this.go()
    }

    capture() {
        this.camera.dataset.status = "dictionary.capturedFrameLocked"
        if (this.scanning) this.scan(true)
    }

    resume() {
        this.camera.dataset.status = "dictionary.liveCameraResumed"
        if (this.scanning) this.schedule(0)
    }

    async inspect() {
        const state = {
            supported: !!navigator.mediaDevices?.getUserMedia,
            hasCamera: false,
            permission: "prompt"
        }

        if (!state.supported) return state

        try {
            if (navigator.mediaDevices?.enumerateDevices) {
                const devices = await navigator.mediaDevices.enumerateDevices()
                state.hasCamera = devices.some((device) => device.kind === "videoinput")
            }
        } catch {}

        try {
            if (navigator.permissions?.query) {
                const permission = await navigator.permissions.query({ name: "camera" })
                state.permission = permission?.state || state.permission
            }
        } catch {}

        return state
    }

    async ensure() {
        if (this.camera?.stream) return true
        const state = await this.inspect()
        if (!state.supported) {
            this.camera.dataset.status = "dictionary.cameraUnavailable"
            return false
        }
        if (!state.hasCamera) {
            this.camera.dataset.status = "dictionary.noCameraDetected"
            return false
        }
        if (state.permission === "denied") {
            this.camera.dataset.status = "dictionary.cameraPermissionDenied"
            return false
        }
        try {
            await this.camera?.start?.()
            return true
        } catch (error) {
            if (error?.message) notify({ content: error.message })
            this.camera.dataset.status = "dictionary.cameraAccessFailed"
            return false
        }
    }

    async start() {
        if (this.mode !== "scan" || this.scanning) return
        const ready = await this.ensure()
        // ready fires during camera.start() and calls go()
        // fallback: if camera was already streaming, ready won't fire
        if (ready && !this.scanning) this.go()
    }

    go() {
        if (this.scanning) return
        this.scanning = true
        this.$start.hidden = true
        this.$stop.hidden = false
        this.camera.dataset.status = this.camera?.captured ? "dictionary.scanningCapturedFrame" : "dictionary.scanningLiveFrames"
        logic.configure(QR).catch((error) => { if (error?.message) notify({ content: error.message }) })
        this.schedule(0)
    }

    stop() {
        this.scanning = false
        this.pending = false
        if (this.timer) clearTimeout(this.timer)
        this.timer = null
        if (this.mode === "scan") {
            if (this.$start) this.$start.hidden = false
            if (this.$stop) this.$stop.hidden = true
            this.camera.dataset.status = "dictionary.scanningStopped"
        }
    }

    clear() {
        if (!this.result) return
        this.result.textContent = ""
        if (this.$clear) this.$clear.hidden = true
        this.camera.dataset.status = this.camera?.captured ? "dictionary.capturedFrameReady" : "dictionary.resultCleared"
    }

    schedule(delay = 180) {
        if (!this.scanning) return
        if (this.timer) clearTimeout(this.timer)
        this.timer = setTimeout(this.scan, delay)
    }

    async scan(force = false) {
        if ((!this.scanning && !force) || this.pending || this.mode !== "scan") return
        const frame = await this.camera?.frame?.()
        if (!frame) {
            this.schedule(250)
            return
        }

        this.pending = true
        logic.decode(frame, QR)
            .then((response) => {
                this.pending = false
                if (response?.ok && response?.data) {
                    if (this.result) {
                        this.result.textContent = response.data
                        if (this.$clear) this.$clear.hidden = false
                    }
                    this.camera.dataset.status = frame.captured ? "dictionary.decodedFromCapturedFrame" : "dictionary.decodedFromLiveFrame"
                    this.events.emit("scan", response, { bubbles: true, composed: true })
                    this.stop()
                    return
                }

                if (response?.error) notify({ content: response.error })
                else this.camera.dataset.status = frame.captured ? "dictionary.noQrFoundInCapturedFrameYet" : "dictionary.scanningLiveFrames"

                if (this.scanning) this.schedule(frame.captured ? 500 : 180)
            })
            .catch((error) => {
                this.pending = false
                if (error?.message) notify({ content: error.message })
                if (this.scanning) this.schedule(400)
            })
    }

    render() {
        if (this.mode !== "encode") return
        const data = this.dataset.value
        const qr = this.$qr || this.shadowRoot.querySelector("#qr")
        if (!data) {
            qr.innerHTML = ""
            return
        }
        const code = logic.render(data, QR)
        render(html`${code}`, qr)
    }
}

customElements.define("ui-qr", $QR)

export { $QR as QR }

export default $QR