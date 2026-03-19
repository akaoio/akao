import "/UI/components/select/index.js"

export default {
    name: "Select",
    stories: [
        {
            name: "Sort options",
            code: `<ui-select data-name="sort" data-placeholder="home"></ui-select>`,
            setup(container) {
                const select = container.querySelector("ui-select")
                select.states.set({
                    options: [
                        { value: "price-asc",  label: "Price: Low to High" },
                        { value: "price-desc", label: "Price: High to Low" },
                        { value: "newest",     label: "Newest First" },
                        { value: "popular",    label: "Most Popular" }
                    ]
                })
            }
        },
        {
            name: "Pre-selected",
            code: `<ui-select data-name="category" data-selected="clothing" data-placeholder="home"></ui-select>`,
            setup(container) {
                const select = container.querySelector("ui-select")
                select.states.set({
                    options: [
                        { value: "all",         label: "All Categories" },
                        { value: "electronics", label: "Electronics" },
                        { value: "clothing",    label: "Clothing" },
                        { value: "books",       label: "Books" }
                    ]
                })
            }
        }
    ]
}
