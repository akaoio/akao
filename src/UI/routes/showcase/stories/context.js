import "/UI/components/context/index.js"

export default {
    name: "Context",
    stories: [
        {
            name: "dictionary.home",
            code: `<ui-context data-key="dictionary.home"></ui-context>`
        },
        {
            name: "dictionary.cart",
            code: `<ui-context data-key="dictionary.cart"></ui-context>`
        },
        {
            name: "dictionary.checkout",
            code: `<ui-context data-key="dictionary.checkout"></ui-context>`
        },
        {
            name: "dictionary.profile",
            code: `<ui-context data-key="dictionary.profile"></ui-context>`
        }
    ]
}
