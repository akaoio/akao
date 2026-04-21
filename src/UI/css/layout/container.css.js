import { css } from "/core/UI.js"
import { mq } from "/UI/css/layout/breakpoints.js"

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

            @media ${mq.smDown} {
                width: 75%;
            }

            @media ${mq.xsDown} {
                width: 100%;
            }
        }
    }
`

export default styles
