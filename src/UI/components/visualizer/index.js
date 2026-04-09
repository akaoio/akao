import { render } from "/core/UI.js"
import template from "./template.js"
import logic, { NOISE_FLOOR, DECAY, DEFAULT_COLORS } from "./logic.js"

export class VISUALIZER extends HTMLElement {
    static module = import.meta.url
    constructor(props = {}) {
        super()
        this.props  = props
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.renderer  = null
        this.scene     = null
        this.camera    = null
        this.group    = null
        this.mat      = null
        this.tubes    = []          // [{ mesh, ai, bi }] — 32 hypercube edges
        this.analyser  = null   // set externally via setanalyser()
        this.raf       = null
        this.amp       = 0
        this.prevRaw   = 0          // previous raw amplitude — for transient detection
        this._glitch   = 0          // current glitch shake magnitude
        this.time      = 0
        this.theta     = 0          // 4D XW-plane rotation angle
        this.probe     = null
        this.ro       = null
        this._mo      = null
        // 16 pre-allocated 3D projected positions — no GC per frame
        this._pts3D    = logic.initpts()
        // scratch vectors for _updateTube
        const { vd, up } = logic.initscratch()
        this._vd = vd
        this._up = up
        this.colorBase    = [255, 255, 255]
        this.effectColors = DEFAULT_COLORS
        this.colorFrame   = 0
        this.draw   = this.draw.bind(this)
        this.resize = this.resize.bind(this)
        this.stop   = this.stop.bind(this)
    }

    connectedCallback() {
        this.probe = this.shadowRoot.querySelector(".probe")
        this.syncattrs()
        this.readcolors()
        this.build()
        this.ro = new ResizeObserver(this.resize)
        this.ro.observe(this)
        // Watch for runtime attribute changes (data-color, data-colors-N)
        this._mo = new MutationObserver(mutations => {
            if (mutations.some(m => m.attributeName === "data-color" ||
                    /^data-colors-\d+$/.test(m.attributeName))) {
                this.syncattrs()
                this.readcolors()
            }
        })
        this._mo.observe(this, { attributes: true })
        this.draw()
    }

    disconnectedCallback() {
        cancelAnimationFrame(this.raf)
        this.raf = null
        this.stop()
        this.ro?.disconnect()
        this._mo?.disconnect()
        this.renderer?.dispose()
    }

    build() {
        const container = this.shadowRoot.querySelector(".scene")
        const w = this.offsetWidth  || 300
        const h = this.offsetHeight || 300
        const built = logic.build(container, w, h)
        this.scene    = built.scene
        this.camera   = built.camera
        this.renderer = built.renderer
        this.group    = built.group
        this.mat      = built.mat
        this.tubes    = built.tubes
    }

    resize() {
        if (!this.renderer || !this.camera) return
        const w = this.offsetWidth  || 300
        const h = this.offsetHeight || 300
        this.camera.aspect = w / h
        this.camera.updateProjectionMatrix()
        this.renderer.setSize(w, h)
    }

    // Set the active AnalyserNode — wave component owns the audio graph,
    // visualizer is generic and just reads whatever analyser it is given.
    // Reset accumulated amplitude state on every analyser swap so the effect
    // does not stay stuck at the previous audio source's energy level.
    setanalyser(analyser) {
        this.analyser = analyser || null
        this.amp      = 0
        this._glitch  = 0
        this.prevRaw  = 0
        if (!this.raf) this.draw()
    }

    // Merge data-color and data-colors-N attributes into this.props
    syncattrs() {
        if (this.hasAttribute("data-color"))
            this.props.color = this.getAttribute("data-color")
        // data-colors-0, data-colors-1 … → sorted array in props.colors
        const entries = Object.entries(this.dataset)
            .filter(([k]) => /^colors\d+$/.test(k))
            .sort(([a], [b]) => +a.replace("colors", "") - +b.replace("colors", ""))
            .map(([, v]) => v)
        if (entries.length >= 1) this.props.colors = entries
    }

    readcolors() {
        // base color: props.color → CSS --color
        if (this.props.color) {
            this.colorBase = logic.parsecsscolor(this.props.color)
        } else if (this.probe) {
            this.colorBase = logic.parsergb(getComputedStyle(this.probe).color)
        }
        // effect colors: props.colors → defaults
        this.effectColors = this.props.colors?.length >= 2
            ? this.props.colors.map(c => logic.parsecsscolor(c))
            : DEFAULT_COLORS
    }

    draw() {
        this.raf = requestAnimationFrame(this.draw)

        this.time += 0.006 + this.amp * 0.06

        if (++this.colorFrame >= 120) { this.readcolors(); this.colorFrame = 0 }

        // Amplitude from analyser — playback analyser takes priority over mic
        let raw = 0
        if (this.analyser) {
            const freq = new Uint8Array(this.analyser.frequencyBinCount)
            this.analyser.getByteFrequencyData(freq)
            // MAX bin (not average): works for both mic (diffuse) and direct wave audio
            // (narrow-band tones like ggwave concentrate energy in few bins — averaging dilutes below noise floor)
            let max = 0
            for (let i = 0; i < freq.length; i++) if (freq[i] > max) max = freq[i]
            raw = max / 255
            raw = raw < NOISE_FLOOR ? 0 : (raw - NOISE_FLOOR) / (1 - NOISE_FLOOR)
        }
        if (raw > this.amp) this.amp = raw
        else this.amp = Math.max(0, this.amp - DECAY)

        // Glitch: spike = positive transient; decays at 0.60/frame (very fast)
        const spike  = Math.max(0, raw - this.prevRaw)
        this._glitch = Math.max(spike * 0.55, this._glitch * 0.60)
        this.prevRaw = raw

        // Apply glitch shake as random group position offset
        if (this._glitch > 0.002) {
            this.group.position.set(
                (Math.random() * 2 - 1) * this._glitch * 0.35,
                (Math.random() * 2 - 1) * this._glitch * 0.35,
                0
            )
        } else {
            this.group.position.set(0, 0, 0)
        }

        // 4D swap speed + 3D tumble: both scale with audio
        const spin   = 0.002 + this.amp * 0.04
        this.theta             += 0.004 + this.amp * 0.05
        this.group.rotation.y  += spin
        this.group.rotation.x  += spin * 0.4

        // Project all 16 hypercube corners from 4D to 3D
        logic.project4D(this.theta, this.amp, this.time, this._pts3D)

        // Update all 32 tube meshes
        for (const { mesh, ai, bi } of this.tubes) {
            logic.updatetube(mesh, this._pts3D[ai], this._pts3D[bi], this._vd, this._up)
        }

        // Multi-stop color gradient: colorBase (silence) → effectColors by amp
        const [r, g, b] = logic.colorblend(this.amp, [this.colorBase, ...this.effectColors])
        this.mat.color.setRGB(r, g, b)
        this.mat.opacity = 0.55 + this.amp * 0.45

        this.renderer.render(this.scene, this.camera)
    }

    stop() {
        this.analyser = null
    }
}

customElements.define("ui-visualizer", VISUALIZER)
export default VISUALIZER
