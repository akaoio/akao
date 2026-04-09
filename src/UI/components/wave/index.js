import { render } from "/core/UI.js"
import template from "./template.js"
import Events from "/core/Events.js"
import { notify } from "/core/Utils/browser.js"
import { Context } from "/core/Context.js"
import logic from "./logic.js"

export class WAVE extends HTMLElement {
    static module = import.meta.url
    constructor() {
        super()
        this.template = template // Store for HMR
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.events = new Events(this)
        this.audioContext = null
        this.stream = null
        this.source = null
        this.processor = null
        this.sink = null
        this.micTemporarilyDisabled = false
        this.pendingDecode = false
        this.audioBacklog = []
        this.audioBacklogBytes = 0
        this.maxsize = 140
        this.chunksttl = 60000
        this.chunks = new Map()
        this.micAnalyser = null
        this.running = false
        this.sending = false
        this.last = ""
        this.activesource = null
        this.ackcb = null
        this.nacktimer = null
        this.nackchannel = null
        this.onaudio = this.onaudio.bind(this)
        this.stop = this.stop.bind(this)
        this.listen = this.listen.bind(this)
        this.send = this.send.bind(this)
        this.play = this.play.bind(this)
    }

    connectedCallback() {
        this.status = this.shadowRoot.querySelector("#status")
    }

    disconnectedCallback() {
        this.stop()
    }

    tobytes(floatSamples) {
        const copy = new Float32Array(floatSamples)
        return new Int8Array(copy.buffer)
    }

    ensurecontext() {
        const AudioEngine = globalThis.AudioContext || globalThis.webkitAudioContext
        if (!AudioEngine) {
            notify({ content: Context.get(["dictionary", "webAudioUnsupported"]) })
            return null
        }
        if (!this.audioContext) this.audioContext = new AudioEngine({ sampleRate: 48000 })
        return this.audioContext
    }

    async prepare() {
        const context = this.ensurecontext()
        if (!context) return null
        await logic.configure(context.sampleRate, { volume: 50 })
        return context
    }

    async listen() {
        if (this.running) return true
        this.running = true  // claim slot early — prevents concurrent double-init
        if (!navigator?.mediaDevices?.getUserMedia) {
            this.running = false
            notify({ content: Context.get(["dictionary", "microphoneUnsupported"]) })
            return false
        }

        const context = this.ensurecontext()
        if (!context) { this.running = false; return false }
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

        await logic.configure(inputSampleRate, { volume: 25, reset: true })

        this.source = context.createMediaStreamSource(this.stream)
        await context.audioWorklet.addModule("/UI/components/wave/worklet.js")
        this.processor = new AudioWorkletNode(context, "audio-capture")
        this.processor.port.onmessage = (e) => this.onaudio(e.data)
        this.sink = context.createGain()
        this.sink.gain.value = 0
        this.source.connect(this.processor)
        this.processor.connect(this.sink)
        this.sink.connect(context.destination)
        // Tap mic audio for visualizer: side branch through zero-gain node so analyser
        // is in the pull graph (processed by audio engine) without affecting decoding chain
        this.micAnalyser = context.createAnalyser()
        this.micAnalyser.fftSize = 256
        this.micAnalyser.smoothingTimeConstant = 0.4
        const micTap = context.createGain()
        micTap.gain.value = 0
        this.source.connect(this.micAnalyser)
        this.micAnalyser.connect(micTap)
        micTap.connect(context.destination)
        this.events.emit("stream", { stream: this.stream })
        this.events.emit("analyser", { analyser: this.micAnalyser })
        this.shadowRoot.querySelector("ui-visualizer")?.setanalyser(this.micAnalyser)
        this.status.dataset.key = "dictionary.listening"
        return true
    }

