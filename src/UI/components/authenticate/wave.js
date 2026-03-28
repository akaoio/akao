/**
 * Wave Authentication Protocol
 *
 * Message format (on-air, sent as raw object via ggwave):
 *
 *   "~" always = epub of whoever sends the message.
 *
 *   Auth request  (receiver → broadcast):
 *     { "~": session.epub, ":": "auth" }
 *
 *   Auth reply    (sender → broadcast):
 *     { "~": pair.epub,  ← from: sender's epub (needed for ECDH)
 *       ":": "seed",    ← message type
 *       ct, iv, s        ← AES-GCM fields from sea.encrypt(..., {raw:true})
 *     }
 *
 * Receiver identifies its reply by attempting decrypt with top-level ct/iv/s.
 * If decrypt yields a valid seed the message was addressed to it.
 *
 * Both sides compute the same ECDH shared secret:
 *   Sender:   sea.secret(session.epub, sender_pair)
 *   Receiver: sea.secret(pair.epub,    session_pair)
 */

export class WaveAuth {
    constructor() {
        this.role = null
        this.session = null
        this.builder = null
        this.sending = false
    }

    /**
     * Receiver side: create ephemeral pair, broadcast auth request.
     * @param {HTMLElement} waveEl - ui-wave element
     */
    async startsignin(waveEl) {
        const { sea } = globalThis
        if (!sea?.pair) throw new Error("SEA is not available")
        this.role = "receiver"
        this.builder = null
        this.session = await sea.pair()
        if (!this.session?.epub) throw new Error("Unable to create temporary pair")
        await waveEl?.listen()
        const request = { "~": this.session.epub, ":": "auth" }
        await waveEl?.send(request)
        waveEl?.setstatus?.("Auth request broadcasted via wave")
        return request
    }

    /**
     * Sender side: listen for an auth request then reply with encrypted seed.
     * @param {HTMLElement} waveEl
     * @param {Function} builder  async (peerEpub) => { "~", "^", ":" } | null
     */
    async startshare(waveEl, builder) {
        this.role = "sender"
        this.session = null
        this.builder = typeof builder === "function" ? builder : null
        await waveEl?.listen()
        waveEl?.setstatus?.("Listening for auth request to share seed")
    }

    /**
     * Handle an incoming wave message event.
     * @param {CustomEvent} event
     * @param {HTMLElement} waveEl
     * @param {Function} onSeedReceived  ({ seed, from, channel }) => void
     */
    async onmessage(event, waveEl, onSeedReceived) {
        const { parsed } = event?.detail || {}
        if (!parsed || typeof parsed !== "object") return

        // ── Receiver: try to decrypt any "seed" message ─────────────────────
        if (this.role === "receiver" && parsed[":"] === "seed") {
            const from = parsed["~"]
            if (!from) return
            const { sea } = globalThis
            if (!sea?.secret || !sea?.decrypt) return
            const secret = await sea.secret(from, this.session)
            const seed = await sea.decrypt({ ct: parsed.ct, iv: parsed.iv, s: parsed.s }, secret)
            if (!seed) return
            waveEl?.setstatus?.("Seed received. Completing signin...")
            onSeedReceived?.({ seed, from, channel: "wave" })
            return
        }

        // ── Sender: check if this is an auth request ─────────────────────────
        if (
            this.role === "sender" &&
            parsed[":"] === "auth" &&
            parsed["~"] &&
            typeof this.builder === "function"
        ) {
            if (this.sending) return
            this.sending = true
            try {
                const reply = await this.builder(parsed["~"])
                if (reply) await waveEl?.send(reply)
            } finally {
                this.sending = false
                if (waveEl?.audioBacklog) waveEl.audioBacklog = []
                if (waveEl) waveEl.audioBacklogBytes = 0
            }
        }
    }

    stop() {
        this.role = null
        this.session = null
        this.builder = null
        this.sending = false
    }
}

export default WaveAuth
