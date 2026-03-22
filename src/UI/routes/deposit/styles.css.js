import { css } from "/core/UI.js"

export const styles = css`
    :host {
        main {
            display: flex;
            align-self: center;
            flex-direction: column;
            gap: var(--space);
            max-width: var(--box-width);
        }
    }
`

export default styles