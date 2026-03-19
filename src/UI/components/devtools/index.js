import { html, render } from "/core/UI.js"
import styles from "./styles.css.js"

export class DEVTOOLS extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(html`${styles}<button>⚡ Showcase</button>`, this.shadowRoot)
        this.shadowRoot.querySelector("button").addEventListener("click", () => window.open(`/${document.documentElement.lang}/showcase/`, "_blank"))
    }
}

customElements.define("ui-devtools", DEVTOOLS)

export default DEVTOOLS
