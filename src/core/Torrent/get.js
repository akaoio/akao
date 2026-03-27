export function get(id) {
    if (!id) return undefined
    if (this.torrents.has(id)) return this.torrents.get(id)
    return this.client?.get?.(id)
}