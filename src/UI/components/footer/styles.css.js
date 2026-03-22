import { css } from "/core/UI.js"

export const styles = css`
    :host {
        footer {
            position: fixed;
            bottom: 0;
            display: flex;
            justify-content: center;
            align-items: center;
            width: 100%;
            padding: var(--space) 0;
            background: var(--footer-background, var(--background));
            border-top: 1px solid var(--game-text-color, var(--game-primary, var(--neon-c)));
            backdrop-filter: var(--footer-backdrop, none);
            -webkit-backdrop-filter: var(--footer-backdrop, none);
        }
    }
`

export default styles
