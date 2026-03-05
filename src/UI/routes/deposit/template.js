import { html } from "/core/UI.js"
import "/UI/layouts/main/index.js"
import "/UI/components/wallets/index.js"
import "/UI/components/context/index.js"
import "/UI/components/select/index.js"

export const template = html`
    <layout-main>
        <h1><ui-context data-key="dictionary.deposit" /></h1>
        <div>
            <ui-wallets />
            <ui-select id="chain" data-name="chain" data-placeholder="dictionary.chain" />
            <ui-select id="currency" data-name="currency" data-placeholder="dictionary.currency" />
        </div>
    </layout-main>
`
export default template
