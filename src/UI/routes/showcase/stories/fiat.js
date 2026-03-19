import "/UI/components/fiat/index.js"

export default {
    name: "Fiat",
    stories: [
        {
            name: "Small price",
            code: `<ui-fiat data-amount="9.99" data-base="USD"></ui-fiat>`
        },
        {
            name: "Regular price",
            code: `<ui-fiat data-amount="49.99" data-base="USD"></ui-fiat>`
        },
        {
            name: "Large price",
            code: `<ui-fiat data-amount="1299.99" data-base="USD"></ui-fiat>`
        },
        {
            name: "Free",
            code: `<ui-fiat data-amount="0" data-base="USD"></ui-fiat>`
        }
    ]
}
