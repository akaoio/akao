import { css } from "/core/UI.js"
import { bp } from "/UI/css/breakpoints.js"

export const styles = css`
    :host {
        .container {
            margin: auto;
            overflow: hidden;
            height: auto;
            width: 50%;
            display: flex;
            flex-direction: column;
            justify-content: center;

            @media (max-width: ${bp.sm}px) {
                width: 75%;
            }

            @media (max-width: ${bp.xs}px) {
                width: 100%;
            }
        }
    }
`

export default styles
