import { html } from "/core/UI.js"
import "/UI/components/token/index.js"
import styles from "./styles.css.js"

export const template = html`
    ${styles}
    <div class="pair">
        <ui-token id="token0"></ui-token>
        <ui-token id="token1"></ui-token>
    </div>
    <div class="info">
        <div class="rate"></div>
        <div class="tvl"></div>
    </div>
    <div class="badges">
        <span class="badge dex"></span>
        <span class="badge chain"></span>
    </div>
`

export default template
