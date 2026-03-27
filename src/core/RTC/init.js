import { BROWSER } from "../Utils/environment.js"

function bind(channel) {
    if (!channel) return
    this.data = channel
    this.data.binaryType = "arraybuffer"
    this.data.onopen = () => this.$emit("open", this.snapshot())
    this.data.onclose = () => this.$emit("close", this.snapshot())
    this.data.onerror = (error) => this.$emit("error", error)
    this.data.onmessage = (event) => this.$emit("message", event.data)
}

function json(candidate) {
    if (!candidate) return null
    return typeof candidate.toJSON === "function" ? candidate.toJSON() : candidate
}

export async function init({ initiator = false } = {}) {
    if (this.connection) return this.connection
    if (!BROWSER || typeof RTCPeerConnection === "undefined")
        throw new Error("RTC is only available in browsers with RTCPeerConnection support")

    this.candidates = []
    this.connection = new RTCPeerConnection(this.config)

    this.connection.onicecandidate = (event) => {
        const candidate = json(event.candidate)
        if (candidate) {
            this.candidates.push(candidate)
            this.$emit("candidate", candidate)
            return
        }
        this.$emit("candidates", [...this.candidates])
    }

    this.connection.ondatachannel = (event) => {
        bind.call(this, event.channel)
        this.$emit("datachannel", this.snapshot())
    }

    this.connection.onconnectionstatechange = () => this.$emit("connection", this.snapshot())
    this.connection.oniceconnectionstatechange = () => this.$emit("iceconnection", this.snapshot())
    this.connection.onicegatheringstatechange = () => this.$emit("icegathering", this.snapshot())
    this.connection.onsignalingstatechange = () => this.$emit("signaling", this.snapshot())

    if (initiator && this.channel && !this.data) bind.call(this, this.connection.createDataChannel(this.channel, this.dataopts))

    return this.connection
}