    async stop() {
        this.running = false
        this.pendingDecode = false
        this.sending = false
        this.audioBacklog = []
        this.audioBacklogBytes = 0
        try { this.activesource?.stop() } catch {}
        this.activesource = null
        if (this.processor) {
            this.processor.port.onmessage = null
            this.processor.disconnect()
        }
        if (this.source) this.source.disconnect()
        if (this.sink) this.sink.disconnect()
        if (this.stream) this.stream.getTracks().forEach((track) => track.stop())
        this.processor = null
        this.source = null
        this.sink = null
        this.stream = null
        this.chunks.clear()
        if (this.nacktimer) clearTimeout(this.nacktimer)
        this.nacktimer = null
        this.nackchannel = null
        this.ackcb = null
        this.micAnalyser?.disconnect()
        this.micAnalyser = null
        this.events.emit("stream", { stream: null })
        this.events.emit("analyser", { analyser: null })
        this.status.dataset.key = "dictionary.stopped"
        if (this.audioContext) {
            this.audioContext.close()
            this.audioContext = null
        }
    }

    sleep(ms = 0) {
        return new Promise((resolve) => setTimeout(resolve, ms))
    }

    rndchannel() {
        const chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
        let result = ""
        const arr = crypto.getRandomValues(new Uint8Array(5))
        for (const b of arr) result += chars[b % chars.length]
        return result
    }

    // Wait for an ACK or NACK matching expectedIdx on channel.
    // Stale ACKs for other indices are ignored; NACK (-1) always resolves immediately.
    waitack(channel, expectedIdx, timeoutMs = 6000) {
        return new Promise((resolve) => {
            const timer = setTimeout(() => { this.ackcb = null; resolve(null) }, timeoutMs)
            this.ackcb = (parsed) => {
                if (parsed["."] !== channel) return
                if (typeof expectedIdx === "number" && parsed["*"] !== expectedIdx && parsed["*"] !== -1) return
                clearTimeout(timer)
                this.ackcb = null
                resolve(parsed)
            }
        })
    }

    // Schedule a NACK on channel if no chunk arrives within delay ms.
    schedulenack(channel, delay = 14000) {
        if (this.nacktimer) clearTimeout(this.nacktimer)
        this.nacktimer = setTimeout(async () => {
            this.nacktimer = null
            if (this.nackchannel !== channel) return
            if (this.pendingDecode || this.micTemporarilyDisabled) return
            await this.frame(JSON.stringify({ "*": -1, ".": channel })).catch(() => {})
        }, delay)
    }

    mic(enabled) {
        const tracks = this.stream?.getAudioTracks?.()
        if (!Array.isArray(tracks) || !tracks.length) return
        tracks.forEach((track) => { track.enabled = Boolean(enabled) })
    }

    cleanup() {
        const now = Date.now()
        for (const [id, entry] of this.chunks.entries())
            if (!entry?.updatedAt || now - entry.updatedAt > this.chunksttl) this.chunks.delete(id)
    }

    async frame(text) {
        const response = await logic.encode(text)
        if (!response?.ok || !response?.bytes) {
            notify({ content: response?.error || Context.get(["dictionary", "encodeFailed"]) })
            return null
        }
        this.status.dataset.key = "dictionary.broadcasting"
        await this.play(response.bytes, response.sampleRate)
        const sampleRate = response.sampleRate || 48000
        const durationMs = Math.ceil((response.bytes.length / 2 / sampleRate) * 1000)
        await this.sleep(Math.min(80, Math.max(20, durationMs + 10)))
        return response
    }

