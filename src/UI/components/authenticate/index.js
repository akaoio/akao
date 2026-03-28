import { render } from "/core/UI.js"
import template from "./template.js"
import Events from "/core/Events.js"
import { wave } from "/core/Access.js"
import WaveAuth from "./wave.js"
import signinWithPasskey from "./passkey.js"

function shortenepub(epub) {
    if (!epub || epub.length <= 12) return epub || ""
    return `${epub.slice(0, 5)}...${epub.slice(-5)}`
}

export class AUTHENTICATE extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.events = new Events(this)
        this.subscriptions = []
        this.waveauth = new WaveAuth()
        this.state = "neutral"
        this.onwave = this.onwave.bind(this)
        this.onrequestbtn = this.onrequestbtn.bind(this)
        this.onstopbtn = this.onstopbtn.bind(this)
        this.onpasskeybtn = this.onpasskeybtn.bind(this)
        this.stop = this.stop.bind(this)
    }

    connectedCallback() {
        this.$wave = this.shadowRoot.querySelector("#wave")
        this.$vis = this.shadowRoot.querySelector("#vis")
        this.$requestbtn = this.shadowRoot.querySelector("#request-btn")
        this.$stopbtn = this.shadowRoot.querySelector("#stop-btn")
        this.$passkeybtn = this.shadowRoot.querySelector("#passkey-btn")
        this.$epub = this.shadowRoot.querySelector("#epub")
        this.$msg = this.shadowRoot.querySelector("#msg")
        this.$requestbtn.addEventListener("click", this.onrequestbtn)
        this.$stopbtn.addEventListener("click", this.onstopbtn)
        this.$passkeybtn.addEventListener("click", this.onpasskeybtn)
        this.subscriptions.push(
            () => this.$requestbtn.removeEventListener("click", this.onrequestbtn),
            () => this.$stopbtn.removeEventListener("click", this.onstopbtn),
            () => this.$passkeybtn.removeEventListener("click", this.onpasskeybtn),
            this.$wave.events.on("message", this.onwave),
            this.$wave.events.on("analyser", (e) => this.$vis?.setanalyser(e.detail?.analyser ?? null))
        )
        this.setstate("neutral")
        this.initpair()
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
        this.stop()
    }

    async initpair() {
        const { sea } = globalThis
        if (!sea?.pair) return
        const pair = await sea.pair()
        this.waveauth.session = pair
        if (this.$epub) this.$epub.textContent = shortenepub(pair.epub)
    }

    setstate(state) {
        this.state = state
        this.$requestbtn.hidden = state !== "neutral"
        this.$stopbtn.hidden = state === "neutral"
        if (this.$msg) this.$msg.textContent = ""
    }

    done(response) {
        this.events.emit("done", { response }, { bubbles: true, composed: true })
    }

    async onwave(event) {
        const { parsed } = event?.detail || {}
        const result = await this.waveauth.handle(parsed)
        if (!result) return
        if (result.type === "deny") {
            if (this.$msg) this.$msg.textContent = "Access denied"
            return
        }
        if (result.type === "grant") {
            this.stop()
            wave({ seed: result.seed }).then((response) => this.done(response))
        }
    }

    onrequestbtn() {
        this.setstate("listening")
        this.waveauth.request(this.$wave).catch((error) => {
            this.setstate("neutral")
            console.error(error)
        })
    }

    onstopbtn() {
        this.stop()
    }

    onpasskeybtn() {
        signinWithPasskey().then((response) => this.done(response))
    }

    stop() {
        this.waveauth.reset()
        this.$wave?.stop?.()
        this.setstate("neutral")
        this.initpair()
    }
}

customElements.define("ui-authenticate", AUTHENTICATE)
export default AUTHENTICATE
