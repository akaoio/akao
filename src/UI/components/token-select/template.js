import { html } from "/core/UI.js"
import "/UI/components/svg/index.js"
import styles from "./styles.css.js"

export const template = html`
    ${styles}
    <button class="trigger" type="button">
        <ui-svg class="icon"></ui-svg>
        <span class="label"></span>
    </button>
    <div class="dropdown" hidden>
        <input class="search" type="text" autocomplete="off" />
        <div class="options"></div>
    </div>
`

export default template
