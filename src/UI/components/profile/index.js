import { Elements } from "/core/Stores.js"
import { Access } from "/core/Access.js"
import { render } from "/core/UI.js"
import template from "./template.js"
import Logic from "/UI/components/user/logic.js"

export class PROFILE extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        Elements.Profile = this
        render(template, this.shadowRoot)
        this.subscriptions = []
        this.toggle = this.toggle.bind(this)
        this._renderIdentity = this._renderIdentity.bind(this)
    }

    get modal() {
        return this.shadowRoot.querySelector("ui-modal")
    }

    connectedCallback() {
        const $link = this.shadowRoot.querySelector("#profile-modal-link")
        const onLinkClick = () => this.modal.close()
        $link.addEventListener("click", onLinkClick)

        this.subscriptions.push(
            () => $link.removeEventListener("click", onLinkClick),
            Access.on("authenticated", ({ value }) => {
                if (!value) this.modal.close()
                else this._renderIdentity()
            }),
            Access.on("avatar", this._renderIdentity)
        )
        if (Access.get("authenticated")) this._renderIdentity()
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
    }

    toggle() {
        const check = Elements.Access?.checkpoint()
        if (!check) return
        this.modal.toggleModal()
    }

    async _renderIdentity() {
        const $icon = this.shadowRoot.querySelector("#profile-modal-identicon")
        const $name = this.shadowRoot.querySelector("#profile-modal-name")

        const seed = await Logic.seed()
        if (seed != null && $icon) {
            $icon.dataset.seed = seed
            let hash = 0
            for (let i = 0; i < seed.length; i++) {
                const c = seed.charCodeAt(i)
                hash = (hash << 5) - hash + c
                hash = hash & hash
            }
            $icon.style.color = `hsl(${Math.abs(hash) % 360}deg 100% 65%)`
        }

        const pair = Access.get("pair")
        if (pair && $name) {
            const raw = await globalThis.gun.get(`~${pair.pub}`).get("name")
            $name.textContent = typeof raw === "string" && raw ? raw : "Anonymous"
        }
    }
}

customElements.define("ui-profile", PROFILE)

export default PROFILE
