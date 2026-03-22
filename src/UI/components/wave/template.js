import { html } from "/core/UI.js"
import styles from "./styles.css.js"
import "/UI/components/qr/index.js"

export const template = html`
    ${styles}
    <div class="controls">
        <textarea id="input" rows="4" placeholder="Type something or keep auto payload"></textarea>
        <textarea id="incoming" rows="5" placeholder="Incoming messages..." readonly></textarea>
        <div class="actions">
            <button id="send">Send</button>
            <button id="listen">Listen</button>
            <button id="stop">Stop</button>
        </div>
        <pre id="status">Idle</pre>
    </div>
    <div class="channels">
        <section>
            <h4>Outgoing QR</h4>
            <ui-qr id="outgoing" data-mode="encode"></ui-qr>
        </section>
        <section>
            <h4>Incoming QR</h4>
            <ui-qr id="incoming-qr" data-mode="scan"></ui-qr>
        </section>
    </div>
`

export default template