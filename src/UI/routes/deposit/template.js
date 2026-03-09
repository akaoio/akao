import { html } from "/core/UI.js"
import "/UI/layouts/main/index.js"
import "/UI/components/wallets/index.js"
import "/UI/components/context/index.js"

export const template = html`
    <layout-main>
        <h1><ui-context data-key="dictionary.deposit" /></h1>
        <div>
            <ui-wallets />
        </div>
    </layout-main>
`
export default template
