import styles from "./styles.css.js"
import { html } from "/core/UI.js"
import "/UI/components/icon/index.js"
import "/UI/components/context/index.js"

const template = html`
    ${styles}
    <video id="video" autoplay playsinline muted></video>
    <ui-context id="status" data-key="dictionary.idle" />
    <div class="controls">
        <ui-icon id="switch" data-icon="arrow-repeat" />
        <ui-icon id="capture" data-icon="camera" />
        <ui-icon id="resume" data-icon="record-circle" hidden />
        <slot></slot>
    </div>
`

export default template