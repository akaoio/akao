import "/UI/components/identicon/index.js"
import "/UI/components/svg/index.js"
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
            <!-- carousel mode: prev/next page -->
            <button id="status-decrease" class="status-nav-btn status-nav-btn--carousel" aria-label="Show fewer" disabled>
                <ui-svg data-src="/images/icons/chevron-left.svg" />
            </button>
            <span id="status-total" class="status-total status-nav-btn--carousel"></span>
            <button id="status-increase" class="status-nav-btn status-nav-btn--carousel" aria-label="Show more">
                <ui-svg data-src="/images/icons/chevron-right.svg" />
            </button>
            <!-- picker mode: add new slot -->
            <button id="status-add" class="status-nav-btn status-nav-btn--picker" aria-label="Add">
                <ui-svg data-src="/images/icons/plus-lg.svg" />
            </button>
            <slot name="actions"></slot>
        </span>
    </div>
    <div id="scroll-wrap">
        <div id="container"></div>
        <div id="loader" hidden aria-hidden="true"><span class="loader-spinner"></span></div>
    </div>
`

export default template
