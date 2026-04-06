import { driver } from "./shared.js"

export async function copy(src, dest) {
    if (!Array.isArray(src)) src = src.split("/").filter(Boolean)
    if (!Array.isArray(dest)) dest = dest.split("/").filter(Boolean)
    try {
        if (await driver.isDir(src)) {
            await driver.mkdir(dest)
            for (const { name } of await driver.entries(src))
                await copy([...src, name], [...dest, name])
        } else {
            await driver.copyFile(src, dest)
        }
        return { success: true, path: dest.join("/") }
    } catch (error) {
        console.error("Error copying:", error)
    }
}
