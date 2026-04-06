import { driver } from "./shared.js"

export async function move(src, dest) {
    try {
        await driver.move(src, dest)
        return true
    } catch (error) {
        console.error("Error moving:", error)
        return false
    }
}
