import * as THREE from "/core/Three.js"

export const NOISE_FLOOR = 0.20
export const DECAY = 0.08
const NOISE_IDLE = 0.008
const NOISE_AUDIO = 0.14

export const DEFAULT_COLORS = [[0, 230, 80], [160, 0, 255]]

// 4D perspective: D4=3 gives inner:outer = 1:2 exactly at theta=0
const D4 = 3.0
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

export class Logic {
    static initpts() {
        return Array.from({ length: 16 }, () => new THREE.Vector3())
    }

    static initscratch() {
        return { vd: new THREE.Vector3(), up: new THREE.Vector3(0, 1, 0) }
    }

    static parsergb(str) {
        const m = str.match(/[\d.]+/g)
        return m ? [+m[0], +m[1], +m[2]] : [128, 128, 128]
    }

    static parsecsscolor(val) {
        if (!val) return [128, 128, 128]
        if (val.startsWith("#")) {
            const h = val.slice(1)
            const x = h.length === 3 ? h.split("").map(c => c + c).join("") : h
            return [parseInt(x.slice(0, 2), 16), parseInt(x.slice(2, 4), 16), parseInt(x.slice(4, 6), 16)]
        }
        return Logic.parsergb(val)
    }

    static noise(x, y, z, time) {
        const t = time
        return (
            Math.sin(x * 2.9 + t)       * Math.cos(y * 3.3 + t * 1.1) * Math.sin(z * 2.7 + t * 0.8) +
            Math.sin(x * 6.1 + t * 1.4) * Math.cos(z * 5.7 + t * 0.6) * Math.sin(y * 4.9 + t * 1.3) * 0.38
        )
    }

    static build(container, w, h) {
        const scene = new THREE.Scene()
        const camera = new THREE.PerspectiveCamera(55, w / h, 0.1, 100)
        camera.position.z = 2.8

        const group = new THREE.Group()
        const tubes = []

        const mat = new THREE.MeshBasicMaterial({
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
                mat
            )
            group.add(mesh)
            tubes.push({ mesh, ai, bi })
        }

        scene.add(group)

        const renderer = new THREE.WebGLRenderer({ antialias: true, alpha: true })
        renderer.setPixelRatio(window.devicePixelRatio || 1)
        renderer.setSize(w, h)
        container.appendChild(renderer.domElement)

        return { scene, camera, renderer, group, mat, tubes }
    }

    // Project all 16 hypercube corners from 4D → 3D via XW-plane rotation.
    // Rotation: x' = x·cos(θ) - w·sin(θ),  w' = x·sin(θ) + w·cos(θ)
    static project4D(theta, amp, time, pts3D) {
        const cosT     = Math.cos(theta)
        const sinT     = Math.sin(theta)
        const noiseAmp = NOISE_IDLE + Math.pow(amp, 0.6) * NOISE_AUDIO
        for (let i = 0; i < 16; i++) {
            const [bx, by, bz, bw] = CN4[i]
            const rx  = bx * cosT - bw * sinT
            const rw  = bx * sinT + bw * cosT
            const s   = D4 / (D4 - rw) * NORM_SCALE
            const px  = rx * s, py = by * s, pz = bz * s
            const len = Math.sqrt(px * px + py * py + pz * pz)
            const nd  = len > 1e-6 ? (Logic.noise(bx, by, bz, time) / 1.38) * noiseAmp : 0
            const inv = 1 / (len || 1)
            pts3D[i].set(px + px * inv * nd, py + py * inv * nd, pz + pz * inv * nd)
        }
    }

    // Stretch a CylinderGeometry(height=1) mesh to span from a to b, no allocation
    static updatetube(mesh, a, b, vd, up) {
        vd.subVectors(b, a)
        const len = vd.length()
        if (len < 1e-6) { mesh.visible = false; return }
        mesh.visible = true
        mesh.position.addVectors(a, b).multiplyScalar(0.5)
        mesh.scale.y = len
        vd.divideScalar(len)
        const dot = vd.dot(up)
        if (dot > 0.9999) {
            mesh.quaternion.identity()
        } else if (dot < -0.9999) {
            mesh.quaternion.set(1, 0, 0, 0)   // 180° around X
        } else {
            mesh.quaternion.setFromUnitVectors(up, vd)
        }
    }

    static colorblend(amp, palette) {
        const n   = palette.length - 1
        const ca  = Math.min(1, amp * 1.2)
        const seg = ca * n
        const idx = Math.min(Math.floor(seg), n - 1)
        const tt  = seg - idx
        const [r0, g0, b0] = palette[idx]
        const [r1, g1, b1] = palette[idx + 1]
        return [
            (r0 + (r1 - r0) * tt) / 255,
            (g0 + (g1 - g0) * tt) / 255,
            (b0 + (b1 - b0) * tt) / 255,
        ]
    }
}

export default Logic
