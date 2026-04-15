import { css } from "/core/UI.js"

export const styles = css`
    @keyframes ui-loader-spin {
        to { transform: rotate(360deg); }
    }

    :host {
        display: flex;
        flex-direction: column;
        align-items: center;
        justify-content: center;
        gap: var(--space-3);
        min-height: var(--loader-min-height, 40vh);
    }

    :host([hidden]) {
        display: none;
    }

    .spinner {
        width: 2rem;
        height: 2rem;
        border-radius: 50%;
        border: 2px solid transparent;
        border-top-color: var(--loader-color, var(--neon-g));
        border-right-color: color-mix(in hsl, var(--loader-color, var(--neon-g)) 35%, transparent);
        box-shadow: 0 0 12px color-mix(in hsl, var(--loader-color, var(--neon-g)) 20%, transparent);
        animation: ui-loader-spin 0.7s linear infinite;
    }

    .label {
        font-size: var(--text-xs);
        color: var(--loader-color, var(--neon-g));
        letter-spacing: 0.08em;
        text-transform: uppercase;
        opacity: 0.7;
    }

    @media (prefers-reduced-motion: reduce) {
        .spinner {
            animation: none;
            opacity: 0.4;
        }
    }
`

export default styles
