import "/UI/components/identicon/index.js"

export default {
    name: "Identicon",
    stories: [
        {
            name: "Default",
            code: `<ui-identicon data-seed="akao-user-001"></ui-identicon>`
        },
        {
            name: "Row of users",
            code: `<ui-identicon data-seed="user-alpha"></ui-identicon>
<ui-identicon data-seed="user-beta"></ui-identicon>
<ui-identicon data-seed="user-gamma"></ui-identicon>
<ui-identicon data-seed="user-delta"></ui-identicon>
<ui-identicon data-seed="user-epsilon"></ui-identicon>`
        },
        {
            name: "Large grid (size 7)",
            code: `<ui-identicon data-seed="akao-featured-seller" data-size="7"></ui-identicon>`
        }
    ]
}
