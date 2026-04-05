import "/UI/components/identicon/index.js"
import styles from "./styles.css.js"
import { html } from "/core/UI.js"

export const template = html`
    ${styles}
    <div id="container"></div>
    <div id="status" aria-live="polite" aria-atomic="true">
        <span class="status-item">
            <span class="status-label">current</span>
            <span id="status-saved" class="status-num status-num--saved">—</span>
        </span>
        <span id="status-arrow" class="status-arrow" hidden>→</span>
        <span id="status-preview" class="status-item" hidden>
            <span class="status-label">selecting</span>
            <span id="status-preview-num" class="status-num status-num--preview">—</span>
        </span>
        <span class="status-sep" aria-hidden="true">·</span>
        <span id="status-total" class="status-total"></span>
    </div>
`

export default template
