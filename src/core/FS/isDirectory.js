import { driver } from "./shared.js"

export function isDirectory(path) {
    return driver.isDir(path)
}
