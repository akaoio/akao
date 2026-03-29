import { render } from "/core/UI.js"
import template from "./template.js"
import { Wave } from "/core/Wave.js"
import Events from "/core/Events.js"

export class WAVE extends HTMLElement {
    constructor() {
        super()
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

    setstatus(text) {
        if (this.status) this.status.textContent = text
    }

    tobytes(floatSamples) {
        const copy = new Float32Array(floatSamples)
        return new Int8Array(copy.buffer)
    }

    ensurecontext() {
        const AudioEngine = globalThis.AudioContext || globalThis.webkitAudioContext
        if (!AudioEngine) throw new Error("Web Audio API is not supported in this browser")
        if (!this.audioContext) this.audioContext = new AudioEngine({ sampleRate: 48000 })
        return this.audioContext
    }

    async prepare() {
        const context = this.ensurecontext()
        await Wave.request({
            method: "configure",
            params: {
                sampleRate: context.sampleRate,
                sampleRateInp: context.sampleRate,
                sampleRateOut: context.sampleRate,
                samplesPerFrame: 1024,
                volume: 25
            }
        })
        return context
    }

    async listen() {
        if (this.running) return true
        if (!navigator?.mediaDevices?.getUserMedia) {
            this.setstatus("Microphone is not supported in this browser")
            return false
        }

        const context = this.ensurecontext()
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

        await Wave.request({
            method: "configure",
            params: {
                sampleRate: inputSampleRate,
                sampleRateInp: inputSampleRate,
                sampleRateOut: inputSampleRate,
                samplesPerFrame: 1024,
                volume: 25,
                reset: true
            }
        })

        this.source = context.createMediaStreamSource(this.stream)
        this.processor = context.createScriptProcessor(1024, 1, 1)
        this.sink = context.createGain()
        this.sink.gain.value = 0
        this.processor.onaudioprocess = this.onaudio
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
        this.running = true
        this.events.emit("stream", { stream: this.stream })
        this.events.emit("analyser", { analyser: this.micAnalyser })
        this.shadowRoot.querySelector("ui-visualizer")?.setanalyser(this.micAnalyser)
        this.setstatus(`Listening`)
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
        this.chunks.clear()
        this.micAnalyser?.disconnect()
        this.micAnalyser = null
        this.events.emit("stream", { stream: null })
        this.events.emit("analyser", { analyser: null })
        this.setstatus("Stopped")
    }

    sleep(ms = 0) {
        return new Promise((resolve) => setTimeout(resolve, ms))
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
        const response = await Wave.encode({ message: text })
        if (!response?.ok || !response?.bytes) throw new Error(response?.error || "Unable to encode payload")
        this.setstatus("Broadcasting")
        await this.play(response.bytes, response.sampleRate)
        const sampleRate = response.sampleRate || 48000
        const durationMs = Math.ceil((response.bytes.length / 2 / sampleRate) * 1000)
        await this.sleep(Math.min(80, Math.max(20, durationMs + 10)))
        return response
    }

    async chunked(text) {
        const total = Math.ceil(text.length / this.maxsize)
        for (let index = 0; index < total; index++) {
            const from = index * this.maxsize
            const part = text.slice(from, from + this.maxsize)
            const last = index === total - 1
            const envelope = JSON.stringify({ "&": last ? `${index}!` : index, ":": part })
            await this.frame(envelope)
            this.setstatus(`Broadcasting chunk ${index + 1}/${total}`)
        }
    }

    async consume(chunk = {}) {
        this.cleanup()
        const raw = chunk["&"]
        const part = chunk[":"]
        if (typeof part !== "string") return
        const last = typeof raw === "string" && raw.endsWith("!")
        const index = last ? parseInt(raw, 10) : Number(raw)
        if (!Number.isInteger(index) || index < 0) return
        if (index === 0) {
            this.chunks.set("_", { parts: [part], updatedAt: Date.now() })
            if (last) {
                this.chunks.delete("_")
                await this.handle(part)
            }
            return
        }
        const current = this.chunks.get("_")
        if (!current) return
        if (current.parts.length !== index) {
            this.chunks.delete("_")
            return
        }
        current.parts.push(part)
        current.updatedAt = Date.now()
        if (last) {
            this.chunks.delete("_")
            await this.handle(current.parts.join(""))
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
                const response = await Wave.decode({ bytes }, { transfer: [bytes.buffer] })
                if (response?.found && response?.message) await this.handle(response.message)
            }
        } catch (error) {
            this.setstatus(error?.message || String(error))
        } finally {
            this.pendingDecode = false
            if (this.running && !this.sending && this.audioBacklogBytes > 0) this.pump()
        }
    }

    async onaudio(event) {
        if (!this.running) return
        if (this.micTemporarilyDisabled) return   // playing — keep mic on but skip decode
        const input = event?.inputBuffer?.getChannelData?.(0)
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
        await new Promise((resolve) => { source.onended = resolve })
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
        if (parsed !== null && "&" in parsed) {
            await this.consume(parsed)
            return
        }
        if (message === this.last) return
        this.last = message
        this.events.emit("message", { message, parsed }, { bubbles: true, composed: true })
    }


}

customElements.define("ui-wave", WAVE)
export default WAVE