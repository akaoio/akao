/**
 * FSDriver — the interface every environment implementation must satisfy.
 * All methods accept array paths (e.g. ["statics", "chains", "eth.json"]).
 *
 * @typedef {Object} FSDriver
 * @property {(path: string[]) => Promise<Uint8Array|null>}                              readBytes
 * @property {(path: string[], bytes: Uint8Array) => Promise<{success:boolean,path:string}>} writeBytes
 * @property {(path: string[]) => Promise<void>}                                         remove
 * @property {(path: string[]) => Promise<string[]>}                                     list
 * @property {(path: string[]) => Promise<Array<{name:string,isDir:boolean}>>}           entries
 * @property {(path: string[]) => Promise<boolean>}                                      exists
 * @property {(path: string[]) => Promise<boolean>}                                      isDir
 * @property {(path: string[]) => Promise<void>}                                         mkdir
 * @property {(src: string[], dst: string[]) => Promise<void>}                          move
 * @property {(src: string[], dst: string[]) => Promise<void>}                          copyFile
 */

const REQUIRED = ["readBytes", "writeBytes", "remove", "list", "entries", "exists", "isDir", "mkdir", "move", "copyFile"]

/**
 * Validates that an implementation satisfies the FSDriver contract.
 * Throws at module load time if any method is missing — fails fast, never silently.
 *
 * @param {FSDriver} impl
 * @returns {FSDriver}
 */
export function createDriver(impl) {
    for (const method of REQUIRED)
        if (typeof impl[method] !== "function")
            throw new Error(`[FSDriver] missing required method: "${method}"`)
    return impl
}
