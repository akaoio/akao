import template from "./template.js"
import DB from "/core/DB.js"
import { Context } from "/core/Context.js"
import States from "/core/States.js"
import { render } from "/core/UI.js"

export class GAME_NAV extends HTMLElement {
    constructor() {
        super()
        this.states = new States({ open: false, games: [] })
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.subscriptions = []
        this._toggle = this._toggle.bind(this)
        this._close = this._close.bind(this)
        this._render = this._render.bind(this)
    }

    async connectedCallback() {
        this.subscriptions.push(Context.on("locale", this._render))
        this.subscriptions.push(Context.on("params", this._render))
        this._bindEvents()
        await this._loadGames()
        this._render()
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
    }

    async _loadGames() {
        const ids = await DB.get(["statics", "games", "1.json"])
        if (!Array.isArray(ids)) return

        const metas = await Promise.all(
            ids.map((id) => DB.get(["statics", "games", id, "meta.json"]).then((m) => (m ? { id, ...m } : null)))
        )
        this.states.set({ games: metas.filter(Boolean) })
    }

    _toggle() {
        this.states.set({ open: !this.states.get("open") })
        this._render()
    }

    _close() {
        this.states.set({ open: false })
        this._render()
    }

    _render() {
        const open = this.states.get("open")
        const games = this.states.get("games")
        const locale = Context.get("locale")?.code || "en"
        const currentId = Context.get("params")?.game

        const panel = this.shadowRoot.querySelector(".game-nav__panel")
        const backdrop = this.shadowRoot.querySelector(".game-nav__backdrop")
        panel?.classList.toggle("open", open)
        backdrop?.classList.toggle("open", open)

        const list = this.shadowRoot.querySelector(".game-nav__list")
        if (!list) return

        const items = games.map((game) => {
            const a = document.createElement("a")
            a.href = `/${locale}/game/${game.id}`
            a.className = "game-nav__item" + (game.id === currentId ? " active" : "")

            if (game.logo) {
                const img = document.createElement("img")
                img.src = game.logo
                img.alt = game.name || game.id
                img.className = "game-nav__item-logo"
                img.loading = "lazy"
                a.appendChild(img)
            }

            const info = document.createElement("span")
            info.className = "game-nav__item-info"

            const nameEl = document.createElement("span")
            nameEl.className = "game-nav__item-name"
            nameEl.textContent = game.name || game.id

            const tags = [...(game.genres || []), ...(game.modes || [])].slice(0, 3).join(" · ")
            const tagsEl = document.createElement("span")
            tagsEl.className = "game-nav__item-tags"
            tagsEl.textContent = tags

            info.append(nameEl, tagsEl)
            a.appendChild(info)
            return a
        })

        list.replaceChildren(...items)
    }

    _bindEvents() {
        this.shadowRoot.querySelector(".game-nav__toggle")?.addEventListener("click", this._toggle)
        this.shadowRoot.querySelector(".game-nav__close")?.addEventListener("click", this._close)
        this.shadowRoot.querySelector(".game-nav__backdrop")?.addEventListener("click", this._close)
    }
}

customElements.define("game-nav", GAME_NAV)

export default GAME_NAV
