import "/UI/components/picker/index.js"
import "/UI/components/button/index.js"

export default {
    name: "Picker",
    stories: [
        {
            name: "Category picker",
            code: `<ui-button data-right="arrow-right-short">Browse Categories</ui-button>
<ui-picker data-name="category" data-header="home"></ui-picker>`,
            setup(container) {
                const btn = container.querySelector("ui-button")
                const picker = container.querySelector("ui-picker")
                picker.states.set({
                    options: [
                        { value: "all",         label: "All Categories" },
                        { value: "electronics", label: "Electronics" },
                        { value: "clothing",    label: "Clothing" },
                        { value: "books",       label: "Books" },
                        { value: "home",        label: "Home & Garden" },
                        { value: "sports",      label: "Sports & Outdoors" }
                    ]
                })
                btn.addEventListener("click", () => picker.show())
            }
        },
        {
            name: "Sort picker",
            code: `<ui-button data-left="sliders">Sort by</ui-button>
<ui-picker data-name="sort" data-header="home"></ui-picker>`,
            setup(container) {
                const btn = container.querySelector("ui-button")
                const picker = container.querySelector("ui-picker")
                picker.states.set({
                    options: [
                        { value: "relevance",  label: "Relevance" },
                        { value: "price-asc",  label: "Price: Low to High" },
                        { value: "price-desc", label: "Price: High to Low" },
                        { value: "newest",     label: "Newest First" }
                    ]
                })
                btn.addEventListener("click", () => picker.show())
            }
        }
    ]
}
