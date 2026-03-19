import "/UI/components/a/index.js"

export default {
    name: "Link",
    stories: [
        {
            name: "Home",
            code: `<a is="ui-a" data-to="/">Go Home</a>`
        },
        {
            name: "To item",
            code: `<a is="ui-a" data-to="/item/organic-green-tea">View Product →</a>`
        },
        {
            name: "To checkout",
            code: `<a is="ui-a" data-to="/checkout">Proceed to Checkout →</a>`
        }
    ]
}
