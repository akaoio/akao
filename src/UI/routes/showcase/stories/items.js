import "/UI/components/items/index.js"
import "/UI/components/item/index.js"

export default {
    name: "Items",
    stories: [
        {
            name: "4-column grid",
            wide: true,
            code: `<ui-items data-columns="4" data-start="1" data-end="1"></ui-items>`
        },
        {
            name: "3-column grid",
            wide: true,
            code: `<ui-items data-columns="3" data-start="1" data-end="1"></ui-items>`
        }
    ]
}
