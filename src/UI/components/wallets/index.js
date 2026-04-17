/* eslint-disable curly */
import template from "./template.js"
import { Access } from "/core/Access.js"
import { html, render } from "/core/UI.js"
import { Chains, Lives, Wallets } from "/core/Stores.js"
import States from "/core/States.js"
import { Context } from "/core/Context.js"
import SELECT from "/UI/components/select/index.js"
import { notify, formatBalance } from "/core/Utils.js"
import { sha256 } from "/core/Utils/crypto.js"
import logic from "./logic.js"

export class WALLETS extends HTMLElement {
    static module = import.meta.url

    constructor() {
        super()
        this.template = template // Store for HMR
        this.states = new States({
            chain: Context.get("params")?.chain || null,
            currency: Context.get("params")?.currency || null,
            address: null
        })
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.subs = []
        this.step = 5
        this.change = this.change.bind(this)

        // Holds the current chain options array so trigger/modal can look up names
        this._chainOptions = []
        // Wallet being previewed (selected but not yet committed); null = resting
        this._previewId = null
        // Unsubscribe fn for the active Zen wallet-label subscription
        this._labelUnsub = null
        // Last persisted label value — used to revert the input on blur/Escape
        this._labelSaved = ""
    }

    get id() {
        return logic.id()
    }

    set id(value) {
        return logic.setid(value, this.step, this.total)
    }

    get total() {
        return Number(typeof this.dataset.total !== "undefined" ? this.dataset.total : logic.total() || 1)
    }

    set total(value) {
        return logic.settotal(value)
    }

