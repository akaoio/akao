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
            border-top: var(--footer-border-top, none);
            backdrop-filter: var(--footer-backdrop, none);
            -webkit-backdrop-filter: var(--footer-backdrop, none);
        }
    }
`

export default styles
