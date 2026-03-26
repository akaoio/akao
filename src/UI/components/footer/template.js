import { html } from "/core/UI.js"
import "/UI/components/navigator/index.js"
import "/UI/components/icon/index.js"
import "/UI/components/a/index.js"
import "/UI/components/locales/index.js"
import "/UI/components/themes/index.js"
import "/UI/components/user/index.js"
import "/UI/components/fiats/index.js"
import "/UI/components/profile/index.js"
import "/UI/components/notifications/index.js"
import styles from "./styles.css.js"

export const template = html`
    ${styles}
    <footer>
        <ui-navigator>
            <a is="ui-a" data-to="/"><ui-icon data-icon="house" /></a>
            <a is="ui-a" data-to="/deposit"><ui-icon data-icon="download" /></a>
            <a is="ui-a" data-to="/withdraw"><ui-icon data-icon="upload" /></a>
            <ui-user />
            <ui-navigator data-icon="sliders">
                <ui-locales />
                <ui-fiats />
                <ui-themes />
                <a is="ui-a" data-to="/authorize"><ui-icon data-icon="qr-code-scan" /></a>
            </ui-navigator>
        </ui-navigator>
    </footer>
    <ui-profile />
    <ui-notifications />
`

export default template
