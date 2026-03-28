import { render } from "/core/UI.js"
import * as THREE from "/core/Three.js"
import template from "./template.js"

const NOISE_FLOOR  = 0.20
const DECAY        = 0.08
const NOISE_IDLE   = 0.008
const NOISE_AUDIO  = 0.14

const DEFAULT_COLORS = [[0, 230, 80], [160, 0, 255]]

// 4D perspective: D4=3 gives inner:outer = 1:2 exactly at theta=0
// proof: D4/(D4-1) = 1.5,  D4/(D4+1) = 0.75,  ratio = 0.5
const D4         = 3.0
const OUTER_HALF = 0.5
const NORM_SCALE = OUTER_HALF / (D4 / (D4 - 1))    // = 0.333…
const TUBE_R_EDGE = 0.028
const TUBE_R_CONN = 0.018

// 16 corners of the 4D unit hypercube  [x, y, z, w]
// [0-7]  w=+1  →  projects to outer cube at theta=0
// [8-15] w=-1  →  projects to inner cube at theta=0
const CN4 = [
    [-1,-1,-1,+1],[+1,-1,-1,+1],[+1,+1,-1,+1],[-1,+1,-1,+1],
    [-1,-1,+1,+1],[+1,-1,+1,+1],[+1,+1,+1,+1],[-1,+1,+1,+1],
    [-1,-1,-1,-1],[+1,-1,-1,-1],[+1,+1,-1,-1],[-1,+1,-1,-1],
    [-1,-1,+1,-1],[+1,-1,+1,-1],[+1,+1,+1,-1],[-1,+1,+1,-1],
]

// 32 edges of the hypercube
const EDGES4 = [
    // 12 outer edges (w=+1, indices 0-7)
    [0,1],[1,2],[2,3],[3,0],[4,5],[5,6],[6,7],[7,4],[0,4],[1,5],[2,6],[3,7],
    // 12 inner edges (w=-1, indices 8-15)
    [8,9],[9,10],[10,11],[11,8],[12,13],[13,14],[14,15],[15,12],[8,12],[9,13],[10,14],[11,15],
    // 8 connector edges: outer corner i <-> inner corner i+8
    [0,8],[1,9],[2,10],[3,11],[4,12],[5,13],[6,14],[7,15],
]

export class VISUALIZER extends HTMLElement {
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
        this._pts3D    = Array.from({ length: 16 }, () => new THREE.Vector3())
        // scratch vectors for _updateTube
        this._vd = new THREE.Vector3()
        this._up = new THREE.Vector3(0, 1, 0)
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

        this.scene  = new THREE.Scene()
        this.camera = new THREE.PerspectiveCamera(55, w / h, 0.1, 100)
        this.camera.position.z = 2.8

        this.group = new THREE.Group()
        this.tubes = []

        this.mat = new THREE.MeshBasicMaterial({
            color:       new THREE.Color(1, 1, 1),
            transparent: true,
            opacity:     0.9,
            blending:    THREE.AdditiveBlending,
            depthWrite:  false,
        })

        // 32 edges: 12 outer + 12 inner + 8 connectors (all from EDGES4)
        for (let e = 0; e < EDGES4.length; e++) {
            const [ai, bi] = EDGES4[e]
            const isConnector = e >= 24    // last 8 entries are connector edges
            const r = isConnector ? TUBE_R_CONN : TUBE_R_EDGE
            const mesh = new THREE.Mesh(
                new THREE.CylinderGeometry(r, r, 1, 6),
                this.mat
            )
            this.group.add(mesh)
            this.tubes.push({ mesh, ai, bi })
        }

        this.scene.add(this.group)

