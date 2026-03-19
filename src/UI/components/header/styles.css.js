import { css } from "/core/UI.js"

export const styles = css`
    :host {
        header {
            position: fixed;
            top: 0;
            width: 100%;
            background: var(--header-background, var(--background));
            border-bottom: var(--header-border-bottom, none);
            backdrop-filter: var(--header-backdrop, none);
            -webkit-backdrop-filter: var(--header-backdrop, none);

            nav {
                max-height: var(--header-height);
                display: flex;
                align-items: center;
                justify-content: space-between;
                padding: var(--space);
                & a,
                & div {
                    display: flex;
                    gap: var(--space);
                }
            }

            .brand {
                max-width: 8rem;
                color: var(--neon-c);
                display: flex;
                filter: drop-shadow(var(--glow-c, none));
            }
        }
    }
`

export default styles
