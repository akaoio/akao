import fieldset from "/UI/css/elements/fieldset.css.js"
import input from "/UI/css/elements/input.css.js"
import radioItem from "/UI/css/elements/radio-item.css.js"
import { css } from "/core/UI.js"

export const styles = css`
    ${fieldset}
    ${input}
    ${radioItem}
    :host {
        #item {
            display: grid;
            grid-template-columns: 1fr 1fr;
            grid-template-areas:
                "image header"
                "image main"
                "image footer";
            gap: var(--space);
            #quantity {
                display: flex;
                gap: var(--space);
            }
            #pricing {
                display: flex;
                gap: var(--space);
                align-items: center;
                flex-direction: row;
            }
        }
    }
`

export default styles
