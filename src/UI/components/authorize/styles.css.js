import { css } from "/core/UI.js"
import buttons from "/css/elements/buttons.css.js"

export const styles = css`
    ${buttons}
    :host {
        display: flex;
        flex-direction: column;
        align-items: center;
        gap: var(--space);

        [hidden] { display: none !important; }

        .main {
            display: flex;
            flex-direction: column;
            gap: var(--space);
            width: 100%;
        }

        #confirm {
            display: flex;
            flex-direction: column;
            gap: var(--space);
            width: 100%;

            #note {
                text-align: center;
                font-family: monospace;
                font-size: 0.9em;
            }

            .actions {
                display: flex;
                gap: var(--space);

                ui-button {
                    flex: 1;
                }
            }
        }
    }
`

export default styles