    async chunked(text) {
        // Pre-compute all envelopes once; channel ties all chunks of this broadcast
        // together so consumer can isolate parallel/echo transmissions by channel key.
        const channel = this.rndchannel()
        const envelopes = []
        let start = 0
        let index = 0
        while (start < text.length) {
            let end = Math.min(start + this.maxsize, text.length)
            let envelope
            // Shrink slice until serialized envelope fits within maxsize.
            // JSON.stringify escapes " and \ inside the content, inflating size beyond
            // the raw content length — compensate by measuring the actual envelope.
            while (end > start) {
                const part = text.slice(start, end)
                const isLast = end === text.length
                envelope = JSON.stringify({ "&": isLast ? `${index}!` : index, ":": part, ".": channel })
                if (envelope.length <= this.maxsize) break
                end -= (envelope.length - this.maxsize)
            }
            envelopes.push(envelope)
            start = end
            index++
        }
        // Announce ready so receiver arms its NACK timer for this channel
        this.sending = true
        try { await this.frame(JSON.stringify({ "&": "^", ".": channel })) }
        finally { this.sending = false }
        await this.sleep(300)
        // ACK-driven: send each chunk, then wait for receiver ACK.
        // sending=false during waitack so pump() can decode incoming audio.
        // Retry up to 2 extra times on NACK (-1) or timeout (null).
        for (let i = 0; i < envelopes.length; i++) {
            let acked = false
            for (let attempt = 0; attempt < 3 && !acked; attempt++) {
                if (i > 0 || attempt > 0) await this.sleep(attempt > 0 ? 400 : 600)
                this.sending = true
                try { await this.frame(envelopes[i]) }
                finally { this.sending = false }
                const ack = await this.waitack(channel, i, 6000)
                if (ack !== null && ack["*"] === i) acked = true
            }
            this.status.dataset.key = acked ? "dictionary.broadcasting" : "dictionary.retrying"
        }
    }

    async consume(chunk = {}) {
        this.cleanup()
        const raw = chunk["&"]
        const part = chunk[":"]
        // Channel isolates parallel/echo broadcasts; fall back to "_" for legacy messages
        const channel = typeof chunk["."] === "string" ? chunk["."] : "_"
        if (typeof part !== "string") return
        const last = typeof raw === "string" && raw.endsWith("!")
        const index = last ? parseInt(raw, 10) : Number(raw)
        if (!Number.isInteger(index) || index < 0) return
        // Cancel pending NACK — we received something — then ACK this chunk back to sender
        if (channel !== "_") {
            if (this.nackchannel === channel && this.nacktimer) { clearTimeout(this.nacktimer); this.nacktimer = null }
            await this.frame(JSON.stringify({ "*": index, ".": channel })).catch(() => {})
        }
        if (index === 0) {
            const existing = this.chunks.get(channel)
            if (existing?.parts[0] === part) {
                existing.updatedAt = Date.now()
            } else {
                this.chunks.set(channel, { parts: [part], total: null, updatedAt: Date.now() })
            }
            if (last) {
                if (this.nackchannel === channel) this.nackchannel = null
                this.chunks.delete(channel)
                await this.handle(part)
            } else if (channel !== "_") {
                this.schedulenack(channel)  // expect next chunk
            }
            return
        }
        const current = this.chunks.get(channel)
        if (!current) return
        // Gap-tolerant: extend array with null placeholders so a missed chunk in one
        // pass can be filled by a later pass without losing already-received chunks.
        while (current.parts.length <= index) current.parts.push(null)
        if (current.parts[index] === part && current.parts[index] !== null) return  // exact duplicate
        current.parts[index] = part
        current.updatedAt = Date.now()
        if (last) current.total = index + 1
        // Only assemble once the last chunk is known and every slot is filled
        if (current.total != null && !current.parts.slice(0, current.total).includes(null)) {
            const joined = current.parts.slice(0, current.total).join("")
            if (this.nackchannel === channel) { if (this.nacktimer) clearTimeout(this.nacktimer); this.nacktimer = null; this.nackchannel = null }
            this.chunks.delete(channel)
            await this.handle(joined)
        } else if (channel !== "_") {
            this.schedulenack(channel)  // expect next chunk
        }
    }

