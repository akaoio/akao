import icon from "/css/elements/icon.css.js"
import { css } from "/core/UI.js"

export const styles = css`
    ${icon}
    :host {
        width: fit-content;
        height: fit-content;
        border-radius: 50%;
    }
    :host([data-size="sm"]) {
        --icon: var(--icon-sm);
    }
    :host([data-size="md"]) {
        --icon: var(--icon-md);
    }
    :host([data-size="lg"]) {
        --icon: var(--icon-lg);
    }
    :host([data-size="xlg"]) {
        --icon: var(--icon-xlg);
    }
`

export default styles
