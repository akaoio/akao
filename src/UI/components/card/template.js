import styles from "./styles.css.js"
import { html } from "/core/UI.js"

export const template = html`
    ${styles}
    <div id="header">
        <span id="title"></span>
        <span id="meta"></span>
    </div>
    <slot></slot>
`

export default template
