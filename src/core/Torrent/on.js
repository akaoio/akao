export function on(event, callback) {
    if (!event || typeof callback !== "function") return () => {}
    if (!this.callbacks.has(event)) this.callbacks.set(event, new Set())
    this.callbacks.get(event).add(callback)
    return () => this.off(event, callback)
}