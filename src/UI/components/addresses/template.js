import { html } from "/core/UI.js"
import styles from "./styles.css.js"

export const template = html`
    ${styles}
    <header>
        <ui-context data-key="dictionary.addresses" />
        <nav>
            <ui-icon id="add" data-icon="plus-lg" />
        </nav>
    </header>
    <div id="addresses"></div>
`

export default template