import { css } from "/core/UI.js"

export const styles = css`
    :root[data-theme="dark"],
    .dark {
        --l1: 0%;
        --l1-inverted: 100%;

        --background: hsl(215, 20%, 9%);
        --background-accent: #1a2733;
        --background-focus: rgba(0, 229, 255, 0.08);
        --background-inverted: #c8dde8;
        --color: #c8dde8;
        --color-accent: #00ff9d;
        --color-inverted: #121c26;

        --border: var(--border-width) solid #233c4d;
        --border-inset: inset 0 0 0 var(--border-width) #233c4d;

        --header-background: hsla(215, 20%, 9%, 0.88);
        --header-border-bottom: var(--border-width) solid var(--neon-c);
        --footer-background: hsla(215, 20%, 9%, 0.88);
        --footer-border-top: var(--border-width) solid var(--neon-g);

        --scrollbar-track: hsl(215, 20%, 9%);
        --scrollbar-thumb: #233c4d;
        --scrollbar-thumb-hover: var(--neon-c);
        --scrollbar-corner: var(--background);
        --symbol-fill: #c8c8c8;

        --section-line: linear-gradient(to right, var(--neon-g) 0%, #233c4d 40%, transparent 100%);
        --item-background: #1a2733;
        --item-hover-shadow: 0 0 20px #00e5ff11, inset 0 0 30px #00e5ff06;
    }
`

export default styles
