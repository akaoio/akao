import template from "./template.js"
import { render } from "/core/UI.js"

export class NAVIGATOR extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.subscriptions = []
    }

    connectedCallback() {
        const state = this.shadowRoot.querySelector("#state")
        const label = this.shadowRoot.querySelector("label")
        const icon = this.shadowRoot.querySelector("ui-icon")
        const vibrate = () => {
            if ("vibrate" in navigator) navigator.vibrate(20)
        }
        if (icon)
            if (this.dataset.icon) icon.dataset.icon = this.dataset.icon
            else delete icon.dataset.icon

        label.addEventListener("click", vibrate)
        this.subscriptions.push(() => label.removeEventListener("click", vibrate))

        const active = () => {
            let i = -1
            let el = this
            while (el.tagName === "UI-NAVIGATOR") {
                if (el.shadowRoot.querySelector("#state").checked) i++
                if (el.tagName !== el.parentElement.tagName) break
                el = el.parentElement
            }
            el.style.setProperty("--active", i)
        }
        state.addEventListener("change", active)
        this.subscriptions.push(() => state.removeEventListener("change", active))

        // Count the number of children in slot
        // Set --total for parent, and --i for each child
        const slot = this.shadowRoot.querySelector("slot")
        const children = slot.assignedElements()
        const total = children.length
        this.style.setProperty("--total", total)
        children.forEach((child, i) => {
            child.style.setProperty("--i", i + 1)
            if (child.tagName !== "UI-NAVIGATOR") {
                child.addEventListener("click", vibrate)
                this.subscriptions.push(() => child.removeEventListener("click", vibrate))
            }
        })
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
    }
}

customElements.define("ui-navigator", NAVIGATOR)

export default NAVIGATOR
