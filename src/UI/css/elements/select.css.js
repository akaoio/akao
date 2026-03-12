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
            gap: var(--space);
            background: var(--background);
            border: var(--border);
            border-radius: var(--radius);
            padding: var(--space);
            color: var(--color);
            cursor: pointer;
            &::picker-icon {
                display: none;
            }
            &:hover {
                border: var(--border-accent);
                & > button:first-child ui-context {
                    opacity: 0.75;
                }
            }
            &:focus {
                border: var(--border-accent);
            }
            option {
                background: var(--background);
                color: var(--color);
                padding: var(--space);
            }
            option, selectedcontent {
                display: flex;
                width: 100%;
                gap: var(--space);
                align-items: center;
                border: none;
                box-sizing: border-box;
                &::checkmark {
                    display: none;
                }
            }
            option:hover, option:checked {
                background: var(--background-inverted);
                color: var(--color-inverted);
            }
            selectedcontent {
                padding: 0;
            }
            & > button:first-child {
                display: flex;
                flex-direction: column;
                gap: var(--unit-1);
                ui-context {
                    color: var(--color);
                    opacity: 0.25;
                    font-size: smaller;
                    font-weight: lighter;
                    letter-spacing: var(--unit-1);
                    text-transform: uppercase;
                }
        }
        }
    }
`

export default styles