import { html } from "/core/UI.js"
import styles from "./styles.css.js"
import "/UI/components/select/index.js"

export const template = html`
    ${styles}
    <ui-select id="country" data-name="country" data-placeholder="dictionary.country" data-required />
`

export default template