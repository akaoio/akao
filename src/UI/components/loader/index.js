import template from "./template.js"
import { render } from "/core/UI.js"
import Component from "/core/UI/Component.js"

export class LOADER extends Component {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
    }

    onConnect() {
        const slot = this.shadowRoot.querySelector("slot")
        const label = this.shadowRoot.querySelector(".label")
        const sync = () => {
            label.hidden = !slot.assignedNodes({ flatten: true }).length
        }
        slot.addEventListener("slotchange", sync)
        sync()
    }
}

customElements.define("ui-loader", LOADER)