    dequeue(maxChunks = 6) {
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

    async pump() {
        if (!this.running || this.pendingDecode || this.sending) return
        this.pendingDecode = true
        try {
            while (this.running && !this.sending) {
                const bytes = this.dequeue(8)
                if (!bytes?.length) break
                const response = await logic.decode(bytes)
                if (response?.found && response?.message) await this.handle(response.message)
            }
        } catch (error) {
            if (error?.message) notify({ content: error.message })
        } finally {
            this.pendingDecode = false
            if (this.running && !this.sending && this.audioBacklogBytes > 0) this.pump()
        }
    }

    async onaudio(input) {
        if (!this.running) return
        if (this.micTemporarilyDisabled) return   // playing — keep mic on but skip decode
        if (!input?.length) return
        const bytes = this.tobytes(input)
        this.audioBacklog.push(bytes)
        this.audioBacklogBytes += bytes.byteLength
        if (this.audioBacklog.length > 64) {
            const removed = this.audioBacklog.shift()
            if (removed?.byteLength) this.audioBacklogBytes -= removed.byteLength
        }
        this.pump()
    }

    async play(bytes, sampleRate = 48000) {
        const context = this.ensurecontext()
        if (!context) return false
        if (context.state === "suspended") await context.resume()
        const pcm = bytes instanceof Int8Array ? bytes : new Int8Array(bytes)
        if (!pcm.length) return false
        const samples = Math.floor(pcm.length / 2)
        const audio = context.createBuffer(1, samples, sampleRate)
        const channel = audio.getChannelData(0)
        const view = new DataView(pcm.buffer, pcm.byteOffset, pcm.byteLength)
        for (let i = 0; i < samples; i++) channel[i] = view.getInt16(i * 2, true) / 32768
        const source   = context.createBufferSource()
        const gain     = context.createGain()
        gain.gain.value = 1.0
        source.buffer = audio
        // Restore original playback chain exactly; tap analyser as a silent side branch
        // so getByteFrequencyData works without inline-analyser latency/browser quirks
        source.connect(gain)
        gain.connect(context.destination)
        // Merged analyser: combines wave output + live mic so visualizer reacts to both.
        // Multiple sources can connect to the same AudioNode — Web Audio mixes them.
        const mergeAnalyser = context.createAnalyser()
        mergeAnalyser.fftSize = 256
        mergeAnalyser.smoothingTimeConstant = 0.4
        const mergeTap = context.createGain()
        mergeTap.gain.value = 0
        gain.connect(mergeAnalyser)            // wave audio
        if (this.source) this.source.connect(mergeAnalyser)  // mic audio (if listening)
        mergeAnalyser.connect(mergeTap)
        mergeTap.connect(context.destination)
        this.micTemporarilyDisabled = true     // suppress decode; mic track stays enabled
        this.events.emit("analyser", { analyser: mergeAnalyser })
        source.start()
        this.activesource = source
        await new Promise((resolve) => { source.onended = resolve })
        this.activesource = null
        if (this.source) this.source.disconnect(mergeAnalyser)
        this.audioBacklog = []                 // discard ggwave's own reflected audio
        this.audioBacklogBytes = 0
        this.micTemporarilyDisabled = false
        this.events.emit("analyser", { analyser: this.micAnalyser })
        return true
    }

    async send(payload) {
        await this.prepare()
        const text = typeof payload === "string" ? payload : JSON.stringify(payload)
        if (!text) return null
        if (text.length > this.maxsize) await this.chunked(text)
        else await this.frame(text)
        return text
    }

    parse(message) {
        if (typeof message !== "string") return null
        try { return JSON.parse(message) } catch { return null }
    }

    async handle(message) {
        if (!message) return
        const parsed = this.parse(message)
        if (parsed !== null) {
            if (parsed["&"] === "^") {
                // Ready announcement from sender — arm NACK timer for this channel
                const ch = parsed["."]
                if (ch) { this.nackchannel = ch; this.schedulenack(ch) }
                return
            }
            if ("&" in parsed) { return await this.consume(parsed) }
            if ("*" in parsed) {
                // ACK or NACK from receiver
                if (this.ackcb) this.ackcb(parsed)
                return
            }
        }
        if (message === this.last) return
        this.last = message
        this.events.emit("message", { message, parsed }, { bubbles: true, composed: true })
    }
}

customElements.define("ui-wave", WAVE)
export default WAVE