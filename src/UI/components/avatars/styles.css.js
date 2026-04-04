import { css } from "/core/UI.js"

export const styles = css`
    :host {
        width: 100%;
        height: 100%;
        display: flex;
        flex-direction: column;
        gap: var(--space);

        ui-identicons {
            flex: 1;
            min-height: 0;
        }
    }
`

export default styles
