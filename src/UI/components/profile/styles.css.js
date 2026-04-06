import { css } from "/core/UI.js"

export const styles = css`
    :host {
        ui-modal section {
            display: flex;
            flex-direction: column;
            gap: var(--space-3);
            padding: var(--space-3) var(--space-4);
        }

        .profile-identity {
            display: flex;
            flex-direction: row;
            align-items: center;
            gap: var(--space-3);

            ui-identicon {
                flex-shrink: 0;
                width: var(--icon-md);
                min-width: var(--icon-md);
            }
        }

        .profile-identity__info {
            display: flex;
            flex-direction: column;
            gap: var(--space-1);
            min-width: 0;
        }

        .profile-modal-name {
            font-family: var(--header-font);
            font-size: var(--text-md);
            font-weight: 600;
            color: var(--color);
            letter-spacing: 0.04em;
            white-space: nowrap;
            overflow: hidden;
            text-overflow: ellipsis;
        }

        .profile-modal-link {
            font-family: var(--header-font);
            font-size: var(--text-xs);
            letter-spacing: 0.08em;
            text-transform: uppercase;
            color: var(--neon-g);
            text-decoration: none;
            opacity: 0.75;
            transition: opacity var(--speed);

            &:hover { opacity: 1; }
            &:active { opacity: 0.5; }
        }
    }
`

export default styles
