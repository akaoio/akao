import container from "/css/elements/container.css.js"
import scrollbar from "/css/elements/scrollbar.css.js"
import close from "/css/elements/close.css.js"
import inAnimation from "/css/animations/in.css.js"
import { css } from "/core/UI.js"

export const styles = css`
    ${container}
    ${scrollbar}
    ${close}
    ${inAnimation}

    /* Common styles for all modals */
    :host {
        position: absolute;

        dialog {
            --header-size: var(--icon-sm);
            --footer-size: var(--icon-md);
            width: auto;
            height: auto;
            margin: auto;
            padding: 0;
            display: flex;
            justify-content: center;
            flex-direction: column;
            background: none;
            border: none;
            overflow: hidden;
            animation: in var(--speed) ease-in-out none;

            &::backdrop {
                background-color: var(--background);
                opacity: 0.75;
            }

            &:not([open]) {
                display: none;
                header,
                section {
                    display: none;
                }
            }

            header {
                display: flex;
                align-items: center;
                justify-content: space-between;
                text-align: center;
                gap: var(--space);

                #header {
                    position: relative;
                    display: flex;
                    align-items: center;
                    justify-content: center;
                    width: max-content;
                    white-space: nowrap;
                    max-width: 100%;
                    margin-bottom: -0.5px;
                    --clip-x1: 0;
                    --clip-x2: calc(100% - var(--header-size));
                    padding: 0 calc(var(--space) + var(--header-size)) 0 var(--space);
                    background: var(--background-inverted);
                    color: var(--color-inverted);
                    clip-path: polygon(var(--clip-x1) 0, var(--clip-x2) 0, 100% 100%, 0 100%);
                    text-transform: uppercase;
                }

                slot[name="header"] {
                    position: absolute;
                    right: var(--space);
                    display: flex;
                    gap: var(--space);
                }
            }

            footer {
                --icon: var(--footer-size);
                display: flex;
                align-items: center;
                justify-content: center;
                transform: translateY(-50%);
                height: var(--footer-size);
                gap: var(--space);
                .close {
                    background: var(--background-inverted);
                    &::before,
                    &::after {
                        background: var(--color-inverted);
                    }
                }
            }

            section {
                display: block;
                overflow: auto;
                background-color: var(--background);
                color: var(--color);
                padding: var(--space);
                padding-bottom: calc(var(--space) + var(--footer-size) * 0.5);
                border: var(--border);
            }
        }
    }

    :host(:dir(rtl)) {
        dialog {
            header {
                #header {
                    --clip-x1: var(--header-size);
                    --clip-x2: 100%;
                    padding: 0 var(--space) 0 calc(var(--space) + var(--header-size));
                }
                slot[name="header"] {
                    left: var(--space);
                    right: auto;
                }
            }
        }
    }

    :host(.center) {
        display: flex;
        align-items: center;
        justify-content: center;

        dialog {
            header {
                height: calc(auto - var(--header-size));
                padding: 0 var(--space);
                transform: translateY(50%);
                justify-content: center;

                #header {
                    padding: 0 calc(var(--space) + var(--header-size));
                    clip-path: polygon(0 50%, var(--header-size) 0, calc(100% - var(--header-size)) 0, 100% 50%, calc(100% - var(--header-size)) 100%, var(--header-size) 100%);
                }

                slot {
                    display: flex;
                }
            }

            section {
                padding-top: calc(var(--space) + var(--header-size));
            }
        }
    }
`

export default styles
