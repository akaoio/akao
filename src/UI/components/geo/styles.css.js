import { css } from "/core/UI.js"

export const styles = css`
    :host {
        display: flex;
        flex-direction: row;
        gap: var(--space);
        justify-content: flex-start;
        flex-wrap: wrap;
    }
`

export default styles