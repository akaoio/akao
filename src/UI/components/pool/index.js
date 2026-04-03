import { render } from "/core/UI.js"
import { Context } from "/core/Context.js"
import { Lives } from "/core/Stores.js"
import { events } from "/core/Events.js"
import { fiatValue } from "/core/Utils/contracts.js"
import { formatNumber, beautifyNumber } from "/core/Utils.js"
import template from "./template.js"

const formatrate = (rate) => {
    if (!rate) return "0"
    return rate < 0.0001 ? rate.toExponential(4) : rate < 1 ? rate.toFixed(6) : rate.toFixed(2)
}

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
        const fiat = Context.get("fiat")?.code || "USD"
        const locale = Context.get("locale")?.code || "en"
        const fmt = (n) => new Intl.NumberFormat(locale, { style: "currency", currency: fiat, notation: "compact" }).format(n)

        let price0, price1
        if ($t0 && t0?.configs) {
            $t0.dataset.symbol = t0.configs.symbol || ""
            $t0.dataset.name = t0.configs.name || ""
            $t0.dataset.amount = beautifyNumber(formatNumber(t0.quantity || 0, 4))
            price0 = await fiatValue({ chain, currency: t0.configs, amount: t0.quantity || 0, fiat })
            $t0.dataset.fiat = price0 > 0 ? fmt(price0) : ""
        }
        if ($t1 && t1?.configs) {
            $t1.dataset.symbol = t1.configs.symbol || ""
            $t1.dataset.name = t1.configs.name || ""
            $t1.dataset.amount = beautifyNumber(formatNumber(t1.quantity || 0, 4))
            price1 = await fiatValue({ chain, currency: t1.configs, amount: t1?.quantity || 0, fiat })
            $t1.dataset.fiat = price1 > 0 ? fmt(price1) : ""
        }

        const rate01 = pool.pairs?.[t0?.address]?.[t1?.address]
        const rate10 = pool.pairs?.[t1?.address]?.[t0?.address]
        const $rate0 = this.shadowRoot.querySelector("#rate0")
        const $rate1 = this.shadowRoot.querySelector("#rate1")
        if ($rate0) {
            $rate0.textContent = rate01 > 0 && t0?.configs && t1?.configs
                ? `1 ${t0.configs.name} = ${formatrate(rate01)} ${t1.configs.name}`
                : ""
        }
        if ($rate1) {
            $rate1.textContent = rate10 > 0 && t0?.configs && t1?.configs
                ? `1 ${t1.configs.name} = ${formatrate(rate10)} ${t0.configs.name}`
                : ""
        }

        const $tvl = this.shadowRoot.querySelector(".tvl")
        if ($tvl) {
            const tvl = (price0 || 0) + (price1 || 0)
            $tvl.textContent = tvl > 0 ? fmt(tvl) : ""
        }

        const $dex = this.shadowRoot.querySelector(".badge.dex")
        if ($dex) $dex.textContent = `${pool.dex || ""} ${pool.version || ""}`

        const $chain = this.shadowRoot.querySelector(".badge.chain")
        if ($chain) $chain.textContent = `Chain ${chain}`
    }
}

customElements.define("ui-pool", POOL)

export default POOL
