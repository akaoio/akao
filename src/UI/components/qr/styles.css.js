import {css} from "/core/UI.js"

export const styles = css`
    :host {
        #qr {
            display: flex;
            background: white;
            padding: var(--space);
            &:empty {
                display: none;
            }
        }
    }
`

export default styles