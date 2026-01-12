import { css } from "/core/UI.js"

export const styles = css`
    :host {
        header {
            position: fixed;
            top: 0;
            width: 100%;

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
                color: var(--color);
                display: flex;
            }
        }
    }
`

export default styles
