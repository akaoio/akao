import { css } from "/core/UI.js"
import externals from "./externals.css.js"
import vars from "./vars.css.js"
import light from "./light.css.js"
import dark from "./dark.css.js"
import defaultWhitelabel from "./whitelabels/default.css.js"
import cyberpunk from "./whitelabels/cyberpunk.css.js"
import scrollbar from "./elements/scrollbar.css.js"
import body from "./elements/body.css.js"
import header from "./elements/header.css.js"
import footer from "./elements/footer.css.js"
import brand from "./elements/brand.css.js"

export const styles = css`
    ${externals}
    ${vars}
    ${light}
    ${dark}
    ${defaultWhitelabel}
    ${cyberpunk}
    ${scrollbar}
    ${body}
    ${header}
    ${footer}
    ${brand}
`

export default styles
