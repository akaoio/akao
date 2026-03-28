import QR from "/core/QR.js"
import Events from "/core/Events.js"
import template from "./template.js"
import { html, render } from "/core/UI.js"

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
            if (!state.supported) this.status(null, "Camera is not supported in this browser")
            else if (!state.hasCamera) this.status(null, "No camera detected on this device")
            else if (state.permission === "denied") this.status(null, "Camera permission is denied. Please allow camera access in browser settings")
            else if (state.permission === "granted") this.status("dictionary.cameraReady")
            else this.status(null, "Press Start to scan and request camera permission")
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
        console.log("Camera is ready")
        this.status("dictionary.cameraReadyScanning")
        this.go()
    }

    capture() {
        this.status("dictionary.capturedFrameLocked")
        if (this.scanning) this.scan(true)
    }

    resume() {
        this.status("dictionary.liveCameraResumed")
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
            this.status(null, "Camera is not supported in this browser")
            return false
        }
        if (!state.hasCamera) {
            this.status(null, "No camera detected on this device")
            return false
        }
        if (state.permission === "denied") {
            this.status(null, "Camera permission is denied. Please allow camera access in browser settings")
            return false
        }
        try {
            await this.camera?.start?.()
            return true
        } catch (error) {
            this.status(null, error?.message || "Unable to access camera")
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
        this.status(this.camera?.captured ? "dictionary.scanningCapturedFrame" : "dictionary.scanningLiveFrames")
        QR.request({method: "configure", params: { alsoTryWithoutScanRegion: true }}).catch((error) => this.status(null, error.message || "Error"))
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
            this.status("dictionary.scanningStopped")
        }
    }

    clear() {
        if (!this.result) return
        this.result.textContent = ""
        if (this.$clear) this.$clear.hidden = true
        this.status(this.camera?.captured ? "dictionary.capturedFrameReady" : "dictionary.resultCleared")
    }

    status(key, fallback) {
        if (!this.camera) return
        if (key) this.camera.dataset.status = key
        else if (fallback) {
            this.camera.removeAttribute("data-status")
            this.camera.querySelector("#status").textContent = fallback
        }
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
        const transfer = frame.bitmap ? [frame.bitmap] : undefined
        QR.decode(frame, { transfer })
            .then((response) => {
                this.pending = false
                if (response?.ok && response?.data) {
                    if (this.result) {
                        this.result.textContent = response.data
                        if (this.$clear) this.$clear.hidden = false
                    }
                    this.status(frame.captured ? "dictionary.decodedFromCapturedFrame" : "dictionary.decodedFromLiveFrame")
                    this.events.emit("scan", response, { bubbles: true, composed: true })
                    this.stop()
                    return
                }

                if (response?.error) this.status(null, response.error)
                else this.status(frame.captured ? "dictionary.noQrFoundInCapturedFrameYet" : "dictionary.scanningLiveFrames")

                if (this.scanning) this.schedule(frame.captured ? 500 : 180)
            })
            .catch((error) => {
                this.pending = false
                this.status(null, error.message || "Error")
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
        const code = QR.render(data, { border: 0 })
        render(html`${code}`, qr)
    }
}

customElements.define("ui-qr", $QR)

export { $QR as QR }

export default $QR