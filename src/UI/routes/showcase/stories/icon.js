import "/UI/components/icon/index.js"

export default {
    name: "Icon",
    stories: [
        {
            name: "Small",
            code: `<ui-icon data-icon="house" data-size="sm"></ui-icon>`
        },
        {
            name: "Medium (default)",
            code: `<ui-icon data-icon="bag" data-size="md"></ui-icon>`
        },
        {
            name: "Large",
            code: `<ui-icon data-icon="star" data-size="lg"></ui-icon>`
        },
        {
            name: "Common set",
            code: `<ui-icon data-icon="house"></ui-icon>
<ui-icon data-icon="bag"></ui-icon>
<ui-icon data-icon="star"></ui-icon>
<ui-icon data-icon="person"></ui-icon>
<ui-icon data-icon="gear"></ui-icon>
<ui-icon data-icon="search"></ui-icon>
<ui-icon data-icon="heart"></ui-icon>
<ui-icon data-icon="download"></ui-icon>
<ui-icon data-icon="upload"></ui-icon>
<ui-icon data-icon="sliders"></ui-icon>`
        }
    ]
}
