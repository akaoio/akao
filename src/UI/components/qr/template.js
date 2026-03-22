import { html } from "/core/UI.js"
import styles from "./styles.css.js"
import "/UI/components/camera/index.js"
import "/UI/components/button/index.js"
import "/UI/components/context/index.js"

export const template = html`
    ${styles}
    <div id="qr"></div>
    <div id="scanner" hidden>
        <div class="scanner-shell">
            <ui-camera id="camera" data-autostart="false"></ui-camera>
            <div class="scanner-panel">
                <div class="actions">
                    <ui-button id="start"><ui-context data-key="dictionary.startScan" /></ui-button>
                    <ui-button id="stop"><ui-context data-key="dictionary.stopScan" /></ui-button>
                    <ui-button id="clear"><ui-context data-key="dictionary.clear" /></ui-button>
                </div>
                <div id="status"><ui-context data-key="dictionary.waitingForCamera" /></div>
                <pre id="result"></pre>
            </div>
        </div>
    </div>
`

export default template