    async connectedCallback() {
        // ── DOM refs ──────────────────────────────────────────────
        this.$identicons = this.shadowRoot.querySelector("ui-identicons")
        this.$chainTrigger = this.shadowRoot.querySelector("#chain-trigger")
        this.$chainIcon = this.shadowRoot.querySelector("#chain-icon")
        this.$chainName = this.shadowRoot.querySelector("#chain-name")
        this.$chainModal = this.shadowRoot.querySelector("#chain-modal")
        this.$chainList = this.shadowRoot.querySelector("#chain-list")
        this.$address = this.shadowRoot.querySelector("#address")
        this.$copyBtn = this.shadowRoot.querySelector("#copy-btn")
        this.$balance = this.shadowRoot.querySelector("#balance")
        this.$balanceSymbol = this.shadowRoot.querySelector("#balance-symbol")
        this.$walletNum = this.shadowRoot.querySelector("#wallet-num")
        this.$labelWrap = this.shadowRoot.querySelector("#label-wrap")
        this.$labelInput = this.shadowRoot.querySelector("#label-input")
        this.$labelConfirm = this.shadowRoot.querySelector("#label-confirm")
        this.$walletPreviewActions = this.shadowRoot.querySelector("#wallet-preview-actions")
        this.$walletRemove = this.shadowRoot.querySelector("#wallet-remove")
        this.$walletSwitch = this.shadowRoot.querySelector("#wallet-switch")
        this.$deleteModal = this.shadowRoot.querySelector("#delete-modal")
        this.$deleteModalBody = this.shadowRoot.querySelector("#delete-modal-body")
        this.$deleteConfirm = this.shadowRoot.querySelector("#delete-confirm")

        // ── Seed helper ───────────────────────────────────────────
        const seed = async () => {
            const s = await logic.seed()
            if (s != null) this.$identicons.dataset.seed = s
        }

        // ── Subscriptions ─────────────────────────────────────────
        this.subs.push(
            // Identicons: clicking a different wallet enters preview; clicking the
            // current wallet dismisses any active preview
            this.$identicons.events.on("select", ({ detail: { id } }) => {
                if (id === this.id) this._cancelPreview()
                else this._previewWallet(id)
            }),
            this.$identicons.events.on("add", () => {
                const MAX_WALLETS = 20
                if (this.total >= MAX_WALLETS) return
                const newId = this.total // index of the slot about to be created
                this.total += 1
                this.$identicons.addDisabled = this.total >= MAX_WALLETS
                this._commitWallet(newId) // select it — sets $id before render fires
                this.$identicons.render(true) // scroll to $id once created
            }),

            // Auth state changes
            Access.on("authenticated", async ({ value }) => {
                this.toggleAttribute("hidden", !value)
                if (value) {
                    await seed()
                    this.change()
                    this._subscribeLabel(this.id)
                } else {
                    this.states.set({ address: null })
                    this.$identicons.clear()
                    if (this._labelUnsub) {
                        this._labelUnsub()
                        this._labelUnsub = null
                    }
                    if (this.$labelInput) this.$labelInput.value = ""
                }
            }),
            // External wallet change (e.g. another tab) — cancel any preview
            Access.on("wallet", (e) => {
                this._cancelPreview()
                this.change(e)
            })
        )

        // ── Preview action buttons ────────────────────────────────
        this.$walletSwitch.addEventListener("click", () => this._commitWallet(this._previewId))
        this.$walletRemove.addEventListener("click", () => this._promptDeleteWallet())

        // ── Delete confirm modal ──────────────────────────────────
        this.$deleteConfirm.addEventListener("click", () => {
            this.$deleteModal.close()
            this._removeWallet()
        })

        // ── Chain trigger opens the modal ─────────────────────────
        this.$chainTrigger.addEventListener("click", () => this.$chainModal.showModal())

        // ── Address copy button ───────────────────────────────────
        this.$copyBtn.addEventListener("click", async () => {
            const full = this.states.get("address")
            if (!full) return
            await navigator.clipboard.writeText(full)
            this.$copyBtn.classList.add("copied")
            setTimeout(() => this.$copyBtn.classList.remove("copied"), 600)
            notify({ content: Context.get(["dictionary", "copiedToClipboard"]), autoClose: true })
        })

        // ── Wallet label editor ───────────────────────────────────
        this._initLabelEditor()

        this.toggleAttribute("hidden", !Access.get("authenticated"))

        // ── Load currency contracts for all chains ─────────────────
        for (const chain of Object.values(Chains)) {
            if (!chain.configs?.currencies?.length) continue
            if (chain.configs?.currencies?.length !== Object.values(chain?.currencies)?.length) await chain.load()
        }

        let currency, chains, chain

        if (this.dataset.currency === "false") {
            // Swap route: show all chains, no currency selector
            chains = Object.values(Chains).map((c) => ({
                label: html`
                    <ui-svg class="icon" data-src="/images/cryptos/${c.configs?.symbol || ""}" />
                    ${c.configs?.name || c.id}
                `,
                value: c.id,
                _name: c.configs?.name || c.id,
                _symbol: c.configs?.symbol || ""
            }))
            chain = this.states.get("chain") || chains[0]?.value || null
            this.states.set({ chain })
        } else {
            // Deposit/withdraw: currency selector + filtered chains
            currency = this.states.get("currency") || this.currencies[0]?.value || null
            chains = this.chains(currency)
            chain = chains.some((o) => o.value === this.states.get("chain")) ? this.states.get("chain") : chains[0]?.value || null
            this.states.set({ currency: currency, chain: chain })
        }

        // ── Build chain modal list and sync the trigger button ─────
        this._buildChainModal(chains)

        // ── Currency select (deposit/withdraw only) ────────────────
        if (this.dataset.currency !== "false") {
            this.$currencies = new SELECT({
                name: "currency",
                options: this.currencies,
                placeholder: "dictionary.currency",
                selected: currency,
                change: this.change
            })
            render(this.$currencies, this.shadowRoot.querySelector("#currencies"), { append: true })
        }

        this.$identicons.addDisabled = this.total >= 20
        if (Access.get("authenticated")) seed().then(() => this.change())
    }

    disconnectedCallback() {
        this.subs.forEach((off) => off())
        if (this._labelUnsub) {
            this._labelUnsub()
            this._labelUnsub = null
        }
    }

