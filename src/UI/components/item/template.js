import { html } from "/core/UI.js"
import styles from "./styles.css.js"
import "/UI/components/a/index.js"
import "/UI/components/fiat/index.js"

export const template = html`
    ${styles}
    <header>
        <a is="ui-a">
            <h3 id="name"></h3>
            <slot name="name"></slot>
        </a>
    </header>
    <div>
        <div id="description"></div>
        <slot name="description"></slot>
    </div>
    <div id="pricing">
        <ui-fiat id="sale"></ui-fiat>
        <slot name="sale"></slot>
        <ui-fiat id="price"></ui-fiat>
        <slot name="price"></slot>
    </div>
`

export default template
