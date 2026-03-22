import styles from "./styles.css.js"
import { html } from "/core/UI.js"
import "/UI/components/button/index.js"
import "/UI/components/context/index.js"

const template = html`
    ${styles}
    <div class="camera-shell">
        <div class="viewport">
            <video id="video" autoplay playsinline muted></video>
            <div id="badge"><ui-context data-key="dictionary.idle" /></div>
        </div>
        <div class="controls">
            <ui-button id="switch"><ui-context data-key="dictionary.switch" /></ui-button>
            <ui-button id="capture"><ui-context data-key="dictionary.capture" /></ui-button>
            <ui-button id="resume" hidden><ui-context data-key="dictionary.resume" /></ui-button>
        </div>
    </div>
`

export default template