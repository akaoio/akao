import template from "./template.js"
import { render } from "/core/UI.js"

export class SWAP_FORM extends HTMLElement {
    static module = import.meta.url

    constructor() {
        super()
        this.template = template
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this._tokenMap = new Map()
        this._fromOpt = null
        this._toOpt = null
        this._onInput = this._onInput.bind(this)
        this._onFlip = this._onFlip.bind(this)
        this._onSubmit = this._onSubmit.bind(this)
    }

    connectedCallback() {
        const r = (id) => this.shadowRoot.querySelector(id)

        this.$amountIn   = r("#amount-in")
        this.$quoteOut   = r("#quote-out")
        this.$balanceIn  = r("#balance-in")
        this.$gas        = r("#gas")
        this.$error      = r("#error")
        this.$submit     = r("#submit")
        this.$slippage   = r("#slippage")
        this.$fromPicker = r("#from-picker")
        this.$toPicker   = r("#to-picker")
        this.$fromTrigger = r("#from-trigger")
        this.$toTrigger   = r("#to-trigger")

        this.$fromPicker.callback = (address) => {
            const opt = this._tokenMap.get(address)
            if (!opt) return
            this._fromOpt = opt
            this._updateTrigger(this.$fromTrigger, opt)
            if (!this._toOpt || !this.amountIn) this.submitReady = false
            this.onquote?.()
        }
        this.$toPicker.callback = (address) => {
            const opt = this._tokenMap.get(address)
            if (!opt) return
            this._toOpt = opt
            this._updateTrigger(this.$toTrigger, opt)
            if (!this._fromOpt || !this.amountIn) this.submitReady = false
            this.onquote?.()
        }

        this.$fromTrigger.addEventListener("click", () => this.$fromPicker.show())
        this.$toTrigger.addEventListener("click",   () => this.$toPicker.show())
        this.$amountIn.addEventListener("input", this._onInput)
        this.$submit.addEventListener("click", this._onSubmit)
        r("#flip-btn").addEventListener("click", this._onFlip)

        this.shadowRoot.querySelectorAll(".slippage-preset").forEach((btn) => {
            btn.addEventListener("click", () => {
                this.$slippage.value = btn.dataset.value
                this.shadowRoot.querySelectorAll(".slippage-preset").forEach(b => b.classList.remove("active"))
                btn.classList.add("active")
            })
        })
        const defaultPreset = this.shadowRoot.querySelector(`.slippage-preset[data-value="0.5"]`)
        if (defaultPreset) defaultPreset.classList.add("active")

        this.$slippage.addEventListener("input", () => {
            this.shadowRoot.querySelectorAll(".slippage-preset").forEach(b => b.classList.remove("active"))
        })
    }

    // ── Public API ────────────────────────────────────────────────────────────

    /** Replace the token list. Resets both pickers; re-selects from/to if still present. */
    set options(opts) {
        this._tokenMap = new Map(opts.map(o => [o.address, o]))
        const pickerOpts = opts.map(o => ({
            value:  o.address,
            icon:   o.configs?.symbol || "",
            symbol: o.configs?.name   || o.address,
        }))
        this.$fromPicker?.states.set({ options: pickerOpts })
        this.$toPicker?.states.set({ options: pickerOpts })

        // Re-select previously chosen tokens if still in the new list
        if (this._fromOpt) {
            const still = this._tokenMap.get(this._fromOpt.address)
            if (still) { this.$fromPicker.select(still.address); this._updateTrigger(this.$fromTrigger, still) }
            else this._clearTrigger(this.$fromTrigger, "from")
        }
        if (this._toOpt) {
            const still = this._tokenMap.get(this._toOpt.address)
            if (still) { this.$toPicker.select(still.address); this._updateTrigger(this.$toTrigger, still) }
            else this._clearTrigger(this.$toTrigger, "to")
        }
    }

