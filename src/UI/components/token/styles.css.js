import { css } from "/core/UI.js"

export const styles = css`
    :host {
        display: inline-flex;
        align-items: center;
        gap: var(--space-sm);
    }
    ui-svg {
        width: var(--icon-sm);
        height: var(--icon-sm);
        flex-shrink: 0;
    }
    .name {
        font-weight: 500;
    }
    .amount {
        opacity: 0.6;
    }
    .fiat {
        opacity: 0.5;
        font-size: var(--text-sm);
    }
`

export default styles
