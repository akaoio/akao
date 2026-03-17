import { css } from "/core/UI.js"

export const styles = css`
    body {
        background: var(--background);
        color: var(--color);
        font-family: var(--font);
        font-size: var(--text);
        line-height: 1.5;
        margin: 0;
        padding: calc(var(--header-height) + var(--space) * 2) 0 var(--footer-height) 0;
        transition: var(--speed) ease-in-out;
        cursor: crosshair;
    }

    :root[data-theme="dark"] body::after {
        content: '';
        position: fixed;
        inset: 0;
        background: repeating-linear-gradient(
            to bottom,
            transparent 0px,
            transparent 3px,
            rgba(0, 0, 0, 0.07) 3px,
            rgba(0, 0, 0, 0.07) 4px
        );
        pointer-events: none;
        z-index: 9999;
    }
`

export default styles
