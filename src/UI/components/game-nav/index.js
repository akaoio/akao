import template from "./template.js"
import { Context } from "/core/Context.js"
import States from "/core/States.js"
import { html, render } from "/core/UI.js"
import { events } from "/core/Events.js"
import "/UI/components/a/index.js"
import "/UI/components/svg/index.js"
import logic from "./logic.js"

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
        this.shadowRoot.querySelector(".game-nav__toggle")?.addEventListener("click", this._toggle)
        this.shadowRoot.querySelector(".game-nav__close")?.addEventListener("click", this._close)
        this.shadowRoot.querySelector(".game-nav__backdrop")?.addEventListener("click", this._close)
        this.subscriptions.push(
            Context.on("locale", this._render),
            Context.on("params", this._render),
            events.on("game-nav:open", this._toggle),
            () => this.shadowRoot.querySelector(".game-nav__toggle")?.removeEventListener("click", this._toggle),
            () => this.shadowRoot.querySelector(".game-nav__close")?.removeEventListener("click", this._close),
            () => this.shadowRoot.querySelector(".game-nav__backdrop")?.removeEventListener("click", this._close)
        )
        await this._loadGames()
        this._render()
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
    }

    async _loadGames() {
        this.states.set({ games: await logic.games() })
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
                                  <ui-svg data-src="${game.logo}" alt="${game.name || game.id}" class="game-nav__item-logo" />
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
}

customElements.define("game-nav", GAME_NAV)

export default GAME_NAV