    async change(event) {
        if (!Access.get("authenticated")) return

        if (event?.target) {
            const { name, value } = event.target

            if (name === "currency") {
                // Currency changed — rebuild chain list to reflect supported chains
                const options = this.chains(value)
                const chain = options.some((o) => o.value === this.states.get("chain")) ? this.states.get("chain") : options[0]?.value || null
                this._buildChainModal(options)
                this.states.set({ currency: value, chain })
            } else {
                this.states.set({ [name]: value })

                if (name === "chain") {
                    // Keep trigger button and modal selection in sync
                    this._updateChainTrigger(value)
                    this._updateChainListSelection(value)
                }
            }
        }

        this.$identicons.id = this.id
        this.$identicons.total = this.total
        if (this._previewId === null) this.$identicons.savedId = this.id

        if (this.$walletNum) this.$walletNum.textContent = `#${(this._previewId ?? this.id) + 1}`

        this._subscribeLabel(this._previewId ?? this.id)

        const chain = this.states.get("chain")
        const wallet = Wallets[chain]

        // Derive address for the displayed wallet id (preview or committed)
        const displayId = this._previewId ?? this.id
        const address = (await this._addressForId(displayId, chain)) || null
        this.states.set({ address })

        // Truncated address shown in the field; full address kept in title for tooltip
        this.$address.textContent = address ? this._truncateAddress(address) : ""
        this.$address.title = address || ""
        this.$copyBtn.disabled = !address

        if (this.dataset.currency === "false") {
            // Swap route: show native chain balance
            const chainObj = Chains[chain]
            if (chainObj && address) {
                const raw = await chainObj.balance({ address, currency: null })
                if (raw != null) {
                    const locale = Context.get("locale")?.code || "en"
                    this.$balance.textContent = formatBalance(raw, locale)
                    this.$balanceSymbol.textContent = chainObj.currencies?.native?.name || ""
                }
            }
            return
        }

        if (chain && this.states.get("currency")) {
            const currency = logic.currency(wallet, this.states.get("currency"))
            const fiat = Context.get("fiat")?.code || "USD"
            const { raw } = await logic.balance({ wallet, currency, fiat, forex: Lives.forex, address })
            if (raw !== null) {
                const locale = Context.get("locale")?.code || "en"
                this.$balance.textContent = formatBalance(raw, locale)
                this.$balanceSymbol.textContent = currency.symbol || ""
            }
        }
    }

    // ── Getters ───────────────────────────────────────────────────

    get currencies() {
        return logic.currencies(Chains).map((c) => ({
            label: html`
                <ui-svg class="icon" data-src="/images/cryptos/${c.symbol}" />
                ${c.name}
            `,
            value: c.name,
            _name: c.name,
            _symbol: c.symbol
        }))
    }

    chains(currency = null) {
        return logic.chains(Chains, currency).map((c) => ({
            label: html`
                <ui-svg class="icon" data-src="/images/cryptos/${c.symbol}" />
                ${c.name}
            `,
            value: c.id,
            _name: c.name,
            _symbol: c.symbol
        }))
    }

    // ── Wallet selection ──────────────────────────────────────────

    /** Show DELETE + SWITCH for a previewed (not-yet-committed) wallet. */
    _previewWallet(id) {
        const editing = this.$labelWrap.classList.contains("editing")
        this._previewId = id
        this.$identicons.id = id
        this.$identicons.previewId = id
        this.$walletPreviewActions.hidden = false
        this.$walletSwitch.hidden = editing
        this.$walletRemove.hidden = id !== this.total - 1
        this.$labelInput.disabled = true
        this.$address.textContent = ""
        this.$balance.textContent = ""
        this.$balanceSymbol.textContent = ""
        this.change()
    }

    /** Dismiss the preview and snap back to the committed wallet. */
    _cancelPreview() {
        if (this._previewId === null) return
        const editing = this.$labelWrap.classList.contains("editing")
        this._previewId = null
        this.$identicons.id = this.id
        this.$identicons.previewId = null
        this.$walletPreviewActions.hidden = !editing
        this.$walletRemove.hidden = true
        this.$walletSwitch.hidden = true
        this.$labelInput.disabled = false
        this.$address.textContent = ""
        this.$balance.textContent = ""
        this.$balanceSymbol.textContent = ""
        this.change()
    }

