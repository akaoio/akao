import "../picker/index.js"
import { Context } from "/core/Context.js"
import styles from "./styles.css.js"
import { html } from "/core/UI.js"

export const template = html`
    ${styles}
    <span class="icon"></span>
    <ui-picker data-name="fiat" data-header="fiats" data-selected="${Context.get("fiat")?.code}" />
`

export default template
