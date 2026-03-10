import select from "/css/elements/select.css.js";
import icon from "/css/elements/icon.css.js";
import { css } from "/core/UI.js";

export const styles = css`
    ${select}
    ${icon}
    :host {
        select {
            width: var(--select-width, auto);
            option[disabled]:nth-of-type(1):has(ui-context.placeholder) {
                display: none;   
            }
        }
        .icon {
            background: transparent;
        }
    }
`;

export default styles;