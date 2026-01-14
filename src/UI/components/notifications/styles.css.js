import { css } from "/core/UI.js"

export const styles = css`
    :host {
        position: fixed;
        bottom: 0;
        left: 0;
        z-index: 1000;
        display: flex;
        flex-direction: column;
        gap: var(--space);
        padding: var(--space);
        .notification {
            background: var(--background-inverted);
            border: 1px solid var(--border-inverted);
            color: var(--color-inverted);
            padding: var(--space);
            display: flex;
            flex-direction: row;
            justify-content: space-between;
            align-items: center;
            gap: var(--space);
        }
    }
`

export default styles