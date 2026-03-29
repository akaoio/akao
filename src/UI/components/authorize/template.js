import "/UI/components/wave/index.js"
import "/UI/components/button/index.js"
import "/UI/components/context/index.js"
import styles from "./styles.css.js"
import { html } from "/core/UI.js"

export const template = html`
    ${styles}
    <div class="main">
        <ui-wave id="wave" />
        <div id="confirm" hidden>
            <p id="note"></p>
            <div class="actions">
                <ui-button data-left="x-circle" id="deny-btn">
                    <ui-context data-key="dictionary.deny" />
                </ui-button>
                <ui-button data-left="shield-check" id="grant-btn">
                    <ui-context data-key="dictionary.grant" />
                </ui-button>
            </div>
        </div>
    </div>
`

export default template
