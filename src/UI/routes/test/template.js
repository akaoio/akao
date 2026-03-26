import { html } from "/core/UI.js"
import styles from "./styles.css.js"

export const template = html`
    ${styles}
    <div id="test-root">
        <div id="summary">
            <h1>Unit Tests</h1>
            <div class="actions">
                <button id="run-all">Run All</button>
                <button id="run-failed">Run Failed</button>
            </div>
            <div class="progress-bar"><div class="fill" style="width:0%"></div></div>
        </div>
        <div id="suites"></div>
    </div>
`
export default template
