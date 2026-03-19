import { css } from "/core/UI.js"

export const styles = css`
    :host {
        display: block;
        background: var(--item-background, transparent);
        border: var(--border);
        border-left: var(--item-border-left, var(--border));
        padding: var(--space-3);
        transition: box-shadow var(--speed) ease-in-out;

        &:hover {
            box-shadow: var(--item-hover-shadow, var(--border-inset));
        }

        header a {
            text-decoration: none;

            h3 {
                font-family: var(--header-font);
                font-size: var(--text-sm);
                font-weight: 700;
                letter-spacing: 0.04em;
                color: var(--item-name-color, var(--color-accent));
                text-shadow: var(--item-name-shadow, none);
                margin-bottom: var(--space-1);
                transition: text-shadow var(--speed) ease-in-out;
            }
        }

        #pricing {
            display: flex;
            gap: var(--space);
            align-items: center;
            flex-direction: row;
            margin-top: var(--space-2);
        }
    }
`

export default styles
