import { render } from "/core/UI.js"
import template from "./template.js"
import Events from "/core/Events.js"
import { Access } from "/core/Access.js"

function shortenepub(epub) {
    if (!epub || epub.length <= 12) return epub || ""
    return `${epub.slice(0, 5)}...${epub.slice(-5)}`
}

export class AUTHORIZE extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.events = new Events(this)
        this.subscriptions = []
        this.state = "listening"
        this.pending = null
        this.onwave = this.onwave.bind(this)
        this.ondeny = this.ondeny.bind(this)
        this.ongrant = this.ongrant.bind(this)
    }

    connectedCallback() {
        this.$wave = this.shadowRoot.querySelector("#wave")
        this.$vis = this.shadowRoot.querySelector("#vis")
        this.$confirm = this.shadowRoot.querySelector("#confirm")
        this.$note = this.shadowRoot.querySelector("#note")
        this.$denybtn = this.shadowRoot.querySelector("#deny-btn")
        this.$grantbtn = this.shadowRoot.querySelector("#grant-btn")
        this.$denybtn.addEventListener("click", this.ondeny)
        this.$grantbtn.addEventListener("click", this.ongrant)
        this.subscriptions.push(
            () => this.$denybtn.removeEventListener("click", this.ondeny),
            () => this.$grantbtn.removeEventListener("click", this.ongrant),
            this.$wave.events.on("message", this.onwave),
            this.$wave.events.on("analyser", (e) => this.$vis?.setanalyser(e.detail?.analyser ?? null))
        )
        this.$wave.listen()
        this.setstate("listening")
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
        this.$wave?.stop?.()
    }

    setstate(state) {
        this.state = state
        this.$confirm.hidden = state !== "confirming"
    }

    async onwave(event) {
        const { parsed } = event?.detail || {}
        if (!parsed || typeof parsed !== "object") return
        if (parsed[":"] !== ">" || !parsed["~"]) return
        if (this.state !== "listening") return
        this.pending = parsed
        this.$note.textContent = shortenepub(parsed["~"])
        this.setstate("confirming")
    }

    async ondeny() {
        if (!this.$wave) return
        await this.$wave.send({ ":": "!>" })
        this.pending = null
        this.setstate("listening")
    }

    async ongrant() {
        if (!this.pending) return
        const { sea } = globalThis
        const pair = Access.get("pair")
        const seed = Access.get("seed")
        if (!Access.get("authenticated") || !pair || !seed || !sea?.secret || !sea?.encrypt) return
        const secret = await sea.secret(this.pending["~"], pair)
        const encrypted = await sea.encrypt(seed, secret, null, { raw: true })
        await this.$wave.send({
            "~": pair.epub,
            "!": encrypted.ct,
            "@": encrypted.iv,
            "#": encrypted.s
        })
        this.pending = null
        this.setstate("listening")
    }
}

customElements.define("ui-authorize", AUTHORIZE)
export default AUTHORIZE
