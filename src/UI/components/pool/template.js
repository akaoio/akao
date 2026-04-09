import { html } from "/core/UI.js"
import "/UI/components/svg/index.js"
import "./crypto/index.js"
import styles from "./styles.css.js"

export const template = html`
    ${styles}
    <div class="col-pair">
        <div class="pair">
            <ui-pool-crypto id="token0"></ui-pool-crypto>
            <span class="pair-sep">/</span>
            <ui-pool-crypto id="token1"></ui-pool-crypto>
        </div>
    </div>
    <div class="col-tvl">
        <span class="tvl"></span>
    </div>
    <div class="col-rate">
        <div class="rates">
            <div id="rate0" class="rate"></div>
            <div id="rate1" class="rate"></div>
        </div>
    </div>
    <div class="col-badges">
        <div class="badges">
            <span class="badge dex">
                <ui-svg class="logo"></ui-svg>
                <span class="version"></span>
            </span>
            <span class="badge chain">
                <ui-svg class="logo"></ui-svg>
            </span>
        </div>
    </div>
`

export default template
