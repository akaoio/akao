import { html } from "/core/UI.js"
import "/UI/components/token/index.js"
import "/UI/components/svg/index.js"
import styles from "./styles.css.js"

export const template = html`
    ${styles}
    <div class="pair">
        <ui-token id="token0"></ui-token>
        <ui-token id="token1"></ui-token>
    </div>
    <div class="info">
        <div class="rates">
            <div id="rate0" class="rate"></div>
            <div id="rate1" class="rate"></div>
        </div>
        <div class="tvl"></div>
    </div>
    <div class="badges">
        <span class="badge dex"><ui-svg class="logo"></ui-svg><span class="version"></span></span>
        <span class="badge chain"><ui-svg class="logo"></ui-svg></span>
    </div>
`

export default template