    /** Commit a wallet id — write to Access/localStorage, clear preview, flash border. */
    _commitWallet(id) {
        if (id === null || id === undefined) return
        this.id = id
        this.$identicons.savedId = id
        this._previewId = null
        this.$identicons.previewId = null
        this.$walletPreviewActions.hidden = true
        this.$walletRemove.hidden = true
        this.$walletSwitch.hidden = false
        this.$labelInput.disabled = false
        this.$identicons.flashSelected()
        this.change()
    }

    /** Open the confirm modal before deleting. */
    _promptDeleteWallet() {
        const idx = this._previewId ?? this.id
        const label = this.$labelInput?.value.trim()
        const name = label || `#${idx + 1}`
        this.$deleteModalBody.textContent = `Delete wallet ${name}?`
        this.$deleteModal.showModal()
    }

    /**
     * Remove the previewed wallet slot (decrement total by 1).
     * Shows an error notification if the operation fails.
     */
    _removeWallet() {
        const MIN_WALLETS = 1
        if (this.total <= MIN_WALLETS) {
            notify({ content: Context.get(["dictionary", "cannotDeleteLastWallet"]) || "Cannot delete the last wallet", autoClose: true })
            return
        }
        try {
            this.total -= 1
            this._previewId = null
            this.$identicons.previewId = null
            this.$walletPreviewActions.hidden = true
            if (this.id >= this.total) {
                this._commitWallet(this.total - 1)
            } else {
                this.$identicons.id = this.id
                this.$identicons.savedId = this.id
                this.change()
            }
            this.$identicons.addDisabled = false
        } catch {
            notify({ content: Context.get(["dictionary", "deleteWalletFailed"]) || "Failed to delete wallet", autoClose: true })
        }
    }

    // ── Wallet label persistence (Zen-backed) ────────────────────

    /**
     * Subscribe to live Zen updates for the given wallet index.
     * Tears down any prior subscription first so only one is ever active.
     * Does not update the input while the user is actively typing.
     */
    _subscribeLabel(id) {
        if (this._labelUnsub) {
            this._labelUnsub()
            this._labelUnsub = null
        }
        const pub = Access.get("pair")?.pub
        if (!pub || !this.$labelInput) return
        this._labelUnsub = logic.subscribeLabel(pub, id, (v) => {
            if (document.activeElement === this.$labelInput) return
            this._labelSaved = v || ""
            this.$labelInput.value = this._labelSaved
        })
    }

    /**
     * Wire up the label input + confirm button.
     * - On focus: SWAP hides, SAVE appears in its place; preview-actions container
     *   becomes visible so the SAVE button has a slot to render in.
     * - On blur/Escape: revert input and restore the previous button state.
     * - Only the SAVE button (or Enter) persists the value.
     */
    _initLabelEditor() {
        if (!this.$labelInput || !this.$labelConfirm) return

        this.$labelInput.placeholder = Context.get(["dictionary", "walletName"]) || "Name this wallet"

        const showConfirm = () => {
            this.$walletSwitch.hidden = true
            this.$labelConfirm.hidden = false
            this.$walletPreviewActions.hidden = false
            this.$labelWrap.classList.add("editing")
        }
        const hideConfirm = () => {
            this.$labelConfirm.hidden = true
            this.$labelWrap.classList.remove("editing")
            // Restore preview-actions visibility to match current preview state
            if (this._previewId !== null) {
                this.$walletSwitch.hidden = false
                this.$walletRemove.hidden = this._previewId !== this.total - 1
                this.$walletPreviewActions.hidden = false
            } else {
                this.$walletPreviewActions.hidden = true
            }
        }

        const revert = () => {
            this.$labelInput.value = this._labelSaved
            hideConfirm()
        }

        let _committing = false

        const commit = async () => {
            if (_committing) return
            _committing = true
            const label = this.$labelInput.value.trim()
            const pair = Access.get("pair")
            hideConfirm()
            this.$labelInput.blur()
            if (!pair || label === this._labelSaved) {
                _committing = false
                return
            }
            try {
                this.$labelInput.classList.add("saving")
                await logic.writeLabel(pair, this.id, label)
                this._labelSaved = label
                this.$labelInput.classList.replace("saving", "saved")
                setTimeout(() => this.$labelInput.classList.remove("saved"), 1000)
            } catch {
                this.$labelInput.classList.remove("saving")
            } finally {
                _committing = false
            }
        }

        this.$labelInput.addEventListener("focus", showConfirm)
        this.$labelInput.addEventListener("blur", () => {
            // Skip revert if commit already handled it
            setTimeout(() => {
                if (!_committing) revert()
            }, 150)
        })
        this.$labelInput.addEventListener("keydown", (e) => {
            if (e.key === "Enter") {
                e.preventDefault()
                commit()
            }
            if (e.key === "Escape") {
                e.preventDefault()
                revert()
                this.$labelInput.blur()
            }
        })

        this.$labelConfirm.addEventListener("mousedown", (e) => e.preventDefault()) // keep input focused
        this.$labelConfirm.addEventListener("click", commit)
    }

