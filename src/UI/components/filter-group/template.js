import { html } from "/core/UI.js"
import styles from "./styles.css.js"

export const template = html`
    ${styles}
    <div class="filter-group">
        <span class="filter-group__label">
            <slot name="label"></slot>
        </span>
        <div class="filter-choices" id="choices">
            <div class="filter-tabs" id="tabs"></div>
            <div class="filter-select-wrap">
                <select class="filter-select" id="select"></select>
            </div>
        </div>
    </div>
`

export default template
