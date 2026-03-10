import { css } from "/core/UI.js"
import { rules as scrollbar } from "/css/elements/scrollbar.css.js"

export const styles = css`
    :host {
        select, ::picker(select) {
            appearance: base-select;
        }

        ${scrollbar("::picker(select)")}

        select {
            display: flex;
            flex-direction: column;
            background: var(--background);
            border: var(--border);
            border-radius: var(--radius);
            color: var(--color);
            cursor: pointer;
            &::picker-icon {
                display: none;
            }
            &:hover {
                border: var(--border);
            }
            &:focus {
                border: var(--border-accent);
            }
            option {
                background: var(--background);
                color: var(--color);
            }
            option, selectedcontent {
                display: flex;
                width: 100%;
                padding: var(--space);
                gap: var(--space);
                align-items: center;
                border: none;
                box-sizing: border-box;
                &::checkmark {
                    display: none;
                }
            }
            option:hover, option:checked, selectedcontent {
                background: var(--background-inverted);
                color: var(--color-inverted);
            }
        }
    }
`

export default styles