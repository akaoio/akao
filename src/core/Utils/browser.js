import { events } from "../Events.js"

export function notify({ content, callback, className, autoClose, onClose, delay } = {}) {
    events.emit("notify", {
        content,
        callback,
        className,
        autoClose,
        onClose,
        delay
    })
}
