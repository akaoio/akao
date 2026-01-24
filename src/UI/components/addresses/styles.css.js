import { css } from "/core/UI.js"
import form from "/css/elements/form.css.js"
import input from "/css/elements/input.css.js"
import buttons from "/css/elements/buttons.css.js"

export const styles = css`
    ${form}
    ${input}
    ${buttons}
    :host {
        width: 100%;
        display: flex;
        flex-direction: column;
        gap: var(--space);
        header {
            --icon: var(--icon-md);
            width: 100%;
            display: inline-flex;
            justify-content: space-between;
            align-items: center;
            text-transform: uppercase;
            nav {
                display: inline-flex;
                gap: var(--space);
            }
        }
        #address-form {
            display: flex;
            flex-direction: column;
            gap: var(--space);
        }
    }
`

export default styles