import { css } from "/core/UI.js"
import buttons from "/css/elements/buttons.css.js"

export const styles = css`
    ${buttons}
    :host {
        display: flex;
        flex-direction: column;
        gap: var(--space);

        .wave {
            display: flex;
            flex-direction: column;
            gap: var(--space);

            #epub {
                text-align: center;
                font-family: monospace;
                color: var(--color-accent);
            }
        }

        .msg {
            text-align: center;
        }
    }
`

export default styles
