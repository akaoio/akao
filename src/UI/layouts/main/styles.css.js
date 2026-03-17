import { css } from "/core/UI.js"

export const styles = css`
    :host {
        main {
            width: var(--content-width);
            margin: 0 auto;
            padding: var(--space-4) 0;
            display: flex;
            flex-direction: column;
            gap: var(--space-6);
        }
    }
`

export default styles
