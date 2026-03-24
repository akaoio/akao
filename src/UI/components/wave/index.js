import { render } from "/core/UI.js"
import template from "./template.js"
import { Wave } from "/core/Wave.js"

export class WAVE extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.role = null
        this.session = null
        this.responseBuilder = null
        this.audioContext = null
        this.stream = null
        this.source = null
        this.processor = null
        this.sink = null
        this.micTemporarilyDisabled = false
        this.pendingDecode = false
        this.audioBacklog = []
        this.audioBacklogBytes = 0
        this.maxWavePayloadSize = 140
        this.chunkTtlMs = 60000
        this.chunks = new Map()
        this.running = false
        this.sending = false
        this.lastIncoming = ""
        this.onScan = this.onScan.bind(this)
        this.onAudio = this.onAudio.bind(this)
        this.onSend = this.onSend.bind(this)
        this.onListen = this.onListen.bind(this)
        this.onStop = this.onStop.bind(this)
        this.startSignin = this.startSignin.bind(this)
        this.startShare = this.startShare.bind(this)
        this.stop = this.stop.bind(this)
        this.listen = this.listen.bind(this)
        this.send = this.send.bind(this)
        this.play = this.play.bind(this)
    }

    connectedCallback() {
        this.input = this.shadowRoot.querySelector("#input")
        this.incoming = this.shadowRoot.querySelector("#incoming")
        this.status = this.shadowRoot.querySelector("#status")
        this.outgoingQR = this.shadowRoot.querySelector("#outgoing")
        this.incomingQR = this.shadowRoot.querySelector("#incoming-qr")
        this.$send = this.shadowRoot.querySelector("#send")
        this.$listen = this.shadowRoot.querySelector("#listen")
        this.$stop = this.shadowRoot.querySelector("#stop")
        this.$send?.addEventListener("click", this.onSend)
        this.$listen?.addEventListener("click", this.onListen)
        this.$stop?.addEventListener("click", this.onStop)
        this.incomingQR?.addEventListener("scan", this.onScan)
    }

    disconnectedCallback() {
        this.$send?.removeEventListener("click", this.onSend)
        this.$listen?.removeEventListener("click", this.onListen)
        this.$stop?.removeEventListener("click", this.onStop)
        this.incomingQR?.removeEventListener("scan", this.onScan)
        this.stop()
    }

    setStatus(text) {
        if (this.status) this.status.textContent = text
    }

    appendIncoming(text) {
        if (!this.incoming || !text) return
        const line = `${new Date().toISOString()}  ${text}`
        this.incoming.value = this.incoming.value ? `${this.incoming.value}\n${line}` : line
    }

    toAudioBytes(floatSamples) {
        const copy = new Float32Array(floatSamples)
        return new Int8Array(copy.buffer)
    }

    ensureAudioContext() {
        const AudioEngine = globalThis.AudioContext || globalThis.webkitAudioContext
        if (!AudioEngine) throw new Error("Web Audio API is not supported in this browser")
        if (!this.audioContext) this.audioContext = new AudioEngine({ sampleRate: 48000 })
        return this.audioContext
    }

    async prepareWorker() {
        const context = this.ensureAudioContext()
        await Wave.setup({
            sampleRate: context.sampleRate,
            sampleRateInp: context.sampleRate,
            sampleRateOut: context.sampleRate,
            samplesPerFrame: 1024,
            volume: 25
        })
        return context
    }

    async listen() {
        if (this.running) return true
        if (!navigator?.mediaDevices?.getUserMedia) {
            this.setStatus("Microphone is not supported in this browser")
            return false
        }

        const context = this.ensureAudioContext()
        if (context.state === "suspended") await context.resume()

        const constraints = {
            audio: {
                echoCancellation: false,
                noiseSuppression: false,
                autoGainControl: false,
                channelCount: 1,
                sampleRate: context.sampleRate
            }
        }
        try {
            this.stream = await navigator.mediaDevices.getUserMedia(constraints)
        } catch {
            this.stream = await navigator.mediaDevices.getUserMedia({ audio: true })
        }

        const track = this.stream.getAudioTracks?.()[0]
        const settings = track?.getSettings ? track.getSettings() : {}
        const inputSampleRate = context.sampleRate || 48000

        await Wave.setup({
            sampleRate: inputSampleRate,
            sampleRateInp: inputSampleRate,
            sampleRateOut: inputSampleRate,
            samplesPerFrame: 1024,
            volume: 25,
            reset: true
        })

        this.source = context.createMediaStreamSource(this.stream)
        this.processor = context.createScriptProcessor(1024, 1, 1)
        this.sink = context.createGain()
        this.sink.gain.value = 0
        this.processor.onaudioprocess = this.onAudio
        this.source.connect(this.processor)
        this.processor.connect(this.sink)
        this.sink.connect(context.destination)
        this.running = true
        this.setStatus(`Listening on wave + scanning QR (context ${context.sampleRate}Hz, track ${settings?.sampleRate || "unknown"}Hz, ${settings?.channelCount || 1}ch)`)
        return true
    }

    async stop() {
        this.running = false
        this.pendingDecode = false
        this.sending = false
        this.audioBacklog = []
        this.audioBacklogBytes = 0
        if (this.processor) {
            this.processor.onaudioprocess = null
            this.processor.disconnect()
        }
        if (this.source) this.source.disconnect()
        if (this.sink) this.sink.disconnect()
        if (this.stream) this.stream.getTracks().forEach((track) => track.stop())
        this.processor = null
        this.source = null
        this.sink = null
        this.stream = null
        this.role = null
        this.responseBuilder = null
        this.session = null
        this.chunks.clear()
        this.setStatus("Stopped")
    }

    sleep(ms = 0) {
        return new Promise((resolve) => setTimeout(resolve, ms))
    }

    setMicEnabled(enabled) {
        const tracks = this.stream?.getAudioTracks?.()
        if (!Array.isArray(tracks) || !tracks.length) return
        tracks.forEach((track) => {
            track.enabled = Boolean(enabled)
        })
    }

    createMessageId() {
        return `${Date.now().toString(36)}-${Math.random().toString(36).slice(2, 10)}`
    }

    cleanupChunks() {
        const now = Date.now()
        for (const [id, entry] of this.chunks.entries()) {
            if (!entry?.updatedAt || now - entry.updatedAt > this.chunkTtlMs) this.chunks.delete(id)
        }
    }

    async sendWaveFrame(text) {
        const response = await Wave.encode({ message: text })
        if (!response?.ok || !response?.bytes) throw new Error(response?.error || "Unable to encode payload")
        this.setStatus("Broadcasting via wave")
        await this.play(response.bytes, response.sampleRate)
        const sampleRate = response.sampleRate || 48000
        const durationMs = Math.ceil((response.bytes.length / 2 / sampleRate) * 1000)
        await this.sleep(Math.min(240, Math.max(90, durationMs + 50)))
        return response
    }

    async sendChunked(text) {
        const total = Math.ceil(text.length / this.maxWavePayloadSize)
        const id = this.createMessageId()
        for (let index = 0; index < total; index++) {
            const from = index * this.maxWavePayloadSize
            const part = text.slice(from, from + this.maxWavePayloadSize)
            const envelope = JSON.stringify({ __waveChunk: 1, id, index, total, part })
            await this.sendWaveFrame(envelope)
            this.setStatus(`Broadcasting chunk ${index + 1}/${total}`)
        }
    }

    async consumeChunk(message = {}, channel = "wave") {
        this.cleanupChunks()
        const id = message?.id
        const index = Number(message?.index)
        const total = Number(message?.total)
        const part = message?.part
        if (!id || !Number.isInteger(index) || !Number.isInteger(total) || total <= 0 || index < 0 || index >= total || typeof part !== "string") return

        const current = this.chunks.get(id) || { total, parts: new Array(total).fill(null), updatedAt: Date.now() }
        if (!Array.isArray(current.parts) || current.parts.length !== total) {
            current.parts = new Array(total).fill(null)
            current.total = total
        }

        current.parts[index] = part
        current.updatedAt = Date.now()
        this.chunks.set(id, current)

        if (current.parts.some((value) => typeof value !== "string")) return

        this.chunks.delete(id)
        const payload = current.parts.join("")
        this.appendIncoming(`[${channel}] [reassembled ${total} chunks]`)
        await this.handleIncoming(payload, channel)
    }

    dequeueAudioBytes(maxChunks = 6) {
        if (!this.audioBacklog?.length) return null
        const chunks = []
        let bytes = 0
        for (let i = 0; i < maxChunks && this.audioBacklog.length; i++) {
            const chunk = this.audioBacklog.shift()
            if (!chunk?.length) continue
            chunks.push(chunk)
            bytes += chunk.byteLength
            this.audioBacklogBytes -= chunk.byteLength
        }
        if (!chunks.length || !bytes) return null
        if (chunks.length === 1) return chunks[0]
        const merged = new Int8Array(bytes)
        let offset = 0
        for (const chunk of chunks) {
            merged.set(chunk, offset)
            offset += chunk.byteLength
        }
        return merged
    }

    async pumpDecode() {
        if (!this.running || this.pendingDecode || this.sending) return
        this.pendingDecode = true
        try {
            while (this.running && !this.sending) {
                const bytes = this.dequeueAudioBytes(8)
                if (!bytes?.length) break
                const response = await Wave.decode({ bytes }, { transfer: [bytes.buffer] })
                if (response?.found && response?.message) await this.handleIncoming(response.message, "wave")
            }
        } catch (error) {
            this.setStatus(error?.message || String(error))
        } finally {
            this.pendingDecode = false
            if (this.running && !this.sending && this.audioBacklogBytes > 0) this.pumpDecode()
        }
    }

    async onAudio(event) {
        if (!this.running) return
        const input = event?.inputBuffer?.getChannelData?.(0)
        if (!input?.length) return
        const bytes = this.toAudioBytes(input)
        this.audioBacklog.push(bytes)
        this.audioBacklogBytes += bytes.byteLength
        if (this.audioBacklog.length > 64) {
            const removed = this.audioBacklog.shift()
            if (removed?.byteLength) this.audioBacklogBytes -= removed.byteLength
        }
        this.pumpDecode()
    }

    async play(bytes, sampleRate = 48000) {
        const context = this.ensureAudioContext()
        if (context.state === "suspended") await context.resume()
        const pcm = bytes instanceof Int8Array ? bytes : new Int8Array(bytes)
        if (!pcm.length) return false
        const samples = Math.floor(pcm.length / 2)
        const audio = context.createBuffer(1, samples, sampleRate)
        const channel = audio.getChannelData(0)
        const view = new DataView(pcm.buffer, pcm.byteOffset, pcm.byteLength)
        for (let i = 0; i < samples; i++) channel[i] = view.getInt16(i * 2, true) / 32768
        const source = context.createBufferSource()
        const gain = context.createGain()
        gain.gain.value = 1.0
        source.buffer = audio
        source.connect(gain)
        gain.connect(context.destination)
        this.micTemporarilyDisabled = true
        this.setMicEnabled(false)
        source.start()
        await new Promise((resolve) => {
            source.onended = resolve
        })
        this.setMicEnabled(true)
        this.micTemporarilyDisabled = false
        this.audioBacklog = []
        this.audioBacklogBytes = 0
        return true
    }

    async send(payload) {
        await this.prepareWorker()
        const text = typeof payload === "string" ? payload : JSON.stringify(payload)
        if (!text) return null
        if (this.outgoingQR) this.outgoingQR.dataset.value = text
        if (this.input && this.input.value !== text) this.input.value = text
        if (text.length > this.maxWavePayloadSize) await this.sendChunked(text)
        else await this.sendWaveFrame(text)
        return text
    }

    parseMessage(message) {
        if (typeof message !== "string") return null
        try {
            return JSON.parse(message)
        } catch {
            return null
        }
    }

    async handleIncoming(message, channel = "wave") {
        if (!message) return
        const parsed = this.parseMessage(message)
        if (parsed?.__waveChunk === 1) {
            await this.consumeChunk(parsed, channel)
            return
        }

        if (message === this.lastIncoming) return
        this.lastIncoming = message
        this.appendIncoming(`[${channel}] ${message}`)
        if (!parsed || typeof parsed !== "object") return

        if (this.role === "sender" && parsed?.[":"] === "auth" && parsed?.["~"] && typeof this.responseBuilder === "function") {
            if (this.sending) return
            this.sending = true
            try {
                const reply = await this.responseBuilder(parsed["~"], parsed)
                if (reply) await this.send(reply)
            } finally {
                this.sending = false
                // Flush audio captured during playback to prevent self-loopback decode
                this.audioBacklog = []
                this.audioBacklogBytes = 0
            }
            return
        }

        if (this.role === "receiver" && parsed?.["~"] === this.session?.pub && parsed?.[":"] && parsed[":"] !== "auth") {
            const from = parsed.from || parsed["^"] || parsed.pub
            if (!from) return
            const { sea } = globalThis
            if (!sea?.secret || !sea?.decrypt) return
            const secret = await sea.secret(from, this.session)
            const seed = await sea.decrypt(parsed[":"], secret)
            if (!seed) return
            this.setStatus("Seed received. Completing signin...")
            this.dispatchEvent(new CustomEvent("signin", {
                detail: { seed, from, channel },
                bubbles: true,
                composed: true
            }))
        }
    }

    async startSignin() {
        const { sea } = globalThis
        if (!sea?.pair) throw new Error("SEA is not available")
        this.role = "receiver"
        this.responseBuilder = null
        this.session = await sea.pair()
        if (!this.session?.pub) throw new Error("Unable to create temporary pair")
        await this.listen()
        const request = { "~": this.session.pub, ":": "auth" }
        await this.send(request)
        this.setStatus("Auth request broadcasted via QR + wave")
        return request
    }

    async startShare(builder) {
        this.role = "sender"
        this.session = null
        this.responseBuilder = typeof builder === "function" ? builder : null
        await this.listen()
        this.setStatus("Listening for auth request to share seed")
    }

    onScan(event) {
        const message = event?.detail?.data
        if (!message) return
        this.handleIncoming(message, "qr").catch((error) => this.setStatus(error?.message || String(error)))
    }

    onSend() {
        const payload = this.input?.value?.trim()
        if (!payload) return
        this.send(payload).catch((error) => this.setStatus(error?.message || String(error)))
    }

    onListen() {
        this.listen().catch((error) => this.setStatus(error?.message || String(error)))
    }

    onStop() {
        this.stop()
    }
}

customElements.define("ui-wave", WAVE)