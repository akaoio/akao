import { css } from "/core/UI.js"

export const styles = css`
    :host {
        width: 100%;
        display: flex;
        flex-direction: column;
        gap: var(--space);
        header {
            --icon: var(--icon-md);
            width: 100%;
            display: inline-flex;
            justify-content: space-between;
            align-items: center;
            text-transform: uppercase;
            nav {
                display: inline-flex;
                gap: var(--space);
            }
        }
    }
`

export default styles