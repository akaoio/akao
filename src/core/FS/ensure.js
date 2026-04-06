import { driver } from "./shared.js"

export async function ensure(path) {
    try {
        await driver.mkdir(Array.isArray(path) ? path : path.split("/").filter(Boolean))
        return true
    } catch (error) {
        console.error("Error creating directory:", path, error)
        return false
    }
}
