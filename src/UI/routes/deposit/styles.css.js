import { css } from "/core/UI.js"

export const styles = css`
    :host {
        main {
            display: flex;
            flex-direction: column;
            gap: var(--space);
            #qr {
                display: flex;
                background: white;
                padding: var(--space);
                &:empty {
                    display: none;
                }
            }
        }
    }
`

export default styles