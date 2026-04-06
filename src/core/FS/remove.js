import { driver } from "./shared.js"

export async function remove(path) {
    try {
        await driver.remove(path)
        return true
    } catch (error) {
        console.error("Error removing path:", path, error)
        return false
    }
}
