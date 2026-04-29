import { css } from "/core/UI.js"

export const styles = css`
    :host {
        width: 100%;
        height: 100%;
        display: flex;
        flex-direction: column;
        gap: var(--space);

        ui-identicons {
            flex: 1;
            min-height: 0;
            --identicons-pad-v: var(--space-5);
            --identicons-gap: var(--space-6);
        }
    }

    .avatar-actions {
        display: flex;
        align-items: center;
        gap: var(--space-1);
    }

    .avatar-actions__sep {
        display: block;
        width: 1px;
        height: 1.2em;
        background: color-mix(in hsl, var(--color) 20%, transparent);
        margin: 0 var(--space-1);
    }

    .avatar-btn {
        display: inline-flex;
        align-items: center;
        justify-content: center;
        min-width: 1.75rem;
        min-height: 2.75rem;
        background: none;
        border: none;
        font-size: var(--text-md);
        line-height: 1;
        cursor: pointer;
        pointer-events: all;
        transition: opacity var(--speed), color var(--speed);

        &:active { opacity: 0.6; }
    }

    .avatar-btn--cancel {
        color: var(--neon-m);
        opacity: 1;
        text-shadow: var(--glow-m);
        &:hover { opacity: 0.7; }
        &:active { opacity: 0.5; }
    }

    .avatar-btn--accept {
        color: var(--neon-g);
        opacity: 1;
        text-shadow: var(--glow-g);
        &:hover { opacity: 0.7; }
        &:active { opacity: 0.5; }
    }
`

export default styles
