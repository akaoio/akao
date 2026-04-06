import { driver } from "./shared.js"

export async function exist(path = []) {
    if (typeof path === "string") path = [path]
    return driver.exists(path)
}
