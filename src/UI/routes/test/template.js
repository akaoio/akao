import "/UI/layouts/main/index.js"
import "/UI/components/context/index.js"
import { html } from "/core/UI.js"
import "/UI/components/addresses/index.js"

export const template = html`
    <layout-main>
        <h1><ui-context data-key="dictionary.test" /></h1>
        <div>
            <ui-geo data-id="2483743" />
        </div>
    </layout-main>
`
export default template
