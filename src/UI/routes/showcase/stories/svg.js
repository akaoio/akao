import "/UI/components/svg/index.js"

export default {
    name: "SVG",
    stories: [
        {
            name: "Brand text",
            code: `<ui-svg data-src="/statics/sites/localhost/images/akao-text.svg" style="width:8rem;color:var(--color)"></ui-svg>`
        },
        {
            name: "Brand symbol",
            code: `<ui-svg data-src="/statics/sites/localhost/images/akao-symbol.svg" style="width:3rem;color:var(--color)"></ui-svg>`
        },
        {
            name: "Neon accent",
            code: `<ui-svg data-src="/statics/sites/localhost/images/akao-symbol.svg" style="width:3rem;color:var(--color-accent)"></ui-svg>`
        },
        {
            name: "Bootstrap icon inline",
            code: `<ui-svg data-src="/images/icons/star-fill.svg" style="width:2rem;color:var(--color-accent)"></ui-svg>`
        }
    ]
}
