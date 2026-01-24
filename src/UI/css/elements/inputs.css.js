import { css } from "/core/UI.js"

export const styles = css`
    :host {
        .inputs {
            display: flex;
            justify-content: space-between;
            align-items: auto;
            gap: var(--space);
            & > * {
                flex-grow: 1;
                box-sizing: border-box;
                width: 50%;
            }
        }
    }
`

export default styles
