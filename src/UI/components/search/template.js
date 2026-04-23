import { html } from "/core/UI.js"
import styles from "./styles.css.js"

export const template = html`
    ${styles}
    <div class="search-wrap">
        <input class="search-input" type="search" autocomplete="off" spellcheck="false" />
        <span class="search-count" aria-hidden="true"></span>
        <ul class="search-suggestions" role="listbox" aria-label="Suggestions"></ul>
    </div>
`

export default template
