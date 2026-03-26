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
        this.syncMode = this.syncMode.bind(this)
        this.onCameraReady = this.onCameraReady.bind(this)
        this.onCameraCapture = this.onCameraCapture.bind(this)
        this.onCameraResume = this.onCameraResume.bind(this)
        this.inspectCamera = this.inspectCamera.bind(this)
        this.ensureCameraReady = this.ensureCameraReady.bind(this)
        this.start = this.start.bind(this)
        this.stop = this.stop.bind(this)
        this.clear = this.clear.bind(this)
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
        if (name === "data-mode") return this.syncMode()
        if (name === "data-value" && value && last !== value) this.render()
        if (name === "data-controls") this.shadowRoot.querySelector("#controls").style.display = value !== "false" ? "flex" : "none"
    }

    connectedCallback() {
        this.syncMode()
    }

    disconnectedCallback() {
        this.detachScanner()
    }

    get mode() {
        return this.dataset.mode === "scan" ? "scan" : "encode"
    }

    syncMode() {
        this.$qr = this.$qr || this.shadowRoot.querySelector("#qr")
        this.$scanner = this.$scanner || this.shadowRoot.querySelector("#scanner")
        this.$qr.hidden = this.mode !== "encode"
        this.$scanner.hidden = this.mode !== "scan"
        if (this.mode === "scan") this.attachScanner()
        else this.detachScanner()
    }

    attachScanner() {
        if (this._scannerAttached) return
        this._scannerAttached = true
        this.camera = this.shadowRoot.querySelector("#camera")
        this.result = this.shadowRoot.querySelector("#result")
        this.$start = this.shadowRoot.querySelector("#start")
        this.$stop = this.shadowRoot.querySelector("#stop")
        this.$clear = this.shadowRoot.querySelector("#clear")
        this.$start.addEventListener("click", this.start)
        this.$stop.addEventListener("click", this.stop)
        this.$clear.addEventListener("click", this.clear)
        this.$cameraReady = this.camera?.events?.on?.("ready", this.onCameraReady)
        this.$cameraCapture = this.camera?.events?.on?.("capture", this.onCameraCapture)
        this.$cameraResume = this.camera?.events?.on?.("resume", this.onCameraResume)
        this.inspectCamera().then((state) => {
            if (!state.supported) this.setStatus(null, "Camera is not supported in this browser")
            else if (!state.hasCamera) this.setStatus(null, "No camera detected on this device")
            else if (state.permission === "denied") this.setStatus(null, "Camera permission is denied. Please allow camera access in browser settings")
            else if (state.permission === "granted") this.setStatus("dictionary.cameraReady")
            else this.setStatus(null, "Press Start to scan and request camera permission")
        })
    }

    detachScanner() {
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

    onCameraReady() {
        this.setStatus("dictionary.cameraReadyScanning")
        this.start()
    }

    onCameraCapture() {
        this.setStatus("dictionary.capturedFrameLocked")
        if (this.scanning) this.scan(true)
    }

    onCameraResume() {
        this.setStatus("dictionary.liveCameraResumed")
        if (this.scanning) this.schedule(0)
    }

    async inspectCamera() {
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

    async ensureCameraReady() {
        if (this.camera?.stream) return true
        const state = await this.inspectCamera()
        if (!state.supported) {
            this.setStatus(null, "Camera is not supported in this browser")
            return false
        }
        if (!state.hasCamera) {
            this.setStatus(null, "No camera detected on this device")
            return false
        }
        if (state.permission === "denied") {
            this.setStatus(null, "Camera permission is denied. Please allow camera access in browser settings")
            return false
        }
        try {
            await this.camera?.start?.()
            return true
        } catch (error) {
            this.setStatus(null, error?.message || "Unable to access camera")
            return false
        }
    }

    async start() {
        if (this.mode !== "scan" || this.scanning) return
        const ready = await this.ensureCameraReady()
        if (!ready) return
        this.scanning = true
        this.setStatus(this.camera?.isCaptured() ? "dictionary.scanningCapturedFrame" : "dictionary.scanningLiveFrames")
        QR.request({method: "configure", params: { alsoTryWithoutScanRegion: true }}).catch((error) => this.setStatus(null, error.message || "Error"))
        this.schedule(0)
    }

    stop() {
        this.scanning = false
        this.pending = false
        if (this.timer) clearTimeout(this.timer)
        this.timer = null
        if (this.mode === "scan") this.setStatus("dictionary.scanningStopped")
    }

    clear() {
        if (!this.result) return
        this.result.textContent = ""
        this.setStatus(this.camera?.isCaptured() ? "dictionary.capturedFrameReady" : "dictionary.resultCleared")
    }

    setStatus(key, fallback) {
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
        const frame = await this.camera?.getFrame?.()
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
                    if (this.result) this.result.textContent = response.data
                    this.setStatus(frame.captured ? "dictionary.decodedFromCapturedFrame" : "dictionary.decodedFromLiveFrame")
                    this.events.emit("scan", response, { bubbles: true, composed: true })
                    this.stop()
                    return
                }

                if (response?.error) this.setStatus(null, response.error)
                else this.setStatus(frame.captured ? "dictionary.noQrFoundInCapturedFrameYet" : "dictionary.scanningLiveFrames")

                if (this.scanning) this.schedule(frame.captured ? 500 : 180)
            })
            .catch((error) => {
                this.pending = false
                this.setStatus(null, error.message || "Error")
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