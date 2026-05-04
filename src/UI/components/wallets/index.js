/* eslint-disable curly */
import template from "./template.js"
import { Access } from "/core/Access.js"
import { render } from "/core/UI.js"
import { Chains, Lives, Wallets } from "/core/Stores.js"
import States from "/core/States.js"
import { Context } from "/core/Context.js"
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
            address: null,
            balance: null
        })
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.subs = []
        this.step = 5
        this.change = this.change.bind(this)

        // Ephemeral wallet index/total used when data-no-persist is set (swap/withdraw routes)
        this._ephemeralId = 0
        this._ephemeralTotal = null
        // Wallet being previewed (selected but not yet committed); null = resting
        this._previewId = null
        // Unsubscribe fn for the active Zen wallet-label subscription
        this._labelUnsub = null
        // Last persisted label value — used to revert the input on blur/Escape
        this._labelSaved = ""
    }

    get id() {
        if (this.dataset.noPersist !== undefined) return this._ephemeralId
        return logic.id()
    }

    set id(value) {
        if (this.dataset.noPersist !== undefined) { this._ephemeralId = Number(value); return Number(value) }
        return logic.setid(value, this.step, this.total)
    }

    get total() {
        if (this.dataset.noPersist !== undefined) return this._ephemeralTotal ?? (logic.total() || 1)
        return Number(typeof this.dataset.total !== "undefined" ? this.dataset.total : logic.total() || 1)
    }

    set total(value) {
        if (this.dataset.noPersist !== undefined) { this._ephemeralTotal = Number(value); return Number(value) }
        return logic.settotal(value)
    }

    async connectedCallback() {
        // ── DOM refs ──────────────────────────────────────────────
        this.$identicons = this.shadowRoot.querySelector("ui-identicons")
        this.$assetSelect = this.shadowRoot.querySelector("#asset-select")
        this.$address = this.shadowRoot.querySelector("#address")
        this.$copyBtn = this.shadowRoot.querySelector("#copy-btn")
        this.$balance = this.shadowRoot.querySelector("#balance")
        this.$balanceSymbol = this.shadowRoot.querySelector("#balance-symbol")
        this.$walletNum = this.shadowRoot.querySelector("#wallet-num")
        this.$labelWrap = this.shadowRoot.querySelector("#label-wrap")
        this.$labelInput = this.shadowRoot.querySelector("#label-input")
        this.$labelConfirm = this.shadowRoot.querySelector("#label-confirm")
        this.$labelEdit = this.shadowRoot.querySelector("#label-edit")
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
        this.$labelEdit.addEventListener("click", () => this.$labelInput.focus())

        // ── Delete confirm modal ──────────────────────────────────
        this.$deleteConfirm.addEventListener("click", () => {
            this.$deleteModal.close()
            this._removeWallet()
        })

        // ── Token select change events ────────────────────────────
        this.$assetSelect.addEventListener("change", (e) => this.change({ target: e.detail }))

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

        const showCurrency = this.dataset.currency !== "false" && this.dataset.noAssetSelect === undefined
        const showChain = this.dataset.noAssetSelect === undefined

        currency = this.states.get("currency") || null
        chains = this.chains(currency)
        chain = chains.some((o) => o.value === this.states.get("chain")) ? this.states.get("chain") : null
        this.states.set({ currency, chain })

        if (showCurrency) {
            this.$assetSelect.setCurrencyOptions(this.currencies)
            this.$assetSelect.setCurrencyValue(currency)
            this.$assetSelect.setCurrencySelection(currency)
        }

        this.$assetSelect.setChainOptions(chains)
        this.$assetSelect.setChainValue(chain)
        this.$assetSelect.setChainSelection(chain)

        this.$assetSelect.reveal({ currency: showCurrency, chain: showChain })

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
                const chain = options.some((o) => o.value === this.states.get("chain")) ? this.states.get("chain") : null
                this.$assetSelect.setChainOptions(options)
                this.$assetSelect.setChainValue(chain)
                this.$assetSelect.setChainSelection(chain)
                this.$assetSelect.setCurrencyValue(value)
                this.$assetSelect.setCurrencySelection(value)
                this.states.set({ currency: value, chain })
                if (this.dataset.promptChain !== undefined) this.$assetSelect.$chainModal.showModal()
            } else {
                this.states.set({ [name]: value })

                if (name === "chain") {
                    this.$assetSelect.setChainValue(value)
                    this.$assetSelect.setChainSelection(value)
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

        if (chain && this.states.get("currency")) {
            const currency = logic.currency(wallet, this.states.get("currency"))
            const fiat = Context.get("fiat")?.code || "USD"
            const { raw } = await logic.balance({ wallet, currency, fiat, forex: Lives.forex, address })
            if (raw !== null) {
                const locale = Context.get("locale")?.code || "en"
                const formatted = formatBalance(raw, locale)
                this.$balance.textContent = formatted
                this.$balanceSymbol.textContent = currency.name || ""
                this.states.set({ balance: formatted })
            } else {
                this.states.set({ balance: null })
            }
        }
    }

    // ── External API ──────────────────────────────────────────────

    setCurrency(name) {
        this.$balance.textContent = ""
        this.$balanceSymbol.textContent = name || ""
        this.states.set({ currency: name || null })
        this.change()
    }

    // ── Getters ───────────────────────────────────────────────────

    get currencies() {
        return logic.currencies(Chains).map((c) => ({
            value: c.name,
            _name: c.name,
            _symbol: c.symbol
        }))
    }

    chains(currency = null) {
        return logic.chains(Chains, currency).map((c) => ({
            value: c.id,
            _name: c.name,
            _symbol: c.symbol,
            _standard: c.standard || null
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
        this.$labelEdit.hidden = true
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
        this.$walletPreviewActions.hidden = false
        this.$labelEdit.hidden = editing
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
        this.$walletPreviewActions.hidden = false
        this.$labelEdit.hidden = false
        this.$walletRemove.hidden = true
        this.$walletSwitch.hidden = true
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
            this.$labelEdit.hidden = true
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
                this.$labelEdit.hidden = true
                this.$walletSwitch.hidden = false
                this.$walletRemove.hidden = this._previewId !== this.total - 1
                this.$walletPreviewActions.hidden = false
            } else {
                this.$labelEdit.hidden = false
                this.$walletPreviewActions.hidden = false
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
