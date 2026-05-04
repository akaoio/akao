import "/UI/components/wave/index.js"
import "/UI/components/button/index.js"
import "/UI/components/context/index.js"
import "/UI/components/icon/index.js"
import styles from "./styles.css.js"
import { html } from "/core/UI.js"

export const template = html`
    ${styles}
    <div id="methods-screen">
        <div>
            <p><ui-context data-key="dictionary.pleaseChooseYourAuthenticationMethod" /></p>
        </div>
        <div>
            <ui-icon data-size="xlg" data-icon="fingerprint" id="passkey"/>
            <ui-context data-key="dictionary.passkey" />
        </div>
        <div>
            <ui-icon data-size="xlg" data-icon="soundwave" id="wave"/>
            <ui-context data-key="dictionary.wave" />
        </div>
    </div>
    <div id="passkey-screen" hidden>
        <h3><ui-context data-key="dictionary.passkey" /></h3>
        <ui-icon data-size="xlg" data-icon="fingerprint" />
    </div>
    <div id="wave-screen" hidden>
        <h3><ui-context data-key="dictionary.wave" /></h3>
        <ui-wave/>
        <span id="pub"></span>
        <span class="msg" id="msg"></span>
        <div class="buttons">
            <ui-button class="full" data-left="soundwave" id="request-btn">
                <ui-context data-key="dictionary.request" />
            </ui-button>
            <ui-button class="full" data-left="stop-circle" id="stop-btn" hidden>
                <ui-context data-key="dictionary.stop" />
            </ui-button>
        </div>
    </div>
`

export default template
