import { css } from "/core/UI.js"

export const styles = css`
    :host {
        ui-modal {
            section {
                display: flex;
                flex-direction: column;
                align-items: center;
                gap: var(--space);
                ui-identicon {
                    width: var(--icon-md);
                    min-width: var(--icon-md);
                }
            }
        }
    }
`

export default styles