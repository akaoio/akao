import "/UI/components/svg/index.js"

export default {
    name: "SVG",
    stories: [
        {
            name: "Brand text",
            code: `<ui-svg data-src="/statics/sites/localhost/images/text.svg" class="svg-lg svg-color"></ui-svg>`
        },
        {
            name: "Brand symbol",
            code: `<ui-svg data-src="/statics/sites/localhost/images/symbol.svg" class="svg-md svg-color"></ui-svg>`
        },
        {
            name: "Neon accent",
            code: `<ui-svg data-src="/statics/sites/localhost/images/symbol.svg" class="svg-md svg-color-accent"></ui-svg>`
        },
        {
            name: "Bootstrap icon inline",
            code: `<ui-svg data-src="/images/icons/star-fill.svg" class="svg-sm svg-color-accent"></ui-svg>`
        }
    ]
}
