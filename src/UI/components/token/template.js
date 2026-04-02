import { html } from "/core/UI.js"
import "/UI/components/svg/index.js"
import styles from "./styles.css.js"

export const template = html`
    ${styles}
    <ui-svg class="icon"></ui-svg>
    <span class="name"></span>
    <span class="amount"></span>
    <span class="fiat"></span>
`

export default template
