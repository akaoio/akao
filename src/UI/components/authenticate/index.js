import { render } from "/core/UI.js"
import template from "./template.js"
import Events from "/core/Events.js"
import { wave, passkey } from "/core/Access.js"

export class AUTHENTICATE extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.events = new Events(this)
        this.subscriptions = []
        this.role = null
        this.session = null
        this.builder = null
        this.sending = false
        this.onwave = this.onwave.bind(this)
        this.onwavebtn = this.onwavebtn.bind(this)
        this.onpasskeybtn = this.onpasskeybtn.bind(this)
        this.stop = this.stop.bind(this)
        this.startsignin = this.startsignin.bind(this)
        this.startshare = this.startshare.bind(this)
    }

    connectedCallback() {
        this.$wave = this.shadowRoot.querySelector("#wave")
        this.$vis = this.shadowRoot.querySelector("#vis")
        this.$wavesection = this.shadowRoot.querySelector(".wave-section")
        this.$wavebtn = this.shadowRoot.querySelector("#wave-btn")
        this.$passkeybtn = this.shadowRoot.querySelector("#passkey-btn")
        this.$wavebtn.addEventListener("click", this.onwavebtn)
        this.$passkeybtn.addEventListener("click", this.onpasskeybtn)
        this.subscriptions.push(
            () => this.$wavebtn.removeEventListener("click", this.onwavebtn),
            () => this.$passkeybtn.removeEventListener("click", this.onpasskeybtn),
            this.$wave.events.on("message", this.onwave),
            this.$wave.events.on("stream", (e) => this.$vis?.stream(e.detail?.stream ?? null))
        )
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
        this.stop()
    }

    showwave(visible) {
        this.$wavesection?.classList?.toggle("active", visible)
    }

    done(response) {
        this.events.emit("done", { response }, { bubbles: true, composed: true })
    }

    async onwave(event) {
        const { parsed } = event?.detail || {}
        if (!parsed || typeof parsed !== "object") return

        if (this.role === "receiver" && parsed?.["~"] === this.session?.pub && parsed?.[":"] && parsed[":"] !== "auth") {
            const from = parsed.from || parsed["^"] || parsed.pub
            if (!from) return
            const { sea } = globalThis
            if (!sea?.secret || !sea?.decrypt) return
            const secret = await sea.secret(from, this.session)
            const seed = await sea.decrypt(parsed[":"], secret)
            if (!seed) return
            this.$wave?.setstatus?.("Seed received. Completing signin...")
            wave({ seed, from, channel: "wave" }).then((response) => this.done(response))
            return
        }

        if (this.role === "sender" && parsed?.[":"] === "auth" && parsed?.["~"] && typeof this.builder === "function") {
            if (this.sending) return
            this.sending = true
            try {
                const reply = await this.builder(parsed["~"], parsed)
                if (reply) await this.$wave?.send(reply)
            } finally {
                this.sending = false
                this.$wave?.audioBacklog && (this.$wave.audioBacklog = [])
                this.$wave && (this.$wave.audioBacklogBytes = 0)
            }
        }
    }

    onwavebtn() {
        this.showwave(true)
        this.startsignin().catch((error) => console.error(error))
    }

    onpasskeybtn() {
        this.showwave(false)
        passkey().then((response) => this.done(response))
    }

    async startsignin() {
        const { sea } = globalThis
        if (!sea?.pair) throw new Error("SEA is not available")
        this.role = "receiver"
        this.builder = null
        this.session = await sea.pair()
        if (!this.session?.pub) throw new Error("Unable to create temporary pair")
        await this.$wave?.listen()
        const request = { "~": this.session.pub, ":": "auth" }
        await this.$wave?.send(request)
        this.$wave?.setstatus?.("Auth request broadcasted via wave")
        return request
    }

    async startshare(builder) {
        this.role = "sender"
        this.session = null
        this.builder = typeof builder === "function" ? builder : null
        this.showwave(true)
        await this.$wave?.listen()
        this.$wave?.setstatus?.("Listening for auth request to share seed")
    }

    stop() {
        this.role = null
        this.session = null
        this.builder = null
        this.sending = false
        this.$wave?.stop?.()
        this.showwave(false)
    }
}

customElements.define("ui-authenticate", AUTHENTICATE)
export default AUTHENTICATE
