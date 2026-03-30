import { css } from "/core/UI.js"
import form from "/css/elements/form.css.js"
import input from "/css/elements/input.css.js"
import buttons from "/css/elements/buttons.css.js"
import heading from "/css/elements/heading.css.js"

export const styles = css`
    ${form}
    ${input}
    ${buttons}
    ${heading}
    :host {
        main {
            display: none;
            flex-direction: column;
            gap: var(--space);
            &.active {
                display: flex;
            }
        }
    }
`

export default styles
