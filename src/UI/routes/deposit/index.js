/* eslint-disable curly */
import template from "./template.js"
import Route from "/core/UI/Route.js"
import { Access } from "/core/Access.js"
import { render, html } from "/core/UI.js"
import { Chains, Elements } from "/core/Stores.js"
import { Context } from "/core/Context.js"
import { notify } from "/core/Utils.js"
import { sha256 } from "/core/Utils/crypto.js"
import Logic from "/UI/components/wallets/logic.js"

export class DEPOSIT extends Route {
    static module = import.meta.url

    constructor() {
        super(template)
        this._currency = null
        this._chain = null
        this._address = null
        this._currencyOptions = []
        this._chainOptions = []
    }

    async onconnect() {
        // ── DOM refs ──────────────────────────────────────────────
        this.$selection = this.shadowRoot.querySelector("#selection")
        this.$coinBadge = this.shadowRoot.querySelector("#coin-badge")
        this.$coinIcon = this.shadowRoot.querySelector("#coin-icon")
        this.$coinName = this.shadowRoot.querySelector("#coin-name")
        this.$chainBadge = this.shadowRoot.querySelector("#chain-badge")
        this.$chainIcon = this.shadowRoot.querySelector("#chain-icon")
        this.$chainName = this.shadowRoot.querySelector("#chain-name")
        this.$coinModal = this.shadowRoot.querySelector("#coin-modal")
        this.$coinList = this.shadowRoot.querySelector("#coin-list")
        this.$chainModal = this.shadowRoot.querySelector("#chain-modal")
        this.$chainList = this.shadowRoot.querySelector("#chain-list")
        this.$qrWrap = this.shadowRoot.querySelector("#qr-section")
        this.$qr = this.shadowRoot.querySelector("#qr")
        this.$addressWrap = this.shadowRoot.querySelector("#address-wrap")
        this.$depositBody = this.shadowRoot.querySelector("#deposit-body")
        this.$addressBox = this.shadowRoot.querySelector("#address-box")
        this.$address = this.shadowRoot.querySelector("#address")
        this.$copyBtn = this.shadowRoot.querySelector("#copy-btn")
        this.$chainWarning = this.shadowRoot.querySelector("#chain-warning")

        // ── Load currency contracts ───────────────────────────────
        for (const chain of Object.values(Chains)) {
            if (!chain.configs?.currencies?.length) continue
            if (chain.configs.currencies.length !== Object.values(chain?.currencies).length) await chain.load()
        }

        // ── Build currency options ────────────────────────────────
        this._currencyOptions = Logic.currencies(Chains).map((c) => ({
            value: c.name,
            _name: c.name,
            _symbol: c.symbol,
            _meta: c.symbol ? c.symbol.replace(/-logo\.svg$/, "").replace(/-[^-]+$/, "").replace(/-/g, " ") : null
        }))

        // ── Seed from URL params ──────────────────────────────────
        const params = Context.get("params") || {}
        if (params.currency) {
            const match = this._currencyOptions.find((o) => o.value === params.currency)
            if (match) this._currency = match.value
        }
        if (params.chain) {
            this._chain = params.chain
        }

        // ── Wire modals ───────────────────────────────────────────
        this.$coinList.setOptions(this._currencyOptions)
        this.$coinList.setSelection(this._currency)
        this.$coinList.addEventListener("change", (e) => this._onCurrencyChange(e.detail.value))
        this.$chainList.addEventListener("change", (e) => this._onChainChange(e.detail.value))
        this.$coinModal.setRequired(true)
        this.$coinBadge.addEventListener("click", () => this.$coinModal.showModal())
        this.$chainBadge.addEventListener("click", () => {
            if (!this._currency) return
            this._renderChainModal(this._buildChainOptions(this._currency))
            this.$chainModal.showModal()
        })

        // ── Copy on click anywhere in address box ─────────────────
        this.$addressBox.addEventListener("click", async () => {
            if (!this._address) return
            await navigator.clipboard.writeText(this._address)
            this.$copyBtn.classList.add("copied")
            setTimeout(() => this.$copyBtn.classList.remove("copied"), 600)
            notify({ content: Context.get(["dictionary", "copiedToClipboard"]), autoClose: true })
        })

        // ── Auth gate ─────────────────────────────────────────────
        this.watch(Access, "authenticated", ({ value }) => {
            if (value) this._refresh()
            else this._clearAddress()
        })

        Elements.Access?.checkpoint()

        // ── Step sequencing on load ───────────────────────────────
        if (this._currency && this._chain) {
            // Both pre-filled from URL — go straight to address view
            this._syncBadges()
            this.$depositBody.classList.add("ready")
            if (Access.get("authenticated")) await this._refresh()
        } else if (this._currency) {
            // Coin known, prompt chain
            this._syncBadges()
            this._promptChain()
        } else {
            // Nothing known — brief delay so user registers the deposit page before modal opens
            setTimeout(() => this.$coinModal.showModal(), 150)
        }
    }

