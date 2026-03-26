import { render } from "/core/UI.js"
import Events from "/core/Events.js"
import template from "./template.js"

export class CAMERA extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.events = new Events(this)
        this.stream = null
        this.devices = []
        this.currentDeviceId = null
        this.facingMode = this.dataset.facingMode || "environment"
        this.captured = false
        this.canvas = document.createElement("canvas")
        this.start = this.start.bind(this)
        this.stop = this.stop.bind(this)
        this.switch = this.switch.bind(this)
        this.capture = this.capture.bind(this)
        this.resume = this.resume.bind(this)
        this.subscriptions = []
    }

    static get observedAttributes() {
        return ["data-facing-mode", "data-controls", "data-status"]
    }

    attributeChangedCallback(name, last, value) {
        if (last === value) return
        if (name === "data-facing-mode" && value) {
            this.facingMode = value
            if (this.isConnected) this.start({ facingMode: value })
        }
        if (name === "data-controls") this.shadowRoot.querySelector(".controls").style.display = value !== "false" ? "flex" : "none"
        if (name === "data-status") this.status.dataset.key = value
    }

    connectedCallback() {
        this.video = this.shadowRoot.querySelector("#video")
        this.$switch = this.shadowRoot.querySelector("#switch")
        this.$capture = this.shadowRoot.querySelector("#capture")
        this.$resume = this.shadowRoot.querySelector("#resume")
        this.status = this.shadowRoot.querySelector("ui-context#status")
        this.$switch.addEventListener("click", this.switch)
        this.$capture.addEventListener("click", this.capture)
        this.$resume.addEventListener("click", this.resume)
        this.subscriptions.push(
            () => this.$switch.removeEventListener("click", this.switch),
            () => this.$capture.removeEventListener("click", this.capture),
            () => this.$resume.removeEventListener("click", this.resume)
        )
        if (this.dataset.autostart !== "false") this.start()
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
        this.subscriptions = []
        this.stop()
    }

    async listCameras() {
        if (!navigator.mediaDevices?.enumerateDevices) return []
        const devices = await navigator.mediaDevices.enumerateDevices()
        this.devices = devices.filter((device) => device.kind === "videoinput")
        return this.devices
    }

    async start({ deviceId, facingMode } = {}) {
        if (!navigator.mediaDevices?.getUserMedia) {
            const error = new Error("Camera is not supported in this browser")
            this.events.emit("error", error, { bubbles: true, composed: true })
            this.status.dataset.key = "dictionary.cameraUnavailable"
            return null
        }

        this.stop()

        const video = deviceId ? { deviceId: { exact: deviceId } } : { facingMode: { ideal: facingMode || this.facingMode || "environment" } }
        const stream = await navigator.mediaDevices.getUserMedia({ video, audio: false })
        this.stream = stream
        this.video.srcObject = stream
        await this.video.play()
        this.captured = false
        const track = stream.getVideoTracks()[0]
        const settings = track?.getSettings?.() || {}
        this.currentDeviceId = settings.deviceId || deviceId || this.currentDeviceId
        this.facingMode = settings.facingMode || facingMode || this.facingMode
        await this.listCameras()
        this.status.dataset.key = track?.label ? null : "dictionary.cameraReady"
        if (track?.label && this.status) this.status.innerText = track.label
        this.$resume.hidden = true
        this.$capture.hidden = false
        this.events.emit("ready", { deviceId: this.currentDeviceId, facingMode: this.facingMode, devices: this.devices }, { bubbles: true, composed: true })
        return stream
    }

    stop() {
        if (this.video) this.video.pause()
        if (this.stream) this.stream.getTracks().forEach((track) => track.stop())
        this.stream = null
    }

    async switch() {
        await this.listCameras()
        if (this.devices.length > 1 && this.currentDeviceId) {
            const index = this.devices.findIndex((device) => device.deviceId === this.currentDeviceId)
            const next = this.devices[(index + 1 + this.devices.length) % this.devices.length]
            if (next?.deviceId) return this.start({ deviceId: next.deviceId })
        }
        const nextFacingMode = this.facingMode === "user" ? "environment" : "user"
        return this.start({ facingMode: nextFacingMode })
    }

    capture() {
        if (!this.video?.videoWidth || !this.video?.videoHeight) return null
        this.canvas.width = this.video.videoWidth
        this.canvas.height = this.video.videoHeight
        const context = this.canvas.getContext("2d", { willReadFrequently: true })
        context.drawImage(this.video, 0, 0, this.canvas.width, this.canvas.height)
        this.captured = true
        this.video.pause()
        this.$resume.hidden = false
        this.$capture.hidden = true
        this.status.dataset.key = "dictionary.capturedFrame"
        const detail = { width: this.canvas.width, height: this.canvas.height, captured: true }
        this.events.emit("capture", detail, { bubbles: true, composed: true })
        return detail
    }

    async resume() {
        if (!this.stream) return this.start()
        this.captured = false
        await this.video.play()
        this.$resume.hidden = true
        this.$capture.hidden = false
        this.status.dataset.key = "dictionary.liveCamera"
        this.events.emit("resume", undefined, { bubbles: true, composed: true })
    }

    isCaptured() {
        return this.captured
    }

    async getFrame() {
        if (typeof createImageBitmap === "function") {
            if (this.captured && this.canvas.width && this.canvas.height) {
                return { bitmap: await createImageBitmap(this.canvas), captured: true, width: this.canvas.width, height: this.canvas.height }
            }
            if (this.video?.videoWidth && this.video?.videoHeight) {
                return { bitmap: await createImageBitmap(this.video), captured: false, width: this.video.videoWidth, height: this.video.videoHeight }
            }
        }

        if (!this.video?.videoWidth || !this.video?.videoHeight) return null
        this.canvas.width = this.video.videoWidth
        this.canvas.height = this.video.videoHeight
        const context = this.canvas.getContext("2d", { willReadFrequently: true })
        context.drawImage(this.captured && this.canvas.width ? this.canvas : this.video, 0, 0, this.canvas.width, this.canvas.height)
        return { imageData: context.getImageData(0, 0, this.canvas.width, this.canvas.height), captured: this.captured, width: this.canvas.width, height: this.canvas.height }
    }
}

customElements.define("ui-camera", CAMERA)

export default CAMERA