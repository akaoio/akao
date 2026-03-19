import "/UI/components/button/index.js"

export default {
    name: "Button",
    stories: [
        {
            name: "Default",
            code: `<ui-button>Click me</ui-button>`
        },
        {
            name: "Right icon",
            code: `<ui-button data-right="arrow-right-short">Shop Now</ui-button>`
        },
        {
            name: "Left icon",
            code: `<ui-button data-left="star">Favourite</ui-button>`
        },
        {
            name: "Both icons",
            code: `<ui-button data-left="bag" data-right="arrow-right-short">Add to Cart</ui-button>`
        }
    ]
}
