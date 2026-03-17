import template from "./template.js"
import { Statics } from "/core/Stores.js"
import { Context, setWhitelabel } from "/core/Context.js"
import { render } from "/core/UI.js"

export class WHITELABELS extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
    }

    connectedCallback() {
        const button = this.shadowRoot.querySelector("button")
        const whitelabels = Statics.whitelabels || []
        button.textContent = Context.get("whitelabel")

        this._onClick = () => {
            const current = Context.get("whitelabel")
            const index = whitelabels.findIndex((w) => w.code === current)
            const next = whitelabels[(index + 1) % whitelabels.length]
            setWhitelabel(next.code)
        }
        button.addEventListener("click", this._onClick)

        this.subscription = Context.on("whitelabel", ({ value }) => {
            button.textContent = value
        })
    }

    disconnectedCallback() {
        this.shadowRoot.querySelector("button").removeEventListener("click", this._onClick)
        this.subscription.off()
    }
}

customElements.define("ui-whitelabels", WHITELABELS)

export default WHITELABELS