    /** Enable/disable amount input and submit button (tied to wallet connection). */
    set enabled(val) {
        if (!this.$amountIn) return
        this.$amountIn.disabled = !val
        // When disabling, always disable submit too.
        // When enabling, only un-disable submit if the form has all required fields —
        // submitReady controls the CTA state once the user fills the form.
        if (!val) {
            this.$submit.toggleAttribute("disabled", true)
            this.clear()
        } else {
            const formReady = !!(this._fromOpt && this._toOpt && this.amountIn)
            this.$submit.toggleAttribute("disabled", !formReady)
        }
    }

    /** Select a token programmatically (e.g. from URL params). side = "from"|"to" */
    selectToken(side, address) {
        const opt = this._tokenMap.get(address)
        if (!opt) return
        if (side === "from") {
            this._fromOpt = opt
            this.$fromPicker?.select(opt.address)
            this._updateTrigger(this.$fromTrigger, opt)
        } else {
            this._toOpt = opt
            this.$toPicker?.select(opt.address)
            this._updateTrigger(this.$toTrigger, opt)
        }
    }

    get from()     { return this._fromOpt }
    get to()       { return this._toOpt }
    get amountIn() { return Number(this.$amountIn?.value) || 0 }
    get slippage() { return Number(this.$slippage?.value) || 0.5 }

    set quoteOut(text) { if (this.$quoteOut) this.$quoteOut.textContent = text ?? "0" }
    set balanceIn(text) { if (this.$balanceIn) this.$balanceIn.textContent = text ?? "" }
    set gas(text)  { if (this.$gas) this.$gas.textContent = text ?? "" }
    set error(text) { if (this.$error) this.$error.textContent = text ?? "" }
    set submitReady(val) { this.$submit?.toggleAttribute("disabled", !val) }

    clear() {
        if (this.$amountIn) this.$amountIn.value = ""
        this.quoteOut  = "0"
        this.gas       = ""
        this.error     = ""
        this.balanceIn = ""
    }

    // ── Private ───────────────────────────────────────────────────────────────

    _onInput() {
        if (!this.amountIn) this.submitReady = false
        this.onquote?.()
    }

    _onFlip() {
        const prevFrom = this._fromOpt
        const prevTo   = this._toOpt
        this._fromOpt = prevTo
        this._toOpt   = prevFrom
        if (this._fromOpt) { this.$fromPicker.select(this._fromOpt.address); this._updateTrigger(this.$fromTrigger, this._fromOpt) }
        else this._clearTrigger(this.$fromTrigger, "from")
        if (this._toOpt)   { this.$toPicker.select(this._toOpt.address);   this._updateTrigger(this.$toTrigger,   this._toOpt)   }
        else this._clearTrigger(this.$toTrigger, "to")
        if (this.$amountIn) this.$amountIn.value = ""
        this.quoteOut  = "0"
        this.gas       = ""
        this.error     = ""
        this.balanceIn = ""
        this.onflip?.()
    }

    _onSubmit() {
        if (this.$submit?.hasAttribute("disabled")) return
        this.onswap?.()
    }

    _updateTrigger(btn, opt) {
        const symbol = opt?.configs?.symbol || ""
        const name   = opt?.configs?.name   || opt?.address || ""
        const icon   = btn.querySelector(".trigger-icon")
        const value  = btn.querySelector(".trigger-value")
        if (icon)  icon.dataset.src = symbol ? `/images/cryptos/${symbol}` : ""
        if (value) value.textContent = name
        btn.toggleAttribute("data-has-value", !!name)
    }

    _clearTrigger(btn, side) {
        const icon  = btn.querySelector(".trigger-icon")
        const value = btn.querySelector(".trigger-value")
        if (icon)  icon.dataset.src = ""
        if (value) value.textContent = ""
        btn.removeAttribute("data-has-value")
        if (side === "from") this._fromOpt = null
        else this._toOpt = null
        this.submitReady = false
    }
}

customElements.define("ui-swap-form", SWAP_FORM)

export default SWAP_FORM
