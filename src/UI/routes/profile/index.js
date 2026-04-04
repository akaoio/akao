import template from "./template.js"
import { html, render } from "/core/UI.js"
import { Access } from "/core/Access.js"
import States from "/core/States.js"
import logic, { SOCIAL_PLATFORMS } from "./logic.js"

export class PROFILE extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.states = new States({ name: "", bio: "", links: {}, following: [], editing: false, editingBio: false, editingLinks: false })
        this.subscriptions = []
        this._followingSubs = []
        this._followingScope = null
        this._enterEditMode = this._enterEditMode.bind(this)
        this._exitEditMode = this._exitEditMode.bind(this)
        this._onKeydown = this._onKeydown.bind(this)
        this._onFollowSubmit = this._onFollowSubmit.bind(this)
        this._enterBioEditMode = this._enterBioEditMode.bind(this)
        this._exitBioEditMode = this._exitBioEditMode.bind(this)
        this._enterLinksEditMode = this._enterLinksEditMode.bind(this)
        this._exitLinksEditMode = this._exitLinksEditMode.bind(this)
    }

    connectedCallback() {
        const $ = (id) => this.shadowRoot.querySelector(id)

        // Wire name edit controls
        const $editBtn = $("#profile-name-edit")
        const $saveBtn = $("#profile-name-save")
        const $cancelBtn = $("#profile-name-cancel")
        const $input = $("#profile-name-input")

        $editBtn.addEventListener("click", this._enterEditMode)
        $saveBtn.addEventListener("click", () => this._exitEditMode(true))
        $cancelBtn.addEventListener("click", () => this._exitEditMode(false))
        $input.addEventListener("keydown", this._onKeydown)

        this.subscriptions.push(
            () => $editBtn.removeEventListener("click", this._enterEditMode),
            () => $saveBtn.removeEventListener("click", () => this._exitEditMode(true)),
            () => $cancelBtn.removeEventListener("click", () => this._exitEditMode(false)),
            () => $input.removeEventListener("keydown", this._onKeydown)
        )

        // Wire follow form
        const $form = $("#profile-follow-form")
        $form.addEventListener("submit", this._onFollowSubmit)
        this.subscriptions.push(() => $form.removeEventListener("submit", this._onFollowSubmit))

        // Wire bio edit controls
        const $bioEdit = $("#profile-bio-edit")
        const $bioSave = $("#profile-bio-save")
        const $bioCancel = $("#profile-bio-cancel")
        const $bioInput = $("#profile-bio-input")
        const $bioCount = $("#profile-bio-count")
        const updateBioCount = () => { $bioCount.textContent = 360 - $bioInput.value.length }
        $bioEdit.addEventListener("click", this._enterBioEditMode)
        $bioSave.addEventListener("click", () => this._exitBioEditMode(true))
        $bioCancel.addEventListener("click", () => this._exitBioEditMode(false))
        $bioInput.addEventListener("input", updateBioCount)
        this.subscriptions.push(
            () => $bioEdit.removeEventListener("click", this._enterBioEditMode),
            () => $bioSave.removeEventListener("click", () => this._exitBioEditMode(true)),
            () => $bioCancel.removeEventListener("click", () => this._exitBioEditMode(false)),
            () => $bioInput.removeEventListener("input", updateBioCount)
        )

        // Wire avatar edit button
        const $avatarEdit = $("#profile-avatar-edit")
        const $avatarPicker = $("#profile-avatar-picker")
        const togglePicker = () => {
            const opening = !$avatarPicker.classList.contains("is-open")
            $avatarPicker.classList.toggle("is-open")
            if (opening) {
                $avatarPicker.classList.add("is-loading")
                setTimeout(() => $avatarPicker.classList.remove("is-loading"), 1200)
            }
        }
        $avatarEdit.addEventListener("click", togglePicker)
        this.subscriptions.push(() => $avatarEdit.removeEventListener("click", togglePicker))

        // Wire links edit controls
        const $linksEdit = $("#profile-links-edit")
        const $linksSave = $("#profile-links-save")
        const $linksCancel = $("#profile-links-cancel")
        $linksEdit.addEventListener("click", this._enterLinksEditMode)
        $linksSave.addEventListener("click", () => this._exitLinksEditMode(true))
        $linksCancel.addEventListener("click", () => this._exitLinksEditMode(false))
        this.subscriptions.push(
            () => $linksEdit.removeEventListener("click", this._enterLinksEditMode),
            () => $linksSave.removeEventListener("click", () => this._exitLinksEditMode(true)),
            () => $linksCancel.removeEventListener("click", () => this._exitLinksEditMode(false))
        )

        // Subscribe to auth changes
        this.subscriptions.push(
            Access.on("authenticated", async ({ value }) => {
                this._applyAuthState(value)
                if (value) {
                    await this._loadName()
                    await this._updateHeroIdenticon()
                    await this._loadBio()
                    await this._loadLinks()
                    this._loadFollowing()
                } else {
                    this._exitEditMode(false)
                    this._exitBioEditMode(false)
                    this._exitLinksEditMode(false)
                    $("#profile-name").textContent = ""
                    $("#profile-bio").textContent = ""
                    this._followingScope?.off?.()
                    this._followingScope = null
                    this.states.set({ following: [] })
                    this._renderFollowing()
                    this._renderStats()
                }
            }),
            Access.on("avatar", async () => {
                if (logic.authenticated()) await this._updateHeroIdenticon()
            })
        )

        // Apply initial state (component may mount after auth is already set)
        const authenticated = logic.authenticated()
        this._applyAuthState(authenticated)
        if (authenticated) {
            this._loadName()
            this._updateHeroIdenticon()
            this._loadBio()
            this._loadLinks()
            this._loadFollowing()
        }
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
        this._followingSubs.forEach((off) => off())
        this._followingScope?.off?.()
    }

    _applyAuthState(authenticated) {
        const $ = (id) => this.shadowRoot.querySelector(id)
        const gate = $("#profile-gate")
        const sections = $("#profile-sections")
        const editBtn = $("#profile-name-edit")
        const authControls = $("#profile-auth-controls")
        const bioRow = $("#profile-bio-row")
        const statsRow = $("#profile-stats-row")
        const avatarEdit = $("#profile-avatar-edit")
        const avatarPicker = $("#profile-avatar-picker")

        if (gate) gate.style.display = authenticated ? "none" : "flex"
        if (sections) sections.style.display = authenticated ? "flex" : "none"
        if (editBtn) editBtn.style.display = authenticated ? "inline-flex" : "none"
        if (authControls) authControls.style.display = authenticated ? "flex" : "none"
        if (bioRow) bioRow.style.display = authenticated ? "flex" : "none"
        if (statsRow) statsRow.style.display = authenticated ? "flex" : "none"
        if (avatarEdit) avatarEdit.style.display = authenticated ? "flex" : "none"
        if (avatarPicker && !authenticated) avatarPicker.classList.remove("is-open")
    }

    async _updateHeroIdenticon() {
        const result = await logic.identiconseed()
        if (!result) return
        const { idSeed, h1, h2 } = result
        const glow = (h, a) => `hsl(${h}deg 100% 65% / ${a})`
        const $icon = this.shadowRoot.querySelector("#profile-identicon")
        if ($icon) $icon.dataset.seed = idSeed
        const $gradient = this.shadowRoot.querySelector(".profile-hero__gradient")
        if ($gradient) {
            $gradient.style.setProperty("--hero-hue-1", h1)
            $gradient.style.setProperty("--hero-hue-2", h2)
        }
        if ($icon) {
            $icon.style.color = `hsl(${h1}deg 100% 65%)`
            $icon.style.border = `2px solid hsl(${h1}deg 100% 65% / 40%)`
            $icon.style.boxShadow = `0 0 8px ${glow(h1, "53%")}, 0 0 24px ${glow(h1, "20%")}`
        }
    }

    async _loadName() {
        const name = await logic.loadname()
        if (name === null) return
        this.states.set({ name })
        const $name = this.shadowRoot.querySelector("#profile-name")
        if ($name) $name.textContent = name || "Anonymous"
    }

    async _saveName(value) {
        const saved = await logic.savename(value)
        if (!saved) return
        this.states.set({ name: saved })
        const $name = this.shadowRoot.querySelector("#profile-name")
        if ($name) $name.textContent = saved
    }

    _enterEditMode() {
        const $ = (id) => this.shadowRoot.querySelector(id)
        const $name = $("#profile-name")
        const $editBtn = $("#profile-name-edit")
        const $input = $("#profile-name-input")
        const $saveBtn = $("#profile-name-save")
        const $cancelBtn = $("#profile-name-cancel")

        $name.style.display = "none"
        $editBtn.style.display = "none"
        $input.value = this.states.get("name")
        $input.style.display = "inline-block"
        $saveBtn.style.display = "inline-flex"
        $cancelBtn.style.display = "inline-flex"
        $input.focus()
        $input.select()
        this.states.set({ editing: true })
    }

    _exitEditMode(save) {
        const $ = (id) => this.shadowRoot.querySelector(id)
        const $name = $("#profile-name")
        const $editBtn = $("#profile-name-edit")
        const $input = $("#profile-name-input")
        const $saveBtn = $("#profile-name-save")
        const $cancelBtn = $("#profile-name-cancel")

        if (save) this._saveName($input.value)

        $input.style.display = "none"
        $saveBtn.style.display = "none"
        $cancelBtn.style.display = "none"
        $name.style.display = ""
        if (logic.authenticated()) $editBtn.style.display = "inline-flex"
        this.states.set({ editing: false })
    }

    async _loadBio() {
        const bio = await logic.loadbio()
        if (bio === null) return
        this.states.set({ bio })
        const $bio = this.shadowRoot.querySelector("#profile-bio")
        if ($bio) $bio.textContent = bio
    }

    async _saveBio(value) {
        const saved = await logic.savebio(value)
        if (saved === null) return
        this.states.set({ bio: saved })
        const $bio = this.shadowRoot.querySelector("#profile-bio")
        if ($bio) $bio.textContent = saved
    }

    _enterBioEditMode() {
        const $ = (id) => this.shadowRoot.querySelector(id)
        const bio = this.states.get("bio")
        $("#profile-bio").style.display = "none"
        $("#profile-bio-edit").style.display = "none"
        $("#profile-bio-input").value = bio
        $("#profile-bio-count").textContent = 360 - bio.length
        $("#profile-bio-edit-wrap").style.display = "flex"
        $("#profile-bio-save").style.display = "inline-flex"
        $("#profile-bio-cancel").style.display = "inline-flex"
        $("#profile-bio-input").focus()
        this.states.set({ editingBio: true })
    }

    _exitBioEditMode(save) {
        const $ = (id) => this.shadowRoot.querySelector(id)
        if (save) this._saveBio($("#profile-bio-input").value)
        $("#profile-bio-edit-wrap").style.display = "none"
        $("#profile-bio-save").style.display = "none"
        $("#profile-bio-cancel").style.display = "none"
        $("#profile-bio").style.display = ""
        if (logic.authenticated()) $("#profile-bio-edit").style.display = "inline-flex"
        this.states.set({ editingBio: false })
    }

    _onKeydown(e) {
        if (e.key === "Enter") {
            e.preventDefault()
            this._exitEditMode(true)
        }
        if (e.key === "Escape") {
            e.preventDefault()
            this._exitEditMode(false)
        }
    }

    _onFollowSubmit(e) {
        e.preventDefault()
        const pub = this.shadowRoot.querySelector("#profile-follow-pub").value
        const name = this.shadowRoot.querySelector("#profile-follow-name").value
        this._addFollow(pub, name)
        e.target.reset()
    }

    async _loadLinks() {
        const links = await logic.loadlinks()
        if (!links) return
        this.states.set({ links })
        this._renderLinks()
    }

    async _saveLinks(obj) {
        const clean = await logic.savelinks(obj)
        if (!clean) return
        this.states.set({ links: clean })
        this._renderLinks()
    }

    _enterLinksEditMode() {
        const $ = (id) => this.shadowRoot.querySelector(id)
        $("#profile-links-display").style.display = "none"
        $("#profile-links-edit").style.display = "none"
        $("#profile-links-edit-form").style.display = "flex"
        $("#profile-links-save-row").style.display = "flex"
        this._renderLinksForm()
        this.states.set({ editingLinks: true })
    }

    _exitLinksEditMode(save) {
        const $ = (id) => this.shadowRoot.querySelector(id)
        if (save) {
            const inputs = this.shadowRoot.querySelectorAll("#profile-links-edit-form input[data-platform]")
            const obj = {}
            inputs.forEach(($i) => {
                if ($i.value.trim()) obj[$i.dataset.platform] = $i.value.trim()
            })
            this._saveLinks(obj)
        }
        $("#profile-links-display").style.display = ""
        $("#profile-links-edit").style.display = "inline-flex"
        $("#profile-links-edit-form").style.display = "none"
        $("#profile-links-save-row").style.display = "none"
        this.states.set({ editingLinks: false })
    }

    _renderLinksForm() {
        const container = this.shadowRoot.querySelector("#profile-links-edit-form")
        if (!container) return
        const current = this.states.get("links") || {}
        const items = SOCIAL_PLATFORMS.map(
            ({ key, label }) => html`
                <div class="profile-links__field">
                    <label class="profile-links__label">${label}</label>
                    <input class="profile-links__input" type="text" data-platform="${key}" value="${current[key] || ""}" placeholder="Username" autocomplete="off" />
                </div>
            `
        )
        render(items, container)
    }

    _renderLinks() {
        const container = this.shadowRoot.querySelector("#profile-links-display")
        if (!container) return
        const links = this.states.get("links") || {}
        const active = SOCIAL_PLATFORMS.filter(({ key }) => links[key])
        if (!active.length) {
            container.replaceChildren()
            return
        }
        const items = active.map(({ key, label, url }) => {
            const href = url ? url(links[key]) : null
            return href
                ? html`
                      <a class="profile-link" href="${href}" target="_blank" rel="noopener noreferrer" title="${label}: ${links[key]}" aria-label="${label}">
                          <ui-icon data-icon="${key}" data-size="md" />
                      </a>
                  `
                : html`
                      <span class="profile-link" title="${label}: ${links[key]}" aria-label="${label}">
                          <ui-icon data-icon="${key}" data-size="md" />
                      </span>
                  `
        })
        render(items, container)
    }

    _renderStats() {
        const $n = this.shadowRoot.querySelector("#profile-following-count")
        if ($n) $n.textContent = (this.states.get("following") || []).length
    }

    _loadFollowing() {
        this.states.set({ following: [] })
        this._followingScope?.off?.()
        this._followingScope = logic.loadfollowing((data, key) => {
            if (!key || key === "_") return
            const list = this.states.get("following") || []
            if (!data) {
                const filtered = list.filter((f) => f.pub !== key)
                if (filtered.length === list.length) return
                this.states.set({ following: filtered })
                this._renderFollowing()
                this._renderStats()
                return
            }
            if (list.some((f) => f.pub === key)) return
            this.states.set({ following: [...list, { pub: key, name: data.name || `${key.slice(0, 8)}…` }] })
            this._renderFollowing()
            this._renderStats()
        })
    }

    _addFollow(pub, name) {
        const list = this.states.get("following") || []
        const entry = logic.addfollow(pub, name, list)
        if (!entry) return
        this.states.set({ following: [...list, entry] })
        this._renderFollowing()
        this._renderStats()
    }

    _removeFollow(pub) {
        logic.removefollow(pub)
        this.states.set({ following: (this.states.get("following") || []).filter((f) => f.pub !== pub) })
        this._renderFollowing()
        this._renderStats()
    }

    _renderFollowing() {
        this._followingSubs.forEach((off) => off())
        this._followingSubs = []

        const container = this.shadowRoot.querySelector("#profile-following-list")
        if (!container) return

        const list = this.states.get("following") || []
        if (!list.length) {
            container.replaceChildren()
            return
        }

        const items = list.map(
            ({ pub, name }) => html`
                <div class="profile-following__item">
                    <ui-identicon data-seed="${pub}" data-size="5" />
                    <span class="profile-following__item-name">${name}</span>
                    <button
                        class="profile-following__unfollow"
                        ${({ element }) => {
                            const unfollow = () => this._removeFollow(pub)
                            element.addEventListener("click", unfollow)
                            this._followingSubs.push(() => element.removeEventListener("click", unfollow))
                        }}>
                        Unfollow
                    </button>
                </div>
            `
        )

        render(items, container)
        this._renderStats()
    }
}

customElements.define("route-profile", PROFILE)

export default PROFILE
