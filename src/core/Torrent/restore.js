import { $load } from "./persist.js"

export async function restore() {
    const saved = await $load()
    for (const { magnetURI, infoHash } of saved) this.add(magnetURI).catch((error) => console.error("Failed to restore torrent:", infoHash, error))
    
}
