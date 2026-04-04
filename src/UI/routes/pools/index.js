import template from "./template.js"
import { html, render } from "/core/UI.js"
import { Context } from "/core/Context.js"
import { events } from "/core/Events.js"
import { Indexes, Lives } from "/core/Stores.js"
import logic from "./logic.js"

export class POOLS extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.subscriptions = []
        this.render = this.render.bind(this)
    }

    async connectedCallback() {
        this.$list = this.shadowRoot.querySelector("#list")
        this.$empty = this.shadowRoot.querySelector("#empty")

        // Load cached pool data from IDB into Lives.pools
        await this.loadcache()

        // Render whatever is in Lives.pools now
        this.render()

        this.subscriptions.push(
            events.on("Lives.pools", this.render),
            Context.on("fiat", this.render)
        )
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
    }

    async loadcache() {
        const cached = await Indexes.Lives.get("pools").once()
        const flat = logic.flatten(cached)
        if (!Object.keys(flat).length) return
        if (!Lives.pools) Lives.pools = {}
        for (const [chain, chainPools] of Object.entries(flat)) {
            if (!Lives.pools[chain]) Lives.pools[chain] = {}
            Object.assign(Lives.pools[chain], chainPools)
        }
    }

    render() {
        const rows = logic.filter(Lives.pools, Context.get("params"))
        if (!rows.length) {
            this.$list.innerHTML = ""
            this.$empty.removeAttribute("hidden")
            this.$empty.textContent = Context.get(["dictionary", "nopoolFound"]) || "No pools found"
            return
        }
        this.$empty.setAttribute("hidden", "")
        render(rows.map(({ chain, address }) => html`
            <ui-pool data-chain="${chain}" data-address="${address}"></ui-pool>
        `), this.$list)
    }
}

customElements.define("route-pools", POOLS)

export default POOLS