    // ── Chain modal helpers ───────────────────────────────────────

    /**
     * Populate the chain picker modal with a fresh set of options and sync
     * the trigger button to the currently selected chain.
     */
    _buildChainModal(options) {
        this._chainOptions = options
        this.$chainList.innerHTML = ""

        for (const opt of options) {
            const li = document.createElement("li")
            li.dataset.value = opt.value

            // Chain icon
            const icon = document.createElement("ui-svg")
            if (opt._symbol) icon.dataset.src = `/images/cryptos/${opt._symbol}`

            // Chain name
            const name = document.createElement("span")
            name.textContent = opt._name || opt.value

            li.append(icon, name)

            // Clicking a row selects the chain and closes the modal
            li.addEventListener("click", () => {
                this.$chainModal.close()
                this.change({ target: { name: "chain", value: opt.value } })
            })

            this.$chainList.append(li)
        }

        // Sync trigger button and highlight active row
        this._updateChainTrigger(this.states.get("chain"))
        this._updateChainListSelection(this.states.get("chain"))
    }

    /**
     * Update the chain trigger button to reflect the currently selected chain
     * (icon, name text, and data-has-value attribute for CSS show/hide).
     */
    _updateChainTrigger(chainId) {
        const opt = this._chainOptions?.find((o) => o.value === chainId)
        if (this.$chainIcon) this.$chainIcon.dataset.src = opt?._symbol ? `/images/cryptos/${opt._symbol}` : ""
        if (this.$chainName) this.$chainName.textContent = opt?._name || opt?.value || ""
        this.$chainTrigger?.toggleAttribute("data-has-value", !!opt)
    }

    /**
     * Set data-selected="true" on the active chain row so the modal list
     * highlights it in green.
     */
    _updateChainListSelection(chainId) {
        this.$chainList?.querySelectorAll("li").forEach((li) => {
            li.dataset.selected = li.dataset.value === chainId ? "true" : "false"
        })
    }

    // ── Address helpers ───────────────────────────────────────────

    /**
     * Derive the public address for any wallet index without mutating Access.
     * Mirrors the Wallet class: seed = sha256(priv + id), then chain.public(chain.private(seed)).
     * Returns null if the key material isn't available.
     */
    async _addressForId(id, chainId) {
        const priv = Access.get("pair")?.priv
        const chain = Chains[chainId]
        if (!priv || !chain) return null
        try {
            const seed = sha256(priv + id)
            return chain.public(chain.private(seed)) || null
        } catch {
            return null
        }
    }

    /**
     * Return a truncated form of a 42-char hex address: 0x1a2b…ef3f
     * Full address is preserved in the element's title attribute for hover/a11y.
     */
    _truncateAddress(addr) {
        if (!addr || addr.length < 12) return addr || ""
        return `${addr.slice(0, 6)}\u2026${addr.slice(-4)}`
    }
}

customElements.define("ui-wallets", WALLETS)
export default WALLETS
