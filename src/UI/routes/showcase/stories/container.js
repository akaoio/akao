const CONTAINERS = [
    { name: ".container-form", var: "--container-form", px: "480px" },
    { name: ".container-sm", var: "--container-sm", px: "540px" },
    { name: ".container-md", var: "--container-md", px: "720px" },
    { name: ".container-lg", var: "--container-lg", px: "960px" },
    { name: ".container-xl", var: "--container-xl", px: "1140px" },
    { name: ".container-xxl", var: "--container-xxl", px: "1320px" },
    { name: ".container-fluid", var: null, px: "100%" }
]

function makeRow({ name, var: cssVar, px }) {
    const maxWidth = cssVar ? `var(${cssVar}, ${px})` : "none"
    return `<div class="sc-row">
  <div class="sc-bar" style="max-width: ${maxWidth}">
    <span class="sc-label">${name}</span>
    <span class="sc-meta">${cssVar ? `${cssVar} · ` : ""}${px}</span>
  </div>
</div>`
}

export default {
    name: "Container",

    stories: [
        {
            name: "All Containers",
            wide: true,
            code: CONTAINERS.map(makeRow).join("\n")
        }
    ]
}
