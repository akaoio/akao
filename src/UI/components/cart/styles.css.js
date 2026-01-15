import icon from "/css/elements/icon.css.js"
import { css } from "/core/UI.js"

export const styles = css`
    ${icon}
    :host {
        #items {
            display: flex;
            flex-direction: column;
            gap: var(--space);
            .item {
                display: flex;
                flex-direction: row;
                align-items: center;
                gap: var(--space);
                .actions {
                    display: flex;
                    flex-direction: row;
                    gap: var(--space);
                }
            }
        }
    }
`

export default styles
