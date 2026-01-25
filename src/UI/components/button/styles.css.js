import button from "/css/elements/button.css.js"
import { css } from "/core/UI.js"

export const styles = css`
    ${button}
    :host {
        button {
            border-radius: var(--radius);
        }
    }
`

export default styles
