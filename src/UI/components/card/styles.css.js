import { css } from "/core/UI.js"

export const styles = css`
    :host {
        display: flex;
        flex-direction: column;
        width: 100%;
        background: var(--card-bg);
        box-shadow: var(--card-shadow);
    }

    #header {
        display: flex;
        align-items: center;
        justify-content: space-between;
        padding: var(--space-sm) var(--space);
        min-height: 2.5rem;
        gap: var(--space-sm);
        background: var(--card-header-bg);
        border-bottom: 1px solid var(--card-header-border);
    }

    #title {
        font-family: var(--header-font);
        font-size: var(--text-sm);
        font-weight: 600;
        letter-spacing: 0.12em;
        text-transform: uppercase;
        color: var(--accent-action);
        text-shadow: var(--glow-action);
    }

    #meta:empty {
        display: none;
    }
`

export default styles