        this.renderer = new THREE.WebGLRenderer({ antialias: true, alpha: true })
        this.renderer.setPixelRatio(window.devicePixelRatio || 1)
        this.renderer.setSize(w, h)
        container.appendChild(this.renderer.domElement)
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
            this.colorBase = this.parsecsscolor(this.props.color)
        } else if (this.probe) {
            this.colorBase = this.parsergb(getComputedStyle(this.probe).color)
        }
        // effect colors: props.colors → defaults
        this.effectColors = this.props.colors?.length >= 2
            ? this.props.colors.map(c => this.parsecsscolor(c))
            : DEFAULT_COLORS
    }

    parsergb(str) {
        const m = str.match(/[\d.]+/g)
        return m ? [+m[0], +m[1], +m[2]] : [128, 128, 128]
    }

    parsecsscolor(val) {
        if (!val) return [128, 128, 128]
        if (val.startsWith("#")) {
            const h = val.slice(1)
            const x = h.length === 3 ? h.split("").map(c => c + c).join("") : h
            return [parseInt(x.slice(0, 2), 16), parseInt(x.slice(2, 4), 16), parseInt(x.slice(4, 6), 16)]
        }
        return this.parsergb(val)
    }

    noise(x, y, z) {
        const t = this.time
        return (
            Math.sin(x * 2.9 + t)       * Math.cos(y * 3.3 + t * 1.1) * Math.sin(z * 2.7 + t * 0.8) +
            Math.sin(x * 6.1 + t * 1.4) * Math.cos(z * 5.7 + t * 0.6) * Math.sin(y * 4.9 + t * 1.3) * 0.38
        )
    }

    // Project all 16 hypercube corners from 4D → 3D via XW-plane rotation.
    // Rotation: x' = x·cos(θ) - w·sin(θ),  w' = x·sin(θ) + w·cos(θ)
    // At θ=0: outer (w=+1) projects at scale 1.5×NORM = OUTER_HALF
    //         inner (w=-1) projects at scale 0.75×NORM = OUTER_HALF/2
    // At θ=π: the cubes are fully swapped.
    // Corners never collide: at θ=π/2 same-xyz pairs project to x=−1 vs x=+1 (mirrored).
    _project4D() {
        const cosT     = Math.cos(this.theta)
        const sinT     = Math.sin(this.theta)
        const noiseAmp = NOISE_IDLE + Math.pow(this.amp, 0.6) * NOISE_AUDIO
        for (let i = 0; i < 16; i++) {
            const [bx, by, bz, bw] = CN4[i]
            const rx  = bx * cosT - bw * sinT
            const rw  = bx * sinT + bw * cosT
            const s   = D4 / (D4 - rw) * NORM_SCALE
            const px  = rx * s, py = by * s, pz = bz * s
            const len = Math.sqrt(px * px + py * py + pz * pz)
            const nd  = len > 1e-6 ? (this.noise(bx, by, bz) / 1.38) * noiseAmp : 0
            const inv = 1 / (len || 1)
            this._pts3D[i].set(px + px * inv * nd, py + py * inv * nd, pz + pz * inv * nd)
        }
    }

    // Stretch a CylinderGeometry(height=1) mesh to span from a to b, no allocation
    _updateTube(mesh, a, b) {
        this._vd.subVectors(b, a)
        const len = this._vd.length()
        if (len < 1e-6) { mesh.visible = false; return }
        mesh.visible = true
        mesh.position.addVectors(a, b).multiplyScalar(0.5)
        mesh.scale.y = len
        this._vd.divideScalar(len)
        const dot = this._vd.dot(this._up)
        if (dot > 0.9999) {
            mesh.quaternion.identity()
        } else if (dot < -0.9999) {
            mesh.quaternion.set(1, 0, 0, 0)   // 180° around X
        } else {
            mesh.quaternion.setFromUnitVectors(this._up, this._vd)
        }
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
        this._project4D()

        // Update all 32 tube meshes
        for (const { mesh, ai, bi } of this.tubes) {
            this._updateTube(mesh, this._pts3D[ai], this._pts3D[bi])
        }

        // Multi-stop color gradient: colorBase (silence) → effectColors by amp
        const palette = [this.colorBase, ...this.effectColors]
        const n   = palette.length - 1
        const ca  = Math.min(1, this.amp * 1.2)
        const seg = ca * n
        const idx = Math.min(Math.floor(seg), n - 1)
        const tt  = seg - idx
        const [r0, g0, b0] = palette[idx]
        const [r1, g1, b1] = palette[idx + 1]
        this.mat.color.setRGB(
            (r0 + (r1 - r0) * tt) / 255,
            (g0 + (g1 - g0) * tt) / 255,
            (b0 + (b1 - b0) * tt) / 255
        )
        this.mat.opacity = 0.55 + this.amp * 0.45

        this.renderer.render(this.scene, this.camera)
    }

    stop() {
        this.analyser = null
    }
}

customElements.define("ui-visualizer", VISUALIZER)
export default VISUALIZER
