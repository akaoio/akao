import template from "./template.js"
import DB from "/core/DB.js"
import { Context } from "/core/Context.js"
import States from "/core/States.js"
import { html, render } from "/core/UI.js"
import "/UI/components/a/index.js"

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
        window.addEventListener("game-nav:open", this._toggle)
        await this._loadGames()
        this._render()
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
        window.removeEventListener("game-nav:open", this._toggle)
    }

    async _loadGames() {
        const ids = await DB.get(["statics", "games", "1.json"])
        if (!Array.isArray(ids)) return

        const metas = await Promise.all(ids.map((id) => DB.get(["statics", "games", id, "meta.json"]).then((m) => (m ? { id, ...m } : null))))
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
        const currentId = Context.get("params")?.game

        const panel = this.shadowRoot.querySelector(".game-nav__panel")
        const backdrop = this.shadowRoot.querySelector(".game-nav__backdrop")
        panel?.classList.toggle("open", open)
        backdrop?.classList.toggle("open", open)

        const list = this.shadowRoot.querySelector(".game-nav__list")
        if (!list) return

        render(
            games.map(
                (game) => html`
                    <a is="ui-a" data-to="/game/${game.id}" class="game-nav__item${game.id === currentId ? " active" : ""}">
                        ${game.logo
                            ? html`
                                  <img src="${game.logo}" alt="${game.name || game.id}" class="game-nav__item-logo" loading="lazy" />
                              `
                            : ""}
                        <span class="game-nav__item-info">
                            <span class="game-nav__item-name">${game.name || game.id}</span>
                            <span class="game-nav__item-tags">${[...(game.genres || []), ...(game.modes || [])].slice(0, 3).join(" · ")}</span>
                        </span>
                    </a>
                `
            ),
            list
        )
    }

    _bindEvents() {
        if (this._bound) return
        this._bound = true
        this.shadowRoot.querySelector(".game-nav__toggle")?.addEventListener("click", this._toggle)
        this.shadowRoot.querySelector(".game-nav__close")?.addEventListener("click", this._close)
        this.shadowRoot.querySelector(".game-nav__backdrop")?.addEventListener("click", this._close)
    }
}

customElements.define("game-nav", GAME_NAV)

export default GAME_NAV
