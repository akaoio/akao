import { css } from "/core/UI.js"

export const styles = css`
    :host {
        select, ::picker(select) {
            appearance: base-select;
        }

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
            option, selectedcontent {
                display: flex;
                width: 100%;
                padding: var(--space);
                border: none;
                background: var(--background);
                color: var(--color);
                box-sizing: border-box;
            }
            option::checkmark {
                display: none;
            }
        }
    }
`

export default styles