import { html } from "/core/UI.js"
import styles from "./styles.css.js"
import "/UI/components/camera/index.js"
import "/UI/components/button/index.js"
import "/UI/components/context/index.js"
import "/UI/components/icon/index.js"

export const template = html`
    ${styles}
    <div id="qr"></div>
    <div id="scanner" hidden>
        <ui-camera id="camera" data-autostart="false" data-controls="false">
            <div id="controls">
                <ui-icon id="start" data-icon="qr-code-scan" />
                <ui-icon id="stop" data-icon="stop-fill" />
                <ui-icon id="clear" data-icon="eraser" />
            </div>
        </ui-camera>
        <pre id="result"></pre>
    </div>
`

export default template