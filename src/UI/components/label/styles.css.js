import { css } from "/core/UI.js"

export const styles = css`
    :host {
        display: inline;
        font-family: var(--header-font);
        font-size: var(--text-xs);
        letter-spacing: 0.1em;
        text-transform: uppercase;
        color: var(--color);
        padding-left: var(--label-padding-left, 0px);
        white-space: nowrap;
    }
`

export default styles
