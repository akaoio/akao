import "/UI/components/identicons/index.js"
import styles from "./styles.css.js"
import { html } from "/core/UI.js"

export const template = html`
    ${styles}
    <ui-identicons data-name="avatar" data-size="7">
        <div slot="actions" class="avatar-actions">
            <span class="avatar-actions__sep" aria-hidden="true"></span>
            <button class="avatar-btn avatar-btn--cancel" id="avatar-cancel" aria-label="Cancel">✕</button>
            <button class="avatar-btn avatar-btn--accept" id="avatar-accept" aria-label="Apply">✓</button>
        </div>
    </ui-identicons>
`

export default template
