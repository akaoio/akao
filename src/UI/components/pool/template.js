import { html } from "/core/UI.js"
import "/UI/components/crypto/index.js"
import "/UI/components/svg/index.js"
import styles from "./styles.css.js"

export const template = html`
    ${styles}
    <div class="pair">
        <ui-crypto id="token0"></ui-crypto>
        <ui-crypto id="token1"></ui-crypto>
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
