import "/UI/components/wave/index.js"
import "/UI/components/button/index.js"
import "/UI/components/context/index.js"
import "/UI/components/icon/index.js"
import "/UI/components/modal/index.js"
import styles from "./styles.css.js"
import { html } from "/core/UI.js"

export const template = html`
    ${styles}
    <ui-icon data-size="md" data-icon="soundwave" id="authorize" />
    <ui-modal data-header="authorization" class="center">
        <div class="wave">
            <ui-wave />
            <span id="pub"></span>
        </div>
        <div class="buttons">
            <ui-button class="full" data-left="shield-check" id="grant">
                <ui-context data-key="dictionary.grant" />
            </ui-button>
            <ui-button class="full" data-left="x-circle" id="deny">
                <ui-context data-key="dictionary.deny" />
            </ui-button>
            <ui-button class="full" data-left="stop-circle" id="stop">
                <ui-context data-key="dictionary.stop" />
            </ui-button>
        </div>
    </ui-modal>
`

export default template
