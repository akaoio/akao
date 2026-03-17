import "/UI/components/themes/index.js"
import "/UI/components/locales/index.js"
import "/UI/components/fiats/index.js"

export default {
    name: "Selectors",
    stories: [
        {
            name: "Theme toggle",
            code: `<ui-themes></ui-themes>`
        },
        {
            name: "Locale picker",
            code: `<ui-locales></ui-locales>`
        },
        {
            name: "Fiat picker",
            code: `<ui-fiats></ui-fiats>`
        }
    ]
}
