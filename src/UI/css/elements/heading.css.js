import { css } from "/core/UI.js"

export const styles = css`
    :root {
        --heading-1: var(--unit-5);
        --heading-2: var(--unit-4);
        --heading-3: var(--unit-3);
    }

    :host {
        h1, h2, h3, h4, h5, h6 {
            font-weight: lighter;
            margin: 0;
        }
    }
`

export default styles
