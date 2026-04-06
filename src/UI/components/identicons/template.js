import "/UI/components/identicon/index.js"
import styles from "./styles.css.js"
import { html } from "/core/UI.js"

export const template = html`
    ${styles}
    <div id="status" aria-live="polite" aria-atomic="true">
        <span class="status-left">
            <span class="status-item">
                <span id="status-saved" class="status-num status-num--saved">—</span>
            </span>
            <span id="status-arrow" class="status-arrow" hidden>→</span>
            <span id="status-preview" class="status-item" hidden>
                <span id="status-preview-num" class="status-num status-num--preview">—</span>
            </span>
        </span>
        <span class="status-right">
            <span class="actions__sep" aria-hidden="true"></span>
            <span class="status-total-group">
                <button id="status-decrease" class="status-nav-btn" aria-label="Show fewer avatars" disabled>‹</button>
                <span id="status-total" class="status-total"></span>
                <button id="status-increase" class="status-nav-btn" aria-label="Show more avatars">›</button>
            </span>
            <slot name="actions"></slot>
        </span>
    </div>
    <div id="scroll-wrap">
        <div id="container"></div>
        <div id="loader" hidden aria-hidden="true"><span class="loader-spinner"></span></div>
    </div>
`

export default template
