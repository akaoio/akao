import { write } from "../../src/core/FS.js"
import { paths } from "./config.js"

// ============ Routes Generation ============

export async function generateRoutes(locales, items, tags, indexContent, outputBase = "build") {
    const dynamicPaths = outputBase === "build" ? paths : {
        ...paths,
        build: {
            ...paths.build,
            root: [outputBase]
        }
    }
    
    const routes = [{ path: [...dynamicPaths.build.root, "index.html"], label: "Root" }]

    for (const locale of locales) {
        routes.push({ path: [...dynamicPaths.build.root, locale, "index.html"], label: `/${locale}` }, { path: [...dynamicPaths.build.root, locale, "item", "index.html"], label: `/${locale}/item` })

        for (const item of items)
            routes.push({
                path: [...dynamicPaths.build.root, locale, "item", item, "index.html"],
                label: `/${locale}/item/${item}`
            })

        routes.push({
            path: [...dynamicPaths.build.root, locale, "tag", "index.html"],
            label: `/${locale}/tag`
        })

        for (const tag of tags)
            routes.push({
                path: [...dynamicPaths.build.root, locale, "tag", tag, "index.html"],
                label: `/${locale}/tag/${tag}`
            })

        // Add test route
        routes.push({
            path: [...dynamicPaths.build.root, locale, "test", "index.html"],
            label: `/${locale}/test`
        })
    }

    for (const route of routes) await write(route.path, indexContent)

    return routes.length
}
