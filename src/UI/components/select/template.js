import styles from "./styles.css.js"
import { html } from "/core/UI.js"
import "/UI/components/context/index.js"

export const template = html`
    ${styles}
    <slot></slot>
    <select>
        <button><ui-context class="placeholder" /><selectedcontent></selectedcontent></button>
    </select>
`

export default template