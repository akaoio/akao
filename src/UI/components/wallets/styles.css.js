import { css } from "/core/UI.js"

export const styles = css`
    :host([hidden]) {
        display: none;
    }
    :host {
        width: 100%;
        display: grid;
        grid-template-columns: 1fr 1fr;
        gap: var(--space);
        div {
            display: flex;
            flex-direction: column;
            gap: var(--space);
        }
        div:nth-child(2), div:nth-child(3) {
            grid-column: 1 / -1;
        }
        ui-select {
            --select-width: stretch;
        }
        #balance {
            width: 100%;
            text-align: center;
            font-size: var(--text-xl);
        }
        #address {
            text-transform: lowercase;
        }
    }
`

export default styles
