import { render } from "/core/UI.js"
import { Context } from "/core/Context.js"
import { Lives, Chains } from "/core/Stores.js"
import { events } from "/core/Events.js"
import { formatNumber, beautifyNumber } from "/core/Utils.js"
import template from "./template.js"
import logic from "./logic.js"

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

        if ($t0 && t0?.configs) {
            $t0.dataset.symbol = t0.configs.symbol || ""
            $t0.dataset.name = t0.configs.name || ""
            $t0.dataset.amount = beautifyNumber(formatNumber(t0.quantity || 0, 4))
        }
        if ($t1 && t1?.configs) {
            $t1.dataset.symbol = t1.configs.symbol || ""
            $t1.dataset.name = t1.configs.name || ""
            $t1.dataset.amount = beautifyNumber(formatNumber(t1.quantity || 0, 4))
        }

        const [{ price0, price1, tvl, rate01, rate10 }, dex] = await Promise.all([
            logic.prices({ pool, fiat, forex: Lives.forex }),
            logic.dex(pool.dex),
        ])

        if ($t0) $t0.dataset.fiat = price0 > 0 ? fmt(price0) : ""
        if ($t1) $t1.dataset.fiat = price1 > 0 ? fmt(price1) : ""

        const $rate0 = this.shadowRoot.querySelector("#rate0")
        const $rate1 = this.shadowRoot.querySelector("#rate1")
        if ($rate0) {
            $rate0.textContent = rate01 > 0 && t0?.configs && t1?.configs
                ? `1 ${t0.configs.name} = ${logic.format(rate01)} ${t1.configs.name}`
                : ""
        }
        if ($rate1) {
            $rate1.textContent = rate10 > 0 && t0?.configs && t1?.configs
                ? `1 ${t1.configs.name} = ${logic.format(rate10)} ${t0.configs.name}`
                : ""
        }

        const $tvl = this.shadowRoot.querySelector(".tvl")
        if ($tvl) $tvl.textContent = tvl > 0 ? fmt(tvl) : ""

        const $dex = this.shadowRoot.querySelector(".badge.dex")
        if ($dex) {
            const $dexsvg = $dex.querySelector("ui-svg")
            if ($dexsvg) $dexsvg.dataset.src = dex?.symbol ? `/images/cryptos/${dex.symbol}` : ""
            const $version = $dex.querySelector(".version")
            if ($version) $version.textContent = pool.version || ""
        }

        const $chain = this.shadowRoot.querySelector(".badge.chain")
        if ($chain) {
            const chainsymbol = Chains[chain]?.configs?.symbol
            const $chainsvg = $chain.querySelector("ui-svg")
            if ($chainsvg) $chainsvg.dataset.src = chainsymbol ? `/images/cryptos/${chainsymbol}` : ""
        }
    }
}

customElements.define("ui-pool", POOL)

export default POOL
