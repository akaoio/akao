import { css } from "/core/UI.js"

export const styles = css`
    :host([hidden]) {
        display: none;
    }
    :host {
        width: 100%;
        display: grid;
        grid-template-columns: 1fr 1fr;
        gap: var(--space);
        div {
            display: flex;
            flex-direction: column;
            gap: var(--space);
        }
        div:nth-child(2), div:nth-child(3) {
            grid-column: 1 / -1;
        }
        ui-select {
            --select-width: stretch;
        }
        header {
            --icon: var(--icon-md);
            width: 100%;
            display: inline-flex;
            justify-content: space-between;
            align-items: center;
            text-transform: uppercase;
            nav {
                display: inline-flex;
                gap: var(--space);
            }
        }
        #balance {
            width: 100%;
            text-align: center;
            font-size: var(--text-xl);
        }
        #wallets {
            width: 100%;
            display: grid;
            grid-template-columns: repeat(auto-fill, minmax(var(--icon-md), 1fr));
            gap: var(--space);
            .wallet {
                width: var(--icon-md);
                max-width: var(--icon-md);
                aspect-ratio: 1 / 1;
                input[type="radio"] {
                    display: none;
                    &:checked + label {
                        color: var(--color-accent);
                    }
                }
                label {
                    display: block;
                    box-sizing: border-box;
                    cursor: pointer;
                    &:hover {
                        color: var(--color-accent);
                    }
                }
            }
        }
        #address {
            text-transform: lowercase;
        }
    }
`

export default styles
