import { css } from "/core/UI.js"

export const styles = css`
    :host {
        display: grid;
        grid-template-columns: 1fr auto auto auto;
        align-items: center;
        gap: var(--space);
        padding: var(--space);
        border-bottom: var(--border);
    }
    .pair {
        display: flex;
        align-items: center;
        gap: var(--space-sm);
        font-weight: 600;
    }
    .pair ui-crypto + ui-crypto::before {
        content: "/";
        margin-right: var(--space-sm);
        opacity: 0.5;
    }
    .info {
        display: flex;
        flex-direction: column;
        gap: var(--space-sm);
        font-size: var(--text-sm);
    }
    .rates {
        display: flex;
        flex-direction: column;
        gap: 2px;
    }
    .rate {
        color: var(--color);
    }
    .tvl {
        opacity: 0.6;
    }
    .badges {
        display: flex;
        gap: var(--space-sm);
        flex-wrap: wrap;
    }
    .badge {
        display: flex;
        align-items: center;
        gap: 4px;
        padding: var(--space-sm) var(--space);
        border: var(--border);
        border-radius: var(--radius);
        background: var(--background-inverted);
        color: var(--color-inverted);
        font-size: var(--text-xs);
        white-space: nowrap;
    }
    .badge ui-svg {
        width: 1.2em;
        height: 1.2em;
        flex-shrink: 0;
    }
`

export default styles
