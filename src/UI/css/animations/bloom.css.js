import { css } from "/core/UI.js"

export const styles = css`
    @keyframes bloom {
        from {
            scale: 1;
        }

        to {
            scale: 100;
            filter: blur(var(--blur-xl));
        }
    }
`

export default styles
