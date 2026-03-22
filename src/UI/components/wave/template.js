import { html } from "/core/UI.js"

export const template = html`
    ${styles}
    <textarea id="input" placeholder="Type something..." rows="4"></textarea>
    <textarea id="incoming" placeholder="Incoming messages..." rows="4" readonly></textarea>
    <button id="send">Send</button>
    <button id="listen">Listen</button>
`

export default template