import { html } from "/core/UI.js"
import styles from "./styles.css.js"

export const template = html`
    ${styles}
    <span class="spinner" aria-hidden="true"></span>
    <span class="label" hidden><slot></slot></span>
`

export default template
