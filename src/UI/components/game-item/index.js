import template from "./template.js"
import { render } from "/core/UI.js"
import Component from "/core/UI/Component.js"

export class GAME_ITEM extends Component {
    static module = import.meta.url
    constructor() {
        super()
        this.template = template // Store for HMR
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
    }

    onconnect() {
        this._populate()
    }

    _populate() {
        let item
        try {
            item = JSON.parse(this.dataset.item || "{}")
        } catch {
            return
        }

        const { id, name, icon, rarity = "common", type = "", subtype = "", value = 0 } = item
        const rarityKey = rarity.toLowerCase().replace(/\s+/g, "-")
        const rarityColor = `var(--rarity-${rarityKey}, var(--rarity-common, #888888))`

        // Set rarity CSS custom property on host for border + hover
        this.style.setProperty("--item-rarity-color", rarityColor)

        // Rarity badge
        const rarityBadge = this.shadowRoot.querySelector("#rarity-badge")
        rarityBadge.textContent = rarity
        rarityBadge.style.background = rarityColor

        // Type badge
        const typeBadge = this.shadowRoot.querySelector("#type-badge")
        typeBadge.textContent = type
        typeBadge.style.display = type ? "" : "none"

        // Subtype badge
        const subtypeBadge = this.shadowRoot.querySelector("#subtype-badge")
        subtypeBadge.textContent = subtype
        subtypeBadge.style.display = subtype ? "" : "none"

        // Icon
        const iconWrap = this.shadowRoot.querySelector("#icon-wrap")
        if (icon) {
            const img = document.createElement("img")
            img.src = icon
            img.alt = name || ""
            img.loading = "lazy"
            img.onerror = () => img.remove()
            iconWrap.replaceChildren(img)
        }

        // Name / link
        const link = this.shadowRoot.querySelector("#item-link")
        link.textContent = name || id || ""
        if (id) link.dataset.to = item.game ? `/item/${item.game}/${id}` : `/item/${id}`

        // Price
        const priceEl = this.shadowRoot.querySelector("#price")
        if (value) {
            priceEl.textContent = value.toLocaleString()
            priceEl.classList.remove("price--zero")
        } else {
            priceEl.textContent = "—"
            priceEl.classList.add("price--zero")
        }
    }
}

customElements.define("ui-game-item", GAME_ITEM)

export default GAME_ITEM
