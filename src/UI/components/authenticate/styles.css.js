import { css } from "/core/UI.js"
import buttons from "/css/elements/buttons.css.js"

export const styles = css`
    ${buttons}
    :host {
        display: flex;
        flex-direction: column;
        gap: var(--space);

        .wave-section {
            display: none;
            flex-direction: column;
            gap: var(--space);
            &.active {
                display: flex;
            }

            ui-visualizer {
                width: min(100%, 10rem);
                align-self: center;
            }
        }
    }
`

export default styles
