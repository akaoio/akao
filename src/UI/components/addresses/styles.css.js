import { css } from "/core/UI.js"
import form from "/css/elements/form.css.js"
import input from "/css/elements/input.css.js"
import buttons from "/css/elements/buttons.css.js"
import inputs from "/css/elements/inputs.css.js"
import checkbox from "/css/elements/checkbox.css.js"
import radio from "/css/elements/radio.css.js"
import select from "/css/elements/select.css.js"

export const styles = css`
    ${form}
    ${checkbox}
    ${radio}
    ${select}
    ${input}
    ${inputs}
    ${buttons}
    :host {
        width: 100%;
        display: flex;
        flex-direction: column;
        gap: var(--space);
        ui-select {
            --select-width: max-content;
        }
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
            display: none;
        }
        #addresses {
            display: flex;
            flex-direction: column;
            gap: var(--space);
            div {
                display: flex;
                flex-direction: row;
                gap: var(--space);
                justify-content: space-between;
                border: var(--border);
                padding: var(--space);
                main {
                    span {
                        display: block;
                    }
                }
                nav {
                    display: flex;
                    flex-direction: row;
                    gap: var(--space);
                }
            }
        }
    }
`

export default styles