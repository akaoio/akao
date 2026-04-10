import { render } from "/core/UI.js"
import Events from "/core/Events.js"
import template from "./template.js"
import Component from "/core/UI/Component.js"

export class CAMERA extends Component {
    static module = import.meta.url
    constructor() {
        super()
        this.template = template // Store for HMR
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.events = new Events(this)
        this.stream = null
        this.devices = []
        this.$id = null
        this.facing = this.dataset.facing || "environment"
        this.captured = false
        this.canvas = document.createElement("canvas")
        this.start = this.start.bind(this)
        this.stop = this.stop.bind(this)
        this.switch = this.switch.bind(this)
        this.capture = this.capture.bind(this)
        this.resume = this.resume.bind(this)
    }

    static get observedAttributes() {
        return ["data-facing", "data-controls", "data-status"]
    }

    attributeChangedCallback(name, last, value) {
        if (last === value) return
        if (name === "data-facing" && value) {
            this.facing = value
            if (this.isConnected) this.start({ facing: value })
        }
        if (name === "data-controls") this.shadowRoot.querySelector(".controls").style.display = value !== "false" ? "flex" : "none"
        if (name === "data-status") this.status.dataset.key = value
    }

    onconnect() {
        this.video = this.shadowRoot.querySelector("#video")
        this.$switch = this.shadowRoot.querySelector("#switch")
        this.$capture = this.shadowRoot.querySelector("#capture")
        this.$resume = this.shadowRoot.querySelector("#resume")
        this.status = this.shadowRoot.querySelector("ui-context#status")
        this.listen(this.$switch, "click", this.switch)
        this.listen(this.$capture, "click", this.capture)
        this.listen(this.$resume, "click", this.resume)
        this.sub(this.stop.bind(this))
        if (this.dataset.autostart !== "false") this.start()
    }

    ondisconnect() {
        this.stop()
    }

    async list() {
        if (!navigator.mediaDevices?.enumerateDevices) return []
        const devices = await navigator.mediaDevices.enumerateDevices()
        this.devices = devices.filter((device) => device.kind === "videoinput")
        return this.devices
    }

    async start({ id, facing } = {}) {
        if (!navigator.mediaDevices?.getUserMedia) {
            const error = new Error("Camera is not supported in this browser")
            this.events.emit("error", error, { bubbles: true, composed: true })
            this.status.dataset.key = "dictionary.cameraUnavailable"
            return null
        }

        this.stop()

        try {
            const video = id ? { deviceId: { exact: id } } : { facingMode: { ideal: facing || this.facing || "environment" } }
            const stream = await navigator.mediaDevices.getUserMedia({ video, audio: false })
            this.stream = stream
            this.video.srcObject = stream
            await this.video.play()
            this.captured = false
            const track = stream.getVideoTracks()[0]
            const settings = track?.getSettings?.() || {}
            this.$id = settings.deviceId || id || this.$id
            this.facing = settings.facingMode || facing || this.facing
            await this.list()
            this.status.dataset.key = track?.label ? null : "dictionary.cameraReady"
            if (track?.label && this.status) this.status.innerText = track.label
            this.$resume.hidden = true
            this.$capture.hidden = false
            this.events.emit("ready", { id: this.$id, facing: this.facing, devices: this.devices }, { bubbles: true, composed: true })
            return stream
        } catch (error) {
            this.events.emit("error", error, { bubbles: true, composed: true })
            this.status.dataset.key = "dictionary.cameraAccessFailed"
            return null
        }
    }

    stop() {
        if (this.video) this.video.pause()
        if (this.stream) this.stream.getTracks().forEach((track) => track.stop())
        this.stream = null
    }

    async switch() {
        await this.list()
        if (this.devices.length > 1 && this.$id) {
            const index = this.devices.findIndex((device) => device.deviceId === this.$id)
            const next = this.devices[(index + 1 + this.devices.length) % this.devices.length]
            if (next?.deviceId) return this.start({ id: next.deviceId })
        }
        return this.start({ facing: this.facing === "user" ? "environment" : "user" })
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

    async frame() {
        if (typeof createImageBitmap === "function") {
            if (this.captured && this.canvas.width && this.canvas.height) 
                return { bitmap: await createImageBitmap(this.canvas), captured: true, width: this.canvas.width, height: this.canvas.height }
            
            if (this.video?.videoWidth && this.video?.videoHeight) 
                return { bitmap: await createImageBitmap(this.video), captured: false, width: this.video.videoWidth, height: this.video.videoHeight }
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