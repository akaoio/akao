import "/UI/components/identicons/index.js"
import styles from "./styles.css.js"
import { html } from "/core/UI.js"

export const template = html`
    ${styles}
    <ui-identicons data-name="wallet" data-size="7"></ui-identicons>
    <div id="address"></div>
    <div id="balance"></div>
    <div id="currencies"></div>
    <div id="chains"></div>
`

export default template
