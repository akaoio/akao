import { css } from "/core/UI.js"
import form from "/css/elements/form.css.js"
import input from "/css/elements/input.css.js"
import buttons from "/css/elements/buttons.css.js"

export const styles = css`
    ${form}
    ${input}
    ${buttons}
    :host {
        main {
            display: flex;
            flex-direction: column;
            gap: var(--space);
        }
        #gas {
            font-size: 0.875em;
            opacity: 0.75;
            &:empty {
                display: none;
            }
        }
    }
`

export default styles
