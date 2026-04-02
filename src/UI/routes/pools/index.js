import template from "./template.js"
import { html, render } from "/core/UI.js"
import { Context } from "/core/Context.js"
import { events } from "/core/Events.js"
import { Indexes, Lives } from "/core/Stores.js"
import { merge } from "/core/Utils.js"

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
        if (!cached || typeof cached !== "object") return
        // Flatten IDB structure: { "1.UniswapV2": { "0x...": data } } → Lives.pools[1]["0x..."]
        if (!Lives.pools) Lives.pools = {}
        for (const [dexid, pools] of Object.entries(cached)) {
            if (!pools || typeof pools !== "object") continue
            for (const [address, data] of Object.entries(pools)) {
                if (!data || typeof data !== "object") continue
                const chain = data.chain
                if (!chain) continue
                if (!Lives.pools[chain]) Lives.pools[chain] = {}
                merge(Lives.pools[chain], { [address]: data })
            }
        }
    }

    render() {
        const pools = Lives.pools
        if (!pools) return

        const params = Context.get("params") || {}
        const filterChain = params.chain ? Number(params.chain) : null
        const filterDex = params.dex ? params.dex.toLowerCase() : null

        const rows = []
        for (const [chain, chainPools] of Object.entries(pools)) {
            if (filterChain && Number(chain) !== filterChain) continue
            for (const [address, pool] of Object.entries(chainPools)) {
                if (!pool?.token0 || !pool?.token1) continue
                if (filterDex && pool.dex?.toLowerCase() !== filterDex) continue
                rows.push({ chain: Number(chain), address, pool })
            }
        }

        if (!rows.length) {
            this.$list.innerHTML = ""
            this.$empty.removeAttribute("hidden")
            const key = Context.get(["dictionary", "nopoolFound"]) || "No pools found"
            this.$empty.textContent = key
            return
        }

        this.$empty.setAttribute("hidden", "")

        const items = rows.map(({ chain, address }) => html`
            <ui-pool data-chain="${chain}" data-address="${address}"></ui-pool>
        `)
        render(items, this.$list)
    }
}

customElements.define("route-pools", POOLS)

export default POOLS
