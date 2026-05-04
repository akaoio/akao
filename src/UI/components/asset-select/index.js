import template from "./template.js"
import { render } from "/core/UI.js"
import { Context } from "/core/Context.js"

export class ASSET_SELECT extends HTMLElement {
    static module = import.meta.url

    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this._currencyOptions = []
        this._chainOptions = []
    }

    connectedCallback() {
        this.$currencyRow = this.shadowRoot.querySelector("#currency-row")
        this.$currencyTrigger = this.shadowRoot.querySelector("#currency-trigger")
        this.$currencyIcon = this.shadowRoot.querySelector("#currency-icon")
        this.$currencyName = this.shadowRoot.querySelector("#currency-name")
        this.$currencyPlaceholder = this.shadowRoot.querySelector("#currency-placeholder")
        this.$currencyModal = this.shadowRoot.querySelector("#currency-modal")
        this.$currencyList = this.shadowRoot.querySelector("#currency-list")

        this.$chainRow = this.shadowRoot.querySelector("#chain-row")
        this.$chainTrigger = this.shadowRoot.querySelector("#chain-trigger")
        this.$chainIcon = this.shadowRoot.querySelector("#chain-icon")
        this.$chainName = this.shadowRoot.querySelector("#chain-name")
        this.$chainPlaceholder = this.shadowRoot.querySelector("#chain-placeholder")
        this.$chainModal = this.shadowRoot.querySelector("#chain-modal")
        this.$chainList = this.shadowRoot.querySelector("#chain-list")

        this.$currencyPlaceholder.textContent = Context.get(["dictionary", "selectCoin"]) || ""
        this.$chainPlaceholder.textContent = Context.get(["dictionary", "selectNetwork"]) || ""

        this.$currencyTrigger.addEventListener("click", () => this.$currencyModal.showModal())
        this.$chainTrigger.addEventListener("click", () => this.$chainModal.showModal())

        this.$currencyList.addEventListener("change", (e) => {
            this.$currencyModal.close()
            this.dispatchEvent(new CustomEvent("change", { detail: { name: "currency", value: e.detail.value }, bubbles: true }))
        })
        this.$chainList.addEventListener("change", (e) => {
            this.$chainModal.close()
            this.dispatchEvent(new CustomEvent("change", { detail: { name: "chain", value: e.detail.value }, bubbles: true }))
        })
    }

    /** Called by the parent once setup is complete to reveal only the needed rows. */
    reveal({ currency = false, chain = false } = {}) {
        if (this.$currencyRow) this.$currencyRow.hidden = !currency
        if (this.$chainRow) this.$chainRow.hidden = !chain
    }

    // ── Currency ──────────────────────────────────────────────────

    setCurrencyOptions(options) {
        this._currencyOptions = options
        this.$currencyList.setOptions(options)
    }

    setCurrencyValue(value) {
        const opt = this._currencyOptions.find((o) => o.value === value)
        this.$currencyIcon.dataset.src = opt?._symbol ? `/images/cryptos/${opt._symbol}` : ""
        this.$currencyName.textContent = opt?._name || opt?.value || ""
        this.$currencyTrigger.toggleAttribute("data-has-value", !!opt)
    }

    setCurrencySelection(value) {
        this.$currencyList.setSelection(value)
    }

    // ── Chain ─────────────────────────────────────────────────────

    setChainOptions(options) {
        this._chainOptions = options
        this.$chainList.setOptions(options)
    }

    setChainValue(value) {
        const opt = this._chainOptions.find((o) => o.value === value)
        this.$chainIcon.dataset.src = opt?._symbol ? `/images/cryptos/${opt._symbol}` : ""
        if (this.$chainName) {
            const std = opt?._standard ? ` (${opt._standard})` : ""
            this.$chainName.textContent = opt ? `${opt._name || opt.value}${std}` : ""
        }
        this.$chainTrigger.toggleAttribute("data-has-value", !!opt)
    }

    setChainSelection(value) {
        this.$chainList.setSelection(value)
    }
}

customElements.define("ui-asset-select", ASSET_SELECT)
export default ASSET_SELECT
