import { css } from "/core/UI.js"

export const styles = css`
    :host {
        display: flex;
        flex-direction: column;
        min-height: calc(100dvh - calc(var(--header-height) + var(--space) * 2) - var(--footer-height));

        main {
            flex: 1;
            min-height: 0;
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
