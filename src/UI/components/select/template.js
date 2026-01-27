import styles from "./styles.css.js"
import { html } from "/core/UI.js"
import "/UI/components/context/index.js"

export const template = html`
    ${styles}
    <slot></slot>
    <select>
        <selectedcontent></selectedcontent>
        <option disabled selected>
            <ui-context class="placeholder" />
        </option>
    </select>
`

export default template