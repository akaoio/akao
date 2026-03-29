import "/UI/components/wave/index.js"
import "/UI/components/button/index.js"
import "/UI/components/context/index.js"
import styles from "./styles.css.js"
import { html } from "/core/UI.js"

export const template = html`
    ${styles}
    <div class="wave">
        <ui-wave id="wave" />
        <span id="epub"></span>
    </div>
    <span class="msg" id="msg"></span>
    <div class="buttons">
        <ui-button class="full" data-left="soundwave" id="request-btn">
            <ui-context data-key="dictionary.request" />
        </ui-button>
        <ui-button class="full" data-left="stop-circle" id="stop-btn" hidden>
            <ui-context data-key="dictionary.stop" />
        </ui-button>
        <ui-button class="full" data-left="key-fill" id="passkey-btn">
            <ui-context data-key="dictionary.passkey" />
        </ui-button>
    </div>
`

export default template
