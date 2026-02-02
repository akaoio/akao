import select from "/css/elements/select.css.js";
import { css } from "/core/UI.js";

export const styles = css`
    ${select}
    :host {
        select {
            width: max-content;
            option[disabled]:nth-of-type(1):has(ui-context.placeholder) {
                display: none;   
            }
        }
    }
`;

export default styles;