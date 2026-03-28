import { render } from "/core/UI.js"
import template from "./template.js"

const RINGS = 3
const RING_T = [0.28, 0.55, 0.82]   // radii as fraction of maxR
const SPACING = 3                   // CSS px between particle centers → dense rings
const SPRING = 0.07
const DAMPING = 0.80
const NOISE_FLOOR = 0.12
const DECAY = 0.05
const PUSH_FRAC = 0.018  // push = amp * buildMaxR * PUSH_FRAC; tỷ lệ với canvas
const JITTER = 0.006     // per-frame random noise strength

export class VISUALIZER extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.audioCtx = null
        this.analyser = null
        this.source = null
        this.raf = null
        this.amps = new Float32Array(RINGS)
        this.particles = []
        this.canvas = null
        this.ctx = null
        this.probe = null
        this.ro = null
        this.built = false
        this.colorBase = [255, 255, 255]
        this.colorAccent = [0, 229, 255]
        this.colorFrame = 0
        this.stream = this.stream.bind(this)
        this.draw = this.draw.bind(this)
        this.stop = this.stop.bind(this)
        this.resize = this.resize.bind(this)
    }

    connectedCallback() {
        this.canvas = this.shadowRoot.querySelector("canvas")
        this.ctx = this.canvas.getContext("2d")
        this.probe = this.shadowRoot.querySelector(".probe")
        this.readcolors()
        this.resize()
        this.ro = new ResizeObserver(this.resize)
        this.ro.observe(this)
    }

    disconnectedCallback() {
        this.stop()
        this.ro?.disconnect()
    }

    resize() {
        if (!this.canvas) return
        const dpr = window.devicePixelRatio || 1
        const w = this.offsetWidth || 200
        const h = this.offsetHeight || 200
        this.canvas.width = Math.round(w * dpr)
        this.canvas.height = Math.round(h * dpr)
        this.canvas.style.width = `${w}px`
        this.canvas.style.height = `${h}px`
        this.built = false
    }

    build() {
        const W = this.canvas.width
        const H = this.canvas.height
        const cx = W / 2
        const cy = H / 2
        const dpr = window.devicePixelRatio || 1
        const maxR = Math.min(cx, cy) * 0.9
        const cssMaxR = maxR / dpr   // convert canvas px → CSS px for spacing calc
        this.particles = []
        for (let r = 0; r < RINGS; r++) {
            const radius = RING_T[r] * maxR
            const cssR = RING_T[r] * cssMaxR
            const count = Math.max(20, Math.round(2 * Math.PI * cssR / SPACING))
            for (let i = 0; i < count; i++) {
                const angle = (i / count) * Math.PI * 2
                const hx = cx + Math.cos(angle) * radius
                const hy = cy + Math.sin(angle) * radius
                this.particles.push({ hx, hy, x: hx, y: hy, vx: 0, vy: 0, ring: r,
                    dir: Math.random() < 0.5 ? 1 : -1,
                    tan: (Math.random() - 0.5) * 0.6,
                    noise: 0.7 + Math.random() * 0.6
                })
            }
        }
        this.built = true
    }

    stream(mediaStream) {
        this.source?.disconnect()
        this.source = null
        this.analyser = null
        if (this.audioCtx) { this.audioCtx.close(); this.audioCtx = null }
        if (mediaStream) {
            const AudioEngine = globalThis.AudioContext || globalThis.webkitAudioContext
            if (AudioEngine) {
                this.audioCtx = new AudioEngine()
                this.analyser = this.audioCtx.createAnalyser()
                this.analyser.fftSize = 256
                this.analyser.smoothingTimeConstant = 0.8
                this.source = this.audioCtx.createMediaStreamSource(mediaStream)
                this.source.connect(this.analyser)
            }
        }
        if (this.raf) cancelAnimationFrame(this.raf)
        this.raf = null
        this.draw()
    }

    readcolors() {
        if (!this.probe) return
        const s = getComputedStyle(this.probe)
        this.colorBase = this.parsergb(s.color)
        this.colorAccent = this.parsergb(s.backgroundColor)
    }

    parsergb(str) {
        const m = str.match(/[\d.]+/g)
        return m ? [+m[0], +m[1], +m[2]] : [128, 128, 128]
    }

    draw() {
        this.raf = requestAnimationFrame(this.draw)
        const canvas = this.canvas
        if (!canvas) return
        if (!this.built) this.build()
        // Throttle getComputedStyle: re-read theme colors every 120 frames (~2s)
        if (++this.colorFrame >= 120) { this.readcolors(); this.colorFrame = 0 }

        const W = canvas.width
        const H = canvas.height
        const dpr = window.devicePixelRatio || 1
        const cx = W / 2
        const cy = H / 2
        const buildMaxR = Math.min(cx, cy) * 0.9
        const maxDisp = (PUSH_FRAC / SPRING) * buildMaxR
        const clipR = RING_T[RINGS - 1] * buildMaxR + maxDisp + dpr * 3
        const c = this.ctx

        c.clearRect(0, 0, W, H)
        c.save()
        c.beginPath()
        c.arc(cx, cy, clipR, 0, Math.PI * 2)
        c.clip()

        // Per-ring amplitude with noise gate
        let freq = null
        if (this.analyser) {
            freq = new Uint8Array(this.analyser.frequencyBinCount)
            this.analyser.getByteFrequencyData(freq)
        }

        for (let i = 0; i < RINGS; i++) {
            let amp = 0
            if (freq) {
                const bins = freq.length
                const start = Math.floor((i / RINGS) * bins * 0.55)
                const end = Math.max(start + 1, Math.floor(((i + 1) / RINGS) * bins * 0.55))
                let sum = 0
                for (let b = start; b < end; b++) sum += freq[b]
                amp = sum / ((end - start) * 255)
                amp = amp < NOISE_FLOOR ? 0 : (amp - NOISE_FLOOR) / (1 - NOISE_FLOOR)
            }
            if (amp > this.amps[i]) this.amps[i] = amp
            else this.amps[i] = Math.max(0, this.amps[i] - DECAY)
        }

        // Update and draw particles
        const jitterScale = buildMaxR * JITTER
        for (const p of this.particles) {
            const amp = this.amps[p.ring]

            // Spring toward home
            const sx = (p.hx - p.x) * SPRING
            const sy = (p.hy - p.y) * SPRING

            // Radial unit vector from home position outward
            const rdx = p.hx - cx
            const rdy = p.hy - cy
            const rlen = Math.sqrt(rdx * rdx + rdy * rdy) || 1
            const rnx = rdx / rlen
            const rny = rdy / rlen

            // jitter scales with amp → zero when silent, so self-cancel can trigger
            const push = amp * buildMaxR * PUSH_FRAC
            const jx = amp * (Math.random() - 0.5) * jitterScale * p.noise * 4
            const jy = amp * (Math.random() - 0.5) * jitterScale * p.noise * 4

            p.vx = (p.vx + sx + rnx * push * p.dir + (-rny) * push * p.tan + jx) * DAMPING
            p.vy = (p.vy + sy + rny * push * p.dir + (rnx)  * push * p.tan + jy) * DAMPING
            p.x += p.vx
            p.y += p.vy

            // Color: inline lerp --color → --color-accent by displacement from home
            const ddx = p.x - p.hx
            const ddy = p.y - p.hy
            const disp = Math.min(1, Math.sqrt(ddx * ddx + ddy * ddy) / (maxDisp * 0.5 || 1))
            const lr = (this.colorBase[0] + (this.colorAccent[0] - this.colorBase[0]) * disp) | 0
            const lg = (this.colorBase[1] + (this.colorAccent[1] - this.colorBase[1]) * disp) | 0
            const lb = (this.colorBase[2] + (this.colorAccent[2] - this.colorBase[2]) * disp) | 0
            const alpha = 0.2 + disp * 0.8

            // Short radial segment oriented toward center
            const tox = cx - p.x
            const toy = cy - p.y
            const tlen = Math.sqrt(tox * tox + toy * toy) || 1
            const tnx = tox / tlen
            const tny = toy / tlen
            const segLen = dpr * (1.2 + amp * 2)

            // Gradient: bright at head (toward center) → transparent at tail
            const hx0 = p.x + tnx * segLen   // head, toward center
            const hy0 = p.y + tny * segLen
            const hx1 = p.x - tnx * segLen   // tail, away from center
            const hy1 = p.y - tny * segLen
            const grad = c.createLinearGradient(hx0, hy0, hx1, hy1)
            grad.addColorStop(0, `rgba(${lr},${lg},${lb},0)`)
            grad.addColorStop(1, `rgba(${lr},${lg},${lb},${alpha.toFixed(2)})`)
            c.beginPath()
            c.moveTo(hx0, hy0)
            c.lineTo(hx1, hy1)
            c.strokeStyle = grad
            c.lineWidth = dpr * 0.6
            c.stroke()
        }

        c.restore()

        // Self-cancel when no audio and all particles returned to rest
        if (!this.analyser && this.amps.every(a => a < 0.001)) {
            const rested = this.particles.every(p => {
                const dx = p.x - p.hx
                const dy = p.y - p.hy
                return Math.abs(p.vx) < 0.05 && Math.abs(p.vy) < 0.05
                    && dx * dx + dy * dy < 0.25
            })
            if (rested) {
                cancelAnimationFrame(this.raf)
                this.raf = null
            }
        }
    }

    stop() {
        this.source?.disconnect()
        this.source = null
        this.analyser = null
        if (this.audioCtx) { this.audioCtx.close(); this.audioCtx = null }
        // raf keeps running so particles spring back to rest, then self-cancels
    }
}

customElements.define("ui-visualizer", VISUALIZER)
export default VISUALIZER
