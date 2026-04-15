import { css } from "/core/UI.js"
import { bp } from "/UI/css/breakpoints.js"

export const styles = css`
    :host {
        main {
            flex: 1;
            min-height: 0;
            display: flex;
            flex-direction: column;
            align-items: center;
            padding: var(--space) var(--space-sm);
            gap: var(--space-sm);
        }

        ui-card {
            width: 100%;
            max-width: 480px;
        }

        /* Swap card grows to fill all remaining vertical space */
        #swap-card {
            flex: 1;
            display: flex;
            flex-direction: column;

            ui-swap-form {
                flex: 1;
                display: flex;
                flex-direction: column;
            }
        }

        @media (max-width: ${bp.sm}px) {
            main {
                padding: var(--space-sm) 0;
            }
            ui-card {
                max-width: 100%;
            }
        }
    }
`

export default styles
