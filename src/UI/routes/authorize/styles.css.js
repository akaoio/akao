import { css } from "/core/UI.js"

export const styles = css`
    :host {
        ui-authorize {
            position: fixed;
            top: 50%;
            left: 50%;
            transform: translate(-50%, -50%);
            width: min(calc(100vw - var(--space-4) * 2), 20rem);
        }
    }
`

export default styles