import { render } from "/core/UI.js"
import { Context } from "/core/Context.js"
import { Lives } from "/core/Stores.js"
import { events } from "/core/Events.js"
import { fiatValue } from "/core/Utils/contracts.js"
import { formatNumber } from "/core/Utils.js"
import template from "./template.js"

export class POOL extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.subscriptions = []
        this.update = this.update.bind(this)
    }

    get chain() {
        return Number(this.dataset.chain)
    }

    get address() {
        return this.dataset.address
    }

    connectedCallback() {
        this.subscriptions.push(
            events.on("Lives.pools", this.update),
            events.on("Lives.forex", this.update),
            Context.on("fiat", this.update)
        )
        this.update()
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
    }

    async update() {
        const chain = this.chain
        const address = this.address
        if (!chain || !address) return

        const pool = Lives.pools?.[chain]?.[address]
        if (!pool) return

        const t0 = pool.token0
        const t1 = pool.token1

        const $t0 = this.shadowRoot.querySelector("#token0")
        const $t1 = this.shadowRoot.querySelector("#token1")
        if ($t0 && t0?.configs) {
            $t0.dataset.symbol = t0.configs.symbol || ""
            $t0.dataset.name = t0.configs.name || ""
        }
        if ($t1 && t1?.configs) {
            $t1.dataset.symbol = t1.configs.symbol || ""
            $t1.dataset.name = t1.configs.name || ""
        }

        const rate = pool.pairs?.[t0?.address]?.[t1?.address]
        const $rate = this.shadowRoot.querySelector(".rate")
        if ($rate) {
            if (rate > 0 && t0?.configs && t1?.configs) {
                $rate.textContent = `1 ${t0.configs.name} = ${formatNumber(rate)} ${t1.configs.name}`
            } else {
                $rate.textContent = ""
            }
        }

        const fiat = Context.get("fiat")?.code || "USD"
        const $tvl = this.shadowRoot.querySelector(".tvl")
        if ($tvl && t0?.configs) {
            const price0 = await fiatValue({ chain, currency: t0.configs, amount: t0.quantity || 0, fiat })
            const price1 = await fiatValue({ chain, currency: t1?.configs, amount: t1?.quantity || 0, fiat })
            const tvl = (price0 || 0) + (price1 || 0)
            $tvl.textContent = tvl > 0 ? `${fiat} ${formatNumber(tvl, 0)}` : ""
        }

        const $dex = this.shadowRoot.querySelector(".badge.dex")
        if ($dex) $dex.textContent = `${pool.dex || ""} ${pool.version || ""}`

        const $chain = this.shadowRoot.querySelector(".badge.chain")
        if ($chain) $chain.textContent = `Chain ${chain}`
    }
}

customElements.define("ui-pool", POOL)

export default POOL
