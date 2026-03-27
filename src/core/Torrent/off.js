export function off(event, callback) {
    const callbacks = this.callbacks.get(event)
    if (!callbacks?.size) return false
    const removed = callbacks.delete(callback)
    if (!callbacks.size) this.callbacks.delete(event)
    return removed
}