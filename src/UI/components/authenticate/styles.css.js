import { css } from "/core/UI.js"
import buttons from "/css/elements/buttons.css.js"

export const styles = css`
    ${buttons}
    [hidden] { display: none !important }
    :host {
        display: flex;
        flex-direction: row;
        gap: var(--space);
        position: relative;

        #methods-screen {
            width: 100%;
            display: grid;
            grid-template-columns: repeat(2, 1fr);
            gap: var(--space);
            div:first-child {
                grid-column: 1 / -1;
                text-align: center;
            }
            div:nth-child(2), div:nth-child(3) {
                display: flex;
                flex-direction: column;
                align-items: center;
                gap: var(--space);
            }
        }

        #passkey-screen, #wave-screen {
            width: 100%;
            display: flex;
            flex-direction: column;
            gap: var(--space);
            align-items: center;
        }

        #wave-screen {
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
