import { html, render } from "/core/UI.js"
import styles from "./styles.css.js"
import "/UI/components/a/index.js"

export class DEVTOOLS extends HTMLElement {
    static module = import.meta.url
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(html`${styles}<a is="ui-a" data-to="/showcase">⚡ Showcase</a>`, this.shadowRoot)
    }
}

customElements.define("ui-devtools", DEVTOOLS)

export default DEVTOOLS