    // ── Step helpers ──────────────────────────────────────────────

    _promptChain() {
        const opts = this._buildChainOptions(this._currency)
        this._chainOptions = opts
        if (!opts.some((o) => o.value === this._chain)) this._chain = null
        this._renderChainModal(opts, /* required */ !this._chain)
        this.$chainModal.showModal()
    }

    // ── Coin modal ────────────────────────────────────────────────

    _onCurrencyChange(currency) {
        this.$coinModal.setRequired(false)
        this.$coinModal.close()
        this._currency = currency
        this.$coinList.setSelection(currency)
        this._clearAddress()
        this._promptChain()
        this._syncBadges()
    }

    // ── Chain modal ───────────────────────────────────────────────

    _buildChainOptions(currency) {
        return Logic.chains(Chains, currency).map((c) => ({
            value: c.id,
            _name: c.name,
            _symbol: c.symbol,
            _meta: c.standard || null
        }))
    }

    _renderChainModal(options, required = false) {
        this.$chainModal.setRequired(required)
        this._chainOptions = options
        this.$chainList.setOptions(options)
        this.$chainList.setSelection(this._chain)
    }

    _onChainChange(chainId) {
        this.$chainModal.setRequired(false)
        this.$chainModal.close()
        this._chain = chainId
        this.$chainList.setSelection(chainId)
        this._syncBadges()
        this.$depositBody.classList.add("ready")
        if (Access.get("authenticated")) this._refresh()
    }

    // ── Badges ────────────────────────────────────────────────────

    _syncBadges() {
        const coin = this._currencyOptions.find((o) => o.value === this._currency)
        const chain = this._chainOptions.find((o) => o.value === this._chain)

        if (this.$coinIcon) this.$coinIcon.dataset.src = coin?._symbol ? `/images/cryptos/${coin._symbol}` : ""
        if (this.$coinName) this.$coinName.textContent = coin?._name || coin?.value || ""
        if (this.$chainIcon) this.$chainIcon.dataset.src = chain?._symbol ? `/images/cryptos/${chain._symbol}` : ""
        if (this.$chainName) {
            const std = chain?._meta ? ` (${chain._meta})` : ""
            this.$chainName.textContent = chain ? `${chain._name || chain.value}${std}` : ""
        }

        this.$selection.hidden = !coin
        this.$chainBadge.toggleAttribute("data-has-value", !!chain)
        this.$chainBadge.toggleAttribute("data-empty", !chain)
        this.$chainWarning.hidden = !chain
    }

    // ── Address ───────────────────────────────────────────────────

    async _refresh() {
        const address = await this._deriveAddress()
        this._address = address || null
        this._showAddress(address)
    }

    async _deriveAddress() {
        const priv = Access.get("pair")?.priv
        const id = Number(Access.get("wallet")?.id || 0)
        const chain = Chains[this._chain]
        if (!priv || !chain) return null
        try {
            const seed = sha256(priv + id)
            return chain.public(chain.private(seed)) || null
        } catch {
            return null
        }
    }

    _showAddress(address) {
        const hasAddress = !!address

        // QR
        this.$qrWrap.hidden = !hasAddress
        if (hasAddress) this.$qr.dataset.value = address
        else this.$qr.removeAttribute("data-value")

        // Address with highlighted first-6 / last-6
        this.$addressWrap.hidden = !hasAddress
        if (address && this.$address) {
            const pre = address.slice(0, 6)
            const mid = address.slice(6, -6)
            const post = address.slice(-6)
            render(
                html`
                    <span class="addr-hi">${pre}</span>
                    <span class="addr-mid">${mid}</span>
                    <span class="addr-hi">${post}</span>
                `,
                this.$address
            )
        } else if (this.$address) {
            this.$address.innerHTML = ""
        }
    }

    _clearAddress() {
        this._address = null
        this._showAddress(null)
    }
}

customElements.define("route-deposit", DEPOSIT)
export default DEPOSIT
