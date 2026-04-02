import { css } from "/core/UI.js"

export const styles = css`
    :host {
        main {
            display: flex;
            align-self: center;
            flex-direction: column;
            gap: var(--space);
            max-width: var(--max-width);
        }
        #filters {
            display: flex;
            gap: var(--space);
            flex-wrap: wrap;
        }
        #list {
            display: flex;
            flex-direction: column;
        }
        #empty {
            padding: var(--space-lg);
            text-align: center;
            opacity: 0.5;
        }
    }
`

export default styles
