import { css } from "/core/UI.js"
import { rules as scrollbar } from "/css/elements/scrollbar.css.js"

export const styles = css`
    :host {
        select, ::picker(select) {
            appearance: base-select;
        }

        ${scrollbar("::picker(select)")}

        ::picker(select) {
            border-radius: var(--select-radius, var(--radius));
            border: var(--select-border, var(--border));
            box-shadow: var(--shadow-md);
            padding: 0;
            background: var(--select-bg, var(--background));
            overflow: hidden;
        }

        select {
            display: flex;
            flex-direction: column;
            gap: 0;
            background: var(--select-bg, var(--background));
            border: var(--select-border, var(--border));
            border-radius: var(--select-radius, var(--radius));
            padding: var(--select-padding, var(--space));
            color: var(--color);
            font-family: var(--select-font, inherit);
            font-size: var(--select-font-size, inherit);
            letter-spacing: var(--select-letter-spacing, inherit);
            cursor: pointer;
            white-space: nowrap;
            transition: border-color var(--speed), background var(--speed), box-shadow var(--speed);
            &::picker-icon {
                display: none;
            }
            &:hover {
                border: var(--select-border-hover, var(--border-accent));
                background: var(--select-bg-hover, var(--background));
                box-shadow: var(--shadow-sm);
                & > button:first-child ui-context {
                    opacity: 0.65;
                }
            }
            &:focus {
                border: var(--select-border-focus, var(--border-accent));
                outline: none;
            }
            option, selectedcontent {
                display: flex;
                width: 100%;
                gap: var(--space-sm);
                align-items: center;
                box-sizing: border-box;
                &::checkmark {
                    display: none;
                }
            }
            option {
                background: var(--background);
                color: var(--color);
                padding: var(--space-2) var(--space-sm);
                min-height: calc(var(--icon-sm, 1rem) + var(--space-2) * 2);
                max-height: calc(var(--icon-sm, 1rem) + var(--space-2) * 2);
                overflow: hidden;
                border: none;
                border-bottom: 1px solid color-mix(in srgb, var(--color) 6%, transparent);
                font-size: var(--select-font-size, var(--text-sm));
                &:last-child {
                    border-bottom: none;
                }
                ui-svg {
                    width: var(--icon-sm, 1rem);
                    height: var(--icon-sm, 1rem);
                    min-width: var(--icon-sm, 1rem);
                    max-width: var(--icon-sm, 1rem);
                    max-height: var(--icon-sm, 1rem);
                    flex-shrink: 0;
                    overflow: hidden;
                }
            }
            option:hover {
                background: color-mix(in srgb, var(--background-inverted) 10%, transparent);
                color: var(--color);
            }
            option:checked {
                background: color-mix(in srgb, var(--color-accent) 12%, transparent);
                color: var(--color-accent);
            }
            selectedcontent {
                padding: 0;
                gap: var(--space-sm);
                visibility: var(--select-content-visibility, visible);
                ui-svg {
                    width: var(--icon-sm, 1rem);
                    height: var(--icon-sm, 1rem);
                    min-width: var(--icon-sm, 1rem);
                    max-width: var(--icon-sm, 1rem);
                    max-height: var(--icon-sm, 1rem);
                    flex-shrink: 0;
                    overflow: hidden;
                }
            }
            & > button:first-child {
                display: flex;
                flex-direction: row;
                align-items: center;
                gap: var(--space-sm);
                ui-context {
                    color: var(--color);
                    opacity: 0.4;
                    font-size: var(--text-xs);
                    font-weight: lighter;
                    letter-spacing: 0.06em;
                    text-transform: uppercase;
                }
            }
        }
    }
`

export default styles