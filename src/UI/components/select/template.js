import styles from "./styles.css.js"
import { html } from "/core/UI.js"
import "/UI/components/context/index.js"

export const template = html`
    ${styles}
    <slot></slot>
    <select>
        <button>
            <selectedcontent></selectedcontent>
        </button>
    </select>
`

export default template
