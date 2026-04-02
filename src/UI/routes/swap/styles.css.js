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
            align-self: center;
            flex-direction: column;
            gap: var(--space);
            max-width: var(--box-width);
        }
        .field {
            display: flex;
            flex-direction: column;
            gap: var(--space-sm);
        }
        label {
            font-size: var(--text-sm);
            opacity: 0.6;
        }
        .row {
            display: flex;
            gap: var(--space);
            align-items: center;
        }
        .row input {
            flex: 1;
        }
        .info {
            font-size: var(--text-sm);
            opacity: 0.6;
            min-height: 1.2em;
            &:empty {
                display: none;
            }
        }
        #slippage-row {
            display: flex;
            align-items: center;
            gap: var(--space);
            font-size: var(--text-sm);
        }
        #slippage-row input {
            width: 70px;
        }
        #gas, #quote-out {
            font-size: var(--text-sm);
            opacity: 0.6;
            &:empty {
                display: none;
            }
        }
        #error {
            color: var(--neon-m);
            font-size: var(--text-sm);
            &:empty {
                display: none;
            }
        }
    }
`

export default styles
