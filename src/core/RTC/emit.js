export function $emit(event, payload) {
    const callbacks = this.callbacks.get(event)
    if (!callbacks?.size) return

    for (const callback of [...callbacks])
        try {
            callback(payload)
        } catch (error) {
            console.error(`RTC ${event} callback error:`, error)
        }
}