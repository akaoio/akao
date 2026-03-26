import { join, ensure, load, exist, write } from "../FS.js"
import { NODE } from "../Utils.js"

export async function init() {
    // Create indexed directory if it doesn't exist
    await ensure(join(["indexed"]))
    // Load initial data from filesystem
    await this.load()
}

export async function $load() {
    const fileExists = await exist(["indexed", this.name + ".json"])
    if (fileExists)
        try {
            const data = await load(["indexed", this.name + ".json"])
            if (data) this.data = data
        } catch (error) {
            console.error("Error loading from disk:", error)
        }
}

export async function $save() {
    if (NODE)
        try {
            await write(["indexed", this.name + ".json"], this.data)
        } catch (error) {
            console.error("Error saving to disk:", error)
        }
}

export { $load as load, $save as save }