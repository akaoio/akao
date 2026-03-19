import { css } from "/core/UI.js"

export const styles = css`
    :host {
        position: fixed;
        bottom: var(--space-4, 16px);
        right: var(--space-4, 16px);
        z-index: 9999;
    }

    button {
        display: flex;
        align-items: center;
        gap: 6px;
        padding: 8px 14px;
        background: #7c3aed;
        color: #fff;
        border: none;
        border-radius: 999px;
        font-size: 12px;
        font-weight: 600;
        letter-spacing: 0.04em;
        cursor: pointer;
        box-shadow: 0 2px 12px rgba(124,58,237,0.45);
        opacity: 0.85;
        transition: opacity 0.15s, transform 0.15s;
    }

    button:hover {
        opacity: 1;
        transform: scale(1.05);
    }
`

export default styles
