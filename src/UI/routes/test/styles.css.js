import { css } from "/core/UI.js"
import checkbox from "/UI/css/elements/checkbox.css.js"

export const styles = css`
    ${checkbox}
    :host {
        #container {
            display: flex;
            gap: var(--space);
            flex-direction: column;
        }
    }
`

export default styles